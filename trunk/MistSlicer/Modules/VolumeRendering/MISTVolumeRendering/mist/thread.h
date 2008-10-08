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


#ifndef __INCLUDE_MIST_THREAD__
#define __INCLUDE_MIST_THREAD__


#ifndef __INCLUDE_MIST_CONF_H__
#include "config/mist_conf.h"
#endif

#ifndef __INCLUDE_MIST_SINGLETON__
#include "singleton.h"
#endif



#if !defined( _MIST_THREAD_SUPPORT_ ) || _MIST_THREAD_SUPPORT_ == 0
    
#elif defined( __MIST_WINDOWS__ ) && __MIST_WINDOWS__ > 0
    #include <windows.h>
    #include <process.h>
#else
    #include <pthread.h>
    #include <unistd.h>
    #include <time.h>
#endif

#ifndef INFINITE
#define INFINITE    ( ( unsigned long ) -1 )
#endif

#include <map>
#include <string>



_MIST_BEGIN




typedef unsigned long ThreadExitCode;
typedef ThreadExitCode ( LPTHREADFUNC ) ( void *thread_param );



struct thread_dmy_class{ };


struct thread_object
{
    virtual bool create( ) = 0;
    virtual bool wait( unsigned long dwMilliseconds = INFINITE ) = 0;
    virtual bool close( ) = 0;
    virtual bool suspend( ) = 0;
    virtual bool resume( ) = 0;
    virtual ~thread_object( ){ }
};



inline size_t get_cpu_num( )
{
#if !defined( _MIST_THREAD_SUPPORT_ ) || _MIST_THREAD_SUPPORT_ == 0

    return( 1 );
#elif defined( __MIST_WINDOWS__ ) && __MIST_WINDOWS__ > 0
    SYSTEM_INFO sysInfo;
    GetSystemInfo( &sysInfo );
    return( static_cast< size_t >( sysInfo.dwNumberOfProcessors ) );
#else
    return( static_cast< size_t >( sysconf( _SC_NPROCESSORS_ONLN ) ) );
#endif
}


inline void sleep( size_t dwMilliseconds )
{
#if defined( __MIST_WINDOWS__ ) && __MIST_WINDOWS__ > 0
    SleepEx( static_cast< DWORD >( dwMilliseconds ), false );
#else
    timespec treq, trem;
    treq.tv_sec = static_cast< time_t >( dwMilliseconds / 1000 );
    treq.tv_nsec = static_cast< long >( ( dwMilliseconds % 1000 ) * 1000000 );
    
    while( nanosleep( &treq, &trem ) != 0 )
    {
        treq = trem;
    }
#endif
}



template < class thread_parameter = thread_dmy_class >
class thread : public thread_object
{
public:
    typedef unsigned long thread_exit_type;     
private:

#if !defined( _MIST_THREAD_SUPPORT_ ) || _MIST_THREAD_SUPPORT_ == 0
    
#elif defined( __MIST_WINDOWS__ ) && __MIST_WINDOWS__ > 0
    HANDLE       thread_handle_;        
    unsigned int thread_id_;            
#else
    pthread_t thread_id_;               
    bool      thread_finished_;         
#endif

    thread_exit_type thread_exit_code_; 

public:


    thread_exit_type exit_code( ) const { return( thread_exit_code_ ); }


 
    const thread &operator =( const thread &t )
    {
#if !defined( _MIST_THREAD_SUPPORT_ ) || _MIST_THREAD_SUPPORT_ == 0

#elif defined( __MIST_WINDOWS__ ) && __MIST_WINDOWS__ > 0
        thread_handle_ = t.thread_handle_;
        thread_id_ = t.thread_id_;
#else
        thread_finished_ = t.thread_finished_;
        thread_id_ = t.thread_id_;
#endif
        thread_exit_code_ = t.thread_exit_code_;
        return( *this );
    }


