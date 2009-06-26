/******************************************************************************
 * FileName      : Robot.cpp
 * Created       : 2008/11/1
 * LastModified  : 2009/
 * Author        : Hiroaki KOZUKA
 * Aim           : Main program for a slave Robot Contol
 *                 robot class
 * OS            : VxWorks 5.5.1
 *****************************************************************************/
#include "Robot.h"

//-----------------------------------------------------------------------------
//
Robot::Robot(int jNum){
  cout<<"Init Robot..."<<endl;

  SInfo.Event = E_PRGM_START;
  SInfo.State = S_PRGM_ENTRANCE;
  SInfo.State_1 = S_PRGM_ENTRANCE;

  JointNum = jNum;
  Dr = NULL;


  destAngle = new double[JointNum];
  curAngle = new double[JointNum];
  Angle_0 = new double[JointNum];

  memset(destAngle, 0, sizeof(double)*JointNum);
  memset(curAngle, 0, sizeof(double)*JointNum);
  memset(&destPee, 0, sizeof(Coord_6DoF));
  memset(&destPee_1, 0, sizeof(Coord_6DoF));

  Jt = new JointBase*[JointNum];
  for(int j=0;j<jNum; j++ )
    Jt[j] = NULL;

  Dr = new Driver(JointNum);
  IFM.SetJointNum(jNum);

  timingSem = semBCreate(SEM_Q_FIFO, SEM_EMPTY);

  //task (thread)
  kernelTimeSlice(1);
  taskSpawn( "tTiming", 90, VX_FP_TASK, 20000, (FUNCPTR)Timing,
             (int)&timingSem,0,0,0,0,0,0,0,0,0 );

  cout<<"Robot done."<<endl;
}

//-----------------------------------------------------------------------------
//
Robot::~Robot(){

  delete [] destAngle;
  delete [] curAngle;
  delete [] Angle_0;

  delete [] Jt;
  delete Dr;
  taskDelete( taskNameToId("tTiming"));

  cout<<"End Robot."<<endl;
}


//
double
Robot::TimeGet(){
  UINT32 tbu, tbl;
  vxTimeBaseGet(&tbu, &tbl);
  return (tbu * 4294967296.0 + tbl)/25000000.0;
}



//-----------------------------------------------------------------------------
//
void
Robot::SetJoint(int jID, JointBase* ptr){
  Jt[jID] = ptr;
  Jt[jID]->SetIFM(&IFM);
}

//-----------------------------------------------------------------------------
//
void
Robot::SetFrameInv(FrameBase* ptr){
  FrmI = ptr;
}

//-----------------------------------------------------------------------------
//
void
Robot::SetFrameFor(FrameBase* ptr){
  FrmF = ptr;
}

//-----------------------------------------------------------------------------
//
Driver*
Robot::DrPtr(){
  return Dr;
}


//-----------------------------------------------------------------------------
// Routine for the internal system clock
void
Robot::Timing (void* sem){
  SEM_ID* timing = (SEM_ID*)sem;
  sysClkRateSet( 1000 ); // default clock rate is 60 defined in configAll.h
  do {
    taskDelay(sysClkRateGet()/1000); // 1ms
    semGive(*timing);
  } while(true);
}

