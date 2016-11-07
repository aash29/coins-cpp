#ifndef COINS_GEOMETRY2D_H
#define COINS_GEOMETRY2D_H

#include "Box2D/Box2D.h"

class Segment {

    struct Rect {
        float xMin;
        float xMax;
        float yMin;
        float yMax;
    };
public:

    constexpr static float defaultAccuracy = 1e-6f;

    enum ContainmentMethod {
        Default, Precise
    };
    const static ContainmentMethod defaultContainmentMethod = Default;


    b2Vec2 a;
    b2Vec2 b;


    static Segment SegmentWithPoints(b2Vec2 a_, b2Vec2 b_) {
        Segment instance = Segment();
        instance.a = a_;
        instance.b = b_;
        return instance;
    }


    /*
     * 
     * Accessors
     * 
     */

    Rect getBounds() {
        Rect bounds = Rect();
        bounds.xMin = std::min(a.x, b.x);
        bounds.xMax = std::max(a.x, b.x);
        bounds.yMin = std::min(a.y, b.y);
        bounds.yMax = std::max(a.y, b.y);
        return bounds;
    }


    Rect ExpandedBounds(float accuracy) {
        float treshold = accuracy / 2.0f;
        Rect bounds_ = getBounds();
        bounds_.xMin = bounds_.xMin - treshold;
        bounds_.xMax = bounds_.xMax + treshold;
        bounds_.yMin = bounds_.yMin - treshold;
        bounds_.yMax = bounds_.yMax + treshold;

        return bounds_;
    }

    bool RectContains(Rect r, b2Vec2 point) {
        return ((point.x < r.xMax) && (r.xMin < point.x) && (point.y < r.yMax) && (r.yMin < point.y));
    }


    bool RectOverlap(Rect RectA, Rect RectB) {

        return (RectA.xMin < RectB.xMax && RectA.xMax > RectB.xMin &&
                RectA.yMin < RectB.yMax && RectA.yMax > RectB.yMin);
    }

    bool ContainsPoint(b2Vec2 point) { return ContainsPoint(point, defaultAccuracy); }

    bool ContainsPoint(b2Vec2 point, float accuracy) { return ContainsPoint(point, accuracy, Default); }


    bool ArePointsCCW(b2Vec2 a, b2Vec2 b, b2Vec2 c) {
        return ((c.y - a.y) * (b.x - a.x) > (b.y - a.y) * (c.x - a.x));
    }

    bool ArePointsEqualWithAccuracy(b2Vec2 a, b2Vec2 b, float accuracy)
    {
        return (a - b).Length() <= accuracy;
    }

    bool HaveSegmentsCommonPointsWithAccuracy(b2Vec2 segment1_a, b2Vec2 segment1_b, b2Vec2 segment2_a,
                                              b2Vec2 segment2_b, float accuracy) {
        return (
                ArePointsEqualWithAccuracy(segment1_a, segment2_a, accuracy) ||
                ArePointsEqualWithAccuracy(segment1_a, segment2_b, accuracy) ||
                ArePointsEqualWithAccuracy(segment1_b, segment2_a, accuracy) ||
                ArePointsEqualWithAccuracy(segment1_b, segment2_b, accuracy)
        );
    }

    bool AreSegmentsIntersecting(b2Vec2 segment1_a, b2Vec2 segment1_b, b2Vec2 segment2_a, b2Vec2 segment2_b) {
        return (
                (ArePointsCCW(segment1_a, segment2_a, segment2_b) !=
                 ArePointsCCW(segment1_b, segment2_a, segment2_b)) &&
                (ArePointsCCW(segment1_a, segment1_b, segment2_a) != ArePointsCCW(segment1_a, segment1_b, segment2_b))
        );
    }

