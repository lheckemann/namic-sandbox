SET (SOURCES
newmat9.cpp
newmatrm.cpp
nm_misc.cpp
bandmat.cpp
newmat1.cpp
solution.cpp
cholesky.cpp
hholder.cpp
newmat2.cpp
newmatex.cpp
newmat3.cpp
sort.cpp
evalue.cpp
jacobi.cpp
newmat4.cpp
submat.cpp
example.cpp
newmat5.cpp
svd.cpp
myexcept.cpp
newmat6.cpp
newmatnl.cpp
fft.cpp
newmat7.cpp
newfft.cpp
newmat8.cpp
 )

ADD_LIBRARY (newmat STATIC
${SOURCES}
)
