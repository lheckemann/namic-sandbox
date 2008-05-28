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

STATE_MACHINE ROBOT::flag;

//Initialize
ROBOT::ROBOT():driver(),IF_manager(){
cout<<"Initialaze robot class..."<<endl;

destPee.init();
destPee_1.init();
curPee.init();
errPee.init();

for(int jID=0; jID<jNum; jID++){
joint[jID] = new JOINT(driver, jID);
}

timeLimit = wdCreate();
kinematics = new FRAME(360);

exclusMutex = semBCreate(SEM_Q_FIFO,SEM_FULL);
timingSem = semBCreate(SEM_Q_FIFO,SEM_EMPTY);

    semTake( exclusMutex, WAIT_FOREVER );
    flag.event = E.UNKNOWN;
    flag.state = S.PRGM_ENTRANCE;
    flag.state_1 = S.PRGM_ENTRANCE;
    semGive( exclusMutex );
    
    time = 0;
    
    sysClkRateSet( 5000 ); // default clock rate is 60 defined in configAll.h
    cout<<"clock:"<<sysClkRateGet()<<endl;
    
    kernelTimeSlice(1);
    taskSpawn("tTiming", 90, VX_FP_TASK, 20000, (FUNCPTR)timing,
              (int)&timingSem,0,0,0,0,0,0,0,0,0);
    
    taskSpawn("tStatTran", 95, VX_FP_TASK, 60000, (FUNCPTR)stateTransition,
               (int)&exclusMutex,(int)&IF_manager,(int)&S,(int)&E,0,0,0,0,0,0);
    
    cout<<"Robot done."<<endl;

    init();
    robotMain();
}

ROBOT::~ROBOT(){
    for(int jID=0; jID<jNum; jID++){
        delete joint[jID];
    }
    semDelete(timingSem);
    semDelete(exclusMutex);
    taskDelete(taskNameToId("tTiming"));
    taskDelete(taskNameToId("tStatTran"));
    
    delete kinematics;
    wdDelete(timeLimit);
    //logMsg("End Robot class.\n",0,0,0,0,0,0);
    cout<<"End Robot class."<<endl;
}

//Setting end efector to origin 
void
ROBOT::originSet(){
    driver.stop();
    driver.angleZeroSet();
}


// Control Joint
void
ROBOT::jointControl(double* destjAngle, double* curjAngle, JOINT_DATA* jData){
#if defined POSITION__CTRL
    driver.angleReadWrite(1);
    for(int jID=0; jID<jNum;jID++){
        jData[jID] = joint[jID]->angleControl(destjAngle[jID]);
        curjAngle[jID] = jData[jID].angle;
    }
#endif
    driver.voltWrite();
    //send IF_manager
    
}

// Routine for the internal system clock
void
ROBOT::timing (void* sem){
    SEM_ID* timing = (SEM_ID*)sem;
    do {
        taskDelay(sysClkRateGet()/1000 ); // 1ms
        semGive(*timing);
    } while(true);
}

