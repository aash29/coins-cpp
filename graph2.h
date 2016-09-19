//
// Created by aash29 on 9/17/16.
//

#ifndef COINS_GRAPH2_H
#define COINS_GRAPH2_H
#include <vector>
#include <Box2D/Box2D.h>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>




    class CGraph {


    public:

        CGraph(std::vector<std::vector<int>> iGraph, std::vector<b2Vec2> iVertices);

        std::vector<std::vector<int>> findCycles2();
        void printNeighbors();


        std::vector<std::vector<int>> graph;
		std::vector<b2Vec2> vertices;
        std::unordered_map<int, std::vector<int>> neighbors = std::unordered_map<int, std::vector<int>>();
        std::vector<int> protectedCoins = std::vector<int>();

	private:
		int findLeftVertex(std::unordered_set<int> nodes);
		int steepestNeighbor(int leftmost);
		int getNextCWNeighbor(int i, int p);
		float angle(b2Vec2 a, b2Vec2 b, b2Vec2 center);
		std::vector<int> cycle_red(std::vector<int> cycle, int n);
        std::vector<b2Vec2> cycleToPolygon2(std::vector<int> cycle);

};

void testCgraph();


#endif //COINS_GRAPH2_H
