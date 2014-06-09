include ./Makefile.config

export FLAG_BATCH = 1

SUBDIRS = \
          ../testbench/at_1_phase \
          ../testbench/fork_join \
          ../testbench/sc_dut \
          ../testbench/sc_report \
          ../testbench/scx_mutex_w_policy \
          ../testbench/dpipe \
          ../testbench/reset_signal_is \
          ../testbench/sc_export \
          ../testbench/sc_simple_fifo

.PHONY: all clean check run

all:
	@for i in $(SUBDIRS); do \
        (cd $$i/build-unix && $(MAKE)); done

check:
	@for i in $(SUBDIRS); do \
        (cd $$i/build-unix && $(MAKE) check); done

run:
	@for i in $(SUBDIRS); do \
        (cd $$i/build-unix && $(MAKE) run); done

clean:
	@if ( test ! -d $(SYSTEMC_HOME) ); \
		then echo 'SYSTEMC_HOME [$(SYSTEMC_HOME)] is not present. Update $(PWD)/Makefile.config'; \
	else \
		for i in $(SUBDIRS); do (cd $$i/build-unix && $(MAKE) clean); done; \
    fi
       	
