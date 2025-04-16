#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>     // Pour strcmp
#include "core/graph.h" // Assurer que destroyGraph est déclaré
#include "network/cJSON.h"
#include <sys/stat.h>  // Pour stat
#include <sys/types.h> // Pour mkdir

// Fonction pour générer un nombre flottant aléatoire entre min et max
float randomFloat(float min, float max)
{
    return min + ((float)rand() / RAND_MAX) * (max - min);
}

// Fonction pour générer un graphe aléatoire et le sauvegarder en JSON
void generateGraphJSON(const char *filename, int numNodes, int numEdges, const char *scenario)
{
    srand(time(NULL));

    // Créer un graphe
    Graph *graph = createGraph(numNodes);

    // Assurer que le graphe est connecté en ajoutant un arbre couvrant
    for (int i = 1; i < numNodes; i++)
    {
        int src = i - 1;
        int dest = i;
        EdgeAttr attr;
        attr.distance = randomFloat(10.0, 50.0);  // Plage raisonnable pour la distance
        attr.baseTime = randomFloat(5.0, 20.0);   // Plage raisonnable pour le temps de base
        attr.cost = randomFloat(100.0, 500.0);    // Plage raisonnable pour le coût
        attr.roadType = rand() % 3;               // 0 : asphalte, 1 : terre, 2 : gravier
        attr.reliability = randomFloat(0.7, 1.0); // Haute fiabilité pour l'arbre couvrant
        attr.restrictions = 0;
        addEdge(graph, src, dest, attr);
    }

    // Ajouter des arêtes aléatoires
    for (int i = 0; i < numEdges; i++)
    {
        int src = rand() % numNodes;
        int dest = rand() % numNodes;
        if (src != dest)
        {
            EdgeAttr attr;
            attr.distance = randomFloat(1.0, 100.0);
            attr.baseTime = randomFloat(1.0, 60.0);
            attr.cost = randomFloat(10.0, 1000.0);
            attr.roadType = rand() % 3; // 0 : asphalte, 1 : terre, 2 : gravier
            attr.reliability = randomFloat(0.5, 1.0);
            attr.restrictions = 0;

            // Ajuster les attributs en fonction du scénario
            if (strcmp(scenario, "peak") == 0)
            {
                attr.baseTime *= 1.5; // Augmenter le temps de trajet
                attr.cost *= 1.2;     // Augmenter le coût
            }
            else if (strcmp(scenario, "crisis") == 0)
            {
                if (rand() % 4 == 0)
                {
                    attr.reliability = randomFloat(0.1, 0.5); // Réduire la fiabilité
                }
            }

            addEdge(graph, src, dest, attr);
        }
    }

    // Créer un objet JSON
    cJSON *jsonGraph = cJSON_CreateObject();
    cJSON *nodes = cJSON_CreateArray();
    cJSON *edges = cJSON_CreateArray();

    // Ajouter les nœuds au JSON
    for (int i = 0; i < numNodes; i++)
    {
        cJSON *node = cJSON_CreateObject();
        cJSON_AddNumberToObject(node, "id", i);
        char name[50];
        snprintf(name, sizeof(name), "%s %d", (i % 3 == 0) ? "Hub" : ((i % 3 == 1) ? "Relais" : "Station"), i);
        cJSON_AddStringToObject(node, "name", name);
        cJSON_AddStringToObject(node, "type", (i % 3 == 0) ? "hub" : ((i % 3 == 1) ? "relay" : "station"));
        cJSON *coordinates = cJSON_CreateArray();
        cJSON_AddItemToArray(coordinates, cJSON_CreateNumber(randomFloat(4.0, 10.0)));  // Latitude
        cJSON_AddItemToArray(coordinates, cJSON_CreateNumber(randomFloat(-8.0, -2.0))); // Longitude
        cJSON_AddItemToObject(node, "coordinates", coordinates);
        cJSON_AddNumberToObject(node, "capacity", rand() % 1000 + 100);
        cJSON_AddItemToArray(nodes, node);
    }

    // Ajouter les arêtes au JSON
    for (int i = 0; i < graph->V; i++)
    {
        AdjListNode *current = graph->array[i].head;
        while (current)
        {
            cJSON *edge = cJSON_CreateObject();
            cJSON_AddNumberToObject(edge, "source", i);
            cJSON_AddNumberToObject(edge, "destination", current->dest);
            cJSON_AddNumberToObject(edge, "distance", current->attr.distance);
            cJSON_AddNumberToObject(edge, "baseTime", current->attr.baseTime);
            cJSON_AddNumberToObject(edge, "cost", current->attr.cost);
            cJSON_AddNumberToObject(edge, "roadType", current->attr.roadType);
            cJSON_AddNumberToObject(edge, "reliability", current->attr.reliability);
            cJSON_AddNumberToObject(edge, "restrictions", current->attr.restrictions);
            cJSON_AddItemToArray(edges, edge);
            current = current->next;
        }
    }

    cJSON_AddItemToObject(jsonGraph, "nodes", nodes);
    cJSON_AddItemToObject(jsonGraph, "edges", edges);

    // Écrire le JSON dans un fichier
    FILE *file = fopen(filename, "w");
    if (file)
    {
        char *jsonString = cJSON_Print(jsonGraph);
        fprintf(file, "%s", jsonString);
        fclose(file);
        free(jsonString);
    }

    // Nettoyer
    cJSON_Delete(jsonGraph);
    freeGraph(graph);
}

int main()
{
    // Vérifier si le dossier 'datasets' existe, sinon le créer automatiquement
    struct stat st = {0};
    if (stat("datasets", &st) == -1)
    {
        printf("Le dossier 'datasets' n'existe pas. Création en cours...\n");
        if (mkdir("datasets", 0700) == -1)
        {
            perror("Erreur lors de la création du dossier 'datasets'");
            return 1;
        }
        printf("Dossier 'datasets' créé avec succès.\n");
    }

    // Générer des petits réseaux
    generateGraphJSON("datasets/small_network_normal.json", 15, 30, "normal");
    generateGraphJSON("datasets/small_network_peak.json", 15, 30, "peak");
    generateGraphJSON("datasets/small_network_crisis.json", 15, 30, "crisis");

    // Générer des réseaux moyens
    generateGraphJSON("datasets/medium_network_normal.json", 120, 300, "normal");
    generateGraphJSON("datasets/medium_network_peak.json", 120, 300, "peak");
    generateGraphJSON("datasets/medium_network_crisis.json", 120, 300, "crisis");

    // Générer des grands réseaux
    generateGraphJSON("datasets/large_network_normal.json", 250, 600, "normal");
    generateGraphJSON("datasets/large_network_peak.json", 250, 600, "peak");
    generateGraphJSON("datasets/large_network_crisis.json", 250, 600, "crisis");

    return 0;
}