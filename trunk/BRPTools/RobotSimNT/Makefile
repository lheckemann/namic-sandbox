NTDIR   = /Users/junichi/igtdev/navitrack/NaviTrack
#NTDIR   = /projects/igtdev/tokuda/NaviTrack/
CXX     = g++ -g
LIBS    = -lNaviTrack
INCDIR  = -I${NTDIR}/include
LIBDIR  = -L. -L${NTDIR}

SCOBJ   = simplecanvas.o
PROGRAMS= robotsim

### Program section
all: ${PROGRAMS}

robotsim: robotsim.o
	${CXX} -o $@ $? ${CXXFLAGS} ${LIBDIR} ${LIBS}

robotsim.o: robotsim.cxx
	${CXX} -Wall ${INCDIR}  -c $< 

clean:
	rm -f *.o ${PROGRAMS}

