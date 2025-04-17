#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
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
#include <sys/resource.h> // Pour l'utilisation de la mémoire
#include "main.h"

// Définition des noms des datasets pour un affichage plus ergonomique
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

const char *dataset_names[] = {
    "Petit réseau - Condition normale",
    "Petit réseau - Heure de pointe",
    "Petit réseau - Condition de crise",
    "Réseau moyen - Condition normale",
    "Réseau moyen - Heure de pointe",
    "Réseau moyen - Condition de crise",
    "Grand réseau - Condition normale",
    "Grand réseau - Heure de pointe",
    "Grand réseau - Condition de crise"};

// Fonction utilitaire pour compter les arêtes dans un graphe
int countEdges(Graph *graph)
{
    int count = 0;
    for (int i = 0; i < graph->V; i++)
    {
        AdjListNode *current = graph->array[i].head;
        while (current)
        {
            count++;
            current = current->next;
        }
    }
    return count;
}

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

/**
 * @brief Vider le buffer d'entrée
 * Fonction utilitaire pour nettoyer le tampon d'entrée après saisie
 */
void flushInputBuffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

/**
 * @brief Obtenir une entrée entière sécurisée
 * Lit une entrée utilisateur et s'assure qu'il s'agit d'un entier valide
 * @return Le nombre entier entré par l'utilisateur, -1 en cas d'erreur
 */
int getValidIntegerInput()
{
    char buffer[MAX_INPUT_SIZE];
    if (fgets(buffer, sizeof(buffer), stdin) == NULL)
    {
        return -1;
    }

    // Supprimer le caractère de nouvelle ligne
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n')
    {
        buffer[len - 1] = '\0';
    }

    // Vérifier que tous les caractères sont des chiffres
    for (size_t i = 0; i < strlen(buffer); i++)
    {
        if (!isdigit((unsigned char)buffer[i]))
        {
            return -1;
        }
    }

    // Convertir en entier
    char *endptr;
    long val = strtol(buffer, &endptr, 10);

    // Vérifier les erreurs de conversion
    if (*endptr != '\0' || val < 0 || val > INT_MAX)
    {
        return -1;
    }

    return (int)val;
}

/**
 * @brief Afficher le menu pour sélectionner un dataset
 * @return L'index du dataset choisi ou -1 en cas d'annulation
 */
int showDatasetSelectionMenu()
{
    int numDatasets = sizeof(datasets) / sizeof(datasets[0]);
    int choice;

    while (1)
    {
        CLEAR_SCREEN();
        printf("\n\n");
        printf("╔════════════════════════════════════════════════╗\n");
        printf("║             SÉLECTION DU DATASET                ║\n");
        printf("╠════════════════════════════════════════════════╣\n");

        for (int i = 0; i < numDatasets; i++)
        {
            printf("║ %2d. %-42s ║\n", i + 1, dataset_names[i]);
        }

        printf("║ %2d. Retour au menu précédent                   ║\n", numDatasets + 1);
        printf("╚════════════════════════════════════════════════╝\n\n");

        printf("Entrez votre choix (1-%d): ", numDatasets + 1);
        choice = getValidIntegerInput();

        if (choice >= 1 && choice <= numDatasets)
        {
            return choice - 1;
        }
        else if (choice == numDatasets + 1)
        {
            return -1; // Retour au menu précédent
        }
        else
        {
            printf("\nChoix invalide. Appuyez sur Entrée pour réessayer...");
            flushInputBuffer();
            getchar();
        }
    }
}

/**
 * @brief Exécuter l'algorithme sélectionné sur le dataset choisi
 * @param algorithmFunc Pointeur de fonction vers l'algorithme à exécuter
 * @param algorithmName Nom de l'algorithme pour l'affichage
 */
void runAlgorithmOnDataset(void (*algorithmFunc)(Graph *), const char *algorithmName)
{
    int datasetIndex = showDatasetSelectionMenu();
    if (datasetIndex == -1)
    {
        return; // L'utilisateur a annulé
    }

    CLEAR_SCREEN();
    printf("\n╔════════════════════════════════════════════════╗\n");
    printf("║          EXÉCUTION DE L'ALGORITHME             ║\n");
    printf("╠════════════════════════════════════════════════╣\n");
    printf("║ Algorithme: %-33s ║\n", algorithmName);
    printf("║ Dataset: %-36s ║\n", dataset_names[datasetIndex]);
    printf("╚════════════════════════════════════════════════╝\n\n");

    printf("Chargement du graphe depuis %s...\n", datasets[datasetIndex]);
    Graph *graph = loadGraphFromJSON(datasets[datasetIndex]);

    if (!graph)
    {
        printf("\nÉchec du chargement du graphe depuis %s\n", datasets[datasetIndex]);
        printf("\nAppuyez sur Entrée pour revenir au menu principal...");
        getchar();
        return;
    }

    printf("\nGraphe chargé avec succès : %d nœuds et %d arêtes.\n\n", graph->V, countEdges(graph));
    printf("Exécution de %s en cours...\n", algorithmName);

    // Mesurer les performances
    long memoryBefore = getMemoryUsage();
    clock_t start = clock();

    // Exécuter l'algorithme
    algorithmFunc(graph);

    // Calculer les métriques
    clock_t end = clock();
    double executionTime = ((double)(end - start)) / CLOCKS_PER_SEC;
    long memoryAfter = getMemoryUsage();

    printf("\n╔════════════════════════════════════════════════╗\n");
    printf("║              RÉSULTATS D'EXÉCUTION             ║\n");
    printf("╠════════════════════════════════════════════════╣\n");
    printf("║ Temps d'exécution : %-26.6f ║\n", executionTime);
    printf("║ Mémoire utilisée  : %-26ld ║\n", memoryAfter - memoryBefore);
    printf("╚════════════════════════════════════════════════╝\n\n");

    freeGraph(graph);

    printf("Appuyez sur Entrée pour revenir au menu principal...");
    getchar();
}

