/****************************************************************************************************************************
**       MIST ( Media Integration Standard Toolkit )
**
**        We defined following template classes.
**
**          array  : template class of one dimensionl array with STL support.
**          array1 : template class of one dimensionl array containing resolution.
**          array2 : template class of two dimensionl array containing pixel resolution.
**          array3 : template class of three dimensionl array containing voxel resolution.
**          matrix : template class of matrix with its operations, and impremented using expression template technique.
**          vector : template class of vector with its operations, and impremented using expression template technique.
**
**
**         We developed these programs since 2003/09/05.
**
**             $LastChangedDate:: 2008-03-12 16:24:25 #$
**             $LastChangedRevision: 1085 $
**             $LastChangedBy: ddeguchi $
**             $HeadURL: http://mist.suenaga.m.is.nagoya-u.ac.jp/svn/mist/trunk/mist/mist.h $
**
**
**              Copyright MIST Project Team.
**                    All Rights Reserved.
**
****************************************************************************************************************************/

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



#ifndef __INCLUDE_MIST_H__
#define __INCLUDE_MIST_H__


#ifndef __INCLUDE_MIST_CONF_H__
#include "config/mist_conf.h"
#endif


#ifndef __INCLUDE_MIST_ALLOC_H__
#include "config/mist_alloc.h"
#endif

#ifndef __INCLUDE_MIST_ITERATOR_H__
#include "config/iterator.h"
#endif



_MIST_BEGIN



template < class T, class Allocator = std::allocator< T > >
class array
{
public:
    typedef Allocator allocator_type;                               
    typedef typename Allocator::reference reference;                
    typedef typename Allocator::const_reference const_reference;    
    typedef typename Allocator::value_type value_type;              
    typedef typename Allocator::size_type size_type;                
    typedef typename Allocator::difference_type difference_type;    
    typedef typename Allocator::pointer pointer;                    
    typedef typename Allocator::const_pointer const_pointer;        


    typedef mist_iterator1< T, ptrdiff_t, pointer, reference > iterator;


    typedef mist_iterator1< T, ptrdiff_t, const_pointer, const_reference > const_iterator;


    typedef mist_reverse_iterator< mist_iterator1< T, ptrdiff_t, pointer, reference > > reverse_iterator;


    typedef mist_reverse_iterator< mist_iterator1< T, ptrdiff_t, const_pointer, const_reference > > const_reverse_iterator;


    template < class TT, class AAllocator = std::allocator< TT > > 
    struct rebind
    {
        typedef array< TT, AAllocator > other;
    };


private:
    mist_allocator< T, Allocator > allocator_;      
    size_type size_;                                

protected:
    T* data_;   

public:
    
    bool resize( size_type num, size_type dmy1 = 0, size_type dmy2 = 0 )
    {
        if( size_ < num )
        {
            allocator_.deallocate_objects( data_, size_ );
            data_ = allocator_.allocate_objects( num );
            size_ = data_ == NULL ? 0 : num;
        }
        else if( size_ > num )
        {
            data_ = allocator_.trim_objects( data_, size_, num );
            size_ = data_ == NULL ? 0 : num;
        }

        return( data_ != NULL );
    }


    bool trim( size_type index, difference_type num = -1 )
    {
        difference_type num_ = size( );
        if( num_ <= static_cast< difference_type >( index ) || num_ < static_cast< difference_type >( index + num ) )
        {
            return( false );
        }
        else if( num_ == num )
        {
            return( true );
        }

        if( num < 0 )
        {
            num = size( ) - index;
        }

        if( is_memory_shared( ) )
        {

            array o( *this );

            if( resize( num ) )
            {
                for( difference_type i = 0 ; i < num ; i++ )
                {
                    operator []( i ) = o( i + index );
                }
            }
            else
            {
                return( false );
            }
        }
        else
        {
            array o( num );
            for( difference_type i = 0 ; i < num ; i++ )
            {
                o[ i ] = operator []( i + index );
            }

            swap( o );
        }

        return( true );
    }



    bool swap( array &a )
    {
        if( is_memory_shared( ) || a.is_memory_shared( ) )
        {
            return( false );
        }
        else
        {
            size_type _dmy_size = size_;
            size_ = a.size_;
            a.size_ = _dmy_size;

            value_type *dmy_data_ = data_;
            data_ = a.data_;
            a.data_ = dmy_data_;

            return( true );
        }
    }

    void clear( )
    {
        allocator_.deallocate_objects( data_, size_ );
        size_ = 0;
        data_ = NULL;
    }


    void fill( )
    {
        allocator_.fill_objects( data_, size_ );
    }


    void fill( const value_type &val )
    {
        allocator_.fill_objects( data_, size_, val );
    }

 
    bool empty( ) const { return( size_ == 0 ); }


    size_type size( ) const { return( size_ ); }    
    size_type size1( ) const { return( size_ ); }   
    size_type size2( ) const { return( 1 ); }       
    size_type size3( ) const { return( 1 ); }       
    size_type width( ) const { return( size_ ); }   
    size_type height( ) const { return( 1 ); }      
    size_type depth( ) const { return( 1 ); }       

    double reso1( double r1 ){ return( 1.0 ); }     
    double reso1( ) const { return( 1.0 ); }        
    double reso2( double r2 ){ return( 1.0 ); }     
    double reso2( ) const { return( 1.0 ); }        
    double reso3( double r3 ){ return( 1.0 ); }     
    double reso3( ) const { return( 1.0 ); }        



    size_type byte( ) const { return( size_ * sizeof( value_type ) ); }


    bool is_memory_shared( ) const { return( allocator_.is_memory_shared( ) ); }



    iterator begin( ){ return( iterator( paccess( 0 ), 1 ) ); }


    const_iterator begin( ) const { return( const_iterator( paccess( 0 ), 1 ) ); }



    iterator end( ){ return( iterator( paccess( size( ) ), 1 ) ); }


    const_iterator end( ) const { return( const_iterator( paccess( size( ) ), 1 ) ); }



    reverse_iterator rbegin( ){ return( reverse_iterator( end( ) ) ); }


    const_reverse_iterator rbegin( ) const { return( const_reverse_iterator( end( ) ) ); }



    reverse_iterator rend( ){ return( reverse_iterator( begin( ) ) ); }


    const_reverse_iterator rend( ) const { return( const_reverse_iterator( begin( ) ) ); }



    size_type max_size( ) const { return( allocator_.max_size( ) ); }



private: 
    iterator erase( iterator i );
    iterator erase( iterator s, iterator e );
    iterator insert( iterator i, const value_type &val );
    void insert( iterator i, size_type num, const value_type &val );

public:

    template < class TT, class AAlocator >
    const array& operator =( const array< TT, AAlocator >  &o )
    {
        if( size_ > o.size( ) )
        {
            data_ = allocator_.trim_objects( data_, size_, o.size( ) );
            size_ = data_ == NULL ? 0 : o.size( );
        }
        else if( size_ < o.size( ) )
        {
            allocator_.deallocate_objects( data_, size_ );
            data_ = allocator_.allocate_objects( o.size( ) );
            size_ = data_ == NULL ? 0 : o.size( );
        }

        if( data_ != NULL )
        {
            for( size_type i = 0 ; i < size_ ; i++ )
            {
                data_[ i ] = static_cast< value_type >( o[ i ] );
            }
        }

        return( *this );
    }


    const array& operator =( const array  &o )
    {
        if( this == &o ) return( *this );


        allocator_ = o.allocator_;

        if( size_ > o.size_ )
        {
            data_ = allocator_.trim_objects( data_, size_, o.size_ );
            size_ = data_ == NULL ? 0 : o.size( );
        }
        else if( size_ < o.size_ )
        {
            allocator_.deallocate_objects( data_, size_ );
            data_ = allocator_.allocate_objects( o.size( ) );
            size_ = data_ == NULL ? 0 : o.size( );
        }

        if( data_ != NULL )
        {
            allocator_.copy_objects( o.data_, size_, data_ );
        }

        return( *this );
    }

protected:
    
    pointer paccess( size_type index )
    {
        return( data_ + index );
    }


    const_pointer paccess( size_type index ) const
    {
        return( data_ + index );
    }

public:

    reference at( size_type index, size_type dmy1 = 0, size_type dmy2 = 0 )
    {
        _CHECK_ACCESS_VIOLATION1_( index )
        return( data_[ index ] );
    }


    const_reference at( size_type index, size_type dmy1 = 0, size_type dmy2 = 0 ) const
    {
        _CHECK_ACCESS_VIOLATION1_( index )
        return( data_[ index ] );
    }


