/***********************************************************
 * FileName      : keyConsole.h
 * Created       : 2008/01/
 * LastModified  : 
 * Author        : Hiroaki KOZUKA
 * Aim           : Key board consloe 
  ***********************************************************/
#ifndef KEYCON__H
#define KEYCON__H

#include <vxWorks.h>
#include <kernelLib.h>
#include <stdlib.h>
#include <usrLib.h>
#include <intLib.h>
#include <vxLib.h>
#include <memLib.h>
#include <iostream>
#include <strstream>
#include <string>
#include <fstream>
#include <iomanip>
#include <stdio.h>
#include <semLib.h>
#include <msgQLib.h>
#include <sysLib.h>
#include <ioLib.h>
#include <logLib.h>
#include <selectLib.h>
#include <taskLib.h>
#include <fcntl.h>
#include <math.h>
#include <time.h>
#include "drv/pci/pciConfigLib.h"
#include "mcpx800.h"
#include "/usr/local/tornado.ppc/target/config/mcp820/config.h"


#include "common.h"

class KEYCONSOLE{
 private:
    SEM_ID exclusMutex;
    static int eventFlag;
    static string buffer;
    static int checkInput( void* thisFunc, void* );
    
    // event of robot
    EVENT E;
    
    int helpMessage();
    int keyHandler(int flag);
    
 public:
    KEYCONSOLE();
    ~KEYCONSOLE();
    int stateRead();
};

#endif KEYCON__H

