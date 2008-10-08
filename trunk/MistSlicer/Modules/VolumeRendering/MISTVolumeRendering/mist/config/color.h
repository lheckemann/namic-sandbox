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



#ifndef __INCLUDE_MIST_COLOR_H__
#define __INCLUDE_MIST_COLOR_H__


#ifndef __INCLUDE_MIST_CONF_H__
#include "mist_conf.h"
#endif

#ifndef __INCLUDE_MIST_TYPE_TRAIT_H__
#include "type_trait.h"
#endif

#include <iostream>
#include <cmath>


_MIST_BEGIN



template < class T > struct bgr;


template< class T >
struct rgb
{
public:
    typedef size_t size_type;               
    typedef ptrdiff_t difference_type;      
    typedef T& reference;                   
    typedef const T& const_reference;       
    typedef T value_type;                   
    typedef T* pointer;                     
    typedef const T* const_pointer;         

public:
    value_type r;       
    value_type g;       
    value_type b;       


    rgb( ) : r( 0 ), g( 0 ), b( 0 ){ }


    explicit rgb( const value_type &pix ) : r( pix ), g( pix ), b( pix ){ }


    template < class TT >
    rgb( const rgb< TT > &c ) : r( static_cast< value_type >( c.r ) ), g( static_cast< value_type >( c.g ) ), b( static_cast< value_type >( c.b ) ){ }


    template < class TT >
    rgb( const bgr< TT > &c ) : r( static_cast< value_type >( c.r ) ), g( static_cast< value_type >( c.g ) ), b( static_cast< value_type >( c.b ) ){ }


    rgb( const rgb< T > &c ) : r( c.r ), g( c.g ), b( c.b ){ }


    rgb( const value_type &rr, const value_type &gg, const value_type &bb ) : r( rr ), g( gg ), b( bb ){ }



    template < class TT >
    const rgb &operator =( const rgb< TT > &c )
    {
        r = static_cast< value_type >( c.r );
        g = static_cast< value_type >( c.g );
        b = static_cast< value_type >( c.b );
        return( *this );
    }


    template < class TT >
    const rgb &operator =( const bgr< TT > &c )
    {
        r = static_cast< value_type >( c.r );
        g = static_cast< value_type >( c.g );
        b = static_cast< value_type >( c.b );
        return( *this );
    }


    const rgb &operator =( const rgb< T > &c )
    {
        if( &c != this )
        {
            r = c.r;
            g = c.g;
            b = c.b;
        }
        return( *this );
    }


    const rgb &operator =( const value_type &pix )
    {
        r = pix;
        g = pix;
        b = pix;
        return( *this );
    }



    const rgb  operator -( ) const { return( rgb( -r, -g, -b ) ); }


    template < class TT >
    const rgb &operator +=( const rgb< TT > &c ){ r = static_cast< value_type >( r + c.r ); g = static_cast< value_type >( g + c.g ); b = static_cast< value_type >( b + c.b ); return( *this ); }


    template < class TT >
    const rgb &operator -=( const rgb< TT > &c ){ r = static_cast< value_type >( r - c.r ); g = static_cast< value_type >( g - c.g ); b = static_cast< value_type >( b - c.b ); return( *this ); }


    template < class TT >
    const rgb &operator *=( const rgb< TT > &c ){ r = static_cast< value_type >( r * c.r ); g = static_cast< value_type >( g * c.g ); b = static_cast< value_type >( b * c.b ); return( *this ); }


    template < class TT >
    const rgb &operator /=( const rgb< TT > &c ){ r = static_cast< value_type >( r / c.r ); g = static_cast< value_type >( g / c.g ); b = static_cast< value_type >( b / c.b ); return( *this ); }


    const rgb &operator %=( const rgb &c ){ r %= c.r; g %= c.g; b %= c.b; return( *this ); }


    const rgb &operator |=( const rgb &c ){ r |= c.r; g |= c.g; b |= c.b; return( *this ); }


    const rgb &operator &=( const rgb &c ){ r &= c.r; g &= c.g; b &= c.b; return( *this ); }