    bool operator ==( const thread &t ) const
    {
#if !defined( _MIST_THREAD_SUPPORT_ ) || _MIST_THREAD_SUPPORT_ == 0

        return( this == &t );
#elif defined( __MIST_WINDOWS__ ) && __MIST_WINDOWS__ > 0
        return( thread_id_ == t.thread_id_ );
#else
        return( pthread_equal( thread_id_, t.thread_id_ ) != 0 );
#endif
    }

    bool operator !=( const thread &t ) const
    {
        return( !( *this == t ) );
    }


#if !defined( _MIST_THREAD_SUPPORT_ ) || _MIST_THREAD_SUPPORT_ == 0

    thread( const thread &t ) :thread_exit_code_( t.thread_exit_code_ ){ }
    thread( ) : thread_exit_code_( 0 ){ }
#elif defined( __MIST_WINDOWS__ ) && __MIST_WINDOWS__ > 0
    thread( const thread &t ) : thread_handle_( t.thread_handle_ ), thread_id_( t.thread_id_ ), thread_exit_code_( t.thread_exit_code_ ){ }
    thread( ) : thread_handle_( NULL ), thread_id_( -1 ), thread_exit_code_( 0 ){ }
#else
    thread( const thread &t ) : thread_id_( t.thread_id ), thread_finished_( t.thread_finished ), thread_exit_code_( t.thread_exit_code ){ }
    thread(  ) : thread_id_( ( pthread_t ) ( -1 ) ), thread_finished_( false ), thread_exit_code_( 0 ){ }
#endif

    virtual ~thread( ){ }



    virtual bool create( )
    {
#if !defined( _MIST_THREAD_SUPPORT_ ) || _MIST_THREAD_SUPPORT_ == 0

        bool ret = true;
        thread_exit_code_ = thread_function( );
#elif defined( __MIST_WINDOWS__ ) && __MIST_WINDOWS__ > 0
        if( thread_handle_ != NULL ) return( false );
        thread_handle_ = ( HANDLE )_beginthreadex( NULL, 0, map_thread_function, ( void * )this, 0, &thread_id_ );
        bool ret = thread_handle_ != NULL ? true : false;
#else
        if( thread_id_ != ( pthread_t ) ( -1 ) ) return( false );
        bool ret = pthread_create( &( thread_id_ ), NULL, map_thread_function, ( void * )this ) == 0 ? true : false;
#endif

        return ( ret );
    }



    virtual bool create_without_thread( )
    {
        thread_exit_code_ = thread_function( );
        return ( true );
    }



    virtual bool wait( unsigned long dwMilliseconds = INFINITE )
    {
#if !defined( _MIST_THREAD_SUPPORT_ ) || _MIST_THREAD_SUPPORT_ == 0

        return( true );
#elif defined( __MIST_WINDOWS__ ) && __MIST_WINDOWS__ > 0
        DWORD ret = WaitForSingleObject( thread_handle_, dwMilliseconds );
        return ( SUCCEEDED( ret ) );
#else
        if( dwMilliseconds == INFINITE )
        {
            return ( pthread_join( thread_id_, NULL ) == 0 );
        }
        else
        {
            unsigned long count = 0;

            while( true )
            {
                usleep( 1 );

                if( count < dwMilliseconds )
                {
                    if( thread_finished_ )
                    {
                        return ( true );
                    }
                }
                else
                {
                    return ( false );
                }

                count++;
            }
        }
#endif
    }


    virtual bool close( )
    {
#if !defined( _MIST_THREAD_SUPPORT_ ) || _MIST_THREAD_SUPPORT_ == 0

        return( true );
#elif defined( __MIST_WINDOWS__ ) && __MIST_WINDOWS__ > 0
        BOOL ret = CloseHandle( thread_handle_ );
        thread_handle_ = NULL;
        return ( ret != 0 );
#else
        int ret = pthread_detach( thread_id_ );
        thread_id_ = ( pthread_t ) ( -1 );
        return ( ret == 0 );
#endif
    }



