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
IGTLSRC= /projects/igtdev/tokuda/igtl/OpenIGTLink
IGTLBLD= /projects/igtdev/tokuda/igtl/OpenIGTLink-build

### for Matlab
MEX    = /local/os-exact/bin/mex
MEXOPT = 

### for Octave
#MEX    = /opt/local/bin/mkoctfile
#MEXOPT = --mex

### Headers and libraries
INCDIR = -I${IGTLBLD} -I${IGTLSRC}/Source
LIBDIR = -L${IGTLBLD}/bin
LIB    = -lOpenIGTLink


all: igtlsend.mex igtlopen.mex igtlreceive.mex igtlclose.mex igtlwaitcon.mex

igtlopen.mex: igtlopen.cpp igtlMexClientSocket.o igtlMexServerSocket.o igtlMexSocket.o
	${MEX} ${MEXOPT} ${INCDIR} ${LIBDIR} ${LIB} $^

igtlsend.mex: igtlsend.cpp igtlMexClientSocket.o igtlMexSocket.o
	${MEX} ${MEXOPT} ${INCDIR} ${LIBDIR} ${LIB} $^

igtlreceive.mex: igtlreceive.cpp igtlMexClientSocket.o igtlMexSocket.o
	${MEX} ${MEXOPT} ${INCDIR} ${LIBDIR} ${LIB} $^

igtlclose.mex: igtlclose.cpp igtlMexClientSocket.o igtlMexSocket.o
	${MEX} ${MEXOPT} ${INCDIR} ${LIBDIR} ${LIB} $^

igtlwaitcon.mex: igtlwaitcon.cpp igtlMexServerSocket.o igtlMexClientSocket.o igtlMexSocket.o
	${MEX} ${MEXOPT} ${INCDIR} ${LIBDIR} ${LIB} $^

igtlMexClientSocket.o: igtlMexClientSocket.cpp
	${MEX} ${MEXOPT} ${INCDIR} -c $<

igtlMexServerSocket.o: igtlMexServerSocket.cpp
	${MEX} ${MEXOPT} ${INCDIR} -c $<

igtlMexSocket.o: igtlMexSocket.cpp
	${MEX} ${MEXOPT} ${INCDIR} -c $<

#igtlget.mex: igtlget.cpp
#	${MEX} ${INCDIR} ${LIBDIR} ${LIB} $<

clean:
	rm -f *.o *.mex*