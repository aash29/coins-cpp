
#include "graph2.h"
#include <iostream>


CGraph::CGraph(std::vector<std::vector<int>> iGraph) {
	graph = iGraph;
	//vertices = iVertices;

	for (int i = 0; i < graph.size(); i++) {

		std::vector<int> *v1 = new std::vector<int>();
		neighbors->emplace(graph[i][0],*v1);
		neighbors->at(graph[i][0]).push_back(graph[i][1]);

		std::vector<int> *v2 = new std::vector<int>();
		neighbors->emplace(graph[i][1], *v2);
		neighbors->at(graph[i][1]).push_back(graph[i][0]);
		std::sort(neighbors->at(graph[i][0]).begin(), neighbors->at(graph[i][0]).end());
		std::unique(neighbors->at(graph[i][0]).begin(), neighbors->at(graph[i][0]).end());

		std::sort(neighbors->at(graph[i][1]).begin(), neighbors->at(graph[i][1]).end());
		std::unique(neighbors->at(graph[i][1]).begin(), neighbors->at(graph[i][1]).end());
	}
}

void CGraph::printNeighbors() {
	std::string s1;
	for (auto it = neighbors->begin();it!=neighbors->end();it++) {
		std::cout << it->first << ":";
		for (auto itt = it->second.begin(); itt != it->second.end(); itt++) {
			std::cout << *itt << ",";
		}
		std::cout << std::endl;
	}
}

void testCgraph()
{
std::vector<std::vector<int>>* ig = new std::vector<std::vector<int>>();

std::vector<int>* v1 = new std::vector<int>();
v1->push_back(0);
v1->push_back(1);

ig->push_back(*v1);

std::vector<int>* v2 = new std::vector<int>();
v2->push_back(1);
v2->push_back(2);

ig->push_back(*v2);

CGraph* cg1 = new CGraph(*ig);

cg1->printNeighbors();
};



/*
				if (neighbors->find(graph[i][0])!=neighbors->cend())
				{
					std::vector<int> v0 = neighbors->at(graph[i][0]);

					if (std::find(v0.begin(), find(graph[i][1])!=neighbors[graph[i][0]].cend()) {
						((*neighbors)[graph[i][0]]).push_back(graph[i][1]);
					}
				}
				else
				{
					neighbors.Add(graph[i][0], new list<int>());
					neighbors[graph[i][0]].Add(graph[i][1]);
				}

				if (neighbors.ContainsKey(graph[i][1]))
				{
					if (!neighbors[graph[i][1]].Contains(graph[i][0]))
						neighbors[graph[i][1]].Add(graph[i][0]);
				}
				else
				{
					neighbors.Add(graph[i][1], new list<int>());
					neighbors[graph[i][1]].Add(graph[i][0]);
				}
			}

        }


		public list<list<int>> findCycles2()
		{

			HashSet<int> unprocessedVertices = new HashSet<int>();

			list<list<int>> cycleslist = new list<list<int>> ();

			int currentCycleNum = 0;

			for (int i=0; i<graph.Count; i++) 
			{
				unprocessedVertices.Add(graph[i][0]);
				unprocessedVertices.Add(graph[i][1]);
			}
			int uv = 0;

			while (unprocessedVertices.Count>0) {
				uv++;
				if (uv>100)
					break;

								int leftmost = findLeftVertex (unprocessedVertices);
								if (leftmost == -1) {
										return null;
								}
								//sw.WriteLine("Leftmost vertex [" + leftmost + "]" + vertices[leftmost]);


								int imax = steepestNeighbor (leftmost);

								//sw.WriteLine("Steepest neighbor [" + imax + "]" + vertices[imax]);


			

								list<int> currentCycle = new list<int> ();

								currentCycle.Add (leftmost);
								currentCycle.Add (imax);

								int prevNode = leftmost;
								int currentNode = imax;
								int nextCWNeighbor = getNextCWNeighbor (imax, prevNode);

								//sw.WriteLine("Next vertex [" + nextCWNeighbor + "]" + vertices[nextCWNeighbor]);

								currentCycle.Add (nextCWNeighbor);



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
										currentCycle.Add (nextCWNeighbor);
										prevNode = currentNode;
										currentNode = nextCWNeighbor;

										//if (currentCycle[currentCycle.Count-1]==currentCycle[currentCycle.Count-3])
								}



								foreach (int n1 in currentCycle)
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

			return cycleslist;

		}


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

		public int getNextCWNeighbor(int i,int p)
		{
			list<int> neighArray = new list<int>(neighbors [i]);
			neighArray.Remove (p);

			float minAngle = 100.0f;
			if (neighArray.Count==0)
			{
				return p;
			}


			int minInd = neighArray [0];

			foreach (var n1 in neighArray) 
			{
				float a1 = angle(vertices[p],vertices[n1],vertices[i]);
				if (Mathf.Abs(a1-minAngle)<1e-4)				{
					if (Vector3.Distance(vertices[i],vertices[n1])<Vector3.Distance(vertices[i],vertices[minInd]))
					{
						minAngle=a1;
						minInd=n1;
					}
				}
				else
				if (a1<minAngle)
				{
					minAngle=a1;
					minInd=n1;
				}

			}
			return minInd;

		}




		float angle(Vector3 a, Vector3 b, Vector3 center)
		{
			Vector3 ca = a - center;
			Vector3 cb = b - center;

			Vector3 t = Vector3.Normalize (ca);

			float bx = Vector3.Dot (t, cb);

			Vector3 n = ca;
			n.x = ca.z;
			n.z = -ca.x;

			n.Normalize ();

			float by = Vector3.Dot (n, cb);

			float angle = Mathf.Atan2 (by, bx);

			return (angle > 0 ? angle : (2*Mathf.PI + angle));
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


		public int steepestNeighbor(int leftmost)
		{
			float kmax = -100000;
			int imax = neighbors[leftmost][0];
			
			list<int> verticalNeighbors = new list<int>();
			
			foreach (int n1 in neighbors[leftmost]) 
			{
				
				if ((Mathf.Abs(vertices[n1].x-vertices[leftmost].x)<1e-3) && (vertices[n1].z>vertices[leftmost].z) )
				{
					verticalNeighbors.Add(n1);
				}
				else
				{
					
					float k=(vertices[n1].z-vertices[leftmost].z)/(vertices[n1].x-vertices[leftmost].x);
					if (k>kmax)
					{
						kmax=k;
						imax = n1;
					}
				} 
			}
			
			if (verticalNeighbors.Count > 0) 
			{

				imax=verticalNeighbors.Aggregate((agg, next) => (vertices[next].z > vertices[agg].z ? next : agg));
				
			}

			return imax;

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
