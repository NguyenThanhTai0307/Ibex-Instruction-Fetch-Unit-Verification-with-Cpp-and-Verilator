#ifndef IBEX_FUNCTIONAL_COVERAGE_CPP
#define IBEX_FUNCTIONAL_COVERAGE_CPP

#include <iostream>
#include"ibex_functional_coverage.h"

    void FunctionalCoverage::sample_coverage(bool is_compressed, bool is_misaligned) {
        
        // Sample Instruction Type
        if (is_compressed) {
            cp_compressed += 1;
        } else {
            cp_uncompressed += 1;
        }

        // Sample Alignment Boundary
        if (is_misaligned) {
            cp_misaligned += 1;
        } else {
            cp_aligned += 1;
        }

        // Sample Cross-Coverage
        cross_type_align_bin[is_compressed][is_misaligned] += 1;
    }

    void FunctionalCoverage::report_coverage() {
        std::cout << "\n===================================================\n";
        std::cout << "         DPI-C FUNCTIONAL COVERAGE REPORT          \n";
        std::cout << "===================================================\n";
        
        std::cout << "[Instruction Type Bins]\n";
        std::cout << "  - Compressed   : " << cp_compressed << "\n";
        std::cout << "  - Uncompressed : " << cp_uncompressed << "\n\n";
        
        std::cout << "[Alignment Bins]\n";
        std::cout << "  - Aligned      : " << cp_aligned << "\n";
        std::cout << "  - Misaligned   : " << cp_misaligned << "\n\n";
        
        std::cout << "[Cross-Coverage] Combinations (Type x Alignment):\n";
        std::cout << "  - (0, 0) Uncompressed & Aligned   : " << cross_type_align_bin[0][0] << "\n";
        std::cout << "  - (0, 1) Uncompressed & Misaligned: " << cross_type_align_bin[0][1] << "\n";
        std::cout << "  - (1, 0) Compressed & Aligned     : " << cross_type_align_bin[1][0] << "\n";
        std::cout << "  - (1, 1) Compressed & Misaligned  : " << cross_type_align_bin[1][1] << "\n";
        std::cout << "===================================================\n\n";
    }

#endif // IBEX_FUNCTIONAL_COVERAGE_CPP