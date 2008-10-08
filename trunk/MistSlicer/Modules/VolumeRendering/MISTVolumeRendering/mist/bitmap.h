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


#ifndef __INCLUDE_BITMAP_H__
#define __INCLUDE_BITMAP_H__


#ifndef __INCLUDE_MIST_CONF_H__
#include "config/mist_conf.h"
#endif

#ifndef __INCLUDE_MIST_H__
#include "mist.h"
#endif

#ifndef __INCLUDE_MIST_COLOR_H__
#include "config/color.h"
#endif


_MIST_BEGIN

template < size_t BITS >
struct bitmap_type
{
    typedef bgr< unsigned char > value_type;
};

template < >
struct bitmap_type< 8 >
{
    typedef unsigned char value_type;
};



template < size_t BITS = 24, class Allocator = std::allocator< unsigned char > >
class bitmap : public array< unsigned char, Allocator >
{
public:
    typedef typename bitmap_type< BITS >::value_type value_type;    
    typedef value_type* pointer;                                    
    typedef value_type& reference;                                  
    typedef const value_type& const_reference;                      
    typedef const value_type* const_pointer;                        
    typedef typename Allocator::size_type size_type;                
    typedef typename Allocator::difference_type difference_type;    


protected:
    typedef array< unsigned char, Allocator > base;     
    size_type size1_;                                   
    size_type size2_;                                   
    size_type nbytes_;                                  

    
    static size_type pixel_bytes( )
    {
        return( BITS / 8 );
    }

    
    static size_type num_bytes( size_type width )
    {
        width = width * pixel_bytes( );
        if( width % 4 > 0 )
        {
            width += 4 - ( width % 4 );
        }
        return( width );
    }

public:
    
