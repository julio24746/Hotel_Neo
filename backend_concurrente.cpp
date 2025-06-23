// backend.cpp
#include "backend_concurrente.h"
#include <string>
#include <mutex>
#include "Modelado_hotel/Modelado_hotel.h"
#include "pathfinding/pathfinding.h"
#include "Modelado_hotel/Grafos_hotel.h"

 Hotel hotel(10, 10);
std::mutex mtx_hotel;
std::mutex mtx_grafo;
std::mutex mtx_id;

static GrafoHotel grafo;
static bool grafo_inicializado = false;


json procesarReserva(const json& j) {
    int reserva_id;

    if (!j.contains("nombre") || !j.contains("fecha_ingreso") ||
        !j.contains("fecha_salida") || !j.contains("evento") || !j.contains("tipo")) {
        return {
            {"success", false},
            {"mensaje", "Faltan campos obligatorios"}
        };
    }

    {
        std::lock_guard<std::mutex> lock(mtx_grafo);
        if (!grafo_inicializado) {
            construirGrafo(grafo, 10, 10);
            grafo_inicializado = true;
        }
    }

    std::string tipo = j["tipo"];
    Habitacion* hab = nullptr;
    {
        std::lock_guard<std::mutex> lock(mtx_hotel);
        hab = hotel.buscarHabitacionDisponible(tipo);
        if (hab) hab->ocupada = true;
    }
    if (!hab) {
        return {
            {"success", false},
            {"mensaje", "Lo sentimos, no hay habitaciones disponibles"}
        };
    }

    static int id = 1;

    {
        std::lock_guard<std::mutex> lock(mtx_id);
        reserva_id = id++;
    }

    std::vector<json> ocupadas_json;
    {
        std::lock_guard<std::mutex> lock(mtx_hotel);
        for (int p = 0; p < 10; ++p) {
            for (int h = 0; h < 10; ++h) {
                Habitacion& actual = hotel.getHabitacion(p, h);
                if (actual.ocupada) {
                    ocupadas_json.push_back({{"piso", p}, {"habitacion", h}});
                }
            }
        }
    }

    // Calcular ruta desde recepción (0,0) hasta la habitación asignada
    Nodo entrada = {0, 0};
    Nodo destino = {hab->piso, hab->numero};
    auto ruta = dijkstraCaminoMasCorto(grafo, entrada, destino);

    std::vector<json> ruta_json;
    for (const auto& paso : ruta) {
        ruta_json.push_back({{"piso", paso.piso}, {"habitacion", paso.habitacion}});
    }

    return {
        {"success", true},
        {"reserva_id", reserva_id},
        {"habitacion", {
            {"piso", hab->piso},
            {"numero", hab->numero}
        }},
        {"ruta", ruta_json},
        {"ocupadas", ocupadas_json},
        {"mensaje", "Reserva exitosa para " + std::string(j["nombre"])}
    };
}
