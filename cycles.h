#include <vector>
#include <map>
#include <Box2D/Box2D.h>
#include "coin.h"
#include "geometry2d.h"
#include "graph2.h"


class cycles {
public:

    float proximityRadius = 10.0f;

    bool calculatedCyclesThisPush = true;

    std::vector<std::vector<int>> *cyclesList = new std::vector<std::vector<int> >[2];

    std::vector<std::vector<b2Vec2>> *coinCycles = new std::vector<std::vector<b2Vec2> >[2];

    std::vector<std::vector<b2Vec2>> *edges = new std::vector<std::vector<b2Vec2> >[2];

    std::vector<b2Vec2> *allVertices = new std::vector<b2Vec2>[2];


    bool IsPointInPolygon(b2Vec2 p, std::vector<b2Vec2> polygon);


    bool IsPolygonInPolygon(std::vector<b2Vec2> p1, std::vector<b2Vec2> p2);

    std::vector<std::vector<b2Vec2> > FindCycles(int player, std::vector<std::vector<b2Vec2>> polygons,
                                                 std::vector<std::vector<int> > cyclesOut,
                                                 std::map<int, coin> &allCoins);


    void removeDuplicates(std::vector<std::vector<int>> &g2, std::vector<b2Vec2> v2) ;

    int addOrGetExisting(b2Vec2 vi, std::vector<b2Vec2> &v2) ;

};