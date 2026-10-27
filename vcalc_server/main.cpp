#include "auth_base.h"
#include "errors.h"
#include "interface.h"
#include "logger.h"
#include "server.h"
#include <iostream>

int main(int argc, const char** argv)
{
    try {
        Interface iface;
        if (!iface.Parser(argc, argv)) {
            std::cout << iface.getDescription();
            return 0;
        }
        Params params = iface.getParams();
        Logger log(params.logFile);
        try {
            AuthBase base(params.baseFile);
            Server server(params.port, base, log);
            server.run();
        } catch (ProgramError& e) {
            // критическая ошибка запуска - пишем в журнал и завершаемся
            log.write(true, e.what());
            throw;
        }
    } catch (po::error& e) {
        std::cerr << "error: " << e.what() << "\n";
        std::cerr << "Use -h for help\n";
        return 1;
    } catch (std::exception& e) {
        std::cerr << "error: " << e.what() << "\n";
        return 2;
    }
    return 0;
}
