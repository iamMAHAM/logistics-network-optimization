#include "tsp.h"
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <limits.h>

// Fonction pour résoudre le problème du voyageur de commerce (TSP) avec programmation dynamique
int tsp(int **graph, int n, int pos, int visited, int **dp)
{
    if (visited == (1 << n) - 1)
        return graph[pos][0]; // Retour au point de départ

    if (dp[pos][visited] != -1)
        return dp[pos][visited];

    int minCost = INT_MAX;
    for (int city = 0; city < n; city++)
    {
        if ((visited & (1 << city)) == 0)
        {
            int cost = graph[pos][city] + tsp(graph, n, city, visited | (1 << city), dp);
            if (cost < minCost)
                minCost = cost;
        }
    }

    return dp[pos][visited] = minCost;
}

void solveTSP(int **graph, int n)
{
    int **dp = (int **)malloc(n * sizeof(int *));
    for (int i = 0; i < n; i++)
    {
        dp[i] = (int *)malloc((1 << n) * sizeof(int));
        for (int j = 0; j < (1 << n); j++)
            dp[i][j] = -1;
    }

    int result = tsp(graph, n, 0, 1, dp);
    printf("Coût minimum pour le TSP : %d\n", result);

    for (int i = 0; i < n; i++)
        free(dp[i]);
    free(dp);
}