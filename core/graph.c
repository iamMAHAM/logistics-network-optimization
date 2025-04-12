#include "graph.h"

// Fonction pour créer un graphe avec V sommets
Graph *createGraph(int V)
{
    Graph *graph = (Graph *)malloc(sizeof(Graph));
    graph->V = V;
    graph->array = (AdjList *)malloc(V * sizeof(AdjList));

    for (int i = 0; i < V; i++)
    {
        graph->array[i].head = NULL;
    }

    return graph;
}

// Fonction pour ajouter une arête au graphe
void addEdge(Graph *graph, int src, int dest, EdgeAttr attr)
{
    AdjListNode *newNode = (AdjListNode *)malloc(sizeof(AdjListNode));
    newNode->dest = dest;
    newNode->attr = attr;
    newNode->next = graph->array[src].head;
    graph->array[src].head = newNode;
}

// Fonction pour supprimer une arête du graphe
void removeEdge(Graph *graph, int src, int dest)
{
    AdjListNode *current = graph->array[src].head;
    AdjListNode *prev = NULL;

    while (current != NULL && current->dest != dest)
    {
        prev = current;
        current = current->next;
    }

    if (current == NULL)
        return; // Arête non trouvée

    if (prev == NULL)
    {
        graph->array[src].head = current->next;
    }
    else
    {
        prev->next = current->next;
    }

    free(current);
}

// Fonction pour ajouter un nœud au graphe
void addNode(Graph *graph)
{
    graph->V++;
    graph->array = (AdjList *)realloc(graph->array, graph->V * sizeof(AdjList));
    graph->array[graph->V - 1].head = NULL;
}

// Fonction pour supprimer un nœud du graphe
void removeNode(Graph *graph, int node)
{
    for (int i = 0; i < graph->V; i++)
    {
        removeEdge(graph, i, node);
    }

    free(graph->array[node].head);

    for (int i = node; i < graph->V - 1; i++)
    {
        graph->array[i] = graph->array[i + 1];
    }

    graph->V--;
    graph->array = (AdjList *)realloc(graph->array, graph->V * sizeof(AdjList));
}

// Fonction pour libérer la mémoire allouée au graphe
void freeGraph(Graph *graph)
{
    for (int i = 0; i < graph->V; i++)
    {
        AdjListNode *current = graph->array[i].head;
        while (current != NULL)
        {
            AdjListNode *temp = current;
            current = current->next;
            free(temp);
        }
    }

    free(graph->array);
    free(graph);
}

// Fonction pour afficher le graphe
void printGraph(Graph *graph)
{
    for (int v = 0; v < graph->V; ++v)
    {
        AdjListNode *current = graph->array[v].head;
        printf("Noeud %d:\n", v);
        while (current)
        {
            printf("  -> %d (distance: %.2f, temps: %.2f, coût: %.2f, type de route: %d, fiabilité: %.2f)\n",
                   current->dest, current->attr.distance, current->attr.baseTime, current->attr.cost,
                   current->attr.roadType, current->attr.reliability);
            current = current->next;
        }
        printf("\n");
    }
}

// Fonction pour afficher le graphe sous forme de listes d'adjacence
void displayAdjacencyList(Graph *graph)
{
    printf("Représentation par listes d'adjacence :\n");
    for (int v = 0; v < graph->V; ++v)
    {
        printf("Noeud %d :", v);
        AdjListNode *current = graph->array[v].head;
        while (current)
        {
            printf(" -> %d (distance: %.2f, coût: %.2f, fiabilité: %.2f)",
                   current->dest, current->attr.distance, current->attr.cost, current->attr.reliability);
            current = current->next;
        }
        printf("\n");
    }
}