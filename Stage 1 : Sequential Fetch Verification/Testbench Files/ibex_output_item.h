#ifndef OUTPUT_ITEM_H
#define OUTPUT_ITEM_H

#include<iostream>
#include<stdint.h>

class OutputItem {
public :
    uint32_t pc;
    uint32_t fetched_instr;
    uint16_t c_fetched_instr;
    bool is_compressed;
    bool is_misaligned;

    OutputItem() : pc(0), fetched_instr(0), is_misaligned(false),
    c_fetched_instr(0), is_compressed(false) {}

};
#endif // OUTPUT_ITEM_H