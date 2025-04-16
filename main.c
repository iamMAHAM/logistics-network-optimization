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

#define MAX_TSP_SIZE 20 // Maximum number of vertices for TSP algorithm

// Function to measure execution time
double measureExecutionTime(void (*algorithm)(Graph *), Graph *graph)
{
    clock_t start = clock();
    algorithm(graph);
    clock_t end = clock();
    return ((double)(end - start)) / CLOCKS_PER_SEC;
}

// Function to test and measure BFS
void testBFS(Graph *graph)
{
    printf("\nTesting BFS:\n");
    BFS(graph, 0);
}

// Update testDFS to call DFS directly with a valid startVertex
void testDFS(Graph *graph)
{
    printf("\nTesting DFS:\n");
    int startVertex = 0; // Start DFS from vertex 0
    DFS(graph, startVertex);
}

// Function to test and measure Floyd-Warshall
void testFloydWarshall(Graph *graph)
{
    printf("\nTesting Floyd-Warshall:\n");

    // Convert adjacency list to adjacency matrix
    double **adjMatrix = (double **)malloc(graph->V * sizeof(double *));
    for (int i = 0; i < graph->V; i++)
    {
        adjMatrix[i] = (double *)malloc(graph->V * sizeof(double));
        for (int j = 0; j < graph->V; j++)
        {
            adjMatrix[i][j] = (i == j) ? 0 : DBL_MAX; // Initialize with 0 for self-loops, DBL_MAX for no direct edge
        }

        AdjListNode *current = graph->array[i].head;
        while (current)
        {
            adjMatrix[i][current->dest] = current->attr.distance;
            current = current->next;
        }
    }

    // Measure execution time of Floyd-Warshall
    clock_t start = clock();
    floydWarshall(adjMatrix, graph->V);
    clock_t end = clock();

    printf("Execution time: %.6f seconds\n", ((double)(end - start)) / CLOCKS_PER_SEC);

    // Free adjacency matrix
    for (int i = 0; i < graph->V; i++)
    {
        free(adjMatrix[i]);
    }
    free(adjMatrix);
}

// Function to test and measure TSP
void testTSP(Graph *graph)
{
    printf("\nTesting TSP:\n");

    // Check graph size to avoid memory issues
    if (graph->V > MAX_TSP_SIZE)
    {
        printf("Graph is too large for the TSP algorithm (has %d vertices). Maximum supported size is %d vertices.\n",
               graph->V, MAX_TSP_SIZE);
        printf("Skipping TSP test for this graph.\n");
        return;
    }

    // Convert adjacency list to adjacency matrix
    double **adjMatrix = (double **)malloc(graph->V * sizeof(double *));
    if (!adjMatrix)
    {
        printf("Memory allocation failed for adjacency matrix.\n");
        return;
    }

    for (int i = 0; i < graph->V; i++)
    {
        adjMatrix[i] = (double *)malloc(graph->V * sizeof(double));
        if (!adjMatrix[i])
        {
            printf("Memory allocation failed for adjacency matrix row %d.\n", i);
            // Free previously allocated memory
            for (int j = 0; j < i; j++)
            {
                free(adjMatrix[j]);
            }
            free(adjMatrix);
            return;
        }

        for (int j = 0; j < graph->V; j++)
        {
            adjMatrix[i][j] = (i == j) ? 0 : DBL_MAX; // Initialize with 0 for self-loops, DBL_MAX for no direct edge
        }

        AdjListNode *current = graph->array[i].head;
        while (current)
        {
            adjMatrix[i][current->dest] = current->attr.distance;
            current = current->next;
        }
    }

    // Convert adjacency list to adjacency matrix (int version)
    int **intAdjMatrix = (int **)malloc(graph->V * sizeof(int *));
    if (!intAdjMatrix)
    {
        printf("Memory allocation failed for integer adjacency matrix.\n");
        // Free double adjacency matrix
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
            printf("Memory allocation failed for integer adjacency matrix row %d.\n", i);
            // Free previously allocated memory
            for (int j = 0; j < i; j++)
            {
                free(intAdjMatrix[j]);
            }
            free(intAdjMatrix);

            // Free double adjacency matrix
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

    // Call TSP solver
    solveTSP(intAdjMatrix, graph->V);

    // Free int adjacency matrix
    for (int i = 0; i < graph->V; i++)
    {
        free(intAdjMatrix[i]);
    }
    free(intAdjMatrix);

    // Free adjacency matrix
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
        printf("Testing dataset: %s\n", datasets[i]);
        printf("==============================\n");

        Graph *graph = loadGraphFromJSON(datasets[i]);
        if (!graph)
        {
            printf("Failed to load graph from %s\n", datasets[i]);
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