#ifndef FUNCTIONAL_COVERAGE_H
#define FUNCTIONAL_COVERAGE_H

#include<map>
#include<tuple>

class FunctionalCoverage {
private:
    // Instruction Types & Alignment Bins
    int cp_compressed = 0;
    int cp_uncompressed = 0;
    int cp_aligned = 0;
    int cp_misaligned = 0;

    // Cross Coverage Array: [is_compressed][is_misaligned]
    // [0][0] = Uncompressed & Aligned
    // [0][1] = Uncompressed & Misaligned
    // [1][0] = Compressed & Aligned
    // [1][1] = Compressed & Misaligned
    int cross_type_align_bin[2][2] = {{0, 0}, {0, 0}};

    // Cross Coverage Map : [gnt_delay][rvalid_delay]
    std::map<std::tuple<int, int>, int> cross_delay_bin;
    bool map_instantiated = false;
    
public:

    void initialize_delay_map();

    void sample_coverage(bool is_compressed, bool is_misaligned, int gnt_delay, int rvalid_delay);

    void report_coverage();
};
#endif // FUNCTIONAL_COVERAGE_H