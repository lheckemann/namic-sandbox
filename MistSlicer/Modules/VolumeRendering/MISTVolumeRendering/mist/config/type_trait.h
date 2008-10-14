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



#ifndef __INCLUDE_MIST_TYPE_TRAIT_H__
#define __INCLUDE_MIST_TYPE_TRAIT_H__

#ifndef __INCLUDE_MIST_CONF_H__
#include "mist_conf.h"
#endif


_MIST_BEGIN

 
template< class T > struct is_char        { _MIST_CONST( bool, value, false ); };
template<> struct is_char< unsigned char >{ _MIST_CONST( bool, value, true  ); };
template<> struct is_char< signed char >  { _MIST_CONST( bool, value, true  ); };
template<> struct is_char< char >         { _MIST_CONST( bool, value, true  ); };



template< class T > struct is_float       { _MIST_CONST( bool, value, false ); };
template<> struct is_float< float >       { _MIST_CONST( bool, value, true  ); };
template<> struct is_float< double >      { _MIST_CONST( bool, value, true  ); };
template<> struct is_float< long double > { _MIST_CONST( bool, value, true  ); };



template< class T > struct is_integer                  { _MIST_CONST( bool, value, false ); };
template<>          struct is_integer< unsigned char > { _MIST_CONST( bool, value, true  ); };
template<>          struct is_integer< unsigned short >{ _MIST_CONST( bool, value, true  ); };
template<>          struct is_integer< unsigned int >  { _MIST_CONST( bool, value, true  ); };
template<>          struct is_integer< unsigned long > { _MIST_CONST( bool, value, true  ); };
template<>          struct is_integer< signed char >   { _MIST_CONST( bool, value, true  ); };
template<>          struct is_integer< signed short >  { _MIST_CONST( bool, value, true  ); };
template<>          struct is_integer< signed int >    { _MIST_CONST( bool, value, true  ); };
template<>          struct is_integer< signed long >   { _MIST_CONST( bool, value, true  ); };
template<>          struct is_integer< bool >          { _MIST_CONST( bool, value, true  ); };
template<>          struct is_integer< char >          { _MIST_CONST( bool, value, true  ); };

#if defined( __MIST64__ ) && __MIST64__ != 0 && !( defined( __APPLE__ ) && defined( __ICC ) )
template<>          struct is_integer< size_t >        { _MIST_CONST( bool, value, true  ); };
template<>          struct is_integer< ptrdiff_t >     { _MIST_CONST( bool, value, true  ); };
#endif




template< class T > struct is_arithmetic                  { _MIST_CONST( bool, value, false ); };
template<>          struct is_arithmetic< unsigned char > { _MIST_CONST( bool, value, true  ); };
template<>          struct is_arithmetic< unsigned short >{ _MIST_CONST( bool, value, true  ); };
template<>          struct is_arithmetic< unsigned int >  { _MIST_CONST( bool, value, true  ); };
template<>          struct is_arithmetic< unsigned long > { _MIST_CONST( bool, value, true  ); };
template<>          struct is_arithmetic< signed char >   { _MIST_CONST( bool, value, true  ); };
template<>          struct is_arithmetic< signed short >  { _MIST_CONST( bool, value, true  ); };
template<>          struct is_arithmetic< signed int >    { _MIST_CONST( bool, value, true  ); };
template<>          struct is_arithmetic< signed long >   { _MIST_CONST( bool, value, true  ); };
template<>          struct is_arithmetic< bool >          { _MIST_CONST( bool, value, true  ); };
template<>          struct is_arithmetic< char >          { _MIST_CONST( bool, value, true  ); };
template<>          struct is_arithmetic< float >         { _MIST_CONST( bool, value, true  ); };
template<>          struct is_arithmetic< double >        { _MIST_CONST( bool, value, true  ); };
template<>          struct is_arithmetic< long double >   { _MIST_CONST( bool, value, true  ); };

