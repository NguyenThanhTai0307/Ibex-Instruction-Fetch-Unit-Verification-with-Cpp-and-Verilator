#ifndef OUTPUT_MONITOR_H
#define OUTPUT_MONITOR_H
#include <queue>
#include "Vtop.h"

// Forward declaration informs the compiler that Scoreboard exists
class Scoreboard;
class FunctionalCoverage; 

class OutputMonitor {
private:
    Vtop* dut;
    Scoreboard* scb;
    FunctionalCoverage* fcov;

public:
    OutputMonitor(Vtop* _dut, Scoreboard* _scb, FunctionalCoverage* _fcov);
    void run();
};
#endif // OUTPUT_MONITOR_H