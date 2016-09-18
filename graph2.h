//
// Created by aash29 on 9/17/16.
//

#ifndef COINS_GRAPH2_H
#define COINS_GRAPH2_H
#include <vector>
#include <Box2D/Box2D.h>
#include <unordered_map>


    class CGraph {


    public:
        CGraph(std::vector<std::vector<int>> iGraph);

        void printNeighbors();

        std::vector<std::vector<int>> graph;

        std::vector<b2Vec2> vertices;

        std::unordered_map<int, std::vector<int>> *neighbors = new std::unordered_map<int, std::vector<int>>();


        std::vector<int> *protectedCoins = new std::vector<int>();


};

void testCgraph();

#endif //COINS_GRAPH2_H
