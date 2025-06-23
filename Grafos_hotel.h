#ifndef GRAFO_HOTEL_H
#define GRAFO_HOTEL_H

#include <vector>
#include <unordered_map>
#include <queue>
#include <limits>
#include <utility>
#include <algorithm>

// Representa un punto del hotel: piso y habitación
struct Nodo {
    int piso;
    int habitacion;

    // Operador de igualdad
    bool operator==(const Nodo& otro) const {
        return piso == otro.piso && habitacion == otro.habitacion;
    }

    // Operador de desigualdad
    bool operator<(const Nodo& otro) const {
        return std::tie(piso, habitacion) < std::tie(otro.piso, otro.habitacion);
    }
};


// Hash personalizado para usar Nodo como clave en unordered_map
struct NodoHash {
    size_t operator()(const Nodo& n) const {
        return std::hash<int>()(n.piso * 100 + n.habitacion);
    }
};

// Grafo: cada nodo se conecta con vecinos con un costo (peso)
using GrafoHotel = std::unordered_map<Nodo, std::vector<std::pair<Nodo, int>>, NodoHash>;

// Construye el grafo conectando habitaciones por pasillos, escaleras y ascensor
void construirGrafo(GrafoHotel& grafo, int pisos, int habitacionesPorPiso);

// Usa Dijkstra para hallar el camino más corto entre dos nodos
std::vector<Nodo> dijkstraCaminoMasCorto(const GrafoHotel& grafo, Nodo inicio, Nodo destino);

#endif 