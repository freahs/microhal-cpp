
CXX_FLAGS = -fdiagnostics-color=always -std=c++14 -Wfatal-errors -Wall -Wextra -pedantic -Wshadow -g -ftemplate-backtrace-limit=0
all:
	g++ microhal.cpp $(CXX_FLAGS) -o microhal
