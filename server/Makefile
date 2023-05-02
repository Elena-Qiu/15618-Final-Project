SHELL = /bin/bash
CC = g++
CFLAGS = -g -Og -Wall -std=c++14 -I.
CFLAGS += -MMD -D_FORTIFY_SOURCE=2 -D_XOPEN_SOURCE=700
CFLAGS += -O2 -fopenmp

DEPEND = src/csapp.h src/util.h src/conversion.h src/fourColor.h
PROGRAM = server.cpp src/csapp.c src/util.cpp src/fourColor.cpp src/conversion.cpp

# Uncomment this to enable debug macros
CFLAGS += -DDEBUG

# Default build rule
.PHONY: all
all: server

server: $(PROGRAM) $(DEPEND)
	$(CC) $(CFLAGS) $(PROGRAM) -o server

.PHONY: clean
clean:
	rm -f *~ *.o *.d core server client
	rm -rf logs source_files response_files results.log get_files
