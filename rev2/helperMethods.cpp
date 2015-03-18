// ECE556 - Copyright 2013 University of Wisconsin-Madison.  All Rights Reserved.

#include "ece556.h"

/*Takes pointers to two points and returns the manhattan distance between them*/
int manhattanDistance(point *a, point *b){
	return abs(a->x - b->x) + abs(a->y - b->y);
}

/*Takes a point and returns the edge directly right of given point*/
int getXEdge(point a){
	return (a.x + a.y*(xGridSize -1));
}
/*Takes a point and returns the edge directly above the given point*/
int getYEdge(point a){
	return (yGridSize*(xGridSize - 1) + (a.x + a.y*xGridSize));
}

/*Takes pointers to two points and returns the edge between them*/
int getEdge(point *a, point *b){
	point *smallerPt;
	if (a->x == b->x){
		if (a->y < b->y){
			smallerPt = a;
		}
		else{
			smallerPt = b;
		}
		return (yGridSize*(xGridSize - 1) + (smallerPt->x + smallerPt->y*xGridSize));
	}
	else {
		if (a->x < b->x){
			smallerPt = a;
		}
		else{
			smallerPt = b;
		}
		return (smallerPt->x + smallerPt->y*(xGridSize -1));
	}
}

void ZRoutes(routingInst *rst) {
	int j, k;
	cout << "Routing Z-shaped nets..." << endl;
	for (k = 0; k < rst->numNets; k++) {
		rst->nets[k].croutes = new route[1];
		point small;  //the smaller coordinate
		point big;  //the larger coordinate
		point temp;
		route zRoute;
		vector<point> points;  //Store all the points passed on the way,
								//parsed later into edges and segments
		int xUtil = 0; //Holds temporary utilization score to move in X direction
		int yUtil = 0; //Holds temporary utilization score to move in Y direction
		int xEdge;
		int yEdge;
		zRoute.numSegs = 0;
		for (j = 1; j < rst->nets[k].numPins; j++){
			points.clear();
			small = rst->nets[k].pins[j-1];
			big = rst->nets[k].pins[j];
			if (small.x != big.x){ //points are on different X axis
				if (small.x > big.x){ //Organize the points that small is to the left of big
					temp = small;
					small = big;
					big = temp;
				}
				if (big.y > small.y){ //Route up and to the right
					while ((big.x >= small.x) && (big.y >= small.y)){
						if(big.x == small.x){ //small is directly under big
							while (big.y >= small.y){
								points.push_back(small);
								small.y++;
							}
						}
						else if(big.y == small.y){ //small is directly to left of big
							while (big.x >= small.x){
								points.push_back(small);
								small.x++;
							}
						}
						else { // small is to left and below big
							points.push_back(small);
							xEdge = getXEdge(small);
							xUtil = rst->edgeUtils[xEdge] - rst->edgeCaps[xEdge];
							yEdge = getYEdge(small);
							yUtil = rst->edgeUtils[yEdge] - rst->edgeCaps[yEdge];
							if (xUtil < yUtil){
								small.x++;
							}
							else {
								small.y++;
							}
						}
					}
				}
				else if (big.y < small.y){ //Route down and to the right
					points.push_back(small);
					while ((big.x >= small.x) && (big.y <= small.y)){
						if(big.x == small.x){ //small is directly above big
							while (big.y <= small.y){
								points.push_back(small);
								small.y--;
							}
						}
						else if(big.y == small.y){ //small is directly to left of
							while (big.x >= small.x){
								points.push_back(small);
								small.x++;
							}
						}
						else { // small is to left and above big
							point modify; //Temporary point to be used in the edge calculation
							modify.x = small.x;
							modify.y = small.y - 1;
							points.push_back(small);
							xEdge = getXEdge(small);
							xUtil = rst->edgeUtils[xEdge] - rst->edgeCaps[xEdge];
							yEdge = getYEdge(modify);
							yUtil = rst->edgeUtils[yEdge] - rst->edgeCaps[yEdge];
							if (xUtil < yUtil){
								small.x++;
							}
							else {
								small.y--;
							}
						}
					}
				}
				else { //Route directly across in the x direction
					while (big.x >= small.x){
						points.push_back(small);
						small.x++;
					}
				}
			}
			else { // Route directly up and down as x values are the same
				if (small.y > big.y){
					temp = small;
					small = big; //small is lower on the y axis
					big = temp; //big is higher on the y axis
				}
				while (big.y >= small.y){
					points.push_back(small);
					small.y++;
				}
			}
			//Parse the points into edges and segments and add them to the route
			point T = points.at(points.size()-1); //The target
			point A = points.at(0); //The starting point
			point next = points.at(1); //The next point in path
			point prev;
			segment seg;
			seg.numEdges = 0;
			int currEdge;
			int nextIterator = 1;
			seg.p1 = A;
			prev = A;
			while (T != A){
				nextIterator++;
				currEdge = getEdge(&next, &A);
				//Detects a bend, completes the segment, and starts a new segment
				if ((next.y != seg.p1.y) && (next.x != seg.p1.x)){
					seg.p2 = A;
					zRoute.numSegs++;
					zRoute.segments.push_back(seg);
					seg.p1 = A;
					seg.numEdges = 1;
				}
				//No bend detected, continue on
				else {
					seg.numEdges++;
				}
				seg.edges.push_back(currEdge);
				rst->edgeUtils[currEdge]++;
				//Update points on the path and next iterator
				prev = A;
				A = next;
				//Make sure to not run off the point vector
				if (nextIterator < points.size()){
					next = points.at(nextIterator);
				}
			}
			seg.p2 = A;
			zRoute.numSegs++;
			zRoute.segments.push_back(seg);
		}
		//Add the route to the routing instance
		rst->nets[k].croutes[0] = zRoute;
	}
}

/*Updates the net weights of each net after initial routing or rip up and reroute*/
void updateUtil(routingInst* rst) {//Returns total overflow
	int i, j, k, tempEdge, netWeight;
	while (!(rst->pQueNets.empty())){
		rst->pQueNets.pop();
	}
	while (!rst->pQueNets.empty()){
		rst->pQueNets.pop();
	}
	for (i = 0; i < rst->numNets; i++){
		rst->nets[i].weight = 0;
		for (j = 0; j < rst->nets[i].croutes[0].numSegs; j++){
			netWeight = 0;
			for(k = 0; k < rst->nets[i].croutes[0].segments.at(j).numEdges; k++){
				tempEdge = rst->nets[i].croutes[0].segments.at(j).edges.at(k);
				netWeight = netWeight + (rst->edgeUtils[tempEdge] - rst->edgeCaps[tempEdge]);
				rst->nets[i].weight = rst->nets[i].weight + netWeight;
			}
		}
		rst->pQueNets.push(rst->nets[i]);
	}
}

/*Rips up a net and updates its edge utilization*/
void RipNet(routingInst* rst, int net){
	for (int i = 0; i<rst->nets[net].croutes[0].numSegs; i++){
		for (int j = 0; j < rst->nets[net].croutes[0].segments.at(i).numEdges; j++){
			int k = rst->nets[net].croutes[0].segments.at(i).edges.at(j);
			rst->edgeUtils[k]--;
		}
	}
	rst->nets[net].croutes[0].segments.clear();
	rst->nets[net].croutes[0].numSegs = 0;
	cout << "Rerouting net: " << net << endl;
}
