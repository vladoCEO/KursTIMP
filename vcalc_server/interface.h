#pragma once
#include <boost/program_options.hpp>
#include <cstdint>
#include <string>

namespace po = boost::program_options;

// структура для хранения полученных параметров
struct Params {
    std::string baseFile; // файл с базой клиентов
    std::string logFile;  // файл журнала
    uint16_t port;        // порт сервера
};

// Интерфейс командной строки
class Interface {
private:
    // описание параметров
    po::options_description desc;
    // переменная для результатов парсинга
    po::variables_map vm;
    // полученные параметры
    Params params;
public:
    Interface();
    // разбор параметров: true - параметры получены, false - нужна справка
    bool Parser(int argc, const char** argv);
    std::string getDescription();
    Params getParams() {
        return params;
    };
};
