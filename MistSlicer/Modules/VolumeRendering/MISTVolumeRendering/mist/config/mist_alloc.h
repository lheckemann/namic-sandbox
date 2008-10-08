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



#ifndef __INCLUDE_MIST_ALLOC_H__
#define __INCLUDE_MIST_ALLOC_H__

#include <memory>

#ifndef __INCLUDE_MIST_CONF_H__
#include "mist_conf.h"
#endif

#ifndef __INCLUDE_MIST_TYPE_TRAIT_H__
#include "type_trait.h"
#endif



_MIST_BEGIN




template < bool b >
struct mist_memory_operator
{

    template < class Allocator >
    static typename Allocator::pointer allocate_objects1( Allocator &allocator, typename Allocator::size_type num )
    {
#if _MIST_ALLOCATE_TEST_ != 0
        ::std::cerr << "Normal Allocator" << ::std::endl;
#endif
        typename Allocator::pointer ptr = allocator.allocate( num, 0 );
        typename Allocator::value_type obj;
        for( typename Allocator::size_type i = 0 ; i < num ; i++ ) allocator.construct( &( ptr[i] ), obj );
        return( ptr );
    }



    template < class Allocator >
    static typename Allocator::pointer allocate_objects2( Allocator &allocator, typename Allocator::size_type num, typename Allocator::const_reference obj )
    {
#if _MIST_ALLOCATE_TEST_ != 0
        ::std::cerr << "Normal Allocator" << ::std::endl;
#endif
        typename Allocator::pointer ptr = allocator.allocate( num, 0 );
        for( typename Allocator::size_type i = 0 ; i < num ; i++ ) allocator.construct( &( ptr[i] ), obj );
        return( ptr );
    }



    template < class Allocator >
    static typename Allocator::pointer allocate_objects3( Allocator &allocator, typename Allocator::const_pointer s, typename Allocator::const_pointer e )
    {
#if _MIST_ALLOCATE_TEST_ != 0
        ::std::cerr << "Normal Allocator" << ::std::endl;
#endif
        typename Allocator::pointer ptr = allocator.allocate( e - s, 0 );
        for( typename Allocator::pointer p = ptr ; s != e ; p++, s++ ) allocator.construct( p, *s );
        return( ptr );
    }


    template < class Allocator >
    static void deallocate_objects( Allocator &allocator, typename Allocator::pointer ptr, typename Allocator::size_type num )
    {
#if _MIST_ALLOCATE_TEST_ != 0
        ::std::cerr << "Normal Deallocator" << ::std::endl;
#endif
        for( typename Allocator::size_type i = 0 ; i < num ; i++ ) allocator.destroy( &( ptr[i] ) );
        allocator.deallocate( ptr, num );
    }



    template < class Allocator >
    static typename Allocator::pointer copy_objects1( Allocator &allocator, typename Allocator::const_pointer s, typename Allocator::const_pointer e, typename Allocator::pointer x )
    {
#if _MIST_ALLOCATE_TEST_ != 0
        ::std::cerr << "Normal Copy Function" << ::std::endl;
#endif
        while( s != e )
        {
            *x = *s;
            ++x;
            ++s;
        }
        return( x );
    }


    template < class Allocator >
    static typename Allocator::pointer copy_objects2( Allocator &allocator, typename Allocator::const_pointer ptr, typename Allocator::size_type num, typename Allocator::pointer to )
    {
#if _MIST_ALLOCATE_TEST_ != 0
        ::std::cerr << "Normal Copy Function" << ::std::endl;
#endif
        for( typename Allocator::size_type i = 0 ; i < num ; i++ ) to[i] = ptr[i];
        return( to + num );
    }


    template < class Allocator >
    static void fill_objects1( Allocator &allocator, typename Allocator::pointer ptr, typename Allocator::size_type num, typename Allocator::const_reference obj )
    {
#if _MIST_ALLOCATE_TEST_ != 0
        ::std::cerr << "Normal Clean Function" << ::std::endl;
#endif
        for( typename Allocator::size_type i = 0 ; i < num ; i++ ) ptr[i] = obj;
    }



    template < class Allocator >
    static void fill_objects2( Allocator &allocator, typename Allocator::pointer ptr, typename Allocator::size_type num )
    {
#if _MIST_ALLOCATE_TEST_ != 0
        ::std::cerr << "Normal Clean Function" << ::std::endl;
#endif
        typename Allocator::value_type obj;
        for( typename Allocator::size_type i = 0 ; i < num ; i++ ) ptr[i] = obj;
    }
};





template <>
struct mist_memory_operator< true >
{

