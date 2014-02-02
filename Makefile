CXXFLAGS+=-std=c++11 -pedantic -Werror -Wall -Wextra -Iinclude
CC=${CXX}

main: main.o eval.o environment.o value.o

read: read.o value.o environment.o

eval.o: include/eval.hpp

environment.o: include/environment.hpp

value.o: include/value.hpp
