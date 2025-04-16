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
        printf("Error: Stack overflow while pushing %d\n", value);
        return;
    }
    stack->data[++stack->top] = value;
    printf("Pushed %d onto the stack.\n", value);
}

// Dépile un élément
int pop(Stack *stack)
{
    if (isStackEmpty(stack))
    {
        printf("Error: Stack underflow while popping.\n");
        return -1; // Return an invalid value to indicate error
    }
    int value = stack->data[stack->top--];
    printf("Popped %d from the stack.\n", value);
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
    printf("Starting DFS from vertex %d\n", startVertex);

    while (!isStackEmpty(stack))
    {
        int currentVertex = pop(stack);
        printf("Popped vertex %d from stack\n", currentVertex);

        if (!visited[currentVertex])
        {
            printf("Visiting vertex %d\n", currentVertex);
            visited[currentVertex] = true;
        }

        AdjListNode *adjList = graph->array[currentVertex].head;
        while (adjList != NULL)
        {
            int adjVertex = adjList->dest;
            printf("Checking adjacent vertex %d\n", adjVertex);
            if (!visited[adjVertex])
            {
                printf("Pushing vertex %d onto stack\n", adjVertex);
                push(stack, adjVertex);
            }
            adjList = adjList->next;
        }
    }

    free(visited);
    freeStack(stack);
}