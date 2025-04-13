#include "genetic_algorithm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <time.h>

// Initialiser la configuration de l'algorithme génétique
GAConfig initGAConfig(int populationSize, int maxGenerations, double crossoverRate,
                      double mutationRate, int eliteCount, int tournamentSize,
                      bool adaptiveParams, int stagnationLimit)
{
    GAConfig config;
    config.populationSize = populationSize;
    config.maxGenerations = maxGenerations;
    config.crossoverRate = crossoverRate;
    config.mutationRate = mutationRate;
    config.eliteCount = eliteCount;
    config.tournamentSize = tournamentSize;
    config.adaptiveParams = adaptiveParams;
    config.stagnationLimit = stagnationLimit;
    return config;
}

// Générer un nombre aléatoire entre min et max
static int randomInt(int min, int max)
{
    return min + rand() % (max - min + 1);
}

// Générer un nombre aléatoire entre 0 et 1
static double randomDouble()
{
    return (double)rand() / RAND_MAX;
}

// Initialiser un chromosome vide
static Chromosome initEmptyChromosome(int vehicleCount)
{
    Chromosome chromosome;
    chromosome.routeCount = vehicleCount;
    chromosome.routes = (ChromosomeRoute *)malloc(vehicleCount * sizeof(ChromosomeRoute));
    chromosome.fitnessValue = 0.0;
    chromosome.totalDistance = 0.0;
    chromosome.totalTime = 0.0;
    chromosome.totalCost = 0.0;
    chromosome.unservedPackages = 0;
    chromosome.isValid = false;

    for (int i = 0; i < vehicleCount; i++)
    {
        chromosome.routes[i].vehicleId = i + 1;
        chromosome.routes[i].nodeSequence = NULL;
        chromosome.routes[i].sequenceLength = 0;
        chromosome.routes[i].distance = 0.0;
        chromosome.routes[i].time = 0.0;
        chromosome.routes[i].cost = 0.0;
        chromosome.routes[i].isValid = false;
    }

    return chromosome;
}

// Générer une route aléatoire valide pour un véhicule
void generateRandomRoute(ChromosomeRoute *route, Graph *graph, Vehicle *vehicle, Package *packages, int packageCount)
{
    // Debug: Print vehicle info
    printf("Debug - Vehicle %d at location %d has %d packages assigned\n",
           vehicle->id, vehicle->location, vehicle->packageCount);

    // Déterminer les destinations possibles (nœuds où il y a des colis à livrer pour ce véhicule)
    int maxDestinations = 0;
    for (int i = 0; i < vehicle->packageCount; i++)
    {
        int packageId = vehicle->packageIds[i];
        printf("Debug - Vehicle %d has package ID %d assigned\n", vehicle->id, packageId);
        for (int j = 0; j < packageCount; j++)
        {
            if (packages[j].id == packageId)
            {
                maxDestinations++;
                printf("Debug - Package %d destination is node %d\n", packageId, packages[j].destination);
                break;
            }
        }
    }

    // Si aucun colis n'est assigné à ce véhicule, créer une route vide
    if (maxDestinations == 0)
    {
        printf("Debug - No destinations for vehicle %d, creating empty route\n", vehicle->id);
        route->nodeSequence = (int *)malloc(sizeof(int));
        route->nodeSequence[0] = vehicle->location; // Le véhicule reste à sa position initiale
        route->sequenceLength = 1;
        route->distance = 0.0;
        route->time = 0.0;
        route->cost = 0.0;
        route->isValid = true;
        return;
    }

    // Allouer de l'espace pour la séquence de nœuds (position initiale + destinations + retour)
    int maxNodes = maxDestinations + 2;
    route->nodeSequence = (int *)malloc(maxNodes * sizeof(int));

    // La route commence toujours à la position actuelle du véhicule
    route->nodeSequence[0] = vehicle->location;

    // Ajouter les nœuds de destination dans un ordre aléatoire
    int destinationCount = 0;
    int *destinations = (int *)malloc(maxDestinations * sizeof(int));

    // Récupérer les destinations des colis attribués à ce véhicule
    for (int i = 0; i < vehicle->packageCount; i++)
    {
        int packageId = vehicle->packageIds[i];
        for (int j = 0; j < packageCount; j++)
        {
            if (packages[j].id == packageId)
            {
                destinations[destinationCount++] = packages[j].destination;
                break;
            }
        }
    }

    // Mélanger les destinations aléatoirement (algorithme de Fisher-Yates)
    for (int i = destinationCount - 1; i > 0; i--)
    {
        int j = randomInt(0, i);
        int temp = destinations[i];
        destinations[i] = destinations[j];
        destinations[j] = temp;
    }

    // Ajouter les destinations mélangées à la séquence
    for (int i = 0; i < destinationCount; i++)
    {
        route->nodeSequence[i + 1] = destinations[i];
    }

    // Terminer la route en retournant à la position initiale
    route->nodeSequence[destinationCount + 1] = vehicle->location;
    route->sequenceLength = destinationCount + 2;

    free(destinations);

    // La route est considérée comme valide, mais sera évaluée plus précisément plus tard
    route->isValid = true;

    // Calculer la distance, le temps et le coût initiaux (approximatifs)
    route->distance = 0.0;
    route->time = 0.0;
    route->cost = 0.0;

    // Cette méthode simple est approximative; une évaluation plus précise sera faite dans evaluateFitness
}

