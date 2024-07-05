
#include <stdint.h>
#include <stdlib.h>

#include "include/emulator.h"
#include "include/display.h"
#include "include/util.h"

void emulator_init(EmulatorCtx* ctx) {
    /* Allocate emulated memory */
    ctx->mem = malloc(MEM_SZ);

    /* Clear general purpose registers */
    for (size_t i = 0; i < LENGTH(ctx->V); i++)
        ctx->V[i] = 0;

    /* Clear I register, and delay and sound timers */
    ctx->I = ctx->DT = ctx->ST = 0;

    /* TODO: Initialize the program counter */
    ctx->PC = 0;

    /* Initialize the stack pointer, pointing to the bottom of the stack */
    ctx->SP = 0;

    /* Initialize the stack */
    for (size_t i = 0; i < LENGTH(ctx->stack); i++)
        ctx->stack[i] = 0;
}

void emulator_free(EmulatorCtx* ctx) {
    free(ctx->mem);
    free(ctx);
}

/*----------------------------------------------------------------------------*/

void stack_push(EmulatorCtx* ctx, uint16_t val) {
    if (ctx->SP >= LENGTH(ctx->stack)) {
        ERR("Tried pushing with SP: %d", ctx->SP);
        return;
    }

    ctx->stack[ctx->SP++] = val;
}

uint16_t stack_pop(EmulatorCtx* ctx) {
    if (ctx->SP <= 0) {
        ERR("Tried popping with SP: %d", ctx->SP);
        return 0;
    }

    return ctx->stack[--ctx->SP];
}

/*----------------------------------------------------------------------------*/

void parse_instruction(EmulatorCtx* ctx, uint16_t opcode) {
    /* Groups of 8 bits, from left to right */
    const uint8_t byte1 = (opcode >> 8) & 0xFF;
    const uint8_t byte2 = opcode & 0xFF;

    /* Groups of 4 bits, from left to right */
    const uint8_t nibble1 = (byte1 >> 4) & 0xF;
    const uint8_t nibble2 = byte1 & 0xF;
    const uint8_t nibble3 = (byte2 >> 4) & 0xF;
    const uint8_t nibble4 = byte2 & 0xF;

    /* First 4 bits of the opcode */
    switch (nibble1) {
        case 0:   /* SYS addr */
        case 1: { /* JP addr */
            ctx->PC = opcode & 0xFFF;
            return;
        }

        /* CALL addr */
        case 2: {
            /* Push address of current instruction + size of opcode */
            stack_push(ctx, ctx->PC + sizeof(opcode));
            ctx->PC = opcode & 0xFFF;
            return;
        }

        /* SE Vx, byte */
        case 3: {
            if (ctx->V[nibble2] == byte2)
                ctx->PC += 2;
            return;
        }

        /* SE Vx, Vy */
        case 5: {
            if (nibble4 != 0) {
                ERR("Invalid 4th nibble of opcode: %04X", opcode);
                return;
            }

            if (ctx->V[nibble2] == ctx->V[nibble3])
                ctx->PC += 2;
            return;
        }

        /* LD Vx, byte */
        case 6: {
            ctx->V[nibble2] = byte2;
            return;
        }

        /* ADD Vx, byte */
        case 7: {
            /* NOTE: Unlike with `ADD Vx, Vy', the carry flag is not changed */
            ctx->V[nibble2] += byte2;
            return;
        }

        case 8: {
            switch (nibble4) {
                /* LD Vx, byte */
                case 0: {
                    ctx->V[nibble2] = ctx->V[nibble3];
                    return;
                }

                /* OR Vx, Vy */
                case 1: {
                    ctx->V[nibble2] |= ctx->V[nibble3];
                    return;
                }

                /* AND Vx, Vy */
                case 2: {
                    ctx->V[nibble2] &= ctx->V[nibble3];
                    return;
                }

                /* XOR Vx, Vy */
                case 3: {
                    ctx->V[nibble2] ^= ctx->V[nibble3];
                    return;
                }

                /* ADD Vx, Vy */
                case 4: {
                    const uint16_t result = ctx->V[nibble2] + ctx->V[nibble3];

                    /* Set the carry flag, if needed */
                    if (result > 0xFF)
                        ctx->V[0xF] = 1;

                    ctx->V[nibble2] = result & 0xFF;
                    return;
                }

                /* SUB Vx, Vy */
                case 5: {
                    /* Set the carry flag, if needed */
                    ctx->V[0xF] = ctx->V[nibble2] > ctx->V[nibble3];

                    ctx->V[nibble2] -= ctx->V[nibble3];
                    return;
                }

                /* SHR Vx {, Vy} */
                case 6: {
                    /* Store in VF if bit 0 of Vx is set before the operation */
                    ctx->V[0xF] = ctx->V[nibble2] & 1;

                    /* Shift 1 bit to the right, effectively dividing by 2 */
                    ctx->V[nibble2] >>= 1;
                    return;
                }

                /* SUBN Vx, Vy */
                case 7: {
                    /* Set the carry flag, if needed */
                    ctx->V[0xF] = ctx->V[nibble3] > ctx->V[nibble2];

                    ctx->V[nibble2] = ctx->V[nibble3] - ctx->V[nibble2];
                    return;
                }

                /* SHL Vx {, Vy} */
                case 0xE: {
                    /* Store in VF if bit 0 of Vx is set before the operation */
                    ctx->V[0xF] = ctx->V[nibble2] & 1;

                    /* Shift 1 bit to the left, effectively multiplying by 2 */
                    ctx->V[nibble2] <<= 1;
                    return;
                }

                /* TODO: Continue from: 9xy0 - SNE Vx, Vy */

                default: {
                    ERR("Unknown 4th nibble of opcode: %04X", opcode);
                    return;
                }
            }
        }

        default:
            break;
    }

    /* Check full 2-byte instructions */
    switch (opcode) {
        /* CLS */
        case 0x00E0: {
            display_clear();
            return;
        }

        /* RET */
        case 0x00EE: {
            ctx->PC = stack_pop(ctx);
            return;
        }

        default:
            break;
    }

    /* If we reached here, this was an invalid instruction */
    ERR("Invalid or unsupported opcode: %04X", opcode);
}
