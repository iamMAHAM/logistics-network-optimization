#include "greedy_algorithms.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <float.h>

// Fonction de comparaison pour le tri des colis par priorité
static int comparePackages(const void *a, const void *b)
{
    Package *pa = (Package *)a;
    Package *pb = (Package *)b;
    return pa->priority - pb->priority; // Tri croissant par priorité (1: haute, 3: basse)
}

// Fonction pour calculer la distance entre deux nœuds dans le graphe
static double getDistance(Graph *graph, int source, int destination)
{
    AdjListNode *current = graph->array[source].head;
    while (current)
    {
        if (current->dest == destination)
        {
            return current->attr.distance;
        }
        current = current->next;
    }
    return -1; // Pas de connexion directe
}

// 1. Algorithme glouton pour l'affectation des colis aux véhicules
int greedyPackageAssignment(Package *packages, int packageCount, Vehicle *vehicles, int vehicleCount)
{
    // Trier les colis par ordre de priorité
    qsort(packages, packageCount, sizeof(Package), comparePackages);

    // Compteur de colis affectés
    int assignedCount = 0;

    // Pour chaque colis...
    for (int i = 0; i < packageCount; i++)
    {
        // Chercher le véhicule le plus adapté (celui qui a le plus d'espace disponible)
        int bestVehicleIndex = -1;
        int maxRemainingCapacity = -1;

        for (int j = 0; j < vehicleCount; j++)
        {
            int remainingCapacity = vehicles[j].capacity - vehicles[j].currentLoad;

            // Vérifier si ce véhicule peut prendre le colis
            if (remainingCapacity >= packages[i].weight && remainingCapacity > maxRemainingCapacity)
            {
                bestVehicleIndex = j;
                maxRemainingCapacity = remainingCapacity;
            }
        }

        // Si un véhicule a été trouvé, lui affecter le colis
        if (bestVehicleIndex != -1)
        {
            Vehicle *vehicle = &vehicles[bestVehicleIndex];

            // Ajouter l'ID du colis au véhicule
            vehicle->packageIds = (int *)realloc(vehicle->packageIds, (vehicle->packageCount + 1) * sizeof(int));
            vehicle->packageIds[vehicle->packageCount] = packages[i].id;
            vehicle->packageCount++;

            // Mettre à jour la charge actuelle
            vehicle->currentLoad += packages[i].weight;

            // Incrémenter le compteur de colis affectés
            assignedCount++;

            // printf("Colis %d (priorité %d, poids %d) affecté au véhicule %d\n",
            //        packages[i].id, packages[i].priority, packages[i].weight, vehicle->id);
        }
        else
        {
            // printf("Impossible d'affecter le colis %d (priorité %d, poids %d) à un véhicule\n",
            //        packages[i].id, packages[i].priority, packages[i].weight);
        }
    }

    // Retourner 1 si tous les colis ont été affectés, 0 sinon
    return (assignedCount == packageCount) ? 1 : 0;
}

