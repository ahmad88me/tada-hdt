HEADERS = filter_num.h
#headers = 
OBJS = main.o filter_num.o
#objects = main.o
#SOURCES = filter_numerical_columns.c main.c
SOURCES = main.cpp filter_num.cpp
CXXFLAGS = -std=c++11 
CC = g++
OBJ_DIR = build
SRC_DIR = src
BIN_DIR = bin
LIBS = -lhdt  

#objects_with_dir = $(patsubst %, $(OBJ_DIR/%),$(OBJ_DIR))

#$(BIN_DIR)/tadanum : $(objects_with_dir)
#	echo $(objects_with_dir)
#	$(CC) -o $(BIN_DIR)/tadanum $(objects)
#	mv *.o $(OBJ_DIR) 

#$(OBJ_DIR)/%.o :  $(SRC_DIR)/%.cpp
#	$(CC) -c $(SRC_DIR)/$(SOURCES)

OBJS_ABS = $(patsubst %,$(OBJ_DIR)/%,$(OBJS))
SOURCES_ABS = $(patsubst %,$(SRC_DIR)/%,$(SOURCES))


$(BIN_DIR)/tadanum : $(OBJS_ABS)
	$(CC) -o $@ $? $(LIBS)

$(OBJS_ABS): $(SOURCES_ABS)
	$(CC) $(CXXFLAGS) -c $(SOURCES_ABS)
	mv *.o build/

.PHONY: clean run

run: 
	bin/tadanum

clean:
	-rm $(BIN_DIR)/* 
	-rm $(OBJ_DIR)/* 
