#include "bfs.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

// Initialisation de la file
Queue *createQueue(int capacity)
{
    Queue *queue = (Queue *)malloc(sizeof(Queue));
    queue->data = (int *)malloc(capacity * sizeof(int));
    queue->front = 0;
    queue->rear = -1;
    queue->capacity = capacity;
    return queue;
}

// Vérifie si la file est vide
bool isQueueEmpty(Queue *queue)
{
    return queue->rear < queue->front;
}

// Enfile un élément
void enqueue(Queue *queue, int value)
{
    queue->data[++queue->rear] = value;
}

// Défile un élément
int dequeue(Queue *queue)
{
    return queue->data[queue->front++];
}

// Libère la mémoire de la file
void freeQueue(Queue *queue)
{
    free(queue->data);
    free(queue);
}

// Fonction BFS
void BFS(Graph *graph, int startVertex)
{
    bool *visited = (bool *)calloc(graph->V, sizeof(bool));
    Queue *queue = createQueue(graph->V);

    enqueue(queue, startVertex);
    visited[startVertex] = true;

    printf("Parcours en largeur (BFS) :\n");
    while (!isQueueEmpty(queue))
    {
        int currentVertex = dequeue(queue);
        // printf("%d ", currentVertex);

        AdjListNode *adjList = graph->array[currentVertex].head;
        while (adjList)
        {
            if (!visited[adjList->dest])
            {
                visited[adjList->dest] = true;
                enqueue(queue, adjList->dest);
            }
            adjList = adjList->next;
        }
    }

    printf("\n");
    free(visited);
    freeQueue(queue);
}