
#ifdef _WIN32_WINDOWS
#error This is for Posix only! pthread and named pipe
#endif

// Status: ps=0 (false) means ERROR, ps=1 (true) - OK



void CREATE_THREAD(pthread_t & threadID, void * (*start_routine)(void *), void * arg, int & status)
{
 status = !pthread_create( &threadID, NULL, start_routine, arg);

}



// Pipe


// Return TRUE (1) if success


void JHU_OPEN_PIPE_WRITE(char *Lin, char *Win,JHU_PIPE_HANDLE & ph,JHU_PIPE_STATUS & ps)
{ 
 umask(0);
 mknod(Lin, S_IFIFO|0666, 0);
 ph = fopen(Lin, "w");
 ps = (ph!=0);
 printf("Open pipe for write: %d\n", ps);
}

void JHU_OPEN_PIPE_WRITE2(char *Lin, char *Win,JHU_PIPE_HANDLE & ph,JHU_PIPE_STATUS & ps)
{
        JHU_OPEN_PIPE_WRITE(Lin,Win,ph,ps);
}


void JHU_OPEN_PIPE_READ(char *Lin, char *Win,JHU_PIPE_HANDLE & ph,JHU_PIPE_STATUS & ps)
{
 umask(0);
 mknod(Lin, S_IFIFO|0666, 0);
 ph = fopen(Lin, "r");
 ps = (ph!=0);
 printf("Open pipe for read: %d\n", ps);
}

void JHU_OPEN_PIPE_READ2(char *Lin, char *Win,JHU_PIPE_HANDLE & ph,JHU_PIPE_STATUS & ps)
{
        JHU_OPEN_PIPE_READ(Lin,Win,ph,ps);
}



void JHU_PIPE_CONNECT(JHU_PIPE_HANDLE & ph, JHU_PIPE_STATUS & ps)
{
        ps=1;
}


void JHU_PIPE_WRITE(void * buffer, int size,JHU_PIPE_HANDLE & ph,JHU_PIPE_STATUS & ps )
{
        int BytesWritten;
        BytesWritten = fwrite(buffer, size, 1, ph);
        fflush(ph);
        ps = (BytesWritten == 1);
}


void JHU_PIPE_READ(void * buffer, int size,JHU_PIPE_HANDLE & ph,JHU_PIPE_STATUS & ps )
{
        int BytesRead;
        BytesRead = fread(buffer, size, 1, ph);
        ps = (BytesRead == 1);
}



void JHU_PIPE_CLOSE(JHU_PIPE_HANDLE & ph)
{
        fclose(ph);
        ph=0;
} 

void JHU_PIPE_DISCONNECT(JHU_PIPE_HANDLE & ph)
{
}


