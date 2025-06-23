#ifndef MODELADO_HOTEL_H
#define MODELADO_HOTEL_H

#include <vector>
#include <string>

// Representa una habitación dentro del hotel
struct Habitacion {
    int piso;
    int numero;
    bool ocupada;
    std::string tipo; // Por ejemplo: "simple", "doble", "suite"
};

// Representa el hotel como una matriz de habitaciones
class Hotel {
public:
    Hotel(int pisos, int habitacionesPorPiso);
Habitacion& getHabitacion(int piso, int numero);

    Habitacion* buscarHabitacionDisponible(const std::string& tipo = "");
    void marcarOcupada(int piso, int numero);
    void imprimirEstadoHotel() const;

private:
    int pisos_;
    int habitacionesPorPiso_;
    std::vector<std::vector<Habitacion>> habitaciones_;
};

extern Hotel hotel;
#endif 