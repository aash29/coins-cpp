
#include "graph2.h"
#include <iostream>


CGraph::CGraph(std::vector<std::vector<int>> iGraph) {
	graph = iGraph;
	//vertices = iVertices;

	for (int i = 0; i < graph.size(); i++) {						//turn pairs of vertices into neighbour lists

		std::vector<int> *v1 = new std::vector<int>();
		neighbors.emplace(graph[i][0],*v1);
		neighbors.at(graph[i][0]).push_back(graph[i][1]);

		std::vector<int> *v2 = new std::vector<int>();
		neighbors.emplace(graph[i][1], *v2);
		neighbors[graph[i][1]].push_back(graph[i][0]);

		std::sort(neighbors[graph[i][0]].begin(), neighbors[graph[i][0]].end());
		neighbors[graph[i][0]].erase(std::unique(neighbors[graph[i][0]].begin(), neighbors[graph[i][0]].end()), neighbors[graph[i][0]].end());

		std::sort(neighbors[graph[i][1]].begin(), neighbors[graph[i][1]].end());
		neighbors[graph[i][1]].erase(std::unique(neighbors[graph[i][1]].begin(), neighbors[graph[i][1]].end()), neighbors[graph[i][1]].end());
	}
}

void CGraph::printNeighbors() {
	std::string s1;
	for (auto it = neighbors.begin();it!=neighbors.end();it++) {
		std::cout << it->first << ":";
		for (auto itt = it->second.begin(); itt != it->second.end(); itt++) {
			std::cout << *itt << ",";
		}
		std::cout << std::endl;
	}
}


int CGraph::findLeftVertex(std::unordered_set<int> nodes)
{
	int indMin = -1;
	float xMin = 100000;
	for (auto n1 = nodes.begin(); n1 != nodes.end();n1++)
	{
		if (vertices[*n1].x<xMin)
		{
			xMin = vertices[*n1].x;
			indMin = *n1;
		}
	}
	return indMin;
};


int CGraph::getNextCWNeighbor(int i, int p)
{
	std::vector<int> neighArray = std::vector<int>(neighbors[i]);
	neighArray.erase(std::remove(neighArray.begin(), neighArray.end(), p), neighArray.end());


	float minAngle = 100.0f;
	if (neighArray.size() == 0)
	{
		return p;
	}


	int minInd = neighArray[0];

	foreach(var n1 in neighArray)
	{
		float a1 = angle(vertices[p], vertices[n1], vertices[i]);
		if (Mathf.Abs(a1 - minAngle)<1e-4) {
			if (Vector3.Distance(vertices[i], vertices[n1])<Vector3.Distance(vertices[i], vertices[minInd]))
			{
				minAngle = a1;
				minInd = n1;
			}
		}
		else
			if (a1<minAngle)
			{
				minAngle = a1;
				minInd = n1;
			}

	}
	return minInd;

}




