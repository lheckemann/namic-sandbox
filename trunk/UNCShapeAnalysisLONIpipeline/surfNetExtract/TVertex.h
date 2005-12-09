// -*- C++ -*-
/****************************************************************************
 *
 *  class TVertex
 *
 ****************************************************************************
 *
 *  File         :  TVertex.h
 *  Type         :  
 *  Purpose      :  info about one vertex
 *
 ****************************************************************************
 *                 
 *  started      :  3  Jan 95     Robert Schweikert
 *  last change  :  3  Jan 95    
 *
 ****************************************************************************/
 
#ifndef TVERTEX_H
#define TVERTEX_H

#include <iostream.h>
#include <math.h>
#include "TPoint3D.h"

#define SQUARE(x) ((x)*(x))

class TVertex {
public:  
   TVertex();
   TVertex(int wh[3], int count, int neighb[14]);

   TPoint3D GetCoords3D() { return TPoint3D(wh_); }
   TPointFloat3D GetCoordsFloat3D() { return TPointFloat3D(wh_); }
  
   friend int operator<(const TVertex& v1, const TVertex& v2) { return 0; }
   friend int operator>(const TVertex& v1, const TVertex& v2) { return 0; }  
   friend int operator==(const TVertex& v1, const TVertex& v2);  
   friend ostream& operator<< (ostream& os, const TVertex& v);
   friend float vdist(const TVertex& v1, const TVertex& v2) 
           { return sqrt( SQUARE(v1.wh_[0]-v2.wh_[0]) + SQUARE(v1.wh_[1]-v2.wh_[1]) + SQUARE(v1.wh_[2]-v2.wh_[2]) ); }

   int wh_[3];
   int count_;
   int neighb_[14];
};

#endif
