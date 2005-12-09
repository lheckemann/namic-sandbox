// -*- C++ -*-
/****************************************************************************
 *
 *  class TArray
 *
 ****************************************************************************
 *
 *  File         :  TArray.h
 *  Type         :  
 *  Purpose      :  generic C++ Template Array Class
 *
 ****************************************************************************
 *                 
 *  started      :  3  Jan 95     Robert Schweikert
 *  last change  :  3  Jan 95
 *
 ****************************************************************************/
 
#ifndef TARRAY_H
#define TARRAY_H

#include <iostream.h>
#include <stdlib.h>
#include "util.h"

enum { not_found = -1 };

template <class Type>
class TArray {
public: 
   TArray(int sz = 10)             { init(0, sz); }
   TArray(Type *ar, int sz)        { init(ar, sz); }
   TArray(const TArray<Type>& iA)    { init(iA.ia_, iA.size_); }
   ~TArray()                 { delete [] ia_; }

   TArray<Type>& operator= (const TArray<Type>&);
   int size()               { return size_; }
   int length()              { return size_; }

   Type& operator[] (int ix)              
                      { assertStr((ix>=0) && (ix<size_), "TArray index violation"); return ia_[ix]; }
   const Type& operator[] (int ix) const        
                      { assertStr((ix>=0) && (ix<size_), "TArray index violation"); return ia_[ix]; }

   int find(Type) const;
   void print(ostream& = cout) const;
  
protected:
   void init(const Type*, int);

   int size_;  
   Type* ia_;
};

#endif
