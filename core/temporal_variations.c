#include "graph.h"
#include <string.h>

// Fonction pour ajuster les attributs d’une arête en fonction de l’heure de la journée
void adjustEdgeAttributesForTime(EdgeAttr *attr, int hour)
{
    if (hour >= 7 && hour <= 9)
    {                             // Heures de pointe matin
        attr->baseTime *= 1.5;    // Augmentation du temps de trajet
        attr->reliability *= 0.8; // Réduction de la fiabilité
    }
    else if (hour >= 17 && hour <= 19)
    { // Heures de pointe soir
        attr->baseTime *= 1.4;
        attr->reliability *= 0.85;
    }
    else
    { // Heures creuses
        attr->baseTime *= 0.9;
        attr->reliability *= 1.1;
    }
}

// Fonction pour marquer une arête comme indisponible pour une saison donnée
void markEdgeUnavailableForSeason(EdgeAttr *attr, const char *season)
{
    if (strcmp(season, "rainy") == 0 && attr->roadType == 1)
    {                            // Routes en latérite
        attr->reliability = 0.0; // Route impraticable
    }
}