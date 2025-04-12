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
    stack->data[++stack->top] = value;
}

// Dépile un élément
int pop(Stack *stack)
{
    return stack->data[stack->top--];
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
    bool *visited = (bool *)calloc(graph->V, sizeof(bool));
    Stack *stack = createStack(graph->V);

    push(stack, startVertex);

    printf("Parcours en profondeur (DFS) :\n");
    while (!isStackEmpty(stack))
    {
        int currentVertex = pop(stack);

        if (!visited[currentVertex])
        {
            printf("%d ", currentVertex);
            visited[currentVertex] = true;
        }

        AdjListNode *adjList = graph->array[currentVertex].head;
        while (adjList)
        {
            if (!visited[adjList->dest])
            {
                push(stack, adjList->dest);
            }
            adjList = adjList->next;
        }
    }

    printf("\n");
    free(visited);
    freeStack(stack);
}