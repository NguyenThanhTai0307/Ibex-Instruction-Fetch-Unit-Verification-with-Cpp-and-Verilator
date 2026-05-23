#ifndef OUTPUT_MONITOR_CPP
#define OUTPUT_MONITOR_CPP

#include "ibex_output_monitor.h"
#include <iostream>

OutputMonitor::OutputMonitor(Vtop* _dut, Scoreboard* _scb, FunctionalCoverage* _fcov) :
    dut(_dut),
    scb(_scb),
    fcov(_fcov) {}

void OutputMonitor::delay_sample(int gnt, int rvalid){
    gnt_delay = gnt;
    rvalid_delay = rvalid;
}

// Fulfills the TbComponent polymorphic contract
void OutputMonitor::reset() {}

void OutputMonitor::run() {
    if(dut->instr_new_o == 1) {
        OutputItem tr;
        
        tr.pc = dut->pc_o;
        tr.is_compressed = dut->instr_is_compressed_o;
        tr.is_misaligned = (tr.pc % 4 != 0);

        if(tr.is_compressed) {
            tr.c_fetched_instr = dut->c_instr_rdata_o;
            std::cout << "[IF-ID MON] COMPRESSED   - PC: 0x" << std::hex << tr.pc 
                      << " | INSTR: 0x" << tr.c_fetched_instr << std::endl;
        }
        else {
            tr.fetched_instr = dut->instr_rdata_o;
            std::cout << "[IF-ID MON] UNCOMPRESSED - PC: 0x" << std::hex << tr.pc 
                      << " | INSTR: 0x" << tr.fetched_instr << std::endl;
        }

        // Broadcast the transaction to both components
        scb->write(tr);
        fcov->sample_coverage(tr.is_compressed, tr.is_misaligned, gnt_delay, rvalid_delay);
    }
}   
#endif // OUTPUT_MONITOR_CPP