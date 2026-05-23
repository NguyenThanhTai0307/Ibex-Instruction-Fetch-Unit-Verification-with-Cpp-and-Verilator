`ifndef IBEX_DUT_WRAPPER_SV
`define IBEX_DUT_WRAPPER_SV

import ibex_pkg::*;

module top (
    input  logic        clk_i,
    input  logic        rst_ni,
    input  logic        pc_set_i,

    // Flattened Memory Interface Signals
    input  logic        instr_gnt_i,
    input  logic        instr_rvalid_i,
    input  logic [31:0] instr_rdata_i,
    output logic        instr_req_o,
    output logic [31:0] instr_addr_o,

    // Flattened Output Interface Signals
    output logic        instr_new_o,
    output logic [31:0] pc_o,
    output logic [31:0] instr_rdata_o,
    output logic [15:0] c_instr_rdata_o, 
    output logic        instr_is_compressed_o
);

  ibex_if_stage u_if_stage (
      // Active Clocks & Resets
      .clk_i                    (clk_i),
      .rst_ni                   (rst_ni),
      
      // Instruction Memory Interface
      .instr_req_o              (instr_req_o),
      .instr_addr_o             (instr_addr_o),
      .instr_gnt_i              (instr_gnt_i),
      .instr_rvalid_i           (instr_rvalid_i),
      .instr_rdata_i            (instr_rdata_i),
      .instr_bus_err_i          ('0),
      .instr_intg_err_o         (),

      // IF-ID Pipeline Outputs
      .instr_valid_id_o         (),
      .instr_new_id_o           (instr_new_o),
      .instr_rdata_id_o         (instr_rdata_o),
      .instr_rdata_alu_id_o     (),
      .instr_rdata_c_id_o       (c_instr_rdata_o),
      .instr_is_compressed_id_o (instr_is_compressed_o),
      .instr_gets_expanded_id_o (),
      .instr_expanded_id_o      (),
      .instr_bp_taken_o         (),
      .instr_fetch_err_o        (),
      .instr_fetch_err_plus2_o  (),
      .illegal_c_insn_id_o      (),
      .dummy_instr_id_o         (),
      .pc_if_o                  (),
      .pc_id_o                  (pc_o),

      // Control & Configuration Ties
      .boot_addr_i              (32'h00100000),
      .req_i                    (1'b1),
      .pmp_err_if_i             ('0),
      .pmp_err_if_plus2_i       ('0),
      .instr_valid_clear_i      ('0),
      .pc_set_i                 (pc_set_i),
      .pc_mux_i                 (ibex_pkg::PC_BOOT),
      .nt_branch_mispredict_i   ('0),
      .nt_branch_addr_i         ('0),
      .exc_pc_mux_i             (ibex_pkg::EXC_PC_EXC),
      .exc_cause                ('{default: '0}),
      .dummy_instr_en_i         ('0),
      .dummy_instr_mask_i       ('0),
      .dummy_instr_seed_en_i    ('0),
      .dummy_instr_seed_i       ('0),
      .icache_enable_i          ('0),
      .icache_inval_i           ('0),
      .icache_ecc_error_o       (),
      .branch_target_ex_i       ('0),

      // CSRs
      .csr_mepc_i               ('0),
      .csr_depc_i               ('0),
      .csr_mtvec_i              ('0),
      .csr_mtvec_init_o         (),

      // ICache RAM IO (Tied off for simulation)
      .ic_tag_req_o             (),
      .ic_tag_write_o           (),
      .ic_tag_addr_o            (),
      .ic_tag_wdata_o           (),
      .ic_tag_rdata_i           ('{default: '0}),
      .ic_data_req_o            (),
      .ic_data_write_o          (),
      .ic_data_addr_o           (),
      .ic_data_wdata_o          (),
      .ic_data_rdata_i          ('{default: '0}),
      .ic_scr_key_valid_i       ('0),
      .ic_scr_key_req_o         (),

      // CRITICAL PIPELINE FIX: Tell the IFU the ID stage is always ready to accept data
      .id_in_ready_i            (1'b1),

      // Misc
      .pc_mismatch_alert_o      (),
      .if_busy_o                ()
  );

  // Bind Statements
  bind ibex_if_stage Assertion #(
    .ADDR_WIDTH(32), .DATA_WIDTH(32), .FIFO_DEPTH(3)
  ) des_asst_bind (
    .clk_i       (clk_i),
    .rst_ni      (rst_ni),
    .instr_req_o (instr_req_o),
    .instr_gnt_i (instr_gnt_i),
    .instr_rvalid_i (instr_rvalid_i),
    .instr_addr_o (instr_addr_o)
  );

endmodule
`endif // IBEX_DUT_WRAPPER_SV