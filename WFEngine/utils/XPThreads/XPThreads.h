/*******************************************************************************
** FILE:   XPThreads.h

** DESCRIPTION:  C++ wrapper on threads
     X-Platform Threads Class. Tested on Mac and Windows, should 
     with other flavors of unix..

** AUTHOR:   Amit Gupta

** CREATED:   07-OCT-2006
********************************************************************************/
#ifndef _XPThreads_H_
#define _XPThreads_H_


#if _WINDOWS
 #include <windows.h>

 typedef unsigned long (*ThreadProc)( void* param );
 typedef DWORD XPThreadId;

#elif __MACH__
 #include "pthread.h"

 typedef pthread_t XPThreadId;
 typedef void* (*ThreadProc)( void* param );

#endif

/*********************************************************************************
** CLASS:   XPThreads

** DESCRIPTION:  
**********************************************************************************/
class XPThreads
{
 private:
  
  XPThreadId  m_Threadid;
  ThreadProc  m_Callback;

#if _WINDOWS
  long   m_nTimeout;
  HANDLE   m_ptrThread;
#endif
 
/******************************************************************************
** FUNCTION: Stop
*******************************************************************************/
   void Stop();   

 public:

/******************************************************************************
** FUNCTION: Constructor & Destructor
*******************************************************************************/
   XPThreads(ThreadProc ptrCallback = NULL, long nTimeout = 3000);
   virtual ~XPThreads() throw();

/******************************************************************************
** FUNCTION: SetThreadProc
*******************************************************************************/
   void SetThreadProc(ThreadProc ptrCallback); 

/******************************************************************************
** FUNCTION: GetThreadId
*******************************************************************************/
   XPThreadId GetThreadId() 
   { 
    return m_Threadid; 
   }

/******************************************************************************
** FUNCTION: Run
*******************************************************************************/
   bool Run();

/******************************************************************************
** FUNCTION: IsThreadCreated
*******************************************************************************/
   bool IsThreadCreated();
};



#endif
