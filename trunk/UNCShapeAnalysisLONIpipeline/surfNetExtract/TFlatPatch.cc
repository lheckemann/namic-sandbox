// -*- C++ -*-
/****************************************************************************
 *
 *  class TFlatPatch
 *
 ****************************************************************************
 *
 *  File         :  TFlatPatch.cc
 *  Type         :  
 *  Purpose      : 
 *
 ****************************************************************************
 *                 
 *  started      :  4  Jan 95     Robert Schweikert
 *  last change  :  29 Feb 95
 *
 ****************************************************************************/

#include "patch.hh"

#include "TFlatPatch.h"
#include "TGeometryCreator.h"

void TFlatPatch::make_flat(TSurfacePatch& net)
{
   L_  = net.nvert();
   uv_ = new double [L_][2];
   param_patch(net, uv_);
}

void TFlatPatch::print(ostream& os) const
{
   os << "{" << endl;
   for (int j=0; j<L_; ++j) {
      os << "{" << uv_[j][0] << "," << uv_[j][1] << "}";
      if (j != (L_-1) ) 
   os << ",\n";
   }
   os << endl << "}" << endl;
}

  
ostream& operator<< (ostream& os, const TFlatPatch& fp)
{
   fp.print(os);
   return os;
}