// Créer une population initiale
Population createInitialPopulation(GAConfig config, Graph *graph, Vehicle *vehicles,
                                   int vehicleCount, Package *packages, int packageCount)
{
    Population population;
    population.capacity = config.populationSize;
    population.size = 0;
    population.individuals = (Chromosome *)malloc(config.populationSize * sizeof(Chromosome));
    population.avgFitness = 0.0;
    population.generationsWithoutImprovement = 0;

    // Initialiser bestEver comme un chromosome vide
    population.bestEver = initEmptyChromosome(vehicleCount);
    population.bestEver.fitnessValue = -DBL_MAX; // Commencer avec la pire fitness possible

    // Créer les individus de la population initiale
    for (int i = 0; i < config.populationSize; i++)
    {
        Chromosome chromosome = initEmptyChromosome(vehicleCount);

        // Générer des routes aléatoires pour chaque véhicule
        for (int j = 0; j < vehicleCount; j++)
        {
            generateRandomRoute(&chromosome.routes[j], graph, &vehicles[j], packages, packageCount);
        }

        // Évaluer la fitness de ce chromosome
        evaluateFitness(&chromosome, graph, vehicles, vehicleCount, packages, packageCount);

        // Ajouter le chromosome à la population
        population.individuals[i] = chromosome;
        population.size++;
    }

    return population;
}

// Calculer la distance entre deux nœuds dans le graphe
static double calculateDistance(Graph *graph, int source, int dest)
{
    if (source == dest)
        return 0.0;

    AdjListNode *current = graph->array[source].head;
    while (current)
    {
        if (current->dest == dest)
        {
            return current->attr.distance;
        }
        current = current->next;
    }

    // Si les nœuds ne sont pas directement connectés, retourner une grande valeur
    return DBL_MAX;
}

// Calculer le coût entre deux nœuds dans le graphe
static double calculateCost(Graph *graph, int source, int dest)
{
    if (source == dest)
        return 0.0;

    AdjListNode *current = graph->array[source].head;
    while (current)
    {
        if (current->dest == dest)
        {
            return current->attr.cost;
        }
        current = current->next;
    }

    // Si les nœuds ne sont pas directement connectés, retourner une grande valeur
    return DBL_MAX;
}

