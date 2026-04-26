# ===========================================================================
# Makefile — vache_folle (inférence locale GGUF via llama.cpp)
#
# Prérequis système :
#   sudo apt install build-essential cmake git
#
# Utilisation :
#   make            → clone llama.cpp (si absent) + compile
#   make run        → exécute avec le prompt par défaut
#   make clean      → supprime le binaire
#   make distclean  → supprime le binaire ET le répertoire llama.cpp
# ===========================================================================

TARGET      = vache_folle
SRC         = vache_folle.c

LLAMA_DIR   = llama.cpp
LLAMA_BUILD = $(LLAMA_DIR)/build

# En-têtes llama.cpp
INCLUDES    = -I$(LLAMA_DIR)/include -I$(LLAMA_DIR)/ggml/include

# Bibliothèques produites par cmake
LLAMA_LIB   = $(LLAMA_BUILD)/src/libllama.a
GGML_LIBS   = $(LLAMA_BUILD)/ggml/src/libggml.a \
              $(LLAMA_BUILD)/ggml/src/libggml-base.a \
              $(LLAMA_BUILD)/ggml/src/libggml-cpu.a

LDFLAGS     = -L$(LLAMA_BUILD)/src \
              -L$(LLAMA_BUILD)/ggml/src \
              -lllama -lggml -lggml-base -lggml-cpu \
              -lstdc++ -lm -lpthread -fopenmp

CC          = gcc
CFLAGS      = -Wall -Wextra -O2 -std=c11

# ---------------------------------------------------------------------------
.PHONY: all run clean distclean

all: $(TARGET)

# --- Clonage de llama.cpp si le répertoire est absent ----------------------
$(LLAMA_DIR):
	@echo "==> Clonage de llama.cpp..."
	git clone --depth 1 https://github.com/ggml-org/llama.cpp $(LLAMA_DIR)

# --- Construction de la bibliothèque llama.cpp (une seule fois) ------------
$(LLAMA_LIB): $(LLAMA_DIR)
	@echo "==> Build de llama.cpp (CPU uniquement)..."
	cmake -S $(LLAMA_DIR) -B $(LLAMA_BUILD) \
	      -DCMAKE_BUILD_TYPE=Release \
	      -DBUILD_SHARED_LIBS=OFF \
	      -DLLAMA_BUILD_TESTS=OFF \
	      -DLLAMA_BUILD_EXAMPLES=OFF \
	      -DLLAMA_BUILD_SERVER=OFF \
	      -DGGML_CUDA=OFF \
	      -DGGML_METAL=OFF
	cmake --build $(LLAMA_BUILD) --config Release -j$(shell nproc)

# --- Compilation du programme de test --------------------------------------
$(TARGET): $(SRC) $(LLAMA_LIB)
	@echo "==> Compilation de $(TARGET)..."
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET) $(SRC) $(LDFLAGS)
	@echo "==> Binaire prêt : ./$(TARGET)"

# --- Lancement rapide -------------------------------------------------------
run: $(TARGET)
	./$(TARGET) "Qu'est-ce qu'une vache ?"

# --- Nettoyage --------------------------------------------------------------
clean:
	rm -f $(TARGET) test_model

distclean: clean
	rm -rf $(LLAMA_BUILD)
