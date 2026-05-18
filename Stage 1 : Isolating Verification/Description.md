1. Primary Task :
- Build a C++-based verification environment to verify the IFU as an isolated module ( The IFU will be separated from
the rest of the  RISC-V core ).

2. Goals :
- Verify the basic operation of the IFU.
- Ensure the correctness and integrity of the instructions going into and out of the IFU.
- Ensure the timing and the word-aligned fetching of instruction are not violated.

3. Tools :
- FuseSoc

4. Note :
- We will not check the flush of the prefetch FIFO (i.e. Branch Flush, Invalid Flush, No Flush,...)
since we are isolating the IFU from the main components of the core.
- Before compiling the testbench, we have to install FuseSoc and all the required python packages of Ibex.