#if defined( __MIST64__ ) && __MIST64__ != 0 && !( defined( __APPLE__ ) && defined( __ICC ) )
template<>          struct is_arithmetic< size_t >        { _MIST_CONST( bool, value, true  ); };
template<>          struct is_arithmetic< ptrdiff_t >     { _MIST_CONST( bool, value, true  ); };
#endif

 
template< class T > struct is_builtin                  { _MIST_CONST( bool, value, false ); };
template<>          struct is_builtin< unsigned char > { _MIST_CONST( bool, value, true  ); };
template<>          struct is_builtin< unsigned short >{ _MIST_CONST( bool, value, true  ); };
template<>          struct is_builtin< unsigned int >  { _MIST_CONST( bool, value, true  ); };
template<>          struct is_builtin< unsigned long > { _MIST_CONST( bool, value, true  ); };
template<>          struct is_builtin< signed char >   { _MIST_CONST( bool, value, true  ); };
template<>          struct is_builtin< signed short >  { _MIST_CONST( bool, value, true  ); };
template<>          struct is_builtin< signed int >    { _MIST_CONST( bool, value, true  ); };
template<>          struct is_builtin< signed long >   { _MIST_CONST( bool, value, true  ); };
template<>          struct is_builtin< bool >          { _MIST_CONST( bool, value, true  ); };
template<>          struct is_builtin< char >          { _MIST_CONST( bool, value, true  ); };
template<>          struct is_builtin< float >         { _MIST_CONST( bool, value, true  ); };
template<>          struct is_builtin< double >        { _MIST_CONST( bool, value, true  ); };
template<>          struct is_builtin< long double >   { _MIST_CONST( bool, value, true  ); };

#if defined( __MIST64__ ) && __MIST64__ != 0 && !( defined( __APPLE__ ) && defined( __ICC ) )
template<>          struct is_builtin< size_t >        { _MIST_CONST( bool, value, true  ); };
template<>          struct is_builtin< ptrdiff_t >     { _MIST_CONST( bool, value, true  ); };
#endif

 
template< class T > struct float_type       { typedef double value_type; };
template<> struct float_type< float >       { typedef float value_type; };
template<> struct float_type< long double > { typedef long double value_type; };




template< class T1, class T2 > struct promote_trait { typedef T1 value_type; };

#define PROMOTE_TRAIT( T1, T2, T3 ) \
        template< > struct promote_trait< T1, T2 > { typedef T3 value_type; };  \
        template< > struct promote_trait< T2, T1 > { typedef T3 value_type; };

