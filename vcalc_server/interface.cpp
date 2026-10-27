#include "interface.h"
#include <sstream>

Interface::Interface() : desc("Allowed options")
{
    desc.add_options()
        ("help,h", "Show help")
        ("base,b",
         po::value<std::string>(&params.baseFile)->default_value("./vcalc.conf"),
         "Set clients base file name")
        ("log,l",
         po::value<std::string>(&params.logFile)->default_value("./vcalc.log"),
         "Set log file name")
        ("port,p",
         po::value<uint16_t>(&params.port)->default_value(33333),
         "Set server port");
}

bool Interface::Parser(int argc, const char** argv)
{
    // запуск без параметров - справка
    if (argc <= 1)
        return false;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    // --help обрабатываем до notify
    if (vm.count("help"))
        return false;
    po::notify(vm);
    if (params.port == 0)
        throw po::error("port must be in range 1..65535");
    return true;
}

std::string Interface::getDescription()
{
    std::ostringstream ss;
    ss << "Usage: vcalc [options]\n" << desc;
    return ss.str();
}
