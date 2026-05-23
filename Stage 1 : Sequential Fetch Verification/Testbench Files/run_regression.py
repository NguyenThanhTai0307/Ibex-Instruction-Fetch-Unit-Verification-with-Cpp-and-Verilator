import subprocess
import random
import re
import sys

# --- Configuration ---
NUM_RUNS = 50
FUSESOC_CMD = ["fusesoc", "run", "--target=sim", "thanhtai:verification:ifu_cpp_tb:1.0"]

print("======================================================")
print(f"  STARTING RANDOMIZED REGRESSION SUITE ({NUM_RUNS} RUNS)")
print("======================================================")

passed_count = 0
failed_count = 0
failed_seeds = []

for i in range(1, NUM_RUNS + 1):
    # 1. Generate a random master seed for this specific run
    current_seed = random.randint(1, 999999999)
    
    # 2. Append the seed argument for Verilator
    run_cmd = FUSESOC_CMD + ['--run_options', f'+verilator+seed+{current_seed}']
    
    # 3. Execute the simulation and capture the terminal output silently
    result = subprocess.run(run_cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
    output = result.stdout

    # 4. Parse the Scoreboard output
    if "RESULT: PASS" in output and result.returncode == 0:
        print(f"[{i:03d}/{NUM_RUNS:03d}] Seed: {current_seed:<12} -> \033[92mPASS\033[0m")
        passed_count += 1
    else:
        print(f"[{i:03d}/{NUM_RUNS:03d}] Seed: {current_seed:<12} -> \033[91mFAIL\033[0m")
        failed_count += 1
        failed_seeds.append(current_seed)
        
        # Optional: Save the failing log to a file for later debugging
        #with open(f"fail_log_seed_{current_seed}.txt", "w") as f:
            #f.write(output)

print("\n======================================================")
print("                 REGRESSION SUMMARY                   ")
print("======================================================")
print(f" Total Runs: {NUM_RUNS}")
print(f" Passed:     \033[92m{passed_count}\033[0m")
print(f" Failed:     \033[91m{failed_count}\033[0m")

if failed_count > 0:
    print("\n[!] Failing Seeds to Replicate:")
    for seed in failed_seeds:
        # Provide the exact command the user needs to rerun the failing test
        print(f"    source setup_tb.sh && fusesoc run --target=sim thanhtai:verification:ifu_cpp_tb:1.0 --run_options '+verilator+seed+{seed}'")
    sys.exit(1)
else:
    print("\n\033[92m[SUCCESS] All tests passed! The IFU is bulletproof.\033[0m\n")
    sys.exit(0)