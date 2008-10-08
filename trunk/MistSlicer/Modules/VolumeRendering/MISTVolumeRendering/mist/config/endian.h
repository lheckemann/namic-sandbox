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


#ifndef __INCLUDE_MIST_ENDIAN__
#define __INCLUDE_MIST_ENDIAN__

#ifndef __INCLUDE_MIST_CONF_H__
#include "mist_conf.h"
#endif


_MIST_BEGIN



template < class T >
union byte_array
{
public:
    typedef T      value_type;      
    typedef size_t size_type;       

private:
    value_type value;                               
    unsigned char byte[ sizeof( value_type ) ];     

public:

    byte_array( ) : value( 0 ){ }



    byte_array( const value_type v ) : value( v ){ }



    byte_array( const byte_array &v ) : value( v.value ){ }



    byte_array( const unsigned char *b )
    {
        for( size_type i = 0 ; i < sizeof( value_type ) ; i++ )
        {
            byte[ i ] = b[ i ];
        }
    }


    size_type length( ) const { return( sizeof( value_type ) ); }


    unsigned char &operator[]( size_type index ){ return( byte[ index ] ); }


    const unsigned char &operator[]( size_type index ) const { return( byte[ index ] ); }


    const value_type get_value( ) const { return( value ); }


    value_type set_value( const value_type &v ) { return( value = v ); }


    const unsigned char * get_bytes( ) const { return( byte ); }
};



inline bool _is_little_endian_( )
{
    return( byte_array< unsigned short >( 1 )[ 0 ] == 1 );
}



inline bool _is_big_endian_( )
{
    return( byte_array< unsigned short >( 1 )[ 0 ] == 0 );
}


template < class T >
inline void swap_bytes( byte_array< T > &bytes )
{
    typedef typename byte_array< T >::size_type size_type;
    byte_array< T > tmp( bytes );
    for( size_type i = 0 ; i < sizeof( T ) ; ++i )
    {
        bytes[ i ] = tmp[ sizeof( T ) - i - 1 ];
    }
}


template < class T >
inline byte_array< T > to_current_endian( const byte_array< T > &bytes, bool from_little_endian )
{
    static bool current_endian = _is_little_endian_( );
    if( current_endian != from_little_endian )
    {
        byte_array< T > tmp( bytes );
        swap_bytes( tmp );
        return( tmp );
    }
    else
    {
        return( bytes );
    }
}



template < class T >
inline byte_array< T > from_current_endian( const byte_array< T > &bytes, bool to_little_endian )
{
    static bool current_endian = _is_little_endian_( );
    if( current_endian != to_little_endian )
    {
        byte_array< T > tmp( bytes );
        swap_bytes( tmp );
        return( tmp );
    }
    else
    {
        return( bytes );
    }
}




_MIST_END


#endif // __INCLUDE_MIST_ENDIAN__
