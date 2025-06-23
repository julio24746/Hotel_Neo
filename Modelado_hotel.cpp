#include "Modelado_hotel.h"
#include <iostream>

// Constructor del hotel con n pisos y m habitaciones por piso
Hotel::Hotel(int pisos, int habitacionesPorPiso)
    : pisos_(pisos), habitacionesPorPiso_(habitacionesPorPiso) 
{
    habitaciones_.resize(pisos_);
    for (int piso = 0; piso < pisos_; ++piso) {
        for (int num = 0; num < habitacionesPorPiso_; ++num) {
            Habitacion h;
            h.piso = piso;
            h.numero = num;
            h.ocupada = false;
            h.tipo = (num % 3 == 0) ? "suite" : (num % 2 == 0 ? "doble" : "individual");
            habitaciones_[piso].push_back(h);
        }
    }
}

// Busca una habitación disponible opcionalmente por tipo
Habitacion* Hotel::buscarHabitacionDisponible(const std::string& tipo) {
    for (int piso = 0; piso < pisos_; ++piso) {
        for (int hab = 0; hab < habitacionesPorPiso_; ++hab) {
            Habitacion& h = habitaciones_[piso][hab];
            if (!h.ocupada && (tipo.empty() || h.tipo == tipo)) {
                return &h;
            }
        }
    }
    return nullptr;
}

// Marca una habitación como ocupada
void Hotel::marcarOcupada(int piso, int numero) {
    if (piso >= 0 && piso < pisos_ && numero >= 0 && numero < habitacionesPorPiso_) {
        habitaciones_[piso][numero].ocupada = true;
    }
}

// Imprime estado de todas las habitaciones
void Hotel::imprimirEstadoHotel() const {
    for (int piso = 0; piso < pisos_; ++piso) {
        std::cout << "Piso " << piso << ":\n";
        for (const auto& h : habitaciones_[piso]) {
            std::cout << "  Habitación " << h.numero 
                      << " (" << h.tipo << ") - " 
                      << (h.ocupada ? "Ocupada" : "Libre") << "\n";
        }
    }
}

/*Habitacion& Hotel::getHabitacion(int piso, int numero) {
    return habitaciones_[piso][numero];
}
*/


//Nos ayudara a evitar errores
Habitacion& Hotel::getHabitacion(int piso, int numero) {
    if (piso >= 0 && piso < pisos_ && numero >= 0 && numero < habitacionesPorPiso_) {
        return habitaciones_[piso][numero];
    }
    throw std::out_of_range("Habitación inválida");
}
