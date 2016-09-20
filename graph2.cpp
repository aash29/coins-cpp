#include "graph2.h"
#include <iostream>
#include <sstream>


int pnpoly(int nvert, float *vertx, float *verty, float testx, float testy) {
    int i, j, c = 0;
    for (i = 0, j = nvert - 1; i < nvert; j = i++) {
        if (((verty[i] > testy) != (verty[j] > testy)) &&
            (testx < (vertx[j] - vertx[i]) * (testy - verty[i]) / (verty[j] - verty[i]) + vertx[i]))
            c = !c;
    }
    return c;
};

CGraph::CGraph(std::vector<std::vector<int>> iGraph, std::vector<b2Vec2> iVertices, AppLog* coinsLog) {
    graph = iGraph;
    vertices = iVertices;

    log = coinsLog;


    for (int i = 0; i < graph.size(); i++) {                        //turn pairs of vertices into neighbour lists

        std::vector<int> v1 = std::vector<int>();
        neighbors.emplace(graph[i][0], v1);
        neighbors.at(graph[i][0]).push_back(graph[i][1]);

        std::vector<int> v2 = std::vector<int>();
        neighbors.emplace(graph[i][1], v2);
        neighbors[graph[i][1]].push_back(graph[i][0]);

        std::sort(neighbors[graph[i][0]].begin(), neighbors[graph[i][0]].end());
        neighbors[graph[i][0]].erase(std::unique(neighbors[graph[i][0]].begin(), neighbors[graph[i][0]].end()),
                                     neighbors[graph[i][0]].end());

        std::sort(neighbors[graph[i][1]].begin(), neighbors[graph[i][1]].end());
        neighbors[graph[i][1]].erase(std::unique(neighbors[graph[i][1]].begin(), neighbors[graph[i][1]].end()),
                                     neighbors[graph[i][1]].end());
    }
}


void CGraph::printNeighbors() {
    std::string s1;
    std::stringstream ss;
    //log->AddLog("lalala");
    for (auto it = neighbors.begin(); it != neighbors.end(); it++) {
        ss << it->first << ":";
        for (auto itt = it->second.begin(); itt != it->second.end(); itt++) {
            ss << *itt << ",";
        }
        ss<< std::endl;
    }
    log->AddLog(ss.str().c_str());
}


void CGraph::printVector(std::vector<int> line) {
    std::string s1;
    std::stringstream ss;
    //log->AddLog("lalala");
    for (auto it = line.begin(); it != line.end(); it++) {
            ss << *it << ",";
        }
        ss<< std::endl;
    log->AddLog(ss.str().c_str());
}


int CGraph::findLeftVertex(std::unordered_set<int> nodes) {
    int indMin = -1;
    float xMin = 100000;
    for (auto n1 = nodes.begin(); n1 != nodes.end(); n1++) {
        if (vertices[*n1].x < xMin) {
            xMin = vertices[*n1].x;
            indMin = *n1;
        }
    }
    return indMin;
};

std::vector<int> CGraph::cycle_red(std::vector<int> cycle, int n) {
    for (int i = 0; i < cycle.size(); i++) {
        std::vector<int> nc;
        std::vector<int> cycle_sorted(cycle);

        std::sort(neighbors[cycle[i]].begin(), neighbors[cycle[i]].end());
        std::sort(cycle_sorted.begin(), cycle_sorted.end());
        std::set_intersection(neighbors[cycle[i]].begin(), neighbors[cycle[i]].end(), cycle_sorted.begin(),
                              cycle_sorted.end(), std::back_inserter(nc));
        //nc.shrink_to_fit();
        // = neighbors[cycle[i]].Intersect(cycle).Tolist();
        if (nc.size() < 2) {
            cycle.erase(cycle.begin() + i);
            n = 1;
        }
    }
    if (n == 0)
        return cycle;
    else
        return cycle_red(cycle, 0);

}