    virtual bool suspend( )
    {
#if !defined( _MIST_THREAD_SUPPORT_ ) || _MIST_THREAD_SUPPORT_ == 0

        return( true );
#elif defined( __MIST_WINDOWS__ ) && __MIST_WINDOWS__ > 0
        return( SuspendThread( thread_handle_ ) != static_cast< DWORD >( -1 ) );
#else
//      return( pthread_suspend_np( thread_id_ ) == 0 );
        return( false );
#endif
    }


    virtual bool resume( )
    {
#if !defined( _MIST_THREAD_SUPPORT_ ) || _MIST_THREAD_SUPPORT_ == 0

        return( true );
#elif defined( __MIST_WINDOWS__ ) && __MIST_WINDOWS__ > 0
        return( ResumeThread( thread_handle_ ) == 1 );
#else
//      return( pthread_resume_np( thread_id_ ) == 0 );
        return( false );
#endif
    }


protected:

    virtual thread_exit_type thread_function( ) = 0;


#if !defined( _MIST_THREAD_SUPPORT_ ) || _MIST_THREAD_SUPPORT_ == 0

#elif defined( __MIST_WINDOWS__ ) && __MIST_WINDOWS__ > 0
    static unsigned int __stdcall map_thread_function( void *p )
    {
        thread *obj = static_cast< thread * >( p );
        obj->thread_exit_code_ = obj->thread_function( );
        return( 0 );
    }
#else
    static void *map_thread_function( void *p )
    {
        thread *obj = static_cast< thread * >( p );
        obj->thread_exit_code_ = obj->thread_function( );
        obj->thread_finished_ = true;
        return ( NULL );
    }
#endif
};



template < class Thread >
inline bool do_threads_( Thread *threads, size_t num_threads, unsigned long dwMilliseconds = INFINITE )
{
    bool ret = true;
    size_t i = 0;


    for( i = 1 ; i < num_threads ; i++ )
    {
        if( !threads[ i ].create( ) )
        {
            ret = false;
        }
    }
    if( num_threads > 0 )
    {

        threads[ 0 ].create_without_thread( );
    }


    for( i = 1 ; i < num_threads ; i++ )
    {
        if( !threads[ i ].wait( dwMilliseconds ) )
        {
            ret = false;
        }
    }


    for( i = 1 ; i < num_threads ; i++ )
    {
        if( !threads[ i ].close( ) )
        {
            ret = false;
        }
    }

    return( ret );
}



template < class Thread >
inline bool do_threads( Thread *threads, size_t num_threads, unsigned long dwMilliseconds = INFINITE )
{
    bool ret = true;
    size_t i = 0;


    for( i = 0 ; i < num_threads ; i++ )
    {
        if( !threads[ i ].create( ) )
        {
            ret = false;
        }
    }

    for( i = 0 ; i < num_threads ; i++ )
    {
        if( !threads[ i ].wait( dwMilliseconds ) )
        {
            ret = false;
        }
    }


    for( i = 0 ; i < num_threads ; i++ )
    {
        if( !threads[ i ].close( ) )
        {
            ret = false;
        }
    }

    return( ret );
}



namespace __thread_controller__
{
    template < class Param, class Functor >
    class thread_object_functor : public thread< thread_object_functor< Param, Functor > >
    {
    public:
        typedef thread< thread_object_functor< Param, Functor > > base;
        typedef typename base::thread_exit_type thread_exit_type;

    private:
        Param &param_;
        Functor func_;

    public:
        thread_object_functor( Param &p, Functor f ) : param_( p ), func_( f ){ }

    protected:

        virtual thread_exit_type thread_function( )
        {
            func_( param_ );
            return( 0 );
        }
    };
}





class thread_handle
{
private:
    thread_object *thread_;     

public:


