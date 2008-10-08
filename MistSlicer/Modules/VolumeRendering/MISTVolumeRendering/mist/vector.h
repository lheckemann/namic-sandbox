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


#ifndef __INCLUDE_MIST_VECTOR__
#define __INCLUDE_MIST_VECTOR__


#ifndef __INCLUDE_MIST_CONF_H__
#include "config/mist_conf.h"
#endif

#ifndef __INCLUDE_MIST_TYPE_TRAIT_H__
#include "config/type_trait.h"
#endif

#include <cmath>


_MIST_BEGIN



template < class T >
class vector3
{
public:
    typedef T value_type;                                       
    typedef size_t size_type;                                   
    typedef ptrdiff_t difference_type;                          
    typedef typename float_type< T >::value_type float_type;    

public:
    value_type x;       
    value_type y;       
    value_type z;       

    
    vector3( ) : x( 0 ), y( 0 ), z( 0 ){ }

    
    vector3( const value_type &xx, const value_type &yy, const value_type &zz ) : x( xx ), y( yy ), z( zz ){ }

    
    explicit vector3( const value_type &vv ) : x( vv ), y( vv ), z( vv ){ }


    
    template < class TT >
    vector3( const vector3< TT > &v ) : x( static_cast< value_type >( v.x ) ), y( static_cast< value_type >( v.y ) ), z( static_cast< value_type >( v.z ) ){ }


    
    vector3( const vector3< T > &v ) : x( v.x ), y( v.y ), z( v.z ){ }



    template < class TT >
    const vector3 &operator =( const vector3< TT > &v )
    {
        x = static_cast< value_type >( v.x );
        y = static_cast< value_type >( v.y );
        z = static_cast< value_type >( v.z );
        return ( *this );
    }


    const vector3 &operator =( const vector3< T > &v )
    {
        if( &v != this )
        {
            x = v.x;
            y = v.y;
            z = v.z;
        }
        return ( *this );
    }


    vector3 operator -( ) const { return ( vector3( -x, -y, -z ) ); }



    template < class TT >
    vector3 &operator +=( const vector3< TT > &v )
    {
        x = static_cast< value_type >( x + v.x );
        y = static_cast< value_type >( y + v.y );
        z = static_cast< value_type >( z + v.z );
        return( *this );
    }


    template < class TT >
    vector3 &operator -=( const vector3< TT > &v )
    {
        x = static_cast< value_type >( x - v.x );
        y = static_cast< value_type >( y - v.y );
        z = static_cast< value_type >( z - v.z );
        return( *this );
    }


    template < class TT >
    vector3 &operator *=( const vector3< TT > &v )
    {
        value_type xx = static_cast< value_type >( y * v.z - z * v.y );
        value_type yy = static_cast< value_type >( z * v.x - x * v.z );
        value_type zz = static_cast< value_type >( x * v.y - y * v.x );
        x = xx;
        y = yy;
        z = zz;
        return( *this );
    }


#if defined( __MIST_MSVC__ ) && __MIST_MSVC__ < 7
    vector3 &operator *=( const double &a )
#else
    template < class TT >
    vector3 &operator *=( const TT &a )
#endif
    {
        x = static_cast< value_type >( x * a );
        y = static_cast< value_type >( y * a );
        z = static_cast< value_type >( z * a );
        return( *this );
    }



#if defined( __MIST_MSVC__ ) && __MIST_MSVC__ < 7
    vector3 &operator /=( const double &a )
#else
    template < class TT >
    vector3 &operator /=( const TT &a )
#endif
    {
        x = static_cast< value_type >( x / a );
        y = static_cast< value_type >( y / a );
        z = static_cast< value_type >( z / a );
        return( *this );
    }



    bool operator ==( const vector3 &v ) const { return( x == v.x && y == v.y && z == v.z ); }


    bool operator !=( const vector3 &v ) const { return( !( *this == v ) ); }
 
    bool operator < ( const vector3 &v ) const
    {
        if( x == v.x )
        {
            if( y == v.y )
            {
                return( z < v.z );
            }
            else
            {
                return( y < v.y );
            }
        }
        else
        {
            return( x < v.x );
        }
    }

 
    bool operator <=( const vector3 &v ) const { return( v >= *this ); }

    bool operator > ( const vector3 &v ) const { return( v < *this ); }

    bool operator >=( const vector3 &v ) const { return( !( *this < v ) ); }


    vector3 unit( ) const
    {
        float_type length_ = length( );
        if( length_ > 0 )
        {
            return( vector3( static_cast< value_type >( x / length_ ), static_cast< value_type >( y / length_ ), static_cast< value_type >( z / length_ ) ) );
        }
        else
        {
            return( *this );
        }
    }


