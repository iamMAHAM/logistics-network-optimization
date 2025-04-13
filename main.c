#include "core/graph.h"
#include "core/temporal_variations.h"
#include "network/parser.h"
#include "algorithms/dfs.h"
#include "algorithms/bfs.h"
#include "algorithms/graph_analysis.h"
#include "algorithms/floyd_warshall.h"
#include "algorithms/bellman_ford.h"
#include "algorithms/tsp.h"
#include "algorithms/multi_day_planning.h"
#include "algorithms/greedy_algorithms.h"
#include "algorithms/genetic_algorithm.h"
#include <float.h>
#include <limits.h>
#include <time.h>

// Fonction de test pour démontrer l'algorithme génétique
void testGeneticAlgorithm(Graph *graph)
{
    printf("\n=== Test de l'algorithme génétique ===\n");

    // Initialiser le générateur de nombres aléatoires avec l'heure actuelle
    srand(time(NULL));

    // Créer des véhicules et des colis
    int vehicleCount = 5;
    int packageCount = 20;
    int maxWeight = 50;
    int defaultCapacity = 150;

    Vehicle *vehicles = createVehicles(vehicleCount, defaultCapacity);
    Package *packages = createPackages(packageCount, maxWeight, graph->V);

    printf("Initialisation de %d véhicules et %d colis\n", vehicleCount, packageCount);

    // Afficher les détails des véhicules et des colis
    printf("\nDétails des véhicules:\n");
    for (int i = 0; i < vehicleCount; i++)
    {
        printf("Véhicule %d: capacité=%d, location=%d\n",
               vehicles[i].id, vehicles[i].capacity, vehicles[i].location);
    }

    printf("\nDétails des colis:\n");
    for (int i = 0; i < packageCount; i++)
    {
        printf("Colis %d: source=%d, dest=%d, poids=%d, priorité=%d\n",
               packages[i].id, packages[i].source, packages[i].destination,
               packages[i].weight, packages[i].priority);
    }

    // Assigner des colis aux véhicules
    printf("\nAssignation des colis aux véhicules:\n");
    int allAssigned = greedyPackageAssignment(packages, packageCount, vehicles, vehicleCount);
    printf("Tous les colis ont été affectés: %s\n", allAssigned ? "Oui" : "Non");

    // Afficher le statut des véhicules après l'affectation
    printAllVehicleStatus(vehicles, vehicleCount);

    // Configurer l'algorithme génétique
    GAConfig config = initGAConfig(
        50,   // populationSize
        100,  // maxGenerations
        0.8,  // crossoverRate
        0.2,  // mutationRate
        5,    // eliteCount
        5,    // tournamentSize
        true, // adaptiveParams
        20    // stagnationLimit
    );

    printf("\nConfiguration de l'algorithme génétique:\n");
    printf("Taille de population: %d\n", config.populationSize);
    printf("Générations max: %d\n", config.maxGenerations);
    printf("Taux de croisement: %.2f\n", config.crossoverRate);
    printf("Taux de mutation: %.2f\n", config.mutationRate);

    // Exécuter l'algorithme génétique
    printf("\nExécution de l'algorithme génétique...\n");
    Chromosome bestSolution = runGeneticAlgorithm(graph, vehicles, vehicleCount, packages, packageCount, config);

    // Afficher les résultats
    printf("\nMeilleure solution trouvée:\n");
    printChromosome(&bestSolution);

    // Afficher des statistiques supplémentaires
    printf("\nStatistiques de la solution:\n");
    printf("Distance totale: %.2f\n", bestSolution.totalDistance);
    printf("Temps total: %.2f\n", bestSolution.totalTime);
    printf("Coût total: %.2f\n", bestSolution.totalCost);
    printf("Colis non livrés: %d\n", bestSolution.unservedPackages);
    printf("Solution valide: %s\n", bestSolution.isValid ? "Oui" : "Non");

    // Libérer la mémoire
    freeVehicles(vehicles, vehicleCount);
    freePackages(packages, packageCount);
    freeChromosome(&bestSolution);

    printf("=== Fin du test de l'algorithme génétique ===\n");
}

