#if __has_include(<unittest++/UnitTest++.h>)
#include <unittest++/UnitTest++.h>
#else
#include <UnitTest++/UnitTest++.h>
#endif
#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "interface.h"
#include "calculator.h"
#include "auth_base.h"
#include "authenticator.h"
#include "logger.h"
#include "errors.h"

// Модуль interface

// сценарий тестирования справки
SUITE(HelpTest)
{
    TEST(ShortHelp) {
        Interface iface;
        const char* argv[] = {"test", "-h", nullptr};
        int argc = sizeof argv / sizeof argv[0] - 1;
        CHECK(!iface.Parser(argc, argv));
        CHECK(!iface.getDescription().empty());
    }
    TEST(LongHelp) {
        Interface iface;
        const char* argv[] = {"test", "--help", nullptr};
        int argc = sizeof argv / sizeof argv[0] - 1;
        CHECK(!iface.Parser(argc, argv));
        CHECK(!iface.getDescription().empty());
    }
    TEST(HelpAndPort) {
        Interface iface;
        const char* argv[] = {"test", "-h", "-p", "1000", nullptr};
        int argc = sizeof argv / sizeof argv[0] - 1;
        CHECK(!iface.Parser(argc, argv));
    }
    TEST(NoParams) {
        Interface iface;
        const char* argv[] = {"test", nullptr};
        int argc = sizeof argv / sizeof argv[0] - 1;
        CHECK(!iface.Parser(argc, argv));
    }
}

// сценарий тестирования рабочих параметров
SUITE(WorkParamsTest)
{
    TEST(PortOnly) {
        Interface iface;
        const char* argv[] = {"test", "-p", "5000", nullptr};
        int argc = sizeof argv / sizeof argv[0] - 1;
        CHECK(iface.Parser(argc, argv));
        CHECK_EQUAL("./vcalc.conf", iface.getParams().baseFile);
        CHECK_EQUAL("./vcalc.log", iface.getParams().logFile);
        CHECK_EQUAL(5000, iface.getParams().port);
    }
    TEST(AllParams) {
        Interface iface;
        const char* argv[] = {"test", "-b", "a.conf", "--log", "a.log",
                              "-p", "1234", nullptr};
        int argc = sizeof argv / sizeof argv[0] - 1;
        CHECK(iface.Parser(argc, argv));
        CHECK_EQUAL("a.conf", iface.getParams().baseFile);
        CHECK_EQUAL("a.log", iface.getParams().logFile);
        CHECK_EQUAL(1234, iface.getParams().port);
    }
}

// сценарий тестирования исключений
SUITE(ExceptionTest)
{
    TEST(PortNotNumber) {
        Interface iface;
        const char* argv[] = {"test", "-p", "abc", nullptr};
        int argc = sizeof argv / sizeof argv[0] - 1;
        CHECK_THROW(iface.Parser(argc, argv), po::invalid_option_value);
    }
    TEST(PortZero) {
        Interface iface;
        const char* argv[] = {"test", "-p", "0", nullptr};
        int argc = sizeof argv / sizeof argv[0] - 1;
        CHECK_THROW(iface.Parser(argc, argv), po::error);
    }
    TEST(UnknownParam) {
        Interface iface;
        const char* argv[] = {"test", "-p", "5000", "-v", nullptr};
        int argc = sizeof argv / sizeof argv[0] - 1;
        CHECK_THROW(iface.Parser(argc, argv), po::unknown_option);
    }
}

// Модуль calculator

// вспомогательная функция: произведение значений вектора
static int32_t calc(std::vector<int32_t> v)
{
    return Calculator::product(v.data(), v.size());
}

SUITE(CalculatorTest)
{
    TEST(PositiveProduct) {
        CHECK_EQUAL(24, calc({1, 2, 3, 4}));
    }
    TEST(NegativeProduct) {
        CHECK_EQUAL(-10, calc({-2, 5}));
    }
    TEST(OverflowUp) {
        CHECK_EQUAL(INT32_MAX, calc({100000, 100000}));
    }
    TEST(OverflowDown) {
        CHECK_EQUAL(INT32_MIN, calc({-100000, 100000}));
    }
    TEST(ZeroAfterOverflow) {
        CHECK_EQUAL(0, calc({100000, 100000, 0}));
    }
}

//  Модуль auth_base

#define BASE_FILE "test_base.conf"

// фикстура: создает файл базы с двумя клиентами
struct BaseFile {
    BaseFile() {
        std::ofstream f(BASE_FILE);
        f << "user:P@ssW0rd\nvasya:12345\n";
    }
    ~BaseFile() {
        std::remove(BASE_FILE);
    }
};

// фикстура: создает пустой файл
struct EmptyBaseFile {
    EmptyBaseFile() {
        std::ofstream f(BASE_FILE);
    }
    ~EmptyBaseFile() {
        std::remove(BASE_FILE);
    }
};

SUITE(AuthBaseTest)
{
    TEST_FIXTURE(BaseFile, FindClient) {
        AuthBase base(BASE_FILE);
        std::string pass;
        std::string expected("P@ssW0rd");
        CHECK(base.getPassword("user", pass));
        CHECK_EQUAL(expected, pass);
    }
    TEST_FIXTURE(BaseFile, UnknownClient) {
        AuthBase base(BASE_FILE);
        std::string pass;
        CHECK(!base.getPassword("petya", pass));
    }
    TEST(NoFile) {
        CHECK_THROW(AuthBase base("notfound.conf"), IOError);
    }
    TEST_FIXTURE(EmptyBaseFile, EmptyFile) {
        CHECK_THROW(AuthBase base(BASE_FILE), IOError);
    }
}

// Модуль authenticator

// тестовые значения; хеш получен командой
// echo -n '4F9C429F5C6884DBP@ssW0rd' | openssl md5
static const std::string salt("4F9C429F5C6884DB");
static const std::string pass("P@ssW0rd");
static const std::string hash("C0A6929E87E6A931E0C499810C8B2CC8");

SUITE(AuthenticatorTest)
{
    TEST(CorrectHash) {
        CHECK(Authenticator::verify(salt, pass, hash));
    }
    TEST(WrongHash) {
        std::string bad(hash);
        bad[0] = (bad[0] == '0') ? '1' : '0';
        CHECK(!Authenticator::verify(salt, pass, bad));
    }
    TEST(WrongPassword) {
        CHECK(!Authenticator::verify(salt, "P@ssw0rd", hash));
    }
    TEST(HexCheck) {
        CHECK(Authenticator::isHex("4F9c0"));
        CHECK(!Authenticator::isHex("4G9C"));
    }
}

//  Модуль logger

#define LOG_FILE "test.log"

// читает весь файл в строку
static std::string readFile(const char* name)
{
    std::ifstream f(name);
    std::stringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

SUITE(LoggerTest)
{
    TEST(NonCriticalRecord) {
        std::remove(LOG_FILE);
        {
            Logger log(LOG_FILE);
            log.write(false, "test message");
        }
        std::string text = readFile(LOG_FILE);
        CHECK(text.find("| NON-CRITICAL |") != std::string::npos);
        CHECK(text.find("test message") != std::string::npos);
        std::remove(LOG_FILE);
    }
    TEST(CriticalRecord) {
        std::remove(LOG_FILE);
        {
            Logger log(LOG_FILE);
            log.write(true, "test message");
        }
        std::string text = readFile(LOG_FILE);
        CHECK(text.find("| CRITICAL |") != std::string::npos);
        std::remove(LOG_FILE);
    }
}

int main()
{
    return UnitTest::RunAllTests();
}
