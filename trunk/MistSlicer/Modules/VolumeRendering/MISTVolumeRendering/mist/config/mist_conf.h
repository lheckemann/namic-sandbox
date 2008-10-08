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



#ifndef __INCLUDE_MIST_CONF_H__
#define __INCLUDE_MIST_CONF_H__

#include <iostream>



#ifndef _MIST_BEGIN

#define _MIST_BEGIN     namespace mist{     
#endif

#ifndef _MIST_END
#define _MIST_END       }                   
#endif



_MIST_BEGIN




#if defined( _MSC_VER )
    #if _MSC_VER <= 1200
        // Visual C++6.0
        #define __MIST_MSVC__       6
    #elif _MSC_VER < 1400
        // Visual Studio .NET 2002 - 2003バージョン
        #define __MIST_MSVC__       7
    #else
        // Visual Studio 2005 バージョン
        #define __MIST_MSVC__       8
    #endif
#endif


#if defined( __MIST_MSVC__ ) && __MIST_MSVC__ >= 8



    #pragma warning( disable : 4996 )

    //#undef _CRT_SECURE_CPP_OVERLOAD_SECURE_NAMES
    //#define _CRT_SECURE_CPP_OVERLOAD_SECURE_NAMES     0
    ////#define _CRT_SECURE_NO_DEPRECATE                    1   
    //#undef _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES
    //#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES       1   
#endif


#if defined( WIN64 ) || defined( _WIN64 )
    // 64bit Windows
    #define __MIST_WINDOWS__    2
#elif defined( WIN32 ) || defined( _WIN32 )
    // 32bit Windows
    #define __MIST_WINDOWS__    1
#endif

#if defined( _WIN64 ) || defined( __x86_64__ ) || defined( __AMD64__ )
    // 64bit OS
    #define __MIST64__  64
#else
    // 32bit OS
    #define __MIST32__  32
#endif




#if defined( __MIST_MSVC__ ) && __MIST_MSVC__ <= 6
    #define _MIST_CONST( type, name, value ) enum{ name = value }
#else
    #define _MIST_CONST( type, name, value ) static const type name = value
#endif



#if defined(__MIST_MSVC__) || defined(__INTEL_COMPILER)
    #define _MIST_PACKED
#else
    #define _MIST_PACKED __attribute__( ( packed ) )
#endif




#define _MIST_ALLOCATE_TEST_            0

//#ifdef __MIST_MSVC__
//  #define _MIST_ALLOCATOR_MEMORY_TRIM_    0   
//#else
//  #define _MIST_ALLOCATOR_MEMORY_TRIM_    1   
//#endif

#define _MIST_ALLOCATOR_MEMORY_TRIM_    0       


#if defined( __MIST_MSVC__ ) && __MIST_MSVC__ <= 6



_MIST_END

    #include <cmath>


    #define STD_CMATH_FUNC1( name, func, value ) inline value name ( value v ){ return( ::func( v ) ); }
    #define STD_CMATH_FUNC2( name, func, value ) inline value name ( value v1, value v2 ){ return( ::func( v1, v2 ) ); }
    namespace std
    {
        STD_CMATH_FUNC1(   abs,    abs, int )
        STD_CMATH_FUNC1(   abs,   labs, long )
        STD_CMATH_FUNC1(   abs,   fabs, double )
        STD_CMATH_FUNC1(  sqrt,   sqrt, double )
        STD_CMATH_FUNC1(   sin,    sin, double )
        STD_CMATH_FUNC1(   cos,    cos, double )
        STD_CMATH_FUNC1(   log,    log, double )
        STD_CMATH_FUNC1( log10,  log10, double )
        STD_CMATH_FUNC1(   exp,    exp, double )
        STD_CMATH_FUNC2(   pow,    pow, double )
    }
    #undef STD_CMATH_FUNC1
    #undef STD_CMATH_FUNC2


_MIST_BEGIN

#endif




#ifndef _MIST_USER_CONFIG_

#if !defined( NDEBUG ) || defined( DEBUG )


    #define _CHECK_ACCESS_VIOLATION_        1   
    #define _CHECK_ARRAY_OPERATION_         1   
    #define _CHECK_ARRAY1_OPERATION_        1   
    #define _CHECK_ARRAY2_OPERATION_        1   
    #define _CHECK_ARRAY3_OPERATION_        1   

    #define _CHECK_MATRIX_OPERATION_        1   

#else


    #define _CHECK_ACCESS_VIOLATION_        0   
    #define _CHECK_ARRAY_OPERATION_         0   
    #define _CHECK_ARRAY1_OPERATION_        0   
    #define _CHECK_ARRAY2_OPERATION_        0   
    #define _CHECK_ARRAY3_OPERATION_        0   

    #define _CHECK_MATRIX_OPERATION_        0   

#endif


#define __CHECK_HARD_CASE__                 0   

#define _ARRAY_BIND_OPERATION_SUPPORT_      0   
#define _ARRAY1_BIND_OPERATION_SUPPORT_     0   
#define _ARRAY2_BIND_OPERATION_SUPPORT_     0   
#define _ARRAY3_BIND_OPERATION_SUPPORT_     0   