    const rgb &operator ^=( const rgb &c ){ r ^= c.r; g ^= c.g; b ^= c.b; return( *this ); }



#if defined( __MIST_MSVC__ ) && __MIST_MSVC__ < 7
    const rgb &operator +=( const double &pix )
#else
    template < class TT >
    const rgb &operator +=( const TT &pix )
#endif
    {
        r = static_cast< value_type >( r + pix );
        g = static_cast< value_type >( g + pix );
        b = static_cast< value_type >( b + pix );
        return( *this );
    }


#if defined( __MIST_MSVC__ ) && __MIST_MSVC__ < 7
    const rgb &operator -=( const double &pix )
#else
    template < class TT >
    const rgb &operator -=( const TT &pix )
#endif
    {
        r = static_cast< value_type >( r - pix );
        g = static_cast< value_type >( g - pix );
        b = static_cast< value_type >( b - pix );
        return( *this );
    }


#if defined( __MIST_MSVC__ ) && __MIST_MSVC__ < 7
    const rgb &operator *=( const double &pix )
#else
    template < class TT >
    const rgb &operator *=( const TT &pix )
#endif
    {
        r = static_cast< value_type >( r * pix );
        g = static_cast< value_type >( g * pix );
        b = static_cast< value_type >( b * pix );
        return( *this );
    }


#if defined( __MIST_MSVC__ ) && __MIST_MSVC__ < 7
    const rgb &operator /=( const double &pix )
#else
    template < class TT >
    const rgb &operator /=( const TT &pix )
#endif
    {
        r = static_cast< value_type >( r / pix );
        g = static_cast< value_type >( g / pix );
        b = static_cast< value_type >( b / pix );
        return( *this );
    }


    bool operator ==( const rgb &c ) const { return( r == c.r && g == c.g && b == c.b ); }


    bool operator !=( const rgb &c ) const { return( !( *this == c ) ); }


    bool operator < ( const rgb &c ) const
    {
        if( r == c.r )
        {
            if( g == c.g )
            {
                return( b < c.b );
            }
            else
            {
                return( g < c.g );
            }
        }
        else
        {
            return( r < c.r );
        }
    }


    bool operator <=( const rgb &c ) const { return( c >= *this ); }


    bool operator > ( const rgb &c ) const { return( c < *this ); }


    bool operator >=( const rgb &c ) const { return( !( *this < c ) ); }



    value_type get_value( ) const
    {
        return( half_adjust< value_type >::convert( r * 0.298912 + g * 0.586610 + b * 0.114478 ) );
    }


    //operator value_type( ) const { return( get_value( ) ); }

};


DEFINE_PROMOTE_BIND_OPERATOR1( rgb, + )


DEFINE_PROMOTE_BIND_OPERATOR2( rgb, + )


DEFINE_PROMOTE_BIND_OPERATOR3( rgb, + )


DEFINE_PROMOTE_BIND_OPERATOR1( rgb, - )


DEFINE_PROMOTE_BIND_OPERATOR2( rgb, - )


DEFINE_PROMOTE_BIND_OPERATOR4( rgb, - )


DEFINE_PROMOTE_BIND_OPERATOR1( rgb, * )


DEFINE_PROMOTE_BIND_OPERATOR2( rgb, * )


DEFINE_PROMOTE_BIND_OPERATOR3( rgb, * )


DEFINE_PROMOTE_BIND_OPERATOR1( rgb, / )


DEFINE_PROMOTE_BIND_OPERATOR2( rgb, / )


DEFINE_PROMOTE_BIND_OPERATOR1( rgb, % )



DEFINE_PROMOTE_BIND_OPERATOR1( rgb, | )


DEFINE_PROMOTE_BIND_OPERATOR1( rgb, & )


DEFINE_PROMOTE_BIND_OPERATOR1( rgb, ^ )




template< class T >
struct bgr
{
public:
    typedef size_t size_type;               
    typedef ptrdiff_t difference_type;      
    typedef T& reference;                   
    typedef const T& const_reference;       
    typedef T value_type;                   
    typedef T* pointer;                     
    typedef const T* const_pointer;         

public:
    value_type b;       
    value_type g;       
    value_type r;       


    bgr( ) : b( 0 ), g( 0 ), r( 0 ){ }


    explicit bgr( const value_type &pix ) : b( pix ), g( pix ), r( pix ){ }


    template < class TT >
    bgr( const bgr< TT > &c ) : b( static_cast< value_type >( c.b ) ), g( static_cast< value_type >( c.g ) ), r( static_cast< value_type >( c.r ) ){ }


    bgr( const bgr< T > &c ) : b( c.b ), g( c.g ), r( c.r ){ }


    bgr( const value_type &rr, const value_type &gg, const value_type &bb ) : b( bb ), g( gg ), r( rr ){ }


    template < class TT >
    bgr( const rgb< TT > &c ) : b( static_cast< value_type >( c.b ) ), g( static_cast< value_type >( c.g ) ), r( static_cast< value_type >( c.r ) ){ }



    template < class TT >
    const bgr &operator =( const bgr< TT > &c )
    {
        b = static_cast< value_type >( c.b );
        g = static_cast< value_type >( c.g );
        r = static_cast< value_type >( c.r );
        return( *this );
    }


    template < class TT >
    const bgr &operator =( const rgb< TT > &c )
    {
        b = static_cast< value_type >( c.b );
        g = static_cast< value_type >( c.g );
        r = static_cast< value_type >( c.r );
        return( *this );
    }


