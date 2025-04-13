#ifndef GENETIC_ALGORITHM_H
#define GENETIC_ALGORITHM_H

#include <stdbool.h>
#include "../core/graph.h"
#include "greedy_algorithms.h" // Include this to use shared Vehicle and Package structs

// Structure pour représenter une route dans un chromosome
typedef struct
{
    int vehicleId;      // ID du véhicule associé à cette route
    int *nodeSequence;  // Séquence des noeuds à visiter
    int sequenceLength; // Longueur de la séquence
    double distance;    // Distance totale de la route
    double time;        // Temps total pour parcourir la route
    double cost;        // Coût total de la route
    bool isValid;       // Indique si la route est valide (respecte les contraintes)
} ChromosomeRoute;

// Structure pour représenter un chromosome (solution complète)
typedef struct
{
    ChromosomeRoute *routes; // Tableau des routes pour chaque véhicule
    int routeCount;          // Nombre de routes (= nombre de véhicules)
    double fitnessValue;     // Valeur de fitness
    double totalDistance;    // Distance totale
    double totalTime;        // Temps total
    double totalCost;        // Coût total
    int unservedPackages;    // Nombre de colis non livrés
    bool isValid;            // Indique si la solution est valide
} Chromosome;

// Structure pour représenter une population
typedef struct
{
    Chromosome *individuals;           // Tableau des chromosomes
    int capacity;                      // Capacité maximale de la population
    int size;                          // Taille actuelle de la population
    Chromosome bestEver;               // Meilleur chromosome trouvé jusqu'à présent
    double avgFitness;                 // Fitness moyenne de la population
    int generationsWithoutImprovement; // Nombre de générations sans amélioration
} Population;

// Structure pour configurer l'algorithme génétique
typedef struct
{
    int populationSize;   // Taille de la population
    int maxGenerations;   // Nombre maximal de générations
    double crossoverRate; // Taux de croisement
    double mutationRate;  // Taux de mutation
    int eliteCount;       // Nombre d'élites préservés
    int tournamentSize;   // Taille du tournoi pour la sélection
    bool adaptiveParams;  // Si les paramètres doivent être adaptés au fil du temps
    int stagnationLimit;  // Limite de générations sans amélioration avant adaptation
} GAConfig;

// Fonctions principales

// Initialisation de la configuration de l'algorithme génétique
GAConfig initGAConfig(int populationSize, int maxGenerations, double crossoverRate,
                      double mutationRate, int eliteCount, int tournamentSize,
                      bool adaptiveParams, int stagnationLimit);

// Création de la population initiale
Population createInitialPopulation(GAConfig config, Graph *graph, Vehicle *vehicles,
                                   int vehicleCount, Package *packages, int packageCount);

// Évaluation de la fitness d'un chromosome
void evaluateFitness(Chromosome *chromosome, Graph *graph, Vehicle *vehicles,
                     int vehicleCount, Package *packages, int packageCount);

// Sélection par tournoi
Chromosome *tournamentSelection(Population *population, int tournamentSize);

// Croisement adapté
void crossover(Chromosome *parent1, Chromosome *parent2, Chromosome *offspring1,
               Chromosome *offspring2, double crossoverRate);

// Mutation intelligente
void mutate(Chromosome *chromosome, double mutationRate, Graph *graph);

// Clonage d'un chromosome
Chromosome cloneChromosome(Chromosome *source);

// Mise à jour de la population avec la nouvelle génération
void updatePopulation(Population *population, Chromosome *newGeneration, int eliteCount);

// Adaptation des paramètres génétiques
void adaptParameters(GAConfig *config, Population *population);

// Recherche locale pour améliorer une solution
void localSearch(Chromosome *chromosome, Graph *graph);

// Affichage d'un chromosome
void printChromosome(Chromosome *chromosome);

// Affichage de la meilleure solution
void printBestSolution(Population *population);

// Libération de la mémoire d'un chromosome
void freeChromosome(Chromosome *chromosome);

// Libération de la mémoire d'une population
void freePopulation(Population *population);

// Exécution de l'algorithme génétique
Chromosome runGeneticAlgorithm(Graph *graph, Vehicle *vehicles, int vehicleCount,
                               Package *packages, int packageCount, GAConfig config);

#endif // GENETIC_ALGORITHM_H