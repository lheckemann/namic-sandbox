/***************************************************************************
 * FileName      : IFmanager.cpp
 * Created       : 2007/12/2
 * LastModified  : 2008/
 * Author        : Hiroaki KOZUKA
 * Aim           : management interface classes
 *                 IFmanager class
 * OS            : VxWorks 5.5.1
 ******************z*********************************************************/
#include "IFmanager.h"

PNTM_INTERFACE*  PNTM_INTERFACE::IF_Instance = NULL;
//STATE_MACHINE IF_MANAGER::flag;

IF_MANAGER::IF_MANAGER(){ 
  //logMsg("Initialaze Interface Manager Class...\n",0,0,0,0,0,0);
    cout<<"Initialaze Interface Manager Class..."<<endl;
    //manage flag
    SSFlag = OFF;
    armFlag = OFF;
    PeeFlag = ON;
    jointGetFlag = OFF;
    driverFlag = OFF;
    
    // semaphore
    exclusMutex   = semBCreate(SEM_Q_FIFO, SEM_FULL);
    
    // create socket interface
    // Master:sumatra , Slave:jawa
    slave = PNTM_INTERFACE::createInterface( "jawa", "sumatra",
                                              SLAVE_TO_MASTER_PORT, MASTER_TO_SLAVE_PORT  );
    keyCon = new KEYCONSOLE();
    
    //logMsg("Interface Manager Done.\n",0,0,0,0,0,0);
    cout<<"Interface Manager Done."<<endl;
}

IF_MANAGER::~IF_MANAGER(){
  //sem
  semDelete( exclusMutex );
  
  //I/F
  delete slave;
  //delete keyCon;
  PNTM_INTERFACE::IF_Instance = NULL;
  //logMsg("End Interface Manager.\n",0,0,0,0,0,0);
  cout<<"End Interface Manager."<<endl;
}

float
IF_MANAGER::W_BitSwap( const float* ptr){
    float data = 0;
    const unsigned char *p  = (unsigned char *)ptr;
    unsigned char * p_ = (unsigned char*)&data;
    p_+= 3;
    for(int i=0; i<4; ++i, ++p, --p_){
        *p_ = *p;
    }
    return data;
}

int
IF_MANAGER::byteSwap( const void* ptr, const void* ptr_, unsigned char size){
    // top address into pointer
    const unsigned char* p  = (unsigned char*) ptr;
    unsigned char* p_ = (unsigned char*) ptr_;
    // swapping earh byte
    switch(size){
        case 2:
            for(int i=0; i<2; ++i, ++p){
                *(p_+1-i) = *p;
            }
            break;
        case 4:
            for(int i=0; i<4; ++i, ++p){
                *(p_+3-i) = *p;
            }
            break;
        case 8:
            for(int i=0; i<8; ++i, ++p){
                *(p_+7-i) = *p;
            }
            break;
        default:
            return -1;
    }
    return 1;
}

int
IF_MANAGER::INT_BitSwap(int data){
    int Sdata;
    Sdata  = ((UINT32)data>>24 & (UINT32)255);
    Sdata += ((UINT32)data>>8  & (UINT32)255<<8);
    Sdata += ((UINT32)data<<8  & (UINT32)255<<16);
    Sdata += ((UINT32)data<<24 & (UINT32)255<<24);
    return Sdata;
}

void
IF_MANAGER::errEEPositionSend(EE_POSITION* err){
    sPee.x = (float)err->x;
    sPee.y = (float)err->y;
    sPee.z = (float)err->z;
    sPee.alpha = (float)err->alpha;
    sPee.beta  = (float)err->beta;
    sPee.gamma = (float)err->gamma;
//  slave->send(&sPee);
}


void
IF_MANAGER::robotDataSend(ARM_DATA* arm){
    
}

void
IF_MANAGER::jointDataSend(JOINT_DATA* joint){
    //
    jointData = *joint;
    //
    //etc...
}

void
IF_MANAGER::driverDataSend(DRIVER_STAT_DATA* driver){
    
}

int
IF_MANAGER::destEEPositionGet(int jID, EE_POSITION* destPosition){
    // 2008 2/6 for open lab kozuka
    double scale = 0.1;
    EE_POSITION position = *destPosition;
    slave->recieve(&rPee);
    
    destPosition->z = rPee.x*scale;
    destPosition->x = rPee.y*scale;
    destPosition->y = -rPee.z*scale;
    destPosition->alpha = 0;
    destPosition->beta  = 0;
    destPosition->gamma = 0;
    
    return 0;
}

int
IF_MANAGER::stateRead(int state ){
    int keyFlag = keyCon->stateRead();
    int slaveFlag = slave->stateRead();
    int flag = keyFlag;

    if( keyFlag == E.CTRL_STOP_START && (state == S.CTRL_STOP || state == S.CTRL_RUN ) ){
        flag = E.CTRL_RUN_START;//slaveFlag;
    }

    return flag;
}
