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



#ifndef __INCLUDE_MIST_ITERATOR_H__
#define __INCLUDE_MIST_ITERATOR_H__


#include <iterator>

#ifndef __INCLUDE_MIST_CONF_H__
#include "mist_conf.h"
#endif


_MIST_BEGIN




template< class T, class Distance = ptrdiff_t, class Pointer = T*, class Reference = T& >
#if defined( __MIST_MSVC__ ) && __MIST_MSVC__ <= 6
class mist_iterator1 : public std::iterator< std::random_access_iterator_tag, T, Distance >
#else
class mist_iterator1 : public std::iterator< std::random_access_iterator_tag, T, Distance, Pointer, Reference >
#endif
{
public:
    typedef T value_type;                   
    typedef Pointer pointer;                
    typedef Reference reference;            
    typedef size_t size_type;               
    typedef Distance difference_type;       
    typedef Reference const_reference;      

private:
    pointer data_;                  
    size_type diff_pointer_;        

public:

    mist_iterator1( pointer p = NULL, size_type diff = 1 ) : data_( p ), diff_pointer_( diff ){ }


    template< class TT, class DD, class PP, class RR >
    mist_iterator1( const mist_iterator1< TT, DD, PP, RR > &ite ) : data_( ite.data( ) ), diff_pointer_( ite.diff( ) ){ }



    template< class TT, class DD, class PP, class RR >
    const mist_iterator1& operator =( const mist_iterator1< TT, DD, PP, RR > &ite )
    {
        data_ = ite.data( );
        diff_pointer_ = ite.diff( );
        return( *this );
    }



    reference operator *(){ return( *data_ ); }


    const_reference operator *() const { return( *data_ ); }


    pointer operator->( ){ return( &**this ); }


    const pointer operator->( ) const { return( &**this ); }


    reference operator []( difference_type dist ){ return( data_[ dist * diff_pointer_ ] ); }


    const_reference operator []( difference_type dist ) const { return( data_[ dist * diff_pointer_ ] ); }


    pointer data( )
    {
        return( data_ );
    }


    const pointer data( ) const
    {
        return( data_ );
    }


    size_type diff( ) const
    {
        return( diff_pointer_ );
    }


    mist_iterator1& operator ++( )
    {
        *this += 1;
        return( *this );
    }


    const mist_iterator1 operator ++( int )
    {
        mist_iterator1 old_val( *this );
        *this += 1;
        return( old_val );
    }


    mist_iterator1& operator --( )
    {
        *this -= 1;
        return( *this );
    }


    const mist_iterator1 operator --( int )
    {
        mist_iterator1 old_val( *this );
        *this -= 1;
        return( old_val );
    }



    const mist_iterator1& operator +=( difference_type dist )
    {
        data_ += dist * diff_pointer_;
        return( *this );
    }


    const mist_iterator1& operator -=( difference_type dist )
    {
        data_ -= dist * diff_pointer_;
        return( *this );
    }



    template< class TT, class DD, class PP, class RR >
    const difference_type operator -( const mist_iterator1< TT, DD, PP, RR > &ite ) const
    {
        return( ( data_ - ite.data( ) ) / diff_pointer_ );
    }



    template< class TT, class DD, class PP, class RR >
    bool operator ==( const mist_iterator1< TT, DD, PP, RR > &ite ) const { return( data_ == ite.data( ) ); }


    template< class TT, class DD, class PP, class RR >
    bool operator !=( const mist_iterator1< TT, DD, PP, RR > &ite ) const { return( !( *this == ite )  ); }


    template< class TT, class DD, class PP, class RR >
    bool operator < ( const mist_iterator1< TT, DD, PP, RR > &ite ) const { return( data_ <  ite.data( ) ); }


    template< class TT, class DD, class PP, class RR >
    bool operator <=( const mist_iterator1< TT, DD, PP, RR > &ite ) const { return( data_ <= ite.data( ) ); }


    template< class TT, class DD, class PP, class RR >
    bool operator > ( const mist_iterator1< TT, DD, PP, RR > &ite ) const { return( data_ >  ite.data( ) ); }


    template< class TT, class DD, class PP, class RR >
    bool operator >=( const mist_iterator1< TT, DD, PP, RR > &ite ) const { return( data_ >= ite.data( ) ); }
};



