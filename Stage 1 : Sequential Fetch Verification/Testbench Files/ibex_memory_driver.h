#ifndef MEMORY_DRIVER_H
#define MEMORY_DRIVER_H

#include "Vtop.h"
#include "ibex_tb_component.h"
#include "ibex_output_monitor.h"
#include <stdint.h>
#include <fstream>
#include <string>
#include <unordered_map>
#include <queue>
#include <random>

class OutputMonitor;

class MemoryDriver : public TbComponent {
private:
    Vtop* dut;
    OutputMonitor* mon;

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
    MemoryDriver(Vtop* _dut, OutputMonitor* _mon, std::string& vmem_path, uint32_t seed);

    void load_vmem(const std::string& filepath);

    void reset() override;
    void run() override;
};
#endif // MEMORY_DRIVER_H