// Fonction de test pour démontrer l'approche gloutonne
void testGreedyAlgorithms(Graph *graph)
{
    printf("\n=== Test des algorithmes gloutons ===\n");

    // Initialiser le générateur de nombres aléatoires
    srand(time(NULL));

    // Créer des véhicules et des colis
    int vehicleCount = 3;
    int packageCount = 10;
    int maxWeight = 50;
    int defaultCapacity = 150;

    Vehicle *vehicles = createVehicles(vehicleCount, defaultCapacity);
    Package *packages = createPackages(packageCount, maxWeight, graph->V);

    printf("Initialisation de %d véhicules et %d colis\n", vehicleCount, packageCount);

    // 1. Affectation des colis aux véhicules
    printf("\n1. Affectation des colis aux véhicules\n");
    int allAssigned = greedyPackageAssignment(packages, packageCount, vehicles, vehicleCount);
    printf("Tous les colis ont été affectés: %s\n", allAssigned ? "Oui" : "Non");

    // Afficher le statut des véhicules après l'affectation
    printAllVehicleStatus(vehicles, vehicleCount);

    // 2. Planification des tournées
    printf("\n2. Planification des tournées\n");
    greedyRouteScheduling(graph, vehicles, vehicleCount);

    // Afficher le statut des véhicules après la planification
    printAllVehicleStatus(vehicles, vehicleCount);

    // 3. Simulation d'un imprévu: panne d'un véhicule
    printf("\n3. Simulation d'un imprévu: panne du véhicule %d\n", vehicles[0].id);
    int reallocSuccess = greedyDynamicReallocation(graph, vehicles, vehicleCount, vehicles[0].id, packages, packageCount);

    printf("Redistribution des colis: %s\n", reallocSuccess ? "Réussie" : "Échouée");

    // Afficher le statut final des véhicules
    printAllVehicleStatus(vehicles, vehicleCount);

    // Libérer la mémoire
    freeVehicles(vehicles, vehicleCount);
    freePackages(packages, packageCount);

    printf("=== Fin du test des algorithmes gloutons ===\n");
}

