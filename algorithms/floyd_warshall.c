#include "floyd_warshall.h"
#include <stdio.h>
#include <stdlib.h>
#include <float.h>

// Fonction pour implémenter l'algorithme de Floyd-Warshall
void floydWarshall(double **graph, int V)
{
    double **dist = (double **)malloc(V * sizeof(double *));
    for (int i = 0; i < V; i++)
    {
        dist[i] = (double *)malloc(V * sizeof(double));
        for (int j = 0; j < V; j++)
        {
            dist[i][j] = graph[i][j];
        }
    }

    for (int k = 0; k < V; k++)
    {
        for (int i = 0; i < V; i++)
        {
            for (int j = 0; j < V; j++)
            {
                if (dist[i][k] + dist[k][j] < dist[i][j])
                {
                    dist[i][j] = dist[i][k] + dist[k][j];
                }
            }
        }
    }

    printf("Matrice des plus courts chemins (Floyd-Warshall) :\n");
    for (int i = 0; i < V; i++)
    {
        for (int j = 0; j < V; j++)
        {
            if (dist[i][j] == DBL_MAX)
                printf("INF ");
            else
                printf("%.2f ", dist[i][j]);
        }
        printf("\n");
    }

    for (int i = 0; i < V; i++)
    {
        free(dist[i]);
    }
    free(dist);
}