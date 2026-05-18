#ifndef MEMORY_DRIVER_H
#define MEMORY_DRIVER_H

#include "Vtop.h"
#include <stdint.h>
#include <fstream>
#include <string>
#include <unordered_map>
#include <queue>
#include <random>

class MemoryDriver {
private:
    Vtop* dut;

    std::unordered_map<uint32_t, uint32_t> sram;

    struct PendingTxn {
        uint32_t addr;
        int rvalid_delay;
    };
    std::queue<PendingTxn> txn_queue;
    PendingTxn current_txn;

    bool gnt_pending;
    bool data_pending;
    int gnt_delay;
    int rvalid_delay;
    int data_delay;

    std::mt19937 random;
    std::uniform_int_distribution<int> delay_range;

public:
    MemoryDriver(Vtop* _dut, const std::string& vmem_path);

    void load_vmem(const std::string& filepath);

    void reset();
    void run();
};
#endif // MEMORY_DRIVER_H