    template < class Allocator >
    static typename Allocator::pointer allocate_objects1( Allocator &allocator, typename Allocator::size_type num )
    {
#if _MIST_ALLOCATE_TEST_ != 0
        ::std::cerr << "Memset Allocator" << ::std::endl;
#endif
        typename Allocator::pointer ptr = allocator.allocate( num, 0 );
        ::memset( ptr, 0, num * sizeof( typename Allocator::value_type ) );
        return( ptr );
    }



    template < class Allocator >
    static typename Allocator::pointer allocate_objects2( Allocator &allocator, typename Allocator::size_type num, typename Allocator::const_reference obj )
    {
#if _MIST_ALLOCATE_TEST_ != 0
        ::std::cerr << "Specialized Allocator" << ::std::endl;
#endif
        typename Allocator::pointer ptr = allocator.allocate( num, 0 );
        for( typename Allocator::size_type i = 0 ; i < num ; i++ ) ptr[i] = obj;
        return( ptr );
    }


    template < class Allocator >
    static typename Allocator::pointer allocate_objects3( Allocator &allocator, typename Allocator::const_pointer s, typename Allocator::const_pointer e )
    {
#if _MIST_ALLOCATE_TEST_ != 0
        ::std::cerr << "Memcpy Allocator" << ::std::endl;
#endif
        typename Allocator::pointer ptr = allocator.allocate( e - s, 0 );
        memcpy( ptr, s, ( e - s ) * sizeof( typename Allocator::value_type ) );
        return( ptr );
    }


    template < class Allocator >
    static void deallocate_objects( Allocator &allocator, typename Allocator::pointer ptr, typename Allocator::size_type num )
    {
#if _MIST_ALLOCATE_TEST_ != 0
        ::std::cerr << "Specialized Deallocator" << ::std::endl;
#endif
        allocator.deallocate( ptr, num );
    }



    template < class Allocator >
    static typename Allocator::pointer copy_objects1( Allocator &allocator, typename Allocator::const_pointer s, typename Allocator::const_pointer e, typename Allocator::pointer x )
    {
#if _MIST_ALLOCATE_TEST_ != 0
        ::std::cerr << "Memcpy Copy Function" << ::std::endl;
#endif
        memcpy( x, s, ( e - s ) * sizeof( typename Allocator::value_type ) );
        return( x + ( e - s ) );
    }



    template < class Allocator >
    static typename Allocator::pointer copy_objects2( Allocator &allocator, typename Allocator::const_pointer ptr, typename Allocator::size_type num, typename Allocator::pointer to )
    {
#if _MIST_ALLOCATE_TEST_ != 0
        ::std::cerr << "Memcpy Copy Function" << ::std::endl;
#endif
        memcpy( to, ptr, num * sizeof( typename Allocator::value_type ) );
        return( to + num );
    }


    template < class Allocator >
    static void fill_objects1( Allocator &allocator, typename Allocator::pointer ptr, typename Allocator::size_type num, typename Allocator::const_reference obj )
    {
#if _MIST_ALLOCATE_TEST_ != 0
        ::std::cerr << "Memcpy Clean Function" << ::std::endl;
#endif
        memset( ptr, obj, num * sizeof( typename Allocator::value_type ) );
    }



    template < class Allocator >
    static void fill_objects2( Allocator &allocator, typename Allocator::pointer ptr, typename Allocator::size_type num )
    {
#if _MIST_ALLOCATE_TEST_ != 0
        ::std::cerr << "Memcpy Clean Function" << ::std::endl;
#endif
        memset( ptr, 0, num * sizeof( typename Allocator::value_type ) );
    }
};


template < class T, class Allocator >
class mist_allocator : public Allocator
{
public:
    typedef Allocator base;                                         
    typedef typename Allocator::reference reference;                
    typedef typename Allocator::const_reference const_reference;    
    typedef typename Allocator::value_type value_type;              
    typedef typename Allocator::size_type size_type;                
    typedef typename Allocator::difference_type difference_type;    
    typedef typename Allocator::pointer pointer;                    
    typedef typename Allocator::const_pointer const_pointer;        

protected:
    pointer   shared_pointer;   
    size_type shared_memory;    


public:
    
    pointer allocate_objects( size_type num )
    {
        if( num == 0 || num > max_size( ) )
        {
            return( NULL );
        }
        else if( !is_memory_shared( ) )
        {

            return( mist_memory_operator< is_arithmetic< T >::value >::allocate_objects1( *this, num ) );
        }
        else
        {

            return( shared_pointer );
        }
    }


