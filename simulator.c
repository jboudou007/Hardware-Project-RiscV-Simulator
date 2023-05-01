///////////////////////////////////////////////////////////////////////////////
//
//  Author: Jean Bilong, bilong@etsu.edu
//         
//  Course: CSCI-3160-001 - Computer Systems
//  Assignment: Hardware Project
//  Description: RiscV Simulator
//    
//  
///////////////////////////////////////////////////////////////////////////////





#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "simulator.h"

Instruction Fetch(Memory *memory, uint32_t pc) {
    // Fetch the 32-bit instruction from memory and return it as an Instruction struct.
    Instruction fetched_instruction;

    // Check if the PC is within the memory bounds
    if (pc < memory->size && pc + 3 < memory->size) {
        uint32_t instruction = 0;
        for (int i = 0; i < 4; i++) {
            // Read each byte of the instruction and combine them into a 32-bit unsigned integer
            instruction |= ((uint32_t)memory->data[pc + i]) << (8 * i);
        }
        fetched_instruction.opcode = instruction;
    } else {
        // If the PC is out of bounds, return a "nop" instruction (addi x0, x0, 0)
        fetched_instruction.opcode = 0x13; // ADDI opcode
        fetched_instruction.rd = 0;
        fetched_instruction.rs1 = 0;
        fetched_instruction.imm = 0;
    }

    return fetched_instruction;
}

void Decode(Instruction *instruction, RegisterFile *registerFile) {
    // Decode the given instruction and read the required operands from the register file.
    // The source register values should be stored in the 'rs1' and 'rs2' fields of the Instruction struct.
    uint32_t opcode = instruction->opcode;

    // Extract the lower 7 bits for the primary opcode
    uint32_t primary_opcode = opcode & 0x7F;

    switch (primary_opcode) {
        case 0x03: // Load instructions (lw)
        case 0x23: // Store instructions (sw)
        case 0x13: // Arithmetic-Immediate instructions (addi)
        case 0x33: // Arithmetic instructions (add, sub)
            instruction->rd = (opcode >> 7) & 0x1F;
            instruction->rs1 = (opcode >> 15) & 0x1F;
            instruction->rs2 = (opcode >> 20) & 0x1F;
            instruction->imm = (opcode >> 20) & 0xFFF;
            
            // Read the values from the register file
            instruction->rs1 = registerFile->registers[instruction->rs1];
            instruction->rs2 = registerFile->registers[instruction->rs2];
            break;

        case 0x63: // Branch instructions (bne, beq)
            instruction->rs1 = (opcode >> 15) & 0x1F;
            instruction->rs2 = (opcode >> 20) & 0x1F;

            // Read the values from the register file
            instruction->rs1 = registerFile->registers[instruction->rs1];
            instruction->rs2 = registerFile->registers[instruction->rs2];

            // Reconstruct the immediate value for branch instructions
            instruction->imm = ((opcode >> 7) & 0x1E) | ((opcode >> 20) & 0x7E0) | ((opcode << 4) & 0x800) | ((opcode >> 19) & 0x1000);
            if (instruction->imm & 0x1000) {
                instruction->imm |= 0xFFFFE000; // Sign-extend the immediate value
            }
            break;

        default:
            // If an unknown opcode is encountered, return a "nop" instruction (addi x0, x0, 0)
            instruction->opcode = 0x13; // ADDI opcode
            instruction->rd = 0;
            instruction->rs1 = 0;
            instruction->imm = 0;
            break;
}}

int32_t Execute(Instruction *instruction) {
    // Execute the instruction and return the result.
    // You'll need to implement the arithmetic, data transfer, and control operations.
    // Calculate the effective memory address for load/store instructions.
    uint32_t opcode = instruction->opcode;
    uint32_t primary_opcode = opcode & 0x7F;
    int32_t result = 0;

    switch (primary_opcode) {
        case 0x03: // Load instructions (lw)
            // Calculate the effective address
            result = instruction->rs1 + instruction->imm;
            break;

        case 0x23: // Store instructions (sw)
            // Calculate the effective address
            result = instruction->rs1 + instruction->imm;
            break;

        case 0x13: // Arithmetic-Immediate instructions (addi)
            result = instruction->rs1 + instruction->imm;
            break;

        case 0x33: // Arithmetic instructions (add, sub)
            switch (opcode & 0xFE00707F) {
                case 0x33: // ADD
                    result = instruction->rs1 + instruction->rs2;
                    break;
                case 0x40005033: // SUB
                    result = instruction->rs1 - instruction->rs2;
                    break;
            }
            break;

        case 0x63: // Branch instructions (bne, beq)
            if ((opcode & 0x706F) == 0x63) { // BNE
                result = instruction->rs1 != instruction->rs2 ? 1 : 0;
            } else if ((opcode & 0x706F) == 0x1063) { // BEQ
                result = instruction->rs1 == instruction->rs2 ? 1 : 0;
            }
            break;

        default:
            // Unsupported instruction, treat as a NOP (addi x0, x0, 0)
            break;
    }

    return result;
}

bool AccessMemory(Instruction *instruction, Memory *memory, int32_t result) {
    // Access memory for load and store instructions.
    // If the instruction is a load, update the 'rd' field of the Instruction struct with the loaded value.
    // If the instruction is a store, write the value to memory.
    // Return true if a memory access was performed, false otherwise.
    uint32_t opcode = instruction->opcode;
    uint32_t primary_opcode = opcode & 0x7F;
    bool memory_accessed = false;

    switch (primary_opcode) {
        case 0x03: // Load instructions (lw)
            if (result >= 0 && result < (int32_t)memory->size) {
                instruction->rd = *((int32_t *)(memory->data + (size_t)result));
                memory_accessed = true;
            }
            break;

        case 0x23: // Store instructions (sw)
            if (result >= 0 && result < (int32_t)memory->size) {
                *((int32_t *)(memory->data + (size_t)result)) = instruction->rs2;
                memory_accessed = true;
            }
            break;

        default:
            // No memory access for other instructions
            break;
    }

    return memory_accessed;
}


