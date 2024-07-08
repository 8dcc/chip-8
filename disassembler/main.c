

#include <stdint.h>
#include <stdio.h>

#define P(...)                 \
    do {                       \
        printf("%X:\t", addr); \
        printf(__VA_ARGS__);   \
        putchar('\n');         \
    } while (0)

/*----------------------------------------------------------------------------*/

/* See also `cpu_exec' in the emulator source */
static void print_instruction(int addr, uint16_t opcode) {
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
                case 0xE0: {
                    P("CLS");
                } break;

                case 0xEE: {
                    P("RET");
                } break;

                default: {
                    P("???\t; %04X", opcode);
                } break;
            }
        } break;

        case 1: {
            P("JP %X", opcode & 0xFFF);
        } break;

        case 2: {
            P("CALL %X", opcode & 0xFFF);
        } break;

        case 3: {
            P("SE V%X, %X", nibble2, byte2);
        } break;

        case 4: {
            P("SNE V%X, %X", nibble2, byte2);
        } break;

        case 5: {
            if (nibble4 != 0)
                P("???\t; %04X", opcode);

            P("SE V%X, V%X", nibble2, nibble3);
        } break;

        case 6: {
            P("LD V%X, %X", nibble2, byte2);
        } break;

        case 7: {
            P("ADD V%X, %X", nibble2, byte2);
        } break;

        case 8: {
            switch (nibble4) {
                case 0: {
                    P("LD V%X, V%X", nibble2, nibble3);
                } break;

                case 1: {
                    P("OR V%X, V%X", nibble2, nibble3);
                } break;

                case 2: {
                    P("AND V%X, V%X", nibble2, nibble3);
                } break;

                case 3: {
                    P("XOR V%X, V%X", nibble2, nibble3);
                } break;

                case 4: {
                    P("ADD V%X, V%X", nibble2, nibble3);
                } break;

                case 5: {
                    P("SUB V%X, V%X", nibble2, nibble3);
                } break;

                case 6: {
                    P("SHR V%X", nibble2);
                } break;

                case 7: {
                    P("SUBN V%X, V%X", nibble2, nibble3);
                } break;

                case 0xE: {
                    P("SHL V%X", nibble2);
                } break;

                default: {
                    P("???\t; %04X", opcode);
                } break;
            }
        } break;

        case 9: {
            if (nibble4 != 0)
                P("???\t; %04X", opcode);

            P("SNE V%X, V%X", nibble2, nibble3);
        } break;

        case 0xA: {
            P("LD I, %X", opcode & 0xFFF);
        } break;

        case 0xB: {
            P("JP V0, %X", opcode & 0xFFF);
        } break;

        case 0xC: {
            P("RND V%X, %X", nibble2, byte2);
        } break;

        case 0xD: {
            P("DRW V%X, V%X, %X", nibble2, nibble3, nibble4);
        } break;

        case 0xE: {
            switch (byte2) {
                case 0x9E: {
                    P("SKP V%X", nibble2);
                } break;

                case 0xA1: {
                    P("SKNP V%X", nibble2);
                } break;

                default: {
                    P("???\t; %04X", opcode);
                } break;
            }
        } break;

        case 0xF: {
            switch (byte2) {
                case 0x07: {
                    P("LD V%X, DT", nibble2);
                } break;

                case 0x0A: {
                    P("LD V%X, K", nibble2);
                } break;

                case 0x15: {
                    P("LD DT, V%X", nibble2);
                } break;

                case 0x18: {
                    P("LD ST, V%X", nibble2);
                } break;

                case 0x1E: {
                    P("ADD I, V%X", nibble2);
                } break;

                case 0x29: {
                    P("LD F, V%X", nibble2);
                } break;

                case 0x33: {
                    P("LD B, V%X", nibble2);
                } break;

                case 0x55: {
                    P("LD [I], V%X", nibble2);
                } break;

                case 0x65: {
                    P("LD [I], V%X", nibble2);
                } break;

                default: {
                    P("???\t; %04X", opcode);
                } break;
            }
        } break;

        default: {
            P("???\t; %04X", opcode);
        } break;
    }
}

/*----------------------------------------------------------------------------*/

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <rom>\n", argv[0]);
        return 1;
    }

    const char* rom_filename = argv[1];
    FILE* fp                 = fopen(rom_filename, "rb");
    if (!fp) {
        fprintf(stderr, "Failed to open file: '%s'\n", rom_filename);
        return 1;
    }

    fseek(fp, 0L, SEEK_END);
    size_t file_sz = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    for (size_t i = 0; i < file_sz; i++) {
        uint8_t opcode_hi = (uint8_t)fgetc(fp);
        uint8_t opcode_lo = (uint8_t)fgetc(fp);

        uint16_t opcode = (opcode_hi << 8) | opcode_lo;
        print_instruction(i, opcode);
    }

    fclose(fp);
    return 0;
}