template< class T, class Distance, class Pointer, class Reference >
inline const mist_iterator1< T, Distance, Pointer, Reference > operator +( const mist_iterator1< T, Distance, Pointer, Reference > &ite, typename mist_iterator1< T, Distance, Pointer, Reference >::difference_type dist )
{
    return( mist_iterator1< T, Distance, Pointer, Reference >( ite ) += dist );
}


template< class T, class Distance, class Pointer, class Reference >
inline const mist_iterator1< T, Distance, Pointer, Reference > operator +( typename mist_iterator1< T, Distance, Pointer, Reference >::difference_type dist, const mist_iterator1< T, Distance, Pointer, Reference > &ite )
{
    return( mist_iterator1< T, Distance, Pointer, Reference >( ite ) += dist );
}


template< class T, class Distance, class Pointer, class Reference >
inline const mist_iterator1< T, Distance, Pointer, Reference > operator -( const mist_iterator1< T, Distance, Pointer, Reference > &ite, typename mist_iterator1< T, Distance, Pointer, Reference >::difference_type dist )
{
    return( mist_iterator1< T, Distance, Pointer, Reference >( ite ) -= dist );
}



template< class T, class Distance = ptrdiff_t, class Pointer = T*, class Reference = T& >
#if defined( __MIST_MSVC__ ) && __MIST_MSVC__ <= 6
class mist_iterator2 : public std::iterator< std::random_access_iterator_tag, T, Distance >
#else
class mist_iterator2 : public std::iterator< std::random_access_iterator_tag, T, Distance, Pointer, Reference >
#endif
{
public:
    typedef T value_type;                   
    typedef Pointer pointer;                
    typedef Reference reference;            
    typedef size_t size_type;               
    typedef Distance difference_type;       
    typedef Reference const_reference;      

private:
    pointer data_;              
    difference_type index_;     
    difference_type width_;     
    difference_type step_;      

public:

    mist_iterator2( pointer p = NULL, difference_type index = 0, difference_type width = 1, difference_type step = 1 )
                                                : data_( p ), index_( index ), width_( width ), step_( step )
    {
    }


    template< class TT, class DD, class PP, class RR >
    mist_iterator2( const mist_iterator2< TT, DD, PP, RR > &ite ) : data_( ite.data( ) ), index_( ite.index( ) ), width_( ite.width( ) ), step_( ite.step( ) )
    {
    }



    template< class TT, class DD, class PP, class RR >
    const mist_iterator2& operator =( const mist_iterator2< TT, DD, PP, RR > &ite )
    {
        if( &ite != this )
        {
            data_  = ite.data( );
            index_ = ite.index( );
            width_ = ite.width( );
            step_  = ite.step( );
        }
        return( *this );
    }


    pointer data( )
    {
        return( data_ );
    }


    const pointer data( ) const
    {
        return( data_ );
    }


    difference_type index( ) const
    {
        return( index_ );
    }


    difference_type width( ) const
    {
        return( width_ );
    }


    difference_type step( ) const
    {
        return( step_ );
    }


    reference operator *( )
    {
        difference_type step = index_ / width_;
        difference_type rest = index_ - step * width_;
        return( *( data_ + rest + step * step_ ) );
    }


    const_reference operator *( ) const
    {
        difference_type step = index_ / width_;
        difference_type rest = index_ - step * width_;
        return( *( data_ + rest + step * step_ ) );
    }


    pointer operator->( ){ return( &**this ); }


    const pointer operator->( ) const { return( &**this ); }


    reference operator []( difference_type dist ){ return( *( *this += dist ) ); }


    const_reference operator []( difference_type dist ) const { return( *( *this += dist ) ); }



    mist_iterator2& operator ++( )
    {
        *this += 1;
        return( *this );
    }


    const mist_iterator2 operator ++( int )
    {
        mist_iterator2 old_val( *this );
        *this += 1;
        return( old_val );
    }


    mist_iterator2& operator --( )
    {
        *this -= 1;
        return( *this );
    }


    const mist_iterator2 operator --( int )
    {
        mist_iterator2 old_val( *this );
        *this -= 1;
        return( old_val );
    }


    const mist_iterator2& operator +=( difference_type dist )
    {
        index_ += dist;
        return( *this );
    }


    const mist_iterator2& operator -=( difference_type dist )
    {
        index_ -= dist;
        return( *this );
    }



    template< class TT, class DD, class PP, class RR >
    const difference_type operator -( const mist_iterator2< TT, DD, PP, RR > &ite ) const
    {
        return( index_ - ite.index( ) );
    }


    template< class TT, class DD, class PP, class RR >
    bool operator ==( const mist_iterator2< TT, DD, PP, RR > &ite ) const { return( *this - ite == 0 ); }

    
    template< class TT, class DD, class PP, class RR >
    bool operator !=( const mist_iterator2< TT, DD, PP, RR > &ite ) const { return( !( *this == ite ) ); }

    
    template< class TT, class DD, class PP, class RR >
    bool operator < ( const mist_iterator2< TT, DD, PP, RR > &ite ) const { return( *this - ite < 0 ); }

    
    template< class TT, class DD, class PP, class RR >
    bool operator <=( const mist_iterator2< TT, DD, PP, RR > &ite ) const { return( !( *this > ite ) ); }

    
    template< class TT, class DD, class PP, class RR >
    bool operator > ( const mist_iterator2< TT, DD, PP, RR > &ite ) const { return( ite < *this ); }

    
    template< class TT, class DD, class PP, class RR >
    bool operator >=( const mist_iterator2< TT, DD, PP, RR > &ite ) const { return( !( *this < ite ) ); }
};




