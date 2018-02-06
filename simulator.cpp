#include <iostream>
#include <cstdio>
#include <vector>
#include <algorithm>
#include <queue>
#include "node.cpp"
#include "distributions.hpp"

using namespace std;

int n;              //Number of peers
float z;            //Percent of slow nodes [0-100]
float tMean;        //Mean time of exp-dist of transaction gen
float bMean;        //Mean time of exp-dist of block mine time

const int MINING_FEES = 50;     //Mining fees to generate block
const int INIT_BALANCE = 40;    //Initial balance of nodes

bool** isConnected;             //2D matrix signifying which nodes are connected to each other. indexing from 0 to n-1.
float** speedOfLightDelay;
Node* nodes;                    //Array of all nodes
int latestTransactionID;        //ID of the latest generated transaction
float currentTime;              //Current simulation time

struct TimeInterrupt {
    float time;
    int type;           //1 -> Generate transaction
                        //2 -> Transfer transaction/block message
                        //3 -> generate block
    int from, to;       //Sender and receipient in case of message passing
    Transaction *t;
    int blockGeneratorNodeID;
};

struct InterruptCompare
{
    bool operator()(const TimeInterrupt& lhs, const TimeInterrupt& rhs)
    {
        return lhs.time > rhs.time;
    }
};

priority_queue <TimeInterrupt, vector<TimeInterrupt>, InterruptCompare> timer;      //Priority queue for all events

//Are nodes i and j connected to each other
//TODO
bool areNodesConnected(int i, int j) {
    return (i == j + 1) || (j == i + 1) || (i-j == n-1) || (j-i == n-1);
}

//The initial balance that node i has
//TODO
float getNodeInitBalance(int i) {
    return INIT_BALANCE;
}

void setupConnections() {
    isConnected = new bool*[n];
    speedOfLightDelay = new float*[n];
    for(int i=0;i<=n;i++) {
        isConnected[i] = new bool[n];
        speedOfLightDelay[i] = new float[n];
    }
    for(int i=0;i<n;i++) {
        for(int j=i+1;j<n;j++) {
            isConnected[i][j] = isConnected[j][i] = areNodesConnected(i, j);    //Helper function that uses underlying dist
            if(isConnected[i][j] == 1) {
                speedOfLightDelay[i][j] = speedOfLightDelay[j][i] = rhoLatency();
            }
        }
    }
}

void markSlowNodes() {
    int count = (int) round(z * n / 100);
    for (int i=0; i < n; i++) {
        int val;
        do {
            val = iRand(0,n);
        } while (nodes[val].isNodeSlow);
        nodes[val].isNodeSlow = true;
    }
}

void initNodes() {
    nodes = new Node[n];
    for(int i=0;i<n;i++) {
        nodes[i].id = i;
        nodes[i].isNodeSlow = false;
        nodes[i].balance = getNodeInitBalance(i);           //Initial money balance of the node
    }
    markSlowNodes();
}

void init() {
    setupConnections();
    initNodes();
    latestTransactionID = 0;            //Init transactionID to 0
}

float getTransmissionDelay(int m, int i, int j) {
    float p = speedOfLightDelay[i][j];
    float c = 100.0;
    if(nodes[i].isNodeSlow || nodes[j].isNodeSlow) {
        c = 5.0;
    }
    float d = dLatency(c);
    return p + m/c + d;
}

void forwardTransaction(int id, int recvdFrom, Transaction *t) {
    if (nodes[id].hasSeen(t->id))
        return;
    for(int i=0;i<n;i++) {
        if(i == recvdFrom || i == id)
            continue;
        if(isConnected[id][i]) {
            TimeInterrupt ti;
            float delay = getTransmissionDelay(0.0, id, i);
            ti.time = currentTime + delay;
            ti.type = 2;
            ti.t = t;
            ti.from = id;
            ti.to = i;
            timer.push(ti);
        }
    }
}

void generateTransaction(int from) {
    Transaction *t = (Transaction *) malloc(sizeof(Transaction));
    t->id = ++latestTransactionID;
    t->from = from;
    do {
        t->to = iRand(0,n);
    } while (t->to == from);
    t->value = fRand(0, nodes[from].balance);

    cout<<"Generating txn"<< t->id << " from "<<t->from<<" to "<<t->to<<" for "<<t->value<<endl;
    nodes[t->from].generateNewTransaction(t);

    //Generate TimerInterrupt for message transfer
    forwardTransaction(from, -1, t);
}

void reinitGenerate(int id, int currTime) {
        TimeInterrupt ti;
        ti.time = currTime + exponential(tMean);
        ti.type = 1;
        ti.from = id;
        cout << ti.time << " " << id << " " << ti.from << endl;
        timer.push(ti);
}

void initInterrupt() {
    for (int i = 0; i < n; i++) {
        reinitGenerate(i, 0);
    }
}

void startSimulation() {
    initInterrupt();            //Add the first interrupt, which is transaction generation
    while(!timer.empty()) {
        const TimeInterrupt *ti = &(timer.top());
        currentTime = ti->time;
        int from, to;
        Transaction *t;
        switch(ti->type) {
            case 1:
                from = ti->from;
                cout<<"sim "<<currentTime<<": Generate new transaction"<< " " << from << endl;
                timer.pop();
                generateTransaction(from);
                // TODO: uncomment for continous generation
                // reinitGenerate(from, currentTime); 
                break;
            case 2:
                from = ti->from;
                to = ti->to;
                t = ti->t;
                cout<<"sim "<<currentTime<<": Transaction" << t->id << "  transfer from "<<from<<" to "<<to<<endl;
                timer.pop();
                forwardTransaction(to, from, t);
                break;
        }
    }
}

int main(int argc, char* argv[]) {
    srand (time(NULL));
    cout << "Yo C++" << endl;
    cout << "exp test" << exponential(0.01) << endl;
    tMean = 0.1;
    if(argc > 1) {
        n = atoi(argv[1]);
    } else {
        n = 5;
    }
    cout << "No. of nodes: " << n << endl;

    init();
    startSimulation();              //Start simulation loop

    //nodes[0].receiveNewTransaction(generateTransaction());
    return 0;
}
