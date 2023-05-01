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

SOURCES_FOURCOLOR := fourColor.cpp main.cpp
HEADERS_FOURCOLOR := fourColor.h

SOURCES_CONVERSION := conversion.cpp testConversion.cpp fourColor.cpp
HEADERS_CONVERSION := conversion.h fourColor.h

TARGETBIN_FOURCOLOR := fourcolor_$(CONFIGURATION)
TARGETBIN_CONVERSION := conversion_$(CONFIGURATION)

.SUFFIXES:
.PHONY: all clean

all: $(TARGETBIN_CONVERSION) $(TARGETBIN_FOURCOLOR)

$(TARGETBIN_FOURCOLOR): $(SOURCES_FOURCOLOR) $(HEADERS_FOURCOLOR)
	$(CXX) -o $@ $(CFLAGS) $(SOURCES_FOURCOLOR)
$(TARGETBIN_CONVERSION): $(SOURCES_CONVERSION) $(HEADERS_CONVERSION)
	$(CXX) -o $@ $(CFLAGS) $(SOURCES_CONVERSION)

format:
	clang-format -i *.cpp *.h

clean:
	rm -rf ./fourcolor-$(CONFIGURATION)
	rm -rf ./conversion-$(CONFIGURATION)