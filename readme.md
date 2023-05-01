# RISC-V CPU Simulator

## Introduction
This is a RISC-V CPU simulator that can execute a subset of the RISC-V instruction set. The simulator is written in C and simulates the pipeline stages of a 5-stage pipeline.

## System Requirements
- C compiler (GCC or Clang)
- Make build system

## How to Run the Simulator
1. Clone the repository: `git clone https://github.com/jboudou007/Hardware-Project-RiscV-Simulator.git`
2. Change to the project directory: `cd riscv-simulator`
3. Compile the simulator: `make`
4. Run the simulator: `./simulator <path-to-your-file.txt>`

## What the Simulator Does
The simulator reads in a binary file containing RISC-V machine code and executes it using a simulated 5-stage pipeline. The pipeline stages are as follows:

1. Instruction Fetch (IF)
2. Instruction Decode (ID)
3. Execution (EX)
4. Memory Access (MEM)
5. Write-Back (WB)

The simulator keeps track of the state of each instruction in the pipeline at each cycle and prints out the pipeline state at each cycle. After all instructions have been executed, the simulator outputs statistics such as the total number of cycles, the average number of instructions per cycle, and the stall rate.

## Limitations
This is a basic implementation of a RISC-V CPU simulator and has several limitations. Some of the limitations are:

- The simulator does not support all RISC-V instructions.
- The simulator does not handle data hazards.
- The simulator does not handle control hazards.
- The simulator does not implement branch prediction.
- The simulator does not implement forwarding.
- The simulator does not implement any cache hierarchy.

## Contributing
Contributions are welcome! If you have any ideas for improving the simulator, please open an issue or a pull request.

## License
This project is licensed under the MIT License. See the LICENSE file for details.
