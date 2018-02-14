#include <algorithm>
#include <cstdio>
#include <iostream>
#include <list>
#include <queue>
#include <vector>
#include "transaction.hpp"
#include "block.hpp"
#include "distributions.hpp"
#include "node.cpp"

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
int lastBlkId;
float currentTime;              //Current simulation time

Block *genesisBlk;


struct TimeInterrupt {
    float time;
    int type;           //1 -> Generate transaction
                        //2 -> Transfer transaction/block message
                        //3 -> recv / generate block
    int from, to;       //Sender and receipient in case of message passing
    Transaction *t;
    Block *block;
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
    return false;
    //return (i == j + 1) || (j == i + 1) || (i-j == n-1) || (j-i == n-1);
}

//The initial balance that node i has
//TODO
float getNodeInitBalance(int i) {
    return INIT_BALANCE;
}

// TODO
float getMeanGenerationTime(int i) {
    return bMean;
}

void connectNode(int i) {
    for (int j=0; j<i; j++)
        isConnected[i][j] = isConnected[j][i] = 0;

    int count = iRand(0, n/2.5);//exponential(10.0/n);
    count = max(1, min(i, count));
    while (count) {
        int j = iRand(0,i);
        if (isConnected[i][j])
            continue;
        isConnected[i][j] = isConnected[j][i] = 1;
        speedOfLightDelay[i][j] = speedOfLightDelay[j][i] = rhoLatency();
        count--;
    }
}

void setupConnections() {
    isConnected = new bool*[n];
    speedOfLightDelay = new float*[n];
    for(int i=0;i<=n;i++) {
        isConnected[i] = new bool[n];
        speedOfLightDelay[i] = new float[n];
    }
    for(int i=1;i<n;i++) {
        connectNode(i);
        /*
        for(int j=i+1;j<n;j++) {
            isConnected[i][j] = isConnected[j][i] = areNodesConnected(i, j);    //Helper function that uses underlying dist
            if(isConnected[i][j] == 1) {
                speedOfLightDelay[i][j] = speedOfLightDelay[j][i] = rhoLatency();
            }
        }
        */
    }
}

void markSlowNodes() {
    cout << "Slow Nodes:" << endl;
    int count = (int) round(z * n / 100);
    for (int i=0; i < count; i++) {
        int val;
        do {
            val = iRand(0,n);
        } while (nodes[val].isNodeSlow);
        nodes[val].isNodeSlow = true;
        cout << val << endl;
    }
    cout << endl;
}

void initNodes() {
    nodes = new Node[n];
    // nodes = (Node *)malloc(n * sizeof(Node));
    for(int i=0;i<n;i++) {
        nodes[i].id = i;
        nodes[i].isNodeSlow = false;
        nodes[i].blkGenerateMeanTime = getMeanGenerationTime(i);
    }
    markSlowNodes();
}

void timerBlkGenerate(int i) {
    TimeInterrupt ti;
    ti.time = currentTime + exponential(nodes[i].blkGenerateMeanTime);
    ti.type = 3;
    ti.to = i;
    ti.from = -1;
    timer.push(ti);
}

void createGenesis() {
    lastBlkId = 0;
    // Block *genesisBlk = (Block *)malloc(sizeof(Block));
    genesisBlk = new Block();
    genesisBlk->id = lastBlkId ++;
    for (int i=0;i<n;i++) {
        genesisBlk->balances.push_back(getNodeInitBalance(i));
        timerBlkGenerate(i);
        nodes[i].longestChainLeaf = genesisBlk;
    }
    genesisBlk->generationTime = 0;
    genesisBlk->prevBlk = NULL;
    genesisBlk->len = 1;
}

void init() {
    setupConnections();
    initNodes();
    latestTransactionID = 0;            //Init transactionID to 0
    createGenesis();
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
    if (nodes[id].hasSeenTransaction(t))
        return;
    for(int i=0;i<n;i++) {
        if(i == recvdFrom || i == id)
            continue;
        if(isConnected[id][i]) {
            TimeInterrupt ti;
            float delay = getTransmissionDelay(0, id, i);
            ti.time = currentTime + delay;
            ti.type = 2;
            ti.t = t;
            ti.from = id;
            ti.to = i;
            timer.push(ti);
        }
    }
}

void forwardBlock(int id, int recvdFrom, Block *blk) {
    if (nodes[id].hasSeenBlock(blk->id, currentTime))
        return;
    for(int i=0;i<n;i++) {
        if(i == recvdFrom || i == id)
            continue;
        if(isConnected[id][i]) {
            TimeInterrupt ti;
            ti.time = currentTime + getTransmissionDelay(1000, id, i);
            ti.type = 3;
            ti.block = blk;
            ti.from = id;
            ti.to = i;
            timer.push(ti);
        }
    }
}