    const bgr &operator =( const bgr< T > &c )
    {
        if( &c != this )
        {
            b = c.b;
            g = c.g;
            r = c.r;
        }
        return( *this );
    }


    const bgr &operator =( const value_type &pix )
    {
        b = pix;
        g = pix;
        r = pix;
        return( *this );
    }



    const bgr  operator -( ) const { return( bgr( -r, -g, -b ) ); }


    template < class TT >
    const bgr &operator +=( const bgr< TT > &c ){ b = static_cast< value_type >( b + c.b ); g = static_cast< value_type >( g + c.g ); r = static_cast< value_type >( r + c.r ); return( *this ); }


    template < class TT >
    const bgr &operator -=( const bgr< TT > &c ){ b = static_cast< value_type >( b - c.b ); g = static_cast< value_type >( g - c.g ); r = static_cast< value_type >( r - c.r ); return( *this ); }


    template < class TT >
    const bgr &operator *=( const bgr< TT > &c ){ b = static_cast< value_type >( b * c.b ); g = static_cast< value_type >( g * c.g ); r = static_cast< value_type >( r * c.r ); return( *this ); }


    template < class TT >
    const bgr &operator /=( const bgr< TT > &c ){ b = static_cast< value_type >( b / c.b ); g = static_cast< value_type >( g / c.g ); r = static_cast< value_type >( r / c.r ); return( *this ); }


    const bgr &operator %=( const bgr &c ){ b %= c.b; g %= c.g; r %= c.r; return( *this ); }


    const bgr &operator |=( const bgr &c ){ b |= c.b; g |= c.g; r |= c.r; return( *this ); }


    const bgr &operator &=( const bgr &c ){ b &= c.b; g &= c.g; r &= c.r; return( *this ); }


    const bgr &operator ^=( const bgr &c ){ b ^= c.b; g ^= c.g; r ^= c.r; return( *this ); }



#if defined( __MIST_MSVC__ ) && __MIST_MSVC__ < 7
    const bgr &operator +=( const double &pix )
#else
    template < class TT >
    const bgr &operator +=( const TT &pix )
#endif
    {
        b = static_cast< value_type >( b + pix );
        g = static_cast< value_type >( g + pix );
        r = static_cast< value_type >( r + pix );
        return( *this );
    }


#if defined( __MIST_MSVC__ ) && __MIST_MSVC__ < 7
    const bgr &operator -=( const double &pix )
#else
    template < class TT >
    const bgr &operator -=( const TT &pix )
#endif
    {
        b = static_cast< value_type >( b - pix );
        g = static_cast< value_type >( g - pix );
        r = static_cast< value_type >( r - pix );
        return( *this );
    }


#if defined( __MIST_MSVC__ ) && __MIST_MSVC__ < 7
    const bgr &operator *=( const double &pix )
#else
    template < class TT >
    const bgr &operator *=( const TT &pix )
#endif
    {
        b = static_cast< value_type >( b * pix );
        g = static_cast< value_type >( g * pix );
        r = static_cast< value_type >( r * pix );
        return( *this );
    }

#if defined( __MIST_MSVC__ ) && __MIST_MSVC__ < 7
    const bgr &operator /=( const double &pix )
#else
    template < class TT >
    const bgr &operator /=( const TT &pix )
#endif
    {
        b = static_cast< value_type >( b / pix );
        g = static_cast< value_type >( g / pix );
        r = static_cast< value_type >( r / pix );
        return( *this );
    }



    bool operator ==( const bgr &c ) const { return( b == c.b && g == c.g && r == c.r ); }


    bool operator !=( const bgr &c ) const { return( !( *this == c ) ); }


    bool operator < ( const bgr &c ) const
    {
        if( r == c.r )
        {
            if( g == c.g )
            {
                return( b < c.b );
            }
            else
            {
                return( g < c.g );
            }
        }
        else
        {
            return( r < c.r );
        }
    }


    bool operator <=( const bgr &c ) const { return( c >= *this ); }


    bool operator > ( const bgr &c ) const { return( c < *this ); }


    bool operator >=( const bgr &c ) const { return( !( *this < c ) ); }



    value_type get_value( ) const
    {
        return( static_cast< value_type >( b * 0.114478 + g * 0.586610 + r * 0.298912 ) );
    }


    //operator value_type( ) const { return( get_value( ) ); }

};


DEFINE_PROMOTE_BIND_OPERATOR1( bgr, + )


DEFINE_PROMOTE_BIND_OPERATOR2( bgr, + )


DEFINE_PROMOTE_BIND_OPERATOR3( bgr, + )


DEFINE_PROMOTE_BIND_OPERATOR1( bgr, - )


DEFINE_PROMOTE_BIND_OPERATOR2( bgr, - )


DEFINE_PROMOTE_BIND_OPERATOR4( bgr, - )


DEFINE_PROMOTE_BIND_OPERATOR1( bgr, * )


