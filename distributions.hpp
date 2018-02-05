#include <cmath>
using namespace std;

double iRand(int fMin, int fMax) {
    return fMin + rand() % (fMax - fMin);
}

double fRand(double fMin, double fMax) {
    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

double exponential(double lambda) {
    double randNum = fRand(0,1);
    return - log(1 - randNum) / lambda;
}

double rhoLatency() {
    fRand(10,500);
}

double dLatency(double cLatency) {
    exponential(cLatency / 96);
}
