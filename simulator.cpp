#include <iostream>
#include <cstdio>
#include <vector>
#include <algorithm>
#include "distributions.hpp"

using namespace std;

int n;				//Number of peers
float z;			//Percent of slow nodes [0-100]
float t_mean;		//Mean time of exp-dist of transaction gen
float q_mean;		//Mean time of exp-dist of queue delay 
					//for message transmission on a path
float b_mean;		//Mean time of exp-dist of block mine time
const int MINING_FEES = 50;		//Mining fees to generate block


int main(int argc, char* argv[]) {
    srand (time(NULL));
	cout<<"Yo C++"<<endl;
    cout << exponential(0.01);
	return 0;
}
