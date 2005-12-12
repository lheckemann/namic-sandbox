// -*- C++ -*-
/****************************************************************************
 *
 *  class TEllipsoid
 *
 ****************************************************************************
 *
 *  File         :  TEllipsoid.h
 *  Type         :  
 *  Purpose      :  for generation of test objects
 *
 ****************************************************************************
 *                 
 *  started      :  4  Jan 95     Robert Schweikert
 *  last change  :  4  Jan 95
 *
 ****************************************************************************/
 
#ifndef TELLIPSOID_H
#define TELLIPSOID_H

#include <iostream.h>
#include <math.h>
#include "TPoint3D.h"

class TEllipsoid {
public:  
   TEllipsoid() { TPoint3D npt; m_ = npt; r1_ = 1; r2_ = 1; r3_ = 1; turnoff_ = 1; active_ = 0; }

   TEllipsoid(TPoint3D  m , float r1, float r2, float r3, int turnoff, int active) :
                         m_(m), r1_(r1), r2_(r2), r3_(r3), turnoff_(turnoff), active_(active) { }

   int isinside(const TPoint3D& pt) const 
        { return (( pow(((pt.x_-m_.x_)/r1_),2) + pow(((pt.y_-m_.y_)/r2_),2) + pow(((pt.z_-m_.z_)/r3_),2) ) <= 1) ; }
   int turnoff() const { return turnoff_; }
   int active()  const { return active_; }
   
   void Set(TPoint3D m, float r1, float r2, float r3, int turnoff, int active)
       { m_ = m; r1_ = r1; r2_ = r2; r3_ = r3; turnoff_ = turnoff; active_ = active; }

   TPoint3D m_;
   float r1_, r2_, r3_;
   int turnoff_;
   int active_;
};

#endif
