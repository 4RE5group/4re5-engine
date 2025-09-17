OUT = test
C_FILES = main.c engine/engine.c engine/math.c engine/vector.c engine/graphics.c engine/objects.c engine/conversions.c engine/physics.c engine/matrices.c
CFLAGS = -Wall -Werror -Wextra
INCLUDES = -I./engine/
CC = gcc
LDFLAGS = -lX11 -lm
dependencies = libx11-dev

all: $(OUT)

$(OUT): $(C_FILES)
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@ $(LDFLAGS)

clean:
	rm -rf $(OUT)

deps:
	sudo apt install $(dependencies) -y

.PHONY: all clean deps
