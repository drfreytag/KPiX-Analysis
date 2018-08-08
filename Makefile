# Variables
CC=g++ 
INC=$(PWD)/include

#LDIR=$(PWD)/lib
#KP_CFLAGS=-I/home/mengqing/kpix/kpix -I/home/mengqing/kpix/generic
#KP_LFLAGS=-L$(LDIR) -lkpix 
#KP_RLIB=-Wl,-rpath=$(LDIR)

KP_RLIB=-Wl,-rpath=/usr/local/lib/kpix/
KP_CFLAGS=-I/usr/local/include/kpix
KP_LFLAGS=-L/usr/local/lib/kpix/ -lkpix

CFLAGS=-Wall $(shell xml2-config --cflags) $(shell root-config --cflags) $(KP_CFLAGS) -I$(INC)
LFLAGS=$(shell xml2-config --libs) $(shell root-config --glibs) $(KP_LFLAGS)

# dir:
SRC=$(PWD)/src
BIN=$(PWD)/bin

objects=$(patsubst $(SRC)/%.cxx,$(BIN)/%,$(wildcard $(SRC)/*.cxx))

#default:
all: dir map $(objects)
ana: dir map analysis

$(BIN)/%: $(SRC)/%.cxx
	$(CC) -o $@ $< $(CFLAGS) $(KP_RLIB) $(LFLAGS)

analysis: $(SRC)/analysis.cxx
	$(CC) -o $(BIN)/analysis $(SRC)/analysis.cxx $(CFLAGS) $(KP_RLIB) $(LFLAGS) 

# executable directory
dir:
	test -d $(BIN) || mkdir $(BIN)

map:
	python python/stripMapMaker.py data/tracker_to_kpix_left.dat data/tracker_to_kpix_right.dat include/kpix_left_and_right.h
clean:
	rm $(BIN)/*
