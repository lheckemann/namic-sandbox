# Makefile for F90 version of LSQR.
# Maintained by Michael Saunders <saunders@stanford.edu>
#
# 24 Sep 2007: First version for compiling lsqrTestProgram.f90
#              and associated modules.
#              All files listed are .f90 source code.
#              C is not currently used.
# 24 Oct 2007: Use real(dp) in lsqrDataModule instead of compiler option -r8.
# 19 Dec 2008: lsqrblasInterface module implemented.

  FC      =  g95
  FFLAGS  = -g -O
# FFLAGS  = -g -O0 -pedantic -Wall -Wextra -fbounds-check -ftrace=full

  CC      =  gcc
  CFLAGS  = -g -O

# Clear suffix list, then define the ones we want
  .SUFFIXES:
  .SUFFIXES: .c .f .f90 .o

  .f90.o:; ${FC} ${FFLAGS} -c -o $@ $<
  .f.o:;   ${FC} ${FFLAGS} -c -o $@ $<
  .c.o:;   $(CC) $(CFLAGS) -c -o $@ $<

  files = lsqrDataModule.o lsqrblas.o        lsqrblasInterface.o \
          lsqrModule.o     lsqrCheckModule.o lsqrTestModule.o    \
          lsqrTestProgram.o

TestProgram: ${files}
	${FC} ${FFLAGS} -o $@ ${files}

clean:
	\rm -f *.o *.mod
