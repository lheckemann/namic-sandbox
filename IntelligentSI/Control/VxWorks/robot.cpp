/***************************************************************************
 * FileName      : robot.cpp
 * Created       : 2007/11/1
 * LastModified  : 2007/
 * Author        : Hiroaki KOZUKA
 * Aim           : Main program for a slave Robot Control
 *                 robot class
 * OS            : VxWorks 5.5.1
 ***************************************************************************/
#include "robot.h"
#define sq(x) ((x) * (x))

using namespace std;

STATE_MACHINE ROBOT::flag;

//Initialize
ROBOT::ROBOT():driver(),IF_manager(){
  cout<<"Initialaze robot class..."<<endl;
  
  for(int jID=0; jID<jNum; jID++){
    joint[jID] = new JOINT(driver, jID);
  }
  TF = new FRAME(360);
  
  timingSem = semBCreate(SEM_Q_FIFO,SEM_EMPTY);
  timeLimit = wdCreate();
  
  flag.event = E_UNKNOWN;
  flag.state = S_PRGM_ENTRANCE;
  flag.state_1 = S_PRGM_ENTRANCE;
  time = 0;
  stateSWFlag = 0;

  kernelTimeSlice(1);
  taskSpawn("tTiming", 90, VX_FP_TASK, 20000, (FUNCPTR)timing,
        (int)&timingSem,0,0,0,0,0,0,0,0,0);
  
  cout<<"Robot done."<<endl;
  
  robotMain();
}

ROBOT::~ROBOT(){
    for(int jID=0; jID<jNum; jID++){
    delete joint[jID];
  }
  delete TF;
  wdDelete(timeLimit);
  semDelete(timingSem);
  taskDelete(taskNameToId("tTiming"));
  //logMsg("End Robot class.\n",0,0,0,0,0,0);
  cout<<"End Robot class."<<endl;
}

//Setting end efector to origin 
void
ROBOT::originSet(){
    driver.stop();
  for(int i=0; i<jNum; i++)
    joint[i]->angleSet(0);
   driver.angleReadWrite(0);
}

//! Control Joint
void
ROBOT::jointControl(double* destjAngle, double* curjAngle, JOINT_DATA* jData){
    driver.angleReadWrite(1);
    for(int jID=0; jID<jNum;jID++){
    jData[jID] = joint[jID]->angleControl(destjAngle[jID]);
        curjAngle[jID] = jData[jID].angle;
    }
    
    driver.speedWrite();
    //send IF_manager
    
}

// Routine for the internal system clock
void
ROBOT::timing (void* sem){
  SEM_ID* timing = (SEM_ID*)sem;
  sysClkRateSet( 1000 ); // default clock rate is 60 defined in configAll.h
  do {
    taskDelay(sysClkRateGet()/1000); // 1ms
    semGive(*timing);
  } while(true);
}

// display data
void
ROBOT::dataDisp(int Ts){
  if( time%(Ts*1000) == 0 ){
    cout<<"t= "<<time/1000<<flush;
    cout<<" dPx= "<<(int)destPee.x<<flush;
    cout<<" Py= "<<(int)destPee.y<<flush;
    cout<<" Pz= "<<(int)destPee.z<<flush;
    cout<<" dm1= "<<(int)destAngle[0]<<flush;
    cout<<" m2= "<<(int)destAngle[1]<<flush;
    cout<<" m3= "<<(int)destAngle[2]<<flush;
    cout<<" cm1= "<<(int)curAngle[0]<<flush;
    cout<<" m2= "<<(int)curAngle[1]<<flush;
    cout<<" m3= "<<(int)curAngle[2]<<endl;
  }
}


// for symmetle bilateral control
void
ROBOT::bilateralErrCaluculate(EE_POSITION* dest, EE_POSITION* cur, EE_POSITION*  err){
  err->x = dest->x - cur->x;
  err->y = dest->y - cur->y;
  err->z = dest->z - cur->z;
  err->alpha = 0; // dest->alpha - cur->alpha;
  err->beta = 0;  // dest->beta  - cur->beta;
  err->gamma = 0; // dest->gamma - cur->gamma;
}


void
ROBOT::armDataCalculate(EE_POSITION* Pee, ARM_DATA* armData){
  //
  armData->Pee_P.x = Pee->x;
  armData->Pee_P.y = Pee->y;
  armData->Pee_P.z = Pee->z;
  armData->Pee_D.x = armData->Pee_P.x - armData->Pee_P.x_;
  armData->Pee_D.y = armData->Pee_P.y - armData->Pee_P.y_;
  armData->Pee_D.z = armData->Pee_P.z - armData->Pee_P.z_;
  armData->Pee_P.x_ = armData->Pee_P.x;
  armData->Pee_P.y_ = armData->Pee_P.y;
  armData->Pee_P.z_ = armData->Pee_P.z;
  //   
}