DEFINE_PROMOTE_BIND_OPERATOR2( bgr, * )


DEFINE_PROMOTE_BIND_OPERATOR3( bgr, * )


DEFINE_PROMOTE_BIND_OPERATOR1( bgr, / )


DEFINE_PROMOTE_BIND_OPERATOR2( bgr, / )


DEFINE_PROMOTE_BIND_OPERATOR1( bgr, % )



DEFINE_PROMOTE_BIND_OPERATOR1( bgr, | )


DEFINE_PROMOTE_BIND_OPERATOR1( bgr, & )


DEFINE_PROMOTE_BIND_OPERATOR1( bgr, ^ )




template< class T >
struct rgba : public rgb< T >
{
protected:
    typedef rgb< T > base;

public:
    typedef typename base::size_type        size_type;              
    typedef typename base::difference_type  difference_type;        
    typedef typename base::reference        reference;              
    typedef typename base::const_reference  const_reference;        
    typedef typename base::value_type       value_type;             
    typedef typename base::pointer          pointer;                
    typedef typename base::const_pointer    const_pointer;          

public:
    value_type a;       


    rgba( ) : base( ), a( 255 ){ }


    explicit rgba( const value_type &pix ) : base( pix ), a( 255 ){ }


    template < class TT >
    rgba( const rgba< TT > &c ) : base( c ), a( static_cast< value_type >( c.a ) ){ }


    rgba( const rgba< T > &c ) : base( c ), a( c.a ){ }



    template < class TT >
    rgba( const rgb< TT > &c ) : base( c ), a( 255 ){ }


    rgba( const rgb< T > &c ) : base( c ), a( 255 ){ }



    rgba( const value_type &rr, const value_type &gg, const value_type &bb, const value_type &aa = 255 ) : base( rr, gg, bb ), a( aa ){ }



    template < class TT >
    const rgba &operator =( const rgba< TT > &c )
    {
        base::operator =( c );
        a = static_cast< value_type >( c.a );
        return( *this );
    }


    const rgba &operator =( const rgba< T > &c )
    {
        if( &c != this )
        {
            base::operator =( c );
            a = c.a;
        }
        return( *this );
    }


    const rgba &operator =( const value_type &pix )
    {
        base::operator =( pix );
        return( *this );
    }



    const rgba  operator -( ) const { return( rgba( -base::r, -base::g, -base::b, a ) ); }


    template < class TT >
    const rgba &operator +=( const rgba< TT > &c ){ base::operator +=( ( const base &)c ); return( *this ); }


    template < class TT >
    const rgba &operator -=( const rgba< TT > &c ){ base::operator -=( ( const base &)c ); return( *this ); }


    template < class TT >
    const rgba &operator *=( const rgba< TT > &c ){ base::operator *=( ( const base &)c ); return( *this ); }


    template < class TT >
    const rgba &operator /=( const rgba< TT > &c ){ base::operator /=( ( const base &)c ); return( *this ); }


    const rgba &operator %=( const rgba &c ){ base::operator %=( ( const base &)c ); return( *this ); }


    const rgba &operator |=( const rgba &c ){ base::operator |=( ( const base &)c ); return( *this ); }


    const rgba &operator &=( const rgba &c ){ base::operator &=( ( const base &)c ); return( *this ); }


    const rgba &operator ^=( const rgba &c ){ base::operator ^=( ( const base &)c ); return( *this ); }



#if defined( __MIST_MSVC__ ) && __MIST_MSVC__ < 7
    const rgba &operator +=( const double &pix )
#else
    template < class TT >
    const rgba &operator +=( const TT &pix )
#endif
    {
        base::operator +=( pix );
        return( *this );
    }


#if defined( __MIST_MSVC__ ) && __MIST_MSVC__ < 7
    const rgba &operator -=( const double &pix )
#else
    template < class TT >
    const rgba &operator -=( const TT &pix )
#endif
    {
        base::operator -=( pix );
        return( *this );
    }


#if defined( __MIST_MSVC__ ) && __MIST_MSVC__ < 7
    const rgba &operator *=( const double &pix )
#else
    template < class TT >
    const rgba &operator *=( const TT &pix )
#endif
    {
        base::operator *=( pix );
        return( *this );
    }


#if defined( __MIST_MSVC__ ) && __MIST_MSVC__ < 7
    const rgba &operator /=( const double &pix )
#else
    template < class TT >
    const rgba &operator /=( const TT &pix )
#endif
    {
        base::operator /=( pix );
        return( *this );
    }


    bool operator ==( const rgba &c ) const { return( base::operator ==( c ) ); }


    bool operator !=( const rgba &c ) const { return( !( *this == c ) ); }

    bool operator < ( const rgba &c ) const
    {
        if( a == c.a )
        {
            return( base::operator <( c ) );
        }
        else
        {
            return( a < c.a );
        }
    }


