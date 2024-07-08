
CC=gcc
CFLAGS=-std=gnu99 -Wall -Wextra -Wpedantic -ggdb3
LDFLAGS=$(shell sdl2-config --cflags --libs)

# Emulator
OBJ_FILES=main.c.o util.c.o display.c.o cpu.c.o keyboard.c.o
OBJS=$(addprefix obj/, $(OBJ_FILES))
EMULATOR=chip-8-emulator.out

# Disassembler
DISASSEMBLER=chip-8-disassembler.out

#-------------------------------------------------------------------------------

.PHONY: clean all

all: $(EMULATOR) $(DISASSEMBLER)

clean:
	rm -f $(OBJS)
	rm -f $(EMULATOR) $(DISASSEMBLER)

#-------------------------------------------------------------------------------

$(EMULATOR): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(DISASSEMBLER): disassembler/main.c
	$(CC) $(CFLAGS) -o $@ $^

obj/%.c.o : src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -o $@ -c $<