PROMOTE_TRAIT( unsigned  char, unsigned short, unsigned short )
PROMOTE_TRAIT( unsigned  char, unsigned   int, unsigned   int )
PROMOTE_TRAIT( unsigned  char, unsigned  long, unsigned  long )
PROMOTE_TRAIT( unsigned  char,   signed  char,   signed short )
PROMOTE_TRAIT( unsigned  char,   signed short,   signed short )
PROMOTE_TRAIT( unsigned  char,   signed   int,   signed   int )
PROMOTE_TRAIT( unsigned  char,   signed  long,   signed  long )
PROMOTE_TRAIT( unsigned  char,           bool, unsigned  char )
PROMOTE_TRAIT( unsigned  char,           char,   signed  char )
PROMOTE_TRAIT( unsigned  char,          float,          float )
PROMOTE_TRAIT( unsigned  char,         double,         double )
PROMOTE_TRAIT( unsigned  char,    long double,    long double )
PROMOTE_TRAIT(   signed  char, unsigned short,   signed   int )
PROMOTE_TRAIT(   signed  char, unsigned   int,   signed  long )
PROMOTE_TRAIT(   signed  char, unsigned  long,   signed  long )
PROMOTE_TRAIT(   signed  char,   signed short,   signed short )
PROMOTE_TRAIT(   signed  char,   signed   int,   signed   int )
PROMOTE_TRAIT(   signed  char,   signed  long,   signed  long )
PROMOTE_TRAIT(   signed  char,           bool,   signed  char )
PROMOTE_TRAIT(   signed  char,           char,   signed  char )
PROMOTE_TRAIT(   signed  char,          float,          float )
PROMOTE_TRAIT(   signed  char,         double,         double )
PROMOTE_TRAIT(   signed  char,    long double,    long double )
PROMOTE_TRAIT(           char, unsigned short,   signed   int )
PROMOTE_TRAIT(           char, unsigned   int,   signed  long )
PROMOTE_TRAIT(           char, unsigned  long,   signed  long )
PROMOTE_TRAIT(           char,   signed short,   signed short )
PROMOTE_TRAIT(           char,   signed   int,   signed   int )
PROMOTE_TRAIT(           char,   signed  long,   signed  long )
PROMOTE_TRAIT(           char,           bool,           char )
PROMOTE_TRAIT(           char,          float,          float )
PROMOTE_TRAIT(           char,         double,         double )
PROMOTE_TRAIT(           char,    long double,    long double )
PROMOTE_TRAIT( unsigned short, unsigned   int, unsigned   int )
PROMOTE_TRAIT( unsigned short, unsigned  long, unsigned  long )
PROMOTE_TRAIT( unsigned short,   signed short,   signed   int )
PROMOTE_TRAIT( unsigned short,   signed   int,   signed   int )
PROMOTE_TRAIT( unsigned short,   signed  long,   signed  long )
PROMOTE_TRAIT( unsigned short,           bool, unsigned short )
PROMOTE_TRAIT( unsigned short,          float,          float )
PROMOTE_TRAIT( unsigned short,         double,         double )
PROMOTE_TRAIT( unsigned short,    long double,    long double )
PROMOTE_TRAIT(   signed short, unsigned   int,   signed   int )
PROMOTE_TRAIT(   signed short, unsigned  long,   signed  long )
PROMOTE_TRAIT(   signed short,   signed   int,   signed   int )
PROMOTE_TRAIT(   signed short,   signed  long,   signed  long )
PROMOTE_TRAIT(   signed short,           bool,   signed short )
PROMOTE_TRAIT(   signed short,          float,          float )
PROMOTE_TRAIT(   signed short,         double,         double )
PROMOTE_TRAIT(   signed short,    long double,    long double )
PROMOTE_TRAIT( unsigned   int, unsigned  long, unsigned  long )
PROMOTE_TRAIT( unsigned   int,   signed   int,   signed  long )
PROMOTE_TRAIT( unsigned   int,   signed  long,   signed  long )
PROMOTE_TRAIT( unsigned   int,           bool, unsigned   int )
PROMOTE_TRAIT( unsigned   int,          float,         double )
PROMOTE_TRAIT( unsigned   int,         double,         double )
PROMOTE_TRAIT( unsigned   int,    long double,    long double )
PROMOTE_TRAIT(   signed   int, unsigned  long,   signed  long )
PROMOTE_TRAIT(   signed   int,   signed  long,   signed  long )
PROMOTE_TRAIT(   signed   int,           bool,   signed   int )
PROMOTE_TRAIT(   signed   int,          float,         double )
PROMOTE_TRAIT(   signed   int,         double,         double )
PROMOTE_TRAIT(   signed   int,    long double,    long double )
PROMOTE_TRAIT( unsigned  long,   signed  long,   signed  long )
PROMOTE_TRAIT( unsigned  long,           bool, unsigned  long )
PROMOTE_TRAIT( unsigned  long,          float,    long double )
PROMOTE_TRAIT( unsigned  long,         double,         double )
PROMOTE_TRAIT( unsigned  long,    long double,    long double )
PROMOTE_TRAIT(   signed  long,           bool,   signed   int )
PROMOTE_TRAIT(   signed  long,          float,         double )
PROMOTE_TRAIT(   signed  long,         double,         double )
PROMOTE_TRAIT(   signed  long,    long double,    long double )
PROMOTE_TRAIT(           bool,          float,          float )
PROMOTE_TRAIT(           bool,         double,         double )
PROMOTE_TRAIT(           bool,    long double,    long double )
PROMOTE_TRAIT(          float,         double,         double )
PROMOTE_TRAIT(          float,    long double,    long double )
PROMOTE_TRAIT(         double,    long double,    long double )

#undef PROMOTE_TRAIT