    bool operator <=( const rgba &c ) const { return( c >= *this ); }


    bool operator > ( const rgba &c ) const { return( c < *this ); }

    bool operator >=( const rgba &c ) const { return( !( *this < c ) ); }

};


DEFINE_PROMOTE_BIND_OPERATOR1( rgba, + )


DEFINE_PROMOTE_BIND_OPERATOR2( rgba, + )


DEFINE_PROMOTE_BIND_OPERATOR3( rgba, + )


DEFINE_PROMOTE_BIND_OPERATOR1( rgba, - )


DEFINE_PROMOTE_BIND_OPERATOR2( rgba, - )


DEFINE_PROMOTE_BIND_OPERATOR4( rgba, - )


DEFINE_PROMOTE_BIND_OPERATOR1( rgba, * )


DEFINE_PROMOTE_BIND_OPERATOR2( rgba, * )


DEFINE_PROMOTE_BIND_OPERATOR3( rgba, * )


DEFINE_PROMOTE_BIND_OPERATOR1( rgba, / )


DEFINE_PROMOTE_BIND_OPERATOR2( rgba, / )


DEFINE_PROMOTE_BIND_OPERATOR1( rgba, % )



DEFINE_PROMOTE_BIND_OPERATOR1( rgba, | )


DEFINE_PROMOTE_BIND_OPERATOR1( rgba, & )


DEFINE_PROMOTE_BIND_OPERATOR1( rgba, ^ )









template< class T >
struct bgra : public bgr< T >
{
protected:
    typedef bgr< T > base;

public:
    typedef typename base::size_type        size_type;              
    typedef typename base::difference_type  difference_type;        
    typedef typename base::reference        reference;              
    typedef typename base::const_reference  const_reference;        
    typedef typename base::value_type       value_type;             
    typedef typename base::pointer          pointer;                
    typedef typename base::const_pointer    const_pointer;          

public:
    value_type a;       


    bgra( ) : base( ), a( 255 ){ }


    explicit bgra( const value_type &pix ) : base( pix ), a( 255 ){ }


    template < class TT >
    bgra( const bgra< TT > &c ) : base( c ), a( static_cast< value_type >( c.a ) ){ }


    bgra( const bgra< T > &c ) : base( c ), a( c.a ){ }



    template < class TT >
    bgra( const rgb< TT > &c ) : base( c ), a( 255 ){ }


    bgra( const rgb< T > &c ) : base( c ), a( 255 ){ }



    template < class TT >
    bgra( const bgr< TT > &c ) : base( c ), a( 255 ){ }


    bgra( const bgr< T > &c ) : base( c ), a( 255 ){ }


    template < class TT >
    bgra( const rgba< TT > &c ) : base( c ), a( c.a ){ }


    bgra( const value_type &rr, const value_type &gg, const value_type &bb, const value_type &aa = 255 ) : base( rr, gg, bb ), a( aa ){ }



    template < class TT >
    const bgra &operator =( const bgra< TT > &c )
    {
        base::operator =( c );
        a = static_cast< value_type >( c.a );
        return( *this );
    }


    const bgra &operator =( const bgra< T > &c )
    {
        if( &c != this )
        {
            base::operator =( c );
            a = c.a;
        }
        return( *this );
    }


    const bgra &operator =( const value_type &pix )
    {
        base::operator =( pix );
        return( *this );
    }



    const bgra  operator -( ) const { return( bgra( -base::r, -base::g, -base::b, a ) ); }


    template < class TT >
    const bgra &operator +=( const bgra< TT > &c ){ base::operator +=( ( const base &)c ); return( *this ); }


    template < class TT >
    const bgra &operator -=( const bgra< TT > &c ){ base::operator -=( ( const base &)c ); return( *this ); }


    template < class TT >
    const bgra &operator *=( const bgra< TT > &c ){ base::operator *=( ( const base &)c ); return( *this ); }


    template < class TT >
    const bgra &operator /=( const bgra< TT > &c ){ base::operator /=( ( const base &)c ); return( *this ); }


    const bgra &operator %=( const bgra &c ){ base::operator %=( ( const base &)c ); return( *this ); }


    const bgra &operator |=( const bgra &c ){ base::operator |=( ( const base &)c ); return( *this ); }


    const bgra &operator &=( const bgra &c ){ base::operator &=( ( const base &)c ); return( *this ); }


