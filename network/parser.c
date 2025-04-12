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
void saveGraphToJSON(Graph *graph, const char *filename)
{
    cJSON *json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "vertices", graph->V);

    cJSON *edges = cJSON_CreateArray();
    for (int v = 0; v < graph->V; ++v)
    {
        AdjListNode *current = graph->array[v].head;
        while (current)
        {
            cJSON *edge = cJSON_CreateObject();
            cJSON_AddNumberToObject(edge, "src", v);
            cJSON_AddNumberToObject(edge, "dest", current->dest);
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

    char *data = cJSON_Print(json);
    FILE *file = fopen(filename, "w");
    if (file)
    {
        fprintf(file, "%s", data);
        fclose(file);
    }
    else
    {
        perror("Erreur lors de l'écriture du fichier JSON");
    }

    free(data);
    cJSON_Delete(json);
}