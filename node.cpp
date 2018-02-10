#include <iostream>
#include <cstdio>
#include <vector>
#include <set>
#include <algorithm>

using namespace std;

class Node {
public:
    int id;
    bool isNodeSlow;
    float balance;
    float blkGenerateMeanTime;
    vector<Transaction *> unspentTransactions;
    set<int> seenTransactions;
    set<int> seenBlocks;

    //Check in own longest block chain if transaction is valid
    //TODO
    bool validateTransaction(Transaction *t) {
        return true;
    }

    void generateNewTransaction(Transaction *t) {
        this->balance -= t->value;
        cout<<"node "<<id<<": New balance is "<<balance<<endl;
        unspentTransactions.push_back(t);
    }

    bool receiveNewTransaction(Transaction *t) {
        if(validateTransaction(t)) {
            unspentTransactions.push_back(t);
            return true;
        }
        return false;
    }

    bool hasSeenTransaction(int tid) {
        if (seenTransactions.find(tid) != seenTransactions.end())
            return true;
        seenTransactions.insert(tid);
        return false;
    }

    bool hasSeenBlock(int bid) {
        if (seenBlocks.find(bid) != seenBlocks.end())
            return true;
        seenBlocks.insert(bid);
        return false;
    }
};