    template < class TT >
    typename promote_trait< T, TT >::value_type inner( const vector3< TT > &v ) const
    {
        return( static_cast< typename promote_trait< T, TT >::value_type >( x * v.x + y * v.y + z * v.z ) );
    }


    template < class TT >
    vector3< typename promote_trait< value_type, TT >::value_type > outer( const vector3< TT > &v ) const
    {
        return( vector3< typename promote_trait< value_type, TT >::value_type >( y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x ) );
    }


    float_type length( ) const { return ( static_cast< float_type >( std::sqrt( static_cast< double >( x * x + y * y + z * z ) ) ) ); }
 
    vector3 rotate( const vector3 &v, double theta ) const
    {
        theta *= 3.1415926535897932384626433832795 / 180.0;
        double cs = std::cos( theta ), sn = std::sin( theta );
        value_type xx = static_cast< value_type >( ( v.x * v.x * ( 1.0 - cs ) + cs ) * x + ( v.x * v.y * ( 1.0 - cs ) - v.z * sn ) * y + ( v.x * v.z * ( 1.0 - cs ) + v.y * sn ) * z );
        value_type yy = static_cast< value_type >( ( v.x * v.y * ( 1.0 - cs ) + v.z * sn ) * x + ( v.y * v.y * ( 1.0 - cs ) + cs ) * y + ( v.y * v.z * ( 1.0 - cs ) - v.x * sn ) * z );
        value_type zz = static_cast< value_type >( ( v.x * v.z * ( 1.0 - cs ) - v.y * sn ) * x + ( v.y * v.z * ( 1.0 - cs ) + v.x * sn ) * y + ( v.z * v.z * ( 1.0 - cs ) + cs ) * z );
        return ( vector3( xx, yy, zz ) );
    }
};



template < class T1, class T2 >
inline typename promote_trait< T1, T2 >::value_type operator ^( const vector3< T1 > &v1, const vector3< T2 > &v2 )
{
    typedef typename promote_trait< T1, T2 >::value_type value_type;
    return( vector3< value_type >( v1 ).inner( v2 ) );
}



DEFINE_PROMOTE_BIND_OPERATOR1( vector3, + )


DEFINE_PROMOTE_BIND_OPERATOR1( vector3, - )


DEFINE_PROMOTE_BIND_OPERATOR1( vector3, * )


DEFINE_PROMOTE_BIND_OPERATOR2( vector3, * )


DEFINE_PROMOTE_BIND_OPERATOR3( vector3, * )


DEFINE_PROMOTE_BIND_OPERATOR2( vector3, / )



template < class T > inline std::ostream &operator <<( std::ostream &out, const vector3< T > &v )
{
    out << v.x << ", ";
    out << v.y << ", ";
    out << v.z;
    return( out );
}



template < class T >
class vector2
{
public:
    typedef T value_type;                                       
    typedef size_t size_type;                                   
    typedef ptrdiff_t difference_type;                          
    typedef typename float_type< T >::value_type float_type;    

public:
    value_type x;       
    value_type y;       


    vector2( ) : x( 0 ), y( 0 ){ }


    vector2( const value_type &xx, const value_type &yy ) : x( xx ), y( yy ){ }


    vector2( const value_type &vv ) : x( vv ), y( vv ){ }



    template < class TT >
    vector2( const vector2< TT > &v ) : x( static_cast< value_type >( v.x ) ), y( static_cast< value_type >( v.y ) ){ }



    vector2( const vector2< T > &v ) : x( v.x ), y( v.y ){ }



    template < class TT >
    const vector2 &operator =( const vector2< TT > &v )
    {
        x = static_cast< value_type >( v.x );
        y = static_cast< value_type >( v.y );
        return ( *this );
    }

 
    const vector2 &operator =( const vector2< T > &v )
    {
        if( &v != this )
        {
            x = v.x;
            y = v.y;
        }
        return ( *this );
    }


    vector2 operator -( ) const { return ( vector2( -x, -y ) ); }



    template < class TT >
    vector2 &operator +=( const vector2< TT > &v ){ x = static_cast< value_type >( x + v.x ); y = static_cast< value_type >( y + v.y ); return( *this ); }


    template < class TT >
    vector2 &operator -=( const vector2< TT > &v ){ x = static_cast< value_type >( x - v.x ); y = static_cast< value_type >( y - v.y ); return( *this ); }



#if defined( __MIST_MSVC__ ) && __MIST_MSVC__ < 7
    vector2 &operator *=( const double &a )
#else
    template < class TT >
    vector2 &operator *=( const TT &a )
#endif
    {
        x = static_cast< value_type >( x * a );
        y = static_cast< value_type >( y * a );
        return( *this );
    }


#if defined( __MIST_MSVC__ ) && __MIST_MSVC__ < 7
    vector2 &operator /=( const double &a )
#else
    template < class TT >
    vector2 &operator /=( const TT &a )
#endif
    {
        x = static_cast< value_type >( x / a );
        y = static_cast< value_type >( y / a );
        return( *this );
    }

 
    bool operator ==( const vector2 &v ) const { return( x == v.x && y == v.y ); }