std::vector<b2Vec2> CGraph::cycleToPolygon2(std::vector<int> cycle) {
    std::vector<b2Vec2> polygon = std::vector<b2Vec2>();


    polygon.push_back(vertices[cycle[0]]);
    for (int i = 1; i < cycle.size(); i++) {
        b2Vec2 pos = vertices[cycle[i]];
        polygon.push_back(pos);
    }
    return polygon;
}


int CGraph::getNextCWNeighbor(int i, int p) {
    std::vector<int> neighArray = std::vector<int>(neighbors[i]);
    neighArray.erase(std::remove(neighArray.begin(), neighArray.end(), p), neighArray.end());


    float minAngle = 100.0f;
    if (neighArray.size() == 0) {
        return p;
    }


    int minInd = neighArray[0];

    for (auto n1 = neighArray.begin(); n1 != neighArray.end(); n1++) {
        float a1 = angle(vertices[p], vertices[*n1], vertices[i]);
        if (abs(a1 - minAngle) < 1e-4) {
            if ((vertices[i] - vertices[*n1]).Length() < (vertices[i] - vertices[minInd]).Length()) {
                minAngle = a1;
                minInd = *n1;
            }
        }
        else if (a1 < minAngle) {
            minAngle = a1;
            minInd = *n1;
        }

    }
    return minInd;

}


float CGraph::angle(b2Vec2 a, b2Vec2 b, b2Vec2 center) {
    b2Vec2 ca = a - center;
    b2Vec2 cb = b - center;

    b2Vec2 t = ca;
    t.Normalize();

    float bx = b2Dot(t, cb);

    b2Vec2 n = ca;
    n.x = ca.y;
    n.y = -ca.x;

    n.Normalize();

    float by = b2Dot(n, cb);

    float angle = atan2(by, bx);

    return (angle > 0 ? angle : (2 * b2_pi + angle));
}


int CGraph::steepestNeighbor(int leftmost) {
    float kmax = -100000;
    int imax = neighbors[leftmost][0];

    std::vector<int> verticalNeighbors = std::vector<int>();

    for (auto n1 = neighbors[leftmost].begin(); n1 != neighbors[leftmost].end(); n1++) {

        if ((abs(vertices[*n1].x - vertices[leftmost].x) < 1e-3) && (vertices[*n1].y > vertices[leftmost].y)) {
            verticalNeighbors.push_back(*n1);
        }
        else {

            float k = (vertices[*n1].y - vertices[leftmost].y) / (vertices[*n1].x - vertices[leftmost].x);
            if (k > kmax) {
                kmax = k;
                imax = *n1;
            }
        }
    }


    if (verticalNeighbors.size() > 0) {

        auto imax_it = std::max_element(verticalNeighbors.begin(), verticalNeighbors.end(), [this](int n1, int n2) {
            return (vertices[n1].y < vertices[n2].y);
        });

        imax = *imax_it;

        //imax = verticalNeighbors.Aggregate((agg, next) = > (vertices[next].z > vertices[agg].z ? next : agg));

    }


    return imax;

}


