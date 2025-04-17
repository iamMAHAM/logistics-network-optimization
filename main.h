

#ifndef MAIN_H
#define MAIN_H
// Structure pour les options du menu des algorithmes
typedef struct
{
    const char *name;
    void (*function)(Graph *);
    const char *description;
} AlgorithmOption;

#define MAX_TSP_SIZE 20                       // Nombre maximum de sommets pour l'algorithme TSP
#define MAX_INPUT_SIZE 256                    // Taille maximale pour l'entrée utilisateur
#define CLEAR_SCREEN() printf("\033[H\033[J") // Séquence d'échappement ANSI pour effacer l'écran

// Prototypes des fonctions
void BFSWrapper(Graph *graph);
void DFSWrapper(Graph *graph);
void FloydWarshallWrapper(Graph *graph);
void BellmanFordWrapper(Graph *graph);
void TSPWrapper(Graph *graph);
void GeneticAlgorithmWrapper(Graph *graph);
void MultiDayPlanningWrapper(Graph *graph);
double measureExecutionTime(void (*algorithm)(Graph *), Graph *graph);
long getMemoryUsage(void);
void compareAlgorithms(Graph *graph);
void flushInputBuffer(void);
int getValidIntegerInput(void);
int showDatasetSelectionMenu(void);
void runAlgorithmOnDataset(void (*algorithmFunc)(Graph *), const char *algorithmName);
void generateFullReport(void);
void showAlgorithmSelectionMenu(void);
void showMainMenu(void);

#endif
