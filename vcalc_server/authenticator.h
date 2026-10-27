#pragma once
#include <string>

// Аутентификация: HASH = MD5(SALT16 + PASSWORD) в шестнадцатеричном виде
class Authenticator {
public:
    // строка состоит только из шестнадцатеричных цифр?
    static bool isHex(const std::string& s);
    // проверка хеша, полученного от клиента
    static bool verify(const std::string& salt16,
                       const std::string& password,
                       const std::string& hash16);
};