// Calculer le temps entre deux nœuds dans le graphe
static double calculateTime(Graph *graph, int source, int dest)
{
    if (source == dest)
        return 0.0;

    AdjListNode *current = graph->array[source].head;
    while (current)
    {
        if (current->dest == dest)
        {
            return current->attr.baseTime;
        }
        current = current->next;
    }

    // Si les nœuds ne sont pas directement connectés, retourner une grande valeur
    return DBL_MAX;
}

// Vérifier si une route est valide
bool isRouteValid(ChromosomeRoute *route, Graph *graph, Vehicle *vehicle, Package *packages, int packageCount)
{
    // Vérifier si la route commence et se termine à la position du véhicule
    if (route->nodeSequence[0] != vehicle->location ||
        route->nodeSequence[route->sequenceLength - 1] != vehicle->location)
    {
        return false;
    }

    // Vérifier si tous les nœuds sont connectés dans le graphe
    for (int i = 0; i < route->sequenceLength - 1; i++)
    {
        int source = route->nodeSequence[i];
        int dest = route->nodeSequence[i + 1];

        if (calculateDistance(graph, source, dest) == DBL_MAX)
        {
            // Les nœuds ne sont pas connectés directement
            return false;
        }
    }

    // Vérifier si tous les colis du véhicule peuvent être livrés par cette route
    // (Cette vérification pourrait être plus complexe dans un cas réel)

    return true;
}

// Évaluer la fitness d'un chromosome
void evaluateFitness(Chromosome *chromosome, Graph *graph, Vehicle *vehicles,
                     int vehicleCount, Package *packages, int packageCount)
{
    chromosome->totalDistance = 0.0;
    chromosome->totalTime = 0.0;
    chromosome->totalCost = 0.0;
    chromosome->unservedPackages = 0;
    chromosome->isValid = true;

    // Évaluer chaque route dans le chromosome
    for (int i = 0; i < chromosome->routeCount; i++)
    {
        ChromosomeRoute *route = &chromosome->routes[i];
        Vehicle *vehicle = &vehicles[i];

        // Vérifier si la route est valide
        route->isValid = isRouteValid(route, graph, vehicle, packages, packageCount);
        if (!route->isValid)
        {
            chromosome->isValid = false;
        }

        // Calculer la distance totale, le temps et le coût de la route
        route->distance = 0.0;
        route->time = 0.0;
        route->cost = 0.0;

        for (int j = 0; j < route->sequenceLength - 1; j++)
        {
            int source = route->nodeSequence[j];
            int dest = route->nodeSequence[j + 1];

            double distance = calculateDistance(graph, source, dest);
            double cost = calculateCost(graph, source, dest);
            double time = calculateTime(graph, source, dest);

            if (distance == DBL_MAX || cost == DBL_MAX || time == DBL_MAX)
            {
                // Les nœuds ne sont pas connectés
                route->isValid = false;
                chromosome->isValid = false;
                break;
            }

            route->distance += distance;
            route->time += time;
            route->cost += cost;
        }

        // Ajouter à la somme totale
        chromosome->totalDistance += route->distance;
        chromosome->totalTime += route->time;
        chromosome->totalCost += route->cost;
    }

    // Vérifier combien de colis ne sont pas servis
    // (Cette partie dépend de comment vous définissez les colis comme "servis")
    // Pour cet exemple, nous supposons que tous les colis sont servis si les routes sont valides

    // Fonction de fitness multi-critères
    // Plus la valeur est élevée, meilleure est la solution
    if (chromosome->isValid)
    {
        // Normaliser les valeurs
        double normalizedCost = 100000.0 / (chromosome->totalCost + 1.0);        // Inverser car moins de coût est mieux
        double normalizedDistance = 10000.0 / (chromosome->totalDistance + 1.0); // Inverser car moins de distance est mieux
        double normalizedTime = 1000.0 / (chromosome->totalTime + 1.0);          // Inverser car moins de temps est mieux

        // Pondération des critères
        const double weightCost = 0.5;     // 50% pondération sur le coût
        const double weightDistance = 0.3; // 30% pondération sur la distance
        const double weightTime = 0.2;     // 20% pondération sur le temps

        // Calcul de la fitness finale
        chromosome->fitnessValue = weightCost * normalizedCost +
                                   weightDistance * normalizedDistance +
                                   weightTime * normalizedTime;
    }
    else
    {
        // Pénalité pour les solutions invalides
        chromosome->fitnessValue = 0.0;
    }
}

