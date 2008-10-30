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
    static EVENT eventFlag;
    static string buffer;
    static int checkInput( void* thisFunc );
  
    int helpMessage();
    EVENT keyHandler(EVENT flag);
  
 public:
    KEYCONSOLE();
    ~KEYCONSOLE();
  EVENT eventRead();
};

#endif KEYCON__H