    bool create( ){ return( thread_ == NULL ? false : thread_->create( ) ); }

    bool wait( unsigned long dwMilliseconds = INFINITE ){ return( thread_ == NULL ? false : thread_->wait( INFINITE ) ); }


    bool close( )
    {
        if( thread_ == NULL )
        {
            return( false );
        }
        bool b = thread_->close( );
        delete thread_;
        thread_ = NULL;
        return( b );
    }



    bool suspend( ){ return( thread_ == NULL ? false : thread_->suspend( ) ); }



    bool resume( ){ return( thread_ == NULL ? false : thread_->resume( ) ); }

public:
    thread_handle( ) : thread_( NULL ){ }
    thread_handle( thread_object *t ) : thread_( t ){ }

    const thread_handle &operator =( const thread_handle &t )
    {
        if( &t != this )
        {
            thread_ = t.thread_;
        }
        return( *this );
    }
};


template < class Param, class Functor >
inline thread_handle create_thread( Param &param, Functor f )
{
    thread_handle thread_( new __thread_controller__::thread_object_functor< Param, Functor >( param, f ) );
    thread_.create( );
    return( thread_ );
}


template < class Param, class Functor >
inline void create_threads( thread_handle *handles, Param *param, size_t num_threads, Functor f )
{
    for( size_t i = 0 ; i < num_threads ; i++ )
    {
        handles[ i ] = thread_handle( new __thread_controller__::thread_object_functor< Param, Functor >( param, f ) );
        handles[ i ].create( );
    }
}


inline bool close_thread( thread_handle &thread_ ){ return( thread_.close( ) ); }


inline bool close_threads( thread_handle *handles, size_t num_threads )
{
    bool ret = true;
    for( size_t i = 0 ; i < num_threads ; i++ )
    {
        if( !handles[ i ].close( ) )
        {
            ret = false;
        }
    }
    return( ret );
}


inline bool wait_thread( thread_handle &thread_, unsigned long dwMilliseconds = INFINITE ){ return( thread_.wait( dwMilliseconds ) ); }


inline bool wait_threads( thread_handle *handles, size_t num_threads, unsigned long dwMilliseconds = INFINITE )
{
    bool ret = true;
    for( size_t i = 0 ; i < num_threads ; i++ )
    {
        if( !handles[ i ].wait( dwMilliseconds ) )
        {
            ret = false;
        }
    }
    return( ret );
}



inline bool suspend_thread( thread_handle &thread_ ){ return( thread_.suspend( ) ); }


inline bool suspend_threads( thread_handle *handles, size_t num_threads )
{
    bool ret = true;
    for( size_t i = 0 ; i < num_threads ; i++ )
    {
        if( !handles[ i ].suspend( ) )
        {
            ret = false;
        }
    }
    return( ret );
}



inline bool resume_thread( thread_handle &thread_ ){ return( thread_.resume( ) ); }


inline bool resume_threads( thread_handle *handles, size_t num_threads )
{
    bool ret = true;
    for( size_t i = 0 ; i < num_threads ; i++ )
    {
        if( !handles[ i ].resume( ) )
        {
            ret = false;
        }
    }
    return( ret );
}



template < class Param, class Functor >
inline bool do_thread( Param &param, Functor f, unsigned long dwMilliseconds = INFINITE )
{
    bool ret = true;


    thread_handle thread_ = create_thread( param, f );


    if( !wait_thread( thread_, dwMilliseconds ) )
    {
        ret = false;
    }


    if( !close_thread( thread_ ) )
    {
        ret = false;
    }
    return( ret );
}