void
ROBOT::stateTransition(){
  STATE state   = flag.state;
  STATE state_1 = flag.state_1;
  state_1 = state;
  // read event
  EVENT event = IF_manager.eventRead(state);
  //
  if( event == E_PRGM_EXIT ){
    state = S_PRGM_EXIT;
    cout<<"<event>: exit"<<endl;
  }
  else if( event == E_EMERGENCY_0 ){
    state = S_EMERGENCY_0;
    cout<<"<event>: emg"<<endl;
  }
  else if( event == E_PRGM_START
       && ( state == S_PRGM_ENTRANCE || state ==  S_EMERGENCY_0 ) ){
    state =  S_CTRL_STOP;//S->CTRL_READY;
    cout<<"<event>: prgm_start"<<endl;
  }
  else if( event == E_CTRL_STOP_START
       && ( state == S_CTRL_READY || state ==  S_EMERGENCY_0 ) ){
    state = S_CTRL_STOP;
    cout<<"<event>: ctrl_stop_start"<<endl;
  }
  else if( event == E_CTRL_STOP_STOP
       && ( state == S_CTRL_STOP ||  state == S_EMERGENCY_0 ) ){
    state = S_CTRL_READY;
    cout<<"<event>: ctrl_stop_stop"<<endl;
  }
  else if( (event == E_CTRL_RUN_START || event == E_PRGM_START)
       && state == S_CTRL_STOP ){
    state = S_CTRL_RUN;
    cout<<"<event>: ctrl_run"<<endl;
  }
  else if( event == E_CTRL_RUN_STOP
       && ( state == S_CTRL_RUN || state == S_EMERGENCY_0 ) ){
    state = S_CTRL_STOP;
    cout<<"<event>: ctrl_stop"<<endl;
  }
  
  if( state != state_1 ){
    flag.event = event;
    flag.state = state;
    flag.state_1 = state_1;
    cout<<"state:"<<state<<" state_1:"<<state_1<<" event:"<<event<<endl;
    stateSWFlag = 1;
  }
  else if(state == state_1 &&  stateSWFlag == 1){
    flag.event = event;
    flag.state = state;
    flag.state_1 = state_1;
    stateSWFlag = 0;
  }
  else{
    stateSWFlag = 0;
  }
}

void
ROBOT::init(){
  dx = dy = dz = 0;
  
  destPee_1.x = 0;
  destPee_1.y = 0;
  destPee_1.z = -72.613;
  
  destPee.x = destPee.y = destPee.z = 0;
  destPee.alpha = destPee.beta = destPee.gamma = 0;
  
  for(int i=0; i<jNum; i++){
    destAngle[i] = 0;
  }
  
  originSet();
}

void
ROBOT::destDataGet(int state, int state_1 , unsigned long ctrlTime){

  if( state == S_CTRL_RUN && state_1 == S_CTRL_STOP){
    IF_manager.destEEPositionGet(0, &destPee);
    dx = destPee.x - destPee_1.x;
    dy = destPee.y - destPee_1.y;
    dz = destPee.z - destPee_1.z;
    destPee.x -= dx;
    destPee.y -= dy;
    destPee.z -= dz;
    cout<<"ON"<<endl;
  }
  else if( state == S_CTRL_RUN && state_1 ==  S_CTRL_RUN){
    IF_manager.destEEPositionGet(0, &destPee);
    destPee.x -= dx;
    destPee.y -= dy;
    destPee.z -= dz;
  }
  else if( state == S_CTRL_STOP && state_1 == S_CTRL_RUN){
    IF_manager.destEEPositionGet(0, &destPee);
    destPee_1.x = destPee.x -= dx;
    destPee_1.y = destPee.y -= dy;
    destPee_1.z = destPee.z -= dz;
    cout<<"OFF"<<endl;
  }
  else if( state == S_CTRL_STOP && state_1 == S_CTRL_STOP){
    IF_manager.destEEPositionGet(0, &destPee);
    destPee.x = destPee_1.x;
    destPee.y = destPee_1.y;
    destPee.z = destPee_1.z;
  }
  else if( state == S_EMERGENCY_0 ){
    //
  }
}


//Main loop in the bobot
void
ROBOT::armCtrl(int state, int state_1 ){
  
  destDataGet(state, state_1, time);
  TF->calculateInvKinematics(&destPee, destAngle);
  jointControl(destAngle, curAngle, curJoint);
  dataDisp(1);
  if(time%100 == 0)
    IF_manager.EEPositionSend(&destPee);
}

void
ROBOT::timeOutErr(){
  cout<<"ERR_TIME_OUT"<<endl;
}

void
ROBOT::robotMain(){
  driver.stop();
  init();
  do{
    //for real time
    semTake(timingSem, WAIT_FOREVER);
    //read
    stateTransition();
    
    if( flag.state == S_EMERGENCY_0 || flag.state == S_CTRL_STOP || flag.state == S_CTRL_RUN ){
      armCtrl( flag.state, flag.state_1 );
    }
    else if( flag.state == S_CTRL_READY ){        
      if(time%1000 == 0)
        cout<<"t= "<<time/1000<<endl;
    }
    
    if(time == 0xFFFFFFFF)
      time = 0;
    else
      time++;
  }while(flag.state != S_PRGM_EXIT);
  driver.stop();
}