// Sélectionner un parent par tournoi
Chromosome *tournamentSelection(Population *population, int tournamentSize)
{
    Chromosome *best = NULL;
    double bestFitness = -DBL_MAX;

    // Sélectionner tournamentSize chromosomes aléatoirement et trouver le meilleur
    for (int i = 0; i < tournamentSize; i++)
    {
        int idx = randomInt(0, population->size - 1);
        Chromosome *candidate = &population->individuals[idx];

        if (candidate->fitnessValue > bestFitness)
        {
            best = candidate;
            bestFitness = candidate->fitnessValue;
        }
    }

    return best;
}

// Croisement adapté pour l'optimisation des tournées
void crossover(Chromosome *parent1, Chromosome *parent2, Chromosome *offspring1,
               Chromosome *offspring2, double crossoverRate)
{
    // Si le taux de croisement n'est pas atteint, les enfants sont des copies des parents
    if (randomDouble() > crossoverRate)
    {
        *offspring1 = cloneChromosome(parent1);
        *offspring2 = cloneChromosome(parent2);
        return;
    }

    // Initialiser les enfants avec le même nombre de routes que les parents
    *offspring1 = initEmptyChromosome(parent1->routeCount);
    *offspring2 = initEmptyChromosome(parent1->routeCount);

    // Pour chaque véhicule, effectuer un croisement spécifique
    for (int v = 0; v < parent1->routeCount; v++)
    {
        ChromosomeRoute *route1 = &parent1->routes[v];
        ChromosomeRoute *route2 = &parent2->routes[v];

        // Initialiser les routes des enfants
        offspring1->routes[v].vehicleId = route1->vehicleId;
        offspring2->routes[v].vehicleId = route1->vehicleId;

        // Croisement à point unique adapté aux routes
        if (route1->sequenceLength > 2 && route2->sequenceLength > 2)
        {
            // Points de coupure pour chaque route (on garde le premier et le dernier nœud fixes)
            int cutPoint1 = randomInt(1, route1->sequenceLength - 2);
            int cutPoint2 = randomInt(1, route2->sequenceLength - 2);

            // Allouer l'espace pour les séquences de nœuds des enfants
            offspring1->routes[v].sequenceLength = cutPoint1 + (route2->sequenceLength - cutPoint2 - 1) + 1;
            offspring2->routes[v].sequenceLength = cutPoint2 + (route1->sequenceLength - cutPoint1 - 1) + 1;

            offspring1->routes[v].nodeSequence = (int *)malloc(offspring1->routes[v].sequenceLength * sizeof(int));
            offspring2->routes[v].nodeSequence = (int *)malloc(offspring2->routes[v].sequenceLength * sizeof(int));

            // Remplir la séquence du premier enfant
            int idx = 0;
            for (int i = 0; i <= cutPoint1; i++)
            {
                offspring1->routes[v].nodeSequence[idx++] = route1->nodeSequence[i];
            }
            for (int i = cutPoint2 + 1; i < route2->sequenceLength; i++)
            {
                offspring1->routes[v].nodeSequence[idx++] = route2->nodeSequence[i];
            }

            // Remplir la séquence du deuxième enfant
            idx = 0;
            for (int i = 0; i <= cutPoint2; i++)
            {
                offspring2->routes[v].nodeSequence[idx++] = route2->nodeSequence[i];
            }
            for (int i = cutPoint1 + 1; i < route1->sequenceLength; i++)
            {
                offspring2->routes[v].nodeSequence[idx++] = route1->nodeSequence[i];
            }
        }
        else
        {
            // Si les routes sont trop courtes, simplement copier
            offspring1->routes[v].sequenceLength = route1->sequenceLength;
            offspring1->routes[v].nodeSequence = (int *)malloc(route1->sequenceLength * sizeof(int));
            for (int i = 0; i < route1->sequenceLength; i++)
            {
                offspring1->routes[v].nodeSequence[i] = route1->nodeSequence[i];
            }

            offspring2->routes[v].sequenceLength = route2->sequenceLength;
            offspring2->routes[v].nodeSequence = (int *)malloc(route2->sequenceLength * sizeof(int));
            for (int i = 0; i < route2->sequenceLength; i++)
            {
                offspring2->routes[v].nodeSequence[i] = route2->nodeSequence[i];
            }
        }

        // Initialiser les autres propriétés (seront évaluées plus tard)
        offspring1->routes[v].distance = 0.0;
        offspring1->routes[v].time = 0.0;
        offspring1->routes[v].cost = 0.0;
        offspring1->routes[v].isValid = false;

        offspring2->routes[v].distance = 0.0;
        offspring2->routes[v].time = 0.0;
        offspring2->routes[v].cost = 0.0;
        offspring2->routes[v].isValid = false;
    }
}

