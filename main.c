#include "core/graph.h"
#include "core/temporal_variations.h"
#include "network/parser.h"
#include "algorithms/dfs.h"
#include "algorithms/bfs.h"
#include "algorithms/graph_analysis.h"

int main()
{
    // Exemple de fichier JSON attendu pour décrire le réseau
    const char *json_filename = "network/network_tree.json";

    // Charger le graphe à partir du fichier JSON
    Graph *graph = loadGraphFromJSON(json_filename);
    if (graph == NULL)
    {
        printf("Erreur lors du chargement du graphe depuis le fichier JSON.\n");
        return 1;
    }

    // Afficher le graphe chargé
    printf("Graphe chargé depuis %s :\n", json_filename);

    // Afficher le graphe sous forme de listes d'adjacence
    printf("\nAffichage du graphe sous forme de listes d'adjacence :\n");
    displayAdjacencyList(graph);

    // Exemple de parcours en profondeur (DFS)
    printf("\nExemple de parcours en profondeur (DFS) à partir du sommet 0 :\n");
    DFS(graph, 0);

    // Exemple de parcours en largeur (BFS)
    printf("\nExemple de parcours en largeur (BFS) à partir du sommet 0 :\n");
    BFS(graph, 0);

    // Test de la détection des cycles
    printf("\nTest de la détection des cycles :\n");
    if (detectCycle(graph))
    {
        printf("Le graphe contient un cycle.\n");
    }
    else
    {
        printf("Le graphe ne contient pas de cycle.\n");
    }

    // Test de l'identification des composantes connexes
    printf("\nTest de l'identification des composantes connexes :\n");
    findConnectedComponents(graph);

    // Test de l'accessibilité entre deux nœuds
    printf("\nTest de l'accessibilité entre les nœuds 0 et 3 :\n");
    if (isAccessible(graph, 0, 3))
    {
        printf("Le nœud 3 est accessible depuis le nœud 0.\n");
    }
    else
    {
        printf("Le nœud 3 n'est pas accessible depuis le nœud 0.\n");
    }

    // Test du calcul des statistiques de connectivité
    printf("\nTest du calcul des statistiques de connectivité :\n");
    calculateConnectivityStats(graph);

    // Sauvegarder le graphe dans un autre fichier JSON
    const char *output_filename = "output_network.json";
    saveGraphToJSON(graph, output_filename);
    printf("Graphe sauvegardé dans %s.\n", output_filename);

    // Libération de la mémoire
    freeGraph(graph);

    return 0;
}