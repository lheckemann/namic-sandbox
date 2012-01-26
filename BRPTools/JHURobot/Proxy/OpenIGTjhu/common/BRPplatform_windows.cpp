#ifndef _WIN32_WINDOWS
#error This is for Windows only! pthread and named pipe
#endif
// Quick hack to port POSIX multi-threading and pipe to Windows

#include <boost/bind.hpp>


// Windows' CreateThread might be used also...
void CREATE_THREAD(pthread_t & threadID, void * (*start_routine)(void *), void * arg, int & status)
{
        threadID = new boost::thread( boost::bind(start_routine,arg) );
        status= (threadID!=NULL);
}

void JHU_DISPLAY_LAST_ERROR(LPTSTR lpszFunction) 
{
        LPVOID lpMsgBuf;
        LPVOID lpDisplayBuf;
        DWORD dw = GetLastError(); 

        FormatMessage(
                FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                FORMAT_MESSAGE_FROM_SYSTEM |
                FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL,
                dw,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (LPTSTR) &lpMsgBuf,
                0, NULL );

        lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, 
                (lstrlen((LPCTSTR)lpMsgBuf)+lstrlen((LPCTSTR)lpszFunction)+40)*sizeof(TCHAR)); 

                        /*
                        StringCchPrintf((LPTSTR)lpDisplayBuf, 
                                LocalSize(lpDisplayBuf) / sizeof(TCHAR),
                                TEXT("%s failed with error %d: %s"), 
                                lpszFunction, dw, lpMsgBuf); 
                        MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK); */
        printf("%s failed with error %d: \n[%s]\n", lpszFunction, dw, lpMsgBuf);

        LocalFree(lpMsgBuf);
        LocalFree(lpDisplayBuf);
}

// No security. You might want to use PIPE_REJECT_REMOTE_CLIENTS

// PIPE_ACCESS_OUTBOUND - Server 
void JHU_OPEN_PIPE_WRITE(char *Lin, char *Win,JHU_PIPE_HANDLE & ph,JHU_PIPE_STATUS & ps)
{
 ps = FALSE;
 ph = CreateNamedPipe ( Win, PIPE_ACCESS_DUPLEX | FILE_FLAG_WRITE_THROUGH,
        PIPE_WAIT | PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE , 10, 8*1024, 8*1024, 100, NULL);
 if (ph != INVALID_HANDLE_VALUE) { 
         ps = TRUE; 
 } else { 
         ph=0;
         JHU_DISPLAY_LAST_ERROR("JHU_OPEN_PIPE_WRITE");
 }
}

// Client
void JHU_OPEN_PIPE_WRITE2(char *Lin, char *Win,JHU_PIPE_HANDLE & ph,JHU_PIPE_STATUS & ps)
{
 ps = FALSE;
 ph = CreateFile(Win, GENERIC_WRITE,0,NULL,OPEN_EXISTING,0,NULL);
 if (ph != INVALID_HANDLE_VALUE) { 
         ps = TRUE; 
 } else { 
         ph=0;
         JHU_DISPLAY_LAST_ERROR("JHU_OPEN_PIPE_WRITE2");
 }
}


// PIPE_ACCESS_INBOUND - Server
void JHU_OPEN_PIPE_READ(char *Lin, char *Win,JHU_PIPE_HANDLE & ph,JHU_PIPE_STATUS & ps)
{
 JHU_OPEN_PIPE_WRITE(Lin,Win,ph,ps);
}


// Client
void JHU_OPEN_PIPE_READ2(char *Lin, char *Win,JHU_PIPE_HANDLE & ph,JHU_PIPE_STATUS & ps)
{
 ps = FALSE;
 ph = CreateFile(Win, GENERIC_READ,0,NULL,OPEN_EXISTING,0,NULL);
 if (ph != INVALID_HANDLE_VALUE) { 
         ps = TRUE; 
 } else { 
         ph=0;
         JHU_DISPLAY_LAST_ERROR("JHU_OPEN_PIPE_READ2");
 }
}

#include <strsafe.h>

// this blocks under Windows
void JHU_PIPE_CONNECT(JHU_PIPE_HANDLE & ph, JHU_PIPE_STATUS & ps)
{ 
        ps = ConnectNamedPipe(ph, NULL);
        if (!ps) {
                JHU_DISPLAY_LAST_ERROR("JHU_PIPE_CONNECT");
        }
}

// write buffer - blocking
void JHU_PIPE_WRITE(void * buffer, int size,JHU_PIPE_HANDLE & ph,JHU_PIPE_STATUS & ps )
{
        DWORD cbBytesWritten;
        ps = WriteFile(ph, buffer, size, &cbBytesWritten, NULL);
        if ( !ps ) {
                JHU_DISPLAY_LAST_ERROR("JHU_PIPE_WRITE");
        } else {
                FlushFileBuffers(ph);
        }
        // if (cbBytesWritten!=1) { ps = FALSE; } else { ps = TRUE;  }
}




// read buffer - blocking
void JHU_PIPE_READ(void * buffer, int size,JHU_PIPE_HANDLE & ph,JHU_PIPE_STATUS & ps )
{ 
        DWORD cbBytesRead;
        ps = ReadFile(ph, buffer, size, &cbBytesRead, NULL);
        if (!ps || (cbBytesRead == 0)) {  
                ps = FALSE;
                JHU_DISPLAY_LAST_ERROR("JHU_PIPE_READ");
        } else { 
                ps = TRUE; 
        }
}



void JHU_PIPE_DISCONNECT(JHU_PIPE_HANDLE & ph)
{ 
        DisconnectNamedPipe(ph); 
} 

void JHU_PIPE_CLOSE(JHU_PIPE_HANDLE & ph)
{
        CloseHandle(ph);
        ph=0; 
} 
