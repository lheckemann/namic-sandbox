### for Matlab
MEX    = /local/os-exact/pkg/Matlab71-64/bin/mex
MEXOPT = 
INCDIR = -I/projects/igtdev/tokuda/igtl/OpenIGTLink-build\
	 -I/projects/igtdev/tokuda/igtl/OpenIGTLink/Source 
LIBDIR = -L/projects/igtdev/tokuda/igtl/OpenIGTLink-build/bin
LIB    = -lOpenIGTLink

#### for Octave
#MEX    = /Applications/Octave.app/Contents/Resources/bin/mkoctfile
#MEXOPT = --mex
#INCDIR = -I/Users/junichi/igtdev/igtl/OpenIGTLink-build\
#	 -I/Users/junichi/igtdev/igtl/OpenIGTLink/Source 
#LIBDIR = -L/Users/junichi/igtdev/igtl/OpenIGTLink-build/bin
#LIB    = -lOpenIGTLink

all: igtlsend.mex igtlopen.mex igtlclose.mex

igtlopen.mex: igtlopen.cpp igtlMexClientSocket.o igtlMexSocket.o
	${MEX} ${MEXOPT} ${INCDIR} ${LIBDIR} ${LIB} $^

igtlsend.mex: igtlsend.cpp igtlMexClientSocket.o igtlMexSocket.o
	${MEX} ${MEXOPT} ${INCDIR} ${LIBDIR} ${LIB} $^

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