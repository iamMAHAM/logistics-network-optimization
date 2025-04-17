#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "core/graph.h"
#include "network/parser.h"
#include "algorithms/bfs.h"
#include "algorithms/dfs.h"
#include "algorithms/floyd_warshall.h"
#include "algorithms/tsp.h"
#include "algorithms/bellman_ford.h"
#include "algorithms/genetic_algorithm.h"
#include "algorithms/multi_day_planning.h"
#include <float.h>
#include <limits.h>
#include <sys/resource.h> // For memory usage

#define MAX_TSP_SIZE 20 // Nombre maximum de sommets pour l'algorithme TSP

// Wrapper for BFS to match the expected signature
void BFSWrapper(Graph *graph)
{
    BFS(graph, 0); // Start BFS from vertex 0
}

// Wrapper for DFS to match the expected signature
void DFSWrapper(Graph *graph)
{
    DFS(graph, 0); // Start DFS from vertex 0
}

// Wrapper for Floyd-Warshall
void FloydWarshallWrapper(Graph *graph)
{
    double **adjMatrix = convertGraphToAdjMatrix(graph);
    floydWarshall(adjMatrix, graph->V);
    freeAdjMatrix(adjMatrix, graph->V);
}

// Correcting Bellman-Ford call
void BellmanFordWrapper(Graph *graph)
{
    double *dist = (double *)malloc(graph->V * sizeof(double));
    int *predecessor = (int *)malloc(graph->V * sizeof(int));
    if (!dist || !predecessor)
    {
        fprintf(stderr, "Erreur : Échec de l'allocation de mémoire pour Bellman-Ford.\n");
        exit(EXIT_FAILURE);
    }

    bellmanFord(graph, 0, dist, predecessor);

    free(dist);
    free(predecessor);
}

// Wrapper for TSP
void TSPWrapper(Graph *graph)
{
    int **adjMatrix = convertGraphToIntAdjMatrix(graph);
    solveTSP(adjMatrix, graph->V);
    freeIntAdjMatrix(adjMatrix, graph->V);
}

// Correcting Genetic Algorithm call
void GeneticAlgorithmWrapper(Graph *graph)
{
    // Create example vehicles
    int vehicleCount = 10;
    Vehicle *vehicles = createVehicles(vehicleCount, 100); // Example: 10 vehicles with capacity 100

    // Create example packages
    int packageCount = 10;
    Package *packages = (Package *)malloc(packageCount * sizeof(Package));
    for (int i = 0; i < packageCount; i++)
    {
        packages[i].id = i;
        packages[i].source = rand() % graph->V;
        packages[i].destination = rand() % graph->V;
        packages[i].weight = rand() % 50 + 1;  // Random weight between 1 and 50
        packages[i].priority = rand() % 3 + 1; // Priority between 1 and 3
    }

    // Create example GAConfig
    GAConfig config;
    config.populationSize = 100;
    config.maxGenerations = 50;
    config.mutationRate = 0.1;
    config.crossoverRate = 0.9;

    // Run genetic algorithm
    runGeneticAlgorithm(graph, vehicles, vehicleCount, packages, packageCount, config);

    // Free allocated memory
    free(packages);
    freeVehicles(vehicles, vehicleCount);
}

// Wrapper for Multi-Day Delivery Planning
void MultiDayPlanningWrapper(Graph *graph)
{
    int **costMatrix = convertGraphToIntAdjMatrix(graph);
    multiDayDeliveryPlanning(costMatrix, graph->V, 3); // Example: Plan for 3 days
    freeIntAdjMatrix(costMatrix, graph->V);
}

// Fonction pour mesurer le temps d'exécution
double measureExecutionTime(void (*algorithm)(Graph *), Graph *graph)
{
    clock_t start = clock();
    algorithm(graph);
    clock_t end = clock();
    return ((double)(end - start)) / CLOCKS_PER_SEC;
}

// Fonction pour mesurer l'utilisation de la mémoire
long getMemoryUsage()
{
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    return usage.ru_maxrss; // Memory usage in kilobytes
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

// Fonction pour comparer les performances des algorithmes
void compareAlgorithms(Graph *graph)
{
    printf("\nComparaison des algorithmes :\n");

    long memoryBefore, memoryAfter;
    double executionTime;

    // BFS
    memoryBefore = getMemoryUsage();
    executionTime = measureExecutionTime(BFSWrapper, graph);
    memoryAfter = getMemoryUsage();
    printf("BFS - Temps d'exécution : %.6f secondes, Mémoire utilisée : %ld KB\n", executionTime, memoryAfter - memoryBefore);

    // DFS
    memoryBefore = getMemoryUsage();
    executionTime = measureExecutionTime(DFSWrapper, graph);
    memoryAfter = getMemoryUsage();
    printf("DFS - Temps d'exécution : %.6f secondes, Mémoire utilisée : %ld KB\n", executionTime, memoryAfter - memoryBefore);

    // Floyd-Warshall
    memoryBefore = getMemoryUsage();
    executionTime = measureExecutionTime(FloydWarshallWrapper, graph);
    memoryAfter = getMemoryUsage();
    printf("Floyd-Warshall - Temps d'exécution : %.6f secondes, Mémoire utilisée : %ld KB\n", executionTime, memoryAfter - memoryBefore);

    // Bellman-Ford
    memoryBefore = getMemoryUsage();
    executionTime = measureExecutionTime(BellmanFordWrapper, graph);
    memoryAfter = getMemoryUsage();
    printf("Bellman-Ford - Temps d'exécution : %.6f secondes, Mémoire utilisée : %ld KB\n", executionTime, memoryAfter - memoryBefore);

    // TSP
    memoryBefore = getMemoryUsage();
    executionTime = measureExecutionTime(TSPWrapper, graph);
    memoryAfter = getMemoryUsage();
    printf("TSP - Temps d'exécution : %.6f secondes, Mémoire utilisée : %ld KB\n", executionTime, memoryAfter - memoryBefore);

    // Genetic Algorithm
    memoryBefore = getMemoryUsage();
    executionTime = measureExecutionTime(GeneticAlgorithmWrapper, graph);
    memoryAfter = getMemoryUsage();
    printf("Algorithme génétique - Temps d'exécution : %.6f secondes, Mémoire utilisée : %ld KB\n", executionTime, memoryAfter - memoryBefore);

    // Multi-Day Delivery Planning
    memoryBefore = getMemoryUsage();
    executionTime = measureExecutionTime(MultiDayPlanningWrapper, graph);
    memoryAfter = getMemoryUsage();
    printf("Planification multi-jours - Temps d'exécution : %.6f secondes, Mémoire utilisée : %ld KB\n", executionTime, memoryAfter - memoryBefore);
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

        compareAlgorithms(graph);

        freeGraph(graph);
    }

    return 0;
}