TARGET=camera_server
FLAGS=-std=c++11 -Wall

camera_server: main.o camera_server.o abstract_server.o
	$(CXX) $(FLAGS) -o camera_server main.o camera_server.o abstract_server.o

main.o: main.cpp camera_server.hpp
	$(CXX) $(FLAGS) -c main.cpp

camera_server.o: camera_server.hpp camera_server.cpp
	$(CXX) $(FLAGS) -c camera_server.cpp

abstract_server.o: abstract_server.hpp abstract_server.cpp
	$(CXX) $(FLAGS) -c abstract_server.cpp

.PHONY: clean
clean:
	rm camera_server *.o
