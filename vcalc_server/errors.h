#pragma once
#include <stdexcept>
#include <string>

// Базовый класс ошибок программы, хранит признак критичности
class ProgramError : public std::runtime_error {
private:
    bool critical;
public:
    ProgramError(const std::string& msg, bool crit = false)
        : std::runtime_error(msg), critical(crit) {}
    bool isCritical() const { return critical; }
};

// Ошибки работы с файлами
class IOError : public ProgramError {
public:
    using ProgramError::ProgramError;
};

// Сетевые ошибки
class NetworkError : public ProgramError {
public:
    using ProgramError::ProgramError;
};

// Ошибки аутентификации
class AuthError : public ProgramError {
public:
    using ProgramError::ProgramError;
};
