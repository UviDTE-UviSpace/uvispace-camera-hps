TARGET = camera_vga_test
OBJS = main.o
INC = -I../../inc
FLAGS = -std=c++11 -Wall

build: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(FLAGS) $(INC) -o $@ $^

%.o: %.cpp %.hpp
	$(CXX) $(FLAGS) $(INC) -o $@ -c $<

.PHONY: clean
clean:
	-rm $(TARGET) $(OBJS)
