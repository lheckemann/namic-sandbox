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



#ifndef __INCLUDE_MIST_POINTER_H__
#define __INCLUDE_MIST_POINTER_H__


#ifndef __INCLUDE_MIST_CONF_H__
#include "mist_conf.h"
#endif

#include <iostream>
#include <map>



_MIST_BEGIN




namespace __shared_memory__
{
    template < class T >
    struct shared_base
    {
    private:
        typedef T* pointer;

        struct shared_memory_conter
        {
            typedef T* pointer;
            pointer ptr;
            size_t ref_count;
            size_t ref_weak_count;

            shared_memory_conter( ) : ptr( NULL ), ref_count( 0 ), ref_weak_count( 0 ){ }
            shared_memory_conter( pointer p, size_t rc, size_t rwc ) : ptr( p ), ref_count( rc ), ref_weak_count( rwc ){ }
        };

        typedef shared_memory_conter counter_type;
        typedef std::map< pointer, counter_type > ref_table_type;
        typedef typename ref_table_type::iterator ref_table_iterator;

    protected:
        static ref_table_type &get_ref_table( )
        {
            static ref_table_type ref_table_singleton_;
            return( ref_table_singleton_ );
        }

        static ref_table_iterator get_ref_iterator( pointer p )
        {
            ref_table_type &ref_table_ = get_ref_table( );

            if( p == NULL )
            {
                return( get_null_reference( ) ); 
            }
            else
            {
                ref_table_iterator ite = ref_table_.find( p );

                if( ite == ref_table_.end( ) )
                {
    
                    ite = ref_table_.insert( typename ref_table_type::value_type( p, counter_type( p, 0, 0 ) ) ).first;
                }
                return( ite ); 
            }
        }

        static ref_table_iterator get_null_reference( )
        {
            ref_table_type &ref_table_ = get_ref_table( );
    
            static ref_table_iterator null_ite_ = ref_table_.insert( typename ref_table_type::value_type( NULL, counter_type( NULL, 0, 0 ) ) ).first;
            return( null_ite_ ); 
        }

        ref_table_iterator ref_ite_;

        shared_base( ) : ref_ite_( get_null_reference( ) ){ }
        shared_base( const shared_base &b ) : ref_ite_( b.ref_ite_ ){}

        pointer get_pointer( ) const { return( ref_ite_->second.ptr ); }

    protected:
    
        void null_ref( )
        {
            ref_ite_ = get_null_reference( );
        }

    
        void add_ref( )
        {
    
            ref_ite_->second.ref_count++;
        }

    
        void add_ref( pointer p )
        {
            ref_ite_ = get_ref_iterator( p );

    
            ref_ite_->second.ref_count++;
        }

    
        void add_ref( const shared_base &p )
        {
            ref_ite_ = p.ref_ite_;

    
            ref_ite_->second.ref_count++;
        }

    
        void add_weak_ref( )
        {
    
            ref_ite_->second.ref_weak_count++;
        }

    
        void add_weak_ref( pointer p )
        {
            ref_ite_ = get_ref_iterator( p );

    
            ref_ite_->second.ref_weak_count++;
        }

    
        void add_weak_ref( const shared_base &p )
        {
            ref_ite_ = p.ref_ite_;

    
            ref_ite_->second.ref_weak_count++;
        }

        void release( bool isArray )
        {
            counter_type &c = ref_ite_->second;
            if( c.ptr != NULL )
            {
    
                c.ref_count--;

    
                if( c.ref_count == 0 )
                {
    
                    if( isArray )
                    {
                        delete [] c.ptr;
                    }
                    else
                    {
                        delete c.ptr;
                    }

    
                    if( c.ref_weak_count == 0 )
                    {
                        ref_table_type &table = get_ref_table( );
                        table.erase( ref_ite_ );
                    }
                    else
                    {
                        c.ptr = NULL;
                    }
                }
            }
        }

        void release_weak( )
        {
            counter_type &c = ref_ite_->second;

            if( c.ptr != NULL )
            {
    
                c.ref_weak_count--;

                if( c.ref_weak_count == 0 && c.ref_count == 0 )
                {
    
                    ref_table_type &table = get_ref_table( );
                    table.erase( ref_ite_ );
                }
            }
        }
    };
}





template < class T >
class scoped_ptr
{
public:
    typedef size_t size_type;           
    typedef ptrdiff_t difference_type;  
    typedef T value_type;               
    typedef T* pointer;                 
    typedef T& reference;               
    typedef const T& const_reference;   
    typedef const T* const_pointer;     

private:
    pointer ptr_;       

public:

