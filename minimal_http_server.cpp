#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sstream>
#include <string>
#include "nlohmann/json.hpp"
using json = nlohmann::json;

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) { std::cerr<<"Error al crear socket\n"; return 1; }

    sockaddr_in addr; std::memset(&addr,0,sizeof(addr));
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=INADDR_ANY;
    addr.sin_port=htons(8080);
    if (bind(sockfd,(sockaddr*)&addr,sizeof(addr))<0){std::cerr<<"Error bind\n";close(sockfd);return 1;}
    if (listen(sockfd,5)<0){std::cerr<<"Error listen\n";close(sockfd);return 1;}
    std::cout<<"Servidor en puerto 8080...\n";

    while(true){
        sockaddr_in cli; socklen_t L=sizeof(cli);
        int cs=accept(sockfd,(sockaddr*)&cli,&L);
        if (cs<0) { std::cerr<<"Error accept\n"; continue; }

        char buf[4096]; ssize_t n=read(cs,buf,sizeof(buf)-1);
        if(n<=0){ close(cs); continue; }
        buf[n]='\0';

        std::istringstream req(buf);
        std::string method,path,ver;
        req>>method>>path>>ver;

        std::string body, hdr, resp;
        if(method=="GET" && path=="/"){
            resp="Hola, este es un servidor.";
            hdr="HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n";
        }
        else if(method=="POST" && path=="/api/reservas"){
            std::string s(buf);
            auto p=s.find("\r\n\r\n");
            std::string jb = (p!=std::string::npos? s.substr(p+4):"");
            json j;
            try{ j=json::parse(jb); }
            catch(...){
                resp=R"({"success":false,"mensaje":"JSON inválido"})";
                hdr="HTTP/1.1 400 Bad Request\r\nContent-Type: application/json\r\n";
                goto SEND;
            }
            if(!j.contains("nombre")||!j.contains("fecha_ingreso")||!j.contains("fecha_salida")){
                resp=R"({"success":false,"mensaje":"Faltan campos"})";
                hdr="HTTP/1.1 400 Bad Request\r\nContent-Type: application/json\r\n";
                goto SEND;
            }
            std::string nombre=j["nombre"];
            static int id=1;
            json r={ {"success",true},{"reserva_id",id++},{"mensaje","Reserva creada para "+nombre} };
            resp=r.dump();
            hdr="HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n";
        }
        else {
            resp="404 Not Found";
            hdr="HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n";
        }

    SEND:
        hdr+="Content-Length: "+std::to_string(resp.size())+"\r\n\r\n";
        write(cs,hdr.c_str(),hdr.size());
        write(cs,resp.c_str(),resp.size());
        close(cs);
        std::cout<<method<<" "<<path<<" -> "<<resp<<"\n";
    }
    close(sockfd);
    return 0;
}
