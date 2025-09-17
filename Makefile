OUT = test
C_FILES = main.c engine/engine.c engine/math.c engine/vector.c engine/graphics.c engine/objects.c engine/conversions.c engine/physics.c
CFLAGS = -Wall -Werror -Wextra -g3
INCLUDES = -I./engine/
CC = gcc
LDFLAGS = -lX11 -lm

all: $(OUT)

$(OUT): $(C_FILES)
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@ $(LDFLAGS)

clean:
	rm -rf $(OUT)

.PHONY: all clean
