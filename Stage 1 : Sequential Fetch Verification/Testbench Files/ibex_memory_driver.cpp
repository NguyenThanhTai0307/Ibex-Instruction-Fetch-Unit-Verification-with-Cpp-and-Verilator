#ifndef MEMORY_DRIVER_CPP
#define MEMORY_DRIVER_CPP

#include "ibex_memory_driver.h"
#include <iostream>
#include <fstream>

MemoryDriver::MemoryDriver(Vtop* _dut, OutputMonitor* _mon, std::string& vmem_path, uint32_t seed) : 
    dut(_dut),
    mon(_mon),
    gnt_pending(false),
    data_pending(false),
    gnt_delay(0),
    rvalid_delay(0),
    data_delay(0),
    random(seed), 
    delay_range(0, 5) 
{
    load_vmem(vmem_path);
}

void MemoryDriver::load_vmem(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "[DRIVER] ERROR: Could not open VMEM file: " << filepath << std::endl;
        return;
    }

    std::string token;
    uint32_t current_byte_addr = 0;
    
    while (file >> token) {
        if (token[0] == '@') { 
            current_byte_addr = std::stoul(token.substr(1), nullptr, 16);
        } else { 
            uint32_t byte_val = std::stoul(token, nullptr, 16);
            uint32_t word_addr = current_byte_addr >> 2;
            uint32_t byte_offset = current_byte_addr & 0x3;
            
            if (sram.find(word_addr) == sram.end()) {
                sram[word_addr] = 0;
            }
            sram[word_addr] |= (byte_val << (byte_offset * 8));
            current_byte_addr++; 
        }
    }
    std::cout << "[DRIVER] Successfully loaded VMEM. Total words: " << sram.size() << std::endl;
}

void MemoryDriver::reset() {
    while(!txn_queue.empty()) txn_queue.pop();
    gnt_pending = false;
    data_pending = false;
    dut->instr_gnt_i = 0;
    dut->instr_rvalid_i = 0;
}

void MemoryDriver::run() {
    dut->instr_gnt_i = 0;
    dut->instr_rvalid_i = 0;

    // Track if we deliver data this cycle
    bool rvalid_fired_this_cycle = false; 

    // ==========================================================
    // THREAD A: Data Phase (Response Logic & SRAM Lookup)
    // ==========================================================
    if (!data_pending && !txn_queue.empty()) {
        current_txn = txn_queue.front();
        txn_queue.pop();
        data_delay = current_txn.rvalid_delay;
        data_pending = true;
    }

    if (data_pending) {
        if (data_delay > 0) {
            data_delay--;
        } 
        else {
            dut->instr_rvalid_i = 1; 
            rvalid_fired_this_cycle = true; // Mark the delivery

            uint32_t target_word = current_txn.addr >> 2;
            
            if (sram.find(target_word) != sram.end()) {
                dut->instr_rdata_i = sram[target_word];
            } 
            else {
                dut->instr_rdata_i = 0x00000013; // Default NOP
            }
            
            data_pending = false;
        }
    }

    // ==========================================================
    // THREAD B: Address Phase (Grant Logic & Sequence Mapping)
    // ==========================================================
    if (gnt_pending) {
        if (dut->instr_req_o == 0) {
            gnt_pending = false;
            gnt_delay = 0;
        }
        else if (gnt_delay > 0) {
            gnt_delay--;
        } 
        else if (gnt_delay == 0) { 
            // Mutual Exclusion - Wait 1 cycle if rvalid fired
            if (!rvalid_fired_this_cycle) {
                dut->instr_gnt_i = 1;
                txn_queue.push({dut->instr_addr_o, rvalid_delay});
                gnt_pending = false;
            }
        }
    }
    else {
        if (dut->instr_req_o == 1) {
            uint32_t phys_addr = dut->instr_addr_o;

            if (phys_addr >= 0x80001000 && phys_addr <= 0x80001FFF)
                gnt_delay = 0;
            else
                gnt_delay = delay_range(random);

            rvalid_delay = delay_range(random);
            gnt_pending = true;

            if (gnt_delay == 0) {
                // Mutual Exclusion - Wait 1 cycle if rvalid fired
                if (!rvalid_fired_this_cycle) {
                    dut->instr_gnt_i = 1;
                    txn_queue.push({phys_addr, rvalid_delay});
                    gnt_pending = false;
                }
            }
            else {
                gnt_delay--;
            }

            mon->delay_sample(gnt_delay, rvalid_delay);
        }
    }
} 

#endif // MEMORY_DRIVER_CPP