void WriteResult(Instruction *instruction, RegisterFile *registerFile, int32_t result) {
    // Write the result to the destination register in the register file.
    // Make sure not to write to register x0, as it is hardwired to zero.
    uint32_t opcode = instruction->opcode;
    uint32_t primary_opcode = opcode & 0x7F;

    switch (primary_opcode) {
        case 0x03: // Load instructions (lw)
        case 0x13: // Arithmetic-Immediate instructions (addi)
        case 0x33: // Arithmetic instructions (add, sub)
            if (instruction->rd != 0) { // Make sure not to write to register x0
                registerFile->registers[instruction->rd] = result;
            }
            break;

        default:
            // No result to write for other instructions
            break;
    }
}

void PrintPipelineState(Pipeline *pipeline, uint32_t cycle) {
    // Print the pipeline state for the given cycle.
    // Include information about the instructions in each stage and any stalls (bubbles).
    printf("Cycle %d:\n", cycle);
    printf("IF: ");
    PrintInstruction(&pipeline->IF);
    printf("ID: ");
    PrintInstruction(&pipeline->ID);
    printf("EX: ");
    PrintInstruction(&pipeline->EX);
    printf("MEM: ");
    PrintInstruction(&pipeline->MEM);
    printf("WB: ");
    PrintInstruction(&pipeline->WB);
    printf("\n");
}

void PrintSimulationStatistics(uint32_t cycles, uint32_t instructions, uint32_t stalls) {
    // Calculate and print the simulation statistics, including:
    // - Total number of cycles
    // - Average instructions per cycle (IPC)
    // - Stall rate
    double ipc = (double)instructions / (double)cycles;
    double stall_rate = (double)stalls / (double)cycles * 100;

    printf("Simulation Statistics:\n");
    printf("Total Cycles: %u\n", cycles);
    printf("Instructions: %u\n", instructions);
    printf("Stalls: %u\n", stalls);
    printf("Average Instructions Per Cycle (IPC): %.2f\n", ipc);
    printf("Stall Rate: %.2f%%\n", stall_rate);
}

void PrintInstruction(Instruction *instruction) {
    // Print the instruction in a readable format.
    // You can use a switch statement based on the 'opcode' field to determine the instruction type and print it accordingly.

    uint32_t opcode = instruction->opcode;
    uint32_t primary_opcode = opcode & 0x7F; // Extract the primary opcode from the instruction

    // Determine the instruction type based on the primary opcode
    switch (primary_opcode) {
        case 0x13: // Arithmetic instructions (addi, subi)
            printf("addi\tx%d, x%d, %d\n", instruction->rd, instruction->rs1, instruction->imm);
            break;
        case 0x33: // Arithmetic instructions (add, sub)
            printf("add\tx%d, x%d, x%d\n", instruction->rd, instruction->rs1, instruction->rs2);
            break;
        case 0x23: // Store instructions (sw)
            printf("sw\tx%d, %d(x%d)\n", instruction->rs2, instruction->imm, instruction->rs1);
            break;
        case 0x03: // Load instructions (lw)
            printf("lw\tx%d, %d(x%d)\n", instruction->rd, instruction->imm, instruction->rs1);
            break;
        case 0x63: // Branch instructions (beq, bne)
            printf("beq\tx%d, x%d, %d\n", instruction->rs1, instruction->rs2, instruction->imm);
            break;
        case 0x6F: // Jump instruction (j)
            printf("j\t%d\n", instruction->imm);
            break;
        default:
            // Invalid instruction
            printf("Invalid instruction\n");
            break;
    }
}




void Simulate(Memory *memory, RegisterFile *registerFile) {
    Pipeline pipeline;
    uint32_t pc = 0;
    bool done = false;
    uint32_t cycle = 0;
    uint32_t instruction_count = 0;
    uint32_t stall_count = 0; // Assuming no stalls for this basic implementation



    while (!done) {
        // Write back the result to the register file
        WriteResult(&pipeline.WB, registerFile, pipeline.WB.rd);

        // Access memory for load and store instructions
        AccessMemory(&pipeline.MEM, memory, pipeline.MEM.rd);

        // Execute the instruction and calculate the address
        pipeline.EX.rd = Execute(&pipeline.EX);

        // Decode the instruction and read from the register file
        Decode(&pipeline.ID, registerFile);

        // Fetch the next instruction from memory
        pipeline.IF = Fetch(memory, pc);
        pc += 4; // Increment the program counter

        // Handle hazards and pipeline stalls, if necessary
        // For this basic implementation, we assume no hazards.

        // Move the instructions along the pipeline
        pipeline.WB = pipeline.MEM;
        pipeline.MEM = pipeline.EX;
        pipeline.EX = pipeline.ID;
        pipeline.ID = pipeline.IF;

        // Print the pipeline state for the current cycle
        PrintPipelineState(&pipeline, cycle);

        // Check for termination conditions, such as reaching the end of the instruction stream
        if (pc >= memory->size) {
            done = true;
        }

        // Increment the cycle counter and instruction count
        cycle++;
        instruction_count++;

        
    }

    // Print the simulation statistics
    PrintSimulationStatistics(cycle, instruction_count, stall_count);
}
