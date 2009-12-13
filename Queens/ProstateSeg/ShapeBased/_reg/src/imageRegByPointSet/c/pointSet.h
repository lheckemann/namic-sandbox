#ifndef pointSet_h_
#define pointSet_h_


#include "baseDef.h"

typedef std::vector< VnlDoubleVector2DType > PointSet2DType;
typedef std::vector< VnlDoubleVector3DType > PointSet3DType;

typedef boost::shared_ptr< PointSet2DType > PointSet2DPointerType;
typedef boost::shared_ptr< PointSet3DType > PointSet3DPointerType;



#endif

