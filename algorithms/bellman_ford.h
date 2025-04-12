#ifndef BELLMAN_FORD_H
#define BELLMAN_FORD_H

#include "../core/graph.h"
#include <stdbool.h>

// Prototype de la fonction Bellman-Ford
bool bellmanFord(Graph *graph, int startVertex, double *dist, int *predecessor);

#endif // BELLMAN_FORD_H