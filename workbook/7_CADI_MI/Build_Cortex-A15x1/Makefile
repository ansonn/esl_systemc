#
# Makefile - EVS_GlobalInterface platform makefile.
#
# Copyright 2011-2012 ARM Limited.
# All rights reserved.
#

# Common rules
include Makefile.common

# Specify objects and target(s) to build
SRCS = $(wildcard ../Source/*.cpp)
TARGET = EVS_GlobalInterface_Cortex-A15x1.x

# Determine others...
GENDIR = ./$(CONFIG)/gen
BUILDDIR = ./$(CONFIG)
WRAPPERLIB = scx-Dhrystone-$(CONFIG)
EVSLIB  = Dhrystone-$(CONFIG)
CXXFLAGS += -I$(GENDIR) -I../Source
LDFLAGS += -L$(BUILDDIR) -l$(WRAPPERLIB) -lcomponents -lpvbus \
		   $(PVLIB_HOME)/lib/Linux$(BITNESS_IN_NAME)_GCC-$(GCC_VER)/libarmctmodel.a \
		   -lscx -lfmruntime -lsystemc -l$(EVSLIB) -ldl -lrt \
		   -Wl,-rpath,'$$ORIGIN'

# Determine commands
SIMGEN_CMD = $(MAXCORE_HOME)/bin/simgen \
			 -p ../EVS_Dhrystone/Build_Cortex-A15x1/EVS_Dhrystone_Cortex-A15x1.sgproj \
			 --num-build-cpus $(J) --configuration $(CONFIG) \
			 $(SIMGENFLAGS)
CXX_CMD = $(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS) $(LDFLAGS)

# Local rules
.SUFFIXES:

.PHONY: dbg_gcc41_32 rel_gcc41_32
.PHONY: dbg_gcc44_32 rel_gcc44_32
.PHONY: dbg_gcc41_32_clean rel_gcc41_32_clean
.PHONY: dbg_gcc44_32_clean rel_gcc44_32_clean
.PHONY: dbg_gcc41_64 rel_gcc41_64
.PHONY: dbg_gcc44_64 rel_gcc44_64
.PHONY: dbg_gcc41_64_clean rel_gcc41_64_clean
.PHONY: dbg_gcc44_64_clean rel_gcc44_64_clean

dbg_gcc41_32 rel_gcc41_32 dbg_gcc41_64 rel_gcc41_64 dbg_gcc44_32 rel_gcc44_32 dbg_gcc44_64 rel_gcc44_64:
	$(SIMGEN_CMD) -b
	cp $(BUILDDIR)/lib$(EVSLIB).so ./
	$(CXX_CMD)

dbg_gcc41_32_clean rel_gcc41_32_clean dbg_gcc41_64_clean rel_gcc41_64_clean dbg_gcc44_32_clean rel_gcc44_32_clean dbg_gcc44_64_clean rel_gcc44_64_clean:
	-$(SIMGEN_CMD) --clean
	-rm -f $(TARGET)
	-rm -f ./lib$(EVSLIB).so