    reference operator ()( size_type index, size_type dmy1 = 0, size_type dmy2 = 0 )
    {
        _CHECK_ACCESS_VIOLATION1_( index )
        return( data_[ index ] );
    }

    const_reference operator ()( size_type index, size_type dmy1 = 0, size_type dmy2 = 0 ) const
    {
        _CHECK_ACCESS_VIOLATION1_( index )
        return( data_[ index ] );
    }


    reference operator []( size_type index )
    {
        _CHECK_ACCESS_VIOLATION1_( index )
        return( data_[ index ] );
    }


    const_reference operator []( size_type index ) const
    {
        _CHECK_ACCESS_VIOLATION1_( index )
        return( data_[ index ] );
    }

public:

    array( ) : allocator_( ), size_( 0 ), data_( NULL ){}


    explicit array( const Allocator &a ) : allocator_( a ), size_( 0 ), data_( NULL ){}



    explicit array( size_type num ) : allocator_( ), size_( num ), data_( NULL )
    {
        data_ = allocator_.allocate_objects( size_ );
        if( data_ == NULL ) size_ = 0;
    }

    array( size_type num, const Allocator &a ) : allocator_( a ), size_( num ), data_( NULL )
    {
        data_ = allocator_.allocate_objects( size_ );
        if( data_ == NULL ) size_ = 0;
    }



    array( size_type num, const value_type &val ) : allocator_( ), size_( num ), data_( NULL )
    {
        data_ = allocator_.allocate_objects( size_, val );
        if( data_ == NULL ) size_ = 0;
    }

    array( size_type num, const value_type &val, const Allocator &a ) : allocator_( a ), size_( num ), data_( NULL )
    {
        data_ = allocator_.allocate_objects( size_, val );
        if( data_ == NULL ) size_ = 0;
    }



    array( const_iterator s, const_iterator e ) : allocator_( ), size_( e - s ), data_( NULL )
    {
        data_ = allocator_.allocate_objects( s, e );
        if( data_ == NULL ) size_ = 0;
    }

    array( const_iterator s, const_iterator e, const Allocator &a ) : allocator_( a ), size_( e - s ), data_( NULL )
    {
        data_ = allocator_.allocate_objects( s, e );
        if( data_ == NULL ) size_ = 0;
    }



    array( size_type num, pointer ptr, size_type mem_available ) : allocator_( ptr, mem_available ), size_( num ), data_( NULL )
    {
        data_ = allocator_.allocate_objects( size_ );
        if( data_ == NULL ) size_ = 0;
    }

    array( size_type num, const value_type &val, pointer ptr, size_type mem_available ) : allocator_( ptr, mem_available ), size_( num ), data_( NULL )
    {
        data_ = allocator_.allocate_objects( size_, val );
        if( data_ == NULL ) size_ = 0;
    }



    template < class TT, class AAlocator >
    array( const array< TT, AAlocator > &o ) : allocator_( ), size_( o.size( ) ), data_( NULL )
    {
        data_ = allocator_.allocate_objects( size_ );
        if( data_ == NULL )
        {
            size_ = 0;
        }
        else
        {
            for( size_type i = 0 ; i < size_ ; i++ ) data_[i] = static_cast< value_type >( o[i] );
        }
    }

    array( const array< T, Allocator > &o ) : allocator_( o.allocator_ ), size_( o.size_ ), data_( NULL )
    {
        data_ = allocator_.allocate_objects( size_ );
        if( data_ == NULL )
        {
            size_ = 0;
        }
        else
        {
            allocator_.copy_objects( o.data_, size_, data_ );
        }
    }

    ~array( )
    {
        clear( );
    }
};



template < class T, class Allocator = std::allocator< T > >
class array1 : public array< T, Allocator >
{
public:
    typedef Allocator allocator_type;                               
    typedef typename Allocator::reference reference;                
    typedef typename Allocator::const_reference const_reference;    
    typedef typename Allocator::value_type value_type;              
    typedef typename Allocator::size_type size_type;                
    typedef typename Allocator::difference_type difference_type;    
    typedef typename Allocator::pointer pointer;                    
    typedef typename Allocator::const_pointer const_pointer;        

    
    typedef mist_iterator1< T, ptrdiff_t, pointer, reference > iterator;

    
    typedef mist_iterator1< T, ptrdiff_t, const_pointer, const_reference > const_iterator;

    
    typedef mist_reverse_iterator< mist_iterator1< T, ptrdiff_t, pointer, reference > > reverse_iterator;

    
    typedef mist_reverse_iterator< mist_iterator1< T, ptrdiff_t, const_pointer, const_reference > > const_reverse_iterator;

    
    template < class TT, class AAllocator = std::allocator< TT > > 
    struct rebind
    {
        typedef array1< TT, AAllocator > other;
    };


protected:
    typedef array< T, Allocator > base;     
    double reso1_;                          

public:
    double reso1( double r1 ){ return( reso1_ = r1 ); }                             
    double reso1( ) const { return( reso1_ ); }                                     
    void reso( double r1, double dmy1 = 1.0, double dmy2 = 1.0 ){ reso1_ = r1; }    



    iterator x_begin( ){ return( base::begin( ) ); }


    const_iterator x_begin( ) const { return( base::begin( ) ); }



    iterator x_end( ){ return( base::end( ) ); }


    const_iterator x_end( ) const { return( base::end( ) ); }




    reverse_iterator x_rbegin( ){ return( base::rbegin( ) ); }


    const_reverse_iterator x_rbegin( ) const { return( base::rbegin( ) ); }



    reverse_iterator x_rend( ){ return( base::rend( ) ); }


    const_reverse_iterator x_rend( ) const { return( base::rend( )  ); }


public: 


    bool trim( size_type index, difference_type num = -1 )
    {
        double r = reso1( );
        if( base::trim( index, num ) )
        {
            reso( r );
            return( true );
        }
        else
        {
            return( false );
        }
    }


 
    bool swap( array1 &a )
    {
        if( base::swap( a ) )
        {
            double dmy_reso1_ = reso1_;
            reso1_ = a.reso1_;
            a.reso1_ = dmy_reso1_;
            return( true );
        }
        else
        {
            return( false );
        }
    }

public:

    template < class TT, class AAlocator >
    const array1& operator =( const array1< TT, AAlocator > &o )
    {
        base::operator =( o );
        reso1_ = o.reso1( );

        return( *this );
    }


    const array1& operator =( const array1 &o )
    {
        if( this == &o ) return( *this );

        base::operator =( o );
        reso1_ = o.reso1_;

        return( *this );
    }

public:

    array1( ) : base( ), reso1_( 1.0 ) {}


    explicit array1( const Allocator &a ) : base( a ), reso1_( 1.0 ) {}


    explicit array1( size_type num ) : base( num ), reso1_( 1.0 ) {}



    array1( size_type num, double r1 ) : base( num ), reso1_( r1 ) {}


    array1( size_type num, const Allocator &a ) : base( num, a ), reso1_( 1.0 ) {}


    array1( size_type num, double r1, const Allocator &a ) : base( num, a ), reso1_( r1 ) {}



    array1( size_type num, double r1, const value_type &val ) : base( num, val ), reso1_( r1 ) {}


    array1( size_type num, const value_type &val, const Allocator &a ) : base( num, val, a ), reso1_( 1.0 ) {}


    array1( size_type num, double r1, const value_type &val, const Allocator &a ) : base( num, val, a ), reso1_( r1 ) {}



    array1( size_type num, pointer ptr, size_type mem_available ) : base( num, ptr, mem_available ), reso1_( 1.0 ) {}


    array1( size_type num, double r1, pointer ptr, size_type mem_available ) : base( num, ptr, mem_available ), reso1_( r1 ) {}


    array1( size_type num, double r1, const value_type &val, pointer ptr, size_type mem_available ) : base( num, val, ptr, mem_available ), reso1_( r1 ) {}

    template < class TT, class AAlocator >
    explicit array1( const array1< TT, AAlocator > &o ) : base( o ), reso1_( o.reso1( ) ) {}


    array1( const array1< T, Allocator > &o ) : base( o ), reso1_( o.reso1_ ) {}
};



template < class T, class Allocator = std::allocator< T > >
class array2 : public array1< T, Allocator >
{
public:
    typedef Allocator allocator_type;                               
    typedef typename Allocator::reference reference;                
    typedef typename Allocator::const_reference const_reference;    
    typedef typename Allocator::value_type value_type;              
    typedef typename Allocator::size_type size_type;                
    typedef typename Allocator::difference_type difference_type;    
    typedef typename Allocator::pointer pointer;                    
    typedef typename Allocator::const_pointer const_pointer;        


