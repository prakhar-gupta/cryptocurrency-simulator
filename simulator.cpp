#include <iostream>
#include <cstdio>
#include <vector>
#include <algorithm>
#include "node.cpp"
#include "distributions.hpp"

using namespace std;

int n;				//Number of peers
float z;			//Percent of slow nodes [0-100]
float tMean;		//Mean time of exp-dist of transaction gen
float qMean;		//Mean time of exp-dist of queue delay 
					//for message transmission on a path
float bMean;		//Mean time of exp-dist of block mine time
const int MINING_FEES = 50;		//Mining fees to generate block

bool** isConnected;	//2D matrix signifying which nodes are connected to each other. indexing from 0 to n-1.
Node* nodes;		//Array of all nodes

//Is node i slow
//TODO
bool isNodeSlow(int i) {
	return false;
}

//Are nodes i and j connected to each other
//TODO
bool areNodesConnected(int i, int j) {
	return true;
}

//The initial balance that node i has
//TODO
float getNodeInitBalance(int i) {
	return 20;
}

//Time to next transaction from the exp dist
//TODO
float timeForNextTransaction() {
	return 2.5;
}

Transaction generateTransaction() {
	Transaction t;
	t.from = 0;
	t.to = 3;
	t.value = 1.5;
	return t;
}

int main(int argc, char* argv[]) {
    srand (time(NULL));
	cout << "Yo C++" << endl;
    cout << "exp test" << exponential(0.01) << endl;
	if(argc > 1) {
		n = atoi(argv[1]);
	}
	cout << "No. of nodes: " << n << endl;

	//Setup connection between nodes
	isConnected = new bool*[n];
	for(int i=0;i<=n;i++) {
		isConnected[i] = new bool[n];
	}
	for(int i=0;i<n;i++) {
		for(int j=i+1;j<n;j++) {
			isConnected[i][j] = areNodesConnected(i, j);
			isConnected[j][i] = isConnected[i][j];
		}
	}

	//Initialize nodes
	nodes = new Node[n];
	for(int i=0;i<n;i++) {
		nodes[i].isNodeSlow = isNodeSlow(i);
		nodes[i].isNodeSlow = getNodeInitBalance(i);
	}

	nodes[0].receiveNewTransaction(generateTransaction());
	return 0;
}
