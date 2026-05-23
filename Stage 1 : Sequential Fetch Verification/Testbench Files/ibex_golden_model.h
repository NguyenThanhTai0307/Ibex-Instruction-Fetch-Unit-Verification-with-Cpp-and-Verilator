#ifndef IBEX_GOLDEN_MODEL_H
#define IBEX_GOLDEN_MODEL_H

#include <stdint.h>
#include <string>
#include <map>

class IbexGoldenModel {
private:
    // A simple byte-addressable memory array to act as our source of truth
    std::map<uint32_t, uint8_t> memory; 

public:
    IbexGoldenModel(const std::string& vmem_path);
    uint32_t get_golden_instr(uint32_t pc);
};

#endif // IBEX_GOLDEN_MODEL_H