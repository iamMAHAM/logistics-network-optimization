#ifndef GREEDY_ALGORITHMS_H
#define GREEDY_ALGORITHMS_H

#include "../core/graph.h"

// Structure pour représenter un colis
typedef struct Package
{
    int id;
    int source;
    int weight;
    int destination;
    int priority; // 1: haute, 2: moyenne, 3: basse
} Package;

// Structure pour représenter un véhicule
typedef struct Vehicle
{
    int id;
    int capacity;
    int currentLoad;
    int location;
    int *packageIds;  // IDs des colis affectés
    int packageCount; // Nombre de colis affectés
    int *route;       // Route planifiée
    int routeLength;  // Nombre de nœuds dans la route
} Vehicle;

// 1. Affectation des colis aux véhicules (approche gloutonne)
/**
 * Affecte les colis aux véhicules en utilisant une approche gloutonne.
 * Les colis de priorité élevée sont affectés d'abord, suivis par ceux de priorité moyenne et basse.
 *
 * @param packages Liste des colis à affecter
 * @param packageCount Nombre de colis
 * @param vehicles Liste des véhicules disponibles
 * @param vehicleCount Nombre de véhicules
 * @return 1 si tous les colis ont été affectés, 0 sinon
 */
int greedyPackageAssignment(Package *packages, int packageCount, Vehicle *vehicles, int vehicleCount);

// 2. Planification des tournées dans une journée (approche gloutonne)
/**
 * Planifie les tournées pour chaque véhicule en utilisant une approche gloutonne.
 * Chaque véhicule suit un chemin qui minimise la distance totale pour livrer tous ses colis.
 *
 * @param graph Graphe représentant le réseau
 * @param vehicles Liste des véhicules à planifier
 * @param vehicleCount Nombre de véhicules
 */
void greedyRouteScheduling(Graph *graph, Vehicle *vehicles, int vehicleCount);

// 3. Redistribution dynamique en cas d'imprévu (approche gloutonne)
/**
 * Redistribue les colis entre les véhicules en cas d'imprévu (panne, retard, etc.).
 *
 * @param graph Graphe représentant le réseau
 * @param vehicles Liste des véhicules
 * @param vehicleCount Nombre de véhicules
 * @param failedVehicleId ID du véhicule en panne ou en retard
 * @param packages Liste complète des colis
 * @param packageCount Nombre total de colis
 * @return 1 si la redistribution a réussi, 0 sinon
 */
int greedyDynamicReallocation(Graph *graph, Vehicle *vehicles, int vehicleCount, int failedVehicleId, Package *packages, int packageCount);

// Fonctions d'utilitaire
Vehicle *createVehicles(int count, int defaultCapacity);
Package *createPackages(int count, int maxWeight, int maxDestination);
void freeVehicles(Vehicle *vehicles, int count);
void freePackages(Package *packages, int count);
void printVehicleStatus(Vehicle *vehicle);
void printAllVehicleStatus(Vehicle *vehicles, int count);

#endif // GREEDY_ALGORITHMS_H