// display data
void
ROBOT::dataDisp(int Ts){
    if( time%(Ts*1000) == 0 ){
        cout<<"t= "<<time/1000<<flush;
        cout<<"  Px= "<<(int)destPee.x<<flush;
        cout<<"  Py= "<<(int)destPee.y<<flush;
        cout<<"  Pz= "<<(int)destPee.z<<flush;
//      cout<<"  sw= "<<setw(3)<<(int)destPee.alpha<<flush;
//      cout<<"  q1= "<<setw(3)<<(int)curAngle[0]<<flush;
//      cout<<"  q2= "<<setw(3)<<(int)curAngle[1]<<flush;
        cout<<"  q2= "<<(int)destAngle[0]<<flush;
        cout<<"  Px= "<<(int)curPee.x<<flush;
        cout<<"  Py= "<<(int)curPee.y<<flush;
        cout<<"  Pz= "<<(int)curPee.z<<endl;
//
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
ROBOT::stateTransition( void* exMutex, void* manager, void* st, void* ev ){
    SEM_ID* Mutex = (SEM_ID*) exMutex;
    IF_MANAGER* ifmanager = (IF_MANAGER*) manager;
    STATE* S = (STATE*) st;
    EVENT* E = (EVENT*) ev;
    int state   = S->PRGM_ENTRANCE;
    int state_1 = S->PRGM_ENTRANCE;
    int event = E->PRGM_START;

    do{
        state_1 = state;
        // read event
        event = ifmanager->stateRead();
        //
        if( event == E->PRGM_EXIT ){
            state = S->PRGM_EXIT;
            cout<<"<event>: exit"<<endl;
        }
        else if( event == E->EMERGENCY_0 ){
            state = S->EMERGENCY_0;
            cout<<"<event>: emg"<<endl;
        }
        else if( event == E->PRGM_START
                 && ( state == S->PRGM_ENTRANCE || state ==  S->EMERGENCY_0 ) ){
            state = S->CTRL_READY;
            cout<<"<event>: prgm_start"<<endl;
        }
        else if( event == E->CTRL_STOP_START
                 && ( state == S->CTRL_READY || state ==  S->EMERGENCY_0 ) ){
            state = S->CTRL_STOP;
            cout<<"<event>: ctrl_stop_start"<<endl;
        }
        else if( event == E->CTRL_STOP_STOP
                 && ( state == S->CTRL_STOP ||  state == S->EMERGENCY_0 ) ){
            state = S->CTRL_READY;
            cout<<"<event>: ctrl_stop_stop"<<endl;
        }
        else if( event == E->CTRL_RUN_START && state == S->CTRL_STOP ){
            state = S->CTRL_RUN;
            cout<<"<event>: ctrl_run"<<endl;
        }
        else if( event == E->CTRL_RUN_STOP
                 && ( state == S->CTRL_RUN || state == S->EMERGENCY_0 ) ){
            state = S->CTRL_STOP;
            cout<<"<event>: ctrl_stop"<<endl;
        }
        
        if( state != state_1 ){
            semTake( *Mutex, WAIT_FOREVER );
            flag.event = event;
            flag.state = state;
            flag.state_1 = state_1;
            semGive( *Mutex );
        }
        //cout<<"state"<<state<<endl;
        //cout<<"event"<<event<<endl;
    }while( state != S->PRGM_EXIT );
}

void
ROBOT::robotMain(){
    int state;
    int state_1;
    do{
        semTake(timingSem, WAIT_FOREVER);

        semTake( exclusMutex, NO_WAIT );
        state = flag.state;
        state_1 = flag.state_1;
        semGive( exclusMutex );
        
        if( state == S.EMERGENCY_0 || state == S.CTRL_STOP || state == S.CTRL_RUN ){
            armCtrl();
        }
        else if( state == S.CTRL_READY ){
            init();
            do{
                semTake(timingSem, WAIT_FOREVER);
                
                semTake( exclusMutex, NO_WAIT );
                state = flag.state;
                state_1 = flag.state_1;
                semGive( exclusMutex );
                if(time%1000 == 0){
                    cout<<"t= "<<time/1000<<endl;
                }
                time++;
            }while(state == S.CTRL_READY );
        }
        
    }while(state != S.PRGM_EXIT);
}


void
ROBOT::init(){
    dx = dy = 0;
    
    for(int jID=0; jID<jNum; jID++){
        destAngle[jID] = 0;
        curAngle[jID] = 0;
    }
    
    originSet();
    //save = new SAVE(endtime*1000);
}

void
ROBOT::destDataGet(int state, int state_1 , unsigned long ctrlTime){
    
    if( state == S.CTRL_RUN && state_1 == S.CTRL_STOP){
        //
        IF_manager.destEEPositionGet(0, &destPee);
        dx = destPee.x - destPee_1.x;
        dy = destPee.y - destPee_1.y;
        dz = destPee.z - destPee_1.z;
        destPee.x -= dx;
        destPee.y -= dy;
        destPee.z -= dz;
        cout<<"ON"<<endl;
    }
    else if( state == S.CTRL_RUN && state_1 ==  S.CTRL_RUN){
        //
        IF_manager.destEEPositionGet(0, &destPee);
        destPee.x -= dx;
        destPee.y -= dy;
        destPee.z -= dz;
    }
    else if( state == S.CTRL_RUN && state_1 == S.CTRL_STOP){
        destPee_1.x = destPee.x -= dx;
        destPee_1.y = destPee.y -= dy;
        destPee_1.z = destPee.z -= dz;
        cout<<"OFF"<<endl;
    }
    else if( state == S.CTRL_STOP && state_1 == S.CTRL_STOP){
        destPee.x = destPee_1.x;
        destPee.y = destPee_1.y;
        destPee.z = destPee_1.z;
    }
    else if( state == S.EMERGENCY_0 ){
        //ctrlTime;
    }
}

void
ROBOT::timeOutErr(){
    cout<<"ERR_TIME_OUT"<<endl;
}

//Main loop in the bobot
void
ROBOT::armCtrl(){
    int state;
    int state_1;
    do{
        //
        semTake(timingSem, WAIT_FOREVER);
        //watichdog timer start
        wdStart(timeLimit, sysClkRateGet()/1000, (FUNCPTR)timeOutErr, 0 );
        //
        semTake( exclusMutex, NO_WAIT );
        state = flag.state;
        state_1 = flag.state_1;
        semGive(exclusMutex);
        
        destDataGet(state, state_1, time);
        kinematics->calculateInvKinematics(&destPee, 4, destAngle);
        jointControl(destAngle, curAngle, curJoint);
        kinematics->calculateKinematics(4, curAngle, &curPee);
        bilateralErrCaluculate(&destPee, &curPee, &errPee);
        
        dataDisp(1);
        
        if(time%2 == 0)
            IF_manager.errEEPositionSend(&errPee);
        
        // for over flow
        if(time == 0xFFFFFFFF)
            time = 0;
        else
            time++;
        //watichdog timer stop
        wdCancel(timeLimit);
    } while (state == S.CTRL_STOP || state == S.CTRL_RUN || state == S.EMERGENCY_0 );
}

