`ifndef ASSERTION_SV
`define ASSERTION_SV
module Assertion #(
    parameter DATA_WIDTH = 32, // Standard RISC-V Data Width
    parameter ADDR_WIDTH = 32, // Standard RISC-V Address Width
    parameter FIFO_DEPTH = 3   // Defined by the 3-entry storage buffer spec
    ) (
    input logic clk_i,
    input logic rst_ni,
    input logic instr_req_o,
    input logic instr_gnt_i,
    input logic instr_rvalid_i,
    input logic [ADDR_WIDTH - 1 : 0] instr_addr_o
);

    // ----------------------------------------------------------------------
    // 1. Protocol Integrity Check
    // Requirement: Assert that instr_req_o remains high until instr_gnt_i is asserted.
    // ----------------------------------------------------------------------
    property p_req_held_until_gnt;
        @(posedge clk_i) disable iff (!rst_ni)
        // If there is a request but no grant, the request must remain high next cycle
        (instr_req_o && !instr_gnt_i) |=> instr_req_o;
    endproperty
    
    a_req_held_until_gnt: assert property (p_req_held_until_gnt)
        else $error("Protocol Violation: instr_req_o dropped before receiving instr_gnt_i.");

    // ----------------------------------------------------------------------
    // 2. Alignment Check
    // Requirement: Assert that instr_addr_o[1:0] is always 2'b00 for every request.
    // ----------------------------------------------------------------------
    property p_addr_aligned;
        @(posedge clk_i) disable iff (!rst_ni)
        // Whenever a request is made, the lower 2 bits must be zero (word-aligned)
        instr_req_o |-> (instr_addr_o[1:0] == 2'b00);
    endproperty
    
    a_addr_aligned: assert property (p_addr_aligned)
        else $error("Alignment Violation: IFU requested a non-word-aligned address.");

    // ----------------------------------------------------------------------
    // 3. FIFO Bounds Check (Outstanding Transactions)
    // Requirement: Assert that the FIFO never overflows beyond its DEPTH.
    // ----------------------------------------------------------------------
    
    // Internal counter to track "in-flight" transactions on the OBI bus
    int outstanding_txns;

    always_ff @(posedge clk_i or negedge rst_ni) begin
        if (!rst_ni) begin
            outstanding_txns <= 0;
        end else begin
            // Increment on accepted request (req & gnt), decrement on valid data (rvalid)
            // If both happen in the same cycle, the net change is 0.
            if ((instr_req_o && instr_gnt_i) && !instr_rvalid_i) begin
                outstanding_txns <= outstanding_txns + 1;
            end else if (!(instr_req_o && instr_gnt_i) && instr_rvalid_i) begin
                outstanding_txns <= outstanding_txns - 1;
            end
        end
    end

    property p_fifo_no_overflow;
        @(posedge clk_i) disable iff (!rst_ni)
        // The number of outstanding transactions must never exceed the FIFO capacity
        outstanding_txns <= FIFO_DEPTH;
    endproperty
    
    a_fifo_no_overflow: assert property (p_fifo_no_overflow)
        else $error("FIFO Overflow: Outstanding transactions exceeded FIFO_DEPTH (%0d).", FIFO_DEPTH);

    // Optional Check: Detect Underflow (receiving rvalid without an outstanding request)
    property p_fifo_no_underflow;
        @(posedge clk_i) disable iff (!rst_ni)
        instr_rvalid_i |-> (outstanding_txns > 0);
    endproperty

    a_fifo_no_underflow: assert property (p_fifo_no_underflow)
        else $error("Protocol Violation: Received instr_rvalid_i but no outstanding requests exist.");

endmodule
`endif // ASSERTION_SV