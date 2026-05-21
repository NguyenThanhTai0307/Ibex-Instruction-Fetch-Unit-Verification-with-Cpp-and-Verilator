#ifndef SCOREBOARD_CPP
#define SCOREBOARD_CPP

#include "ibex_scoreboard.h"
#include <iostream>
#include <iomanip>

Scoreboard::Scoreboard(IbexGoldenModel* _gm) : gm(_gm) {}

void Scoreboard::write(const OutputItem& tr) {
    ifu_out_fifo.push(tr); 
}
void Scoreboard::reset() {
    while (!ifu_out_fifo.empty())
        ifu_out_fifo.pop();
}

void Scoreboard::run() {
    uint32_t expected_opcode;
    
    while (!ifu_out_fifo.empty()) {
        OutputItem tr = ifu_out_fifo.front();
        ifu_out_fifo.pop();

        expected_opcode = gm->get_golden_instr(tr.pc);

        if (tr.is_compressed) {
            if (tr.c_fetched_instr != expected_opcode) {
                std::cerr << "[SCB_FAIL] Mismatch at PC: 0x" << std::hex << tr.pc 
                          << " | Expected: 0x" << expected_opcode 
                          << " | IFU Output: 0x" << tr.c_fetched_instr << std::endl;
                error_count++;
            } else {
                match_count++;
            }
        } else {
            if (tr.fetched_instr != expected_opcode) {
                std::cerr << "[SCB_FAIL] Mismatch at PC: 0x" << std::hex << tr.pc 
                          << " | Expected: 0x" << expected_opcode 
                          << " | IFU Output: 0x" << tr.fetched_instr << std::endl;
                error_count++;
            } else {
                match_count++;
            }
        }
    }
}

// Implementation of the UVM report_phase summary
void Scoreboard::print_summary() {
    std::cout << "\n======================================================\n";
    std::cout << "                 SCOREBOARD SUMMARY                   \n";
    std::cout << "======================================================\n";
    std::cout << " Total Matches: " << std::dec << match_count << "\n";
    std::cout << " Total Errors:  " << std::dec << error_count << "\n";
    if (error_count == 0 && match_count > 0) {
        std::cout << " RESULT: PASS \n";
    } else {
        std::cout << " RESULT: FAIL \n";
    }
    std::cout << "======================================================\n";
}

#endif // SCOREBOARD_CPP