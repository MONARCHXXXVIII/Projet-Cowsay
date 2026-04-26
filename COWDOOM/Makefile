CC := cc

ifeq ($(origin SDL_CFLAGS), undefined)
ifneq ($(shell command -v pkg-config 2>/dev/null),)
SDL_CFLAGS := $(shell pkg-config --cflags sdl2)
SDL_LIBS := $(shell pkg-config --libs sdl2)
else ifneq ($(shell command -v sdl2-config 2>/dev/null),)
SDL_CFLAGS := $(shell sdl2-config --cflags)
SDL_LIBS := $(shell sdl2-config --libs)
else
SDL_CFLAGS :=
SDL_LIBS := -lSDL2
$(warning SDL2 detection tool not found (pkg-config/sdl2-config). Set SDL_CFLAGS/SDL_LIBS manually if needed.)
endif
endif

CFLAGS := -D_POSIX_C_SOURCE=200809L -std=c11 -Wall -Wextra -Wpedantic -O2 $(SDL_CFLAGS)
LDFLAGS := -lm $(SDL_LIBS)

SRC := src/main.c src/game.c src/input.c src/video.c
OBJ := $(SRC:.c=.o)
TARGET := cowdoom

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

src/%.o: src/%.c include/game.h include/input.h
	$(CC) $(CFLAGS) -Iinclude -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJ) $(TARGET)
