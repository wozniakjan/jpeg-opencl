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
CC    = g++
OBJS  = $(shell ls src/*cpp | sed s/src/bin/g | sed s/cpp/o/g)
DOC   = dokumentace


ifeq ($(OS), Darwin)
	LIBS=-framework OpenCL
	CL_FLAGS=-I/usr/local/share/cuda/include/
else
ifeq ($(OS), CYGWIN_NT-5.1)
	LIBS=
	CL_FLAGS=
else
	LIBS=-lOpenCL
	CL_FLAGS=-I/usr/local/cuda/include/ #-L/opt/AMDAPP/lib/ -I/opt/AMDAPP/include/
endif
endif

.PHONY: all clean edit $(BIN)

all: $(BIN)$(PROJ)

$(BIN)%.o: $(SRC)%.cpp $(BIN)
	$(CC) -c $(FLAGS) $(CL_FLAGS) $< -o $@ $(LIBS)

$(BIN)$(PROJ): $(OBJS)
	$(CC) $(FLAGS) $(OBJS) -o $(BIN)$(PROJ) $(CL_FLAGS) $(LIBS)

$(BIN):
	$(shell mkdir $(BIN))


doc: doc/$(DOC).tex
	cd doc; pdflatex $(DOC)
	cd doc; pdflatex $(DOC)


edit:
	vim -S session.vim

run:
	cd $(BIN); ./$(PROJ)

clean:
	rm -rf $(BIN)*
	cd doc;rm -f $(DOC).aux
	cd doc;rm -f $(DOC).log
	cd doc;rm -f $(DOC).dvi
	cd doc;rm -f $(DOC).ps
	cd doc;rm -f $(DOC).pdf
	cd doc;rm -f $(DOC).bbl
	cd doc;rm -f $(DOC).blg
	cd doc;rm -f $(DOC).out
