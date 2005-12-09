// -*- C++ -*-
/****************************************************************************
 *
 *  class TIntset
 *
 ****************************************************************************
 *
 *  File         :  TIntset.cc
 *  Type         :  
 *  Purpose      : 
 *
 ****************************************************************************
 *                 
 *  started      :  3  Jan 95     Robert Schweikert
 *  last change  :  3  Jan 95
 *
 ****************************************************************************/
 
#include <stdlib.h>

#include "util.h"
#include "TIntset.h"

TIntset::TIntset(int size, int init)
{
    assertStr(size > 0, "TIntset::TIntset : size <= 0 !");
    size_ = size;
    wordWidth_ = (size_ + 32 - 1)/32;
    //  wordWidth_ = (size_ + WORDSIZE - 1)/WORDSIZE;
    bv_ = new unsigned int[wordWidth_];
    assertStr( bv_ != 0 ,"strange: operator new returned 0");

    if ( init != OFF ) init = ~0;

    for ( int i = 0; i < wordWidth_; i++ )
  *(bv_ + i) = init;
}

TIntset::TIntset(const TIntset& it) : wordWidth_(it.wordWidth_), size_(it.size_)
{
   bv_ = new unsigned int[wordWidth_];

   for ( int i = 0; i < wordWidth_; i++ )
      *(bv_ + i) = *(it.bv_ +i);
}
   
void TIntset::operator+=( int pos )
{
    assertStr(pos >= 0 && pos < size_ , "TIntset::operator+= : not in range");

    int index = getIndex( pos );
    int offset = getOffset( pos );

    *(bv_ + index) |= (ON << offset);
}

void TIntset::operator-=( int pos )
{
    assertStr( pos >= 0 && pos < size_, "TIntset::operator-= : not in range");

    int index = getIndex(pos);
    int offset = getOffset(pos);

    *(bv_ + index) &= (~(ON << offset));
}

int TIntset::member(int pos) const
{
    assertStr( (pos >= 0) && (pos < size_), "TIntset::member : not in range" );

    int index = getIndex(pos);
    int offset = getOffset(pos);
    return( *(bv_ + index) & (ON << offset) );
}

ostream& operator<< (ostream& os, const TIntset& is)
{  
   os << "< ";
   for (int i = 0; i < is.size_; ++i ) 
      if (is.member(i))
   os << i << " ";
   os << ">";
   return os;
}

void TIntset::reset()
{
     for ( int i = 0; i < wordWidth_; ++i )
  // *( bv_ + i ) = 0;
  bv_[i] = 0;
}

TIntset TIntset::operator|( const TIntset& b ) const
{
    TIntset result( size_, OFF );
    unsigned int* tmp = result.bv_;
    for ( int i = 0; i < wordWidth_; ++i )
  *(tmp + i) = *(bv_ + i) | *(b.bv_ + i);
    return result;
}

TIntset TIntset::operator&( const TIntset& b ) const
{
    TIntset result( size_, OFF );
    unsigned int* tmp = result.bv_;
    for ( int i = 0; i < wordWidth_; ++i )
  *(tmp + i) = *(bv_ + i) & *(b.bv_ + i);
    return result;
}
