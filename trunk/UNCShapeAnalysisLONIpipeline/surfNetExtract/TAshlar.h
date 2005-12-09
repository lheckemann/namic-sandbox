// -*- C++ -*-
/****************************************************************************
 *
 *  class TAshlar
 *
 ****************************************************************************
 *
 *  File         :  TAshlar.h
 *  Type         :  
 *  Purpose      :  for generating test objects
 *
 ****************************************************************************
 *                 
 *  started      :  4  Jan 95     Robert Schweikert
 *  last change  :  4  Jan 95
 *
 ****************************************************************************/
 
#ifndef TASHLAR_H
#define TASHLAR_H

#include <iostream.h>
#include "TPoint3D.h"

class TAshlar {
public:  
   TAshlar() { TPoint3D npt; low_ = npt; high_ = npt; turnoff_ = 1; active_ = 0; }
   TAshlar(TPoint3D low, TPoint3D high, int turnoff, int active) : 
              low_(low), high_(high), turnoff_(turnoff), active_(active) { }

   int isinside(const TPoint3D& pt) const { return ((pt >= low_) && (pt <= high_)); }
   int turnoff() const { return turnoff_; }
   int active()  const { return active_; }

   void Set(TPoint3D low, TPoint3D high, int turnoff, int active) 
             { low_ = low; high_ = high; turnoff_ = turnoff; active_ = active; }

   TPoint3D low_;
   TPoint3D high_;
   int turnoff_;
   int active_;
};

#endif
