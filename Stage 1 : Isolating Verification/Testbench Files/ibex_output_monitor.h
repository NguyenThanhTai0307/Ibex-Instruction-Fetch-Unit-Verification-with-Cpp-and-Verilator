#ifndef OUTPUT_MONITOR_H
#define OUTPUT_MONITOR_H
#include <queue>
#include "Vtop.h"
#include "ibex_tb_component.h"
#include "ibex_output_item.h"
#include "ibex_scoreboard.h"
#include "ibex_functional_coverage.h"

// Forward declaration informs the compiler that Scoreboard exists
class Scoreboard;
class FunctionalCoverage; 

class OutputMonitor : public TbComponent {
private:
    Vtop* dut;
    Scoreboard* scb;
    FunctionalCoverage* fcov;

    int gnt_delay;
    int rvalid_delay;

public:
    OutputMonitor(Vtop* _dut, Scoreboard* _scb, FunctionalCoverage* _fcov);
    void delay_sample(int gnt, int rvalid);
    void reset() override;
    void run() override;
};
#endif // OUTPUT_MONITOR_H