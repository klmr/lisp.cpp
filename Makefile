CXXFLAGS+=-std=c++11 -pedantic -Werror -Wall -Wextra -Iinclude

main: main.cpp eval.cpp environment.cpp value.cpp