    b2Vec2 IntersectionPointOfLines(b2Vec2 segment1_a, b2Vec2 segment1_b, b2Vec2 segment2_a, b2Vec2 segment2_b)
    {
        float crossProduct = (segment1_a.x - segment1_b.x) * (segment2_a.y - segment2_b.y) - (segment1_a.y - segment1_b.y) * (segment2_a.x - segment2_b.x);
        if (crossProduct == 0.0f) return b2Vec2(0.f,0.f);

        float x = ((segment2_a.x - segment2_b.x) * (segment1_a.x * segment1_b.y - segment1_a.y * segment1_b.x) - (segment1_a.x - segment1_b.x) * (segment2_a.x * segment2_b.y - segment2_a.y * segment2_b.x)) / crossProduct;
        float y = ((segment2_a.y - segment2_b.y) * (segment1_a.x * segment1_b.y - segment1_a.y * segment1_b.x) - (segment1_a.y - segment1_b.y) * (segment2_a.x * segment2_b.y - segment2_a.y * segment2_b.x)) / crossProduct;

        // Skip segmental tests.
        // if (x < Mathf.Min(a1.x, b1.x) || x > Mathf.Max(a1.x, b1.x)) return Vector2.zero;
        // if (x < Mathf.Min(a2.x, b2.x) || x > Mathf.Max(a2.x, b2.x)) return Vector2.zero;

        return b2Vec2(x, y);
    }

    float PointDistanceFromLine(b2Vec2 point, b2Vec2 segment_a, b2Vec2 segment_b) {
        float a_ = point.x - segment_a.x;
        float b_ = point.y - segment_a.y;
        float c_ = segment_b.x - segment_a.x;
        float d_ = segment_b.y - segment_a.y;
        return abs(a_ * d_ - c_ * b_) / sqrt(c_ * c_ + d_ * d_);
    };

    float DistanceToPoint(b2Vec2 point_) {
        return PointDistanceFromLine(point_, a, b);
    };

    b2Vec2 closestPoint(b2Vec2 point_) {
        b2Vec2 dir = b-a;
        dir.Normalize();
        float pr1 = b2Dot(dir,point_ - a);
        return a+pr1*dir;
    };

    bool intersectsCircle(b2Vec2 cp, float r)
    {
        if (((a-cp).LengthSquared()<=r*r)|| ((b-cp).LengthSquared()<=r*r) ) {
            return true;
        }

		if (DistanceToPoint(cp) <= r) {
			b2Vec2 closest = closestPoint(cp);
			float dp = (closest.x - a.x) * (b.x - a.x) + (closest.y - a.y)*(b.y - a.y);
			float squaredlengthba = (b.x - a.x)*(b.x - a.x) + (b.y - a.y)*(b.y - a.y);
			if ((dp > 0) && (dp < squaredlengthba)) {
				return true;
			};
			//if (ContainsPoint(closestPoint(cp))){
			//    return true;
		//}
        }
        
        return false;
    }

    bool ContainsPoint(b2Vec2 point, float accuracy, ContainmentMethod containmentMethod) {
        float treshold = accuracy / 2.0f;

        // Expanded bounds containment test.
        Rect expandedBounds = ExpandedBounds(accuracy);
        bool expandedBoundsContainment = RectContains(expandedBounds, point);
        if (expandedBoundsContainment == false) return false; // Only if passed

        // Line distance test.
        float distance = DistanceToPoint(point);
        bool lineDistanceTest = distance < treshold;
        if (lineDistanceTest == false) return false; // Only if passed

        if (containmentMethod == Precise) {
            // Perpendicular segment.
            b2Vec2 normalizedHalf = 1.0f / 2.0f * (b - a);
            float halfLength = normalizedHalf.Length();
            b2Vec2 normalizedHalfPerpendicular = b2Vec2(-normalizedHalf.y, normalizedHalf.x);
            b2Vec2 perpendicular_a = a + normalizedHalf;
            b2Vec2 perpendicular_b = a + normalizedHalf + normalizedHalfPerpendicular;

            // Perpendicular line distance test.
            float perpendicularDistance = PointDistanceFromLine(point, perpendicular_a, perpendicular_b);
            bool perpendicularDistanceTest = perpendicularDistance < halfLength;

            // Endpoint distance test if previous failed.
            if (perpendicularDistanceTest == false) {
                float distanceFromEndPoints = std::min((a - point).Length(), (b - point).Length());
                bool endpointDistanceTest = distanceFromEndPoints < treshold;
                if (endpointDistanceTest == false) return false; // Only if passed
            }
        }

        // All passed.
        return true;
    }


