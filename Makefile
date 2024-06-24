CHARMC =/ccs/home/rrao/charm/ofi-linux-x86_64-cxi-slurmpmi2cray-gcc/bin/charmc $(OPTS)
CHARMC_SMP =/ccs/home/rrao/charm/ofi-linux-x86_64-cxi-slurmpmi2cray-smp-gcc/bin/charmc $(OPTS)

CHARMCFLAGS = $(OPTS) -g -O3

BINARY= interop

.PHONY = clean

interop: interop.cpp
	$(CHARMC) interop.ci
	$(CHARMC) -c++ mpicxx -c interop.cpp -o interop.o
	$(CHARMC) -ld++ mpicxx -o interop interop.o

clean:
	rm -f *.o *.decl.h *.def.h $(BINARY) charmrun* *.stamp