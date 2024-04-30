CC ?= gcc
CXX ?= g++
CPP ?= g++

APP_NAME = get_raw_frame
APP_OBJ_FILES = get_raw_frame.o

LIBS = libpixy2.a -lusb-1.0

all: $(APP_NAME)

$(APP_NAME) : $(APP_OBJ_FILES)
	$(CXX) $^ -o $@  $(LIBS)

%.o : %.cc
	$(CXX) -c $^ -o $@


clean:
	rm *.o $(APP_NAME)
