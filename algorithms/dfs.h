#ifndef DFS_H
#define DFS_H

#include "../core/graph.h"
#include "stdbool.h"

// Structure : Stack (pile)
// La pile est utilisée pour gérer les nœuds à explorer dans le parcours en profondeur.
// Elle suit le principe LIFO (Last In, First Out), ce qui correspond à la logique du DFS :
// - Le dernier nœud ajouté est le premier à être exploré.
// - Cela permet de revenir en arrière dans le graphe lorsque tous les voisins d'un nœud ont été visités.
//
// Justification :
// Une pile est idéale pour DFS car elle permet de gérer efficacement les appels récursifs implicites
// dans une version itérative de l'algorithme. Cela évite les débordements de pile (stack overflow)
// qui peuvent survenir avec une implémentation récursive sur de grands graphes.
// Structure pour une pile (utilisée dans DFS)
typedef struct Stack
{
    int *data;
    int top;
    int capacity;
} Stack;

// Prototype de la fonction DFS
void DFS(Graph *graph, int startVertex);

// Fonction pour créer une nouvelle pile avec une capacité donnée
Stack *createStack(int capacity);

// Fonction pour empiler un élément dans la pile
void push(Stack *stack, int value);

// Fonction pour dépiler un élément de la pile
int pop(Stack *stack);

// Fonction pour vérifier si la pile est vide
bool isStackEmpty(Stack *stack);

// Fonction pour libérer la mémoire de la pile
void freeStack(Stack *stack);

#endif // DFS_H