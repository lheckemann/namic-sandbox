/***************************************************************************
 * FileName      : keyConsole.cpp
 * Created       : 2008/01/15
 * LastModified  : 
 * Author        : Hiroaki KOZUKA
 * Aim           : Key board consloe
 ***************************************************************************/
#include "keyConsole.h"
using namespace std;

string KEYCONSOLE::buffer;
EVENT KEYCONSOLE::eventFlag;

KEYCONSOLE::KEYCONSOLE(){
#ifndef __linux__ //if not __linux__
  exclusMutex = semBCreate( SEM_Q_FIFO, SEM_FULL );
  semTake(exclusMutex, WAIT_FOREVER);
#else  //if __linux__
        pthread_mutex_init(&exclusMutex, NULL);
        pthread_mutex_lock(&exclusMutex);
#endif //if not __linux__

  eventFlag = E_UNKNOWN;

#ifndef __linux__ //if not __linux__
  semGive(exclusMutex);
#else  //if __linux__
        pthread_mutex_unlock(&exclusMutex);
#endif //if not __linux__
  
#ifndef __linux__ //if not __linux__
// spawn tasks
  taskSpawn("tkeyInput", 99, VX_FP_TASK, 40000, (FUNCPTR)checkInput,
        (int)this, 0, 0, 0, 0, 0, 0, 0, 0, 0);
#else  //if __linux__
        // spawn a thread
        int ret=0;
        pthread_attr_t pt_attr;
        struct sched_param pt_priority_param;
        if ((ret = pthread_attr_init(&pt_attr))) 
          cerr << "KEYCONSOLE: ERROR initialising the pthread attr parameter; returned " << ret << endl;
        // a root is needed to set some thread parameters; using defaults if not root
        if (geteuid() == 0) {
          //2==SCHED_RR which is RoundRobin; 
          if ((ret = pthread_attr_setschedpolicy(&pt_attr, 2))) 
          cerr << "KEYCONSOLE: ERROR setting the pthread scheduling policy; returned " << ret << endl; 
          //priority
          int maxprio = sched_get_priority_max(2);
          int minprio = sched_get_priority_min(2);
          /* 4/10 comes from vxworks' priority 100 in (max=0;min=255); 
     * TODO: confirm: to use similar priority as in the taskSpawn call above
     * we need vxw 99, which is 1 higher than 100 (the PNTM_intf task), 
     * so we use one higher in ARTLinux too. 
     */
          pt_priority_param.sched_priority = maxprio - (maxprio - minprio)*4/10 + 1; 
          if ((ret = pthread_attr_setschedparam(&pt_attr, &pt_priority_param))) 
          cerr << "KEYCONSOLE: ERROR setting the pthread priority; returned " << ret << endl; 
          //stack size
          if ((ret = pthread_attr_setstacksize(&pt_attr, 40000)))
          cerr << "KEYCONSOLE: ERROR setting the pthread stacksize; returned " << ret << endl; 
        }
        if ((ret = pthread_create(&tkeyInput, &pt_attr, checkInput, (void *) this)))
          cerr << "KEYCONSOLE: ERROR creating the checkInput() thread; returned " << ret << endl;
        pthread_attr_destroy(&pt_attr);
#endif //if not __linux__

  cout<<"KeyConsole done."<<endl;
}

KEYCONSOLE::~KEYCONSOLE(){
#ifndef __linux__ //if not __linux__
  semDelete( exclusMutex );
  taskDelete( taskNameToId("tkeyInput") );
#else  //if __linux__
  pthread_mutex_destroy(&exclusMutex);
  pthread_cancel(tkeyInput);
#endif //if not __linux__
  cout<<"End keyConsole."<<endl;
}

