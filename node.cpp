#include <iostream>
#include <cstdio>
#include <vector>
#include <set>
#include <algorithm>

using namespace std;

struct Transaction {
    int id;
    int from;
    int to;
    float value;
};

class Node {
public:
    int id;
    bool isNodeSlow;
    float balance;
    vector<Transaction *> unspentTransactions;
    set<int> seenTransactions;

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

    bool hasSeen(int tid) {
        if (seenTransactions.find(tid) != seenTransactions.end())
            return true;
        seenTransactions.insert(tid);
        return false;
    }
};
