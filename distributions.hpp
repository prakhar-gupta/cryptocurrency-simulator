#include <cmath>
using namespace std;

double fRand(double fMin, double fMax) {
    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

double exponential(double lambda) {
    double randNum = fRand(0,1);
    return - log(1 - randNum) / lambda;
}
