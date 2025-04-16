# Makefile propre pour le projet

# Compilateur et options
CC = gcc
CFLAGS = -Wall -Wextra -g

# Répertoires
SRC_DIR = .
BUILD_DIR = ./build

# Fichiers sources et objets
MAIN_SRC = main.c
DATA_GEN_SRC = data_generator.c

MAIN_OBJ = $(BUILD_DIR)/main.o
DATA_GEN_OBJ = $(BUILD_DIR)/data_generator.o

# Cibles
all: main data_generator

main: $(MAIN_OBJ)
	$(CC) $(CFLAGS) -o main $(MAIN_OBJ) core/graph.c network/parser.c network/cJSON.c algorithms/bfs.c algorithms/dfs.c algorithms/floyd_warshall.c algorithms/tsp.c

$(MAIN_OBJ): $(MAIN_SRC)
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $(MAIN_SRC) -o $(MAIN_OBJ)

data_generator: $(DATA_GEN_OBJ)
	$(CC) $(CFLAGS) -o data_generator $(DATA_GEN_OBJ) core/graph.c network/parser.c network/cJSON.c

$(DATA_GEN_OBJ): $(DATA_GEN_SRC)
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $(DATA_GEN_SRC) -o $(DATA_GEN_OBJ)

# Nettoyage
clean:
	rm -rf $(BUILD_DIR) main data_generator

.PHONY: all clean