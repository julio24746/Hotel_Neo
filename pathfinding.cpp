#include "pathfinding.h"
#include <cmath>
#include <vector>

// Devuelve una ruta directa en línea recta (Manhattan)
std::vector<PuntoHotel> encontrarCaminoMasCorto(PuntoHotel origen, PuntoHotel destino) {
    std::vector<PuntoHotel> camino;

    int piso_actual = origen.first;
    int habitacion_actual = origen.second;

    // Subir o bajar de piso
    while (piso_actual != destino.first) {
        camino.emplace_back(piso_actual, habitacion_actual);
        piso_actual += (destino.first > piso_actual) ? 1 : -1;
    }

    // Ir a la habitación deseada en ese piso
    while (habitacion_actual != destino.second) {
        camino.emplace_back(piso_actual, habitacion_actual);
        habitacion_actual += (destino.second > habitacion_actual) ? 1 : -1;
    }

    camino.emplace_back(destino); // Añade el destino final
    return camino;
}