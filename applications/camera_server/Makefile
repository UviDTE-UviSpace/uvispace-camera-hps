TARGET = camera_server
OBJS = main.o camera_server.o abstract_server.o
FLAGS = -std=c++11 -Wall

build: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(FLAGS) -o $@ $^

%.o: %.cpp %.hpp
	$(CXX) $(FLAGS) -o $@ -c $<

.PHONY: clean
clean:
	-rm $(TARGET) $(OBJS)
