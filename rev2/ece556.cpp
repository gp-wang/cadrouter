// ECE556 - Copyright 2014 University of Wisconsin-Madison.  All Rights Reserved.

#include "ece556.h"

using namespace std;
net * nets;
point * pins;
int *edge_caps;
int *edge_Utils;
int block = 0;
int xGridSize;
int yGridSize;
/*
 * Read Benchmark parses the entire input file
 */
int readBenchmark(const char *fileName, routingInst *rst){
	ifstream myFile(fileName,ifstream::in);
	if (!myFile){ //if the file was not successfully opened
		cout << "Unable to open " << fileName << " for reading" << endl;
		myFile.close();
		return 0;
	}
	string line;
	int net_cnt = 0;
	int pin_cnt = 0;
	while (!getline(myFile, line).eof()){
		string result;
		istringstream iss(line);

		if (line.find("\t")!=string::npos){
			getline(iss,result, '\t');
		}
		else{
			getline(iss,result, ' ');
		}
			//parses the grid dimensions
			if (result.find("grid")!=string::npos){
				string token;
				getline(iss,token, ' ');
				istringstream (token) >> rst->gx;
				getline(iss,token, ' ');
				istringstream (token) >> rst->gy;
				int x = rst->gx;
				xGridSize = x;
				int y = rst->gy;
				yGridSize = y;
				rst->numEdges = y*(x - 1) + x*(y - 1);
			}
			//parses the edge capacity
			else if (result.find("capacity") != string::npos){
				string token;
				getline(iss, token, ' ');
				istringstream(token) >> rst->cap;
				edge_caps = new int[rst->numEdges];
				edge_Utils = new int[rst->numEdges];
				std::fill(edge_caps, edge_caps + rst->numEdges, rst->cap);
			}
			//parses number of nets
			else if (result.find("num")!=string::npos){
				string token;
				getline(iss, token, ' ');
				getline(iss, token, ' ');
				istringstream(token) >> rst->numNets;
				nets = new net[rst->numNets];
			}
			//parses the net and number of pins
			else if (result.find('n')!= string::npos){
				string token;
				net net;
				net.id = net_cnt;
				getline(iss, token, ' ');
				istringstream(token) >> net.numPins;
				nets[net_cnt] = net;
				pins = new point[nets[net_cnt].numPins];
				net_cnt++;
				pin_cnt = 0;
			}
			//parses each pin
			else if ((pin_cnt < nets[net_cnt - 1].numPins) && ((net_cnt-1) < rst->numNets) ){
				point pin;
				istringstream(result)>>pin.x;
				string token;
				getline(iss,token,'\n');
				istringstream(token) >> pin.y;
				pins[pin_cnt] = pin;
				pin_cnt++;
				if (pin_cnt == nets[net_cnt - 1].numPins){
					vector<int> used;
					point *tmp = new point[nets[net_cnt-1].numPins];
					int loc = 0;
					//Net decomposition, sorts pins in order of Manhattan Distance
					//Initially selects pin closest to origin as the first position
					for(int j = 0; j < pin_cnt;j++) {
							int min = 100000;
							int min_init = 100000;
							for (int i = 0; i < pin_cnt; i++) {
								if (j == 0){
									if ((pins[i].x + pins[i].y)<min_init){
										min_init = pins[i].x + pins[i].y;
										loc = i;
									}
								}
								else if ((manhattanDistance(&pins[i], &tmp[j-1]) < min)&&(find(used.begin(),used.end(), i)==used.end())) {
									min = manhattanDistance(&pins[i], &tmp[j-1]) ;
									loc = i;
								}
							}
							tmp[j] = pins[loc];
							used.push_back(loc);
						}
					nets[net_cnt - 1].pins = tmp;
				}
			}
			//adds the blockages and new edge capacities
			else if (block > 0){
				point p1;
				point p2;
				int cap;
				istringstream(result) >> p1.x;
				string token;
				getline(iss,token,' ');
				istringstream(token) >> p1.y;
				getline(iss,token,' ');
				istringstream(token) >> p2.x;
				getline(iss,token,' ');
				istringstream(token) >> p2.y;
				getline(iss,token,'\n');
				istringstream(token) >> cap;

				point *p1_ptr = &p1;
				point *p2_ptr = &p2;
				int edge = getEdge(p1_ptr,p2_ptr);
				edge_caps[edge] = cap;
				block--;
			}
			else{
				istringstream(result) >> block;
			}
	}
	rst->nets = nets;
	rst->edgeCaps = edge_caps;
	rst->edgeUtils = edge_Utils;
	myFile.close();
	return 1;
}
/*
 * solves the routing instance
 */
int solveRouting(routingInst *rst){
	cout << "Started routing...\n";
	ZRoutes(rst);
	updateUtil(rst);
	aStarRoute(rst);
	cout << "Completed routing\n";
	return 1;
}
/*
 * Writes the output file
 */
int writeOutput(const char *outRouteFile, routingInst *rst){
	ofstream outFile(outRouteFile);
	if (!outFile){
		cout << "Unable to open " << outRouteFile << " for writing" << endl;
		outFile.close();
		return 0;
	}
	for (int i = 0; i < rst->numNets; i++){
		outFile << "n" << rst->nets[i].id << endl;
		for (int j = 0; j < rst->nets[i].croutes->numSegs; j++){
			segment seg = rst->nets[i].croutes->segments.at(j);
			outFile << "(" << seg.p1.x << "," << seg.p1.y << ")-";
			outFile << "(" << seg.p2.x << "," << seg.p2.y << ")" << endl;
		}
		outFile << "!" << endl;
	}

	outFile.close();
	return 1;
}
/*
 * Frees up memory
 */
int release(routingInst *rst){
	rst->cap = 0;
	rst->numEdges = 0;
	rst->gx = 0;
	rst->gy = 0;
	delete [] rst->edgeCaps;
	rst->edgeCaps = NULL;
	delete [] rst->edgeUtils;
	rst->edgeUtils = NULL;
	for (int i = 0; i < rst->numNets; i++){
		delete [] rst->nets[i].croutes;
		delete [] rst->nets[i].pins;
	}
	rst->numNets = 0;
	rst->nets = NULL;
	return 1;
}
