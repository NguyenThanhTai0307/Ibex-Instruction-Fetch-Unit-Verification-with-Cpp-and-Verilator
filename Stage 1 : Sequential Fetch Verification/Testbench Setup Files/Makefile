# ==========================================
# 1. Variables & Paths
# ==========================================
PROG_NAME   = hello_test
SW_DIR      = /home/thanhtai/RISC-V/ibex/examples/sw/simple_system/hello_test

# The shared common directory containing the boot logic, linker, and C code
COMMON_DIR  = /home/thanhtai/RISC-V/ibex/examples/sw/simple_system/common
LINK_SCRIPT = $(COMMON_DIR)/link.ld
COMMON_SRC  = $(COMMON_DIR)/simple_system_common.c
BOOT_SRC    = $(COMMON_DIR)/crt0.S

RV_CC       = riscv32-unknown-elf-gcc
OBJCOPY     = riscv32-unknown-elf-objcopy

# ==========================================
# 2. Main Execution Targets
# ==========================================
# Target is now just VMEM and AUTOINST expansion.
# Standalone Spike execution is removed.
all: $(PROG_NAME).vmem

# ==========================================
# 3. Build Rules
# ==========================================

# Step 1: Build RISC-V Firmware
$(PROG_NAME).elf: $(SW_DIR)/$(PROG_NAME).c $(COMMON_SRC) $(BOOT_SRC) $(LINK_SCRIPT)
	$(RV_CC) -march=rv32imc -mabi=ilp32 -nostartfiles -T $(LINK_SCRIPT) -I $(COMMON_DIR) $(BOOT_SRC) $(COMMON_SRC) $(SW_DIR)/$(PROG_NAME).c -o $(PROG_NAME).elf

# Step 1.5: Generate VMEM for RTL Simulation
$(PROG_NAME).vmem: $(PROG_NAME).elf
	$(OBJCOPY) -O verilog $(PROG_NAME).elf $(PROG_NAME).vmem

# ==========================================
# 4. Clean Up
# ==========================================
clean:
	rm -f *.elf *.vmem