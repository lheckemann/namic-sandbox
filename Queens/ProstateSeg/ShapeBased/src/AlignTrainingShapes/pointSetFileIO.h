#ifndef pointSetFileIO_h_
#define pointSetFileIO_h_


#include "pointSet.h"


PointSet2DType* readPointSetFromTextFile2D(const char* fileName);
void savePointSetToTextFile2D(PointSet2DType* ptSet, const char* fileName);

PointSet3DType* readPointSetFromTextFile3D(const char* fileName);
void savePointSetToTextFile3D(PointSet3DType* ptSet, const char* fileName);


PointSet2DPointerType readPointSetFromTextFile2D1(const char* fileName);
void savePointSetToTextFile2D(PointSet2DPointerType ptSet, const char* fileName);

PointSet3DPointerType readPointSetFromTextFile3D1(const char* fileName);
void savePointSetToTextFile3D(PointSet3DPointerType ptSet, const char* fileName);


#endif