    const bgra &operator ^=( const bgra &c ){ base::operator ^=( ( const base &)c ); return( *this ); }



#if defined( __MIST_MSVC__ ) && __MIST_MSVC__ < 7
    const bgra &operator +=( const double &pix )
#else
    template < class TT >
    const bgra &operator +=( const TT &pix )
#endif
    {
        base::operator +=( pix );
        return( *this );
    }


#if defined( __MIST_MSVC__ ) && __MIST_MSVC__ < 7
    const bgra &operator -=( const double &pix )
#else
    template < class TT >
    const bgra &operator -=( const TT &pix )
#endif
    {
        base::operator -=( pix );
        return( *this );
    }


#if defined( __MIST_MSVC__ ) && __MIST_MSVC__ < 7
    const bgra &operator *=( const double &pix )
#else
    template < class TT >
    const bgra &operator *=( const TT &pix )
#endif
    {
        base::operator *=( pix );
        return( *this );
    }


#if defined( __MIST_MSVC__ ) && __MIST_MSVC__ < 7
    const bgra &operator /=( const double &pix )
#else
    template < class TT >
    const bgra &operator /=( const TT &pix )
#endif
    {
        base::operator /=( pix );
        return( *this );
    }


    bool operator ==( const bgra &c ) const { return( base::operator ==( c ) ); }

    bool operator !=( const bgra &c ) const { return( !( *this == c ) ); }


    bool operator < ( const bgra &c ) const
    {
        if( a == c.a )
        {
            return( base::operator <( c ) );
        }
        else
        {
            return( a < c.a );
        }
    }



    bool operator <=( const bgra &c ) const { return( c >= *this ); }


    bool operator > ( const bgra &c ) const { return( c < *this ); }


    bool operator >=( const bgra &c ) const { return( !( *this < c ) ); }

};


DEFINE_PROMOTE_BIND_OPERATOR1( bgra, + )


DEFINE_PROMOTE_BIND_OPERATOR2( bgra, + )


DEFINE_PROMOTE_BIND_OPERATOR3( bgra, + )


DEFINE_PROMOTE_BIND_OPERATOR1( bgra, - )


DEFINE_PROMOTE_BIND_OPERATOR2( bgra, - )


DEFINE_PROMOTE_BIND_OPERATOR4( bgra, - )


DEFINE_PROMOTE_BIND_OPERATOR1( bgra, * )


DEFINE_PROMOTE_BIND_OPERATOR2( bgra, * )


DEFINE_PROMOTE_BIND_OPERATOR3( bgra, * )


DEFINE_PROMOTE_BIND_OPERATOR1( bgra, / )


DEFINE_PROMOTE_BIND_OPERATOR2( bgra, / )


DEFINE_PROMOTE_BIND_OPERATOR1( bgra, % )



DEFINE_PROMOTE_BIND_OPERATOR1( bgra, | )


DEFINE_PROMOTE_BIND_OPERATOR1( bgra, & )


DEFINE_PROMOTE_BIND_OPERATOR1( bgra, ^ )







template < class T > inline std::ostream &operator <<( std::ostream &out, const rgb< T > &c )
{
    out << "( ";
    out << c.r << ", ";
    out << c.g << ", ";
    out << c.b << " )";
    return( out );
}



template < class T > inline std::ostream &operator <<( std::ostream &out, const rgba< T > &c )
{
    out << "( ";
    out << c.r << ", ";
    out << c.g << ", ";
    out << c.b << ", ";
    out << c.a << " )";
    return( out );
}


namespace __color_utility__
{
    template < class T > inline T maximum( const T &v1, const T &v2 )
    {
        return( v1 > v2 ? v1 : v2 );
    }

    template < class T > inline T minimum( const T &v1, const T &v2 )
    {
        return( v1 < v2 ? v1 : v2 );
    }
}



inline void rgb2hsv( double r, double g, double b, double &h, double &s, double &v )
{
    double max = __color_utility__::maximum( r, __color_utility__::maximum( g, b ) );
    double min = __color_utility__::minimum( r, __color_utility__::minimum( g, b ) );

    double d = max - min;
    v = max / 255.0;

    if( d != 0.0 )
    {
        s = d / max;
    }
    else
    {
        s = 0.0;
    }

    if( s == 0.0 )
    {
        h = 0.0;
    }
    else
    {
        double rt = max - r * 60.0 / d;
        double gt = max - g * 60.0 / d;
        double bt = max - b * 60.0 / d;

        if( r == max )
        {
            h = bt - gt;
        }
        else if( g == max )
        {
            h = 120 + rt - bt;
        }
        else
        {
            h = 240 + gt - rt;
        }

        if( h < 0.0 )
        {
            h += 360.0;
        }
    }
}


 
inline void hsv2rgb( double h, double s, double v, double &r, double &g, double &b )
{
    if( s == 0.0 )
    {
        r = g = b = v * 255.0;
    }
    else
    {
        int ht = static_cast< int >( h * 6.0 );
        int d  = ht % 360;

        ht /= 360;

        double t1 = v * ( 1.0 - s );
        double t2 = v * ( 1.0 - s * d / 360.0 );
        double t3 = v * ( 1.0 - s * ( 360.0 - d ) / 360.0 );

        switch( ht )
        {
        case 0:
            r = v;
            g = t3;
            b = t1;
            break;

        case 1:
            r = t2;
            g = v;
            b = t1;
            break;

        case 2:
            r = t1;
            g = v;
            b = t3;
            break;

        case 3:
            r = t1;
            g = t2;
            b = v;
            break;

        case 4:
            r = t3;
            g = t1;
            b = v;
            break;

        default:
            r = v;
            g = t1;
            b = t2;
            break;
        }

        r *= 255.0;
        g *= 255.0;
        b *= 255.0;
    }
}





