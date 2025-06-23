#ifndef BACKEND_H
#define BACKEND_H

#include "nlohmann/json.hpp"  // para usar json::object

using json = nlohmann::json;

// Declaración de la función que estará implementada en backend.cpp
json procesarReserva(const json& j);

#endif