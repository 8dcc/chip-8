
CC=gcc
CFLAGS=-Wall -Wextra -Wpedantic
LDFLAGS=$(shell sdl2-config --cflags --libs)

OBJ_FILES=main.c.o util.c.o display.c.o emulator.c.o
OBJS=$(addprefix obj/, $(OBJ_FILES))

BIN=chip-8-emulator.out

#-------------------------------------------------------------------------------

.PHONY: clean all

all: $(BIN)

clean:
	rm -f $(OBJS)
	rm -f $(BIN)

#-------------------------------------------------------------------------------

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

obj/%.c.o : src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -o $@ -c $<
