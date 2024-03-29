#include <list>
#include <set>

using namespace std;

struct Block {
public:
    int id;
    int creator;
    list<Transaction *> transList;
    Block *prevBlk;
    int generationTime;
    vector<float> balances;
    set<int> transactions;
    int len;
    list<Block *> child;
};
