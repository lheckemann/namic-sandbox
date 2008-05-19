#ifndef _BRP_Platform_h_
#define _BRP_Platform_h_

#ifdef _WIN32_WINDOWS
 #include "windows.h" 
 #define JHU_PIPE_STATUS BOOL
 #define JHU_PIPE_HANDLE HANDLE
 #include <boost/thread.hpp>
 typedef boost::thread * pthread_t;
 #define sleep Sleep
 typedef int pthread_attr_t;
 #define PTHREAD_CANCEL_ENABLE (1)
 #define PTHREAD_CANCEL_ASYNCHRONOUS (1)
 #define pthread_setcancelstate(a,b) (1)
 #define pthread_setcanceltype(a,b) (1)
 #define pthread_yield() (1)
 #define pthread_testcancel() (1)
 #define pthread_cancel(a) (1)
 #define umask(a) (1)
#else
 #define JHU_PIPE_STATUS bool
 #define JHU_PIPE_HANDLE FILE *
 #include <pthread.h>
 #include <unistd.h>
 #include <stdio.h>
 #include <sys/stat.h>
#endif


void CREATE_THREAD(pthread_t & threadID, void * (*start_routine)(void *), void * arg, int & status);

// Status: ps=0 (false) means ERROR, ps=1 (true) - OK

void JHU_OPEN_PIPE_WRITE(char *Lin, char *Win,JHU_PIPE_HANDLE & ph,JHU_PIPE_STATUS & ps);
void JHU_OPEN_PIPE_READ(char *Lin, char *Win,JHU_PIPE_HANDLE & ph,JHU_PIPE_STATUS & ps);
void JHU_OPEN_PIPE_WRITE2(char *Lin, char *Win,JHU_PIPE_HANDLE & ph,JHU_PIPE_STATUS & ps);
void JHU_OPEN_PIPE_READ2(char *Lin, char *Win,JHU_PIPE_HANDLE & ph,JHU_PIPE_STATUS & ps);

// this blocks under Windows
void JHU_PIPE_CONNECT(JHU_PIPE_HANDLE & ph, JHU_PIPE_STATUS & ps);

void JHU_PIPE_WRITE(void * buffer, int size,JHU_PIPE_HANDLE & ph,JHU_PIPE_STATUS & ps );
void JHU_PIPE_READ(void * buffer, int size,JHU_PIPE_HANDLE & ph,JHU_PIPE_STATUS & ps );

void JHU_PIPE_DISCONNECT(JHU_PIPE_HANDLE & ph);
void JHU_PIPE_CLOSE(JHU_PIPE_HANDLE & ph);


#endif
