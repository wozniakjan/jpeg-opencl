# Projekt do GMU
# JPEG komprese
#
# autori: Lucie Matusova
# 		  Jan Wozniak



OS    = $(shell uname)
PROJ  = jpeg_conv
SRC   = src/
BIN   = bin/
FLAGS = -std=c++98 -Wall -pedantic -w

OBJS  = $(shell ls src/*cpp | sed s/src/bin/g | sed s/cpp/o/g) 

ifeq ($(OS), Darwin)
	CC=g++
	LIBS=-framework OpenCL
	CL_FLAGS=-I/usr/local/share/cuda/include/
endif

.PHONY: all clean edit $(BIN)





all: $(BIN)$(PROJ)

$(BIN)%.o: $(SRC)%.cpp $(BIN)
	$(CC) -c $(FLAGS) $< -o $@

$(BIN)$(PROJ): $(OBJS)
	$(CC) $(FLAGS) $(OBJS) -o $(BIN)$(PROJ) $(CL_LAGS) $(LIBS)

$(BIN):
	$(shell mkdir $(BIN))



edit:
	vim -S session.vim

run:
	cd $(BIN); ./$(PROJ)

clean:
	rm -rf $(BIN)*