#if defined( __MIST_MSVC__ ) && __MIST_MSVC__ < 7

    #define DEFINE_PROMOTE_BIND_OPERATOR1( TYPE, OPERATOR )                                         \
        template < class T1, class T2 >                                                             \
        inline TYPE< promote_trait< T1, T2 >::value_type >                                          \
                    operator OPERATOR( const TYPE< T1 > &t1, const TYPE< T2 > &t2 )                 \
        {                                                                                           \
            return( TYPE< promote_trait< T1, T2 >::value_type >( t1 ) OPERATOR ## = t2 );           \
        }

    #define DEFINE_PROMOTE_BIND_OPERATOR2( TYPE, OPERATOR )                                             \
        template < class T >                                                                            \
        inline TYPE< promote_trait< T, double >::value_type >                                           \
                    operator OPERATOR( const TYPE< T > &t1, const double &t2 )                          \
        {                                                                                               \
            return( TYPE< promote_trait< T, double >::value_type >( t1 ) OPERATOR ## = t2 );            \
        }


    #define DEFINE_PROMOTE_BIND_OPERATOR3( TYPE, OPERATOR )                                             \
        template < class T >                                                                            \
        inline TYPE< promote_trait< T, double >::value_type >                                           \
                    operator OPERATOR( const double &t1, const TYPE< T > &t2 )                          \
        {                                                                                               \
            return( TYPE< promote_trait< T, double >::value_type >( t2 ) OPERATOR ## = t1 );            \
        }


    #define DEFINE_PROMOTE_BIND_OPERATOR4( TYPE, OPERATOR )                                             \
        template < class T >                                                                            \
        inline TYPE< promote_trait< T, double >::value_type >                                           \
                    operator OPERATOR( const double &t1, const TYPE< T > &t2 )                          \
        {                                                                                               \
            return( TYPE< promote_trait< T, double >::value_type >( t1 ) OPERATOR ## = t2 );            \
    }

#else


    #define DEFINE_PROMOTE_BIND_OPERATOR1( TYPE, OPERATOR )                                         \
        template < class T1, class T2 >                                                             \
        inline TYPE< typename promote_trait< T1, T2 >::value_type >                                 \
                    operator OPERATOR( const TYPE< T1 > &t1, const TYPE< T2 > &t2 )                 \
        {                                                                                           \
            return( TYPE< typename promote_trait< T1, T2 >::value_type >( t1 ) OPERATOR ## = t2 );  \
        }


    #define DEFINE_PROMOTE_BIND_OPERATOR2( TYPE, OPERATOR )                                         \
        template < class T1, class T2 >                                                             \
        inline TYPE< typename promote_trait< T1, T2 >::value_type >                                 \
                    operator OPERATOR( const TYPE< T1 > &t1, const T2 &t2 )                         \
        {                                                                                           \
            return( TYPE< typename promote_trait< T1, T2 >::value_type >( t1 ) OPERATOR ## = t2 );  \
        }


    #define DEFINE_PROMOTE_BIND_OPERATOR3( TYPE, OPERATOR )                                         \
        template < class T1, class T2 >                                                             \
        inline TYPE< typename promote_trait< T1, T2 >::value_type >                                 \
                    operator OPERATOR( const T1 &t1, const TYPE< T2 > &t2 )                         \
        {                                                                                           \
            return( TYPE< typename promote_trait< T1, T2 >::value_type >( t2 ) OPERATOR ## = t1 );  \
        }


    #define DEFINE_PROMOTE_BIND_OPERATOR4( TYPE, OPERATOR )                                         \
        template < class T1, class T2 >                                                             \
        inline TYPE< typename promote_trait< T1, T2 >::value_type >                                 \
                    operator OPERATOR( const T1 &t1, const TYPE< T2 > &t2 )                         \
        {                                                                                           \
            return( TYPE< typename promote_trait< T1, T2 >::value_type >( t1 ) OPERATOR ## = t2 );  \
    }

#endif


 
template< class T >
struct type_trait{ typedef T value_type; };


template < bool b >
struct __half_adjust__
{
    static double convert( const double value )
    {
        return( value + 0.5 );
    }
};


template < >
struct __half_adjust__< false >
{
    static double convert( const double value )
    {
        return( value );
    }
};


 
template < class T >
struct half_adjust
{
    typedef T value_type;

    static value_type convert( const double value )
    {
        return( static_cast< value_type >( __half_adjust__< is_integer< value_type >::value >::convert( value ) ) );
    }
};

 
template < bool b1, bool b2, bool b3 = true, bool b4 = true, bool b5 = true, bool b6 = true, bool b7 = true >
struct type_and
{
    _MIST_CONST( bool, value, false );
};

template <>
struct type_and< true, true, true, true, true, true, true >
{
    _MIST_CONST( bool, value, true  );
};



template < bool b1, bool b2, bool b3 = false, bool b4 = false, bool b5 = false, bool b6 = false, bool b7 = false >
struct type_or
{
    _MIST_CONST( bool, value, true );
};

template <>
struct type_or< false, false, false, false, false, false, false >
{
    _MIST_CONST( bool, value, false  );
};




template < bool b1 >
struct type_not
{
    _MIST_CONST( bool, value, true  );
};

template <>
struct type_not< true >
{
    _MIST_CONST( bool, value, false );
};




template < bool b1, bool b2 >
struct type_equal
{
    _MIST_CONST( bool, value, false );
};

template <>
struct type_equal< true, true >
{
    _MIST_CONST( bool, value, true  );
};

template <>
struct type_equal< false, false >
{
    _MIST_CONST( bool, value, true  );
};



_MIST_END


#endif // __INCLUDE_MIST_TYPE_TRAIT_H__
