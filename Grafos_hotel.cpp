#include "Grafos_hotel.h"

void construirGrafo(GrafoHotel& grafo, int pisos, int habitacionesPorPiso) {
    for (int p = 0; p < pisos; ++p) {
        for (int h = 0; h < habitacionesPorPiso; ++h) {
            Nodo actual = {p, h};

            // Pasillos
            if (h > 0)
                grafo[actual].push_back({{p, h - 1}, 1});
            if (h < habitacionesPorPiso - 1)
                grafo[actual].push_back({{p, h + 1}, 1});

            // Escaleras
            if (p > 0)
                grafo[actual].push_back({{p - 1, h}, 2});
            if (p < pisos - 1)
                grafo[actual].push_back({{p + 1, h}, 2});

            // Ascensor (en columna 0 por simplicidad)
            if (h == 0) {
                for (int q = 0; q < pisos; ++q) {
                    if (q != p)
                        grafo[actual].push_back({{q, h}, 1});
                }
            }
        }
    }
}

std::vector<Nodo> dijkstraCaminoMasCorto(const GrafoHotel& grafo, Nodo inicio, Nodo destino) {
    std::unordered_map<Nodo, int, NodoHash> distancias;
    std::unordered_map<Nodo, Nodo, NodoHash> predecesores;
    std::priority_queue<std::pair<int, Nodo>, std::vector<std::pair<int, Nodo>>, std::greater<>> cola;

    for (const auto& par : grafo) {
        distancias[par.first] = std::numeric_limits<int>::max();
    }

    distancias[inicio] = 0;
    cola.push({0, inicio});

    while (!cola.empty()) {
        auto [dist, actual] = cola.top();
        cola.pop();

        if (actual == destino) break;

        for (const auto& [vecino, peso] : grafo.at(actual)) {
            int nuevaDist = distancias[actual] + peso;
            if (nuevaDist < distancias[vecino]) {
                distancias[vecino] = nuevaDist;
                predecesores[vecino] = actual;
                cola.push({nuevaDist, vecino});
            }
        }
    }

    std::vector<Nodo> camino;
    Nodo actual = destino;
    while (actual < inicio) {
        camino.push_back(actual);
        if (predecesores.find(actual) == predecesores.end()) {
            return {}; // No hay camino
        }
        actual = predecesores[actual];
    }
    camino.push_back(inicio);
    std::reverse(camino.begin(), camino.end());
    return camino;
}
