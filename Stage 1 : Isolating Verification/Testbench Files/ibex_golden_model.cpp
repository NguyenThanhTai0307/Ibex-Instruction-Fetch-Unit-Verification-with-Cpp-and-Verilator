#ifndef IBEX_GOLDEN_MODEL_CPP
#define IBEX_GOLDEN_MODEL_CPP

#include "ibex_golden_model.h"
#include <iostream>
#include <fstream>
#include <sstream>

// ============================================================================
// Constructor: Parse the .vmem file natively
// ============================================================================
IbexGoldenModel::IbexGoldenModel(const std::string& vmem_path) {
    std::cout << "[GOLDEN MODEL] Bypassing Spike. Initializing Native C++ Memory..." << std::endl;
    std::cout << "[GOLDEN MODEL] Loading VMEM: " << vmem_path << std::endl;

    std::ifstream file(vmem_path);
    if (!file.is_open()) {
        std::cerr << "[GOLDEN MODEL FATAL] Could not open VMEM file!" << std::endl;
        return;
    }

    std::string line;
    uint32_t current_addr = 0;

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string token;
        while (ss >> token) {
            if (token[0] == '@') {
                current_addr = std::stoul(token.substr(1), nullptr, 16);
            } else {
                uint32_t val = std::stoul(token, nullptr, 16);
                
                if (token.length() <= 2) {
                    // Handles Byte-formatted VMEM
                    memory[current_addr] = val & 0xFF;
                    current_addr++;
                } else {
                    // Handles 32-bit Word-formatted VMEM (Little-Endian)
                    memory[current_addr]     = val & 0xFF;
                    memory[current_addr + 1] = (val >> 8) & 0xFF;
                    memory[current_addr + 2] = (val >> 16) & 0xFF;
                    memory[current_addr + 3] = (val >> 24) & 0xFF;
                    current_addr += 4; // Advance by 4 bytes!
                }
            }
        }
    }
    std::cout << "[GOLDEN MODEL] Native Memory Load Complete." << std::endl;
}

// ============================================================================
// Instruction Fetch Query
// ============================================================================
uint32_t IbexGoldenModel::get_golden_instr(uint32_t pc) {
    uint32_t target_pc = pc;

    // [FIX]: Dynamic Base Address Alignment
    // If the IFU fetches from 0x0, but the VMEM was linked at a high address (like 0x80000000),
    // dynamically shift the PC to match the start of the loaded memory map.
    if (memory.find(target_pc) == memory.end() && !memory.empty()) {
        uint32_t base_addr = memory.begin()->first;
        target_pc = target_pc + base_addr;
    }

    // Safety check: If the address STILL doesn't exist, return 0x0
    if (memory.find(target_pc) == memory.end()) {
        return 0x00000000;
    }

    // RISC-V is Little-Endian. Grab the first two bytes.
    uint16_t lower_half = (memory[target_pc + 1] << 8) | memory[target_pc];
    
    // RISC-V Compression Check: If the lower 2 bits are NOT 11 (0x3), 
    // it is a 16-bit compressed instruction.
    if ((lower_half & 0x03) != 0x03) {
        return lower_half; 
    }
    
    // Otherwise, it is a full 32-bit instruction. Grab the next two bytes.
    uint16_t upper_half = (memory[target_pc + 3] << 8) | memory[target_pc + 2];
    return (upper_half << 16) | lower_half;
}

#endif // IBEX_GOLDEN_MODEL_CPP