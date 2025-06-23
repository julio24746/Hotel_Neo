#ifndef PATHFINDING_H
#define PATHFINDING_H

#include <vector>
#include <utility>

// Tipo para representar un punto del hotel: (piso, habitación)
using PuntoHotel = std::pair<int, int>;

// Calcula el camino más corto entre dos puntos del hotel.
// En una versión simple, solo cuenta distancia de Manhattan.
std::vector<PuntoHotel> encontrarCaminoMasCorto(PuntoHotel origen, PuntoHotel destino);

#endif // PATHFINDING_H