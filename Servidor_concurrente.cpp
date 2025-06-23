// Servidor.cpp unificado y mejorado
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sstream>
#include <string>
#include <thread>
#include <mutex>
#include "../libreria/nlohmann/json.hpp"
#include "../Backend/backend_concurrente.h"


using json = nlohmann::json;
std::mutex mtx;

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cerr << "Error al crear socket\n";
        return 1;
    }

    sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(8080);

    if (bind(sockfd, (sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "Error bind\n";
        close(sockfd);
        return 1;
    }

    if (listen(sockfd, 5) < 0) {
        std::cerr << "Error listen\n";
        close(sockfd);
        return 1;
    }

    std::cout << "Servidor escuchando en puerto 8080...\n";

    
while (true) {
    sockaddr_in cli;
    socklen_t L = sizeof(cli);
    int cs = accept(sockfd, (sockaddr*)&cli, &L);
    if (cs < 0) {
        std::cerr << "Error accept";
        continue;
    }

    auto atenderCliente = [](int cs) {
        char buf[4096];
        ssize_t n = read(cs, buf, sizeof(buf) - 1);
        if (n <= 0) {
            close(cs);
            return;
        }
        buf[n] = '\0';

        std::istringstream req(buf);
        std::string method, path, ver;
        req >> method >> path >> ver;

        std::string hdr, resp;

        if (method == "OPTIONS") {
           hdr = "HTTP/1.1 204 No Content\r\n";
            hdr += "Access-Control-Allow-Origin: *\r\n";
            hdr += "Access-Control-Allow-Methods: POST, GET, OPTIONS\r\n";
            hdr += "Access-Control-Allow-Headers: Content-Type\r\n";
            hdr += "Content-Length: 0\r\n\r\n";
            write(cs, hdr.c_str(), hdr.size());
            close(cs);
         return;
        }

      if (method == "GET" && path == "/") {
            resp = "Buenas mi estimado, aqui tu servidor.";
            hdr = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n";
            hdr += "Access-Control-Allow-Origin: *\r\n";
            hdr += "Content-Length: " + std::to_string(resp.size()) + "\r\n\r\n";
        }
        else if (method == "POST" && path == "/api/reservas") {
            std::string s(buf);
            auto p = s.find("\r\n\r\n");
            if (p == std::string::npos) p = s.find("\n\n");
            std::string jb = (p != std::string::npos ? s.substr(p + 4) : "");

            json j;
            try {
                j = json::parse(jb);
            } catch (...) {
                resp = R"({"success":false,"mensaje":"JSON inválido"})";
                hdr = "HTTP/1.1 400 Bad Request\r\nContent-Type: application/json\r\n";
                hdr += "Access-Control-Allow-Origin: *\r\n";
                hdr += "Content-Length: " + std::to_string(resp.size()) + "";
                write(cs, hdr.c_str(), hdr.size());
                write(cs, resp.c_str(), resp.size());
                close(cs);
                return;
            }

            json r;
            {
                std::lock_guard<std::mutex> lock(mtx);
                r = procesarReserva(j);
            }

            resp = r.dump();
            hdr = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n";
            hdr += "Access-Control-Allow-Origin: *\r\n";
            hdr += "Content-Length: " + std::to_string(resp.size()) + "\r\n\r\n";
        
        // Mostrar datos bonitos por consola
                try {
                    std::string nombre = j.at("nombre");
                    std::string ingreso = j.at("fecha_ingreso");
                    std::string salida = j.at("fecha_salida");
                    std::string evento = j.at("evento");
                    std::string tipo = j.at("tipo");

                    std::cout << "\n\033[1;34m===== Reserva recibida =====\033[0m\n";
                    std::cout << "ðŸ‘¤ Nombre: " << nombre << "\n";
                    std::cout << "ðŸ“… Ingreso: " << ingreso << "\n";
                    std::cout << "ðŸ“… Salida: " << salida << "\n";
                    std::cout << "ðŸŽ‰ Evento: " << evento << "\n";
                    std::cout << "ðŸ›ï¸ Tipo de habitaciÃ³n: " << tipo << "\n";
                    std::cout << "=============================\n";
                } catch (...) {
                    std::cerr << "Error: No se pudieron imprimir los datos recibidos.\n";
                }
        
        }

        if (!hdr.empty()) {
            write(cs, hdr.c_str(), hdr.size());
            write(cs, resp.c_str(), resp.size());
        }

        close(cs);
       // std::cout << method << " " << path << " -> " << resp << "\n";

        
    };

    std::thread clienteThread(atenderCliente, cs);
    clienteThread.detach();
}
close(sockfd);

    return 0;
}