//-----------------------------------------------------------------------------
//
void
Robot::StateTransition(StateInfo& S){
  // read event
  STATE state = S.State;
  STATE state_1 = S.State;
  EVENT event = GetEvent();
  static bool stateSWFlag = 0;

  //
  if( event == E_PRGM_EXIT ){
    state = S_PRGM_EXIT;
//    cout<<"exit"<<endl;
  }
  else if( event == E_EMERGENCY_0 ){
    state = S_EMERGENCY_0;
//    cout<<"emg"<<endl;
  }
  else if( event == E_PRGM_START
           && ( state == S_PRGM_ENTRANCE || state == S_EMERGENCY_0 ) ){
    state =  S_CTRL_READY;
//    cout<<"prgm_start"<<endl;
  }
  else if( (event == E_INIT)
           && ( state == S_CTRL_READY || state ==  S_EMERGENCY_0 ) ){
    state = S_INIT;
//    cout<<"init_prgm"<<endl;
  }
  else if( (event == E_CTRL_STOP_START)
           && ( state == S_INIT || state ==  S_EMERGENCY_0 ) ){
    state = S_CTRL_STOP;
//    cout<<"ctrl_stop_start"<<endl;
  }
  else if( event == E_CTRL_STOP_STOP
           && ( state == S_CTRL_STOP ||  state == S_EMERGENCY_0 ) ){
    state = S_CTRL_READY;
//    cout<<"ctrl_stop_stop"<<endl;
  }
  else if( (event == E_CTRL_RUN_START)
           && state == S_CTRL_STOP ){
    state = S_CTRL_RUN;
//    cout<<"ctrl_run"<<endl;
  }
  else if( event == E_CTRL_RUN_STOP
           && ( state == S_CTRL_RUN || state == S_EMERGENCY_0 ) ){
    state = S_CTRL_STOP;
//    cout<<"ctrl_stop"<<endl;
  }

  if( state != state_1 ){
    S.State = state;
    S.State_1 = state_1;
    S.Event = event;
//    cout<<"state:"<<state<<" state_1:"<<state_1<<" event:"<<event<<endl;
    stateSWFlag = true;
  }
  else {
    if(stateSWFlag){
      S.State = state;
      S.State_1 = state_1;
      S.Event = event;
      stateSWFlag = false;
    }
    else{
      stateSWFlag = false;
    }
  }
  SetState(S.State);
}

//-----------------------------------------------------------------------------
//Setting end efector to origin
void
Robot::SetAngOrigin(int jID, double angle){
  Angle_0[jID] = angle;
}

//-----------------------------------------------------------------------------
//
void
Robot::SetPosOrigin(double x,double y, double z,
                    double alpha,double beta,double gamma ){
  destPee_0.x = x;
  destPee_0.y = y;
  destPee_0.z = z;

  destPee_0.alpha = alpha;
  destPee_0.beta  = beta;
  destPee_0.gamma = gamma;

}

//-----------------------------------------------------------------------------
//
void
Robot::Init(StateInfo& S){
  Dr->TorqueZero();
  destPee.x = destPee_1.x = destPee_0.x;
  destPee.y = destPee_1.y = destPee_0.y;
  destPee.z = destPee_1.z = destPee_0.z;

  destPee.alpha = destPee_1.alpha = destPee_0.alpha;
  destPee.beta  = destPee_1.beta  = destPee_0.beta;
  destPee.gamma = destPee_1.gamma = destPee_0.gamma;

  for(int i=0; i<JointNum; i++){
    destAngle[i] = Angle_0[i];
    Jt[i]->SetAngle(destAngle[i]);
  }
  Dr->WriteAngle();
}

//-----------------------------------------------------------------------------
//
void
Robot::Exit(){
  Dr->TorqueZero();
}

void
Robot::GetDestPosition( StateInfo& S, Coord_6DoF& Pee, Coord_6DoF& Pee_1){
  static double dx = 0;
  static double dy = 0;
  static double dz = 0;
  static double alpha = 0;
  static double beta = 0;
  static double gamma = 0;

  if( S.State == S_CTRL_RUN && (S.State_1 == S_CTRL_STOP) ){
    dx = Pee.x - Pee_1.x;
    dy = Pee.y - Pee_1.y;
    dz = Pee.z - Pee_1.z;
    alpha  = Pee.alpha - Pee_1.alpha;
    beta   = Pee.beta  - Pee_1.beta;
    gamma  = Pee.gamma - Pee_1.gamma;

//    cout<<"<"<<ROBOT_NAME<<">: ON "<<dz<<" "<<Pee_1.z<<" "<<Pee.z<<endl;
    Pee.x -= dx;
    Pee.y -= dy;
    Pee.z -= dz;
    Pee.alpha -= alpha;
    Pee.beta -= beta;
    Pee.gamma -= gamma;

  }
  else if( S.State == S_CTRL_RUN && S.State_1 ==  S_CTRL_RUN){
    Pee.x -= dx;
    Pee.y -= dy;
    Pee.z -= dz;
    Pee.alpha -= alpha;
    Pee.beta -= beta;
    Pee.gamma -= gamma;
//    if(Time%10 == 0)
//      cout<<"ON: "<<Pee.x<<endl;
  }
  else if( S.State == S_CTRL_STOP && S.State_1 == S_CTRL_RUN){
    Pee_1.x = Pee.x -= dx;
    Pee_1.y = Pee.y -= dy;
    Pee_1.z = Pee.z -= dz;
    Pee_1.alpha = Pee.alpha -= alpha;
    Pee_1.beta  = Pee.beta  -= beta;
    Pee_1.gamma = Pee.gamma -= gamma;
    cout<<"<"<<ROBOT_NAME<<">: OFF "<<dx<<endl;
  }
  else if( S.State == S_CTRL_STOP &&
           ( S.State_1 == S_CTRL_STOP || S.State_1== S_INIT) ){
    Pee = Pee_1;
  }
  else if( S.State == S_EMERGENCY_0 ){
    //
  }
}

