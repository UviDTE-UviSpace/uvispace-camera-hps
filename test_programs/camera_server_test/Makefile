TARGET=camera_server
#CROSSCOMPILE=
CROSSCOMPILE=arm-linux-gnueabihf-

camera_server: main.o camera_server.o abstract_server.o avalon_camera.o
	$(CROSSCOMPILE)g++ -std=c++11 -Wall -o camera_server main.o camera_server.o abstract_server.o avalon_camera.o

main.o: main.cpp camera_server.hpp
	$(CROSSCOMPILE)g++ -std=c++11 -c -fpermissive main.cpp

camera_server.o: camera_server.hpp camera_server.cpp
	$(CROSSCOMPILE)g++ -std=c++11 -c -fpermissive camera_server.cpp

abstract_server.o: abstract_server.hpp abstract_server.cpp
	$(CROSSCOMPILE)g++ -std=c++11 -c abstract_server.cpp

avalon_camera.o: avalon_camera.hpp avalon_camera.cpp
	$(CROSSCOMPILE)g++ -std=c++11 -c -fpermissive avalon_camera.cpp

.PHONY: clean
clean:
	rm camera_server *.o