// Mutation intelligente préservant la validité
void mutate(Chromosome *chromosome, double mutationRate, Graph *graph)
{
    // Pour chaque route dans le chromosome
    for (int v = 0; v < chromosome->routeCount; v++)
    {
        // Si le taux de mutation n'est pas atteint, passer à la route suivante
        if (randomDouble() > mutationRate)
        {
            continue;
        }

        ChromosomeRoute *route = &chromosome->routes[v];

        // Si la route est trop courte, ne pas muter
        if (route->sequenceLength <= 3)
        {
            continue;
        }

        // Type de mutation: inversion, échange, ou insertion
        int mutationType = randomInt(0, 2);

        switch (mutationType)
        {
        case 0:
        { // Inversion: inverser un segment de la route
            int start = randomInt(1, route->sequenceLength - 3);
            int end = randomInt(start + 1, route->sequenceLength - 2);

            // Inverser le segment
            for (int i = 0; i < (end - start + 1) / 2; i++)
            {
                int temp = route->nodeSequence[start + i];
                route->nodeSequence[start + i] = route->nodeSequence[end - i];
                route->nodeSequence[end - i] = temp;
            }
            break;
        }
        case 1:
        { // Échange: échanger deux nœuds aléatoires
            int pos1 = randomInt(1, route->sequenceLength - 2);
            int pos2 = randomInt(1, route->sequenceLength - 2);

            while (pos2 == pos1)
            {
                pos2 = randomInt(1, route->sequenceLength - 2);
            }

            int temp = route->nodeSequence[pos1];
            route->nodeSequence[pos1] = route->nodeSequence[pos2];
            route->nodeSequence[pos2] = temp;
            break;
        }
        case 2:
        { // Insertion: déplacer un nœud à une autre position
            int pos1 = randomInt(1, route->sequenceLength - 2);
            int pos2 = randomInt(1, route->sequenceLength - 2);

            while (pos2 == pos1)
            {
                pos2 = randomInt(1, route->sequenceLength - 2);
            }

            int nodeToMove = route->nodeSequence[pos1];

            // Déplacer les nœuds pour faire de la place
            if (pos1 < pos2)
            {
                for (int i = pos1; i < pos2; i++)
                {
                    route->nodeSequence[i] = route->nodeSequence[i + 1];
                }
                route->nodeSequence[pos2] = nodeToMove;
            }
            else
            {
                for (int i = pos1; i > pos2; i--)
                {
                    route->nodeSequence[i] = route->nodeSequence[i - 1];
                }
                route->nodeSequence[pos2] = nodeToMove;
            }
            break;
        }
        }

        // Vérifier si la nouvelle route est connectée dans le graphe
        bool validRoute = true;
        for (int i = 0; i < route->sequenceLength - 1; i++)
        {
            int source = route->nodeSequence[i];
            int dest = route->nodeSequence[i + 1];

            if (calculateDistance(graph, source, dest) == DBL_MAX)
            {
                validRoute = false;
                break;
            }
        }

        // Si la route n'est pas valide, tenter une réparation simple
        if (!validRoute)
        {
            // Ici, on pourrait implémenter une stratégie de réparation
            // Pour cet exemple, nous restaurons simplement la route à son état initial
            // Dans une implémentation complète, une stratégie de réparation intelligente serait préférable

            // Réinitialiser les valeurs (sera recalculé lors de l'évaluation)
            route->distance = 0.0;
            route->time = 0.0;
            route->cost = 0.0;
            route->isValid = false;
        }
    }
}

