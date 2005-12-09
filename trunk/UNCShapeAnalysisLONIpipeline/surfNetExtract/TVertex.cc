// -*- C++ -*-
/****************************************************************************
 *
 *  class TVertex
 *
 ****************************************************************************
 *
 *  File         :  TVertex.cc
 *  Type         :  
 *  Purpose      : 
 *
 ****************************************************************************
 *                 
 *  started      :  3  Jan 95     Robert Schweikert
 *  last change  :  3  Jan 95
 *
 ****************************************************************************/
 
#include "TVertex.h"

TVertex::TVertex()
{
   wh_[0] = wh_[1] = wh_[2] = count_ = 0;
   for (int i = 0; i < 14; ++i)
      neighb_[i] = 0;
}

TVertex::TVertex(int wh[3], int count, int neighb[14])
{
   wh_[0] = wh[0]; wh_[1] = wh[1]; wh_[2] = wh[2];
   count_ = count;
   for (int i = 0; i < count; ++i)
      neighb_[i] = neighb[i];
}


int operator==(const TVertex& v1, const TVertex& v2)
{
   if (v2.count_ != v1.count_)
      return 0;
  
   int result = (v1.wh_[0] == v2.wh_[0] && v1.wh_[1] == v2.wh_[1] && v1.wh_[2] == v2.wh_[2]);
   for (int i = 0; i < v2.count_; ++i)
      result = result && (v1.neighb_[i] == v2.neighb_[i]);
    
   return result;
}
    

ostream& operator<<(ostream& os, const TVertex& v)
{
   os << "{{" << v.wh_[0] << ", " << v.wh_[1] << ", " << v.wh_[2] << "}, {";
   for (int i = 0; i < v.count_; ++i) {
      os << v.neighb_[i];
      if (i != v.count_-1)
   os << ", ";
   }
   os << "}}";
   return os;
}
