#ifndef NETWORK_NETWORK_PARSER_H
#define NETWORK_NETWORK_PARSER_H

#include "../core/graph.h"

// Fonction pour charger un graphe à partir d'un fichier JSON
Graph *loadGraphFromJSON(const char *filename);

// Fonction pour sauvegarder un graphe dans un fichier JSON
void saveGraphToJSON(Graph *graph, const char *filename);

#endif // NETWORK_NETWORK_PARSER_H