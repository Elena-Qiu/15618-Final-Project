SHELL = /bin/bash
CC = g++
CFLAGS = -g -Og -Wall -std=c++14 -I.
CFLAGS += -MMD -D_FORTIFY_SOURCE=2 -D_XOPEN_SOURCE=700

DEPEND = csapp.h map_solver.h
PROGRAM = server.cpp csapp.c map_solver.cpp map_solver_helper.cpp

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
