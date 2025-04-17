#include "dfs.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#define STACK_CAPACITY_MULTIPLIER 2 // Multiplier pour augmenter la capacité de la pile

// Initialisation de la pile
Stack *createStack(int capacity)
{
    Stack *stack = (Stack *)malloc(sizeof(Stack));
    stack->data = (int *)malloc(capacity * sizeof(int));
    stack->top = -1;
    stack->capacity = capacity;
    return stack;
}

// Fonction pour redimensionner dynamiquement la pile
void resizeStack(Stack *stack)
{
    stack->capacity *= STACK_CAPACITY_MULTIPLIER;
    stack->data = (int *)realloc(stack->data, stack->capacity * sizeof(int));
    if (!stack->data)
    {
        fprintf(stderr, "Erreur : Échec du redimensionnement de la pile.\n");
        exit(EXIT_FAILURE);
    }
}

// Vérifie si la pile est vide
bool isStackEmpty(Stack *stack)
{
    return stack->top == -1;
}

// Empile un élément avec redimensionnement si nécessaire
void push(Stack *stack, int value)
{
    if (stack->top + 1 >= stack->capacity)
    {
        printf("Redimensionnement de la pile. Nouvelle capacité : %d\n", stack->capacity * STACK_CAPACITY_MULTIPLIER);
        resizeStack(stack);
    }
    stack->data[++stack->top] = value;
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

    while (!isStackEmpty(stack))
    {
        int currentVertex = pop(stack);

        if (!visited[currentVertex])
        {
            visited[currentVertex] = true;
        }

        AdjListNode *adjList = graph->array[currentVertex].head;
        while (adjList != NULL)
        {
            int adjVertex = adjList->dest;
            if (!visited[adjVertex])
            {
                push(stack, adjVertex);
            }
            adjList = adjList->next;
        }
    }

    free(visited);
    freeStack(stack);
}