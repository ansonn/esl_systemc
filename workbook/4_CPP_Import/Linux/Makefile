
CXX=g++
AR=ar


SRC_DIR=../CPPModel/
SRC=${SRC_DIR}/ext_mem_model.cpp
OBJS=$(SRC:.cpp=.o)

BITNESS ?= 32
BITNESS_IN_NAME_32 =
BITNESS_IN_NAME_64 = 64
BITNESS_IN_NAME = $(BITNESS_IN_NAME_$(BITNESS))

CFLAGS=-Wall -m$(BITNESS)
ifeq ($(BITNESS),64)
CFLAGS+= -fPIC
endif


LIB_NAME=extMem_Linux$(BITNESS_IN_NAME)-GCC-4.4.a

.PHONY: all clean
all: ${LIB_NAME}


${OBJS}: ${SRC}
	${CXX} $(CFLAGS) -I${SRC_DIR} -o $@ -c $<


${LIB_NAME}: ${OBJS}
	${AR} -cvq $@ $< 

clean: 
	rm -f *.a ${OBJS}