    typedef mist_iterator1< T, ptrdiff_t, pointer, reference > iterator;


    typedef mist_iterator1< T, ptrdiff_t, const_pointer, const_reference > const_iterator;


    typedef mist_reverse_iterator< mist_iterator1< T, ptrdiff_t, pointer, reference > > reverse_iterator;


    typedef mist_reverse_iterator< mist_iterator1< T, ptrdiff_t, const_pointer, const_reference > > const_reverse_iterator;


    template < class TT, class AAllocator = std::allocator< TT > > 
    struct rebind
    {
        typedef array2< TT, AAllocator > other;
    };


protected:
    typedef array1< T, Allocator > base;    
    size_type size2_;                       
    size_type size1_;                       
    double reso2_;                          

public:

    bool resize( size_type num1, size_type num2, size_type dmy1 = 0 )
    {
        if( base::resize( num1 * num2 ) )
        {
            size1_ = num1;
            size2_ = num2;
            return( true );
        }
        else
        {
            size1_ = size2_ = 0;
            return( false );
        }
    }


    bool trim( size_type x, size_type y, difference_type w = -1, difference_type h = -1 )
    {
        difference_type w_ = width( );
        difference_type h_ = height( );
        if( w_ <= static_cast< difference_type >( x ) || w_ < static_cast< difference_type >( x + w ) )
        {
            return( false );
        }
        else if( h_ <= static_cast< difference_type >( y ) || h_ < static_cast< difference_type >( y + h ) )
        {
            return( false );
        }
        else if( w_ == w && h_ == h )
        {
            return( true );
        }

        if( w < 0 )
        {
            w = w_ - x;
        }
        if( h < 0 )
        {
            h = h_ - y;
        }

        if( base::is_memory_shared( ) )
        {

            array2 o( *this );
            if( resize( w, h ) )
            {
                for( difference_type j = 0 ; j < h ; j++ )
                {
                    for( difference_type i = 0 ; i < w ; i++ )
                    {
                        operator ()( i, j ) = o( i + x, j + y );
                    }
                }
            }
            else
            {
                return( false );
            }
        }
        else
        {
            array2 o( w, h, base::reso1( ), reso2( ) );
            for( difference_type j = 0 ; j < h ; j++ )
            {
                for( difference_type i = 0 ; i < w ; i++ )
                {
                    o( i, j ) = operator ()( i + x, j + y );
                }
            }

            swap( o );
        }

        return( true );
    }



    bool swap( array2 &a )
    {
        if( base::swap( a ) )
        {
            double dmy_reso2_ = reso2_;
            reso2_ = a.reso2_;
            a.reso2_ = dmy_reso2_;

            size_type _dmy_size1 = size1_;
            size_type _dmy_size2 = size2_;
            size1_ = a.size1_;
            size2_ = a.size2_;
            a.size1_ = _dmy_size1;
            a.size2_ = _dmy_size2;
            return( true );
        }
        else
        {
            return( false );
        }
    }



    void clear( )
    {
        base::clear( );
        size1_ = size2_ = 0;
    }


    size_type size1( )  const { return( size1_ ); }         
    size_type size2( )  const { return( size2_ ); }         
    size_type width( )  const { return( size1_ ); }         
    size_type height( ) const { return( size2_ ); }         

    double reso2( double r2 ){ return( reso2_ = r2 ); }     
    double reso2( ) const { return( reso2_ ); }             

    void reso( double r1, double r2, double dmy = 1.0 ){ base::reso1_ = r1; reso2_ = r2; }



    iterator x_begin( size_type i ){ return( iterator( paccess( i, 0 ), width( ) ) ); }


    const_iterator x_begin( size_type i ) const { return( const_iterator( paccess( i, 0 ), width( ) ) ); }


    iterator x_end( size_type i ){ return( iterator( paccess( i, height( ) ), width( ) ) ); }


    const_iterator x_end( size_type i ) const { return( const_iterator( paccess( i, height( ) ), width( ) ) ); }



    reverse_iterator x_rbegin( size_type i ){ return( reverse_iterator( x_end( i ) ) ); }


    const_reverse_iterator x_rbegin( size_type i ) const { return( const_reverse_iterator( x_end( i ) ) ); }



    reverse_iterator x_rend( size_type i ){ return( reverse_iterator( x_begin( i ) ) ); }


    const_reverse_iterator x_rend( size_type i ) const { return( const_reverse_iterator( x_begin( i ) ) ); }


    iterator y_begin( size_type j ){ return( iterator( paccess( 0, j ), 1 ) ); }


    const_iterator y_begin( size_type j ) const { return( const_iterator( paccess( 0, j ), 1 ) ); }


    iterator y_end( size_type j ){ return( iterator( paccess( width( ), j ), 1 ) ); }


    const_iterator y_end( size_type j ) const { return( const_iterator( paccess( width( ), j ), 1 ) ); }



    reverse_iterator y_rbegin( size_type j ){ return( reverse_iterator( y_end( j ) ) ); }


    const_reverse_iterator y_rbegin( size_type j ) const { return( const_reverse_iterator( y_end( j ) ) ); }


    reverse_iterator y_rend( size_type j ){ return( reverse_iterator( y_begin( j ) ) ); }


    const_reverse_iterator y_rend( size_type j ) const { return( const_reverse_iterator( y_begin( j ) ) ); }


public:

    template < class TT, class AAlocator >
    const array2& operator =( const array2< TT, AAlocator > &o )
    {
        base::operator =( o );

        if( base::empty( ) )
        {
            size1_ = size2_ = 0;
        }
        else
        {
            size1_ = o.size1( );
            size2_ = o.size2( );
        }

        reso2_ = o.reso2( );

        return( *this );
    }



    const array2& operator =( const array2 &o )
    {
        if( this == &o ) return( *this );

        base::operator =( o );

        if( base::empty( ) )
        {
            size1_ = size2_ = 0;
        }
        else
        {
            size1_ = o.size1( );
            size2_ = o.size2( );
        }

        reso2_ = o.reso2( );

        return( *this );
    }


protected:

    const_pointer paccess( size_type i, size_type j ) const
    {
        return( base::data_ + i + j * size1_ );
    }

public:

    reference at( size_type i, size_type j, size_type dmy = 0 )
    {
        _CHECK_ACCESS_VIOLATION2_( i, j )
        return( base::data_[ i + j * size1_ ] );
    }



    const_reference at( size_type i, size_type j, size_type dmy = 0 ) const
    {
        _CHECK_ACCESS_VIOLATION2_( i, j )
        return( base::data_[ i + j * size1_ ] );
    }


    reference operator ()( size_type i, size_type j, size_type dmy = 0 )
    {
        _CHECK_ACCESS_VIOLATION2_( i, j )
        return( base::data_[ i + j * size1_ ] );
    }


    const_reference operator ()( size_type i, size_type j, size_type dmy = 0 ) const
    {
        _CHECK_ACCESS_VIOLATION2_( i, j )
        return( base::data_[ i + j * size1_ ] );
    }


public:

    array2( ) : base( ), size1_( 0 ), size2_( 0 ), reso2_( 1.0 ) {}


    explicit array2( const Allocator &a ) : base( a ), size1_( 0 ), size2_( 0 ), reso2_( 1.0 ) {}


    array2( size_type num1, size_type num2 ) : base( num1 * num2 ), size1_( num1 ), size2_( num2 ), reso2_( 1.0 )
    {
        if( base::empty( ) ) size1_ = size2_ = 0;
    }



    array2( size_type num1, size_type num2, double r1, double r2 ) : base( num1 * num2, r1 ), size1_( num1 ), size2_( num2 ), reso2_( r2 )
    {
        if( base::empty( ) ) size1_ = size2_ = 0;
    }


    array2( size_type num1, size_type num2, const Allocator &a ) : base( num1 * num2, a ), size1_( num1 ), size2_( num2 ), reso2_( 1.0 )
    {
        if( base::empty( ) ) size1_ = size2_ = 0;
    }


    array2( size_type num1, size_type num2, double r1, double r2, const Allocator &a ) : base( num1 * num2, r1, a ), size1_( num1 ), size2_( num2 ), reso2_( r2 )
    {
        if( base::empty( ) ) size1_ = size2_ = 0;
    }



    array2( size_type num1, size_type num2, const value_type &val ) : base( num1 * num2, val ), size1_( num1 ), size2_( num2 ), reso2_( 1.0 )
    {
        if( base::empty( ) ) size1_ = size2_ = 0;
    }


