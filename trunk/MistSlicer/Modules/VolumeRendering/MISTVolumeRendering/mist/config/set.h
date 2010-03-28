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



#ifndef __INCLUDE_MIST_SET_H__
#define __INCLUDE_MIST_SET_H__


#ifndef __INCLUDE_MIST_CONF_H__
#include "mist_conf.h"
#endif

#include <iostream>
#include <set>
#include <algorithm>


_MIST_BEGIN



template< class SetType >
class set_base : public SetType
{
protected:
    typedef SetType base;           

public:
    typedef typename base::key_type key_type;                   
    typedef typename base::key_compare key_compare;             
    typedef typename base::value_compare value_compare;         
    typedef typename base::allocator_type allocator_type;       
    typedef typename base::size_type size_type;                 
    typedef typename base::difference_type difference_type;     

#if __MIST_MSVC__ != 6
    typedef typename base::pointer pointer;                     
    typedef typename base::const_pointer const_pointer;         
#endif

    typedef typename base::reference reference;                             
    typedef typename base::const_reference const_reference;                 
    typedef typename base::iterator iterator;                               
    typedef typename base::const_iterator const_iterator;                   
    typedef typename base::reverse_iterator reverse_iterator;               
    typedef typename base::const_reverse_iterator const_reverse_iterator;   
    typedef typename base::value_type value_type;                           


public:

    set_base( ) : base( ){ }


    explicit set_base( const key_compare &pred ) : base( pred ){ }


    explicit set_base( const key_type &key ) : base( ){ base::insert( key ); }


    set_base( const key_compare &pred, const allocator_type &alloc ) : base( pred, alloc ){ }


    template< class Iterator >
    set_base( Iterator first, Iterator last ) : base( first, last ){ }



    template< class Iterator >
    set_base( Iterator first, Iterator last, const key_compare &pred ) : base( first, last, pred ){ }



    template< class Iterator >
    set_base( Iterator first, Iterator last, const key_compare &pred, const allocator_type &alloc ) : base( first, last, pred, alloc ){ }

public:

    set_base &operator +=( const set_base &s )
    {
#if __MIST_MSVC__ != 6
        base::insert( s.begin( ), s.end( ) );
#else

        const_iterator site = s.begin( );
        const_iterator eite = s.end( );
        for( ; site != eite ; ++site )
        {
            base::insert( *site );
        }
#endif
        return( *this );
    }


    set_base &operator +=( const key_type &s ){ base::insert( s ); return( *this ); }


 
    set_base &operator -=( const set_base &s )
    {
        if( &s == this )
        {
            base::clear( );
        }
        else
        {
            set_base a = *this;
            const_iterator site1 = a.begin( );
            const_iterator eite1 = a.end( );
            const_iterator site2 = s.begin( );
            const_iterator eite2 = s.end( );


            base::clear( );

            key_compare compare;
            while( site1 != eite1 && site2 != eite2 )
            {
                if( compare( *site1, *site2 ) )
                {
                    base::insert( *site1 );
                    ++site1;
                }
                else if( compare( *site2, *site1 ) )
                {
                    ++site2;
                }
                else
                {
                    ++site1;
                    ++site2;
                }
            }

            if( site1 != eite1 )
            {
#if __MIST_MSVC__ != 6
                base::insert( site1, eite1 );
#else

                for( ; site1 != eite1 ; ++site1 )
                {
                    base::insert( *site1 );
                }
#endif
            }
        }
        return( *this );
    }


    set_base &operator -=( const key_type &s ){ base::erase( s ); return( *this ); }


    set_base &operator *=( const set_base &s )
    {
        if( &s != this )
        {
            set_base a = *this;
            const_iterator site1 = a.begin( );
            const_iterator eite1 = a.end( );
            const_iterator site2 = s.begin( );
            const_iterator eite2 = s.end( );


            base::clear( );

            key_compare compare;
            while( site1 != eite1 && site2 != eite2 )
            {
                if( compare( *site1, *site2 ) )
                {
                    ++site1;
                }
                else if ( compare( *site2, *site1 ) )
                {
                    ++site2;
                }
                else
                {
                    base::insert( *site1 );
                    ++site1;
                    ++site2;
                }
            }
        }
        return( *this );
    }



    set_base &operator *=( const key_type &s )
    {
        set_base a = *this;
        const_iterator site = a.begin( );
        const_iterator eite = a.end( );


        base::clear( );

        key_compare compare;
        while( site != eite )
        {
            if( compare( *site, s ) )
            {
                ++site;
            }
            else if ( compare( s, *site ) )
            {
                break;
            }
            else
            {
                base::insert( *site );
                ++site;
            }
        }
        return( *this );
    }



    set_base &operator <<=( const set_base &s ){ return( operator +=( s ) ); }


    set_base &operator <<=( const key_type &s ){ return( operator +=( s ) ); }



    set_base &operator <<( const key_type &s ) const { return( set_base( *this )+=( s ) ); }



