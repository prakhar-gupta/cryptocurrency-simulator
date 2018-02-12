#include <algorithm>
#include <cstdio>
#include <iostream>
#include <list>
#include <map>
#include <set>
#include <vector>

using namespace std;

class Node {
public:
    int id;
    bool isNodeSlow;
    float blkGenerateMeanTime;
    list<Transaction *> unspentTransactions;
    set<int> seenTransactions;
    map<int,float> seenBlocks;
    Block *longestChainLeaf;

    //Check in own longest block chain if transaction is valid
    //TODO
    bool validateTransaction(Transaction *t) {
        return longestChainLeaf->balances[t->from] >= t->value;
    }

//    void generateNewTransaction(Transaction *t) {
//        this->balance -= t->value;
//        cout<<"node "<<id<<": New balance is "<<balance<<endl;
//        unspentTransactions.push_back(t);
//    }

    bool receiveNewTransaction(Transaction *t) {
        if(validateTransaction(t)) {
            unspentTransactions.push_back(t);
            return true;
        }
        return false;
    }

    bool hasSeenTransaction(Transaction *t) {
        int tid = t->id;
        if (seenTransactions.find(tid) != seenTransactions.end())
            return true;
        seenTransactions.insert(tid);
        receiveNewTransaction(t);
        return false;
    }

    bool hasSeenBlock(int bid, float currTime) {
        if (seenBlocks.find(bid) != seenBlocks.end())
            return true;
        seenBlocks[bid] = currTime;
        return false;
    }

    void updateBlock(Block *tmpBlk) {
        tmpBlk->prevBlk = longestChainLeaf;
        tmpBlk->transactions = longestChainLeaf->transactions;
        tmpBlk->balances = longestChainLeaf->balances;
        while (!unspentTransactions.empty()) {
            Transaction *tmpTrans = unspentTransactions.front();
            if (tmpBlk->balances[tmpTrans->from] >= tmpTrans->value && tmpBlk->transactions.find(tmpTrans->id) == tmpBlk->transactions.end()) {
                tmpBlk->transList.push_back(tmpTrans);
                tmpBlk->balances[tmpTrans->from] -= tmpTrans->value;
                tmpBlk->balances[tmpTrans->to] += tmpTrans->value;
                tmpBlk->transactions.insert(tmpTrans->id);
                cout << "#" << tmpTrans->id;
            }
            unspentTransactions.pop_front();
        }
        if (tmpBlk->transList.size() == 0) {
            tmpBlk->id = -1;
            return;
        }
        tmpBlk->balances[id] += 50;
        tmpBlk->len = longestChainLeaf->len + 1;

        longestChainLeaf = tmpBlk;
        longestChainLeaf->child.push_back(tmpBlk);
        for (int i=0;i<5;i++) {
            cout << "_" << tmpBlk->balances[i];
        }
        cout << endl;
    }
};
