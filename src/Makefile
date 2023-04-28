OUTPUTDIR := bin/

CFLAGS := -std=c++14 -fvisibility=hidden -lpthread

ifeq (,$(CONFIGURATION))
	CONFIGURATION := release
endif

ifeq (debug,$(CONFIGURATION))
CFLAGS += -g
else
CFLAGS += -O2 -fopenmp
endif

SOURCES := *.cpp
HEADERS := *.h

TARGETBIN := fourcolor-$(CONFIGURATION)

.SUFFIXES:
.PHONY: all clean

all: $(TARGETBIN)

$(TARGETBIN): $(SOURCES) $(HEADERS)
	$(CXX) -o $@ $(CFLAGS) $(SOURCES) 

format:
	clang-format -i *.cpp *.h

clean:
	rm -rf ./fourcolor-$(CONFIGURATION)

# check:	default
# 	./checker.pl

FILES = *.cpp \
		*.h
