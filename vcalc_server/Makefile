CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
LIBS = -lboost_program_options -lcryptopp
SRC = main.cpp interface.cpp logger.cpp auth_base.cpp authenticator.cpp calculator.cpp server.cpp

vcalc: $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o vcalc $(LIBS)

TESTSRC = tests.cpp interface.cpp logger.cpp auth_base.cpp authenticator.cpp calculator.cpp

tests: $(TESTSRC)
	$(CXX) $(CXXFLAGS) $(TESTSRC) -o tests $(LIBS) -lUnitTest++

clean:
	rm -f vcalc tests
