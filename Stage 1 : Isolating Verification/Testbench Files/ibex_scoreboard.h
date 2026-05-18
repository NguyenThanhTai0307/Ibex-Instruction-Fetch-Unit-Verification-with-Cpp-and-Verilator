#ifndef SCOREBOARD_H
#define SCOREBOARD_H
#include <queue>
#include "ibex_output_item.h"
#include "ibex_golden_model.h"

class Scoreboard {
private:
    std::queue<OutputItem> ifu_out_fifo; 
    IbexGoldenModel* gm;
    
    int match_count = 0;
    int error_count = 0;

public: 
    Scoreboard(IbexGoldenModel* _gm);

    void write(const OutputItem& tr);
    
    void run();
    
    void print_summary(); 
};
#endif // SCOREBOARD_H