    pointer allocate_objects( size_type num, const_reference obj )
    {
        if( num == 0 || num > max_size( ) )
        {
            return( NULL );
        }
        else if( !is_memory_shared( ) )
        {

            return( mist_memory_operator< is_arithmetic< T >::value >::allocate_objects2( *this, num, obj ) );
        }
        else
        {

            mist_memory_operator< is_char< T >::value >::fill_objects1( *this, shared_pointer, num, obj );
            return( shared_pointer );
        }
    }


    pointer allocate_objects( const_pointer s, const_pointer e )
    {
        if( s >= e || e - s > max_size( ) )
        {
            return( NULL );
        }
        else if( !is_memory_shared( ) )
        {

            return( mist_memory_operator< is_arithmetic< T >::value >::allocate_objects3( *this, s, e ) );
        }
        else
        {

            mist_memory_operator< is_char< T >::value >::copy_objects2( *this, s, e - s, shared_pointer );
            return( shared_pointer );
        }
    }


    void deallocate_objects( pointer ptr, size_type num )
    {
        if( !is_memory_shared( ) )
        {

            if( num <= 0 ) return;
            mist_memory_operator< is_arithmetic< T >::value >::deallocate_objects( *this, ptr, num );
        }
    }


    pointer copy_objects( const_pointer s, const_pointer e, pointer x )
    {
        if( s >= e ) return( x );
        return( mist_memory_operator< is_arithmetic< T >::value >::copy_objects1( *this, s, e, x ) );
    }


    pointer copy_objects( const_pointer ptr, size_type num, pointer to )
    {
        if( num <= 0 ) return( to );
        return( mist_memory_operator< is_arithmetic< T >::value >::copy_objects2( *this, ptr, num, to ) );
    }


    void fill_objects( pointer ptr, size_type num, const_reference obj )
    {
        if( num <= 0 ) return;
        mist_memory_operator< is_char< T >::value >::fill_objects1( *this, ptr, num, obj );
    }


    void fill_objects( pointer ptr, size_type num )
    {
        if( num <= 0 ) return;
        mist_memory_operator< is_arithmetic< T >::value >::fill_objects2( *this, ptr, num );
    }

    pointer trim_objects( pointer ptr, size_type num, size_type dest_num )
    {
        if( num < dest_num ) return( ptr );         
        if( num < 0 ) return( NULL );               
        if( num == dest_num ) return( ptr );        
        if( num == 0 ) return( NULL );

        if( !is_memory_shared( ) )
        {

#if _MIST_ALLOCATOR_MEMORY_TRIM_ != 0
            deallocate_objects( ptr + dest_num, num - dest_num );
            fill_objects( ptr, dest_num );
#else
            deallocate_objects( ptr, num );
            ptr = allocate_objects( dest_num );
#endif
        }
        return( dest_num == 0 ? NULL : ptr );
    }


    pointer trim_objects( pointer ptr, size_type num, size_type dest_num, const_reference obj )
    {
        if( num < dest_num ) return( ptr );         
        if( num < 0 ) return( NULL );               
        if( num == dest_num ) return( ptr );        
        if( num == 0 ) return( NULL );

        if( !is_memory_shared( ) )
        {

#if _MIST_ALLOCATOR_MEMORY_TRIM_ != 0
            deallocate_objects( ptr + dest_num, num - dest_num );
            fill_objects( ptr, dest_num, obj );
#else
            deallocate_objects( ptr, num );
            ptr = allocate_objects( dest_num, obj );
#endif
        }
        return( dest_num == 0 ? NULL : ptr );
    }


    size_type max_size( ) const
    {
        return( shared_memory == 0 ? base::max_size( ) : shared_memory );
    }


    bool is_memory_shared( ) const
    {
        return( shared_memory != 0 );
    }


    const Allocator &operator=( const Allocator &alloc )
    {
        if( &alloc != this )
        {

            base::operator=( alloc );
        }
        return( *this );
    }


    const mist_allocator &operator=( const mist_allocator &alloc )
    {
        if( &alloc != this )
        {

            base::operator=( alloc );
        }
        return( *this );
    }



    mist_allocator( ) : base( ), shared_pointer( NULL ), shared_memory( 0 ){}




    mist_allocator( pointer ptr, size_type mem_shared ) : base( ),  shared_pointer( mem_shared == 0 ? NULL : ptr ), shared_memory( ptr == NULL ? 0 : mem_shared ){}



    mist_allocator( const Allocator &alloc ) : base( alloc ), shared_pointer( NULL ), shared_memory( 0 ){}
};




_MIST_END

#endif // __INCLUDE_MIST_ALLOC_H__
