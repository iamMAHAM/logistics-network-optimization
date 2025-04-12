#ifndef GRAPH_ANALYSIS_H
#define GRAPH_ANALYSIS_H
#include <stdbool.h>
#include "../core/graph.h"

// Détecter les cycles dans le réseau
bool detectCycle(Graph *graph);

// Identifier les composantes connexes
void findConnectedComponents(Graph *graph);

// Vérifier l'accessibilité entre deux nœuds
bool isAccessible(Graph *graph, int start, int target);

// Calculer des statistiques sur la connectivité
void calculateConnectivityStats(Graph *graph);

#endif // GRAPH_ANALYSIS_H