std::vector<std::vector<int>> CGraph::findCycles2() {

    std::unordered_set<int> unprocessedVertices = std::unordered_set<int>();

    std::vector<std::vector<int>> cycleslist = std::vector<std::vector<int>>();

    cycleslist.clear();

    int currentCycleNum = 0;

    for (int i = 0; i < graph.size(); i++) {
        unprocessedVertices.insert(graph[i][0]);
        unprocessedVertices.insert(graph[i][1]);
    }
    int uv = 0;

    while (unprocessedVertices.size() > 0) {
        uv++;
        if (uv > 100)
            break;

        int leftmost = findLeftVertex(unprocessedVertices);
        if (leftmost == -1) {
            return cycleslist;
        }

        int imax = steepestNeighbor(leftmost);


        std::vector<int> currentCycle = std::vector<int>();

        currentCycle.push_back(leftmost);
        currentCycle.push_back(imax);

        int prevNode = leftmost;
        int currentNode = imax;
        int nextCWNeighbor = getNextCWNeighbor(imax, prevNode);

        //sw.WriteLine("Next vertex [" + nextCWNeighbor + "]" + vertices[nextCWNeighbor]);

        currentCycle.push_back(nextCWNeighbor);


        prevNode = currentNode;
        currentNode = nextCWNeighbor;
        int cc = 0;


        while (currentNode != leftmost) {
            cc++;
            if (cc > 100)
                break;

            //currentCycle.RemoveAt(currentCycle.Count-1);
            nextCWNeighbor = getNextCWNeighbor(currentNode, prevNode);
            //sw.WriteLine("Next vertex [" + nextCWNeighbor + "]" + vertices[nextCWNeighbor]);
            currentCycle.push_back(nextCWNeighbor);
            prevNode = currentNode;
            currentNode = nextCWNeighbor;

            //if (currentCycle[currentCycle.Count-1]==currentCycle[currentCycle.Count-3])
        }




        //std::set_difference(unprocessedVertices.begin(),unprocessedVertices.end(),s1.begin(),s1.end(),uv1.begin());

        for (auto n1 = currentCycle.begin(); n1 != currentCycle.end(); n1++) {
            auto v1 = unprocessedVertices.find(*n1);
            if (v1 != unprocessedVertices.cend())
                unprocessedVertices.erase(v1);
        }

        currentCycle = cycle_red(currentCycle, 0);


        if (currentCycle.size() > 0) {

            cycleslist.push_back(currentCycle);

            std::vector<b2Vec2> p1 = cycleToPolygon2(currentCycle);

            float vertx[p1.size()];
            float verty[p1.size()];

            for (int i = 0; i < p1.size(); i++) {
                vertx[i] = p1[i].x;
                verty[i] = p1[i].y;
            }

            for (int i = 0; i < vertices.size(); i++) {

                if (pnpoly(p1.size(), vertx, verty, vertices[i].x, vertices[i].y)) {
                    auto v1 = unprocessedVertices.find(i);
                    if (v1 != unprocessedVertices.cend())
                        unprocessedVertices.erase(v1);
                }
            }
        }
    }


    return cycleslist;

}

void testCgraph(AppLog* coinsLog) {

    std::vector<std::vector<int>> ig = std::vector<std::vector<int>>();

    std::vector<b2Vec2> vert = std::vector<b2Vec2>();

    vert.push_back(b2Vec2(0.f, 1.f));
    vert.push_back(b2Vec2(-1.f, 0.f));
    vert.push_back(b2Vec2(1.f, 0.f));




    vert.push_back(b2Vec2(10.f, 1.f));
    vert.push_back(b2Vec2(9.f, 0.f));
    vert.push_back(b2Vec2(11.f, 0.f));


    std::vector<int> v1 = std::vector<int>();

    v1.push_back(0);
    v1.push_back(1);

    ig.push_back(v1);

    v1[0] = 0;
    v1[1] = 2;

    ig.push_back(v1);

    v1[0] = 1;
    v1[1] = 2;

    ig.push_back(v1);


    ig.push_back(v1);

    v1[0] = 3;
    v1[1] = 5;

    ig.push_back(v1);

    v1[0] = 4;
    v1[1] = 5;

    ig.push_back(v1);

    v1[0] = 3;
    v1[1] = 4;

    ig.push_back(v1);



    coinsLog->AddLog("neighbors \n");
    CGraph *cg1 = new CGraph(ig, vert, coinsLog);

    cg1->printNeighbors();

    std::vector<std::vector<int>> c1  = cg1->findCycles2();
    coinsLog->AddLog("cycles \n");
    cg1->printVector(c1[0]);
    cg1->printVector(c1[1]);



};



/*




	








    }


 */
