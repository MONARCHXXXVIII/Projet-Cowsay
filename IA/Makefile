# ===========================================================================
# Makefile — vache_folle (normalement celui la marche)
# ===========================================================================

TARGET      = vache_folle
SRC         = vache_folle.c

LLAMA_DIR   = llama.cpp
LLAMA_BUILD = $(LLAMA_DIR)/build

# Détection Ninja (plus fiable que Make sous Windows)
NINJA_BIN   := $(shell command -v ninja 2>/dev/null)
ifeq ($(strip $(NINJA_BIN)),)
	CMAKE_GEN := Unix Makefiles
else
	CMAKE_GEN := Ninja
endif

# Nombre de coeurs
NPROC := $(shell nproc 2>/dev/null || getconf _NPROCESSORS_ONLN 2>/dev/null || echo 1)

# Compilateur
CC      = gcc
CFLAGS  = -Wall -Wextra -O2 -std=c11 -march=native -fexec-charset=UTF-8

# Includes
INCLUDES = -I$(LLAMA_DIR)/include -I$(LLAMA_DIR)/ggml/include

# LDFLAGS (ordre important !)
LDFLAGS = -L$(LLAMA_BUILD)/src -lllama \
          -L$(LLAMA_BUILD)/ggml/src -lggml -lggml-base -lggml-cpu \
          -lstdc++ -lm -lpthread -fopenmp

LLAMA_LIB = $(LLAMA_BUILD)/src/libllama.a

# ---------------------------------------------------------------------------
.PHONY: all run clean distclean

all: $(TARGET)

# --- Clone ---
$(LLAMA_DIR):
	@echo "==> Clonage de llama.cpp..."
	git clone --depth 1 https://github.com/ggml-org/llama.cpp $(LLAMA_DIR)

# --- Build llama.cpp ---
$(LLAMA_LIB): $(LLAMA_DIR)
	@echo "==> Build de llama.cpp..."
	cmake -S $(LLAMA_DIR) -B $(LLAMA_BUILD) \
	      -G "$(CMAKE_GEN)" \
	      -DCMAKE_BUILD_TYPE=Release \
	      -DBUILD_SHARED_LIBS=OFF \
	      -DLLAMA_BUILD_TESTS=OFF \
	      -DLLAMA_BUILD_EXAMPLES=OFF \
	      -DLLAMA_BUILD_SERVER=OFF \
	      -DLLAMA_BUILD_TOOLS=OFF \
	      -DGGML_CUDA=OFF \
	      -DGGML_METAL=OFF
	cmake --build $(LLAMA_BUILD) --config Release -j$(NPROC)

# --- Build programme ---
$(TARGET): $(SRC) $(LLAMA_LIB)
	@echo "==> Compilation de $(TARGET)..."
	$(CC) $(CFLAGS) $(INCLUDES) $(SRC) -o $(TARGET) $(LDFLAGS)
	@echo "==> OK : ./$(TARGET)"

# --- Run ---
run: $(TARGET)
	./$(TARGET) "Qu'est-ce qu'une vache ?"

# --- Clean ---
clean:
	rm -f $(TARGET)

distclean: clean
	rm -rf $(LLAMA_DIR)
