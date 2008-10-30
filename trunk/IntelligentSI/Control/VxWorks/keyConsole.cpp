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
  exclusMutex = semBCreate( SEM_Q_FIFO, SEM_FULL );

  semTake(exclusMutex, WAIT_FOREVER);
  eventFlag = E_UNKNOWN;
  semGive(exclusMutex);
  
// spawn tasks
  taskSpawn("tkeyInput", 99, VX_FP_TASK, 40000, (FUNCPTR)checkInput,
        (int)this, 0, 0, 0, 0, 0, 0, 0, 0, 0);
  cout<<"KeyConsole done."<<endl;
}

KEYCONSOLE::~KEYCONSOLE(){
  semDelete( exclusMutex );
  taskDelete( taskNameToId("tkeyInput") );
  cout<<"End keyConsole."<<endl;
}

int
KEYCONSOLE::checkInput( void* thisFunc ){
    KEYCONSOLE* keyConsole = (KEYCONSOLE*)thisFunc; // instance pointer
    struct fd_set readFds; // bit mask of fds to read from
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
    semTake(exclusMutex, WAIT_FOREVER);
    eventFlag = flag;
    semGive(exclusMutex);
  }
  
  return (flag);
}

EVENT
KEYCONSOLE::eventRead(){
  EVENT flag = E_UNKNOWN;
  semTake(exclusMutex, NO_WAIT);
  flag = eventFlag;
  semGive(exclusMutex);
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
       << " * cstop     - stop move         \n"
       << " * exit      - Exit program        \n"
       << " ******************************************  \n"<< endl;
  return (OK);
}

