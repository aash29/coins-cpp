#include "cycles.h"


cycles::cycles(std::map<int, coin> *allCoins, float radius) {
    m_coins = allCoins;
    proximityRadius = radius;
};

bool cycles::IsPointInPolygon(b2Vec2 p, std::vector<b2Vec2> polygon) {
    float minX = polygon[0].x;
    float maxX = polygon[0].x;
    float minY = polygon[0].y;
    float maxY = polygon[0].y;
    for (int i = 1; i < polygon.size(); i++) {
        b2Vec2 q = polygon[i];
        minX = std::min(q.x, minX);
        maxX = std::max(q.x, maxX);
        minY = std::min(q.y, minY);
        maxY = std::max(q.y, maxY);
    }

    if (p.x < minX || p.x > maxX || p.y < minY || p.y > maxY) {
        return false;
    }

    // http://www.ecse.rpi.edu/Homepages/wrf/Research/Short_Notes/pnpoly.html
    bool inside = false;
    for (int i = 0, j = polygon.size() - 1; i < polygon.size(); j = i++) {
        if ((polygon[i].y > p.y) != (polygon[j].y > p.y) &&
            p.x <
            (polygon[j].x - polygon[i].x) * (p.y - polygon[i].y) / (polygon[j].y - polygon[i].y) + polygon[i].x) {
            inside = !inside;
        }
    }
    return inside;
};


bool cycles::IsPolygonInPolygon(std::vector<b2Vec2> p1, std::vector<b2Vec2> p2) {


    for (int i = 0; i < p1.size(); i++)
        if (!(IsPointInPolygon(p1[i], p2) || (IsCircleIntersectingPolygon(p1[i], p2))))
            return false;

    return true;
};

bool cycles::IsCircleIntersectingPolygon(b2Vec2 p, std::vector<b2Vec2> polygon){
    for (int i=0;i<polygon.size()-1;i++) {
        Segment s1 = Segment::SegmentWithPoints(polygon[i], polygon[i+1]);
        if (s1.intersectsCircle(p,1.5f)) {
            return true;
        }
    }
	/*
    Segment s1 = Segment::SegmentWithPoints(polygon[polygon.size()-1], polygon[0]);
    if (s1.intersectsCircle(p,1.5f)) {
        return true;
    }
	*/
    return false;

};




void cycles::deleteTrappedCoins(int player, std::vector<std::vector<b2Vec2>> cCycles) {
    //var coins = GameObject.FindGameObjectsWithTag("coin");

    std::vector<int> protectedCycles = std::vector<int>();


    for (int j = 0; j < coinCycles[player % 2].size(); j++) {
        for (int k = 0; k < coinCycles[(player + 1) % 2].size(); k++) {
            if ((IsPolygonInPolygon(coinCycles[player % 2][j], coinCycles[(player + 1) % 2][k])) &&
                (std::find(protectedCycles.begin(), protectedCycles.end(), j) == protectedCycles.end())) {
                for (auto i : cyclesList[player % 2][j]) {
                    (*m_coins)[i].dead = true;
                }
            }

        }

    }

    for (auto c0: *m_coins) {

        int cp1 = c0.second.player;
        if (coinCycles[(player) % 2].size() > 0) {
            for (auto cc : coinCycles[(player) % 2]) {
                if ((IsPointInPolygon(c0.second.wheel->GetPosition(), cc)) ) {
                    if ((cp1 == player)) {
                        //c0.second.connected=true;
                        m_coins->at(c0.first).connected = true;
                    }

                }
            }
        }

    }


    for (auto c0 : *m_coins) {
        int cp1 = c0.second.player;
        if (cCycles.size() > 0) {
            for (auto cc : cCycles) {
                if (IsPointInPolygon(c0.second.wheel->GetPosition(), cc) || (IsCircleIntersectingPolygon(c0.second.wheel->GetPosition(), cc))) {
                    if ((cp1 == player) && (!c0.second.connected)) {
                        //c0.second.dead=true;
                        m_coins->at(c0.first).dead = true;
                    }

                }
            }
        }

    }
}


