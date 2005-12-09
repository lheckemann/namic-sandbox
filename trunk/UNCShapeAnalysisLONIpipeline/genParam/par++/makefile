
CXX  =  g++
CC  = gcc
MY_INCLUDE = .
CXXFLAGS= -I$(MY_INCLUDE)  -O5
OBJS  = read_net.o startup.o iterator.o utility.o vector3.o sparsemat.o 

all  : libiter 

$(OBJS): vertex.hh 
iterator.o: iterator.hh sparsemat.hh
startup.o iterator.o utility.o sparsemat.o: utility.hh
iterator.o vector3.o: vector3.hh
sparsemat.o: sparsemat.hh

libiter: $(OBJS) 
  ar ru libiter.a $(OBJS) 

.cc.o:  ;  $(CXX) $(CXXFLAGS) $(INCLUDE) -c $<