// 2. Algorithme glouton pour la planification des tournées
void greedyRouteScheduling(Graph *graph, Vehicle *vehicles, int vehicleCount)
{
    for (int v = 0; v < vehicleCount; v++)
    {
        Vehicle *vehicle = &vehicles[v];

        // Si le véhicule n'a pas de colis, passer au suivant
        if (vehicle->packageCount == 0)
        {
            printf("Véhicule %d n'a pas de colis à livrer\n", vehicle->id);
            continue;
        }

        // Initialiser la route avec la position actuelle du véhicule
        vehicle->routeLength = vehicle->packageCount + 1; // +1 pour revenir au point de départ
        vehicle->route = (int *)malloc(vehicle->routeLength * sizeof(int));
        vehicle->route[0] = vehicle->location; // Point de départ

        // Créer une liste des destinations à visiter
        int *destinations = (int *)malloc(vehicle->packageCount * sizeof(int));
        for (int i = 0; i < vehicle->packageCount; i++)
        {
            int packageId = vehicle->packageIds[i];
            // Trouver la destination dans la liste des colis - ceci est simplifié, en réalité,
            // vous devriez avoir un moyen efficace de mapper packageId vers la destination
            destinations[i] = packageId % graph->V; // Simplification pour cet exemple
        }

        // Algorithme du plus proche voisin pour construire la tournée
        int currentPosition = vehicle->location;
        int visited = 1; // Nombre de nœuds visités (départ déjà compté)

        // Marquer les destinations comme non visitées
        int *visited_destinations = (int *)calloc(vehicle->packageCount, sizeof(int));

        // Tant qu'il reste des destinations à visiter
        while (visited <= vehicle->packageCount)
        {
            double minDistance = DBL_MAX;
            int nextDestination = -1;
            int nextIndex = -1;

            // Chercher la destination la plus proche non visitée
            for (int i = 0; i < vehicle->packageCount; i++)
            {
                if (!visited_destinations[i])
                {
                    double dist = getDistance(graph, currentPosition, destinations[i]);
                    if (dist > 0 && dist < minDistance)
                    { // Si une connexion directe existe
                        minDistance = dist;
                        nextDestination = destinations[i];
                        nextIndex = i;
                    }
                }
            }

            // Si une destination a été trouvée
            if (nextDestination != -1)
            {
                vehicle->route[visited] = nextDestination;
                visited_destinations[nextIndex] = 1;
                currentPosition = nextDestination;
                visited++;
            }
            else
            {
                // Si aucune destination n'est directement accessible, on pourrait implémenter
                // un algorithme du plus court chemin comme Dijkstra ici
                // printf("Erreur : Impossible de trouver un chemin complet pour le véhicule %d\n", vehicle->id);
                break;
            }
        }

        // Ajouter le retour au point de départ
        vehicle->route[vehicle->packageCount] = vehicle->location;

        // Afficher la route planifiée
        // printf("Route planifiée pour le véhicule %d: ", vehicle->id);
        // for (int i = 0; i < vehicle->routeLength; i++)
        // {
        //     printf("%d ", vehicle->route[i]);
        //     if (i < vehicle->routeLength - 1)
        //         printf("-> ");
        // }
        // printf("\n");

        // Libérer la mémoire
        free(destinations);
        free(visited_destinations);
    }
}

// 3. Algorithme glouton pour la redistribution dynamique en cas d'imprévu
int greedyDynamicReallocation(Graph *graph, Vehicle *vehicles, int vehicleCount, int failedVehicleId, Package *packages, int packageCount)
{
    // Trouver l'index du véhicule en panne
    int failedIndex = -1;
    for (int i = 0; i < vehicleCount; i++)
    {
        if (vehicles[i].id == failedVehicleId)
        {
            failedIndex = i;
            break;
        }
    }

    // Si le véhicule n'est pas trouvé
    if (failedIndex == -1)
    {
        printf("Véhicule %d non trouvé\n", failedVehicleId);
        return 0;
    }

    // Récupérer les colis du véhicule en panne
    Vehicle *failedVehicle = &vehicles[failedIndex];
    int *packagesToReallocate = failedVehicle->packageIds;
    int packagesToReallocateCount = failedVehicle->packageCount;

    // Créer une liste temporaire de colis à réaffecter
    Package *tmpPackages = (Package *)malloc(packagesToReallocateCount * sizeof(Package));
    for (int i = 0; i < packagesToReallocateCount; i++)
    {
        // Trouver le colis correspondant dans la liste complète
        int packageId = packagesToReallocate[i];
        for (int j = 0; j < packageCount; j++)
        {
            if (packages[j].id == packageId)
            {
                tmpPackages[i] = packages[j];
                break;
            }
        }
    }

    // Réinitialiser le véhicule en panne
    failedVehicle->packageCount = 0;
    failedVehicle->currentLoad = 0;
    free(failedVehicle->packageIds);
    failedVehicle->packageIds = NULL;

    // Créer une liste temporaire de véhicules disponibles (excluant celui en panne)
    Vehicle *availableVehicles = (Vehicle *)malloc((vehicleCount - 1) * sizeof(Vehicle));
    int availableCount = 0;
    for (int i = 0; i < vehicleCount; i++)
    {
        if (i != failedIndex)
        {
            availableVehicles[availableCount++] = vehicles[i];
        }
    }

    // Réaffecter les colis en utilisant l'algorithme d'affectation glouton
    int success = greedyPackageAssignment(tmpPackages, packagesToReallocateCount, availableVehicles, availableCount);

    // Mettre à jour les véhicules d'origine avec les nouvelles affectations
    for (int i = 0, j = 0; i < vehicleCount; i++)
    {
        if (i != failedIndex)
        {
            vehicles[i] = availableVehicles[j++];
        }
    }

    // Libérer la mémoire
    free(tmpPackages);
    free(availableVehicles);

    // Replanifier les tournées pour tous les véhicules
    if (success)
    {
        printf("Redistribution des colis réussie. Replanification des tournées...\n");
        greedyRouteScheduling(graph, vehicles, vehicleCount);
        return 1;
    }
    else
    {
        printf("Impossible de redistribuer tous les colis\n");
        return 0;
    }
}

