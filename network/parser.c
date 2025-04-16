#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include "cJSON.h"

// Fonction pour charger un graphe à partir d'un fichier JSON
// Cette fonction lit un fichier JSON, analyse son contenu et crée un graphe en mémoire.
//
// Paramètres :
// - filename : le chemin du fichier JSON à charger.
//
// Retourne :
// - Un pointeur vers un objet Graph si le chargement réussit.
// - NULL en cas d'erreur (par exemple, si le fichier est introuvable ou si le JSON est invalide).
//
// Étapes principales :
// 1. Ouvrir le fichier JSON et lire son contenu dans une chaîne de caractères.
// 2. Analyser la chaîne JSON pour obtenir un objet cJSON.
// 3. Extraire les nœuds et les arêtes du JSON et les ajouter au graphe.
// 4. Libérer les ressources allouées et retourner le graphe.
Graph *loadGraphFromJSON(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        perror("Erreur lors de l'ouverture du fichier JSON");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *data = (char *)malloc(length + 1);
    fread(data, 1, length, file);
    fclose(file);
    data[length] = '\0';

    cJSON *json = cJSON_Parse(data);
    free(data);

    if (!json)
    {
        fprintf(stderr, "Erreur de parsing JSON: %s\n", cJSON_GetErrorPtr());
        return NULL;
    }

    cJSON *nodes = cJSON_GetObjectItem(json, "nodes");
    int V = cJSON_GetArraySize(nodes);
    Graph *graph = createGraph(V);

    // cJSON *node;
    // utiliser pour debugger car le fichier chargé generait une erreur
    // cJSON_ArrayForEach(node, nodes)
    // {
    //     int id = cJSON_GetObjectItem(node, "id")->valueint;
    //     printf("Chargement du nœud %d\n", id);
    // }

    cJSON *edges = cJSON_GetObjectItem(json, "edges");
    cJSON *edge;
    cJSON_ArrayForEach(edge, edges)
    {
        int src = cJSON_GetObjectItem(edge, "source")->valueint;
        int dest = cJSON_GetObjectItem(edge, "destination")->valueint;

        EdgeAttr attr;
        attr.distance = cJSON_GetObjectItem(edge, "distance")->valuedouble;
        attr.baseTime = cJSON_GetObjectItem(edge, "baseTime")->valuedouble;
        attr.cost = cJSON_GetObjectItem(edge, "cost")->valuedouble;
        attr.roadType = cJSON_GetObjectItem(edge, "roadType")->valueint;
        attr.reliability = cJSON_GetObjectItem(edge, "reliability")->valuedouble;
        attr.restrictions = cJSON_GetObjectItem(edge, "restrictions")->valueint;

        addEdge(graph, src, dest, attr);
    }

    cJSON_Delete(json);
    return graph;
}

// Fonction pour sauvegarder un graphe dans un fichier JSON
// Cette fonction crée un fichier JSON à partir d'un graphe en mémoire,
// en suivant exactement le même format que les fichiers JSON existants.
void saveGraphToJSON(Graph *graph, const char *filename)
{
    cJSON *json = cJSON_CreateObject();

    // Création de la section "nodes"
    cJSON *nodes = cJSON_CreateArray();
    for (int i = 0; i < graph->V; ++i)
    {
        cJSON *node = cJSON_CreateObject();
        cJSON_AddNumberToObject(node, "id", i);

        // Déterminer le type de nœud en fonction de l'ID (pour l'exemple)
        char *nodeType;
        char nodeName[50];
        if (i % 3 == 0)
        {
            nodeType = "hub";
            sprintf(nodeName, "Hub %d", i);
        }
        else if (i % 3 == 1)
        {
            nodeType = "relay";
            sprintf(nodeName, "Relais %d", i);
        }
        else
        {
            nodeType = "station";
            sprintf(nodeName, "Station %d", i);
        }

        cJSON_AddStringToObject(node, "name", nodeName);
        cJSON_AddStringToObject(node, "type", nodeType);

        // Coordonnées aléatoires pour l'exemple (entre -10 et 10)
        cJSON *coordinates = cJSON_CreateArray();
        double x = ((double)rand() / RAND_MAX) * 20 - 10;
        double y = ((double)rand() / RAND_MAX) * 20 - 10;
        cJSON_AddItemToArray(coordinates, cJSON_CreateNumber(x));
        cJSON_AddItemToArray(coordinates, cJSON_CreateNumber(y));
        cJSON_AddItemToObject(node, "coordinates", coordinates);

        // Capacité aléatoire entre 100 et 1000
        int capacity = rand() % 901 + 100;
        cJSON_AddNumberToObject(node, "capacity", capacity);

        cJSON_AddItemToArray(nodes, node);
    }
    cJSON_AddItemToObject(json, "nodes", nodes);

    // Création de la section "edges"
    cJSON *edges = cJSON_CreateArray();
    for (int v = 0; v < graph->V; ++v)
    {
        AdjListNode *current = graph->array[v].head;
        while (current)
        {
            cJSON *edge = cJSON_CreateObject();
            cJSON_AddNumberToObject(edge, "source", v);
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
    cJSON_AddItemToObject(json, "edges", edges);

    // Écriture dans le fichier
    char *data = cJSON_Print(json);
    FILE *file = fopen(filename, "w");
    if (file)
    {
        fprintf(file, "%s", data);
        fclose(file);
        printf("Graphe sauvegardé avec succès dans %s\n", filename);
    }
    else
    {
        perror("Erreur lors de l'écriture du fichier JSON");
    }

    free(data);
    cJSON_Delete(json);
}