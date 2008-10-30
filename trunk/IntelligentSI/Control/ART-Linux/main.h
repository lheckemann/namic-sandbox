/***************************************************************************
 * FileName      : main.h
 * Created       : 2007/08/13
 * LastModified  : 
 * Author        : Hiroaki KOZUKA
 * Aim           : Main header for a Robot Control
 ***************************************************************************/

#ifndef MAIN__H
#define MAIN__H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <iostream>
#include <fstream>

#ifndef __linux__ 

#include <strstream>

#include <vxWorks.h>
#include <taskLib.h>
#include <semLib.h>
#include <sysLib.h>
#include <logLib.h>
#include <intLib.h>
#include <memLib.h>
#include "drv/pci/pciConfigLib.h"
#include "mcpx800.h"
#include "/usr/local/tornado.ppc/target/config/mcp820/config.h"

#else // __linux__

#include <sstream>

#endif // not __linux__


//#include "driver.h"
//#include "joint.h"
#include "robot.h"
//#include "save.h"

ROBOT* slaveRobot;
#ifndef __linux__ 

void Main( );

#else // __linux__

int main( );

#endif // not __linux__

#endif //MAIN__H