    array2( size_type num1, size_type num2, double r1, double r2, const value_type &val ) : base( num1 * num2, r1, val ), size1_( num1 ), size2_( num2 ), reso2_( r2 )
    {
        if( base::empty( ) ) size1_ = size2_ = 0;
    }


    array2( size_type num1, size_type num2, const value_type &val, const Allocator &a ) : base( num1 * num2, val, a ), size1_( num1 ), size2_( num2 ), reso2_( 1.0 )
    {
        if( base::empty( ) ) size1_ = size2_ = 0;
    }


    array2( size_type num1, size_type num2, double r1, double r2, const value_type &val, const Allocator &a ) : base( num1 * num2, r1, val, a ), size1_( num1 ), size2_( num2 ), reso2_( r2 )
    {
        if( base::empty( ) ) size1_ = size2_ = 0;
    }



    array2( size_type num1, size_type num2, pointer ptr, size_type mem_available ) : base( num1 * num2, ptr, mem_available ), size1_( num1 ), size2_( num2 ), reso2_( 1.0 )
    {
        if( base::empty( ) ) size1_ = size2_ = 0;
    }


    array2( size_type num1, size_type num2, double r1, double r2, pointer ptr, size_type mem_available ) : base( num1 * num2, r1, ptr, mem_available ), size1_( num1 ), size2_( num2 ), reso2_( r2 )
    {
        if( base::empty( ) ) size1_ = size2_ = 0;
    }


    array2( size_type num1, size_type num2, double r1, double r2, const value_type &val, pointer ptr, size_type mem_available ) : base( num1 * num2, r1, val, ptr, mem_available ), size1_( num1 ), size2_( num2 ), reso2_( r2 )
    {
        if( base::empty( ) ) size1_ = size2_ = 0;
    }




    template < class TT, class AAlocator >
    array2( const array2< TT, AAlocator > &o ) : base( o ), size1_( o.size1( ) ), size2_( o.size2( ) ), reso2_( o.reso2( ) )
    {
        if( base::empty( ) ) size1_ = size2_ = 0;
    }


    array2( const array2< T, Allocator > &o ) : base( o ), size1_( o.size1_ ), size2_( o.size2_ ), reso2_( o.reso2_ )
    {
        if( base::empty( ) ) size1_ = size2_ = 0;
    }
};





template < class T, class Allocator = std::allocator< T > >
class array3 : public array2< T, Allocator >
{
public:
    typedef Allocator allocator_type;                               
    typedef typename Allocator::reference reference;                
    typedef typename Allocator::const_reference const_reference;    
    typedef typename Allocator::value_type value_type;              
    typedef typename Allocator::size_type size_type;                
    typedef typename Allocator::difference_type difference_type;    
    typedef typename Allocator::pointer pointer;                    
    typedef typename Allocator::const_pointer const_pointer;        


    typedef mist_iterator2< T, ptrdiff_t, pointer, reference > iterator;


    typedef mist_iterator2< T, ptrdiff_t, const_pointer, const_reference > const_iterator;


    typedef mist_reverse_iterator< mist_iterator2< T, ptrdiff_t, pointer, reference > > reverse_iterator;


    typedef mist_reverse_iterator< mist_iterator2< T, ptrdiff_t, const_pointer, const_reference > > const_reverse_iterator;


    template < class TT, class AAllocator = std::allocator< TT > > 
    struct rebind
    {
        typedef array3< TT, AAllocator > other;
    };


protected:
    typedef array2< T, Allocator > base;    
    size_type size3_;                       
    size_type size2_;                       
    size_type size1_;                       
    double reso3_;                          

public:
 
    bool resize( size_type num1, size_type num2, size_type num3 )
    {
        if( base::resize( num1 * num2, num3 ) )
        {
            size1_ = num1;
            size2_ = num2;
            size3_ = num3;
            return( true );
        }
        else
        {
            size1_ = size2_ = size3_ = 0;
            return( false );
        }
    }

    bool trim( size_type x, size_type y, size_type z, difference_type w = -1, difference_type h = -1, difference_type d = -1 )
    {
        difference_type w_ = width( );
        difference_type h_ = height( );
        difference_type d_ = depth( );
        if( w_ <= static_cast< difference_type >( x ) || w_ < static_cast< difference_type >( x + w ) )
        {
            return( false );
        }
        else if( h_ <= static_cast< difference_type >( y ) || h_ < static_cast< difference_type >( y + h ) )
        {
            return( false );
        }
        else if( d_ <= static_cast< difference_type >( z ) || d_ < static_cast< difference_type >( z + d ) )
        {
            return( false );
        }
        else if( w_ == w && h_ == h && d_ == d )
        {
            return( true );
        }

        if( w < 0 )
        {
            w = w_ - x;
        }
        if( h < 0 )
        {
            h = h_ - y;
        }
        if( d < 0 )
        {
            d = d_ - z;
        }

        if( base::is_memory_shared( ) )
        {

            array3 o( *this );
            if( resize( w, h, d ) )
            {
                for( difference_type k = 0 ; k < d ; k++ )
                {
                    for( difference_type j = 0 ; j < h ; j++ )
                    {
                        for( difference_type i = 0 ; i < w ; i++ )
                        {
                            operator ()( i, j, k ) = o( i + x, j + y, k + z );
                        }
                    }
                }
            }
            else
            {
                return( false );
            }
        }
        else
        {
            array3 o( w, h, d, base::reso1( ), base::reso2( ), reso3( ) );
            for( difference_type k = 0 ; k < d ; k++ )
            {
                for( difference_type j = 0 ; j < h ; j++ )
                {
                    for( difference_type i = 0 ; i < w ; i++ )
                    {
                        o( i, j, k ) = operator ()( i + x, j + y, k + z );
                    }
                }
            }

            swap( o );
        }

        return( true );
    }


    bool swap( array3 &a )
    {
        if( base::swap( a ) )
        {
            double dmy_reso3_ = reso3_;
            reso3_ = a.reso3_;
            a.reso3_ = dmy_reso3_;

            size_type _dmy_size1 = size1_;
            size_type _dmy_size2 = size2_;
            size_type _dmy_size3 = size3_;
            size1_ = a.size1_;
            size2_ = a.size2_;
            size3_ = a.size3_;
            a.size1_ = _dmy_size1;
            a.size2_ = _dmy_size2;
            a.size3_ = _dmy_size3;

            return( true );
        }
        else
        {
            return( true );
        }
    }



    void clear( )
    {
        base::clear( );
        size1_ = size2_ = size3_ = 0;
    }


    size_type size1( ) const { return( size1_ ); }          
    size_type size2( ) const { return( size2_ ); }          
    size_type size3( ) const { return( size3_ ); }          
    size_type width( ) const { return( size1_ ); }          
    size_type height( ) const { return( size2_ ); }         
    size_type depth( ) const { return( size3_ ); }          

    double reso3( double r3 ){ return( reso3_ = r3 ); }     
    double reso3( ) const { return( reso3_ ); }             


    void reso( double r1, double r2, double r3 ){ base::reso1_ = r1; base::reso2_ = r2; reso3_ = r3; }




    iterator begin( ){ return( iterator( paccess( 0, 0, 0 ), 0, base::size( ), 0 ) ); }


    const_iterator begin( ) const { return( const_iterator( paccess( 0, 0, 0 ), 0, base::size( ), 0 ) ); }


    iterator end( ){ return( iterator( paccess( 0, 0, 0 ), base::size( ), base::size( ), 0 ) ); }


    const_iterator end( ) const { return( const_iterator( paccess( 0, 0, 0 ), base::size( ), base::size( ), 0 ) ); }



    reverse_iterator rbegin( ){ return( reverse_iterator( end( ) ) ); }


    const_reverse_iterator rbegin( ) const { return( const_reverse_iterator( end( ) ) ); }


    reverse_iterator rend( ){ return( reverse_iterator( begin( ) ) ); }


    const_reverse_iterator rend( ) const { return( const_reverse_iterator( begin( ) ) ); }


    iterator x_begin( size_type i ){ return( iterator( paccess( i, 0, 0 ), 0, 1, width( ) ) ); }


    const_iterator x_begin( size_type i ) const { return( const_iterator( paccess( i, 0, 0 ), 0, 1, width( ) ) ); }


    iterator x_end( size_type i ){ return( iterator( paccess( i, 0, 0 ), height( ) * depth( ), 1, width( ) ) ); }