// Cloner un chromosome
Chromosome cloneChromosome(Chromosome *source)
{
    Chromosome clone = initEmptyChromosome(source->routeCount);

    clone.fitnessValue = source->fitnessValue;
    clone.totalDistance = source->totalDistance;
    clone.totalTime = source->totalTime;
    clone.totalCost = source->totalCost;
    clone.unservedPackages = source->unservedPackages;
    clone.isValid = source->isValid;

    for (int i = 0; i < source->routeCount; i++)
    {
        clone.routes[i].vehicleId = source->routes[i].vehicleId;
        clone.routes[i].sequenceLength = source->routes[i].sequenceLength;
        clone.routes[i].distance = source->routes[i].distance;
        clone.routes[i].time = source->routes[i].time;
        clone.routes[i].cost = source->routes[i].cost;
        clone.routes[i].isValid = source->routes[i].isValid;

        clone.routes[i].nodeSequence = (int *)malloc(source->routes[i].sequenceLength * sizeof(int));
        for (int j = 0; j < source->routes[i].sequenceLength; j++)
        {
            clone.routes[i].nodeSequence[j] = source->routes[i].nodeSequence[j];
        }
    }

    return clone;
}

// Mettre à jour la population avec la nouvelle génération
void updatePopulation(Population *population, Chromosome *newGeneration, int eliteCount)
{
    // Trier la population actuelle par fitness (décroissant)
    for (int i = 0; i < population->size; i++)
    {
        for (int j = i + 1; j < population->size; j++)
        {
            if (population->individuals[j].fitnessValue > population->individuals[i].fitnessValue)
            {
                Chromosome temp = population->individuals[i];
                population->individuals[i] = population->individuals[j];
                population->individuals[j] = temp;
            }
        }
    }

    // Sauvegarder les élites
    Chromosome *elites = (Chromosome *)malloc(eliteCount * sizeof(Chromosome));
    for (int i = 0; i < eliteCount; i++)
    {
        elites[i] = cloneChromosome(&population->individuals[i]);
    }

    // Libérer la mémoire de l'ancienne population
    for (int i = 0; i < population->size; i++)
    {
        freeChromosome(&population->individuals[i]);
    }

    // Remplacer par la nouvelle génération
    for (int i = 0; i < population->capacity; i++)
    {
        population->individuals[i] = newGeneration[i];
    }

    // Remplacer les pires individus par les élites
    for (int i = 0; i < eliteCount; i++)
    {
        freeChromosome(&population->individuals[population->capacity - 1 - i]);
        population->individuals[population->capacity - 1 - i] = elites[i];
    }

    free(elites);

    // Mettre à jour la meilleure solution
    double bestFitness = population->individuals[0].fitnessValue;
    if (bestFitness > population->bestEver.fitnessValue)
    {
        // Nouvelle meilleure solution
        if (population->bestEver.routes != NULL)
        {
            freeChromosome(&population->bestEver);
        }
        population->bestEver = cloneChromosome(&population->individuals[0]);
        population->generationsWithoutImprovement = 0;
    }
    else
    {
        population->generationsWithoutImprovement++;
    }

    // Calculer la fitness moyenne
    double sum = 0.0;
    for (int i = 0; i < population->size; i++)
    {
        sum += population->individuals[i].fitnessValue;
    }
    population->avgFitness = sum / population->size;
}