    /**
     * True when the two segments are intersecting. Not true when endpoints
     * are equal, nor when a point is contained by other segment.
     * Can say this algorithm has infinite precision.
     */
    bool IsIntersectingWithSegment(Segment segment) {
        // No intersecting if bounds don't even overlap (slight optimization).
        bool boundsOverlaps = RectOverlap(getBounds(), segment.getBounds());
        if (boundsOverlaps == false) return false;

        // Do the Bryce Boe test.
        return AreSegmentsIntersecting(a, b, segment.a, segment.b);
    }

    /**
     * Returns intersection when the two segments are intersecting. Not returns anything when endpoints
     * are equal, nor when a point is contained by other segment.
     * Can say this algorithm has infinite precision.
     */
    bool IntersectionWithSegment(Segment segment, b2Vec2 &intersectionPoint) {
        return IntersectionWithSegmentWithAccuracy(segment, 0.0f, intersectionPoint);
    }

    bool IntersectionWithSegmentWithAccuracy(Segment segment, float accuracy, b2Vec2 &intersectionPoint) {
        return IntersectionWithSegmentWithAccuracy(segment, accuracy, defaultContainmentMethod, intersectionPoint);
    }

    bool IntersectionWithSegmentWithAccuracy(Segment segment, ContainmentMethod containmentMethod,
                                             b2Vec2 &intersectionPoint) {
        return IntersectionWithSegmentWithAccuracy(segment, defaultAccuracy, containmentMethod, intersectionPoint);
    }

    bool IntersectionWithSegmentWithAccuracy(Segment segment, float accuracy, ContainmentMethod containmentMethod,
                                             b2Vec2 &intersectionPoint) {
        intersectionPoint = b2Vec2(0.f, 0.f); // Default

        // No intersecting if bounds don't even overlap.
        Rect expandedBounds = ExpandedBounds(accuracy);
        Rect otherExpandedBounds = segment.ExpandedBounds(accuracy);
        bool boundsOverlaps = RectOverlap(expandedBounds, otherExpandedBounds);
        if (boundsOverlaps == false) {
            return false; // No intersection
        }

        if (accuracy > 0.0f) // Only any accuracy is given
        {
            // Look up point containments.
            bool containsA = ContainsPoint(segment.a, accuracy, containmentMethod);
            if (containsA) {
                intersectionPoint = segment.a;
                return false; // Intersecting
            }

            bool containsB = ContainsPoint(segment.b, accuracy, containmentMethod);
            if (containsB) {
                intersectionPoint = segment.b;
                return false; // Intersecting
            }

            bool otherContainsA = segment.ContainsPoint(a, accuracy, containmentMethod);
            if (otherContainsA) {
                intersectionPoint = a;
                return false; // Intersecting
            }

            bool otherContainsB = segment.ContainsPoint(b, accuracy, containmentMethod);
            if (otherContainsB) {
                intersectionPoint = b;
                return false; // Intersecting
            }

            if (HaveSegmentsCommonPointsWithAccuracy(a, b, segment.a, segment.b, accuracy)) {
                return false;
            }
        }

        // Do the Bryce Boe test.
        bool isIntersecting = AreSegmentsIntersecting(a, b, segment.a, segment.b);
        if (isIntersecting == false) {
            return false; // No intersection
        }

        // All fine, intersection point can be determined.
        intersectionPoint = IntersectionPointOfLines(a, b, segment.a,
                                                     segment.b); // Actually the intersection of lines defined by segments
        return true; // Intersecting
    }


};


#endif //COINS_GEOMETRY2D_H