    bool operator !=( const vector2 &v ) const { return( !( *this == v ) ); }


    bool operator < ( const vector2 &v ) const
    {
        if( x == v.x )
        {
            return( y < v.y );
        }
        else
        {
            return( x < v.x );
        }
    }


    bool operator <=( const vector2 &v ) const { return( v >= *this ); }


    bool operator > ( const vector2 &v ) const { return( v < *this ); }


    bool operator >=( const vector2 &v ) const { return( !( *this < v ) ); }


    vector2 unit( ) const
    {
        float_type length_ = length( );
        if( length_ > 0 )
        {
            return( vector2( static_cast< value_type >( x / length_ ), static_cast< value_type >( y / length_ ) ) );
        }
        else
        {
            return( *this );
        }
    }


    template < class TT >
    typename promote_trait< T, TT >::value_type inner( const vector2< TT > &v ) const { return( static_cast< typename promote_trait< T, TT >::value_type >( x * v.x + y * v.y ) ); }


    template < class TT >
    typename promote_trait< T, TT >::value_type outer( const vector2< TT > &v ) const { return( static_cast< typename promote_trait< T, TT >::value_type >( x * v.y - y * v.x ) ); }


    float_type length( ) const { return ( static_cast< float_type >( std::sqrt( static_cast< double >( x * x + y * y ) ) ) ); }


    //vector2 rotate( const vector2 &a, double theta ) const
    //{
    //  theta *= 3.1415926535897932384626433832795 / 180.0;
    //  double cs = std::cos( theta ), sn = std::sin( theta );
    //  value_type xx = static_cast< value_type >( ( v.x * v.x * ( 1.0 - cs ) + cs ) * x + ( v.x * v.y * ( 1.0 - cs ) - v.z * sn ) * y + ( v.x * v.z * ( 1.0 - cs ) + v.y * sn ) * z );
    //  value_type yy = static_cast< value_type >( ( v.x * v.y * ( 1.0 - cs ) + v.z * sn ) * x + ( v.y * v.y * ( 1.0 - cs ) + cs ) * y + ( v.y * v.z * ( 1.0 - cs ) - v.x * sn ) * z );
    //  value_type zz = static_cast< value_type >( ( v.x * v.z * ( 1.0 - cs ) - v.y * sn ) * x + ( v.y * v.z * ( 1.0 - cs ) + v.x * sn ) * y + ( v.z * v.z * ( 1.0 - cs ) + cs ) * z );
    //  return ( vector2( xx, yy, zz ) );
    //}

};


template < class T1, class T2 >
inline typename promote_trait< T1, T2 >::value_type operator ^( const vector2< T1 > &v1, const vector2< T2 > &v2 )
{
    typedef typename promote_trait< T1, T2 >::value_type value_type;
    return( vector2< value_type >( v1 ).inner( v2 ) );
}

template < class T1, class T2 >
inline typename promote_trait< T1, T2 >::value_type operator *( const vector2< T1 > &v1, const vector2< T2 > &v2 )
{
    typedef typename promote_trait< T1, T2 >::value_type value_type;
    return( vector2< value_type >( v1 ).outer( v2 ) );
}


DEFINE_PROMOTE_BIND_OPERATOR1( vector2, + )


DEFINE_PROMOTE_BIND_OPERATOR1( vector2, - )


DEFINE_PROMOTE_BIND_OPERATOR2( vector2, * )


DEFINE_PROMOTE_BIND_OPERATOR3( vector2, * )


DEFINE_PROMOTE_BIND_OPERATOR2( vector2, / )



template < class T > inline std::ostream &operator <<( std::ostream &out, const vector2< T > &v )
{
    out << v.x << ", ";
    out << v.y;
    return( out );
}



#if defined( _MIST_VECTOR_SUPPORT_ ) && _MIST_VECTOR_SUPPORT_ != 0


template < class T, class Allocator = ::std::allocator< T > >
class vector : public matrix< T, Allocator >
{
private:
    typedef matrix< T, Allocator > base;        

public:
    typedef typename base::allocator_type allocator_type;       
    typedef typename base::reference reference;                 
    typedef typename base::const_reference const_reference;     
    typedef typename base::value_type value_type;               
    typedef typename base::size_type size_type;                 
    typedef typename base::difference_type difference_type;     
    typedef typename base::pointer pointer;                     
    typedef typename base::const_pointer const_pointer;         

