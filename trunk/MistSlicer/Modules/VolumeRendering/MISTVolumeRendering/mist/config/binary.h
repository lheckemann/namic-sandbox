// 
// Copyright (c) 2003-2008, MIST Project, Intelligent Media Integration COE, Nagoya University
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
// 
// 1. Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
// 
// 3. Neither the name of the Nagoya University nor the names of its contributors
// may be used to endorse or promote products derived from this software
// without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
// IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
// FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
// IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
// THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// 



#ifndef __INCLUDE_MIST_BINARY_H__
#define __INCLUDE_MIST_BINARY_H__


#ifndef __INCLUDE_MIST_CONF_H__
#include "mist_conf.h"
#endif

#include <iostream>

_MIST_BEGIN

 
class binary
{
public:
    typedef size_t size_type;               
    typedef ptrdiff_t difference_type;      
    typedef bool& reference;                
    typedef const bool& const_reference;    
    typedef bool value_type;                
    typedef bool* pointer;                  
    typedef const bool* const_pointer;      

private:
    bool value_;        

public:

    binary( ) : value_( false ){ }


    binary( const value_type &b ) : value_( b ){ }


    binary( const binary &b ) : value_( b.value_ ){ }



    const binary &operator  =( const binary &b ){ value_ = b.value_; return( *this ); }


    const binary &operator  =( const value_type &b ){ value_ = b; return( *this ); }



    const binary &operator +=( const binary &b ){ value_ = value_ || b.value_; return( *this ); }


    const binary &operator -=( const binary &b ){ value_ = value_ && !b.value_; return( *this ); }


    const binary &operator *=( const binary &b ){ value_ = value_ && b.value_; return( *this ); }


    const binary &operator /=( const binary &b ){ value_ = value_ == b.value_; return( *this ); }


    const binary &operator %=( const binary &b ){ value_ = value_ && !b.value_; return( *this ); }


    const binary &operator |=( const binary &b ){ value_ = value_ || b.value_; return( *this ); }


    const binary &operator &=( const binary &b ){ value_ = value_ && b.value_; return( *this ); }

    const binary &operator ^=( const binary &b ){ value_ = value_ != b.value_; return( *this ); }



    const binary &operator +=( const value_type &b ){ return( operator +=( binary( b ) ) ); }


    const binary &operator -=( const value_type &b ){ return( operator -=( binary( b ) ) ); }


    const binary &operator *=( const value_type &b ){ return( operator *=( binary( b ) ) ); }


    const binary &operator /=( const value_type &b ){ return( operator /=( binary( b ) ) ); }



    bool operator ==( const binary &b ) const { return( value_ == b.value_ ); }


    bool operator !=( const binary &b ) const { return( value_ != b.value_ ); }


    bool operator < ( const binary &b ) const { return( value_ <  b.value_ ); }


    bool operator <=( const binary &b ) const { return( value_ <= b.value_ ); }


    bool operator > ( const binary &b ) const { return( value_ >  b.value_ ); }


    bool operator >=( const binary &b ) const { return( value_ >= b.value_ ); }



    value_type get_value( ) const { return( value_ ); }


    //operator bool( ) const { return( value_ ); }
};



inline const binary operator +( const binary &b1, const binary &b2 ){ return( binary( b1 ) += b2 ); }


inline const binary operator -( const binary &b1, const binary &b2 ){ return( binary( b1 ) -= b2 ); }


inline const binary operator *( const binary &b1, const binary &b2 ){ return( binary( b1 ) *= b2 ); }


inline const binary operator /( const binary &b1, const binary &b2 ){ return( binary( b1 ) /= b2 ); }


inline const binary operator %( const binary &b1, const binary &b2 ){ return( binary( b1 ) %= b2 ); }


inline const binary operator |( const binary &b1, const binary &b2 ){ return( binary( b1 ) |= b2 ); }


inline const binary operator &( const binary &b1, const binary &b2 ){ return( binary( b1 ) &= b2 ); }


inline const binary operator ^( const binary &b1, const binary &b2 ){ return( binary( b1 ) ^= b2 ); }



inline const binary operator *( const binary &b1, const binary::value_type &b2 ){ return( binary( b1 ) *= b2 ); }


inline const binary operator *( const binary::value_type &b1, const binary &b2 ){ return( binary( b2 ) *= b1 ); }


inline const binary operator /( const binary &b1, const binary::value_type &b2 ){ return( binary( b1 ) /= b2 ); }



inline const binary operator +( const binary &b1, const binary::value_type &b2 ){ return( binary( b1 ) += b2 ); }


inline const binary operator +( const binary::value_type &b1, const binary &b2 ){ return( binary( b2 ) += b1 ); }


inline const binary operator -( const binary &b1, const binary::value_type &b2 ){ return( binary( b1 ) -= b2 ); }


inline const binary operator -( const binary::value_type &b1, const binary &b2 ){ return( binary( b1 ) -= b2 ); }




inline std::ostream &operator <<( std::ostream &out, const binary &b )
{
    out << b.get_value( );
    return( out );
}


_MIST_END


#endif // __INCLUDE_MIST_BINARY_H__
