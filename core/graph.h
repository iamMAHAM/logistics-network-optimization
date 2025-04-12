#ifndef GRAPH_H
#define GRAPH_H

#include <stdio.h>
#include <stdlib.h>

// Structure pour les attributs d’une arête
typedef struct EdgeAttr
{
    float distance;    // en kilomètres
    float baseTime;    // en minutes (temps nominal)
    float cost;        // coût monétaire
    int roadType;      // type de route (0: asphalte, 1: latérite, etc.)
    float reliability; // indice de fiabilité [0,1]
    int restrictions;  // restrictions codées en bits
    // Autres attributs pertinents
} EdgeAttr;

// Structure pour un nœud de la liste d’adjacence
typedef struct AdjListNode
{
    int dest;                 // identifiant du nœud destination
    EdgeAttr attr;            // attributs de l’arête
    struct AdjListNode *next; // pointeur vers le prochain nœud
} AdjListNode;

// Structure pour la liste d’adjacence
typedef struct AdjList
{
    AdjListNode *head; // tête de la liste
} AdjList;

// Structure pour le graphe
typedef struct Graph
{
    int V;          // nombre de sommets
    AdjList *array; // tableau des listes d’adjacence
    // Informations supplémentaires sur les sommets
} Graph;

// Prototypes des fonctions
Graph *createGraph(int V);
void addEdge(Graph *graph, int src, int dest, EdgeAttr attr);
void removeEdge(Graph *graph, int src, int dest);
void addNode(Graph *graph);
void removeNode(Graph *graph, int node);
void freeGraph(Graph *graph);
void printGraph(Graph *graph);
void displayAdjacencyList(Graph *graph);
void DFS(Graph *graph, int startVertex);
void BFS(Graph *graph, int startVertex);

#endif // GRAPH_H