// Adapter les paramètres génétiques au cours de l'évolution
void adaptParameters(GAConfig *config, Population *population)
{
    // Si pas d'amélioration depuis stagnationLimit générations
    if (population->generationsWithoutImprovement >= config->stagnationLimit)
    {
        // Augmenter le taux de mutation pour favoriser l'exploration
        config->mutationRate = fmin(0.5, config->mutationRate * 1.5);

        // Diminuer le taux de croisement pour favoriser l'exploration
        config->crossoverRate = fmax(0.5, config->crossoverRate * 0.9);

        // Réinitialiser le compteur
        population->generationsWithoutImprovement = 0;

        printf("Adaptation des paramètres: Mutation = %.2f, Croisement = %.2f\n",
               config->mutationRate, config->crossoverRate);
    }

    // Si la population devient trop uniforme (faible écart-type de fitness)
    // On pourrait calculer l'écart-type ici et adapter en conséquence
}

// Recherche locale pour améliorer une solution
void localSearch(Chromosome *chromosome, Graph *graph)
{
    // Pour chaque route dans le chromosome
    for (int v = 0; v < chromosome->routeCount; v++)
    {
        ChromosomeRoute *route = &chromosome->routes[v];

        // Si la route est trop courte, passer à la suivante
        if (route->sequenceLength <= 3)
        {
            continue;
        }

        // Appliquer l'algorithme 2-opt pour améliorer la route
        bool improved = true;
        while (improved)
        {
            improved = false;

            // Essayer toutes les paires d'arêtes non adjacentes
            for (int i = 1; i < route->sequenceLength - 2; i++)
            {
                for (int j = i + 1; j < route->sequenceLength - 1; j++)
                {
                    // Calculer le gain potentiel en inversant le segment
                    int a = route->nodeSequence[i - 1];
                    int b = route->nodeSequence[i];
                    int c = route->nodeSequence[j];
                    int d = route->nodeSequence[j + 1];

                    double currentDist = calculateDistance(graph, a, b) + calculateDistance(graph, c, d);
                    double newDist = calculateDistance(graph, a, c) + calculateDistance(graph, b, d);

                    // Si l'inversion améliore la solution
                    if (newDist < currentDist)
                    {
                        // Inverser le segment
                        for (int k = 0; k < (j - i + 1) / 2; k++)
                        {
                            int temp = route->nodeSequence[i + k];
                            route->nodeSequence[i + k] = route->nodeSequence[j - k];
                            route->nodeSequence[j - k] = temp;
                        }
                        improved = true;
                    }
                }
            }
        }
    }
}

// Afficher un chromosome
void printChromosome(Chromosome *chromosome)
{
    printf("Chromosome: Fitness = %.6f, Valid = %s\n",
           chromosome->fitnessValue, chromosome->isValid ? "Oui" : "Non");
    printf("  Distance totale: %.2f, Temps total: %.2f, Coût total: %.2f\n",
           chromosome->totalDistance, chromosome->totalTime, chromosome->totalCost);

    for (int i = 0; i < chromosome->routeCount; i++)
    {
        printf("  Route %d (Véhicule %d): ", i, chromosome->routes[i].vehicleId);
        for (int j = 0; j < chromosome->routes[i].sequenceLength; j++)
        {
            printf("%d", chromosome->routes[i].nodeSequence[j]);
            if (j < chromosome->routes[i].sequenceLength - 1)
                printf(" -> ");
        }
        printf("\n    Distance: %.2f, Temps: %.2f, Coût: %.2f, Valid: %s\n",
               chromosome->routes[i].distance, chromosome->routes[i].time,
               chromosome->routes[i].cost, chromosome->routes[i].isValid ? "Oui" : "Non");
    }
}

