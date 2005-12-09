#ifndef _READ_SURFACE_H___
#define _READ_SURFACE_H___

int ReadSurface(const char * surfFilename, int &numVertices, double * &vertList, 
    int &numTriangles, int * &triangIndexList);
// read a byu triangulation or a iv IndexedTriangleSet/IndexedTriangleStripSet

int WriteSurfaceIV(const char * surfFilename, int numVertices, double * vertList, 
       int numTriangles, int *triangIndexList);
// writes an iv IndexedTriangleSet/IndexedTriangleStripSet
#endif
