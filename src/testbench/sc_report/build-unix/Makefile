include ../../../build-unix/Makefile.config

OBJ_NAME = sc_report

SRCDIR           = ../src
SRCDIRMODULE     = ../../../module/src

INCDIR =    -I../include \
            -I../../../module/include

CFLAGS = $(FLAGS) $(INCDIR)

OBJS   =    sc_report_main.o

.PHONY: clean all check

all: announce $(OBJ_NAME).exe

announce:
	@if  ( test ! -d $(FLAG_BATCH) ); \
		then echo; echo "$(OBJ_NAME):"; echo; \
	fi
	
check: announce $(OBJ_NAME).exe
	@$(VALGRIND) ./$(OBJ_NAME).exe > run.log
	@cat run.log | grep -v "stopped by user" | awk '{if($$0!="") print $$0}' > run_trimmed.log
	@cat ../results/expected.log | grep -v "stopped by user" | awk '{if($$0!="") print $$0}' > ./expected_trimmed.log
	@diff ./run_trimmed.log ./expected_trimmed.log > diff.log 2>&1;\
	if [ -s diff.log ]; then echo "***ERROR:"; cat diff.log; else echo "OK"; fi

run: announce $(OBJ_NAME).exe
	@./$(OBJ_NAME).exe

$(OBJ_NAME).exe: $(OBJS)
	$(LD) -o $@ $(OBJS) $(LDFLAGS)

sc_report_main.o: $(SRCDIR)/sc_report_main.cpp
	$(GXX) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o *.log $(OBJ_NAME).exe

# DO NOT DELETE