float CGraph::angle(b2Vec2 a, b2Vec2 b, b2Vec2 center)
{
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


 int CGraph::steepestNeighbor(int leftmost)
{
	float kmax = -100000;
	int imax = neighbors[leftmost][0];

	std::vector<int> verticalNeighbors = std::vector<int>();

	for (auto n1 = neighbors[leftmost].begin(); n1 != neighbors[leftmost].end(); n1++)
	{

		if ((abs(vertices[*n1].x - vertices[leftmost].x)<1e-3) && (vertices[*n1].y>vertices[leftmost].y))
		{
			verticalNeighbors.push_back(*n1);
		}
		else
		{

			float k = (vertices[*n1].y - vertices[leftmost].y) / (vertices[*n1].x - vertices[leftmost].x);
			if (k>kmax)
			{
				kmax = k;
				imax = *n1;
			}
		}
	}



	if (verticalNeighbors.size() > 0)
	{

		auto imax_it = std::max_element(verticalNeighbors.begin(), verticalNeighbors.end(),  [this](int n1, int n2) {
			return (vertices[n1].y < vertices[n2].y);
		});

		imax = *imax_it;

		//imax = verticalNeighbors.Aggregate((agg, next) = > (vertices[next].z > vertices[agg].z ? next : agg));

	}


	return imax;

}


std::vector<std::vector<int>> CGraph::findCycles2()
		{

			std::unordered_set<int> unprocessedVertices = std::unordered_set<int>();

			std::vector<std::vector<int>> cycleslist = std::vector<std::vector<int>>();

			cycleslist.clear();

			int currentCycleNum = 0;

			for (int i=0; i<graph.size(); i++) 
			{
				unprocessedVertices.insert(graph[i][0]);
				unprocessedVertices.insert(graph[i][1]);
			}
			int uv = 0;
			
			while (unprocessedVertices.size()>0) {
				uv++;
				if (uv>100)
					break;

								int leftmost = findLeftVertex (unprocessedVertices);
								if (leftmost == -1) {
										return cycleslist;
								}

								int imax = steepestNeighbor (leftmost);


								std::vector<int> currentCycle = std::vector<int> ();

								currentCycle.push_back (leftmost);
								currentCycle.push_back (imax);

								int prevNode = leftmost;
								int currentNode = imax;
								int nextCWNeighbor = getNextCWNeighbor (imax, prevNode);

								//sw.WriteLine("Next vertex [" + nextCWNeighbor + "]" + vertices[nextCWNeighbor]);

								currentCycle.push_back (nextCWNeighbor);



								prevNode = currentNode;
								currentNode = nextCWNeighbor;
								int cc = 0;
								

								while (currentNode!=leftmost) {
										cc++;
										if (cc > 100) 
												break;

										//currentCycle.RemoveAt(currentCycle.Count-1);
										nextCWNeighbor = getNextCWNeighbor (currentNode, prevNode);
										//sw.WriteLine("Next vertex [" + nextCWNeighbor + "]" + vertices[nextCWNeighbor]);
										currentCycle.push_back (nextCWNeighbor);
										prevNode = currentNode;
										currentNode = nextCWNeighbor;

										//if (currentCycle[currentCycle.Count-1]==currentCycle[currentCycle.Count-3])
								}



								for (int n1 in currentCycle)
								{
									unprocessedVertices.Remove(n1);
								}

								currentCycle=cycle_red (currentCycle,0);				
				


								if (currentCycle.Count>0)
								{
								
									cycleslist.Add (currentCycle);

									list<Vector2> p1 = cycleToPolygon2(currentCycle) ;

									Polygon pe = Polygon.PolygonWithPointlist(p1);
								
									for (int i=0; i< vertices.Count;i++)
									{
								
									if (pe.ContainsPoint(vertices[i].xz()) || pe.PermiterContainsPoint(vertices[i].xz()) )
										//if ( pe.PermiterContainsPoint(vertices[i].xz()) )
										//if (pe.ContainsPoint(vertices[i].xz()))
											unprocessedVertices.Remove(i);
											//protectedCoins.Add(i);
									}
								}
						}
						*/

			return cycleslist;

		}

		void testCgraph()
		{
			std::vector<std::vector<int>>* ig = new std::vector<std::vector<int>>();

			std::vector<int> v1 = std::vector<int>();
			v1.push_back(0);
			v1.push_back(1);

			ig->push_back(v1);

			v1[0] = 0;
			v1[1] = 2;

			ig->push_back(v1);

			v1[0] = 0;
			v1[1] = 3;

			ig->push_back(v1);

			v1[0] = 1;
			v1[1] = 0;

			ig->push_back(v1);


			v1[0] = 1;
			v1[1] = 4;

			ig->push_back(v1);


			v1[0] = 2;
			v1[1] = 0;

			ig->push_back(v1);

			v1[0] = 2;
			v1[1] = 3;

			ig->push_back(v1);

			v1[0] = 3;
			v1[1] = 0;

			ig->push_back(v1);


			v1[0] = 3;
			v1[1] = 4;

			ig->push_back(v1);


			v1[0] = 1;
			v1[1] = 2;

			ig->push_back(v1);


			v1[0] = 2;
			v1[1] = 1;

			ig->push_back(v1);

			CGraph* cg1 = new CGraph(*ig);

			cg1->printNeighbors();
		};



/*
		public list<int> cycle_red(list<int> cycle, int n)
		{
			for (int i=0;i<cycle.Count; i++) 
			{
				list<int> nc = neighbors[cycle[i]].Intersect(cycle).Tolist();
				if (nc.Count<2)
				{
					cycle.RemoveAt(i);
					n=1;
				}
			}
			if (n==0) 
				return cycle;
			else
				return cycle_red (cycle,0);

		}



	


        public int findLeftVertex(HashSet<int> nodes)
        {
            int indMin = -1;
            float xMin = 100000;
            foreach (int n1 in nodes)
			//for (int i=0; i<vertices.Count; i++) 
            {
                if (vertices[n1].x<xMin)
                {
                    xMin=vertices[n1].x;
                    indMin = n1;
                }
            }
            return indMin;
        }




		public bool IsPointInPolygon( Vector3 p, Vector3[] polygon )
		{
			double minX = polygon[ 0 ].x;
			double maxX = polygon[ 0 ].x;
			double minY = polygon[ 0 ].z;
			double maxY = polygon[ 0 ].z;
			for ( int i = 1 ; i < polygon.Length ; i++ )
			{
				Vector3 q = polygon[ i ];
				minX = Math.Min( q.x, minX );
				maxX = Math.Max( q.x, maxX );
				minY = Math.Min( q.z, minY );
				maxY = Math.Max( q.z, maxY );
			}
			
			if ( p.x < minX || p.x > maxX || p.z < minY || p.z > maxY )
			{
				return false;
			}
			
			// http://www.ecse.rpi.edu/Homepages/wrf/Research/Short_Notes/pnpoly.html
			bool inside = false;
			for ( int i = 0, j = polygon.Length - 1 ; i < polygon.Length ; j = i++ )
			{
				if ( ( polygon[ i ].z > p.z ) != ( polygon[ j ].z > p.z ) &&
				    p.x < ( polygon[ j ].x - polygon[ i ].x ) * ( p.z - polygon[ i ].z ) / ( polygon[ j ].z - polygon[ i ].z ) + polygon[ i ].x )
				{
					inside = !inside;
				}
			}
			
			return inside;
		}

		public list<Vector2> cycleToPolygon2(list<int> cycle)
		{
			list<Vector2> polygon = new list<Vector2> ();
		

			polygon.Add (vertices[cycle [0]].xz ());
			for (int i = 1; i < cycle.Count; i++) 
				{
			
				Vector2 pos = vertices[cycle [i]].xz ();
					polygon.Add (pos);
				}
			return polygon;
		}

		public list<Vector3> cycleToPolygon(list<int> cycle)
		{
			list<Vector3> polygon = new list<Vector3> ();
			
			
			polygon.Add (vertices[cycle [0]]);
			for (int i = 1; i < cycle.Count; i++) 
			{
				
				Vector3 pos = vertices[cycle [i]];
				polygon.Add (pos);
			}
			return polygon;
		}


    }


 */
