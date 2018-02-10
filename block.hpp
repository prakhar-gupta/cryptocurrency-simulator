#include <list>
#include <set>

using namespace std;

struct Block {
    int id;
    list<Transaction> transList;
    Block *prevBlk;
    int generationTime;
    vector<int> balances;
    set<int> transactions;
    int len;
};
