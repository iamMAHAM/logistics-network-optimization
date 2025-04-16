#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "core/graph.h"
#include "network/parser.h"
#include "algorithms/bfs.h"
#include "algorithms/dfs.h"
#include "algorithms/floyd_warshall.h"
#include "algorithms/tsp.h"
#include <float.h>
#include <limits.h>

#define MAX_TSP_SIZE 20 // Nombre maximum de sommets pour l'algorithme TSP

// Fonction pour mesurer le temps d'exécution
double measureExecutionTime(void (*algorithm)(Graph *), Graph *graph)
{
    clock_t start = clock();
    algorithm(graph);
    clock_t end = clock();
    return ((double)(end - start)) / CLOCKS_PER_SEC;
}

// Fonction pour tester et mesurer BFS
void testBFS(Graph *graph)
{
    printf("\nTest de BFS :\n");
    BFS(graph, 0);
}

// Mettre à jour testDFS pour appeler DFS directement avec un startVertex valide
void testDFS(Graph *graph)
{
    printf("\nTest de DFS :\n");
    int startVertex = 0; // Commencer DFS à partir du sommet 0
    DFS(graph, startVertex);
}

// Fonction pour tester et mesurer Floyd-Warshall
void testFloydWarshall(Graph *graph)
{
    printf("\nTest de Floyd-Warshall :\n");

    // Convertir la liste d'adjacence en matrice d'adjacence
    double **adjMatrix = (double **)malloc(graph->V * sizeof(double *));
    for (int i = 0; i < graph->V; i++)
    {
        adjMatrix[i] = (double *)malloc(graph->V * sizeof(double));
        for (int j = 0; j < graph->V; j++)
        {
            adjMatrix[i][j] = (i == j) ? 0 : DBL_MAX; // Initialiser avec 0 pour les boucles, DBL_MAX pour aucune arête directe
        }

        AdjListNode *current = graph->array[i].head;
        while (current)
        {
            adjMatrix[i][current->dest] = current->attr.distance;
            current = current->next;
        }
    }

    // Mesurer le temps d'exécution de Floyd-Warshall
    clock_t start = clock();
    floydWarshall(adjMatrix, graph->V);
    clock_t end = clock();

    printf("Temps d'exécution : %.6f secondes\n", ((double)(end - start)) / CLOCKS_PER_SEC);

    // Libérer la matrice d'adjacence
    for (int i = 0; i < graph->V; i++)
    {
        free(adjMatrix[i]);
    }
    free(adjMatrix);
}

// Fonction pour tester et mesurer TSP
void testTSP(Graph *graph)
{
    printf("\nTest de TSP :\n");

    // Vérifier la taille du graphe pour éviter les problèmes de mémoire
    if (graph->V > MAX_TSP_SIZE)
    {
        printf("Le graphe est trop grand pour l'algorithme TSP (contient %d sommets). La taille maximale prise en charge est de %d sommets.\n",
               graph->V, MAX_TSP_SIZE);
        printf("Test TSP ignoré pour ce graphe.\n");
        return;
    }

    // Convertir la liste d'adjacence en matrice d'adjacence
    double **adjMatrix = (double **)malloc(graph->V * sizeof(double *));
    if (!adjMatrix)
    {
        printf("Échec de l'allocation de mémoire pour la matrice d'adjacence.\n");
        return;
    }

    for (int i = 0; i < graph->V; i++)
    {
        adjMatrix[i] = (double *)malloc(graph->V * sizeof(double));
        if (!adjMatrix[i])
        {
            printf("Échec de l'allocation de mémoire pour la ligne %d de la matrice d'adjacence.\n", i);
            // Libérer la mémoire précédemment allouée
            for (int j = 0; j < i; j++)
            {
                free(adjMatrix[j]);
            }
            free(adjMatrix);
            return;
        }

        for (int j = 0; j < graph->V; j++)
        {
            adjMatrix[i][j] = (i == j) ? 0 : DBL_MAX; // Initialiser avec 0 pour les boucles, DBL_MAX pour aucune arête directe
        }

        AdjListNode *current = graph->array[i].head;
        while (current)
        {
            adjMatrix[i][current->dest] = current->attr.distance;
            current = current->next;
        }
    }

    // Convertir la liste d'adjacence en matrice d'adjacence (version entière)
    int **intAdjMatrix = (int **)malloc(graph->V * sizeof(int *));
    if (!intAdjMatrix)
    {
        printf("Échec de l'allocation de mémoire pour la matrice d'adjacence entière.\n");
        // Libérer la matrice d'adjacence double
        for (int i = 0; i < graph->V; i++)
        {
            free(adjMatrix[i]);
        }
        free(adjMatrix);
        return;
    }

    for (int i = 0; i < graph->V; i++)
    {
        intAdjMatrix[i] = (int *)malloc(graph->V * sizeof(int));
        if (!intAdjMatrix[i])
        {
            printf("Échec de l'allocation de mémoire pour la ligne %d de la matrice d'adjacence entière.\n", i);
            // Libérer la mémoire précédemment allouée
            for (int j = 0; j < i; j++)
            {
                free(intAdjMatrix[j]);
            }
            free(intAdjMatrix);

            // Libérer la matrice d'adjacence double
            for (int j = 0; j < graph->V; j++)
            {
                free(adjMatrix[j]);
            }
            free(adjMatrix);
            return;
        }

        for (int j = 0; j < graph->V; j++)
        {
            intAdjMatrix[i][j] = (adjMatrix[i][j] == DBL_MAX) ? INT_MAX : (int)adjMatrix[i][j];
        }
    }

    // Appeler le solveur TSP
    solveTSP(intAdjMatrix, graph->V);

    // Libérer la matrice d'adjacence entière
    for (int i = 0; i < graph->V; i++)
    {
        free(intAdjMatrix[i]);
    }
    free(intAdjMatrix);

    // Libérer la matrice d'adjacence double
    for (int i = 0; i < graph->V; i++)
    {
        free(adjMatrix[i]);
    }
    free(adjMatrix);
}

int main()
{
    const char *datasets[] = {
        "datasets/small_network_normal.json",
        "datasets/small_network_peak.json",
        "datasets/small_network_crisis.json",
        "datasets/medium_network_normal.json",
        "datasets/medium_network_peak.json",
        "datasets/medium_network_crisis.json",
        "datasets/large_network_normal.json",
        "datasets/large_network_peak.json",
        "datasets/large_network_crisis.json"};

    int numDatasets = sizeof(datasets) / sizeof(datasets[0]);

    for (int i = 0; i < numDatasets; i++)
    {
        printf("\n==============================\n");
        printf("Test du dataset : %s\n", datasets[i]);
        printf("==============================\n");

        Graph *graph = loadGraphFromJSON(datasets[i]);
        if (!graph)
        {
            printf("Échec du chargement du graphe depuis %s\n", datasets[i]);
            continue;
        }

        testBFS(graph);
        testDFS(graph);
        testFloydWarshall(graph);
        testTSP(graph);

        freeGraph(graph);
    }

    return 0;
}