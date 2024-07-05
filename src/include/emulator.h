
#ifndef EMULATOR_H_
#define EMULATOR_H_ 1

#include <stdint.h>

#define MEM_SZ 0x1000

typedef struct EmulatorCtx {
    /* Memory, array of MEM_SZ bytes */
    void* mem;

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

/* Free an emulator context structure recursively */
void emulator_free(EmulatorCtx* ctx);

/* Push and pop values from the stack of the emulator */
void stack_push(EmulatorCtx* ctx, uint16_t val);
uint16_t stack_pop(EmulatorCtx* ctx);

#endif /* EMULATOR_H_ */
