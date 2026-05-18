#!/bin/bash
# Setup script for the Ibex IFU UVM Testbench

# 1. Activate the Python Virtual Environment
source ~/hw_env/bin/activate

# 2. Register the local cores root for FuseSoC
export FUSESOC_CORES=.:/home/thanhtai/RISC-V/ibex

echo "Verification Environment Ready!"