// Fonctions d'utilitaire
Vehicle *createVehicles(int count, int defaultCapacity)
{
    Vehicle *vehicles = (Vehicle *)malloc(count * sizeof(Vehicle));
    for (int i = 0; i < count; i++)
    {
        vehicles[i].id = i + 1; // IDs commencent à 1
        vehicles[i].capacity = defaultCapacity;
        vehicles[i].currentLoad = 0;
        vehicles[i].location = 0; // Tous les véhicules commencent au nœud 0
        vehicles[i].packageIds = NULL;
        vehicles[i].packageCount = 0;
        vehicles[i].route = NULL;
        vehicles[i].routeLength = 0;
    }
    return vehicles;
}

Package *createPackages(int count, int maxWeight, int maxDestination)
{
    if (maxDestination <= 0)
    {
        printf("Erreur : maxDestination doit être positif\n");
        return NULL;
    }

    Package *packages = (Package *)malloc(count * sizeof(Package));
    for (int i = 0; i < count; i++)
    {
        packages[i].id = i + 1;                       // IDs commencent à 1
        packages[i].weight = rand() % maxWeight + 1;  // Poids entre 1 et maxWeight
        packages[i].source = rand() % maxDestination; // Source entre 0 et maxDestination-1

        // S'assurer que la destination est valide et différente de la source
        do
        {
            packages[i].destination = rand() % maxDestination; // Destination entre 0 et maxDestination-1
        } while (packages[i].destination == packages[i].source && maxDestination > 1);

        packages[i].priority = rand() % 3 + 1; // Priorité entre 1 (haute) et 3 (basse)

        // Afficher les détails pour le débogage
        printf("Création du colis ID=%d, source=%d, dest=%d, poids=%d, priorité=%d\n",
               packages[i].id, packages[i].source, packages[i].destination,
               packages[i].weight, packages[i].priority);
    }
    return packages;
}

void freeVehicles(Vehicle *vehicles, int count)
{
    for (int i = 0; i < count; i++)
    {
        free(vehicles[i].packageIds);
        free(vehicles[i].route);
    }
    free(vehicles);
}

void freePackages(Package *packages, int count)
{
    free(packages);
}

void printVehicleStatus(Vehicle *vehicle)
{
    printf("Véhicule %d - Capacité: %d, Charge: %d, Colis: %d, Position: %d\n",
           vehicle->id, vehicle->capacity, vehicle->currentLoad, vehicle->packageCount, vehicle->location);
    if (vehicle->packageCount > 0)
    {
        printf("  Colis: ");
        for (int i = 0; i < vehicle->packageCount; i++)
        {
            printf("%d ", vehicle->packageIds[i]);
        }
        printf("\n");
    }
    if (vehicle->routeLength > 0)
    {
        printf("  Route: ");
        for (int i = 0; i < vehicle->routeLength; i++)
        {
            printf("%d ", vehicle->route[i]);
            if (i < vehicle->routeLength - 1)
                printf("-> ");
        }
        printf("\n");
    }
}

void printAllVehicleStatus(Vehicle *vehicles, int count)
{
    printf("=== Statut des véhicules ===\n");
    for (int i = 0; i < count; i++)
    {
        printVehicleStatus(&vehicles[i]);
    }
    printf("==========================\n");
}