int main()
{
    // Exemple de fichier JSON attendu pour décrire le réseau
    const char *json_filename = "network/good_voyager.json";

    // Charger le graphe à partir du fichier JSON
    Graph *graph = loadGraphFromJSON(json_filename);
    if (graph == NULL)
    {
        printf("Erreur lors du chargement du graphe depuis le fichier JSON.\n");
        return 1;
    }

    // Afficher le graphe chargé
    printf("Graphe chargé depuis %s :\n", json_filename);

    // Afficher le graphe sous forme de listes d'adjacence
    printf("\nAffichage du graphe sous forme de listes d'adjacence :\n");
    displayAdjacencyList(graph);

    // Exemple de parcours en profondeur (DFS)
    printf("\nExemple de parcours en profondeur (DFS) à partir du sommet 0 :\n");
    DFS(graph, 0);

    // Exemple de parcours en largeur (BFS)
    printf("\nExemple de parcours en largeur (BFS) à partir du sommet 0 :\n");
    BFS(graph, 0);

    // Test de la détection des cycles
    printf("\nTest de la détection des cycles :\n");
    if (detectCycle(graph))
    {
        printf("Le graphe contient un cycle.\n");
    }
    else
    {
        printf("Le graphe ne contient pas de cycle.\n");
    }

    // Test de l'identification des composantes connexes
    printf("\nTest de l'identification des composantes connexes :\n");
    findConnectedComponents(graph);

    // Test de l'accessibilité entre deux nœuds
    printf("\nTest de l'accessibilité entre les nœuds 0 et 3 :\n");
    if (isAccessible(graph, 0, 3))
    {
        printf("Le nœud 3 est accessible depuis le nœud 0.\n");
    }
    else
    {
        printf("Le nœud 3 n'est pas accessible depuis le nœud 0.\n");
    }

    // Test du calcul des statistiques de connectivité
    printf("\nTest du calcul des statistiques de connectivité :\n");
    calculateConnectivityStats(graph);

    // Test de l'algorithme de Floyd-Warshall
    printf("\nTest de l'algorithme de Floyd-Warshall :\n");
    double **graphMatrix = (double **)malloc(graph->V * sizeof(double *));
    for (int i = 0; i < graph->V; i++)
    {
        graphMatrix[i] = (double *)malloc(graph->V * sizeof(double));
        for (int j = 0; j < graph->V; j++)
        {
            if (i == j)
            {
                graphMatrix[i][j] = 0;
            }
            else
            {
                graphMatrix[i][j] = DBL_MAX;
            }
        }
    }

    for (int v = 0; v < graph->V; v++)
    {
        AdjListNode *current = graph->array[v].head;
        while (current)
        {
            graphMatrix[v][current->dest] = current->attr.distance;
            current = current->next;
        }
    }

    floydWarshall(graphMatrix, graph->V);

    for (int i = 0; i < graph->V; i++)
    {
        free(graphMatrix[i]);
    }
    free(graphMatrix);

    // Test de l'algorithme de Bellman-Ford
    printf("\nTest de l'algorithme de Bellman-Ford :\n");
    double *dist = (double *)malloc(graph->V * sizeof(double));
    int *predecessor = (int *)malloc(graph->V * sizeof(int));

    if (bellmanFord(graph, 0, dist, predecessor))
    {
        printf("Distances depuis le sommet 0 :\n");
        for (int i = 0; i < graph->V; i++)
        {
            printf("Sommet %d : %.2f\n", i, dist[i]);
        }
    }
    else
    {
        printf("Un cycle de poids négatif a été détecté dans le graphe.\n");
    }

    free(dist);
    free(predecessor);

    // Test du problème du voyageur de commerce (TSP)
    printf("\nTest du problème du voyageur de commerce (TSP) :\n");
    int **tspGraph = (int **)malloc(graph->V * sizeof(int *));
    for (int i = 0; i < graph->V; i++)
    {
        tspGraph[i] = (int *)malloc(graph->V * sizeof(int));
        for (int j = 0; j < graph->V; j++)
        {
            if (i == j)
            {
                tspGraph[i][j] = 0;
            }
            else
            {
                tspGraph[i][j] = INT_MAX;
            }
        }
    }

    for (int v = 0; v < graph->V; v++)
    {
        AdjListNode *current = graph->array[v].head;
        while (current)
        {
            tspGraph[v][current->dest] = (int)current->attr.cost;
            current = current->next;
        }
    }

    // Afficher la matrice TSP pour débogage
    printf("\nMatrice TSP :\n");
    for (int i = 0; i < graph->V; i++)
    {
        for (int j = 0; j < graph->V; j++)
        {
            if (tspGraph[i][j] == INT_MAX)
            {
                printf("INF ");
            }
            else
            {
                printf("%d ", tspGraph[i][j]);
            }
        }
        printf("\n");
    }

    solveTSP(tspGraph, graph->V);

    for (int i = 0; i < graph->V; i++)
    {
        free(tspGraph[i]);
    }
    free(tspGraph);

    // Test de la planification multi-jours des livraisons
    printf("\nTest de la planification multi-jours des livraisons :\n");
    int days = 3; // Exemple : planification sur 3 jours
    int **costMatrix = (int **)malloc(graph->V * sizeof(int *));
    for (int i = 0; i < graph->V; i++)
    {
        costMatrix[i] = (int *)malloc(graph->V * sizeof(int));
        for (int j = 0; j < graph->V; j++)
        {
            if (i == j)
            {
                costMatrix[i][j] = 0;
            }
            else
            {
                costMatrix[i][j] = INT_MAX;
            }
        }
    }

    for (int v = 0; v < graph->V; v++)
    {
        AdjListNode *current = graph->array[v].head;
        while (current)
        {
            costMatrix[v][current->dest] = (int)current->attr.cost;
            current = current->next;
        }
    }

    // Afficher la matrice des coûts pour la planification multi-jours
    printf("\nMatrice des coûts pour la planification multi-jours :\n");
    for (int i = 0; i < graph->V; i++)
    {
        for (int j = 0; j < graph->V; j++)
        {
            if (costMatrix[i][j] == INT_MAX)
            {
                printf("INF ");
            }
            else
            {
                printf("%d ", costMatrix[i][j]);
            }
        }
        printf("\n");
    }

    multiDayDeliveryPlanning(costMatrix, graph->V, days);

    for (int i = 0; i < graph->V; i++)
    {
        free(costMatrix[i]);
    }
    free(costMatrix);

    // Test des algorithmes gloutons
    testGreedyAlgorithms(graph);

    // Test de l'algorithme génétique
    testGeneticAlgorithm(graph);

    // Sauvegarder le graphe dans un autre fichier JSON
    const char *output_filename = "output_network.json";
    saveGraphToJSON(graph, output_filename);
    printf("Graphe sauvegardé dans %s.\n", output_filename);

    // Libération de la mémoire
    freeGraph(graph);

    return 0;
}