#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1 // нужно для Weak::MD5
#include "authenticator.h"
#include <algorithm>
#include <cryptopp/hex.h>
#include <cryptopp/md5.h>
#include <cryptopp/filters.h>

namespace CPP = CryptoPP;

bool Authenticator::isHex(const std::string& s)
{
    return std::all_of(s.begin(), s.end(),
        [](unsigned char c) { return std::isxdigit(c) != 0; });
}

bool Authenticator::verify(const std::string& salt16,
                           const std::string& password,
                           const std::string& hash16)
{
    CPP::Weak::MD5 md5;              // хеш-функция
    std::string hash;                // двоичный хеш
    bool isVerified = false;         // результат проверки

    // хеш из шестнадцатеричной строки в двоичный вид
    CPP::StringSource(hash16, true,
        new CPP::HexDecoder(
            new CPP::StringSink(hash)));
    // двоичный хеш + соль + пароль -> проверка
    CPP::StringSource(hash.append(salt16).append(password), true,
        new CPP::HashVerificationFilter(
            md5,
            new CPP::ArraySink(reinterpret_cast<CPP::byte*>(&isVerified),
                               sizeof isVerified),
            CPP::HashVerificationFilter::HASH_AT_BEGIN |
            CPP::HashVerificationFilter::PUT_RESULT));
    return isVerified;
}
