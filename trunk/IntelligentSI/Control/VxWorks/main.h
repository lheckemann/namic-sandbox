/***************************************************************************
 * FileName      : main.h
 * Created       : 2007/08/13
 * LastModified  : 
 * Author        : Hiroaki KOZUKA
 * Aim           : Main header for a Robot Control
 ***************************************************************************/

#ifndef MAIN__H
#define MAIN__H

#include <vxWorks.h>
#include <taskLib.h>
#include <stdio.h>
#include <stdlib.h>
#include <semLib.h>
#include <sysLib.h>
#include <logLib.h>
#include <intLib.h>
#include <math.h>
#include <memLib.h>
#include <string.h>
#include <iostream>
#include <strstream>
#include <fstream>
#include "drv/pci/pciConfigLib.h"
#include "mcpx800.h"
#include "/usr/local/tornado.ppc/target/config/mcp820/config.h"


//#include "driver.h"
//#include "joint.h"
#include "robot.h"
//#include "save.h"

ROBOT* slaveRobot;
void Main( );

#endif MAIN__H