// Afficher la meilleure solution
void printBestSolution(Population *population)
{
    printf("\n===== MEILLEURE SOLUTION =====\n");
    printChromosome(&population->bestEver);
    printf("==============================\n");
}

// Libérer la mémoire d'un chromosome
void freeChromosome(Chromosome *chromosome)
{
    for (int i = 0; i < chromosome->routeCount; i++)
    {
        free(chromosome->routes[i].nodeSequence);
        chromosome->routes[i].nodeSequence = NULL;
    }
    free(chromosome->routes);
    chromosome->routes = NULL;
}

// Libérer la mémoire d'une population
void freePopulation(Population *population)
{
    for (int i = 0; i < population->size; i++)
    {
        freeChromosome(&population->individuals[i]);
    }
    free(population->individuals);
    population->individuals = NULL;

    if (population->bestEver.routes != NULL)
    {
        freeChromosome(&population->bestEver);
    }
}

// Exécuter l'algorithme génétique
Chromosome runGeneticAlgorithm(Graph *graph, Vehicle *vehicles, int vehicleCount,
                               Package *packages, int packageCount, GAConfig config)
{
    // Initialiser le générateur de nombres aléatoires
    srand(time(NULL));

    // Créer la population initiale
    Population population = createInitialPopulation(config, graph, vehicles, vehicleCount, packages, packageCount);

    printf("Population initiale créée avec %d individus\n", population.size);

    // Boucle principale de l'algorithme génétique
    for (int generation = 0; generation < config.maxGenerations; generation++)
    {
        printf("Génération %d: Meilleure Fitness = %.6f, Fitness Moyenne = %.6f\n",
               generation, population.bestEver.fitnessValue, population.avgFitness);

        // Créer une nouvelle génération
        Chromosome *newGeneration = (Chromosome *)malloc(config.populationSize * sizeof(Chromosome));

        // Élitisme (déjà géré dans updatePopulation)

        // Créer de nouveaux individus par sélection, croisement et mutation
        for (int i = 0; i < config.populationSize; i += 2)
        {
            // Sélectionner deux parents par tournoi
            Chromosome *parent1 = tournamentSelection(&population, config.tournamentSize);
            Chromosome *parent2 = tournamentSelection(&population, config.tournamentSize);

            // Assurer que les parents sont différents
            while (parent2 == parent1)
            {
                parent2 = tournamentSelection(&population, config.tournamentSize);
            }

            // Créer deux enfants par croisement
            Chromosome offspring1, offspring2;
            crossover(parent1, parent2, &offspring1, &offspring2, config.crossoverRate);

            // Appliquer la mutation
            mutate(&offspring1, config.mutationRate, graph);
            mutate(&offspring2, config.mutationRate, graph);

            // Appliquer une recherche locale (optimisation)
            localSearch(&offspring1, graph);
            localSearch(&offspring2, graph);

            // Évaluer la fitness des enfants
            evaluateFitness(&offspring1, graph, vehicles, vehicleCount, packages, packageCount);
            evaluateFitness(&offspring2, graph, vehicles, vehicleCount, packages, packageCount);

            // Ajouter à la nouvelle génération
            newGeneration[i] = offspring1;
            if (i + 1 < config.populationSize)
            {
                newGeneration[i + 1] = offspring2;
            }
        }

        // Mettre à jour la population
        updatePopulation(&population, newGeneration, config.eliteCount);
        free(newGeneration);

        // Adapter les paramètres si nécessaire
        if (config.adaptiveParams)
        {
            adaptParameters(&config, &population);
        }
    }

    // Afficher la meilleure solution
    printBestSolution(&population);

    // Retourner la meilleure solution
    Chromosome bestSolution = cloneChromosome(&population.bestEver);

    // Libérer la mémoire
    freePopulation(&population);

    return bestSolution;
}