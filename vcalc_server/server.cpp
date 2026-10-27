#include "server.h"
#include "authenticator.h"
#include "calculator.h"
#include "errors.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <memory>

namespace {
constexpr int timeoutSec = 180;    // таймаут приема
constexpr size_t saltLen = 16;     // длина SALT16
constexpr size_t hashLen = 32;     // длина MD5 в hex

std::string sysErr(const std::string& what)
{
    return what + ": " + std::strerror(errno);
}

// MSG_NOSIGNAL - чтобы разрыв соединения клиентом не убивал сервер сигналом SIGPIPE
void sendAll(int s, const void* data, size_t size)
{
    ssize_t rc = send(s, data, size, MSG_NOSIGNAL);
    if (rc == -1)
        throw NetworkError(sysErr("send"));
    if (static_cast<size_t>(rc) != size)
        throw NetworkError("send: incomplete data sent");
}

// MSG_WAITALL - ждать все запрошенные байты (большие блоки приходят частями)
void recvAll(int s, void* data, size_t size)
{
    ssize_t rc = recv(s, data, size, MSG_WAITALL);
    if (rc == -1)
        throw NetworkError(sysErr("recv"));
    if (static_cast<size_t>(rc) != size)
        throw NetworkError("recv: connection closed or incomplete data received");
}
} // namespace

Server::Server(uint16_t port, const AuthBase& b, Logger& l)
    : listen_sock(-1), base(b), log(l)
{
    listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock == -1)
        throw NetworkError(sysErr("socket"), true);

    // чтобы после перезапуска порт не был занят состоянием TIME_WAIT
    int on = 1;
    setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof on);

    std::unique_ptr<sockaddr_in> self_addr(new sockaddr_in{});
    self_addr->sin_family = AF_INET;
    self_addr->sin_port = htons(port);
    self_addr->sin_addr.s_addr = htonl(INADDR_ANY);

    int rc = bind(listen_sock, reinterpret_cast<const sockaddr*>(self_addr.get()),
                  sizeof(sockaddr_in));
    if (rc == -1) {
        std::string msg = sysErr("bind");
        close(listen_sock);
        throw NetworkError(msg, true);
    }
    rc = listen(listen_sock, SOMAXCONN);
    if (rc == -1) {
        std::string msg = sysErr("listen");
        close(listen_sock);
        throw NetworkError(msg, true);
    }
}

Server::~Server()
{
    if (listen_sock != -1)
        close(listen_sock);
}

void Server::authenticate(int sock)
{
    char buf[1024];
    ssize_t rc = recv(sock, buf, sizeof buf, 0);
    if (rc == -1)
        throw NetworkError(sysErr("recv auth message"));
    if (rc == 0)
        throw NetworkError("client closed connection before authentication");

    // MSG = LOGIN || SALT16 || HASH16
    std::string msg(buf, rc);
    if (msg.size() <= saltLen + hashLen) {
        sendAll(sock, "ERR", 3);
        throw AuthError("wrong auth message format");
    }
    size_t loginLen = msg.size() - saltLen - hashLen;
    std::string login = msg.substr(0, loginLen);
    std::string salt = msg.substr(loginLen, saltLen);
    std::string hash = msg.substr(loginLen + saltLen);

    std::string pass;
    bool ok = Authenticator::isHex(salt) && Authenticator::isHex(hash)
              && base.getPassword(login, pass)
              && Authenticator::verify(salt, pass, hash);
    if (!ok) {
        sendAll(sock, "ERR", 3);
        throw AuthError("authentication failed, login: " + login);
    }
    sendAll(sock, "OK", 2);
}

void Server::serve(int sock)
{
    authenticate(sock);

    uint32_t count;                       // число векторов
    recvAll(sock, &count, sizeof count);
    for (uint32_t i = 0; i < count; ++i) {
        uint32_t size;                    // размер вектора
        recvAll(sock, &size, sizeof size);
        std::unique_ptr<int32_t[]> data(new int32_t[size]);
        if (size > 0)
            recvAll(sock, data.get(), size * sizeof data[0]);
        int32_t result = Calculator::product(data.get(), size);
        sendAll(sock, &result, sizeof result);
    }
}

void Server::run()
{
    while (true) {
        sockaddr_in client_addr{};
        socklen_t len = sizeof client_addr;
        int sock = accept(listen_sock, reinterpret_cast<sockaddr*>(&client_addr), &len);
        if (sock == -1) {
            log.write(false, sysErr("accept"));
            continue;
        }
        std::string ip = inet_ntoa(client_addr.sin_addr);

        timeval timeout{timeoutSec, 0};
        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof timeout);

        // ошибки одного клиента не должны останавливать сервер
        try {
            serve(sock);
        } catch (ProgramError& e) {
            log.write(e.isCritical(), "client " + ip + ": " + e.what());
        } catch (std::exception& e) {
            log.write(false, "client " + ip + ": " + e.what());
        }
        close(sock);
    }
}
