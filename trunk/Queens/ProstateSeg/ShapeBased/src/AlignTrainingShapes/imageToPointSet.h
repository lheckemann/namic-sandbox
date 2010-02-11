#ifndef imageToPointSet_h_
#define imageToPointSet_h_

#include "baseDef.h"
#include "pointSet.h"

void imageToPointSet2D( DoubleImage2DPointerType image, unsigned long numOfPt, \
                        PointSet2DType& ptSet);



void imageToPointSet3D( DoubleImage3DPointerType image, unsigned long numOfPt, \
                        PointSet3DType& ptSet);

#endif

