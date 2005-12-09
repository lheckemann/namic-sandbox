#pragma ident "@(#)TNetExtractor.h 1.2 10/31/96"

// -*- C++ -*-
/****************************************************************************
 *
 *  class TNetExtractor
 *
 ****************************************************************************
 *
 *  File         :  TNetExtractor.h
 *  Type         :  
 *  Purpose      :  generate surface net data structure
 *
 ****************************************************************************
 *                 
 *  started      :  4  Jan 95     Robert Schweikert
 *  last change  :  4  Feb 95
 *
 ****************************************************************************/

#ifndef TNETEXTRACTOR_H
#define TNETEXTRACTOR_H

#include "TVoxelVolume.h"
#include "TVertex.h"
#include "TVertexList.h"

struct t1entry {
   int  nbc;
   int  count;
   int  nbcs[4];
};

struct t2entry {
   int  nbc;
   int  count;
   int  dir[7];
   int  ne[7];
};

struct numentry {
   int nums[8];
};

const int tr_direct[6][8] = {{1,-1,3,-1,5,-1,7,-1}, {-1,0,-1,2,-1,4,-1,6}, {2,3,-1,-1,6,7,-1,-1}, \
           {-1,-1,0,1,-1,-1,4,5}, {4,5,6,7,-1,-1,-1,-1}, {-1,-1,-1,-1,0,1,2,3}};

enum {W,E,S,N,B,T};

class TNetExtractor {
public:
   TNetExtractor(const TVoxelVolume* vol) : vol_(vol) { }

   TVertexList* extractnet();

private:
   int direct_translate(int dir, int ne);
   int diag_translate(int dir1, int dir2, int ne);
   void encode(int nx, int ny, int nz, int& number, unsigned char* nbc, numentry* num, int z);

   const TVoxelVolume* vol_;                  // pointer to object volume
//   t1entry* pt1;                              // pointer to first table
//   t2entry* pt2;                              // pointer to second table
//#include <Ttables.hh>
static const t1entry pt1[256];
static const t2entry pt2[256];
};

#endif
