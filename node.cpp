#include <algorithm>
#include <cstdio>
#include <fstream>
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
    ofstream outfile;

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

    bool hasSeenBlockNotSee(int bid) {
        return seenBlocks.find(bid) != seenBlocks.end();
    }

    bool hasSeenBlock(int bid, float currTime) {
        if (seenBlocks.find(bid) != seenBlocks.end())
            return true;
        seenBlocks[bid] = currTime;
        return false;
    }

    void updateBlock(Block *tmpBlk) {
        tmpBlk->creator = id;
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

        longestChainLeaf->child.push_back(tmpBlk);
        longestChainLeaf = tmpBlk;
        for (int i=0;i<5;i++) {
            cout << "_" << tmpBlk->balances[i];
        }
        cout << endl;
    }

    void traverseTree(Block *blk, string prev, bool remPipe) {
        string prev2 = prev;
        replace(prev2.begin(), prev2.end(), '-', ' ');
        outfile << prev2 << endl;
        outfile << prev << blk->id << " (" << blk->creator << ", " << seenBlocks[blk->id] << ")";
        list<Transaction *>::iterator it1 = blk->transList.begin();
        while (it1 != blk->transList.end()) {
            outfile << " #" << (*it1)->id;
            it1++;
        }
        outfile << endl;
        if (remPipe) {
            prev = prev.substr(0, prev.size() - 5) + "     ";
        }
        else {
            replace(prev.begin(), prev.end(), '-', ' ');
        }
        string prev1 = prev + "|----";
        if (blk->child.size() == 0)
            outfile << prev << endl;
        list<Block *>::iterator it = blk->child.begin(), it2;
        while (it != blk->child.end()) {
            it2 = it;
            it ++;
            if (id > 0 && !hasSeenBlockNotSee((*it2)->id))
                continue;
            if (it == blk->child.end())
                traverseTree(*it2, prev1, true);
            else
                traverseTree(*it2, prev1, false);
        }
    }

    void print(Block *genesis) {
        char filename[] = "out00.txt";
        filename[3] = 48 + id/10;
        filename[4] = 48 + id%10;
        outfile.open(filename);
        traverseTree(genesis, "", false);
        outfile.close();
    }
};
