#====================================================================
#
# IO Board driver emulator libraries
#
# Copyright (C) 2004 by The University of Tokyo, All Right Reserved.
#
#====================================================================
# $RCSfile: Makefile,v $
# $Revision: 1.2 $ 
# $Author: junichi $
# $Date: 2005/11/15 11:05:33 $
#====================================================================


## for x86-64
CC        = g++
CFLAGS    = -Wall -O2 #-march=pentium4 -msse2
LINKFLAGS =
DEFS      =
INCDIR    = -I.
LIBDIR    = 
LIBS      = 

### for MacOS X
#CC        = g++
#CFLAGS    = -Wall -O2 #-march=pentium4 -msse2
#LINKFLAGS =
#DEFS      =
#INCDIR    = -I.
#LIBDIR    = 
#LIBS      = 


.SUFFIXES:	.c

all: libgpg2000_sim.a libgpg3300_sim.a libgpg6204_sim.a

libgpg2000_sim.a: fbidio_sim.o
	ar rv $@ $?
	ranlib $@

libgpg3300_sim.a: fbida_sim.o
	ar rv $@ $?
	ranlib $@

libgpg6204_sim.a: fbipenc_sim.o
	ar rv $@ $?
	ranlib $@

.c.o: 
	${CC} ${CFLAGS} ${DEFS} ${INCDIR} -c $<

clean:
	rm -f *.o
	rm -f *.a








