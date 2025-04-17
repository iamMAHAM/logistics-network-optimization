#include "tsp.h"
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <limits.h>

#define MAX_TSP_SIZE 20 // Maximum number of vertices for TSP algorithm

// Fonction pour résoudre le problème du voyageur de commerce (TSP) avec programmation dynamique
int tsp(int **graph, int n, int pos, int visited, int **dp)
{
    // Check for invalid inputs
    if (pos < 0 || pos >= n || visited < 0)
    {
        /*
        printf("Error: Invalid parameters in tsp function: pos=%d, visited=%d\n", pos, visited);
        */
        return INT_MAX;
    }

    // Base case: all cities have been visited
    if (visited == (1 << n) - 1)
        return graph[pos][0]; // Return to the starting point

    // If solution is already memoized
    if (dp[pos][visited] != -1)
        return dp[pos][visited];

    int minCost = INT_MAX;
    for (int city = 0; city < n; city++)
    {
        // If the city has not been visited
        if ((visited & (1 << city)) == 0)
        {
            // Check for overflow before adding costs
            if (graph[pos][city] == INT_MAX)
            {
                continue; // Skip unreachable cities
            }

            int newVisited = visited | (1 << city);
            int subResult = tsp(graph, n, city, newVisited, dp);

            if (subResult != INT_MAX)
            {
                // Safely add the costs, checking for overflow
                long long totalCost = (long long)graph[pos][city] + (long long)subResult;
                if (totalCost < INT_MAX && totalCost < minCost)
                {
                    minCost = (int)totalCost;
                }
            }
        }
    }

    return dp[pos][visited] = minCost;
}

void solveTSP(int **graph, int n)
{
    // Check if the graph is too large for our algorithm
    if (n > MAX_TSP_SIZE)
    {
        /* Commenté pour éviter l'encombrement du rapport
        printf("Error: Graph too large for TSP algorithm. Maximum size is %d vertices.\n", MAX_TSP_SIZE);
        */
        return;
    }

    // Allocate memory for the dynamic programming table
    int **dp = (int **)malloc(n * sizeof(int *));
    if (!dp)
    {
        /* Commenté pour éviter l'encombrement du rapport
        printf("Error: Failed to allocate memory for dp table\n");
        */
        return;
    }

    for (int i = 0; i < n; i++)
    {
        dp[i] = (int *)malloc((1 << n) * sizeof(int));
        if (!dp[i])
        {
            /* Commenté pour éviter l'encombrement du rapport
            printf("Error: Failed to allocate memory for dp[%d]\n", i);
            */
            // Free previously allocated memory
            for (int j = 0; j < i; j++)
            {
                free(dp[j]);
            }
            free(dp);
            return;
        }

        for (int j = 0; j < (1 << n); j++)
            dp[i][j] = -1;
    }

    int result = tsp(graph, n, 0, 1, dp);

    if (result == INT_MAX)
    {
        /* Commenté pour éviter l'encombrement du rapport
        printf("No valid TSP tour found (possibly disconnected graph)\n");
        */
    }
    else
    {
        /* Commenté pour éviter l'encombrement du rapport
        printf("Coût minimum pour le TSP : %d\n", result);
        */
    }

    // Free allocated memory
    for (int i = 0; i < n; i++)
    {
        free(dp[i]);
    }
    free(dp);
}