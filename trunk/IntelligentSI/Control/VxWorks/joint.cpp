/******************************************************************************
 * FileName      : joint.cpp
 * Created       : 2007/08/23
 * LastModified  : 2007/
 * Author        : hiroaki KOZUKA
 * Aim           : For angle control
 * OS            : VxWorks 5.5.1
 *****************************************************************************/
#include "Joint.h"
#include "Driver.h"

//namespace occs{

//-----------------------------------------------------------------------------
//
Joint::Joint(Driver* d, int id){
  cout<<"Initialize joint"<<id<<"..."<<flush;
  drv = d;

  jID = id;

  Kp = 0;
  Ti = 0.0001;
  Td = 0.0001;
  Ts = 0.001;

  curAngle = 0;
  Torque = 0;
  SpeedRadio = 1;

  ie = e = dy = e_= yd = yd_ = 0;

  cout<<"done."<<endl;
}

//-----------------------------------------------------------------------------
//
Joint::~Joint(){
  std::cout<<"End Joint"<<jID<<" Instance."<<std::endl;
}

//-----------------------------------------------------------------------------
//
void
Joint::SetCtrlParam(double kp, double ti, double td){
  Kp = kp;
  Ti = ti;
  Td = td;
}

//-----------------------------------------------------------------------------
//!
void
Joint::SetConst(double speed){
  SpeedRadio = speed;
}

//-----------------------------------------------------------------------------
//
void
Joint::SetAngle(double angle){
  drv->SetAngle(jID, angle );
  curAngle = angle;
}

//-----------------------------------------------------------------------------
//
/*
void
Joint::SetTorque(double torque){
  torque *= SpeedRadio;
  deviceDriver.speedSet(jID, torque);
}
*/

//-----------------------------------------------------------------------------
//
double
Joint::PDController(double dest, double cur){
  double eta = 0.1;
  double y;

  dest = 180.0*dest/PI;
  cur = 180.0*cur/PI;

  e = dest - cur;
  dy =  cur - y_;

  yd = (Td*(dy + eta*yd_))/(Ts + eta*Td);

  if(Ti>0){
    //y = Kp*( e + ie*Ti + yd );
    y = Kp*e + ie*Ti;
  }
  else{
    //y = Kp*( e + yd );
    y = Kp*e;
  }

  ie += e*Ts;
  y_ = cur;
  e_ = e;
  yd_ = yd;

  return(y);
}

double
Joint::LowPassF_FOL(double data){
  const double dt = 0.001; // Simpling time [s]
  const double T  = 0.004; // [s]
  static double data_1 = 0;

  data = (dt*data + T*data_1)/(dt + T);
  data_1 = data;
  return data;
}

//-----------------------------------------------------------------------------
//
double
Joint::CtrlAngle(double destAngle){
  curAngle = drv->GetAngle(jID);
  //GetIFM()->SendCurAngle(jID, curAngle);
  Torque = this->PDController( destAngle, curAngle );
  drv->SetTorque(jID, Torque);
  return (curAngle);
}


//} // End of namespace occs


