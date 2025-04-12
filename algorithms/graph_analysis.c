#include "graph_analysis.h"
#include "dfs.h"
#include "bfs.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

// Fonction pour détecter les cycles dans le réseau
bool detectCycleUtil(Graph *graph, int vertex, bool *visited, int parent)
{
    visited[vertex] = true;
    AdjListNode *adjList = graph->array[vertex].head;

    while (adjList)
    {
        int dest = adjList->dest;
        if (!visited[dest])
        {
            if (detectCycleUtil(graph, dest, visited, vertex))
                return true;
        }
        else if (dest != parent)
        {
            return true;
        }
        adjList = adjList->next;
    }
    return false;
}

bool detectCycle(Graph *graph)
{
    bool *visited = (bool *)calloc(graph->V, sizeof(bool));
    for (int i = 0; i < graph->V; i++)
    {
        if (!visited[i])
        {
            if (detectCycleUtil(graph, i, visited, -1))
            {
                free(visited);
                return true;
            }
        }
    }
    free(visited);
    return false;
}

// Fonction pour identifier les composantes connexes
void findConnectedComponents(Graph *graph)
{
    bool *visited = (bool *)calloc(graph->V, sizeof(bool));
    int componentCount = 0;

    for (int i = 0; i < graph->V; i++)
    {
        if (!visited[i])
        {
            printf("Composante connexe %d :\n", ++componentCount);
            DFS(graph, i);
        }
    }

    free(visited);
}

// Fonction pour vérifier l'accessibilité entre deux nœuds
bool isAccessible(Graph *graph, int start, int target)
{
    bool *visited = (bool *)calloc(graph->V, sizeof(bool));
    Queue *queue = createQueue(graph->V);

    enqueue(queue, start);
    visited[start] = true;

    while (!isQueueEmpty(queue))
    {
        int current = dequeue(queue);
        if (current == target)
        {
            free(visited);
            freeQueue(queue);
            return true;
        }

        AdjListNode *adjList = graph->array[current].head;
        while (adjList)
        {
            if (!visited[adjList->dest])
            {
                visited[adjList->dest] = true;
                enqueue(queue, adjList->dest);
            }
            adjList = adjList->next;
        }
    }

    free(visited);
    freeQueue(queue);
    return false;
}

// Fonction pour calculer des statistiques sur la connectivité
void calculateConnectivityStats(Graph *graph)
{
    int totalDegree = 0;
    for (int i = 0; i < graph->V; i++)
    {
        AdjListNode *adjList = graph->array[i].head;
        while (adjList)
        {
            totalDegree++;
            adjList = adjList->next;
        }
    }

    double averageDegree = (double)totalDegree / graph->V;
    printf("Degré moyen des nœuds : %.2f\n", averageDegree);

    printf("Détection des composantes connexes :\n");
    findConnectedComponents(graph);
}