template< class T, class Distance, class Pointer, class Reference >
inline const mist_iterator2< T, Distance, Pointer, Reference > operator +( const mist_iterator2< T, Distance, Pointer, Reference > &ite, typename mist_iterator2< T, Distance, Pointer, Reference >::difference_type dist )
{
    return( mist_iterator2< T, Distance, Pointer, Reference >( ite ) += dist );
}


template< class T, class Distance, class Pointer, class Reference >
inline const mist_iterator2< T, Distance, Pointer, Reference > operator +( typename mist_iterator2< T, Distance, Pointer, Reference >::difference_type dist, const mist_iterator2< T, Distance, Pointer, Reference > &ite )
{
    return( mist_iterator2< T, Distance, Pointer, Reference >( ite ) += dist );
}


template< class T, class Distance, class Pointer, class Reference >
inline const mist_iterator2< T, Distance, Pointer, Reference > operator -( const mist_iterator2< T, Distance, Pointer, Reference > &ite, typename mist_iterator2< T, Distance, Pointer, Reference >::difference_type dist )
{
    return( mist_iterator2< T, Distance, Pointer, Reference >( ite ) -= dist );
}




template< class T >
class mist_reverse_iterator :
#if defined( __MIST_MSVC__ ) && __MIST_MSVC__ <= 6
    public std::iterator<
        typename T::iterator_category,
        typename T::value_type,
        typename T::difference_type
    >
#else
    public std::iterator<
        typename T::iterator_category,
        typename T::value_type,
        typename T::difference_type,
        typename T::pointer,
        typename T::reference
    >