    scoped_ptr( pointer p ) : ptr_( p ){ }


    ~scoped_ptr( ){ delete ptr_; }


public:
    reference operator *( ){ return( *ptr_ ); }
    const_reference operator *( ) const { return( *ptr_ ); }

    pointer operator ->( ){ return( ptr_ ); }
    const_pointer operator ->( ) const { return( ptr_ ); }


private:
    scoped_ptr( const scoped_ptr &p );
    const scoped_ptr &operator  =( const scoped_ptr &p );
};



template < class T >
class scoped_array
{
public:
    typedef size_t size_type;           
    typedef ptrdiff_t difference_type;  
    typedef T value_type;               
    typedef T* pointer;                 
    typedef T& reference;               
    typedef const T& const_reference;   
    typedef const T* const_pointer;     

private:
    pointer ptr_;       

public:

    scoped_array( pointer p ) : ptr_( p ){ }

    ~scoped_array( ){ delete [] ptr_; }


public:
    reference operator *( ){ return( *ptr_ ); }
    const_reference operator *( ) const { return( *ptr_ ); }

    pointer operator ->( ){ return( ptr_ ); }
    const_pointer operator ->( ) const { return( ptr_ ); }

    reference operator []( difference_type index ){ return( ptr_[ index ] ); }
    const_reference operator []( difference_type index ) const { return( ptr_[ index ] ); }

private:
    scoped_array( const scoped_array &p );
    const scoped_array &operator  =( const scoped_array &p );
};



template < class T >
class shared_ptr : public __shared_memory__::shared_base< T >
{
public:
    typedef size_t size_type;           
    typedef ptrdiff_t difference_type;  
    typedef T value_type;               
    typedef T* pointer;                 
    typedef T& reference;               
    typedef const T& const_reference;   
    typedef const T* const_pointer;     

private:
    typedef __shared_memory__::shared_base< T > base;


public:

    shared_ptr( ){ }


    shared_ptr( pointer p ){ base::add_ref( p ); }


    shared_ptr( const shared_ptr &p ) : base( p ){ base::add_ref( ); }
 
    ~shared_ptr( ){ base::release( false ); }



    const shared_ptr &operator =( const shared_ptr &p )
    {
        if( &p != this )
        {
            base::release( false );
            base::add_ref( p );
        }

        return( *this );
    }

public:

    reference operator *( ){ return( *base::get_pointer( ) ); }


    const_reference operator *( ) const { return( *base::get_pointer( ) ); }


    pointer operator ->( ){ return( base::get_pointer( ) ); }


    const_pointer operator ->( ) const { return( base::get_pointer( ) ); }




    void reset( )
    { 
        base::release( false );
        base::null_ref( );
    }

public:

    bool operator ==( const shared_ptr &p ) const { return( base::get_pointer( ) == p.get_pointer( ) ); }


    bool operator !=( const shared_ptr &p ) const { return( base::get_pointer( ) != p.get_pointer( ) ); }


    bool operator < ( const shared_ptr &p ) const { return( base::get_pointer( ) <  p.get_pointer( ) ); }


    bool operator <=( const shared_ptr &p ) const { return( base::get_pointer( ) <= p.get_pointer( ) ); }


    bool operator > ( const shared_ptr &p ) const { return( base::get_pointer( ) >  p.get_pointer( ) ); }


    bool operator >=( const shared_ptr &p ) const { return( base::get_pointer( ) >= p.get_pointer( ) ); }



    bool operator ==( const pointer &p ) const { return( base::get_pointer( ) == p ); }


    bool operator !=( const pointer &p ) const { return( base::get_pointer( ) != p ); }


    bool operator < ( const pointer &p ) const { return( base::get_pointer( ) <  p ); }


    bool operator <=( const pointer &p ) const { return( base::get_pointer( ) <= p ); }


    bool operator > ( const pointer &p ) const { return( base::get_pointer( ) >  p ); }


    bool operator >=( const pointer &p ) const { return( base::get_pointer( ) >= p ); }

};

template < class T > inline bool operator ==( const typename shared_ptr< T >::pointer p1, const shared_ptr< T > &p2 ){ return( p2 == p1 ); }
template < class T > inline bool operator !=( const typename shared_ptr< T >::pointer p1, const shared_ptr< T > &p2 ){ return( p2 != p1 ); }