    const_iterator x_end( size_type i ) const { return( const_iterator( paccess( i, 0, 0 ), height( ) * depth( ), 1, width( ) ) ); }




    reverse_iterator x_rbegin( size_type i ){ return( reverse_iterator( x_end( i ) ) ); }


    const_reverse_iterator x_rbegin( size_type i ) const { return( const_reverse_iterator( x_end( i ) ) ); }


    reverse_iterator x_rend( size_type i ){ return( reverse_iterator( x_begin( i ) ) ); }


    const_reverse_iterator x_rend( size_type i ) const { return( const_reverse_iterator( x_begin( i ) ) ); }



    iterator y_begin( size_type j ){ return( iterator( paccess( 0, j, 0 ), 0, height( ), width( ) * height( ) ) ); }


    const_iterator y_begin( size_type j ) const { return( const_iterator( paccess( 0, j, 0 ), 0, height( ), width( ) * height( ) ) ); }


    iterator y_end( size_type j ){ return( iterator( paccess( 0, j, 0 ), width( ) * depth( ), height( ), width( ) * height( ) ) ); }


    const_iterator y_end( size_type j ) const { return( const_iterator( paccess( 0, j, 0 ), width( ) * depth( ), height( ), width( ) * height( ) ) ); }



    reverse_iterator y_rbegin( size_type j ){ return( reverse_iterator( y_end( j ) ) ); }


    const_reverse_iterator y_rbegin( size_type j ) const { return( const_reverse_iterator( y_end( j ) ) ); }


    reverse_iterator y_rend( size_type j ){ return( reverse_iterator( y_begin( j ) ) ); }


    const_reverse_iterator y_rend( size_type j ) const { return( const_reverse_iterator( y_begin( j ) ) ); }



    iterator z_begin( size_type k ){ return( iterator( paccess( 0, 0, k ), 0, 1, 1 ) ); }


    const_iterator z_begin( size_type k ) const { return( const_iterator( paccess( 0, 0, k ), 0, 1, 1 ) ); }


    iterator z_end( size_type k ){ return( iterator( paccess( 0, 0, k ), width( ) * height( ), 1, 1 ) ); }


    const_iterator z_end( size_type k ) const { return( const_iterator( paccess( 0, 0, k ), width( ) * height( ), 1, 1 ) ); }


    reverse_iterator z_rbegin( size_type k ){ return( reverse_iterator( z_end( k ) ) ); }


    const_reverse_iterator z_rbegin( size_type k ) const { return( const_reverse_iterator( z_end( k )) ); }


    reverse_iterator z_rend( size_type k ){ return( reverse_iterator( z_begin( k ) ) ); }


    const_reverse_iterator z_rend( size_type k ) const { return( const_reverse_iterator( z_begin( k ) ) ); }


public:

    template < class TT, class AAlocator >
    const array3& operator =( const array3< TT, AAlocator > &o )
    {
        base::operator =( o );

        if( base::empty( ) )
        {
            size1_ = size2_ = size3_ = 0;
        }
        else
        {
            size1_ = o.size1( );
            size2_ = o.size2( );
            size3_ = o.size3( );
        }

        reso3_ = o.reso3( );

        return( *this );
    }


 
    const array3& operator =( const array3 &o )
    {
        if( this == &o ) return( *this );

        base::operator =( o );

        if( base::empty( ) )
        {
            size1_ = size2_ = size3_ = 0;
        }
        else
        {
            size1_ = o.size1( );
            size2_ = o.size2( );
            size3_ = o.size3( );
        }

        reso3_ = o.reso3( );

        return( *this );
    }



protected:
 
    pointer paccess( size_type i, size_type j, size_type k )
    {
        return( base::data_ + i + ( j + k * size2_ ) * size1_ );
    }


    const_pointer paccess( size_type i, size_type j, size_type k ) const
    {
        return( base::data_ + i + ( j + k * size2_ ) * size1_ );
    }

public:

    reference at( size_type i, size_type j, size_type k )
    {
        _CHECK_ACCESS_VIOLATION3_( i, j, k )
        return( base::data_[ i + ( j + k * size2_ ) * size1_ ] );
    }



    const_reference at( size_type i, size_type j, size_type k ) const
    {
        _CHECK_ACCESS_VIOLATION3_( i, j, k )
        return( base::data_[ i + ( j + k * size2_ ) * size1_ ] );
    }


    reference operator ()( size_type i, size_type j, size_type k )
    {
        _CHECK_ACCESS_VIOLATION3_( i, j, k )
        return( base::data_[ i + ( j + k * size2_ ) * size1_ ] );
    }


    const_reference operator ()( size_type i, size_type j, size_type k ) const
    {
        _CHECK_ACCESS_VIOLATION3_( i, j, k )
        return( base::data_[ i + ( j + k * size2_ ) * size1_ ] );
    }


public:

    array3( ) : base( ), size1_( 0 ), size2_( 0 ), size3_( 0 ), reso3_( 1.0 ) {}


    explicit array3( const Allocator &a ) : base( a ), size1_( 0 ), size2_( 0 ), size3_( 0 ), reso3_( 1.0 ) {}


    array3( size_type num1, size_type num2, size_type num3 ) : base( num1 * num2, num3 ), size1_( num1 ), size2_( num2 ), size3_( num3 ), reso3_( 1.0 )
    {
        if( base::empty( ) ) size1_ = size2_ = size3_ = 0;
    }


    array3( size_type num1, size_type num2, size_type num3, double r1, double r2, double r3 ) : base( num1 * num2, num3, r1, r2 ), size1_( num1 ), size2_( num2 ), size3_( num3 ), reso3_( r3 )
    {
        if( base::empty( ) ) size1_ = size2_ = size3_ = 0;
    }


    array3( size_type num1, size_type num2, size_type num3, const Allocator &a ) : base( num1 * num2, num3, a ), size1_( num1 ), size2_( num2 ), size3_( num3 ), reso3_( 1.0 )
    {
        if( base::empty( ) ) size1_ = size2_ = size3_ = 0;
    }


    array3( size_type num1, size_type num2, size_type num3, double r1, double r2, double r3, const Allocator &a ) : base( num1 * num2, num3, r1, r2, a ), size1_( num1 ), size2_( num2 ), size3_( num3 ), reso3_( r3 )
    {
        if( base::empty( ) ) size1_ = size2_ = size3_ = 0;
    }



    array3( size_type num1, size_type num2, size_type num3, const value_type &val ) : base( num1 * num2, num3, val ), size1_( num1 ), size2_( num2 ), size3_( num3 ), reso3_( 1.0 )
    {
        if( base::empty( ) ) size1_ = size2_ = size3_ = 0;
    }


    array3( size_type num1, size_type num2, size_type num3, double r1, double r2, double r3, const value_type &val ) : base( num1 * num2, num3, r1, r2, val ), size1_( num1 ), size2_( num2 ), size3_( num3 ), reso3_( r3 )
    {
        if( base::empty( ) ) size1_ = size2_ = size3_ = 0;
    }


    array3( size_type num1, size_type num2, size_type num3, const value_type &val, const Allocator &a ) : base( num1 * num2, num3, val, a ), size1_( num1 ), size2_( num2 ), size3_( num3 ), reso3_( 1.0 )
    {
        if( base::empty( ) ) size1_ = size2_ = size3_ = 0;
    }


    array3( size_type num1, size_type num2, size_type num3, double r1, double r2, double r3, const value_type &val, const Allocator &a ) : base( num1 * num2, num3, r1, r2, val, a ), size1_( num1 ), size2_( num2 ), size3_( num3 ), reso3_( r3 )
    {
        if( base::empty( ) ) size1_ = size2_ = size3_ = 0;
    }




    array3( size_type num1, size_type num2, size_type num3, pointer ptr, size_type mem_available ) : base( num1 * num2, num3, ptr, mem_available ), size1_( num1 ), size2_( num2 ), size3_( num3 ), reso3_( 1.0 )
    {
        if( base::empty( ) ) size1_ = size2_ = size3_ = 0;
    }


    array3( size_type num1, size_type num2, size_type num3, double r1, double r2, double r3, pointer ptr, size_type mem_available ) : base( num1 * num2, num3, r1, r2, ptr, mem_available ), size1_( num1 ), size2_( num2 ), size3_( num3 ), reso3_( r3 )
    {
        if( base::empty( ) ) size1_ = size2_ = size3_ = 0;
    }


