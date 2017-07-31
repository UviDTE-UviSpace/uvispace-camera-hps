TARGET = camera_capture_test
OBJS = main.o
INC = -I../include/
FLAGS = -std=c++11 -Wall

build: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(FLAGS) $(INC) -o $@ $^

%.o: %.cpp %.hpp
	$(CXX) $(FLAGS) $(INC) -o $@ -c $<

.PHONY: clean
clean:
	-rm $(TARGET) $(OBJS)
