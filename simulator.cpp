#include <iostream>
#include <cstdio>
#include <vector>
#include <algorithm>
#include "node.cpp"

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

bool isNodeSlow() {
	return false;
}

bool areNodesConnected(int i, int j) {
	return true;
}

int main(int argc, char* argv[]) {
	cout<<"Yo C++"<<endl;
	if(argc > 1) {
		n = atoi(argv[1]);
	}
	cout<<"No. of nodes: "<<n<<endl;

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
		nodes[i].isNodeSlow = isNodeSlow();
	}
	return 0;
}