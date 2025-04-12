#ifndef BFS_H
#define BFS_H

#include "../core/graph.h"
#include <stdbool.h>

// Structure : Queue (file)
// La file est utilisée pour gérer les nœuds à explorer dans le parcours en largeur.
// Elle suit le principe FIFO (First In, First Out), ce qui correspond à la logique du BFS :
// - Le premier nœud ajouté est le premier à être exploré.
// - Cela permet d'explorer tous les voisins d'un nœud avant de passer aux niveaux suivants.
//
// Justification :
// Une file est idéale pour BFS car elle garantit que les nœuds sont explorés dans l'ordre
// dans lequel ils sont découverts, ce qui est essentiel pour respecter la logique de parcours en largeur.
// Structure pour une file (utilisée dans BFS)
typedef struct Queue
{
    int *data;
    int front;
    int rear;
    int capacity;
} Queue;

// Prototype de la fonction BFS
void BFS(Graph *graph, int startVertex);

// fonction pour liberer la mémoire de la file
void freeQueue(Queue *queue);

// Fonction pour crée une nouvelle file avec une capacité donnée
Queue *createQueue(int capacity);

// Fonction pour enfiler un élément dans la file
bool isQueueEmpty(Queue *queue);

// Fonction pour défiler un élément de la file
void enqueue(Queue *queue, int value);

// Fonction pour défiler un élément de la file
int dequeue(Queue *queue);

#endif // BFS_H