#endif
{
private:
    typedef T iterator_type;

public:
    typedef typename T::size_type size_type;                    
    typedef typename T::difference_type difference_type;        
    typedef typename T::value_type value_type;                  
    typedef typename T::pointer pointer;                        
    typedef typename T::reference reference;                    
    typedef typename T::const_reference const_reference;        

protected:
    iterator_type current_iterator_;        

public:
    
    mist_reverse_iterator( ){ }

    
    mist_reverse_iterator( const iterator_type &ite ) : current_iterator_( ite ){ }

    
    mist_reverse_iterator( const mist_reverse_iterator &ite ) : current_iterator_( ite.current_iterator_ ){ }


    
    const mist_reverse_iterator& operator =( const iterator_type &ite )
    {
        current_iterator_ = ite;
        return( *this );
    }

    
    const mist_reverse_iterator& operator =( const mist_reverse_iterator &ite )
    {
        current_iterator_ = ite.current_iterator_;
        return( *this );
    }


    
    reference operator *()
    {
        iterator_type _tmp = current_iterator_;
        return( *( --_tmp ) );
    }

    
    const_reference operator *() const
    {
        iterator_type _tmp = current_iterator_;
        return( *( --_tmp ) );
    }

    
    pointer operator->( ){ return( &**this ); }

    
    const pointer operator->( ) const { return( &**this ); }

    
    reference operator []( difference_type dist ){ return( *( *this + dist ) ); }

    
    const_reference operator []( difference_type dist ) const { return( *( *this + dist ) ); }


    
    mist_reverse_iterator& operator ++( )
    {
        --current_iterator_;
        return( *this );
    }

    
    const mist_reverse_iterator operator ++( int )
    {
        mist_reverse_iterator old_val( *this );
        current_iterator_--;
        return( old_val );
    }

    
    mist_reverse_iterator& operator --( )
    {
        ++current_iterator_;
        return( *this );
    }

    
    const mist_reverse_iterator operator --( int )
    {
        mist_reverse_iterator old_val( *this );
        current_iterator_++;
        return( old_val );
    }

    
    const mist_reverse_iterator& operator +=( difference_type dist )
    {
        current_iterator_ -= dist;
        return( *this );
    }

    
    const mist_reverse_iterator& operator -=( difference_type dist )
    {
        current_iterator_ += dist;
        return( *this );
    }

    
    const difference_type operator -( const mist_reverse_iterator &ite ) const
    {
        return( ite.current_iterator_ - current_iterator_ );
    }



    
    bool operator ==( const mist_reverse_iterator &ite ) const { return( current_iterator_ == ite.current_iterator_ ); }

    
    bool operator !=( const mist_reverse_iterator &ite ) const { return( !( *this == ite ) ); }

    
    bool operator < ( const mist_reverse_iterator &ite ) const { return( ite.current_iterator_ < current_iterator_  ); }

    
    bool operator <=( const mist_reverse_iterator &ite ) const { return( !( *this > ite ) ); }

    
    bool operator > ( const mist_reverse_iterator &ite ) const { return( ite < *this ); }

    
    bool operator >=( const mist_reverse_iterator &ite ) const { return( !( *this < ite ) ); }
};



template< class T >
inline const mist_reverse_iterator< T > operator +( const mist_reverse_iterator< T > &ite1, const mist_reverse_iterator< T > ite2 )
{
    return( mist_reverse_iterator< T >( ite1 ) += ite2 );
}


template< class T >
inline const mist_reverse_iterator< T > operator +( const mist_reverse_iterator< T > &ite, typename T::difference_type dist )
{
    return( mist_reverse_iterator< T >( ite ) += dist );
}


template< class T >
inline const mist_reverse_iterator< T > operator +( typename T::difference_type dist, const mist_reverse_iterator< T > &ite )
{
    return( mist_reverse_iterator< T >( ite ) += dist );
}


//template< class T >
//inline const typename mist_reverse_iterator< T >::difference_type operator -( const mist_reverse_iterator< T > &ite1, const mist_reverse_iterator< T > ite2 )
//{
//  return( ite1 - ite2 );
//}



template< class T >
inline const mist_reverse_iterator< T > operator -( const mist_reverse_iterator< T > &ite, typename T::difference_type dist )
{
    return( mist_reverse_iterator< T >( ite ) -= dist );
}


_MIST_END


#endif // __INCLUDE_MIST_ITERATOR_H__