/**
 * @brief Générer un rapport complet en comparant tous les algorithmes sur tous les datasets
 * Cette fonction exécute le programme report généré par rapport.c
 */
void generateFullReport()
{
    CLEAR_SCREEN();
    printf("\n╔════════════════════════════════════════════════╗\n");
    printf("║              GÉNÉRATION DU RAPPORT             ║\n");
    printf("╚════════════════════════════════════════════════╝\n\n");

    printf("Exécution du programme de rapport...\n\n");

    // Exécuter le programme report via system()
    int result = system("./report");

    if (result != 0)
    {
        printf("\nÉchec de l'exécution du programme de rapport (code %d).\n", result);
    }
    else
    {
        printf("\nRapport généré avec succès!\n");
    }

    printf("\nAppuyez sur Entrée pour revenir au menu principal...");
    getchar();
}

/**
 * @brief Afficher le menu pour sélectionner un algorithme
 */
void showAlgorithmSelectionMenu()
{
    // Définir les options d'algorithmes disponibles
    AlgorithmOption algorithms[] = {
        {"BFS (Parcours en largeur)", BFSWrapper, "Explore le graphe niveau par niveau"},
        {"DFS (Parcours en profondeur)", DFSWrapper, "Explore le graphe en descendant aussi loin que possible"},
        {"Floyd-Warshall", FloydWarshallWrapper, "Calcule les plus courts chemins entre toutes les paires de sommets"},
        {"Bellman-Ford", BellmanFordWrapper, "Calcule les plus courts chemins à partir d'un sommet source"},
        {"TSP (Problème du voyageur de commerce)", TSPWrapper, "Trouve le circuit hamiltonien de poids minimum"},
        {"Algorithme génétique", GeneticAlgorithmWrapper, "Optimise les routes de livraison par approche évolutionnaire"},
        {"Planification multi-jours", MultiDayPlanningWrapper, "Planifie les livraisons sur plusieurs jours"}};

    int numAlgorithms = sizeof(algorithms) / sizeof(algorithms[0]);
    int choice;

    while (1)
    {
        CLEAR_SCREEN();
        printf("\n\n");
        printf("╔════════════════════════════════════════════════════════════════════════╗\n");
        printf("║                     SÉLECTION DE L'ALGORITHME                          ║\n");
        printf("╠════════════════════════════════════════════════════════════════════════╣\n");

        for (int i = 0; i < numAlgorithms; i++)
        {
            printf("║ %2d. %-62s ║\n", i + 1, algorithms[i].name);
            printf("║    ► %-64s ║\n", algorithms[i].description);
        }

        printf("║ %2d. Revenir au menu principal                                         ║\n", numAlgorithms + 1);
        printf("╚════════════════════════════════════════════════════════════════════════╝\n\n");

        printf("Entrez votre choix (1-%d): ", numAlgorithms + 1);
        choice = getValidIntegerInput();

        if (choice >= 1 && choice <= numAlgorithms)
        {
            runAlgorithmOnDataset(algorithms[choice - 1].function, algorithms[choice - 1].name);
        }
        else if (choice == numAlgorithms + 1)
        {
            return; // Retour au menu principal
        }
        else
        {
            printf("\nChoix invalide. Appuyez sur Entrée pour réessayer...");
            flushInputBuffer();
            getchar();
        }
    }
}

/**
 * @brief Afficher le menu principal
 */
void showMainMenu()
{
    int choice;

    while (1)
    {
        CLEAR_SCREEN();
        printf("\n\n");
        printf("╔════════════════════════════════════════════════╗\n");
        printf("║      SYSTÈME D'ANALYSE D'ALGORITHMES           ║\n");
        printf("║              DE GRAPHES v1.0                   ║\n");
        printf("╠════════════════════════════════════════════════╣\n");
        printf("║                                                ║\n");
        printf("║  1. Expérimentation et analyse                 ║\n");
        printf("║  2. Générer un rapport complet                 ║\n");
        printf("║  3. Quitter                                    ║\n");
        printf("║                                                ║\n");
        printf("╚════════════════════════════════════════════════╝\n\n");

        printf("Entrez votre choix (1-3): ");
        choice = getValidIntegerInput();

        switch (choice)
        {
        case 1:
            showAlgorithmSelectionMenu();
            break;
        case 2:
            generateFullReport();
            break;
        case 3:
            CLEAR_SCREEN();
            printf("\nMerci d'avoir utilisé le système d'analyse d'algorithmes de graphes!\n\n");
            return;
        default:
            printf("\nChoix invalide. Appuyez sur Entrée pour réessayer...");
            flushInputBuffer();
            getchar();
        }
    }
}

// Fonction principale
int main()
{
    // Initialiser le générateur de nombres aléatoires
    srand((unsigned int)time(NULL));

    // Éviter que l'entrée ne soit automatiquement passée au menu
    setbuf(stdout, NULL);

    // Afficher le menu principal
    showMainMenu();

    return 0;
}