inline void rgb2xyz( double r, double g, double b, double &x, double &y, double &z )
{
    x = ( 0.412453 * r + 0.357580 * g + 0.180423 * b ) / 255.0;
    y = ( 0.212671 * r + 0.715160 * g + 0.072169 * b ) / 255.0;
    z = ( 0.019334 * r + 0.119193 * g + 0.950227 * b ) / 255.0;
}

 
inline void xyz2rgb( double x, double y, double z, double &r, double &g, double &b )
{
    r = (  3.240479 * x - 1.537150 * y - 0.498535 * z ) * 255.0;
    g = ( -0.969256 * x + 1.875991 * y + 0.041556 * z ) * 255.0;
    b = (  0.055648 * x - 0.204043 * y + 1.057311 * z ) * 255.0;
}


 
inline void rgb2lab( double r, double g, double b, double &l_, double &a_, double &b_ )
{
    double x, y, z;

    rgb2xyz( r, g, b, x, y, z );

    static const double Xr = 0.9504;
    static const double Yr = 1.0;
    static const double Zr = 1.0889;

    x /= Xr;
    y /= Yr;
    z /= Zr;

    static const double coef = 16.0 / 116.0;
    x = x > 0.008856 ? std::pow( x, 0.33333333333333333333333333333333333 ) : 7.787 * x + coef;
    y = y > 0.008856 ? std::pow( y, 0.33333333333333333333333333333333333 ) : 7.787 * y + coef;
    z = z > 0.008856 ? std::pow( z, 0.33333333333333333333333333333333333 ) : 7.787 * z + coef;

    l_ = 116.0 * y - 16.0;
    a_ = 500.0 * ( x - y );
    b_ = 200.0 * ( y - z );
}


inline void lab2rgb( double l_, double a_, double b_, double &r, double &g, double &b )
{
    double fy = ( l_ + 16.0 ) / 116.0;
    double fx = fy + a_ / 500.0 ;
    double fz = fy - b_ / 200.0;

    static const double Xr = 0.9504;
    static const double Yr = 1.0;
    static const double Zr = 1.0889;

    static const double delta = 6.0 / 29.0;
    static const double delta3 = 3.0 * delta * delta * delta;
    double y = fy > delta ? Yr * fy * fy * fy : ( fy - 16.0 / 116.0 ) * delta3 * Yr;
    double x = fx > delta ? Yr * fx * fx * fx : ( fx - 16.0 / 116.0 ) * delta3 * Xr;
    double z = fz > delta ? Yr * fz * fz * fz : ( fz - 16.0 / 116.0 ) * delta3 * Zr;


    xyz2rgb( x, y, z, r, g, b );
}


template < class T >
struct is_color
{
    _MIST_CONST( bool, value, false );
};



template < class T >
struct _pixel_converter_
{
    typedef T value_type;
    typedef rgba< T > color_type;
    enum{ color_num = 1 };

    static value_type convert_to( value_type r, value_type g, value_type b, value_type a = 255 )
    {
        return( color_type( r, g, b ).get_value( ) );
    }

    static color_type convert_from( const value_type &pixel )
    {
        return( color_type( pixel ) );
    }
};

