#pragma once
#include <fstream>
#include <string>

// Журнал работы программы
class Logger {
private:
    std::ofstream f;
public:
    // открывает файл журнала, при ошибке исключение IOError
    explicit Logger(const std::string& name);
    // запись: дата и время | критичность | параметры ошибки
    void write(bool critical, const std::string& msg);
};
