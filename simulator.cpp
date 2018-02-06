#include <iostream>
#include <cstdio>
#include <vector>
#include <algorithm>
#include <queue>
#include "node.cpp"
#include "distributions.hpp"

using namespace std;

int n;				//Number of peers
float z;			//Percent of slow nodes [0-100]
float tMean;		//Mean time of exp-dist of transaction gen
float bMean;		//Mean time of exp-dist of block mine time

const int MINING_FEES = 50;		//Mining fees to generate block
const int INIT_BALANCE = 40;	//Initial balance of nodes

bool** isConnected;	//2D matrix signifying which nodes are connected to each other. indexing from 0 to n-1.
float** speedOfLightDelay;
Node* nodes;		//Array of all nodes
int latestTransactionID;	//ID of the latest generated transaction
float currentTime;	//Current simulation time

struct TimeInterrupt {
	float time;
	int type;		//1 -> Generate transaction
					//2 -> Transfer transaction/block message
					//3 -> generate block
	int from, to;	//Sender and receipient in case of message passing
	Transaction t;
	int blockGeneratorNodeID;
};

struct InterruptCompare
{
    bool operator()(const TimeInterrupt& lhs, const TimeInterrupt& rhs)
    {
        return lhs.time < rhs.time;
    }
};

priority_queue <TimeInterrupt, vector<TimeInterrupt>, InterruptCompare> timer;	//Priority queue for all events

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
	return INIT_BALANCE;
}

//Time to next transaction from the exp dist
//TODO
float timeForNextTransaction() {
	return 2.5;				//TODO
}

//Helper for generateTransaction()
int getIDForNewTransaction() {
	return ++latestTransactionID;
}

//Helper for generateTransaction()
int getFromNode() {
	return 3;				//TODO
}

//Helper for generateTransaction()
int getToNode() {
	return 5;				//TODO
}

//Helper for generateTransaction()
//TODO
float getNewTransactionValue(int id) {
	float value = 5.8;				//TODO
	if(value < nodes[id].balance) {
		return value;
	}
	return nodes[id].balance/2;
}

//TODO
float getQueueingDelay(int c, int i, int j) {
	return 2.3;						//TODO
}

float getTransmissionDelay(int m, int i, int j) {
	float p = speedOfLightDelay[i][j];
	float c = 100.0;
	if(nodes[i].isNodeSlow || nodes[j].isNodeSlow) {
		c = 5.0;
	}
	float d = getQueueingDelay(c, i, j);
	return p + m/c + d;
}

void generateTransaction() {
	Transaction t;
	t.id = getIDForNewTransaction();
	t.from = getFromNode();
	t.to = getToNode();
	t.value = getNewTransactionValue(t.from);

	cout<<"Generating txn from "<<t.from<<" to "<<t.to<<" for "<<t.value<<endl;
	nodes[t.from].generateNewTransaction(t);

	//Generate TimerInterrupt for message transfer
	for(int i=0;i<n;i++) {
		if(i == t.from) continue;
		if(isConnected[t.from][i]) {
			TimeInterrupt ti;
			float delay = getTransmissionDelay(0.0, t.from, i);
			ti.time = currentTime + delay;
			ti.type = 2;
			ti.t = t;
			ti.from = t.from;
			ti.to = i;
			timer.push(ti);
		}
	}
}

void addFirstInterrupt() {
	TimeInterrupt ti;
	ti.time = 0.0;
	ti.type = 1;
	timer.push(ti);
}

void startSimulation() {
	while(!timer.empty()) {
		TimeInterrupt ti = timer.top();
		currentTime = ti.time;
		if(ti.type == 1) {
			cout<<"sim "<<currentTime<<": Generate new transaction"<<endl;
			timer.pop();
			generateTransaction();
		} else if(ti.type == 2) {
			cout<<"sim "<<currentTime<<": Transaction transfer from "<<ti.from<<" to "<<ti.to<<endl;
			timer.pop();
		}
	}
}

float getSpeedOfLightDelay(int i, int j) {
	return 5.6;
}

int main(int argc, char* argv[]) {
    srand (time(NULL));
	cout << "Yo C++" << endl;
    cout << "exp test" << exponential(0.01) << endl;
	if(argc > 1) {
		n = atoi(argv[1]);
	} else {
		n = 50;
	}
	cout << "No. of nodes: " << n << endl;

	//Setup connection between nodes
	isConnected = new bool*[n];
	speedOfLightDelay = new float*[n];
	for(int i=0;i<=n;i++) {
		isConnected[i] = new bool[n];
		speedOfLightDelay[i] = new float[n];
	}
	for(int i=0;i<n;i++) {
		for(int j=i+1;j<n;j++) {
			isConnected[i][j] = areNodesConnected(i, j);	//Helper function that uses underlying dist
			isConnected[j][i] = isConnected[i][j];			//Symmetric matrix
			if(isConnected[i][j] == 1) {
				speedOfLightDelay[i][j] = getSpeedOfLightDelay(i, j);
				speedOfLightDelay[j][i] = speedOfLightDelay[i][j];
			}
		}
	}

	//Initialize nodes
	nodes = new Node[n];
	for(int i=0;i<n;i++) {
		nodes[i].isNodeSlow = isNodeSlow(i);
		nodes[i].balance = getNodeInitBalance(i);			//Initial money balance of the node
	}

	latestTransactionID = 0;			//Init transactionID to 0

	addFirstInterrupt();				//Add the first interrupt, which is transaction generation
	startSimulation();					//Start simulation loop

	//nodes[0].receiveNewTransaction(generateTransaction());
	return 0;
}
