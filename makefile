IDIR =./include
CC=gcc
CXX=g++-7
CFLAGS =-std=c++17 -g -Wall
ODIR=./obj
SRC=./src
BIN=./bin/
TST=./test/
OBJ= $(addprefix $(ODIR)/, user_interface.o integer.o reader.o writer.o operations.o index_operations.o)
INC = $(addprefix $(IDIR)/, integer.h metadata.h metadata_struct.h operations.h \
		reader.h user_interface.h writer.h index_operations.h)
# test
all : csdbms

csdbms : $(SRC)/main.cpp $(OBJ)
	$(CXX) $(CFLAGS) -o $(BIN)csdbms $< $(OBJ) -I $(IDIR)

$(ODIR)/%.o : $(SRC)/%.cpp $(INC)
	$(CXX) $(CFLAGS) -I $(IDIR) -c $< -o $@  

$(INC):

$(TST)test_main.cpp: 