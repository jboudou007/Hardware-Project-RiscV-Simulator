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
#include <stdlib.h>
#include "simulator.h"

Memory ReadInstructionStream(const char *filename) {
    Memory memory;

    // Open the file containing the instruction stream
    FILE *file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Error opening file: %s\n", filename);
        exit(EXIT_FAILURE);
    }

    // Get the file size
    fseek(file, 0, SEEK_END);
    memory.size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Allocate memory for the instruction stream
    memory.data = malloc(memory.size);
    if (!memory.data) {
        fprintf(stderr, "Error allocating memory\n");
        exit(EXIT_FAILURE);
    }

    // Read the instruction stream into memory
    fread(memory.data, 1, memory.size, file);

    // Close the file
    fclose(file);

    return memory;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <instruction_stream_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Read the instruction stream from the file
    Memory memory = ReadInstructionStream(argv[1]);

    // Initialize the register file
    RegisterFile registerFile = {0};

    // Simulate the pipeline
    Simulate(&memory, &registerFile);

    // Free the allocated memory
    free(memory.data);

    return 0;
}