#ifndef __linux__ //if not __linux__
int
KEYCONSOLE::checkInput( void* thisFunc ){
    KEYCONSOLE* keyConsole = (KEYCONSOLE*)thisFunc; // instance pointer
    struct fd_set readFds; // bit mask of fds to read from
#else  //if __linux__
void*
KEYCONSOLE::checkInput( void* arg ){
    KEYCONSOLE *keyConsole = (KEYCONSOLE *) arg; // instance pointer
    fd_set readFds; // bit mask of fds to read from
#endif //if not __linux__
    int    width;          // number of fds on which to pend
    string tmpbuf;
  EVENT flag = E_UNKNOWN;
    //istringstream iss2( line.c_str() );
  cout << "<keyCon>: "<<flush;
  
  do{
    width = 0;
    
    // clear bits in read bit mask, and initialize bit mask
    FD_ZERO (&readFds);
    FD_SET (0, &readFds); // stdin
    width++;
    
    // pend, waiting for one or more fds to become ready
    if (select(width, &readFds, NULL, NULL, NULL) == ERROR) {
      return (OK);
    }
    
    // check input from stdin
    if ( FD_ISSET (0, &readFds) ) {
      cout << "<keyCon>: ";
      getline(cin, buffer);
      
      // get rid of backspaces(DEL) from buffer
      bool loopflag = 0;
      
      // a number of string
      unsigned int bspos = buffer.find(char(127));
      
      if ( bspos >0 && bspos < buffer.length() ) {
        loopflag = 1;
      }
      while(loopflag) {
        buffer.replace(buffer.find(char(127))-1,2,"");
        loopflag = 0;
        bspos = buffer.find(char(127));
        if(bspos >0 && bspos < buffer.length()) {
          loopflag = 1;
        }
      }
      flag = keyConsole->keyHandler(flag);
    }
    
  }while( flag != E_PRGM_EXIT );

  return (OK);
}



EVENT
KEYCONSOLE::keyHandler(EVENT flag){
  EVENT flag_1 = flag;
  
  if (buffer == "exit"){
    flag = E_PRGM_EXIT;
    cout << "exit" << endl;
  }
  else if (buffer == "help" || buffer == "?") {
    helpMessage();
  }
  else if (buffer == "istart") {
    flag =  E_PRGM_START;
    cout << "istart" << endl;
  }
  else if( buffer == "cstart") {
    flag = E_CTRL_STOP_START;
    cout<<"cstart"<<endl;
  }
  else if( buffer == "cstop"){
    flag = E_CTRL_STOP_STOP;
    cout<<"cstop"<<endl;
  }
  else if(buffer == ""){
    
  }
    else {
        cout << "\nUndefined symbol: " << buffer << endl;
        cout << "Type 'help' for command" << endl;
    }
    
  if(flag != flag_1){
#ifndef __linux__ //if not __linux__
    semTake(exclusMutex, WAIT_FOREVER);
#else  //if __linux__
                pthread_mutex_lock(&exclusMutex);
#endif //if not __linux__

    eventFlag = flag;

#ifndef __linux__ //if not __linux__
    semGive(exclusMutex);
#else  //if __linux__
                pthread_mutex_unlock(&exclusMutex);
#endif //if not __linux__
  }
  
  return (flag);
}

EVENT
KEYCONSOLE::eventRead(){
  EVENT flag = E_UNKNOWN;
#ifndef __linux__ //if not __linux__
  semTake(exclusMutex, NO_WAIT);
#else  //if __linux__
  //TODO: confirm if this is the desired behaviour
        if (pthread_mutex_trylock(&exclusMutex) == 0) {
#endif //if not __linux__

  flag = eventFlag;

#ifndef __linux__ //if not __linux__
  semGive(exclusMutex);
#else  //if __linux__
        pthread_mutex_unlock(&exclusMutex);
  }
#endif //if not __linux__

  return flag;
}

int
KEYCONSOLE::helpMessage()
{
  cout << "\n"
       << "/****************************************** \n"
       << " * help      - Print this list     \n"
       << " * istart    - start program       \n"
       << " * cstart    - start move        \n"
       << " * cstop     - stop move       \n"
       << " * exit      - Exit program        \n"
       << " ******************************************  \n"<< endl;
  return (OK);
}

