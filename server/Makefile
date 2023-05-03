SHELL = /bin/bash
CC = g++
CFLAGS = -g -Og -Wall -std=c++14 -I.
CFLAGS += -MMD -D_FORTIFY_SOURCE=2 -D_XOPEN_SOURCE=700
CFLAGS += -O2 -fopenmp

DEPEND = src/csapp.h src/util.h src/conversion.h src/fourColor.h
PROGRAM = server.cpp src/csapp.c src/util.cpp src/fourColor.cpp src/conversion.cpp

SOURCES_FOURCOLOR := src/fourColor.cpp main.cpp
HEADERS_FOURCOLOR := src/fourColor.h

SOURCES_CONVERSION := src/conversion.cpp testConversion.cpp
HEADERS_CONVERSION := src/conversion.h

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

.PHONY: clean
clean:
	rm -f *~ *.o *.d core server client fourcolor conversion
	rm -rf logs source_files response_files results.log get_files
