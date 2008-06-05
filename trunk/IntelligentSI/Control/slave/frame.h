/***************************************************************************
 * FileName      : frame.h
 * Created       : 2007/12/17
 * LastModified  : 2008/
 * Author        : Hiroaki KOZUKA Jumpei Arata NIT
 * Aim           : computed inverse/kinematics
 * OS            : VxWorks 5.5.1
 ***************************************************************************/
#ifndef FRAME__H
#define FRAME__H

#include <vxWorks.h>
#include <taskLib.h>
#include <kernelLib.h>
#include <stdio.h>
#include <stdlib.h>
#include <semLib.h>
#include <sysLib.h>
#include <usrLib.h>
#include <msgQLib.h>
#include <logLib.h>
#include <intLib.h>
#include "ioLib.h"
#include "pipeDrv.h"
#include <vxLib.h>
#include <iomanip>
#include <math.h>
#include <memLib.h>
#include <string.h>
#include <iostream.h>
#include <strstream.h>
#include <fstream.h>

#include "drv/pci/pciConfigLib.h"
#include "mcpx800.h"
#include "/usr/local/tornado.ppc/target/config/mcp820/config.h"

#include "common.h"

typedef struct ANGLE_INFO_STRUCT {
    double limitMax;
    double limitMin;
    double Offset;
    void Init(double max, double min, double offset){
        limitMax = max;
        limitMin = min;
        Offset = offset;
    }
}ANGLE_INFO;

class FRAME{
 private:
    int jNum;
    double UNIT_ANGLE;
    ANGLE_INFO* angleInfo;
    
    EE_POSITION limitPeeMax;
    EE_POSITION limitPeeMin;
    EE_POSITION Pee_1;
    EE_POSITION sPee;   
    void limitPosition(EE_POSITION* Pee);
 public:
    const TRANSFORM* getTransform();
    const double* getAngles();
    void calculateKinematics(const double *angle, EE_POSITION *Pee);
    int calculateInvKinematics(const EE_POSITION *Pee, double* angle);
    FRAME(double rad_deg);
    ~FRAME();
};


#endif FRAME__H
