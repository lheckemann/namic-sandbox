MEX    = /local/os-exact/pkg/Matlab71-64/bin/mex
MEXOPT = 
INCDIR = -I/projects/igtdev/tokuda/igtl/OpenIGTLink-build\
	 -I/projects/igtdev/tokuda/igtl/OpenIGTLink/Source 
LIBDIR = -L/projects/igtdev/tokuda/igtl/OpenIGTLink-build/bin
LIB    = -lOpenIGTLink



all: igtlsend #igtlget.mex

igtlsend: igtlsend.cpp igtlMexClientSocket.o
	${MEX} ${MEXOPT} ${INCDIR} ${LIBDIR} ${LIB} $^

igtlMexClientSocket.o: igtlMexClientSocket.cpp
	${MEX} ${MEXOPT} ${INCDIR} -c $<

#igtlget.mex: igtlget.cpp
#	${MEX} ${INCDIR} ${LIBDIR} ${LIB} $<