    array3( size_type num1, size_type num2, size_type num3, double r1, double r2, double r3, const value_type &val, pointer ptr, size_type mem_available ) : base( num1 * num2, num3, r1, r2, val, ptr, mem_available ), size1_( num1 ), size2_( num2 ), size3_( num3 ), reso3_( r3 )
    {
        if( base::empty( ) ) size1_ = size2_ = size3_ = 0;
    }



    template < class TT, class AAlocator >
    array3( const array3< TT, AAlocator > &o ) : base( o ), size1_( o.size1( ) ), size2_( o.size2( ) ), size3_( o.size3( ) ), reso3_( o.reso3( ) )
    {
        if( base::empty( ) ) size1_ = size2_ = size3_ = 0;
    }


    array3( const array3< T, Allocator > &o ) : base( o ), size1_( o.size1_ ), size2_( o.size2_ ), size3_( o.size3_ ), reso3_( o.reso3_ )
    {
        if( base::empty( ) ) size1_ = size2_ = size3_ = 0;
    }
};




template < class Array >
class marray : public Array
{
public:
    typedef typename Array::allocator_type allocator_type;      
    typedef typename Array::reference reference;                
    typedef typename Array::const_reference const_reference;    
    typedef typename Array::value_type value_type;              
    typedef typename Array::size_type size_type;                
    typedef typename Array::difference_type difference_type;    
    typedef typename Array::pointer pointer;                    
    typedef typename Array::const_pointer const_pointer;        

    typedef typename Array::iterator iterator;                              
    typedef typename Array::const_iterator const_iterator;                  
    typedef typename Array::reverse_iterator reverse_iterator;              
    typedef typename Array::const_reverse_iterator const_reverse_iterator;  


protected:
    typedef Array base;                     
    size_type margin1_;                     
    size_type margin2_;                     
    size_type margin3_;                     

public:

    bool resize( size_type num1 )
    {
        return( base::resize( num1 + margin1_ * 2 ) );
    }



    bool resize( size_type num1, size_type num2 )
    {
        return( base::resize( num1 + margin1_ * 2, num2 + margin2_ * 2 ) );
    }



    bool resize( size_type num1, size_type num2, size_type num3 )
    {
        return( base::resize( num1 + margin1_ * 2, num2 + margin2_ * 2, num3 + margin3_ * 2 ) );
    }




    bool swap( marray &a )
    {
        if( base::swap( a ) )
        {
            size_type tmp = margin1_;
            margin1_ = a.margin1_;
            a.margin1_ = tmp;

            tmp = margin2_;
            margin2_ = a.margin2_;
            a.margin2_ = tmp;

            tmp = margin3_;
            margin3_ = a.margin3_;
            a.margin3_ = tmp;

            return( true );
        }
        else
        {
            return( false );
        }
    }


    void clear( )
    {
        base::clear( );
        margin1_ = margin2_ = margin3_ = 0;
    }



    void fill_margin( const value_type &val = value_type( ) )
    {
        if( !base::empty( ) )
        {
            base &o = *this;

            size_type i, j, k;
            for( k = 0 ; k < margin3( ) ; k++ )
            {
                for( j = 0 ; j < o.size2( ) ; j++ )
                {
                    for( i = 0 ; i < o.size1( ) ; i++ )
                    {
                        o( i, j, k ) = val;
                    }
                }
            }
            for( k = o.size3( ) - margin3( ) ; k < o.size3( ) ; k++ )
            {
                for( j = 0 ; j < o.size2( ) ; j++ )
                {
                    for( i = 0 ; i < o.size1( ) ; i++ )
                    {
                        o( i, j, k ) = val;
                    }
                }
            }

            for( j = 0 ; j < margin2( ) ; j++ )
            {
                for( k = 0 ; k < o.size3( ) ; k++ )
                {
                    for( i = 0 ; i < o.size1( ) ; i++ )
                    {
                        o( i, j, k ) = val;
                    }
                }
            }
            for( j = o.size2( ) - margin2( ) ; j < o.size2( ) ; j++ )
            {
                for( k = 0 ; k < o.size3( ) ; k++ )
                {
                    for( i = 0 ; i < o.size1( ) ; i++ )
                    {
                        o( i, j, k ) = val;
                    }
                }
            }

            for( i = 0 ; i < margin1( ) ; i++ )
            {
                for( k = 0 ; k < o.size3( ) ; k++ )
                {
                    for( j = 0 ; j < o.size2( ) ; j++ )
                    {
                        o( i, j, k ) = val;
                    }
                }
            }
            for( i = o.size1( ) - margin1( ) ; i < o.size1( ) ; i++ )
            {
                for( k = 0 ; k < o.size3( ) ; k++ )
                {
                    for( j = 0 ; j < o.size2( ) ; j++ )
                    {
                        o( i, j, k ) = val;
                    }
                }
            }
        }
    }


    size_type size1( ) const { return( base::empty( ) ? 0 : base::size1( ) - 2 * margin1_ ); }  
    size_type size2( ) const { return( base::empty( ) ? 0 : base::size2( ) - 2 * margin2_ ); }  
    size_type size3( ) const { return( base::empty( ) ? 0 : base::size3( ) - 2 * margin3_ ); }  

    size_type width( ) const { return( size1( ) ); }        
    size_type height( ) const { return( size2( ) ); }       
    size_type depth( ) const { return( size3( ) ); }        

    size_type margin1( ) const { return( margin1_ ); }      
    size_type margin2( ) const { return( margin2_ ); }      
    size_type margin3( ) const { return( margin3_ ); }      

private:

    template < class T, class Allocator >
    const marray& copy( const array< T, Allocator > &o )
    {
        size_type length = base::size( ) < o.size( ) ? base::size( ) : o.size( );
        for( size_type i = 0 ; i < length ; i++ )
        {
            ( *this )[ i ] = static_cast< value_type >( o[ i ] );
        }
        return( *this );
    }



    template < class T, class Allocator >
    const marray& copy( const array1< T, Allocator > &o )
    {
        size_type length = base::size( ) < o.size( ) ? base::size( ) : o.size( );
        for( size_type i = 0 ; i < length ; i++ )
        {
            ( *this )[ i ] = static_cast< value_type >( o[ i ] );
        }
        return( *this );
    }



    template < class T, class Allocator >
    const marray& copy( const array2< T, Allocator > &o )
    {
        size_type w = width( )  < o.width( )  ? width( )  : o.width( );
        size_type h = height( ) < o.height( ) ? height( ) : o.height( );
        for( size_type j = 0 ; j < h ; j++ )
        {
            for( size_type i = 0 ; i < w ; i++ )
            {
                ( *this )( i, j ) = static_cast< value_type >( o( i, j ) );
            }
        }
        return( *this );
    }



    template < class T, class Allocator >
    const marray& copy( const array3< T, Allocator > &o )
    {
        size_type w = width( )  < o.width( )  ? width( )  : o.width( );
        size_type h = height( ) < o.height( ) ? height( ) : o.height( );
        size_type d = depth( )  < o.depth( )  ? depth( )  : o.depth( );
        for( size_type k = 0 ; k < d ; k++ )
        {
            for( size_type j = 0 ; j < h ; j++ )
            {
                for( size_type i = 0 ; i < w ; i++ )
                {
                    ( *this )( i, j, k ) = static_cast< value_type >( o( i, j, k ) );
                }
            }
        }
        return( *this );
    }

public:

    const marray& operator =( const marray &o )
    {
        if( this == &o ) return( *this );

        base::operator =( o );
        margin1_ = o.margin1_;
        margin2_ = o.margin2_;
        margin3_ = o.margin3_;

        return( *this );
    }



    template < class T, class Allocator >
    const marray& operator =( const array< T, Allocator > &o )
    {
        if( base::resize( o.size( ) + margin1_ * 2 ) )
        {
            return( copy( o ) );
        }
        else
        {
            return( *this );
        }
    }



    template < class T, class Allocator >
    const marray& operator =( const array1< T, Allocator > &o )
    {
        if( base::resize( o.size( ) + margin1_ * 2 ) )
        {
            reso1( o.reso1( ) );
            return( copy( o ) );
        }
        else
        {
            return( *this );
        }
    }



    template < class T, class Allocator >
    const marray& operator =( const array2< T, Allocator > &o )
    {
        if( base::resize( o.size1( ) + margin1_ * 2, o.size2( ) + margin2_ * 2 ) )
        {
            reso1( o.reso1( ) );
            reso2( o.reso2( ) );
            return( copy( o ) );
        }
        else
        {
            return( *this );
        }
    }