template < class T > inline bool operator < ( const typename shared_ptr< T >::pointer p1, const shared_ptr< T > &p2 ){ return( !( p2 <= p1 ) ); }
template < class T > inline bool operator <=( const typename shared_ptr< T >::pointer p1, const shared_ptr< T > &p2 ){ return( !( p2 <  p1 ) ); }
template < class T > inline bool operator > ( const typename shared_ptr< T >::pointer p1, const shared_ptr< T > &p2 ){ return( !( p2 >= p1 ) ); }
template < class T > inline bool operator >=( const typename shared_ptr< T >::pointer p1, const shared_ptr< T > &p2 ){ return( !( p2 >  p1 ) ); }




template < class T >
class shared_array : public __shared_memory__::shared_base< T >
{
public:
    typedef size_t size_type;           
    typedef ptrdiff_t difference_type;  
    typedef T value_type;               
    typedef T* pointer;                 
    typedef T& reference;               
    typedef const T& const_reference;   
    typedef const T* const_pointer;     

private:
    typedef __shared_memory__::shared_base< T > base;

public:

    shared_array( ){ }


    shared_array( pointer p ){ base::add_ref( p ); }


    shared_array( const shared_array &p ){ base::add_ref( p ); }

    ~shared_array( ){ base::release( true ); }



    const shared_array &operator =( const shared_array &p )
    {
        if( &p != this )
        {
            base::release( true );
            base::add_ref( p );
        }

        return( *this );
    }

public:

    reference operator *( ){ return( *base::get_pointer( ) ); }


    const_reference operator *( ) const { return( *base::get_pointer( ) ); }


    pointer operator ->( ){ return( base::get_pointer( ) ); }


    const_pointer operator ->( ) const { return( base::get_pointer( ) ); }


    reference operator []( difference_type index ){ return( base::get_pointer( )[ index ] ); }


    const_reference operator []( difference_type index ) const { return( base::get_pointer( )[ index ] ); }


    
    void reset( )
    { 
        base::release( true );
        base::null_ref( );
    }


public:

    bool operator ==( const shared_array &p ) const { return( base::get_pointer( ) == p.get_pointer( ) ); }


    bool operator !=( const shared_array &p ) const { return( base::get_pointer( ) != p.get_pointer( ) ); }


    bool operator < ( const shared_array &p ) const { return( base::get_pointer( ) <  p.get_pointer( ) ); }


    bool operator <=( const shared_array &p ) const { return( base::get_pointer( ) <= p.get_pointer( ) ); }


    bool operator > ( const shared_array &p ) const { return( base::get_pointer( ) >  p.get_pointer( ) ); }


    bool operator >=( const shared_array &p ) const { return( base::get_pointer( ) >= p.get_pointer( ) ); }



    bool operator ==( const pointer &p ) const { return( base::get_pointer( ) == p ); }


    bool operator !=( const pointer &p ) const { return( base::get_pointer( ) != p ); }


    bool operator < ( const pointer &p ) const { return( base::get_pointer( ) <  p ); }


    bool operator <=( const pointer &p ) const { return( base::get_pointer( ) <= p ); }


    bool operator > ( const pointer &p ) const { return( base::get_pointer( ) >  p ); }


    bool operator >=( const pointer &p ) const { return( base::get_pointer( ) >= p ); }

};

template < class T > inline bool operator ==( const typename shared_array< T >::pointer p1, const shared_array< T > &p2 ){ return( p2 == p1 ); }
template < class T > inline bool operator !=( const typename shared_array< T >::pointer p1, const shared_array< T > &p2 ){ return( p2 != p1 ); }

template < class T > inline bool operator < ( const typename shared_array< T >::pointer p1, const shared_array< T > &p2 ){ return( !( p2 <= p1 ) ); }
template < class T > inline bool operator <=( const typename shared_array< T >::pointer p1, const shared_array< T > &p2 ){ return( !( p2 <  p1 ) ); }
template < class T > inline bool operator > ( const typename shared_array< T >::pointer p1, const shared_array< T > &p2 ){ return( !( p2 >= p1 ) ); }
template < class T > inline bool operator >=( const typename shared_array< T >::pointer p1, const shared_array< T > &p2 ){ return( !( p2 >  p1 ) ); }



template < class T >
class weak_ptr : public __shared_memory__::shared_base< T >
{
public:
    typedef size_t size_type;           
    typedef ptrdiff_t difference_type;  
    typedef T value_type;               
    typedef T* pointer;                 
    typedef T& reference;               
    typedef const T& const_reference;   
    typedef const T* const_pointer;     

private:
    typedef __shared_memory__::shared_base< T > base;

public:

    weak_ptr( ){ base::add_weak_ref( NULL ); }