/*
	public void Update()
	{
		//sw.Close ();
		//sw = new StreamWriter(logName);


		GameObject.Find("root1").GetComponent<setupLevel>().initCoins();
        bool atRest=true;

		FindCycles (0, ref coinCycles [0], ref cyclesList[0]);
		FindCycles (1, ref coinCycles [1], ref cyclesList[1]);



		 (0, coinCycles [1]);
		deleteTrappedCoins (1, coinCycles [0]);

		calculatedCyclesThisPush = true;
		//		}
		cleanup ();

		//sw.Flush ();

	}

*/
std::vector<std::vector<b2Vec2> > cycles::FindCycles(int player, std::vector<std::vector<b2Vec2>> &polygons,
                                                     std::vector<std::vector<int> > &cyclesOut) {
    //GameObject[] allCoins;

    std::vector<int> coins = std::vector<int>();
    std::vector<int> subst = std::vector<int>();

    //allCoins = GameObject.FindGameObjectsWithTag ("coin");

    //setupLevel sl1 = GameObject.Find("root1").GetComponent<setupLevel>();
    //allCoins = sl1.coinDict;

    std::vector<std::vector<int>> g2 = std::vector<std::vector<int>>();

    std::vector<b2Vec2> v2 = std::vector<b2Vec2>();


    for (auto &kv: *m_coins) {
        int cp1 = kv.second.player;
        if (cp1 == player) {
            v2.push_back(kv.second.wheel->GetPosition());
            coins.push_back(kv.first);
            subst.push_back(kv.first);
        }
    }


    std::vector<std::vector<b2Vec2>> edge = std::vector<std::vector<b2Vec2>>();


    int edgeNum = 0;

    for (int i = 0; i < coins.size(); i++) {
        (*m_coins)[coins[i]].connected = false;
        for (int j = i + 1; j < coins.size(); j++) {
            coin c1 = (*m_coins)[coins[i]];
            coin c2 = (*m_coins)[coins[j]];
            b2Vec2 v1 = c1.wheel->GetPosition() - c2.wheel->GetPosition();

            if (v1.Length() < proximityRadius) {
                g2.push_back(std::vector<int> {i, j});
                edge.push_back(std::vector<b2Vec2> {c1.wheel->GetPosition(), c2.wheel->GetPosition()});
                edgeNum++;
            }

        }

    }
    std::vector<int> ed0;
    for (int i = 0; i < v2.size(); i++) {
        for (int j = 0; j < g2.size(); j++) {
            //edH0.First();

            ed0 = g2[j];
            Segment s1 = Segment();
            s1 = Segment::SegmentWithPoints(b2Vec2(v2[ed0[0]].x, v2[ed0[0]].y), b2Vec2(v2[ed0[1]].x, v2[ed0[1]].y));

            if ((i != ed0[0]) && (i != ed0[1]) && (s1.ContainsPoint(b2Vec2(v2[i].x, v2[i].y), 1e-4f))) {
                //std::vector<int> ed01 = new std::vector<int> {ed0[0],i};
                std::vector<int> ed01 = std::vector<int> {ed0[0], i};
                g2.push_back(ed01);
                std::vector<int> ed02 = std::vector<int> {i, ed0[1]};
                g2.push_back(ed02);
                g2.erase(std::remove(g2.begin(), g2.end(), ed0), g2.end());
            }
        }

    }

    removeDuplicates(g2, v2);


    std::map<int, std::vector<int>> subdivs = std::map<int, std::vector<int>>();
    std::vector<int> edge1, edge2;

    for (int i = 0; i < g2.size(); i++) {
        subdivs.insert(std::make_pair(i, std::vector<int>{g2[i][0], g2[i][1]}));
    }

    for (int i = 0; i < g2.size(); i++)
        for (int j = i + 1; j < g2.size(); j++) {
            edge1 = g2[i];
            edge2 = g2[j];

            Segment s1 = Segment();
            s1 = Segment::SegmentWithPoints(v2[edge1[0]], v2[edge1[1]]);

            Segment s2 = Segment();
            s2 = Segment::SegmentWithPoints(v2[edge2[0]], v2[edge2[1]]);

            b2Vec2 vi;

            if (s1.IntersectionWithSegmentWithAccuracy(s2, 1e-6f, vi)) {
                int cn = addOrGetExisting(vi, v2);

                subdivs[i].push_back(cn);
                subdivs[j].push_back(cn);
            }


        }
    for (auto &kv : subdivs) {
        //int div = subdivs.Keys.ElementAt(i);
        //kv.second.Distinct().Tostd::vector();

        std::sort(kv.second.begin(), kv.second.end(), [v2, g2, kv](const int a, const int b) {
            return (v2[g2[kv.first][0]] - v2[a]).Length() > (v2[g2[kv.first][0]] - v2[b]).Length();
        });
        kv.second.erase(std::unique(kv.second.begin(), kv.second.end()), kv.second.end());
        //subdivs[div] = subdivs[div].OrderBy(};
    }


    std::vector<std::vector<int>> g3 = std::vector<std::vector<int>>();
    for (auto &kv : subdivs) {
        for (int i = 0; i < kv.second.size() - 1; i++) {
            g3.push_back(std::vector<int>{kv.second[i], kv.second[i + 1]});
        }
    }

    g2 = g3;


    removeDuplicates(g2, v2);

    //v2 = v2.Distinct ().Tostd::vector ();

    edges[player] = edge;
    allVertices[player] = v2;


    CGraph cg1 = CGraph(g2, v2);

    std::vector<std::vector<int>> cycles = cg1.findCycles2();
    //cyclesstd::vector = cycles;



    int cp = 0;

    //polygons = null;


    std::vector<std::vector<int>> onlyCoinCycles = std::vector<std::vector<int>>();
    int ci = 0;
    for (auto &cy : cycles) {
        onlyCoinCycles.push_back(std::vector<int>());

        for (int i = 0; i < cy.size(); i++) {
            if (cy[i] < coins.size())
                onlyCoinCycles[ci].push_back(subst[cy[i]]);
        }
        ci++;
    }

    if (cycles.size() != 0) {


        polygons = std::vector<std::vector<b2Vec2>>();

        for (auto &cy : cycles) {
            //string s = "" + cy[0];
            polygons.push_back(std::vector<b2Vec2>());
            polygons[cp].push_back(v2[cy[0]]);
            if (cy[0] < coins.size()) {
                (*m_coins)[coins[cy[0]]].connected = true;
            }
            for (int i = 1; i < cy.size(); i++) {
                //s += "," + cy[i];

                b2Vec2 pos = v2[cy[i]];

                polygons[cp].push_back(pos);
                if (cy[i] < coins.size()) {
                    (*m_coins)[coins[cy[i]]].connected = true;
                    //coins[cy[i]].GetComponent<Multitag>().TagsSet.Add("connected");
                    //coins[cy[i]].GetComponent<Multitag>().TagsSet.Add(cp.ToString());
                }
                //linerenderer.SetPosition (i, pos);

            }

            //polygons[cp].Add(coins[cy[0]].transform.position);

            cp++;
        }


    }

    cyclesOut = onlyCoinCycles;

    return polygons;

}


