#include <iostream>
#include <cstdio>
#include <vector>
#include <algorithm>

using namespace std;

struct Transaction {
	int from;
	int to;
	float value;
};

class Node {
public:
	int id;
	bool isNodeSlow;
	float balance;
	vector<Transaction> unspentTransactions;

	//Check in own longest block chain if transaction is valid
	//TODO
	bool validateTransaction(Transaction t) {
		return true;
	}

	bool receiveNewTransaction(Transaction t) {
		if(validateTransaction(t)) {
			unspentTransactions.push_back(t);
			return true;
		}
		return false;
	}
};