    template < class T, class Allocator >
    const marray& operator =( const array3< T, Allocator > &o )
    {
        if( base::resize( o.size1( ) + margin1_ * 2, o.size2( ) + margin2_ * 2, o.size3( ) + margin3_ * 2 ) )
        {
            reso1( o.reso1( ) );
            reso2( o.reso2( ) );
            reso3( o.reso3( ) );
            return( copy( o ) );
        }
        else
        {
            return( *this );
        }
    }


public:

    reference at( difference_type index )
    {
        return( base::at( index + margin1_ ) );
    }


    reference at( difference_type i, difference_type j )
    {
        return( base::at( i + margin1_, j + margin2_ ) );
    }


    reference at( difference_type i, difference_type j, difference_type k )
    {
        return( base::at( i + margin1_, j + margin2_, k + margin3_ ) );
    }


    const_reference at( difference_type index ) const
    {
        return( base::at( index + margin1_ ) );
    }


    const_reference at( difference_type i, difference_type j ) const
    {
        return( base::at( i + margin1_, j + margin2_ ) );
    }


    const_reference at( difference_type i, difference_type j, difference_type k ) const
    {
        return( base::at( i + margin1_, j + margin2_, k + margin3_ ) );
    }


    reference operator []( difference_type index )
    {
        return( base::operator []( index ) );
    }



    reference operator ()( difference_type index )
    {
        return( base::operator ()( index + margin1_ ) );
    }


    reference operator ()( difference_type i, difference_type j )
    {
        return( base::operator ()( i + margin1_, j + margin2_ ) );
    }


    reference operator ()( difference_type i, difference_type j, difference_type k )
    {
        return( base::operator ()( i + margin1_, j + margin2_, k + margin3_ ) );
    }


    const_reference operator []( difference_type index ) const
    {
        return( base::operator []( index ) );
    }


    const_reference operator ()( difference_type index ) const
    {
        return( base::operator ()( index + margin1_ ) );
    }


    const_reference operator ()( difference_type i, difference_type j ) const
    {
        return( base::operator ()( i + margin1_, j + margin2_ ) );
    }


    const_reference operator ()( difference_type i, difference_type j, difference_type k ) const
    {
        return( base::operator ()( i + margin1_, j + margin2_, k + margin3_ ) );
    }


public:

    marray( ) : base( ), margin1_( 0 ), margin2_( 0 ), margin3_( 0 ) {}


    marray( size_type margin ) : base( ), margin1_( margin ), margin2_( margin ), margin3_( margin ) {}


    marray( const marray &o ) : base( o ), margin1_( o.margin1( ) ), margin2_( o.margin2( ) ), margin3_( o.margin3( ) ) {}




    marray( size_type w, size_type margin ) : base( w + margin * 2 ), margin1_( margin ), margin2_( 0 ), margin3_( 0 ) {}


    marray( size_type w, size_type h, size_type margin ) : base( w + margin * 2, h + margin * 2 ), margin1_( margin ), margin2_( margin ), margin3_( 0 ) {}


    marray( size_type w, size_type h, size_type d, size_type margin ) : base( w + margin * 2, h + margin * 2, d + margin * 2 ), margin1_( margin ), margin2_( margin ), margin3_( margin ) {}





    template < class T, class Allocator >
    marray( const array< T, Allocator > &o, size_type margin1, const value_type &val = value_type( 0 ) )
        : base( o.size( ) + margin1 * 2 ), margin1_( margin1 ), margin2_( 0 ), margin3_( 0 )
    {
        fill_margin( val );
        copy( o );
    }



    template < class T, class Allocator >
    marray( const array1< T, Allocator > &o, size_type margin1, const value_type &val = value_type( ) )
        : base( o.size( ) + margin1 * 2, o.reso1( ) ), margin1_( margin1 ), margin2_( 0 ), margin3_( 0 )
    {
        fill_margin( val );
        copy( o );
    }



    template < class T, class Allocator >
    marray( const array2< T, Allocator > &o, size_type margin1, size_type margin2, const value_type &val = value_type( ) )
        : base( o.size1( ) + margin1 * 2, o.size2( ) + margin2 * 2, o.reso1( ), o.reso2( ) ), margin1_( margin1 ), margin2_( margin2 ), margin3_( 0 )
    {
        fill_margin( val );
        copy( o );
    }


    template < class T, class Allocator >
    marray( const array2< T, Allocator > &o, size_type margin1, size_type margin2, size_type margin3, const value_type &val )
        : base( o.size1( ) + margin1 * 2, o.size2( ) + margin2 * 2, o.reso1( ), o.reso2( ) ), margin1_( margin1 ), margin2_( margin2 ), margin3_( 0 )
    {
        fill_margin( val );
        copy( o );
    }




    template < class T, class Allocator >
    marray( const array3< T, Allocator > &o, size_type margin1, size_type margin2, size_type margin3, const value_type &val = value_type( ) )
        : base( o.size1( ) + margin1 * 2, o.size2( ) + margin2 * 2, o.size3( ) + margin3 * 2, o.reso1( ), o.reso2( ), o.reso3( ) ), margin1_( margin1 ), margin2_( margin2 ), margin3_( margin3 )
    {
        fill_margin( val );
        copy( o );
    }



    template < class T, class Allocator >
    marray( const array2< T, Allocator > &o, size_type margin1, const value_type &val = value_type( ) )
        : base( o.size1( ) + margin1 * 2, o.size2( ) + margin1 * 2, o.reso1( ), o.reso2( ) ), margin1_( margin1 ), margin2_( margin1 ), margin3_( 0 )
    {
        fill_margin( val );
        copy( o );
    }


    template < class T, class Allocator >
    marray( const array3< T, Allocator > &o, size_type margin1, const value_type &val = value_type( ) )
        : base( o.size1( ) + margin1 * 2, o.size2( ) + margin1 * 2, o.size3( ) + margin1 * 2, o.reso1( ), o.reso2( ), o.reso3( ) ), margin1_( margin1 ), margin2_( margin1 ), margin3_( margin1 )
    {
        fill_margin( val );
        copy( o );
    }
};





template < class Array >
class buffered_array : public Array
{
public:
    typedef typename Array::allocator_type allocator_type;      
    typedef typename Array::reference reference;                
    typedef typename Array::const_reference const_reference;    
    typedef typename Array::value_type value_type;              
    typedef typename Array::size_type size_type;                
    typedef typename Array::difference_type difference_type;    
    typedef typename Array::pointer pointer;                    
    typedef typename Array::const_pointer const_pointer;        

    typedef typename Array::iterator iterator;                              
    typedef typename Array::const_iterator const_iterator;                  
    typedef typename Array::reverse_iterator reverse_iterator;              
    typedef typename Array::const_reverse_iterator const_reverse_iterator;  

protected:
    typedef Array base;                     
    size_type size1_;                       
    size_type size2_;                       
    size_type size3_;                       


    inline size_t floor_square_index( size_t v )
    {
        if( v == 0 )
        {
            return( 0 );
        }

        for( size_t i = 1, _2 = 2 ; i < 64 ; i++ )
        {
            if( v <= _2 )
            {
                return( i );
            }
            _2 *= 2;
        }

        return( 0 );
    }


    inline size_t floor_square( size_t v )
    {
        if( v == 0 )
        {
            return( 0 );
        }

        for( size_t i = 1, _2 = 2 ; i < 64 ; i++ )
        {
            if( v <= _2 )
            {
                return( _2 );
            }
            _2 *= 2;
        }

        return( 0 );
    }

public:

    bool resize( size_type num1 )
    {
        if( base::resize( floor_square( num1 ) ) )
        {
            size1_ = num1;
            return( true );
        }
        else
        {
            size1_ = 0;
            return( false );
        }
    }



    bool resize( size_type num1, size_type num2 )
    {
        size_type s1 = floor_square( num1 );
        size_type s2 = floor_square( num2 );
        size1_ = num1;
        size2_ = num2;
        s1 = s1 > s2 ? s1 : s2;
        if( !base::resize( s1, s1 ) )
        {
            size1_ = size2_ = 0;
            return( false );
        }

        return( true );
    }


 
    bool resize( size_type num1, size_type num2, size_type num3 )
    {
        size_type s1 = floor_square( num1 );
        size_type s2 = floor_square( num2 );
        size_type s3 = floor_square( num3 );
        size1_ = num1;
        size2_ = num2;
        size3_ = num3;
        s1 = s1 > s2 ? s1 : s2;
        s1 = s1 > s3 ? s1 : s3;

        if( !base::resize( s1, s1, s1 ) )
        {
            size1_ = size2_ = size3_ = 0;
            return( false );
        }

        return( true );
    }



