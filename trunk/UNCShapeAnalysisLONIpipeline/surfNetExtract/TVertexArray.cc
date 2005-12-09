// -*- C++ -*-
/****************************************************************************
 *
 *  class TVertexArray
 *
 ****************************************************************************
 *
 *  File         :  TVertexArray.cc
 *  Type         :  
 *  Purpose      : 
 *
 ****************************************************************************
 *                 
 *  started      :  3  Jan 95     Robert Schweikert
 *  last change  :  3  Jan 95
 *
 ****************************************************************************/
 
#include "TVertexArray.h"
#include <stdlib.h>


void TVertexArray::init(const TVertex* ar, int sz)
{
    ia_ = new TVertex[size_ = sz];
    TVertex defaultVertex;

    for (int ix = 0; ix < size_; ++ix)
         ia_[ix] = (ar != 0) ? ar[ix] : defaultVertex;
}

TVertexArray& TVertexArray::operator=(const TVertexArray& iA)
{
    if (this == &iA) return *this;
    delete ia_; 
    init( iA.ia_, iA.size_ );
    return *this;
}

int TVertexArray::find(TVertex val) const
{ 
    for (int i=0; i<size_; ++i)
         if (val == ia_[i]) return i;
    return not_found;
}

void TVertexArray::print(ostream& os) const
{
  for (int i = 0; i < size_; ++i) { 
     os << ia_[i];
     if (i != (size_-1) ) 
         os << ",\n";
  }
  os << "\n";
}

// kuerzer, schneller ?

// void TVertexArray::print(ostream& os) const
// {
//   for (int i = 0; i < size_-1; ++i)
//     os << ia_[i] << ",\n";
//   os << ia[size_-1] <<"\n";
// }


ostream& operator<< (ostream& os, const TVertexArray& ar) 
{
    ar.print(os);
    return os;
}

