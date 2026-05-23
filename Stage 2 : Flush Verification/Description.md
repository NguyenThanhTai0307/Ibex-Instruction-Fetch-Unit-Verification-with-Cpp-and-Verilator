# Stage 2 : Flush Verification

1. Primary Task
   - Write a testbench to verify the operation of the IFU when a branch flush or a invalid flush occurs.
2. Goals
   - Ensure the IFU correctly flush its internal prefetch FIFO when a flush occurs.
   - Ensure the stability of the IFU even when the flush is forced during its most vulnerable states :
     - While it is waiting for a memory grant.
     - While it has an active grant but is waiting for valid data.
     - On the exact same cycle rvalid_i arrives.
