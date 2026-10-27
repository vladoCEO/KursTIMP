#include "auth_base.h"
#include "errors.h"
#include <fstream>

AuthBase::AuthBase(const std::string& fileName)
{
    std::ifstream f(fileName);
    if (!f.is_open())
        throw IOError("Cannot open clients base: " + fileName, true);
    std::string line;
    while (std::getline(f, line)) {
        if (!line.empty() && line.back() == '\r')
            line.pop_back();
        size_t pos = line.find(':');
        // строки неверного формата пропускаем
        if (pos == std::string::npos || pos == 0)
            continue;
        users[line.substr(0, pos)] = line.substr(pos + 1);
    }
    if (users.empty())
        throw IOError("Clients base is empty or has wrong format: " + fileName, true);
}

bool AuthBase::getPassword(const std::string& login, std::string& pass) const
{
    auto it = users.find(login);
    if (it == users.end())
        return false;
    pass = it->second;
    return true;
}
