/***************************************************************************
 * FileName      : keyConsole.cpp
 * Created       : 2008/01/15
 * LastModified  : 
 * Author        : Hiroaki KOZUKA Jumpei Arata
 * Aim           : Key board consloe
 ***************************************************************************/
#include "keyConsole.h"
using namespace std;

string KEYCONSOLE::buffer;
int KEYCONSOLE::eventFlag;

KEYCONSOLE::KEYCONSOLE(){
    exclusMutex = semBCreate( SEM_Q_FIFO, SEM_FULL );

    semTake(exclusMutex, WAIT_FOREVER);
    eventFlag = E.UNKNOWN;
    semGive(exclusMutex);
    
// spawn tasks
    taskSpawn("tkeyInput", 99, VX_FP_TASK, 40000, (FUNCPTR)checkInput,
                (int)this, (int)&E, 0, 0, 0, 0, 0, 0, 0, 0);
    cout<<"KeyConsole done."<<endl;
}

KEYCONSOLE::~KEYCONSOLE(){
    semDelete( exclusMutex );
    taskDelete( taskNameToId("tkeyInput") );
    cout<<"End keyConsole."<<endl;
}

int
KEYCONSOLE::checkInput( void* thisFunc,void* event  ){
    KEYCONSOLE* keyConsole = (KEYCONSOLE*)thisFunc; // instance pointer
    EVENT* E = (EVENT*) event;
    struct fd_set readFds; // bit mask of fds to read from
    int    width;          // number of fds on which to pend
    string tmpbuf;
    int flag = E->UNKNOWN;
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
            //getline(cin, buffer, '1');
            //ignore();
            
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
        
    }while( flag != E->PRGM_EXIT );

    return (OK);
}



int
KEYCONSOLE::keyHandler(int flag){
    int flag_1 = flag;
    
    if (buffer == "exit"){
        flag = E.PRGM_EXIT;
        cout << "exit" << endl;
    }
    else if (buffer == "help" || buffer == "?") {
        helpMessage();
    }
    else if (buffer == "istart") {
        flag =  E.PRGM_START;
        cout << "istart" << endl;
    }
    else if( buffer == "cstart") {
        flag = E.CTRL_STOP_START;
        cout<<"cstart"<<endl;
    }
    else if( buffer == "cstop"){
        flag = E.CTRL_STOP_STOP;
        cout<<"cstop"<<endl;
    }
    else if(buffer == ""){
        
    }
    else if(buffer == "1"){
        
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

int
KEYCONSOLE::stateRead(){
    int flag = E.UNKNOWN;
    semTake(exclusMutex, WAIT_FOREVER);
    flag = eventFlag;
    semGive(exclusMutex);
    return flag;
}

int
KEYCONSOLE::helpMessage()
{
  cout << "\n"
       << "/****************************************** \n"
       << " * help      - Print this list           \n"
       << " * istart    - start program             \n"
       << " * cstart    - start move                \n"
       << " * cstop     - stop move                 \n"
       << " * exit      - Exit program              \n"
       << " ******************************************  \n"<< endl;
  return (OK);
}

