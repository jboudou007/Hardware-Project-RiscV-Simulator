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



#include <stdint.h>
#include <stdbool.h>



typedef struct {
    uint32_t opcode;
    uint32_t rd;
    uint32_t rs1;
    uint32_t rs2;
    int32_t imm;
} Instruction;

typedef struct {
    int32_t registers[32];
} RegisterFile;

typedef struct {
    uint8_t *data;
    size_t size;
} Memory;

typedef struct {
    Instruction IF;
    Instruction ID;
    Instruction EX;
    Instruction MEM;
    Instruction WB;
    bool stall;
} Pipeline;


// Defining Function signatures Fetch(), Decode(), Execute()
Instruction Fetch(Memory *memory, uint32_t pc);

void Decode(Instruction *instruction, RegisterFile *registerFile);

int32_t Execute(Instruction *instruction);

//Defining the memory access
bool AccessMemory(Instruction *instruction, Memory *memory, int32_t result);

// Defining Function Signature for Write Result
void WriteResult(Instruction *instruction, RegisterFile *registerFile, int32_t result);

//Defining the function simulator
void Simulate(Memory *memory, RegisterFile *registerFile);

//Defining The print pipeline state
void PrintPipelineState(Pipeline *pipeline, uint32_t cycle);

//Defining Printing function
void PrintInstruction(Instruction *instruction);