    bool resize( size_type num1, size_type num2, size_type dmy1 = 0 )
    {
        nbytes_ = num_bytes( num1 );
        if( base::resize( nbytes_ * num2 ) )
        {
            size1_ = num1;
            size2_ = num2;
            return( true );
        }
        else
        {
            size1_ = size2_ = nbytes_ = 0;
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
            
            bitmap o( *this );
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
            bitmap o( w, h );
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



    bool swap( bitmap &a )
    {
        if( base::swap( a ) )
        {
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

    size_type size( ) const { return( size1_ * size2_ ); }          

public:
    
    template < size_t BBITS, class AAlocator >
    const bitmap& operator =( const bitmap< BBITS, AAlocator > &o )
    {
        if( resize( o.size1( ), o.size2( ) ) )
        {
            for( size_type j = 0 ; j < size2_ ; j++ )
            {
                for( size_type i = 0 ; i < size1_ ; i++ )
                {
                    operator ()( i, j ) = o( i, j );
                }
            }
        }

        return( *this );
    }



    template < class TT, class AAlocator >
    const bitmap& operator =( const array2< TT, AAlocator > &o )
    {
        if( resize( o.size1( ), o.size2( ) ) )
        {
            for( size_type j = 0 ; j < size2_ ; j++ )
            {
                for( size_type i = 0 ; i < size1_ ; i++ )
                {
                    operator ()( i, j ) = o( i, j );
                }
            }
        }

        return( *this );
    }



    const bitmap& operator =( const bitmap &o )
    {
        if( this == &o ) return( *this );

        base::operator =( o );
        size1_ = o.size1_;
        size2_ = o.size2_;

        return( *this );
    }


protected:

    pointer paccess( size_type i, size_type j )
    {
        return( reinterpret_cast< pointer >( base::data_ + i * pixel_bytes( ) + j * nbytes_ ) );
    }


    const_pointer paccess( size_type i, size_type j ) const
    {
        return( reinterpret_cast< const_pointer >( base::data_ + i * pixel_bytes( ) + j * nbytes_ ) );
    }

public:

    reference operator []( size_type index )
    {
        _CHECK_ACCESS_VIOLATION1_( index )
        size_type j = index / size1_;
        size_type i = index - j * size1_;
        return( reinterpret_cast< reference >( base::data_[ i * pixel_bytes( ) + j * nbytes_ ] ) );
    }



    reference operator []( size_type index ) const
    {
        _CHECK_ACCESS_VIOLATION1_( index )
        size_type j = index / size1_;
        size_type i = index - j * size1_;
        return( reinterpret_cast< reference >( base::data_[ i * pixel_bytes( ) + j * nbytes_ ] ) );
    }



    reference at( size_type i, size_type j, size_type dmy = 0 )
    {
        _CHECK_ACCESS_VIOLATION2_( i, j )
        return( reinterpret_cast< reference >( base::data_[ i * pixel_bytes( ) + j * nbytes_ ] ) );
    }


    const_reference at( size_type i, size_type j, size_type dmy = 0 ) const
    {
        _CHECK_ACCESS_VIOLATION2_( i, j )
        return( reinterpret_cast< reference >( base::data_[ i * pixel_bytes( ) + j * nbytes_ ] ) );
    }



    reference operator ()( size_type i, size_type j, size_type dmy = 0 )
    {
        _CHECK_ACCESS_VIOLATION2_( i, j )
        return( reinterpret_cast< reference >( base::data_[ i * pixel_bytes( ) + j * nbytes_ ] ) );
    }



    const_reference operator ()( size_type i, size_type j, size_type dmy = 0 ) const
    {
        _CHECK_ACCESS_VIOLATION2_( i, j )
        return( reinterpret_cast< reference >( base::data_[ i * pixel_bytes( ) + j * nbytes_ ] ) );
    }


public:

    bitmap( ) : base( ), size1_( 0 ), size2_( 0 ), nbytes_( 0 ) {}


    explicit bitmap( const Allocator &a ) : base( a ), size1_( 0 ), size2_( 0 ), nbytes_( 0 ) {}


    bitmap( size_type num1, size_type num2 ) : base( num_bytes( num1 ) * num2 ), size1_( num1 ), size2_( num2 ), nbytes_( num_bytes( num1 ) )
    {
        if( base::empty( ) ) size1_ = size2_ = nbytes_ = 0;
    }


    bitmap( size_type num1, size_type num2, const Allocator &a ) : base( num_bytes( num1 ) * num2, a ), size1_( num1 ), size2_( num2 ), nbytes_( num_bytes( num1 ) )
    {
        if( base::empty( ) ) size1_ = size2_ = nbytes_ = 0;
    }



    bitmap( size_type num1, size_type num2, const value_type &val ) : base( num_bytes( num1 ) * num2, val ), size1_( num1 ), size2_( num2 ), nbytes_( num_bytes( num1 ) )
    {
        if( base::empty( ) ) size1_ = size2_ = nbytes_ = 0;
    }


    bitmap( size_type num1, size_type num2, const value_type &val, const Allocator &a ) : base( num_bytes( num1 ) * num2, val, a ), size1_( num1 ), size2_( num2 ), nbytes_( num_bytes( num1 ) )
    {
        if( base::empty( ) ) size1_ = size2_ = nbytes_ = 0;
    }



    bitmap( size_type num1, size_type num2, void *ptr, size_type mem_available ) : base( num_bytes( num1 ) * num2, reinterpret_cast< unsigned char * >( ptr ), mem_available ), size1_( num1 ), size2_( num2 ), nbytes_( num_bytes( num1 ) )
    {
        if( base::empty( ) ) size1_ = size2_ = nbytes_ = 0;
    }


    bitmap( size_type num1, size_type num2, const value_type &val, void *ptr, size_type mem_available ) : base( num_bytes( num1 ) * num2, reinterpret_cast< unsigned char * >( ptr ), mem_available ), size1_( num1 ), size2_( num2 ), nbytes_( num_bytes( num1 ) )
    {
        if( base::empty( ) )
        {
            size1_ = size2_ = nbytes_ = 0;
        }
        else
        {
            for( size_type j = 0 ; j < size2_ ; j++ )
            {
                for( size_type i = 0 ; i < size1_ ; i++ )
                {
                    operator ()( i, j ) = val;
                }
            }
        }
    }



    template < size_t BBITS, class AAlocator >
    bitmap( const bitmap< BBITS, AAlocator > &o ) : base( ), size1_( 0 ), size2_( 0 ), nbytes_( 0 )
    {
        if( resize( o.size1( ), o.size2( ) ) )
        {
            for( size_type j = 0 ; j < size2( ) ; j++ )
            {
                for( size_type i = 0 ; i < size1( ) ; i++ )
                {
                    operator ()( i, j ) = o( i, j );
                }
            }
        }
    }


    template < class TT, class AAlocator >
    bitmap( const array2< TT, AAlocator > &o ) : base( ), size1_( 0 ), size2_( 0 ), nbytes_( 0 )
    {
        if( resize( o.size1( ), o.size2( ) ) )
        {
            for( size_type j = 0 ; j < size2( ) ; j++ )
            {
                for( size_type i = 0 ; i < size1( ) ; i++ )
                {
                    operator ()( i, j ) = o( i, j );
                }
            }
        }
    }

    bitmap( const bitmap< BITS, Allocator > &o ) : base( o ), size1_( o.size1_ ), size2_( o.size2_ ), nbytes_( o.nbytes_ ) {}
};




template < size_t BITS, class Allocator >
inline std::ostream &operator <<( std::ostream &out, const bitmap< BITS, Allocator > &a )
{
    typename bitmap< BITS, Allocator >::size_type i, j;
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




_MIST_END


#endif // __INCLUDE_MIST_H__
