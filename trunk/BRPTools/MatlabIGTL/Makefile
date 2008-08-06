MEX    = /local/os-exact/pkg/Matlab71-64/bin/mex
INCDIR = -I/projects/igtdev/tokuda/igtl/OpenIGTLink-build\
	 -I/projects/igtdev/tokuda/igtl/OpenIGTLink/Source 
LIBDIR = -L/projects/igtdev/tokuda/igtl/OpenIGTLink-build/bin
LIB    = -lOpenIGTLink



all: igtlsend.mex #igtlget.mex


igtlsend.mex: igtlsend.cpp
	${MEX} ${INCDIR} ${LIBDIR} ${LIB} $<

#igtlget.mex: igtlget.cpp
#	${MEX} ${INCDIR} ${LIBDIR} ${LIB} $<