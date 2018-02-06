#include <iostream>
#include <cstdio>
#include <vector>
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
	vector<Transaction> unspentTransactions;

	//Check in own longest block chain if transaction is valid
	//TODO
	bool validateTransaction(Transaction t) {
		return true;
	}

	void generateNewTransaction(Transaction t) {
		this->balance -= t.value;
		cout<<id<<": New balance is "<<balance<<endl;
		unspentTransactions.push_back(t);
	}

	bool receiveNewTransaction(Transaction t) {
		if(validateTransaction(t)) {
			unspentTransactions.push_back(t);
			return true;
		}
		return false;
	}
};