void cycles::removeDuplicates(std::vector<std::vector<int>> &g2, std::vector<b2Vec2> v2) {
    std::vector<int> edge1, edge2;

    for (int i = 0; i < g2.size(); i++)
        for (int j = i + 1; j < g2.size(); j++) {
            if (i != j) {
                edge1 = g2[i];
                edge2 = g2[j];

                if ((((v2[edge1[0]] - v2[edge2[0]]).Length() < 1e-4f) &&
                     ((v2[edge1[1]] - v2[edge2[1]]).Length() < 1e-4f)) ||
                    (((v2[edge1[0]] - v2[edge2[1]]).Length() < 1e-4f) &&
                     ((v2[edge1[1]] - v2[edge2[0]]).Length() < 1e-4f))) {
                    g2.erase(g2.begin() + j);

                    //sw.WriteLine("Edges (" + edge1[0] +","  + edge1[1] + ") and (" + edge2[0] +","  + edge2[1] + ") have same coords, (" + edge2[0] +","  + edge2[1] + ") removed. ##" + i+ "," + j);

                }
            }
        }

}

int cycles::addOrGetExisting(b2Vec2 vi, std::vector<b2Vec2> &v2) {
    int cn;
    int nn = -1;
    int i = 0;
    for (auto &cv : v2) {
        float mn = (cv - vi).Length();
        if (mn < 0.01f) {
            nn = i;
            break;
        }
        i++;
    }

    if (nn >= 0) {
        cn = nn;
    } else {
        cn = v2.size();
        v2.push_back(b2Vec2(vi.x, vi.y));
    }
    return cn;
}