    typedef typename base::iterator iterator;                               
    typedef typename base::const_iterator const_iterator;                   
    typedef typename base::reverse_iterator reverse_iterator;               
    typedef typename base::const_reverse_iterator const_reverse_iterator;   


    template < class TT, class AAllocator = std::allocator< TT > > 
    struct rebind
    {
        typedef vector< TT, AAllocator > other;
    };


    //template < class TT, class AAlocator >
    //const vector& operator +=( const vector< TT, AAlocator > &v2 )
    //{
    //  base::operator +=( v2 );
    //  return( v1 );
    //}

    //template < class TT, class AAlocator >
    //const vector& operator -=( const vector< TT, AAlocator > &v2 )
    //{
    //  base::operator -=( v2 );
    //  return( v1 );
    //}



    template < class TT, class AAlocator >
    const vector& operator *=( const vector< TT, AAlocator > &v2 )
    {
#if defined( _CHECK_MATRIX_OPERATION_ ) && _CHECK_MATRIX_OPERATION_ != 0
        if( base::size( ) != v2.size( ) || base::size( ) < 3 )
        {

            ::std::cerr << "can't calculate outer product of two vectors." << ::std::endl;
            return( *this );
        }
#endif

        typedef typename vector< T, Allocator >::size_type size_type;
        vector &v1 = *this;
        vector v( v1.size( ) );

        size_type i;
        for( i = 0 ; i < v.size( ) - 2 ; i++ )
        {
            v[ i ] = v1[ i + 1 ] * v2[ i + 2 ] - v1[ i + 2 ] * v2[ i + 1 ];
        }

        v[ i ] = v1[ i + 1 ] * v2[ 0 ] - v1[ 0 ] * v2[ i + 1 ];
        v[ i + 1 ] = v1[ 0 ] * v2[ 1 ] - v1[ 1 ] * v2[ 0 ];

        v1.swap( v );

        return( *this );
    }



    value_type operator ^( const vector &v ) const { return( inner( v ) ); }



    value_type inner( const vector &v2 ) const
    {
#if defined( _CHECK_MATRIX_OPERATION_ ) && _CHECK_MATRIX_OPERATION_ != 0
        if( base::size( ) != v2.size( ) )
        {

            ::std::cerr << "can't calculate inner product of two vectors." << ::std::endl;
            return( value_type( 0 ) );
        }
#endif

        typedef typename vector< T, Allocator >::size_type size_type;
        const vector &v1 = *this;
        value_type v = value_type( 0 );
        for( size_type i = 0 ; i < base::size( ) ; i++ )
        {
            v += v1[ i ] * v2[ i ];
        }

        return( v );
    }



    vector outer( const vector &v ) const
    {
        return( vector( *this ) *= v );
    }


public:

    vector( ) : base( ) {}


    explicit vector( const Allocator &a ) : base( a ) {}



    vector( size_type num ) : base( num, 1 ) {}


    vector( size_type num, const Allocator &a ) : base( num, 1, a ) {}



    vector( size_type num, const T &val ) : base( num, 1, val ) {}


    vector( size_type num, const T &val, const Allocator &a ) : base( num, 1, val, a ) {}



    vector( size_type num, pointer ptr, size_type mem_available ) : base( num, 1, ptr, mem_available ) {}

    
    vector( size_type num, const T &val, pointer ptr, size_type mem_available ) : base( num, 1, val, ptr, mem_available ) {}



    template < class TT, class AAlocator >
    vector( const vector< TT, AAlocator > &o ) : base( o ){ }



    vector( const vector< T, Allocator > &o ) : base( o ){ }

#if _USE_EXPRESSION_TEMPLATE_ != 0

    template < class Expression >
    vector( const matrix_expression< Expression > &expression ) : base( expression.size( ), 1 )
    {
        vector &v = *this;
        for( size_type indx = 0 ; indx < v.size( ) ; indx++ )
        {
            v[ indx ] = expression[ indx ];
        }
    }
#endif
};



template < class T, class Allocator >
inline ::std::ostream &operator <<( ::std::ostream &out, const vector< T, Allocator > &v )
{
    typename vector< T, Allocator >::size_type indx;
    for( indx = 0 ; indx < v.size( ) ; indx++ )
    {
        out << v[ indx ];
        if( indx != v.size( ) - 1 ) out << ", ";
    }

    return( out );
}

#if 0
#else


template < class T, class Allocator >
inline vector< T, Allocator > operator *( const vector< T, Allocator > &v1, const vector< T, Allocator > &v2 )
{
    return( vector< T, Allocator >( v1 ) *= v2 );
}

#endif



#endif // _MIST_VECTOR_SUPPORT_


_MIST_END

#endif // __INCLUDE_MIST_VECTOR__
