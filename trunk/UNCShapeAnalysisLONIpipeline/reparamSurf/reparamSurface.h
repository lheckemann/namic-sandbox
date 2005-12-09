#ifndef __REPARAM_SURFACE_H__
#define __REPARAM_SURFACE_H__

int reparamSurface(int numReparamVertices, double * &surfReparamVertList, double * reparamVertList, 
       int numVertices, double * vertList, double * parVertList,
       int numTriangles, int * triangIndexList);

#endif