    weak_ptr( shared_ptr< T > &p ){ base::add_weak_ref( p ); }


    weak_ptr( const shared_ptr< T > &p ){ base::add_weak_ref( p ); }


    weak_ptr( shared_array< T > &p ){ base::add_weak_ref( p ); }


    weak_ptr( const shared_array< T > &p ){ base::add_weak_ref( p ); }


    ~weak_ptr( ){ base::release_weak( ); }



    const weak_ptr &operator =( const weak_ptr &p )
    {
        if( this != &p )
        {
            base::release_weak( );
            base::add_weak_ref( p );
        }

        return( *this );
    }


    const weak_ptr &operator =( const shared_ptr< T > &p )
    {
        base::release_weak( );
        base::add_weak_ref( p );

        return( *this );
    }


    const weak_ptr &operator =( const shared_array< T > &p )
    {
        base::release_weak( );
        base::add_weak_ref( p );

        return( *this );
    }


public:

    reference operator *( ){ return( *base::get_pointer( ) ); }


    const_reference operator *( ) const { return( *base::get_pointer( ) ); }


    pointer operator ->( ){ return( base::get_pointer( ) ); }


    const_pointer operator ->( ) const { return( base::get_pointer( ) ); }


    reference operator []( difference_type index ){ return( base::get_pointer( )[ index ] ); }


    const_reference operator []( difference_type index ) const { return( base::get_pointer( )[ index ] ); }


    void reset( )
    { 
        base::release_weak( );
        base::null_ref( );
    }


public:

    bool operator ==( const weak_ptr &p ) const { return( base::get_pointer( ) == p.get_pointer( ) ); }


    bool operator !=( const weak_ptr &p ) const { return( base::get_pointer( ) != p.get_pointer( ) ); }


    bool operator < ( const weak_ptr &p ) const { return( base::get_pointer( ) <  p.get_pointer( ) ); }


    bool operator <=( const weak_ptr &p ) const { return( base::get_pointer( ) <= p.get_pointer( ) ); }


    bool operator > ( const weak_ptr &p ) const { return( base::get_pointer( ) >  p.get_pointer( ) ); }


    bool operator >=( const weak_ptr &p ) const { return( base::get_pointer( ) >= p.get_pointer( ) ); }



    bool operator ==( const pointer &p ) const { return( base::get_pointer( ) == p ); }


    bool operator !=( const pointer &p ) const { return( base::get_pointer( ) != p ); }


    bool operator < ( const pointer &p ) const { return( base::get_pointer( ) <  p ); }


    bool operator <=( const pointer &p ) const { return( base::get_pointer( ) <= p ); }


    bool operator > ( const pointer &p ) const { return( base::get_pointer( ) >  p ); }


    bool operator >=( const pointer &p ) const { return( base::get_pointer( ) >= p ); }

};


template < class T > inline bool operator ==( const typename weak_ptr< T >::pointer p1, const weak_ptr< T > &p2 ){ return( p2 == p1 ); }
template < class T > inline bool operator !=( const typename weak_ptr< T >::pointer p1, const weak_ptr< T > &p2 ){ return( p2 != p1 ); }

template < class T > inline bool operator < ( const typename weak_ptr< T >::pointer p1, const weak_ptr< T > &p2 ){ return( !( p2 <= p1 ) ); }
template < class T > inline bool operator <=( const typename weak_ptr< T >::pointer p1, const weak_ptr< T > &p2 ){ return( !( p2 <  p1 ) ); }
template < class T > inline bool operator > ( const typename weak_ptr< T >::pointer p1, const weak_ptr< T > &p2 ){ return( !( p2 >= p1 ) ); }
template < class T > inline bool operator >=( const typename weak_ptr< T >::pointer p1, const
                                             
                                             weak_ptr< T > &p2 ){ return( !( p2 >  p1 ) ); }



template < class T >
inline std::ostream &operator <<( std::ostream &out, const scoped_ptr< T > &p )
{
    out << &( *p );
    return( out );
}



template < class T >
inline std::ostream &operator <<( std::ostream &out, const scoped_array< T > &p )
{
    out << &( *p );
    return( out );
}



template < class T >
inline std::ostream &operator <<( std::ostream &out, const shared_ptr< T > &p )
{
    out << &( *p );
    return( out );
}

 
template < class T >
inline std::ostream &operator <<( std::ostream &out, const weak_ptr< T > &p )
{
    out << &( *p );
    return( out );
}


_MIST_END


#endif // __INCLUDE_MIST_POINTER_H__
