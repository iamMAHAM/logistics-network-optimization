#include "multi_day_planning.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

// Fonction pour planifier les livraisons sur plusieurs jours
void multiDayDeliveryPlanning(int **costMatrix, int n, int days)
{
    // Allouer et initialiser le tableau dp pour la programmation dynamique
    // dp[jour][noeud] = coût minimum pour être au noeud à la fin du jour
    int **dp = (int **)malloc((days + 1) * sizeof(int *));
    int **path = (int **)malloc((days + 1) * sizeof(int *)); // Pour retracer le chemin

    for (int d = 0; d <= days; d++)
    {
        dp[d] = (int *)malloc(n * sizeof(int));
        path[d] = (int *)malloc(n * sizeof(int));

        for (int i = 0; i < n; i++)
        {
            dp[d][i] = INT_MAX;
            path[d][i] = -1;
        }
    }

    // Initialiser: au jour 0, nous commençons au nœud 0 avec un coût de 0
    dp[0][0] = 0;

    // Remplir la table dp
    for (int d = 1; d <= days; d++)
    {
        for (int curr = 0; curr < n; curr++)
        { // nœud actuel
            for (int prev = 0; prev < n; prev++)
            { // nœud précédent
                // On exclut la possibilité de rester au même nœud (i == j)
                if (prev != curr && dp[d - 1][prev] != INT_MAX && costMatrix[prev][curr] != INT_MAX)
                {
                    int newCost = dp[d - 1][prev] + costMatrix[prev][curr];
                    if (newCost < dp[d][curr])
                    {
                        dp[d][curr] = newCost;
                        path[d][curr] = prev; // Stocker le nœud précédent pour reconstruire le chemin
                    }
                }
            }
        }
    }

    // Trouver le nœud final avec le coût minimum
    int minCost = INT_MAX;
    int finalNode = -1;

    for (int i = 0; i < n; i++)
    {
        if (dp[days][i] < minCost)
        {
            minCost = dp[days][i];
            finalNode = i;
        }
    }

    // Affichage commenté pour éviter l'encombrement du rapport
    if (minCost == INT_MAX)
    {
        /*
        printf("Impossible de planifier les livraisons sur %d jours.\n", days);
        */
    }
    else
    {
        /*
        printf("Coût minimum pour la planification sur %d jours : %d\n", days, minCost);

        // Reconstruire et afficher le chemin optimal
        printf("Itinéraire de livraison optimal :\n");
        */

        int *route = (int *)malloc((days + 1) * sizeof(int));
        int node = finalNode;
        route[days] = node;

        for (int d = days; d > 0; d--)
        {
            node = path[d][node];
            route[d - 1] = node;
        }

        /* Affichage commenté pour éviter l'encombrement du rapport
        for (int d = 0; d <= days; d++)
        {
            printf("Jour %d: Nœud %d\n", d, route[d]);
        }
        */

        free(route);
    }

    // Libérer la mémoire
    for (int d = 0; d <= days; d++)
    {
        free(dp[d]);
        free(path[d]);
    }
    free(dp);
    free(path);
}