#include<iostream>
#include<memory>
#include<string>
#include<vector>

#include"Vtop.h"
#include"verilated.h"
#include"verilated_vcd_c.h"
#include "verilated_cov.h"

#include"ibex_memory_driver.h"
#include"ibex_golden_model.h"
#include"ibex_output_monitor.h"
#include"ibex_scoreboard.h"
#include"ibex_functional_coverage.h"
#include"ibex_tb_component.h"

int main(int argc, char** argv){
    //=======================================
    //          DECLARATION STAGE
    //=======================================
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

    std::vector<TbComponent*> tb_env;
    tb_env.push_back(driver);
    tb_env.push_back(monitor);
    tb_env.push_back(scb);

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

    int tb_cooldown = 0; // [FIX]: Add the multi-cycle blindfold variable

    //=========================================
    //           SYSTEM RESET STAGE
    //=========================================
    dut->rst_ni = 0;
    dut->clk_i = 0;
    dut->pc_set_i = 1;
    dut->eval();

    // Reset ALL testbench components instantly via Polymorphism
    for (auto comp : tb_env) {
        comp->reset();
    }

    contextp->timeInc(20);
    tfp->dump(contextp->time());
    dut->rst_ni = 1;
    dut->eval();
    
    tb_cooldown = 2; // [FIX]: Blind the TB for the first 2 clock cycles after initial boot!

    //=========================================
    //          MAIN SIMULATION LOOP
    //=========================================
    while (!contextp->gotFinish() && contextp->time() < 10000) {
    
        // Variables used to enable and disable the mid-flight reset
        static bool in_flight_reset_active = false;
        static bool in_flight_reset_done = false;

        // 1. Falling Edge Phase
        dut->clk_i = 0;
        dut->eval();

        contextp->timeInc(5); // Advance time 5 units
        tfp->dump(contextp->time());

        // 2. Rising Edge Phase (Capture & Drive)
        dut->clk_i = 1;
        dut->eval(); // DUT processes the rising edge

        // [FIX]: Safely drop the initial boot signal after the clock edge
        if (contextp->time() >= 30 && dut->pc_set_i == 1 && !in_flight_reset_active) {
            dut->pc_set_i = 0;
            dut->eval();
        }

        // =======================================================
        // THE SUDDEN MID-FLIGHT RESET TRIGGER
        // =======================================================
        if (contextp->time() >= 5000 && !in_flight_reset_active && !in_flight_reset_done) {
            std::cout << "\n[TESTBENCH] *** IN-FLIGHT SYSTEM RESET INITIATED ***\n";
            
            in_flight_reset_active = true;

            dut->rst_ni = 0;
            dut->eval();
                
            for (auto comp : tb_env) {
                comp->reset();
            }
        }
        
        // Hold the reset for 5 clock cycles
        if (contextp->time() >= 5050 && in_flight_reset_active) {
            std::cout << "[TESTBENCH] *** SYSTEM WAKEUP ***\n\n";

            in_flight_reset_active = false;
            in_flight_reset_done = true;

            dut->rst_ni = 1;
            dut->pc_set_i = 1; 
            dut->eval();
            
            tb_cooldown = 2; // [FIX]: Blind the TB for 2 clock cycles after mid-flight wakeup!
        }

        // Drop the mid-flight boot signal
        if (contextp->time() >= 5060 && dut->pc_set_i == 1) {
            dut->pc_set_i = 0;
            dut->eval();
        }

        // 3. Testbench Execution (With Cooldown Protection)
        if (tb_cooldown > 0) {
            tb_cooldown--; // Hardware is flushing; keep C++ eyes closed!
        } else {
            for (auto comp : tb_env) {
                comp->run(); // Bus is stable; safe to execute!
            }
        }
        
        // Evaluate again in case the driver changed any inputs
        dut->eval(); 

        contextp->timeInc(5); // Advance time 5 units
        tfp->dump(contextp->time());
    }

    //================================================
    //          CLEANUP AND REPORTING STAGE
    //================================================
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
