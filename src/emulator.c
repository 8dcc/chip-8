
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "include/util.h"
#include "include/emulator.h"
#include "include/keyboard.h"
#include "include/display.h"

#define DO_STEP   true
#define DONT_STEP false

void emulator_init(EmulatorCtx* ctx) {
    /* Allocate emulated memory */
    ctx->mem = calloc(MEM_SZ, sizeof(uint8_t));

    /* Store the digit sprites in the "interpreter" memory region */
    memcpy(&ctx->mem[DIGITS_ADDR],
           "\xF0\x90\x90\x90\xF0\x20\x60\x20\x20\x70\xF0\x10\xF0\x80\xF0\xF0"
           "\x10\xF0\x10\xF0\x90\x90\xF0\x10\x10\xF0\x80\xF0\x10\xF0\xF0\x80"
           "\xF0\x90\xF0\xF0\x10\x20\x40\x40\xF0\x90\xF0\x90\xF0\xF0\x90\xF0"
           "\x10\xF0\xF0\x90\xF0\x90\x90\xE0\x90\xE0\x90\xE0\xF0\x80\x80\x80"
           "\xF0\xE0\x90\x90\x90\xE0\xF0\x80\xF0\x80\xF0\xF0\x80\xF0\x80\x80",
           16 * CHAR_SPRITE_H);

    /* Clear general purpose registers */
    for (size_t i = 0; i < LENGTH(ctx->V); i++)
        ctx->V[i] = 0;

    /* Clear I register, and delay and sound timers */
    ctx->I = ctx->DT = ctx->ST = 0;

    /* Initialize the program counter to where the programs are loaded */
    ctx->PC = ROM_LOAD_ADDR;

    /* Initialize the stack pointer, pointing to the bottom of the stack */
    ctx->SP = 0;

    /* Initialize the stack */
    for (size_t i = 0; i < LENGTH(ctx->stack); i++)
        ctx->stack[i] = 0;
}

void emulator_free(EmulatorCtx* ctx) {
    free(ctx->mem);
}

void emulator_load_rom(EmulatorCtx* ctx, const char* rom_filename) {
    FILE* fp = fopen(rom_filename, "rb");
    if (!fp)
        die("Failed to open file: '%s'\n", rom_filename);

    fseek(fp, 0L, SEEK_END);
    size_t file_sz = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    const size_t max_sz = MEM_SZ - ROM_LOAD_ADDR;
    if (file_sz > max_sz) {
        ERR("Warning: ROM is too large. Reading up to 0x%X bytes.", max_sz);
        file_sz = max_sz;
    }

    for (size_t i = 0; i < file_sz; i++)
        ctx->mem[ctx->PC + i] = fgetc(fp);
}

/*----------------------------------------------------------------------------*/

