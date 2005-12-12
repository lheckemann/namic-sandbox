// -*- C++ -*-
/****************************************************************************
 *
 *  class TFlatPatch
 *
 ****************************************************************************
 *
 *  File         :  TFlatPatch.h
 *  Type         :  
 *  Purpose      :  wrapper for ~brech's parametrization routines
 *
 ****************************************************************************
 *                 
 *  started      :  17 Jan 95     Robert Schweikert
 *  last change  :  29 Feb 95
 *
 ****************************************************************************/
 
#ifndef TFLATPATCH_H
#define TFLATPATCH_H

#include <iostream.h>

#include "util_avs.h"
#include "TSurfacePatch.h"


class TFlatPatch {
public:
   TFlatPatch() : L_(0) {  }
   ~TFlatPatch()      { delete [] uv_; }

   // map
   void make_flat(TSurfacePatch& net);

   TPointFloat3D GetPointFloat3D(int i) { return TPointFloat3D(uv_[i][0], uv_[i][1], 0); }

   // output
   void print(ostream& os) const;
   friend ostream& operator<< (ostream& os, const TFlatPatch& fp);

private:
   double (*uv_)[2];                    // coordinates in 2D
   int L_;                              // length of net (to be used by print)
};


#endif
