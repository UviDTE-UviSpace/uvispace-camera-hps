TARGET = img_processing_test
OBJS = main.o
INC = -I../../inc
FLAGS = -std=c++11 -Wall

CROSS_COMPILE := arm-linux-gnueabihf-
CC = $(CROSS_COMPILE)gcc
ARCH= arm

build: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(FLAGS) $(INC) -o $@ $^

%.o: %.cpp %.hpp
	$(CC) $(FLAGS) $(INC) -o $@ -c $<

.PHONY: clean
clean:
	-rm $(TARGET) $(OBJS)
