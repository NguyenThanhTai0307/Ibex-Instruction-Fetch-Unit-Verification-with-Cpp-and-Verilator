#include<iostream>
#include<memory>
#include<string>

#include"Vtop.h"
#include"verilated.h"
#include"verilated_vcd_c.h"
#include "verilated_cov.h"

#include"ibex_memory_driver.h"
#include"ibex_golden_model.h"
#include"ibex_output_monitor.h"
#include"ibex_scoreboard.h"
#include"ibex_functional_coverage.h"

int main(int argc, char** argv){
    std::unique_ptr<VerilatedContext> contextp {new VerilatedContext};

    Vtop* dut = new Vtop{contextp.get()};

    std::string vmem_path = "/home/thanhtai/Documents/Verilog_Projects/Insruction_Fetch_Unit_Verilator_C++/hello_test.vmem";

    if (argc >= 2) {
        vmem_path = argv[1];
    }

    MemoryDriver* driver = new MemoryDriver(dut, vmem_path);
    IbexGoldenModel* gm = new IbexGoldenModel(vmem_path); 
    Scoreboard* scb = new Scoreboard(gm);
    FunctionalCoverage* fcov = new FunctionalCoverage();
    OutputMonitor* monitor = new OutputMonitor(dut, scb, fcov);

    contextp->commandArgs(argc, argv);

    Verilated::traceEverOn(true);
    VerilatedVcdC* tfp = new VerilatedVcdC;
    dut->trace(tfp, 99);
    tfp->open("waveform.vcd");

    if(contextp->randSeed() == 0)
        contextp->randSeed(time(NULL));

    std::cout << "--------------------------------------------" << std::endl;
    std::cout << "  SIMULATION STARTING WITH SEED: " << contextp->randSeed() << std::endl;
    std::cout << "--------------------------------------------" << std::endl;
    
    dut->rst_ni = 0;
    dut->clk_i = 0;
    dut->eval();
    dut->rst_ni = 1;

while (!contextp->gotFinish() && contextp->time() < 10000) {
    // 1. Falling Edge Phase
    dut->clk_i = 0;
    dut->eval();
    contextp->timeInc(5); // Advance time 5 units
    tfp->dump(contextp->time());

    // 2. Rising Edge Phase (Capture & Drive)
    dut->clk_i = 1;
    dut->eval(); // DUT processes the rising edge

    // 3. Testbench Execution (ONLY on rising edge, AFTER eval)
    monitor->run();
    driver->run();
    scb->run();
    
    // Evaluate again in case the driver changed any inputs (like instr_gnt_i)
    dut->eval(); 

    contextp->timeInc(5); // Advance time 5 units
    tfp->dump(contextp->time());
}

    scb->print_summary();
    fcov->report_coverage();
    
    tfp->close();

    Verilated::mkdir("logs");
    VerilatedCov::write("logs/coverage.dat");

    dut->final();
    delete dut;
    delete driver;
    delete monitor;
    delete fcov;
    delete scb;
    return 0;
}