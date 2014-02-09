CXXFLAGS+=-std=c++11 -pedantic -Werror -Wall -Wextra -Iinclude -DNDEBUG -O3 -flto
CC=${CXX}

main: main.o eval.o environment.o value.o read.o

eval.o: include/eval.hpp

environment.o: include/environment.hpp

value.o: include/value.hpp
