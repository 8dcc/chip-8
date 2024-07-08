
#ifndef CPU_H_
#define CPU_H_ 1

#include <stdint.h>
#include <stdbool.h>

/* Size of the memory we are emulating */
#define MEM_SZ 0x1000

/* Address where the ROMs are loaded, and the initial value of PC */
#define ROM_LOAD_ADDR 0x200

/* Emulated address where the digits start */
#define DIGITS_ADDR 0x10

/* Height (number of bytes) of each character sprite */
#define CHAR_SPRITE_H 5

/* Number of cycles that the CPU will emulate on each 60Hz frame. In other
 * words, each instruction will run at (60*N) Hz. */
#define CYCLES_PER_FRAME 10

typedef struct CpuCtx {
    /* Memory, array of MEM_SZ bytes */
    uint8_t* mem;

    /* General purpose registers. V[0xF] is used for flags. */
    uint8_t V[16];

    /* For memory addresses */
    uint16_t I;

    /* Delay and sound timers */
    uint8_t DT, ST;

    /* Program counter */
    uint16_t PC;

    /* Stack pointer */
    uint8_t SP;

    /* Stack */
    uint16_t stack[16];
} CpuCtx;

/*----------------------------------------------------------------------------*/

/* Initialize a CPU context structure */
void cpu_init(CpuCtx* ctx);

/* Free a CPU context structure recursively */
void cpu_free(CpuCtx* ctx);

/* Load a ROM file into memory, at the current Program Counter (PC) address */
void cpu_load_rom(CpuCtx* ctx, const char* rom_filename);

/* This function should be called at a rate of 60Hz. It will run
 * CYCLES_PER_FRAME cycles by calling `cpu_cycle', and then decrement the timers
 * if needed. */
void cpu_frame(CpuCtx* ctx);

/* Increment the Program Counter and execute the next instruction by calling
 * `cpu_exec'. */
void cpu_cycle(CpuCtx* ctx);

/* Parse, execute and (optionally) print the instruction with the specified
 * opcode. */
void cpu_exec(CpuCtx* ctx, uint16_t opcode);

/* Dump the specified number of bytes from the emulated memory, starting at
 * ROM_LOAD_ADDR. */
void cpu_dump_mem(CpuCtx* ctx, size_t sz);

#endif /* CPU_H_ */
