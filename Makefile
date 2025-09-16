OUT = test
C_FILES = main.c engine/engine.c engine/math.c engine/vector.c engine/graphics.c
CFLAGS = -Wall -Werror -Wextra
INCLUDES = -I./engine/
CC = gcc
LDFLAGS = -lX11

all: $(OUT)

$(OUT): $(C_FILES)
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@ $(LDFLAGS)

clean:
	rm -rf $(OUT)

.PHONY: all clean