    bool swap( buffered_array &a )
    {
        if( base::swap( a ) )
        {
            size_type tmp = size1_;
            size1_ = a.size1_;
            a.size1_ = tmp;

            tmp = size2_;
            size2_ = a.size2_;
            a.size2_ = tmp;

            tmp = size3_;
            size3_ = a.size3_;
            a.size3_ = tmp;

            return( true );
        }
        else
        {
            return( false );
        }
    }


    void clear( )
    {
        base::clear( );
        size1_ = size2_ = size3_ = 0;
    }

    size_type size1( ) const { return( size1_ > 0 ? size1_ : base::size1( ) ); }    
    size_type size2( ) const { return( size2_ > 0 ? size2_ : base::size2( ) ); }    
    size_type size3( ) const { return( size3_ > 0 ? size3_ : base::size3( ) ); }    
    size_type width( ) const { return( size1( ) ); }                                
    size_type height( ) const { return( size2( ) ); }                               
    size_type depth( ) const { return( size3( ) ); }                                


private:

    template < class T, class Allocator >
    const buffered_array& copy( const array< T, Allocator > &o )
    {
        size_type length = base::size( ) < o.size( ) ? base::size( ) : o.size( );
        for( size_type i = 0 ; i < length ; i++ )
        {
            ( *this )[ i ] = static_cast< value_type >( o[ i ] );
        }
        return( *this );
    }


    template < class T, class Allocator >
    const buffered_array& copy( const array1< T, Allocator > &o )
    {
        size_type length = base::size( ) < o.size( ) ? base::size( ) : o.size( );
        for( size_type i = 0 ; i < length ; i++ )
        {
            ( *this )[ i ] = static_cast< value_type >( o[ i ] );
        }
        return( *this );
    }



    template < class T, class Allocator >
    const buffered_array& copy( const array2< T, Allocator > &o )
    {
        size_type w = width( )  < o.width( )  ? width( )  : o.width( );
        size_type h = height( ) < o.height( ) ? height( ) : o.height( );
        for( size_type j = 0 ; j < h ; j++ )
        {
            for( size_type i = 0 ; i < w ; i++ )
            {
                ( *this )( i, j ) = static_cast< value_type >( o( i, j ) );
            }
        }
        return( *this );
    }



    template < class T, class Allocator >
    const buffered_array& copy( const array3< T, Allocator > &o )
    {
        size_type w = width( )  < o.width( )  ? width( )  : o.width( );
        size_type h = height( ) < o.height( ) ? height( ) : o.height( );
        size_type d = depth( )  < o.depth( )  ? depth( )  : o.depth( );
        for( size_type k = 0 ; k < d ; k++ )
        {
            for( size_type j = 0 ; j < h ; j++ )
            {
                for( size_type i = 0 ; i < w ; i++ )
                {
                    ( *this )( i, j, k ) = static_cast< value_type >( o( i, j, k ) );
                }
            }
        }
        return( *this );
    }


public:

    const buffered_array& operator =( const buffered_array &o )
    {
        if( this == &o ) return( *this );

        base::operator =( o );

        if( base::empty( ) )
        {
            size1_ = size2_ = size3_ = 0;
        }
        else
        {
            size1_ = o.size1_;
            size2_ = o.size2_;
            size3_ = o.size3_;
        }

        return( *this );
    }



    template < class T, class Allocator >
    const buffered_array& operator =( const array< T, Allocator > &o )
    {
        resize( o.size( ) );
        return( copy( o ) );
    }



    template < class T, class Allocator >
    const buffered_array& operator =( const array1< T, Allocator > &o )
    {
        resize( o.size( ) );
        reso1( o.reso1( ) );
        return( copy( o ) );
    }



    template < class T, class Allocator >
    const buffered_array& operator =( const array2< T, Allocator > &o )
    {
        resize( o.size1( ), o.size2( ) );
        reso1( o.reso1( ) );
        reso2( o.reso2( ) );
        return( copy( o ) );
    }



    template < class T, class Allocator >
    const buffered_array& operator =( const array3< T, Allocator > &o )
    {
        resize( o.size1( ), o.size2( ), o.size3( ) );
        reso1( o.reso1( ) );
        reso2( o.reso2( ) );
        reso3( o.reso3( ) );
        return( copy( o ) );
    }

public:

    buffered_array( ) : base( ), size1_( 0 ), size2_( 0 ), size3_( 0 ) {}


    buffered_array( const buffered_array &o ) : base( o ), size1_( o.size1( ) ), size2_( o.size2( ) ), size3_( o.size3( ) ) {}


    template < class T, class Allocator >
    buffered_array( const array< T, Allocator > &o )
        : base( floor_square( o.size( ) ) ), size1_( o.size( ) ), size2_( 0 ), size3_( 0 )
    {
        copy( o );
    }


    template < class T, class Allocator >
    buffered_array( const array1< T, Allocator > &o )
        : base( floor_square( o.size( ) ), o.reso1( ) ), size1_( o.size( ) ), size2_( 0 ), size3_( 0 )
    {
        copy( o );
    }


    template < class T, class Allocator >
    buffered_array( const array2< T, Allocator > &o )
        : base( floor_square( o.size1( ) ), floor_square( o.size2( ) ), o.reso1( ), o.reso2( ) ), size1_( o.size1( ) ), size2_( o.size2( ) ), size3_( 0 )
    {
        copy( o );
    }


    template < class T, class Allocator >
    buffered_array( const array3< T, Allocator > &o )
        : base( floor_square( o.size1( ) ), floor_square( o.size2( ) ), floor_square( o.size3( ) ), o.reso1( ), o.reso2( ), o.reso3( ) ),
            size1_( o.size1( ) ), size2_( o.size2( ) ), size3_( o.size3( ) )
    {
        copy( o );
    }
};



template < class T, class Allocator >
inline std::ostream &operator <<( std::ostream &out, const array< T, Allocator > &a )
{
    typename array< T, Allocator >::size_type i;
    for( i = 0 ; i < a.size( ) ; i++ )
    {
        out << a[ i ];
        if( i != a.size1( ) - 1 ) out << ", ";
    }

    return( out );
}


template < class T, class Allocator >
inline std::ostream &operator <<( std::ostream &out, const array1< T, Allocator > &a )
{
    typename array1< T, Allocator >::size_type i;
    for( i = 0 ; i < a.size( ) ; i++ )
    {
        out << a[ i ];
        if( i != a.size1( ) - 1 ) out << ", ";
    }

    return( out );
}



template < class T, class Allocator >
inline std::ostream &operator <<( std::ostream &out, const array2< T, Allocator > &a )
{
    typename array2< T, Allocator >::size_type i, j;
    for( j = 0 ; j < a.size2( ) ; j++ )
    {
        if( j != 0 )
        {
            out << std::endl;
        }
        for( i = 0 ; i < a.size1( ) ; i++ )
        {
            out << a( i, j );
            if( i != a.size1( ) - 1 ) out << ", ";
        }
    }

    return( out );
}



template < class T, class Allocator >
inline std::ostream &operator <<( std::ostream &out, const array3< T, Allocator > &a )
{
    typename array3< T, Allocator >::size_type i, j, k;
    for( k = 0 ; k < a.size3( ) ; k++ )
    {
        for( j = 0 ; j < a.size2( ) ; j++ )
        {
            for( i = 0 ; i < a.size1( ) ; i++ )
            {
                out << a( i, j, k );
                if( i != a.size1( ) - 1 ) out << ", ";
            }
            out << std::endl;
        }
        if( k != a.size3( ) - 1 )
        {
            out << "----- separator -----";
        }
        out << std::endl;
    }

    return( out );
}





#if defined(_ARRAY_BIND_OPERATION_SUPPORT_) && _ARRAY_BIND_OPERATION_SUPPORT_ != 0
#include "operator/operator_array.h"
#endif

#if defined(_ARRAY1_BIND_OPERATION_SUPPORT_) && _ARRAY1_BIND_OPERATION_SUPPORT_ != 0
#include "operator/operator_array1.h"
#endif

#if defined(_ARRAY2_BIND_OPERATION_SUPPORT_) && _ARRAY2_BIND_OPERATION_SUPPORT_ != 0
#include "operator/operator_array2.h"
#endif

#if defined(_ARRAY3_BIND_OPERATION_SUPPORT_) && _ARRAY3_BIND_OPERATION_SUPPORT_ != 0
#include "operator/operator_array3.h"
#endif



_MIST_END


#endif // __INCLUDE_MIST_H__
