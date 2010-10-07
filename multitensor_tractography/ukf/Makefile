DEBUG = yes

CC = g++
CFLAGS_NORMAL = -c -Wall -ansi -DGMM_USES_LAPACK -Ilibs/gmm/include -Ilibs/teem-build/include#-pedantic 
ifeq ($(DEBUG), no)
	CFLAGS = $(CFLAGS_NORMAL) -O2 -DNDEBUG -Wno-unused-variable
else
	CFLAGS = $(CFLAGS_NORMAL) -g
endif

LDFLAGS = -llapack -lteem
SOURCES = filter_model.cc unscented_kalman_filter.cc tractography.cc signal_data.cc vtk_writer.cc main.cc
OBJECTS = $(SOURCES:.cc=.o)
EXECUTABLE = ukf

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cc.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *o $(EXECUTABLE)

libs:
	cd libs/gmm-4.1 && ./configure --prefix=`pwd`/../gmm && make install
	cd libs/lapack-3.2.2/BLAS/SRC && make
	cd libs/lapack-3.2.2 && make && cp blas_LINUX.a libblas.a && cp lapack_LINUX.a liblapack.a

cleanlibs:
	rm -rf libs/gmm
	cd libs/lapack-3.2.2/BLAS/SRC && make clean
	cd libs/lapack-3.2.2 && make clean && rm libblas.a && rm liblapack.a && rm blas_LINUX.a && rm lapack_LINUX.a && rm tmglib_LINUX.a
	rm libs/teem-build/* -rf

withlibs: libs manual

manual: $(SOURCES) $(OBJECTS)
	$(CC) -lgfortran $(OBJECTS) libs/lapack-3.2.2/lapack_LINUX.a libs/lapack-3.2.2/blas_LINUX.a -Llibs/teem-build/bin -lteem -lpng -lbz2 -o $(EXECUTABLE)

.PHONY: libs clean cleanlibs
