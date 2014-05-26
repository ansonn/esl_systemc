
# Makefile to build trace plugin
# Copyright ARM Limited 2009-2010 All Rights Reserved.
#

BITS ?= 32

CPPFLAGS = -I $(PVLIB_HOME)/include/fmruntime
CXXFLAGS = -W -Wall -Werror -m$(BITS) -O3 -DNDEBUG -fomit-frame-pointer -fPIC

ifeq ($(BITS),32)
CXXFLAGS += -march=pentiumpro -mtune=pentium4
endif

all: SimpleTraceExample.so

SimpleTraceExample.o: SimpleTraceExample.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

SimpleTraceExample.so: SimpleTraceExample.o
	$(CXX) -m$(BITS) -pthread -ldl -lrt -shared -o $@ SimpleTraceExample.o

clean:
	rm -f *.o
	rm -f SimpleTraceExample.so

# Use C++ linker:
CC=CXX

# End of Makefile
