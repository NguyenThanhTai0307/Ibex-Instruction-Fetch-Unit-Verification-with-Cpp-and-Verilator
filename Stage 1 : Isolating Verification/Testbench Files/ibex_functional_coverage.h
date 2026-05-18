#ifndef FUNCTIONAL_COVERAGE_H
#define FUNCTIONAL_COVERAGE_H
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
public:

    void sample_coverage(bool is_compressed, bool is_misaligned);

    void report_coverage();
};
#endif // FUNCTIONAL_COVERAGE_H