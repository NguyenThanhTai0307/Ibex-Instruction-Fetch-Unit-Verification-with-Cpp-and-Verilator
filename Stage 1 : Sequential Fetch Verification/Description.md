1. Primary Task :
- Build a C++-based verification environment to verify the IFU as an isolated module ( The IFU will be separated from
the rest of the  RISC-V core ).

2. Goals :
- Verify the basic operation of the IFU.
- Ensure the correctness and integrity of the instructions going into and out of the IFU.
- Ensure the timing and the word-aligned fetching of instruction are not violated.
- Check for the operation of the prefetch FIFO feedthrough path.
- Check the FIFO limit, ensure that it will not require any more instruction when it is full.

3. Tools :
- FuseSoc

4. Note :
- We will not check the flush of the prefetch FIFO (i.e. Branch Flush, Invalid Flush, No Flush,...), this will be done in stage 2.
- Before compiling the testbench, we have to install FuseSoc and all the required python packages of Ibex.