template < class Param, class Functor >
inline bool do_threads( Param *params, size_t num_threads, Functor f, unsigned long dwMilliseconds = INFINITE )
{
    bool ret = true;
    thread_handle *threads_ = new thread_handle[ num_threads ];


    create_threads( threads_, params, num_threads, f );


    if( !wait_threads( threads_, num_threads, dwMilliseconds ) )
    {
        ret = false;
    }


    if( !close_threads( threads_, num_threads ) )
    {
        ret = false;
    }

    return( ret );
}





 
struct lock_object
{
protected:
    ::std::string lock_name_;

#if !defined( _MIST_THREAD_SUPPORT_ ) || _MIST_THREAD_SUPPORT_ == 0
    typedef char lock_object_type;                  
#elif defined( __MIST_WINDOWS__ ) && __MIST_WINDOWS__ > 0
    typedef CRITICAL_SECTION lock_object_type;      
#else
    typedef pthread_mutex_t lock_object_type;       
#endif

    typedef ::std::map< ::std::string, lock_object_type > lock_table;

public:
    lock_object( ) : lock_name_( "mist default lock object!!" ){ }
    lock_object( const std::string &name ) : lock_name_( name ){ }

#if !defined( _MIST_THREAD_SUPPORT_ ) || _MIST_THREAD_SUPPORT_ == 0


    bool lock( ){ return( true ); }
    bool unlock( ){ return( true ); }

#else

    bool lock( )
    {
        static bool isFirst = true;
        static lock_object_type __double_lock__;
        if( isFirst )
        {
            isFirst = false;
            initialize( __double_lock__ );
        }


        lock( __double_lock__ );

        lock_table &table = singleton< lock_table >::get_instance( );
        lock_table::iterator ite = table.find( lock_name_ );
        if( ite == table.end( ) )
        {
            
            ::std::pair< lock_table::iterator, bool > p = table.insert( lock_table::value_type( lock_name_, lock_object_type( ) ) );
            if( p.second )
            {
                lock_object_type &obj = p.first->second;
                initialize( obj );

                unlock( __double_lock__ );

                lock( obj );
            }
            else
            {

                unlock( __double_lock__ );


                return( false );
            }
        }
        else
        {

            unlock( __double_lock__ );


            lock( ite->second );
        }

        return( true );
    }



    bool unlock( )
    {
        lock_table &table = singleton< lock_table >::get_instance( );
        lock_table::iterator ite = table.find( lock_name_ );
        if( ite == table.end( ) )
        {

            return( false );
        }
        else
        {
            unlock( ite->second );
        }
        return( true );
    }

#endif


protected:

    static void initialize( lock_object_type &l )
    {
#if !defined( _MIST_THREAD_SUPPORT_ ) || _MIST_THREAD_SUPPORT_ == 0

#elif defined( __MIST_WINDOWS__ ) && __MIST_WINDOWS__ > 0
        InitializeCriticalSection( &l );
#else
        pthread_mutex_init( &l, NULL );     
#endif
    }


    static void lock( lock_object_type &l )
    {
#if !defined( _MIST_THREAD_SUPPORT_ ) || _MIST_THREAD_SUPPORT_ == 0

#elif defined( __MIST_WINDOWS__ ) && __MIST_WINDOWS__ > 0
        EnterCriticalSection( &l );     
#else
        pthread_mutex_lock( &l );       
#endif
    }


    static void unlock( lock_object_type &l )
    {
#if !defined( _MIST_THREAD_SUPPORT_ ) || _MIST_THREAD_SUPPORT_ == 0
        
#elif defined( __MIST_WINDOWS__ ) && __MIST_WINDOWS__ > 0
        LeaveCriticalSection( &l );     
#else
        pthread_mutex_unlock( &l );     
#endif
    }
};



class lock
{
protected:
    lock_object lock_object_;   

public:

    lock( ) : lock_object_( )
    {
        lock_object_.lock( );
    }



    lock( const std::string &name ) : lock_object_( name )
    {
        lock_object_.lock( );
    }



    ~lock( )
    {
        lock_object_.unlock( );
    }
};



_MIST_END


#endif // __INCLUDE_MIST_THREAD__
