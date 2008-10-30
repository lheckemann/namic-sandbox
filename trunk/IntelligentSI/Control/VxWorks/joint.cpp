/***************************************************************************
 * FileName      : joint.cpp
 * Created       : 2007/08/23
 * LastModified  : 2007/
 * Author        : hiroaki KOZUKA
 * Aim           : joint class for a motor control
 * OS            : VxWorks 5.5.1
 ***************************************************************************/
#include "joint.h"
const PID_DEFAULT_DATA JOINT::PID_D[jNum];

JOINT::JOINT(DRIVER& driver,int jid):deviceDriver(driver){
    
    //logMsg("Initialaze joint%d Instance.\n",jid,0,0,0,0,0);
    cout<<"Initialaze joint"<<jid<<" Instance"<<endl;
    jID = jid;

    PID.direction = deviceDriver.directionDataShow(jID);
    PID.Kp = PID_D[jID].Kp;
    PID.Ti = PID_D[jID].Ti;
    PID.Td = PID_D[jID].Td;
    PID.Kv = 0.001;
    PID.Ts = 0.001;
    PID.e = 0;
    PID.e_1 = 0;
    PID.e_2 = 0;
    PID.y_ = 0;
    PID.u_ = 0;
    
    //logMsg("Joint%d done.\n",jid,0,0,0,0,0);
    cout<<"Joint"<<jid<<" done."<<endl;
}

JOINT::~JOINT(){
    //logMsg("End Joint%d Instance.\n",(int)jID,0,0,0,0,0);
    cout<<"End Joint"<<jID<<" Instance."<<endl;
}

void
JOINT::angleSet(double angle){
     deviceDriver.angleSet(jID, angle);
     curAngle = angle;
     jointDataCalculate();
}

void
JOINT::speedSet(double speed){
    deviceDriver.speedSet(jID, speed);
}

double 
JOINT::ctrlForce(){
    
    return(0);
}

void
JOINT::jointDataCalculate(){
    jData.angle = curAngle;
    jData.speed = jData.angle - jData.angle_;
    jData.accel = jData.speed - jData.speed_;
    jData.angle_ = jData.angle;
    jData.speed_ = jData.speed;
}

double
JOINT::PIDCalculate(double destValue, double curValue){
    PID.v = (destValue - PID.u_)/PID.Ts;
    PID.e = destValue - curValue;
    PID.de = PID.e - PID.e_1;
    PID.de_ = PID.e_1 - PID.e_2;
    
    //PID control fomula (velocity)
    PID.dy = PID.Kp * ( PID.de + PID.e*PID.Ts/PID.Ti + PID.Td*(PID.de - PID.de_)/PID.Ts );
    PID.y = PID.y_ + PID.direction * PID.dy; //PID.Kv*PID.v;
    PID.y_ = PID.y;
    PID.e_2 = PID.e_1;
    PID.e_1 = PID.e;
    PID.u_ = destValue; 
  //cout<<"V:"<<PID.y<<endl;
    return(PID.y);
}

JOINT_DATA
JOINT::angleControl(double destAngle){
    
    curAngle = deviceDriver.angleGet(jID);
    speed = PIDCalculate( destAngle, curAngle );
    deviceDriver.speedSet(jID, speed);
    jointDataCalculate();
    
    return(jData);
}
