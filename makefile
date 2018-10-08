#headers = filter_numerical_columns.h
headers = 
#objects = main.o filter_numerical_columns.o
objects = main.o
#SOURCES = filter_numerical_columns.c main.c
SOURCES = main.c
CC = gcc
OBJ_DIR = build
SRC_DIR = src
BIN_DIR = bin

$(BIN_DIR)/tadanum : $(objects)
	$(CC) -o $(BIN_DIR)/tadanum $(objects)
	mv *.o $(OBJ_DIR) 

$(objects): $(SRC_DIR)/$(headers) $(SRC_DIR)/$(SOURCES)
			$(CC) -c $(SRC_DIR)/$(SOURCES)


