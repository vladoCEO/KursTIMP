#pragma once
#include <map>
#include <string>

// База клиентов: файл со строками вида логин:пароль
class AuthBase {
private:
    std::map<std::string, std::string> users; // логин -> пароль
public:
    // читает файл, при ошибке исключение IOError (критическое)
    explicit AuthBase(const std::string& fileName);
    // поиск пароля по логину, true если логин найден
    bool getPassword(const std::string& login, std::string& pass) const;
    size_t size() const { return users.size(); }
};
