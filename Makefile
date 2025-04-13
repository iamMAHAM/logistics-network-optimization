# Variables
CC = clang
CFLAGS = -Wall -Wextra -g
SRC = core/graph.c algorithms/dfs.c algorithms/bfs.c algorithms/graph_analysis.c algorithms/floyd_warshall.c algorithms/bellman_ford.c algorithms/tsp.c algorithms/multi_day_planning.c core/temporal_variations.c network/parser.c network/cJSON.c main.c
OBJ = $(SRC:.c=.o)
TARGET = main

# Règle par défaut
all: $(TARGET)

# Création de l'exécutable
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

# Compilation des fichiers objets
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Nettoyage
clean:
	rm -f $(OBJ) $(TARGET)

# Phony targets
.PHONY: all clean