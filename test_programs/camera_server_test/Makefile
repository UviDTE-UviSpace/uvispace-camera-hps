TARGET=camera_server
#CROSSCOMPILE=
CROSSCOMPILE=arm-linux-gnueabihf-

camera_server: main.o camera_server.o abstract_server.o avalon_camera.o
	$(CROSSCOMPILE)g++ -Wall -o camera_server main.o camera_server.o abstract_server.o avalon_camera.o

main.o: main.cpp camera_server.hpp
	$(CROSSCOMPILE)g++ -c -fpermissive main.cpp $(INC)

camera_server.o: camera_server.hpp camera_server.cpp
	$(CROSSCOMPILE)g++ -c -fpermissive camera_server.cpp $(INC)

abstract_server.o: abstract_server.hpp abstract_server.cpp
	$(CROSSCOMPILE)g++ -c abstract_server.cpp $(INC)

avalon_camera.o: avalon_camera.hpp avalon_camera.cpp
	$(CROSSCOMPILE)g++ -c -fpermissive avalon_camera.cpp $(INC)

.PHONY: clean
clean:
	rm camera_server *.o
