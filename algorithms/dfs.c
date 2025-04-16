#include "dfs.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

// Initialisation de la pile
Stack *createStack(int capacity)
{
    Stack *stack = (Stack *)malloc(sizeof(Stack));
    stack->data = (int *)malloc(capacity * sizeof(int));
    stack->top = -1;
    stack->capacity = capacity;
    return stack;
}

// Vérifie si la pile est vide
bool isStackEmpty(Stack *stack)
{
    return stack->top == -1;
}

// Empile un élément
void push(Stack *stack, int value)
{
    if (stack->top + 1 >= stack->capacity)
    {
        printf("Erreur : Dépassement de la pile lors de l'ajout de %d\n", value);
        return;
    }
    stack->data[++stack->top] = value;
    printf("Ajouté %d à la pile.\n", value);
}

// Dépile un élément
int pop(Stack *stack)
{
    if (isStackEmpty(stack))
    {
        printf("Erreur : Sous-dépassement de la pile lors du retrait.\n");
        return -1; // Retourne une valeur invalide pour indiquer une erreur
    }
    int value = stack->data[stack->top--];
    printf("Retiré %d de la pile.\n", value);
    return value;
}

// Libère la mémoire de la pile
void freeStack(Stack *stack)
{
    free(stack->data);
    free(stack);
}

// Fonction DFS
void DFS(Graph *graph, int startVertex)
{
    Stack *stack = createStack(graph->V);
    bool *visited = (bool *)malloc(graph->V * sizeof(bool));
    for (int i = 0; i < graph->V; i++)
    {
        visited[i] = false;
    }

    push(stack, startVertex);
    printf("Début du DFS à partir du sommet %d\n", startVertex);

    while (!isStackEmpty(stack))
    {
        int currentVertex = pop(stack);
        printf("Sommet %d retiré de la pile\n", currentVertex);

        if (!visited[currentVertex])
        {
            printf("Visite du sommet %d\n", currentVertex);
            visited[currentVertex] = true;
        }

        AdjListNode *adjList = graph->array[currentVertex].head;
        while (adjList != NULL)
        {
            int adjVertex = adjList->dest;
            printf("Vérification du sommet adjacent %d\n", adjVertex);
            if (!visited[adjVertex])
            {
                printf("Ajout du sommet %d à la pile\n", adjVertex);
                push(stack, adjVertex);
            }
            adjList = adjList->next;
        }
    }

    free(visited);
    freeStack(stack);
}