CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
LIBS = -lboost_program_options -lcryptopp
SRC = main.cpp interface.cpp logger.cpp auth_base.cpp authenticator.cpp calculator.cpp server.cpp

vcalc: $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o vcalc $(LIBS)

clean:
	rm -f vcalc
