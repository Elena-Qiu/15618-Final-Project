SHELL = /bin/bash
CC = gcc
CFLAGS = -g -Og -Wall -std=c99 -MMD -D_FORTIFY_SOURCE=2 -D_XOPEN_SOURCE=700 -I.
LDLIBS = -lm -lpcre
LDLIBS += -Wl,-rpath

DEPEND = csapp.h map_solver.h http_parser.h
PROGRAM = server.c csapp.c map_solver.c

# Uncomment this to enable debug macros
CFLAGS += -DDEBUG

# Default build rule
.PHONY: all
all: server

server: server.c csapp.c $(DEPEND)
	$(CC) $(CFLAGS) $(PROGRAM) $(LDLIBS) -o server

.PHONY: clean
clean:
	rm -f *~ *.o *.d core server client
	rm -rf logs source_files response_files results.log get_files
