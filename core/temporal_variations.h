#ifndef TEMPORAL_VARIATIONS_H
#define TEMPORAL_VARIATIONS_H

#include "graph.h"

// Fonction pour ajuster les attributs d’une arête en fonction de l’heure de la journée
void adjustEdgeAttributesForTime(EdgeAttr *attr, int hour);

// Fonction pour marquer une arête comme indisponible pour une saison donnée
void markEdgeUnavailableForSeason(EdgeAttr *attr, const char *season);

#endif // TEMPORAL_VARIATIONS_H