#if defined(__MIST_MSVC__) && __MIST_MSVC__ < 7

    #define IS_COLOR( type ) \
        template < >\
        struct is_color< rgb< type > >\
        {\
            enum{ value = true };\
        };\
        template < >\
        struct is_color< bgr< type > >\
        {\
            enum{ value = true };\
        };\
        template < >\
        struct is_color< rgba< type > >\
        {\
            enum{ value = true };\
        };\
        template < >\
        struct is_color< bgra< type > >\
        {\
            enum{ value = true };\
        };\


    #define __PIXEL_CONVERTER__( type ) \
        template < >\
        struct _pixel_converter_< rgb< type > >\
        {\
            typedef type value_type;\
            typedef rgba< type > color_type;\
            enum{ color_num = 3 };\
            \
            static color_type convert_to( value_type r, value_type g, value_type b, value_type a = 255 )\
            {\
                return( color_type( r, g, b ) );\
            }\
            \
            static color_type convert_from( const color_type &pixel )\
            {\
                return( pixel );\
            }\
        };\
        template < >\
        struct _pixel_converter_< bgr< type > >\
        {\
            typedef type value_type;\
            typedef rgba< type > color_type;\
            enum{ color_num = 3 };\
            \
            static color_type convert_to( value_type r, value_type g, value_type b, value_type a = 255 )\
            {\
                return( color_type( r, g, b ) );\
            }\
            \
            static color_type convert_from( const color_type &pixel )\
            {\
                return( pixel );\
            }\
        };\
        template < >\
        struct _pixel_converter_< rgba< type > >\
        {\
            typedef type value_type;\
            typedef rgba< type > color_type;\
            enum{ color_num = 4 };\
            \
            static color_type convert_to( value_type r, value_type g, value_type b, value_type a = 255 )\
            {\
                return( color_type( r, g, b, a ) );\
            }\
            \
            static color_type convert_from( const color_type &pixel )\
            {\
                return( pixel );\
            }\
        };\
        template < >\
        struct _pixel_converter_< bgra< type > >\
        {\
            typedef type value_type;\
            typedef bgra< type > color_type;\
            enum{ color_num = 4 };\
            \
            static color_type convert_to( value_type r, value_type g, value_type b, value_type a = 255 )\
            {\
                return( color_type( r, g, b, a ) );\
            }\
            \
            static color_type convert_from( const color_type &pixel )\
            {\
                return( pixel );\
            }\
        };\


        IS_COLOR(unsigned char)
        IS_COLOR(unsigned short)
        IS_COLOR(unsigned int)
        IS_COLOR(unsigned long)
        IS_COLOR(signed char)
        IS_COLOR(signed short)
        IS_COLOR(signed int)
        IS_COLOR(signed long)
        IS_COLOR(bool)
        IS_COLOR(char)
        IS_COLOR(float)
        IS_COLOR(double)
        IS_COLOR(long double)
        __PIXEL_CONVERTER__(unsigned char)
        __PIXEL_CONVERTER__(unsigned short)
        __PIXEL_CONVERTER__(unsigned int)
        __PIXEL_CONVERTER__(unsigned long)
        __PIXEL_CONVERTER__(signed char)
        __PIXEL_CONVERTER__(signed short)
        __PIXEL_CONVERTER__(signed int)
        __PIXEL_CONVERTER__(signed long)
        __PIXEL_CONVERTER__(bool)
        __PIXEL_CONVERTER__(char)
        __PIXEL_CONVERTER__(float)
        __PIXEL_CONVERTER__(double)
        __PIXEL_CONVERTER__(long double)

        #undef IS_COLOR
        #undef __PIXEL_CONVERTER__

#else

    template < class T >
    struct is_color< rgb< T > >
    {
        _MIST_CONST( bool, value, true );
    };

    template < class T >
    struct is_color< bgr< T > >
    {
        _MIST_CONST( bool, value, true );
    };

    template < class T >
    struct is_color< rgba< T > >
    {
        _MIST_CONST( bool, value, true );
    };

    template < class T >
    struct is_color< bgra< T > >
    {
        _MIST_CONST( bool, value, true );
    };

    template < class T >
    struct _pixel_converter_< rgb< T > >
    {
        typedef T value_type;
        typedef rgba< T > color_type;
        enum{ color_num = 3 };

        static color_type convert_to( value_type r, value_type g, value_type b, value_type a = 255 )
        {
            return( color_type( r, g, b, a ) );
        }

        static color_type convert_from( const color_type &pixel )
        {
            return( pixel );
        }
    };

    template < class T >
    struct _pixel_converter_< bgr< T > >
    {
        typedef T value_type;
        typedef rgba< T > color_type;
        enum{ color_num = 3 };

        static color_type convert_to( value_type r, value_type g, value_type b, value_type a = 255 )
        {
            return( color_type( r, g, b, a ) );
        }

        static color_type convert_from( const color_type &pixel )
        {
            return( pixel );
        }
    };

    template < class T >
    struct _pixel_converter_< rgba< T > >
    {
        typedef T value_type;
        typedef rgba< T > color_type;
        enum{ color_num = 4 };

        static color_type convert_to( value_type r, value_type g, value_type b, value_type a = 255 )
        {
            return( color_type( r, g, b, a ) );
        }

        static color_type convert_from( const color_type &pixel )
        {
            return( pixel );
        }
    };

    template < class T >
    struct _pixel_converter_< bgra< T > >
    {
        typedef T value_type;
        typedef bgra< T > color_type;
        enum{ color_num = 4 };

        static color_type convert_to( value_type r, value_type g, value_type b, value_type a = 255 )
        {
            return( color_type( r, g, b, a ) );
        }

        static color_type convert_from( const color_type &pixel )
        {
            return( pixel );
        }
    };

#endif



_MIST_END


#endif // __INCLUDE_MIST_COLOR_H__
