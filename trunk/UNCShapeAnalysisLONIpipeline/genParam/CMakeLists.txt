PROJECT( genParam ) 

ADD_DEFINITIONS('-DCOMPLEX=std::complex<double>')

SET(GEN_SRCS genparam3d.cc
par++/read_net.cc par++/startup.cc par++/iterator.cc par++/utility.cc par++/vector3.cc par++/sparsemat.cc )

FIND_PATH(SPARSELIB_BASE spblas /opt/local/include
/usr/local/include)
FIND_PATH(SPARSELIB_INCL_DIR tmvec.h ${SPARSELIB_BASE}/include /opt/local/include
/usr/local/include)
FIND_PATH(SPARSELIB_MV_INCL_DIR mvblasc.h ${SPARSELIB_BASE}/mv/include
 /opt/local/include  /usr/local/include )
FIND_PATH(SPARSELIB_LIB_DIR libsparse.a ${SPARSELIB_BASE}/lib
 /opt/local/lib /usr/local/lib )
FIND_PATH(SPARSELIB_IML_INCLUDE_DIR bicgstab.h ${SPARSELIB_BASE}/iml++ /opt/local/lib 
/usr/local/lib )


INCLUDE_DIRECTORIES(${SPARSELIB_INCL_DIR} ${SPARSELIB_MV_INCL_DIR} ${SPARSELIB_IML_INCLUDE_DIR} )
LINK_DIRECTORIES( ${SPARSELIB_LIB_DIR} )
LINK_LIBRARIES( mv sparse spblas )

ADD_EXECUTABLE( genParamFromNet ${GEN_SRCS})

