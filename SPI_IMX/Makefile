CC ?= gcc
CXX ?= g++
CPP ?= g++

APP_NAME = SPI
APP_OBJ_FILES = SPI.o

LIBS = 

all: $(APP_NAME)

$(APP_NAME) : $(APP_OBJ_FILES)
	$(CXX) $^ -o $@  $(LIBS)

%.o : %.cc
	$(CXX) -c $^ -o $@


clean:
	rm *.o $(APP_NAME)