    bool operator ==( const set_base &s ) const
    {
        if( base::size( ) != s.size( ) )
        {
            return( false );
        }

        key_compare compare;
        const_iterator ite1 = base::begin( );
        const_iterator eite1 = base::end( );
        const_iterator ite2 = s.begin( );
        for( ; ite1 != eite1 ; ++ite1, ++ite2 )
        {
            if( compare( *ite1, *ite2 ) || compare( *ite2, *ite1 ) )
            {
                return( false );
            }
        }
        return( true );
    }



    bool operator ==( const key_type &s ) const
    {
        if( base::size( ) != 1 )
        {
            return( false );
        }

        key_compare compare;
        const_iterator ite = base::begin( );
        const_iterator eite = base::end( );
        for( ; ite != eite ; ++ite )
        {
            if( compare( *ite, s ) || compare( s, *ite ) )
            {
                return( false );
            }
        }
        return( true );
    }



    bool operator !=( const set_base &s ) const { return( !( *this == s ) ); }


    bool operator !=( const key_type &s ) const { return( !( *this == s ) ); }


    bool operator < ( const set_base &s ) const
    {
        if( base::size( ) >= s.size( ) )
        {
            return( false );
        }

        return( std::includes( s.begin( ), s.end( ), base::begin( ), base::end( ), key_compare( ) ) );
    }


    bool operator <=( const set_base &s ) const
    {
        if( base::size( ) > s.size( ) )
        {
            return( false );
        }

        return( std::includes( s.begin( ), s.end( ), base::begin( ), base::end( ), key_compare( ) ) );
    }


    bool operator > ( const set_base &s ) const { return( s < *this ); }


    bool operator >=( const set_base &s ) const { return( s <= *this ); }
};



template< class SetType > inline const set_base< SetType > operator +( const set_base< SetType > &s1, const set_base< SetType > &s2 ){ return( set_base< SetType >( s1 ) += s2 ); }


template< class SetType > inline const set_base< SetType > operator -( const set_base< SetType > &s1, const set_base< SetType > &s2 ){ return( set_base< SetType >( s1 ) -= s2 ); }


template< class SetType > inline const set_base< SetType > operator *( const set_base< SetType > &s1, const set_base< SetType > &s2 ){ return( set_base< SetType >( s1 ) *= s2 ); }


//inline const set_base operator /( const set_base &s1, const set_base &s2 ){ return( set_base( s1 ) /= s2 ); }
//inline const set_base operator %( const set_base &s1, const set_base &s2 ){ return( set_base( s1 ) %= s2 ); }
//inline const set_base operator |( const set_base &s1, const set_base &s2 ){ return( set_base( s1 ) |= s2 ); }
//inline const set_base operator &( const set_base &s1, const set_base &s2 ){ return( set_base( s1 ) &= s2 ); }
//inline const set_base operator ^( const set_base &s1, const set_base &s2 ){ return( set_base( s1 ) ^= s2 ); }




template< class SetType > inline const set_base< SetType > operator *( const set_base< SetType > &s1, const typename set_base< SetType >::key_type &s2 ){ return( set_base< SetType >( s1 ) *= s2 ); }


template< class SetType > inline const set_base< SetType > operator *( const typename set_base< SetType >::key_type &s1, const set_base< SetType > &s2 ){ return( set_base< SetType >( s1 ) *= s2 ); }


template< class SetType > inline const set_base< SetType > operator +( const set_base< SetType > &s1, const typename set_base< SetType >::key_type &s2 ){ return( set_base< SetType >( s1 ) += s2 ); }


template< class SetType > inline const set_base< SetType > operator +( const typename set_base< SetType >::key_type &s1, const set_base< SetType > &s2 ){ return( set_base< SetType >( s2 ) += s1 ); }


template< class SetType > inline const set_base< SetType > operator -( const set_base< SetType > &s1, const typename set_base< SetType >::key_type &s2 ){ return( set_base< SetType >( s1 ) -= s2 ); }


template< class SetType > inline const set_base< SetType > operator -( const typename set_base< SetType >::key_type &s1, const set_base< SetType > &s2 ){ return( set_base< SetType >( s1 ) -= s2 ); }




template< class SetType > bool operator ==( const set_base< SetType > &s1, const typename set_base< SetType >::key_type &s2 ){ return( set_base< SetType >( s2 ) == s1 ); }


template< class SetType > bool operator !=( const set_base< SetType > &s1, const typename set_base< SetType >::key_type &s2 ){ return( set_base< SetType >( s2 ) != s1 ); }


template< class SetType > bool operator < ( const set_base< SetType > &s1, const typename set_base< SetType >::key_type &s2 ){ return( s1 <  set_base< SetType >( s2 ) ); }


template< class SetType > bool operator <=( const set_base< SetType > &s1, const typename set_base< SetType >::key_type &s2 ){ return( s1 <= set_base< SetType >( s2 ) ); }


template< class SetType > bool operator > ( const set_base< SetType > &s1, const typename set_base< SetType >::key_type &s2 ){ return( s1 >  set_base< SetType >( s2 ) ); }


template< class SetType > bool operator >=( const set_base< SetType > &s1, const typename set_base< SetType >::key_type &s2 ){ return( s1 >= set_base< SetType >( s2 ) ); }




