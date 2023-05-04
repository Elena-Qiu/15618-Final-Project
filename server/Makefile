SHELL = /bin/bash
CC = g++
CFLAGS = -g -Og -Wall -std=c++14 -I.
CFLAGS += -MMD -D_FORTIFY_SOURCE=2 -D_XOPEN_SOURCE=700
#CFLAGS += -O2 -fopenmp

DEPEND = src/csapp.h src/pipeline.h src/conversion.h src/unionFind.h src/fourColor.h
PROGRAM = server.cpp src/csapp.c src/pipeline.cpp src/fourColor.cpp src/conversion.cpp src/unionFind.cpp

SOURCES_FOURCOLOR := src/fourColor.cpp testFourColor.cpp
HEADERS_FOURCOLOR := src/fourColor.h

SOURCES_CONVERSION := src/conversion.cpp src/unionFind.cpp testConversion.cpp
HEADERS_CONVERSION := src/conversion.h src/unionFind.h

SOURCES_UNIONFIND := src/unionFind.cpp testUnionFind.cpp
HEADERS_UNIONFIND := src/unionFind.h

# Uncomment this to enable debug macros
CFLAGS += -DDEBUG

# Default build rule
.PHONY: all
all: server fourcolor conversion

server: $(PROGRAM) $(DEPEND)
	$(CC) $(CFLAGS) $(PROGRAM) -o server

fourcolor: $(SOURCES_FOURCOLOR) $(HEADERS_FOURCOLOR)
	$(CC) $(CFLAGS) $(SOURCES_FOURCOLOR) -o fourcolor

conversion: $(SOURCES_CONVERSION) $(HEADERS_CONVERSION)
	$(CC) $(CFLAGS) $(SOURCES_CONVERSION) -o conversion

unionfind: $(SOURCES_UNIONFIND) $(HEADERS_UNIONFIND)
	$(CC) $(CFLAGS) $(SOURCES_UNIONFIND) -o unionfind

.PHONY: clean
clean:
	rm -f *~ *.o *.d core server client fourcolor conversion unionfind
	rm -rf logs source_files response_files results.log get_files
