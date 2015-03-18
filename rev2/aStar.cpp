#include "ece556.h"

class scoreComparison
{
	bool reverse;
public:
	scoreComparison(const bool& revparam= false)
{reverse = revparam;}
	bool operator() (const point &s1, const point &s2) const{
		if (reverse) return (s1.locScore < s2.locScore);
		return (s1.locScore > s2.locScore);
	}
};
/*
 * Performs A* Search and in order to RRR
 */
void aStarRoute (routingInst* rst){
	int j;
	map<point, int> group;
	map<point, int> distance;
	map<point, int> score;
	map<point, point> parent;
	priority_queue<net, vector<net>, NetComparator> pQueNets;
	time_t startTime, elapsedTime;
	time(&startTime);
	int seconds = 0;
	int maxWeight = rst->pQueNets.top().weight;
	net currNet = rst->pQueNets.top();
	//Runs for either 15mins or When the gains are marginal
	while ((seconds < (15*60))&&(currNet.weight>=5*maxWeight/8)&&(!rst->pQueNets.empty())){
		currNet = rst->pQueNets.top();
		RipNet(rst, currNet.id);
		rst->pQueNets.pop();
		for (j = 1; j < currNet.numPins; j++){
			point S, T, u;
			point * T_ptr = &T;
			priority_queue<point, vector<point>, scoreComparison> astar_pq;
			astar_pq = priority_queue<point, vector<point>, scoreComparison>();
			S = currNet.pins[j-1];
			T = currNet.pins[j];

			parent.clear();
			score.clear();
			distance.clear();
			group.clear();

			// initialize the maps
			for (int e = 0; e < rst->gx; e++){
				for (int d = 0; d < rst->gy; d++){
					point temp;
					temp.x = e;
					temp.y = d;
					group[temp] = 1;
					distance[temp] = INT_MAX;
					score[temp] = INT_MAX;
					temp.locScore = INT_MAX;
					parent[temp] = temp;
				}
			}
			distance[S] = 0;
			score[S] = 0;
			S.locScore = 0;
			//Maps initialized

			astar_pq.push(S);
			//run until End is found or A* Queue is empty
			while (!astar_pq.empty()){
				u = astar_pq.top();
				astar_pq.pop();
				group[u] = 3;
				//checks to see if next point is the end
				if (u.x == T.x && u.y == T.y){
					while (!astar_pq.empty()){
						astar_pq.pop();
					}
					retrace(S, u, rst, &parent, currNet.id);
					break;
				}
				else {
					int currEdge;
					int tempDist;
					point modifier;
					currEdge = getYEdge(u);
					//checks net y+1 away
					if (((u.y + 1 ) < rst->gy)){
						//check goal or update queue
						point v;
						point * v_ptr = &v;
						v.x = u.x;
						v.y = u.y + 1;
						tempDist = distance[u] + rst->edgeUtils[currEdge];
						if ((3 == group[v]) && (distance[v] <= tempDist)){
							//continue to next
						}
						else if ((2 != group[v]) || (distance[v] > tempDist)){
							parent[v] = u;
							distance[v] = tempDist;
							score[v] = distance[v] + manhattanDistance(v_ptr,T_ptr);
							v.locScore = score[v];
							group[v] = 2;
							astar_pq.push(v);
						}
					}
					//checks net x+1 away
					currEdge = getXEdge(u);
					if (((u.x + 1 ) < rst->gx)){
						//check goal or update queue
						point v;
						point * v_ptr = &v;
						v.x = u.x + 1;
						v.y = u.y;
						tempDist = distance[u] + rst->edgeUtils[currEdge];
						if ((3 == group[v]) && (distance[v] <= tempDist)){
							//continue to next
						}
						else if ((2 != group[v]) || (distance[v] > tempDist)){
							parent[v] = u;
							distance[v] = tempDist;
							score[v] = distance[v] + manhattanDistance(v_ptr,T_ptr);
							v.locScore = score[v];
							group[v] = 2;
							astar_pq.push(v);
						}
					}
					modifier = u;
					modifier.y--;
					currEdge = getYEdge(modifier);
					//checks net y-1 away
					if (u.y > 0){
						//check goal or update queue
						point v;
						point * v_ptr = &v;
						v.x = u.x;
						v.y = u.y - 1;
						tempDist = distance[u] + rst->edgeUtils[currEdge];
						if ((3 == group[v]) && (distance[v] <= tempDist)){
							//continue to next
						}
						else if ((2 != group[v]) || (distance[v] > tempDist)){
							parent[v] = u;
							distance[v] = tempDist;
							score[v] = distance[v] + manhattanDistance(v_ptr,T_ptr);
							v.locScore = score[v];
							group[v] = 2;
							astar_pq.push(v);
						}
					}
					modifier.y++;
					modifier.x--;
					//checks net x-1 away
					currEdge = getXEdge(modifier);
					if (u.x > 0){
						//check goal or update queue
						point v;
						point * v_ptr = &v;
						v.x = u.x - 1;
						v.y = u.y;
						tempDist = distance[u] + rst->edgeUtils[currEdge];
						if ((3 == group[v]) && (distance[v] <= tempDist)){
						}
						else if ((2 != group[v]) || (distance[v] > tempDist)){
							parent[v] = u;
							distance[v] = tempDist;
							score[v] = distance[v] + manhattanDistance(v_ptr,T_ptr);
							v.locScore = score[v];
							group[v] = 2;
							astar_pq.push(v);
						}
					}

				}
			}
		}
		pQueNets.push(currNet);
		seconds = time(&elapsedTime)-startTime;
	}
	while (!rst->pQueNets.empty()){
		pQueNets.push(rst->pQueNets.top());
		rst->pQueNets.pop();
	}
	rst->pQueNets = pQueNets;
}
void retrace (point S, point A, routingInst* rst, map<point, point> *parent, int netID){
	//trace back from A to parent to parent etc, form segments, and nets,
	//push to priority queue;
	point rent = (*parent)[A];
	point prev;
	segment seg;
	seg.numEdges = 0;
	int currEdge;
	seg.p1 = A;
	prev = A;
	//keep tracing parent until back to origin
	while (S != A){
		currEdge = getEdge(&rent, &A);
		if ((rent.y != seg.p1.y) && (rent.x != seg.p1.x)){
			seg.p2 = A;
			rst->nets[netID].croutes[0].numSegs++;
			rst->nets[netID].croutes[0].segments.push_back(seg);
			//store seg
			seg.p1 = A;
			seg.numEdges = 1;
		}
		else {
			seg.numEdges++;
		}
		seg.edges.push_back(currEdge);
		rst->edgeUtils[currEdge]++;
		prev = A;
		A = rent;
		rent = (*parent)[A];
	}
	seg.p2 = A;
	rst->nets[netID].croutes[0].numSegs++;
	rst->nets[netID].croutes[0].segments.push_back(seg);
}