template< class SetType > bool operator ==( const typename set_base< SetType >::key_type &s1, const set_base< SetType > &s2 ){ return( set_base< SetType >( s1 ) == s2 ); }


template< class SetType > bool operator !=( const typename set_base< SetType >::key_type &s1, const set_base< SetType > &s2 ){ return( set_base< SetType >( s1 ) != s2 ); }


template< class SetType > bool operator < ( const typename set_base< SetType >::key_type &s1, const set_base< SetType > &s2 ){ return( set_base< SetType >( s1 ) <  s2 ); }


template< class SetType > bool operator <=( const typename set_base< SetType >::key_type &s1, const set_base< SetType > &s2 ){ return( set_base< SetType >( s1 ) <= s2 ); }


template< class SetType > bool operator > ( const typename set_base< SetType >::key_type &s1, const set_base< SetType > &s2 ){ return( set_base< SetType >( s1 ) >  s2 ); }


template< class SetType > bool operator >=( const typename set_base< SetType >::key_type &s1, const set_base< SetType > &s2 ){ return( set_base< SetType >( s1 ) >= s2 ); }



template< class SetType >
inline std::ostream &operator <<( std::ostream &out, const set_base< SetType > &s )
{
    typename set_base< SetType >::const_iterator ite = s.begin( );
    if( ite != s.end( ) )
    {
        out << *ite++;
    }

    for( ; ite != s.end( ) ; ++ite )
    {
        out << ", " << *ite;
    }
    return( out );
}




template< class Key, class Comp = std::less< Key >, class Allocator = std::allocator< Key > >
class set : public set_base< std::set< Key, Comp, Allocator > >
{
protected:
    typedef set_base< std::set< Key, Comp, Allocator > > base;      

public:
    typedef typename base::key_type key_type;                   
    typedef typename base::key_compare key_compare;             
    typedef typename base::value_compare value_compare;         
    typedef typename base::allocator_type allocator_type;       
    typedef typename base::size_type size_type;                 
    typedef typename base::difference_type difference_type;     

#if __MIST_MSVC__ != 6
    typedef typename base::pointer pointer;                     
    typedef typename base::const_pointer const_pointer;         
#endif

    typedef typename base::reference reference;                             
    typedef typename base::const_reference const_reference;                 
    typedef typename base::iterator iterator;                               
    typedef typename base::const_iterator const_iterator;                   
    typedef typename base::reverse_iterator reverse_iterator;               
    typedef typename base::const_reverse_iterator const_reverse_iterator;   
    typedef typename base::value_type value_type;                           

public:

    set( ) : base( ){ }


    explicit set( const key_compare &pred ) : base( pred ){ }


    explicit set( const key_type &key ) : base( ){ base::insert( key ); }


    set( const key_compare &pred, const allocator_type &alloc ) : base( pred, alloc ){ }


    template< class Iterator >
    set( Iterator first, Iterator last ) : base( first, last ){ }


    template< class Iterator >
    set( Iterator first, Iterator last, const key_compare &pred ) : base( first, last, pred ){ }


    template< class Iterator >
    set( Iterator first, Iterator last, const key_compare &pred, const allocator_type &alloc ) : base( first, last, pred, alloc ){ }
};



template< class Key, class Comp = std::less< Key >, class Allocator = std::allocator< Key > >
class multiset : public set_base< std::multiset< Key, Comp, Allocator > >
{
protected:
    typedef set_base< std::multiset< Key, Comp, Allocator > > base;

public:
    typedef typename base::key_type key_type;                   
    typedef typename base::key_compare key_compare;             
    typedef typename base::value_compare value_compare;         
    typedef typename base::allocator_type allocator_type;       
    typedef typename base::size_type size_type;                 
    typedef typename base::difference_type difference_type;     

#if __MIST_MSVC__ != 6
    typedef typename base::pointer pointer;                     
    typedef typename base::const_pointer const_pointer;         
#endif

    typedef typename base::reference reference;                             
    typedef typename base::const_reference const_reference;                 
    typedef typename base::iterator iterator;                               
    typedef typename base::const_iterator const_iterator;                   
    typedef typename base::reverse_iterator reverse_iterator;               
    typedef typename base::const_reverse_iterator const_reverse_iterator;   
    typedef typename base::value_type value_type;                           

public:

    multiset( ) : base( ){ }


    explicit multiset( const key_compare &pred ) : base( pred ){ }


    explicit multiset( const key_type &key ) : base( ){ base::insert( key ); }


    multiset( const key_compare &pred, const allocator_type &alloc ) : base( pred, alloc ){ }


    template< class Iterator >
    multiset( Iterator first, Iterator last ) : base( first, last ){ }


    template< class Iterator >
    multiset( Iterator first, Iterator last, const key_compare &pred ) : base( first, last, pred ){ }


    template< class Iterator >
    multiset( Iterator first, Iterator last, const key_compare &pred, const allocator_type &alloc ) : base( first, last, pred, alloc ){ }
};




_MIST_END


#endif // __INCLUDE_MIST_SET_H__