void newBlockHandle(int to, int from, Block *blk) {
    timerBlkGenerate(to);
    Block *tmpBlock = blk;
    if (from == -1) {
        // create new block
        tmpBlock = new Block();
        tmpBlock->id = lastBlkId ++;
        tmpBlock->generationTime = currentTime;
        nodes[to].updateBlock(tmpBlock);
        if (tmpBlock->id == -1) {
            lastBlkId --;
            cout << "No Transactions to add; Not creating block" << endl;
            return;
        }
        cout << "Block Creation Successful" << endl;
    }
    /*
    if (nodes[to].longestChainLeaf->len == 1 || nodes[to].hasSeenBlockNotSee(nodes[to].longestChainLeaf->prevBlk->id)) {
        if (nodes[to].longestChainLeaf->len < tmpBlock->len)
            nodes[to].longestChainLeaf = tmpBlock;
        list<Block *>::iterator it = nodes[to].longestChainLeaf->child.begin();
        while (it != nodes[to].longestChainLeaf->child.end()) {
            cout << nodes[to]
            if (nodes[to].longestChainLeaf->len < (*it)->len)
                nodes[to].longestChainLeaf = *it;
            it ++;
        }
    }
    */
    if (nodes[to].hasSeenBlockNotSee(tmpBlock->prevBlk->id)) {
        if (nodes[to].longestChainLeaf->len < tmpBlock->len)
            nodes[to].longestChainLeaf = tmpBlock;
        list<Block *>::iterator it = tmpBlock->child.begin();
        while (it != tmpBlock->child.end()) {
            if (nodes[to].longestChainLeaf->len < (*it)->len)
                nodes[to].longestChainLeaf = *it;
            it ++;
        }
    }
    forwardBlock(to, from, tmpBlock);
}

void generateTransaction(int from) {
    // Transaction *t = (Transaction *) malloc(sizeof(Transaction));
    Transaction *t = new Transaction();
    t->id = ++latestTransactionID;
    t->from = from;
    do {
        t->to = iRand(0,n);
    } while (t->to == from);
    t->value = fRand(0, 0.1 * nodes[from].longestChainLeaf->balances[from]);

    cout<<"Generating txn"<< t->id << " from "<<t->from<<" to "<<t->to<<" for "<<t->value<< " " << nodes[from].longestChainLeaf->balances[from] << endl;

    //Generate TimerInterrupt for message transfer
    forwardTransaction(from, -1, t);
}

void reinitGenerate(int id, int currTime) {
    TimeInterrupt ti;
    ti.time = currTime + exponential(tMean);
    ti.type = 1;
    ti.from = id;
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
        if (currentTime > 100000) {
            break;
        }
        int from, to;
        Transaction *t;
        Block *blk;
        switch(ti->type) {
            case 1:
                from = ti->from;
                cout<<"sim "<<currentTime<<": Generate new transaction"<< " " << from << endl;
                timer.pop();
                generateTransaction(from);
                reinitGenerate(from, currentTime); 
                break;
            case 2:
                from = ti->from;
                to = ti->to;
                t = ti->t;
                cout<<"sim "<<currentTime<<": Transaction" << t->id << "  transfer from "<<from<<" to "<<to<<endl;
                timer.pop();
                forwardTransaction(to, from, t);
                break;
            case 3:
                from = ti->from;
                to = ti->to;
                blk = ti->block;
                if (from == -1)
                    cout<<"sim "<<currentTime<<": Block create at "<<to<<endl;
                else
                    cout<<"sim "<<currentTime<<": Block" << blk->id << "  transfer from "<<from<<" to "<<to<<endl;
                timer.pop();
                newBlockHandle(to, from, blk);
                break;
        }
    }
}

void printBlocks() {
    for (int i=0; i<n; i++) {
        nodes[i].print(genesisBlk);
    }
}

int main(int argc, char* argv[]) {
    srand (time(NULL));
    n = 5;
    z = 0;
    tMean = 0.0001;
    bMean = 0.00001;
    if(argc > 1)
        n = atoi(argv[1]);
    if(argc > 2)
        z = atof(argv[2]);
    if(argc > 3)
        tMean = 1.0 / atof(argv[3]);
    if(argc > 4)
        bMean = 1.0 / atof(argv[4]);
    cout << "No. of nodes: " << n << endl;

    init();
    startSimulation();              //Start simulation loop
    cerr << "sim done" << endl;
    printBlocks();
    return 0;
}