#define _MIST_THREAD_SUPPORT_               1   

#define _MIST_VECTOR_SUPPORT_               0   

#define _LZW_COMPRESSION_SUPPORT_       false   


#define _USE_INTEL_MATH_KERNEL_LIBRARY_     0   
#define _DESCENDING_ORDER_EIGEN_VALUE_      0   
#define _USE_BALANCING_MATRIX_EIGEN_        1   
#define _USE_DIVIDE_AND_CONQUER_SVD_        1   

#endif


template < class Object1, class Object2 >
inline bool is_same_object( const Object1 &o1, const Object2 &o2 )
{
    return( reinterpret_cast< const void * >( &o1 ) == reinterpret_cast< const void * >( &o2 ) );
}



struct __mist_dmy_callback__
{

    bool operator()( long double percent ) const { return( true ); }
};



template < class Functor >
struct __mist_convert_callback__
{
    Functor f_;
    double lower_;
    double upper_;


    bool operator()( long double percent )
    {
        percent = lower_ + percent / 100.0 * ( upper_ - lower_ );
        return( f_( percent ) );
    }

    __mist_convert_callback__( ) : lower_( 0.0 ), upper_( 100.0 ){ }
    __mist_convert_callback__( Functor f, double l = 0.0, double u = 100.0 ) : f_( f ), lower_( l ), upper_( u ){ }
};



struct __mist_console_callback__
{
private:
    int __percent__;

public:

    __mist_console_callback__( ) : __percent__( -1 )
    {
    }


    bool operator()( long double percent )
    {
        int ppp = static_cast< int >( percent );
        if( __percent__ != ppp )
        {
            __percent__ = ppp;

            int k3 = static_cast< int >( percent / 100.0 );
            percent -= k3 * 100.0;
            int k2 = static_cast< int >( percent / 10.0 );
            percent -= k2 * 10.0;
            int k1 = static_cast< int >( percent );

            std::cerr << "busy... ";
            if( k3 == 0 )
            {
                std::cerr << " ";
                if( k2 == 0 )
                {
                    std::cerr << " " << k1;
                }
                else
                {
                    std::cerr << k2 << k1;
                }
            }
            else
            {
                std::cerr << 1 << k2 << k1;
            }
            if( percent > 100.0 )
            {
                std::cerr << "%" << std::endl;
            }
            else
            {
                std::cerr << "%\r";
            }
        }
        return( true );
    }
};





struct __mist_progress_callback__
{
    __mist_progress_callback__( )
    {
        std::cerr << "0%   10   20   30   40   50   60   70   80   90   100%" << std::endl;
        std::cerr << "|----|----|----|----|----|----|----|----|----|----|" << std::endl;
    }


    bool operator()( long double percent ) const
    {
        if( percent > 100.0 )
        {
            std::cerr << "***************************************************" << std::endl;
        }
        else
        {
            int i, p = static_cast< int >( percent / 2.0 );

            for( i = 0 ; i < p ; i++ )
            {
                std::cerr << "*";
            }
            for( ; i < 51 ; i++ )
            {
                std::cerr << " ";
            }

            std::cerr << "\r";
        }

        return( true );
    }
};






#if _CHECK_ACCESS_VIOLATION_


    inline void mist_debug_assertion( ptrdiff_t index )
    {
        ::std::cerr << "Access Violation at ( " << static_cast< int >( index ) << " )" << ::std::endl;
    }


    inline void mist_debug_assertion( ptrdiff_t index1, ptrdiff_t index2 )
    {
        ::std::cerr << "Access Violation at ( " << static_cast< int >( index1 ) << ", " << static_cast< int >( index2 ) << " )" << ::std::endl;
    }



    inline void mist_debug_assertion( ptrdiff_t index1, ptrdiff_t index2, ptrdiff_t index3 )
    {
        ::std::cerr << "Access Violation at ( " << static_cast< int >( index1 ) << ", " << static_cast< int >( index2 ) << ", " << static_cast< int >( index3 ) << " )" << ::std::endl;
    }


    #define _CHECK_ACCESS_VIOLATION1_( index ) \
        if( index < 0 || index >= size( ) )\
        {\
            static value_type dmy;\
            mist_debug_assertion( index );\
            return( dmy );\
        }\

    #define _CHECK_ACCESS_VIOLATION2_( index1, index2 ) \
        if( index1 < 0 || index1 >= size1( ) || index2 < 0 || index2 >= size2( ) )\
        {\
            static value_type dmy;\
            mist_debug_assertion( index1, index2 );\
            return( dmy );\
        }\

    #define _CHECK_ACCESS_VIOLATION3_( index1, index2, index3 ) \
        if( index1 < 0 || index1 >= size1( ) || index2 < 0 || index2 >= size2( ) || index3 < 0 || index3 >= size3( ) )\
        {\
            static value_type dmy;\
            mist_debug_assertion( index1, index2, index3 );\
            return( dmy );\
        }\

#else

    #define _CHECK_ACCESS_VIOLATION1_( index ) 
    #define _CHECK_ACCESS_VIOLATION2_( index1, index2 )
    #define _CHECK_ACCESS_VIOLATION3_( index1, index2, index3 )

#endif


_MIST_END


#endif
