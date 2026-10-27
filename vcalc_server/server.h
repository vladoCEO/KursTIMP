#pragma once
#include "auth_base.h"
#include "logger.h"
#include <cstdint>
#include <string>

// Сетевая часть сервера (однопоточный TCP-сервер)
class Server {
private:
    int listen_sock;      // слушающий сокет
    const AuthBase& base; // база клиентов
    Logger& log;          // журнал
    void authenticate(int sock);
    void serve(int sock);
public:
    // создает сокет, привязывает порт, при ошибке NetworkError (критическая)
    Server(uint16_t port, const AuthBase& base, Logger& log);
    ~Server();
    // бесконечный цикл приема клиентов
    void run();
};
