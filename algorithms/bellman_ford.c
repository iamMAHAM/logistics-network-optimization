#include "bellman_ford.h"
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <stdbool.h>

// Structure pour représenter une arête
typedef struct Edge
{
    int src;
    int dest;
    double weight;
} Edge;

// Fonction pour implémenter l'algorithme de Bellman-Ford avec contraintes
bool bellmanFord(Graph *graph, int startVertex, double *dist, int *predecessor)
{
    int V = graph->V;
    int E = 0;

    // Compter le nombre d'arêtes
    for (int i = 0; i < V; i++)
    {
        AdjListNode *current = graph->array[i].head;
        while (current)
        {
            E++;
            current = current->next;
        }
    }

    // Créer une liste d'arêtes
    Edge *edges = (Edge *)malloc(E * sizeof(Edge));
    int edgeIndex = 0;
    for (int i = 0; i < V; i++)
    {
        AdjListNode *current = graph->array[i].head;
        while (current)
        {
            edges[edgeIndex].src = i;
            edges[edgeIndex].dest = current->dest;
            edges[edgeIndex].weight = current->attr.cost; // Utiliser le coût comme poids
            edgeIndex++;
            current = current->next;
        }
    }

    // Initialiser les distances et les prédécesseurs
    for (int i = 0; i < V; i++)
    {
        dist[i] = DBL_MAX;
        predecessor[i] = -1;
    }
    dist[startVertex] = 0;

    // Relaxer les arêtes |V| - 1 fois
    for (int i = 1; i <= V - 1; i++)
    {
        for (int j = 0; j < E; j++)
        {
            int u = edges[j].src;
            int v = edges[j].dest;
            double weight = edges[j].weight;
            if (dist[u] != DBL_MAX && dist[u] + weight < dist[v])
            {
                dist[v] = dist[u] + weight;
                predecessor[v] = u;
            }
        }
    }

    // Vérifier les cycles de poids négatif
    for (int j = 0; j < E; j++)
    {
        int u = edges[j].src;
        int v = edges[j].dest;
        double weight = edges[j].weight;
        if (dist[u] != DBL_MAX && dist[u] + weight < dist[v])
        {
            free(edges);
            return false; // Cycle de poids négatif détecté
        }
    }

    free(edges);
    return true;
}