void emulator_dump_mem(EmulatorCtx* ctx, size_t sz) {
    for (size_t i = 0; i < sz; i++) {
        const int addr     = ROM_LOAD_ADDR + i;
        const uint8_t byte = ctx->mem[addr];

        if (addr % 0x10 == 0)
            printf("\n%04X: ", addr);
        else if (addr % 2 == 0)
            putchar(' ');

        printf("%02X", byte);
    }
    printf("\n\n");
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

void emulator_tick(EmulatorCtx* ctx) {
    /* Read next two bytes at the Program Counter. CHIP-8 is always
     * big-endian. */
    uint16_t current_opcode;
    current_opcode = ctx->mem[ctx->PC] << 8;
    current_opcode |= ctx->mem[ctx->PC + 1];

    /* Increment the Program Counter before executing the instruction itself */
    ctx->PC += 2;

    /* Parse and execute the instruction */
    exec_instruction(ctx, current_opcode);
}

void exec_instruction(EmulatorCtx* ctx, uint16_t opcode) {
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
        case 0: {
            switch (byte2) {
                /* CLS */
                case 0xE0: {
                    display_clear();

                    PRNT_I("CLS");
                } break;

                /* RET */
                case 0xEE: {
                    ctx->PC = stack_pop(ctx);

                    PRNT_I("RET");
                } break;

                default: {
                    ERR("Invalid 2nd byte of opcode: %04X", opcode);
                } break;
            }
        } break;

        case 1: { /* JP addr */
            ctx->PC = opcode & 0xFFF;
            PRNT_I("JP %X", opcode & 0xFFF);
        } break;

        /* CALL addr */
        case 2: {
            /* Push address of current instruction + size of opcode */
            stack_push(ctx, ctx->PC);
            ctx->PC = opcode & 0xFFF;

            PRNT_I("CALL %X", opcode & 0xFFF);
        } break;

        /* SE Vx, byte */
        case 3: {
            const bool cmp = ctx->V[nibble2] == byte2;
            if (cmp)
                ctx->PC += 2;

            PRNT_I("SE V%X, %X\t\t; Cmp: %X", nibble2, byte2, cmp);
        } break;

        /* SNE Vx, byte */
        case 4: {
            const bool cmp = ctx->V[nibble2] != byte2;
            if (cmp)
                ctx->PC += 2;

            PRNT_I("SNE V%X, %X\t\t; Cmp: %X", nibble2, byte2, cmp);
        } break;

        /* SE Vx, Vy */
        case 5: {
            if (nibble4 != 0) {
                ERR("Invalid 4th nibble of opcode: %04X", opcode);
            }
            break;

            const bool cmp = ctx->V[nibble2] == ctx->V[nibble3];
            if (cmp)
                ctx->PC += 2;

            PRNT_I("SE V%X, V%X\t\t; Cmp: %X", nibble2, nibble3, cmp);
        } break;

        /* LD Vx, byte */
        case 6: {
            ctx->V[nibble2] = byte2;
            PRNT_I("LD V%X, %X", nibble2, byte2);
        } break;

        /* ADD Vx, byte */
        case 7: {
            /* NOTE: Unlike with `ADD Vx, Vy', the carry flag is not changed */
            const uint16_t result = ctx->V[nibble2] + byte2;
            ctx->V[nibble2]       = result & 0xFF;

            PRNT_I("ADD V%X, %X\t\t; Result: %X", nibble2, byte2,
                   ctx->V[nibble2]);
        } break;

        case 8: {
            switch (nibble4) {
                /* LD Vx, Vy */
                case 0: {
                    ctx->V[nibble2] = ctx->V[nibble3];
                    PRNT_I("LD V%X, V%X", nibble2, nibble3);
                } break;

                /* OR Vx, Vy */
                case 1: {
                    ctx->V[nibble2] |= ctx->V[nibble3];
                    ctx->V[0xF] = 0;
                    PRNT_I("OR V%X, V%X", nibble2, nibble3);
                } break;

                /* AND Vx, Vy */
                case 2: {
                    ctx->V[nibble2] &= ctx->V[nibble3];
                    ctx->V[0xF] = 0;
                    PRNT_I("AND V%X, V%X", nibble2, nibble3);
                } break;

                /* XOR Vx, Vy */
                case 3: {
                    ctx->V[nibble2] ^= ctx->V[nibble3];
                    ctx->V[0xF] = 0;
                    PRNT_I("XOR V%X, V%X", nibble2, nibble3);
                } break;

                /* ADD Vx, Vy */
                case 4: {
                    const uint16_t result = ctx->V[nibble2] + ctx->V[nibble3];

                    /* Store the lower byte of the result */
                    ctx->V[nibble2] = result & 0xFF;

                    /* Set the carry flag, if needed */
                    ctx->V[0xF] = result > 0xFF;

                    PRNT_I("ADD V%X, V%X\t\t; Result: %X, Flag: %X", nibble2,
                           nibble3, ctx->V[nibble2], ctx->V[0xF]);
                } break;

                /* SUB Vx, Vy */
                case 5: {
                    /* Set the (negated) borrow flag, if needed */
                    const bool borrow = ctx->V[nibble2] >= ctx->V[nibble3];

                    /* Perform the subtraction before setting the borrow flag */
                    ctx->V[nibble2] -= ctx->V[nibble3];
                    ctx->V[0xF] = borrow;

                    PRNT_I("SUB V%X, V%X\t\t; Result: %X, Flag: %X", nibble2,
                           nibble3, ctx->V[nibble2], ctx->V[0xF]);
                } break;

                /* SHR Vx {, Vy} */
                case 6: {
                    /* VF will store if bit 7 of Vx was set before the
                     * operation. */
                    const bool discarded = ctx->V[nibble2] & 1;

                    /* Shift 1 bit to the right, effectively dividing by 2.
                     * Make sure the flags are set after the operation. */
                    ctx->V[nibble2] >>= 1;
                    ctx->V[0xF] = discarded;

                    PRNT_I("SHR V%X\t\t\t; Result: %X, Flag: %X", nibble2,
                           ctx->V[nibble2], ctx->V[0xF]);
                } break;

                /* SUBN Vx, Vy */
                case 7: {
                    /* Set the (negated) borrow flag, if needed */
                    const bool borrow = ctx->V[nibble3] >= ctx->V[nibble2];

                    /* Perform the subtraction before setting the borrow flag */
                    ctx->V[nibble2] = ctx->V[nibble3] - ctx->V[nibble2];
                    ctx->V[0xF]     = borrow;

                    PRNT_I("SUBN V%X, V%X\t\t; Result: %X, Flag: %X", nibble2,
                           nibble3, ctx->V[nibble2], ctx->V[0xF]);
                } break;

                /* SHL Vx {, Vy} */
                case 0xE: {
                    /* VF will store if bit 7 of Vx was set before the
                     * operation. */
                    const bool discarded = (ctx->V[nibble2] >> 7) & 1;

                    /* Shift 1 bit to the left, effectively multiplying by 2.
                     * Make sure the flags are set after the operation. */
                    ctx->V[nibble2] <<= 1;
                    ctx->V[0xF] = discarded;

                    PRNT_I("SHL V%X\t\t\t; Result: %X, Flag: %X", nibble2,
                           ctx->V[nibble2], ctx->V[0xF]);
                } break;

                default: {
                    ERR("Unknown 4th nibble of opcode: %04X", opcode);
                } break;
            }
        } break;

        /* SNE Vx, Vy */
        case 9: {
            if (nibble4 != 0) {
                ERR("Invalid 4th nibble of opcode: %04X", opcode);
            }
            break;

            const bool cmp = ctx->V[nibble2] != ctx->V[nibble3];
            if (cmp)
                ctx->PC += 2;

            PRNT_I("SNE V%X, V%X\t\t; Cmp: %X", nibble2, nibble3, cmp);
        } break;

        /* LD I, addr */
        case 0xA: {
            ctx->I = opcode & 0xFFF;
            PRNT_I("LD I, %X", opcode & 0xFFF);
        } break;

        /* JP V0, addr */
        case 0xB: {
            ctx->PC = ctx->V[0] + (opcode & 0xFFF);
            PRNT_I("JP V0, %X\t\t\t; Addr: %X", opcode & 0xFFF,
                   ctx->V[0] + (opcode & 0xFFF));
        } break;

        /* RND Vx, byte */
        case 0xC: {
            const uint8_t random_byte = rand() % 0xFF;
            ctx->V[nibble2]           = random_byte & byte2;

            PRNT_I("RND V%X, %X\t\t\t; Result: %X", nibble2, byte2,
                   ctx->V[nibble2]);
        } break;

        /* DRW Vx, Vy, nibble */
        case 0xD: {
            const uint8_t x           = ctx->V[nibble2];
            const uint8_t y           = ctx->V[nibble3];
            const void* bytes         = &ctx->mem[ctx->I];
            const uint8_t byte_number = nibble4;

            /* If there is a collision (a pixel was set, but is cleared after
             * the draw operation), set VF to 1. Set it to 0 otherwise. */
            ctx->V[0xF] = display_draw_sprite(x, y, bytes, byte_number);

            PRNT_I("DRW V%X, V%X, %X\t\t; I: %X", nibble2, nibble3, nibble4,
                   ctx->I);
        } break;

        case 0xE: {
            const int key   = ctx->V[nibble2];
            const bool held = kb_is_held(key);

            switch (byte2) {
                /* SKP Vx */
                case 0x9E: {
                    if (held)
                        ctx->PC += 2;

                    PRNT_I("SKP V%X\t\t\t; Cmp: %X", nibble2, held);
                } break;

                /* SKNP Vx */
                case 0xA1: {
                    if (!held)
                        ctx->PC += 2;

                    PRNT_I("SKP V%X\t\t\t; Cmp: %X", nibble2, !held);
                } break;

                default: {
                    ERR("Invalid 2nd byte of opcode: %04X", opcode);
                } break;
            }
        } break;

        case 0xF: {
            switch (byte2) {
                /* LD Vx, DT */
                case 0x07: {
                    ctx->V[nibble2] = ctx->DT;
                    PRNT_I("LD V%X, DT\t\t; Result: %X", nibble2,
                           ctx->V[nibble2]);
                } break;

                /* LD Vx, K */
                case 0x0A: {
                    /* TODO: Wait for keypress, save in Vx. Change return
                     * depending on it. */
                    PRNT_I("LD V%X, K\t\t\t; Not implemented", nibble2);
                } break;

                /* LD DT, Vx */
                case 0x15: {
                    ctx->DT = ctx->V[nibble2];
                    PRNT_I("LD DT, V%X", nibble2);
                } break;

                /* LD ST, Vx */
                case 0x18: {
                    ctx->ST = ctx->V[nibble2];
                    PRNT_I("LD ST, V%X", nibble2);
                } break;

                /* ADD I, Vx */
                case 0x1E: {
                    ctx->I += ctx->V[nibble2];
                    PRNT_I("ADD I, V%X\t\t; Result: %X", nibble2, ctx->I);
                } break;

                /* LD F, Vx */
                case 0x29: {
                    ctx->I = DIGITS_ADDR + ctx->V[nibble2] * CHAR_SPRITE_H;
                    PRNT_I("LD F, V%X\t\t\t; Addr: %X", nibble2, ctx->I);
                } break;

                /* LD B, Vx */
                case 0x33: {
                    uint8_t n = ctx->V[nibble2];

                    /* Store right-most decimal digit */
                    ctx->mem[ctx->I + 2] = n % 10;

                    /* Store middle decimal digit */
                    n /= 10;
                    ctx->mem[ctx->I + 1] = n % 10;

                    /* Store left-most decimal digit */
                    n /= 10;
                    ctx->mem[ctx->I] = n % 10;

                    PRNT_I("LD B, V%X", nibble2);
                } break;

                /* LD [I], Vx */
                case 0x55: {
                    for (int i = 0; i <= nibble2; i++)
                        ctx->mem[ctx->I + i] = ctx->V[i];

                    PRNT_I("LD [I], V%X", nibble2);
                } break;

                /* LD Vx, [I] */
                case 0x65: {
                    for (int i = 0; i <= nibble2; i++)
                        ctx->V[i] = ctx->mem[ctx->I + i];

                    PRNT_I("LD [I], V%X", nibble2);
                } break;

                default: {
                    ERR("Invalid 2nd byte of opcode: %04X", opcode);
                } break;
            }
        } break;

        default: {
            /* If we reached here, this was an invalid instruction */
            ERR("Invalid or unsupported opcode: %04X", opcode);
        } break;
    }
}
