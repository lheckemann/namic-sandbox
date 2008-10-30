/***********************************************************
 * FileName      : keyConsole.h
 * Created       : 2008/01/
 * LastModified  : 
 * Author        : Hiroaki KOZUKA
 * Aim           : Key board consloe 
  ***********************************************************/
#ifndef KEYCON__H
#define KEYCON__H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
#include <math.h>
#include <time.h>
#include <fcntl.h>

#ifndef __linux__ //if not __linux__

#include <strstream>
#include <vxWorks.h>
#include <kernelLib.h>
#include <usrLib.h>
#include <intLib.h>
#include <vxLib.h>
#include <memLib.h>
#include <semLib.h>
#include <msgQLib.h>
#include <sysLib.h>
#include <ioLib.h>
#include <logLib.h>
#include <selectLib.h>
#include <taskLib.h>
#include "drv/pci/pciConfigLib.h"
#include "mcpx800.h"
#include "/usr/local/tornado.ppc/target/config/mcp820/config.h"

#else  //if __linux__

#include <pthread.h> 
#include <sstream>
using namespace std;

#endif //if not __linux__

#include "common.h"


class KEYCONSOLE{
 private:
    static EVENT eventFlag;
    static string buffer;
#ifndef __linux__ //if not __linux__
    SEM_ID exclusMutex;
    static int checkInput( void* thisFunc );
#else //if __linux__
    pthread_mutex_t exclusMutex;
    pthread_t tkeyInput;
    static void* checkInput(void* arg);
#endif //if not __linux__
  
    EVENT keyHandler(EVENT flag);
    int helpMessage();
  
 public:
    KEYCONSOLE();
    ~KEYCONSOLE();
  EVENT eventRead();
};

#endif //KEYCON__H

