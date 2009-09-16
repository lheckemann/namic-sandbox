#=========================================================================
#
#  Program:   Matlab OpenIGTLink Interface Makefile
#  Module:    $RCSfile: $
#  Language:  C++
#  Date:      $Date: $
#  Version:   $Revision: $
#
#  Copyright (c) Insight Software Consortium. All rights reserved.
#
#  This software is distributed WITHOUT ANY WARRANTY; without even
#  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
#  PURPOSE.  See the above copyright notices for more information.
#
#=========================================================================


### OpenIGTLink Library
IGTLSRC= /Users/junichi/igtdev/igtl/OpenIGTLink
IGTLBLD= /Users/junichi/igtdev/igtl/OpenIGTLink-build


### for Matlab
MEX    = /local/os-exact/pkg/Matlab71-64/bin/mex
MEXOPT = 

#### for Octave
#MEX    = /opt/local/bin/mkoctfile
#MEXOPT = --mex


### Headers and libraries
INCDIR = -I${IGTLBLD} -I${IGTLSRC}/Source
LIBDIR = -L${IGTLBLD}/bin
LIB    = -lOpenIGTLink


all: igtlsend.mex igtlopen.mex igtlreceive.mex igtlclose.mex

igtlopen.mex: igtlopen.cpp igtlMexClientSocket.o igtlMexSocket.o
	${MEX} ${MEXOPT} ${INCDIR} ${LIBDIR} ${LIB} $^

igtlsend.mex: igtlsend.cpp igtlMexClientSocket.o igtlMexSocket.o
	${MEX} ${MEXOPT} ${INCDIR} ${LIBDIR} ${LIB} $^

#igtlreceive.mex: igtlreceive.cpp igtlMexClientSocket.o igtlMexSocket.o
#	${MEX} ${MEXOPT} ${INCDIR} ${LIBDIR} ${LIB} $^

igtlclose.mex: igtlclose.cpp igtlMexClientSocket.o igtlMexSocket.o
	${MEX} ${MEXOPT} ${INCDIR} ${LIBDIR} ${LIB} $^

igtlMexClientSocket.o: igtlMexClientSocket.cpp
	${MEX} ${MEXOPT} ${INCDIR} -c $<

igtlMexSocket.o: igtlMexSocket.cpp
	${MEX} ${MEXOPT} ${INCDIR} -c $<

#igtlget.mex: igtlget.cpp
#	${MEX} ${INCDIR} ${LIBDIR} ${LIB} $<

clean:
	rm -f *.o *.mex*