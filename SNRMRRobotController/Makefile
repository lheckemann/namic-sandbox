#====================================================================
#
# MRI guided robot control system
#
# Copyright (C) 2003-2005 by The University of Tokyo, 
# All Right Reserved.
#
#====================================================================
# $RCSfile: Makefile,v $
# $Revision: 1.5 $ 
# $Author: junichi $
# $Date: 2005/11/15 11:05:33 $
#====================================================================


#====================================================================
# Description:
#    Makefile for realtime controller and graphical hardware monitor.
#====================================================================


#### for Linux
CXX       = /usr/bin/c++
CXXFLAGS  = -Wall -O2 #-march=pentium4 -msse2
LINKFLAGS =
DEFS      = -D__LINUX__  -DHAVE_JPEG_H=1 -DHAVE_PNG_H=1 -DHAVE_TIFF_H=1 -DHAVE_ZLIB_H=1 -DHAVE_OPENGL  -DUSE_ART #-DFULLSCREEN
INCDIR    = -I. -I/home/atre/program/mri_servo2/common -I/usr/local/include/fox-1.2 -I/home/atre/opt/include/igtl 
LIBDIR    = -L/usr/local/lib -L/usr/lib  -L/usr/X11R6/lib -L. -L/home/atre/opt/lib/igtl
COMLIBS   = -lm
IFLIBS    = -lFOX-1.2 -lXext -lX11 -lm -lGLU -lGL -lXmu -lXi -ltiff -lpthread -lg2c -lOpenIGTLink
CTRLLIBS  = -lgpg2000 -lgpg3300 -lgpg6204 -lg2c
ARTLIB    = /usr/lib/art_syscalls.o

## for Linux x86-64
## If you compile this program without driver libraries for IO Board,
## define _USE_FBISIM, specify -L../fbisim and -I../fbisim, and
## specify "-lgpg2000_sim -lgpg3300_sim -lgpg6204_sim" instead of
## "-lgpg2000 -lgpg3300 -lgpg6204"
## You need to remove -DUSE_ART option in the case of compiling under non-ART linux
#CXX       = c++
#CXXFLAGS  = -Wall -O2 #-march=pentium4 -msse2
#LINKFLAGS =
#DEFS      = -D__LINUX__  -DHAVE_JPEG_H=1 -DHAVE_PNG_H=1 -DHAVE_TIFF_H=1 -DHAVE_ZLIB_H=1 -DHAVE_OPENGL -D_USE_FBISIM -DWITHOUT_ART #-DDEBUG #-DFULLSCREEN
#INCDIR    = -I. -I../common -I../fbisim -I/home/junichi/work/include/fox-1.2 -I/home/junichi/work/include/octave-2.1.57
#LIBDIR    = -L/home/junichi/work/lib64 -L/usr/lib64  -L/usr/X11R6/lib64 -L. -L/home/junichi/work/lib64/octave-2.1.57 -L../fbisim
#COMLIBS   = -lm
#IFLIBS    = -lFOX-1.2 -lXext -lX11 -lm -lGLU -lGL -lXmu -lXi -ltiff -lpthread -loctave -lcruft -lfftw3 -lgfortran #-lg2c
##CTRLLIBS  = -lgpg2000 -lgpg3300 -lgpg6204 -loctave -lcruft -lfftw3 -lg2c
#CTRLLIBS  =  -lgfortran -loctave -lcruft -lgpg2000_sim -lgpg3300_sim -lgpg6204_sim  #-lgpg2000 -lgpg3300 -lgpg6204
##ARTLIB    = /usr/lib/art_syscalls.o


# for MacOS X
# If you compile this program without driver libraries for IO Board,
# define _USE_FBISIM, specify -L../fbisim and -I../fbisim, and
# specify "-lgpg2000_sim -lgpg3300_sim -lgpg6204_sim" instead of
# "-lgpg2000 -lgpg3300 -lgpg6204"
# You need to remove -DUSE_ART option in the case of using MacOS X.

#CXX       = g++
#CXXFLAGS  = -Wall -O2 #-march=pentium4 -msse2
#LINKFLAGS = 
#DEFS      = -DHAVE_JPEG_H=1 -DHAVE_PNG_H=1 -DHAVE_TIFF_H=1 -DHAVE_ZLIB_H=1 -DHAVE_OPENGL -DWITHOUT_ART -D_USE_FBISIM #-DDEBUG #-DFULLSCREEN 
#INCDIR    = -I. -I/opt/local/include/fox-1.6 -I../fbisim
#LIBDIR    = -L/opt/local/lib -L/usr/lib  -L/usr/X11R6/lib -L. -L./fbisim
#COMLIBS   = -lm
#IFLIBS    = -lFOX-1.6 -lXext -lX11 -lGLU -lGL -lXmu -lXi -ltiff -lpthread -ldl -lz -lSM -lICE -lXcursor #-lm 
#CTRLLIBS  = -lgpg2000_sim -lgpg3300_sim -lgpg6204_sim  #-lgpg2000 -lgpg3300 -lgpg6204
#ARTLIB    = #/usr/lib/art_syscalls.o
#INCDIR2   = ${INCDIR} ${MRTCINC}


## for Cygwin
#CXX       = g++
#CXXFLAGS  = -Wall -O2 -march=pentium4 -msse2
#LINKFLAGS =
#DEFS      = -D__CYGWIN__ -DDEBUG
#INCDIR    = -I. -I../common
#LIBDIR    = -L/usr/local/lib -L/usr/lib
#LIBRARY   =  -lm -lpthread

## for Solaris 8/9
#CXX       = /usr/local/bin/c++
#CXXFLAGS  = -Wall -O2 
#LINKFLAGS =
#DEFS      = -D__SOLARIS__ -DDEBUG
#INCDIR    = -I. -I../common -I/local/include -I/local/include/fox
#LIBDIR    = -L/usr/local/lib -L/usr/lib -L/local/lib 
#COMLIBS   = -lm 
#IFLIBS    = -lm -lpthread -lnsl -lsocket -lFOX -lXext -lX11 -lm -ltiff


MRSVRIF   = main
MRSVRCTRL = mainController 
COMOBJS   = MrsvrSharedData.o MrsvrStatus.o MrsvrCommand.o MrsvrLog.o MrsvrRAS.o
IFOBJS    = main.o MrsvrMessageServer.o MrsvrThread.o MrsvrMainWindow.o MrsvrPlotCanvas.o MrsvrTransform.o 
CTRLOBJS  = mainController.o MrsvrDev.o MrsvrTransform.o

.SUFFIXES:	.cpp

all: ${MRSVRIF} ${MRSVRCTRL}

${MRSVRIF}: ${IFOBJS} ${COMOBJS} ${ARTLIB}
	${CXX} ${LINKFLAGS} ${LIBDIR}  -o $@ $? ${IFLIBS} ${COMLIBS}

${MRSVRCTRL}: ${CTRLOBJS} ${COMOBJS} ${ARTLIB}
	${CXX} ${LINKFLAGS} ${LIBDIR}  -o $@ $? ${CTRLLIBS} ${COMLIBS} 

.cpp.o: 
	${CXX} ${CXXFLAGS} ${DEFS} ${INCDIR} -c $<

.c.o:
	${CXX} ${CXXFLAGS} ${DEFS} ${INCDIR2} -g -c $<

clean:
	rm -f ${MRSVRIF} ${MRSVRCTRL} ${IFOBJS} ${CTRLOBJS} ${COMOBJS}