//-----------------------------------------------------------------------------
//
void
Robot::Control( StateInfo& S ){
  static Coord_6DoF curPee;

  Dr->ReadAngle();
//  IFM.ReceiveData(destPee);
//  this->GetDestPosition(S, destPee, destPee_1);

  static const double omega = 0.1;
  static const double a = 10.0;
  static const double aa = 20.0*PI/180.0;
  static long long time = 0;

  if( S.State == S_CTRL_RUN ){
    time++;
//    destPee.x = a*sin(2.0*PI*omega*(double)time/1000.0);
//    destPee.y = a*sin(2.0*PI*omega*(double)time/1000.0);
//    destPee.z = a*cos(2.0*PI*omega*2.0*(double)time/1000.0) + destPee_0.z - a;

    destAngle[0] = aa*sin(2.0*PI*omega*(double)time/1000.0);
    destAngle[1] = aa*sin(2.0*PI*omega*(double)time/1000.0);
    destAngle[2] = 0.0;

    if( time%1000 == 0 ){
      cout<<"<jawa>: "<<"t:"<<time/1000<<" "<<destAngle[0]*180/PI<<" "<<curPee.x<<endl;
        //<<" "<<destPee.x<<" Py:"<<destPee.y<<" Pz:"<<destPee.z<<endl;
    }
  }
//  FrmI->InvKinematics(destPee, JointNum, destAngle);

  for(int j=0; j<JointNum; j++){
    curAngle[j]= Dr->GetAngle(j);
    Jt[j]->CtrlAngle(destAngle[j]);
  }

  FrmF->Kinematics(JointNum, destAngle, curPee);

  IFM.SendDataTf(curPee);
  IFM.SendData(destPee);
  IFM.SendcurData(curPee);
  IFM.SendDestAngle(destAngle);
  IFM.SendCurAngle(curAngle);
  IFM.SendTime();

  Dr->WriteTorque();
}

//-----------------------------------------------------------------------------
//
void
Robot::RobotMain(void* ptr){
  Robot* Rt = (Robot*)ptr;
  Rt->Dr->TorqueZero();
  double dt = 0;
  double tt = 0;

  WDOG_ID TimeLimit = wdCreate();

  do{
    semTake(Rt->timingSem, WAIT_FOREVER);
    wdStart(TimeLimit, 1, (FUNCPTR)TimeErr, 0 );
    tt = Rt->TimeGet();

    Rt->IFM.ReceiveData();

    Rt->StateTransition(Rt->SInfo);

    if(Rt->SInfo.State == S_INIT){
      Rt->Init(Rt->SInfo);
    }
    else if( Rt->SInfo.State == S_CTRL_STOP || Rt->SInfo.State == S_CTRL_RUN){
      Rt->Control(Rt->SInfo);
    }

    Rt->IFM.SendTime_T( dt*1000000000.0 );

    Rt->IFM.SendData();

    Rt->CountUpTime();

    dt = Rt->TimeGet() - tt;

    wdCancel(TimeLimit);
  }while(Rt->SInfo.State != S_PRGM_EXIT);

  wdDelete(TimeLimit);
  Rt->Exit();

}

void
Robot::TimeErr(unsigned long long T){
  cout<<"Time out:"<<T<<endl;
}



