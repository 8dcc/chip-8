
#ifndef EMULATOR_H_
#define EMULATOR_H_ 1

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

typedef struct EmulatorCtx {
    /* Memory, array of MEM_SZ bytes */
    uint8_t* mem;

    /*
     * General purpose registers.
     * V[0xF] is used for flags, and should not be accessed directly by
     * programs.
     */
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
} EmulatorCtx;

/*----------------------------------------------------------------------------*/

/* Initialize a emulator context structure */
void emulator_init(EmulatorCtx* ctx);

/* Free the necessary members of an emulator context structure. Doesn't free the
 * context structure itself. */
void emulator_free(EmulatorCtx* ctx);

/* Load a ROM file into memory, at the current Program Counter (PC) address */
void emulator_load_rom(EmulatorCtx* ctx, const char* rom_filename);

/* Dump the specified number of bytes from the emulated memory, starting at
 * ROM_LOAD_ADDR. */
void emulator_dump_mem(EmulatorCtx* ctx, size_t sz);

/* Push and pop values from the stack of the emulator */
void stack_push(EmulatorCtx* ctx, uint16_t val);
uint16_t stack_pop(EmulatorCtx* ctx);

/* Parse current instruction, and increment the Program Counter if needed */
void emulator_tick(EmulatorCtx* ctx);

/* Parse and execute the instruction with the specified opcode */
void exec_instruction(EmulatorCtx* ctx, uint16_t opcode);

#endif /* EMULATOR_H_ */
