
#include "Driver.h"
#include "Device.h"
#include "InterfaceManager.h"

using namespace std;

//-----------------------------------------------------------------------------
//
Driver::Driver(int jNum){
  cout<<"Init Driver..."<<flush;

  JointNum = jNum;
  for(int type=0; type<DVC_TYPE_NUM; type++){
    J2D[type] = new int[JointNum];
    J2DT[type] = new int[JointNum];
    memset(&J2D[type][0], 0, sizeof(int)*JointNum);
    memset(&J2DT[type][0], 0, sizeof(int)*JointNum);
    Dvc[type] = NULL;
  }

  cout<<"done."<<endl;
}

//-----------------------------------------------------------------------------
//
Driver::~Driver(){
  for(int type=0; type<DVC_TYPE_NUM; type++){
    delete [] J2D[type];
    delete [] J2DT[type];

    if( Dvc[type] != NULL )
      delete [] Dvc[type];
  }

}

//-----------------------------------------------------------------------------
//
void
Driver::SetDID(int jid, DeviceType type, int dvcTag, int did){
  J2D[type][jid] = did;
  J2DT[type][jid] = dvcTag;
}

//-----------------------------------------------------------------------------
//
int
Driver::DID(DeviceType type, int jid){
  return J2D[type][jid];
}

//-----------------------------------------------------------------------------
//
int
Driver::DTg(DeviceType type, int jid){
  return J2DT[type][jid];
}


//-----------------------------------------------------------------------------
//
void
Driver::SetDeviceNum(DeviceType type, int num){
  Dvc[type] = new DeviceBase*[num];
  DvcNum[type] = num;
}

//-----------------------------------------------------------------------------
//
void
Driver::SetDevice(DeviceType type, int id, DeviceBase* ptr){
  Dvc[type][id] = ptr;
}

//-----------------------------------------------------------------------------
//
void
Driver::WriteTorque(){
  for(int id=0; id<DvcNum[ACTUATOR]; id++)
    Dvc[ACTUATOR][id]->Write();
}

//-----------------------------------------------------------------------------
//
void
Driver::ReadAngle(){
  for(int id=0; id<DvcNum[ANGLE_SENSOR]; id++)
    Dvc[ANGLE_SENSOR][id]->Read();
}

//-----------------------------------------------------------------------------
//
void
Driver::WriteAngle(){
  for(int id=0; id<DvcNum[ANGLE_SENSOR]; id++)
    Dvc[ANGLE_SENSOR][id]->Write();
}

//-----------------------------------------------------------------------------
//
void
Driver::SetAngle(int jid, double angle){
  Dvc[ANGLE_SENSOR][ J2DT[ANGLE_SENSOR][jid] ]->Set( J2D[ANGLE_SENSOR][jid],
                                                     angle );
}

//-----------------------------------------------------------------------------
//
void
Driver::SetTorque(int jid, double torque){
  Dvc[ACTUATOR][ J2DT[ACTUATOR][jid] ]->Set( J2D[ACTUATOR][jid], torque );
}

//-----------------------------------------------------------------------------
//
double
Driver::GetAngle(int jid){
  return Dvc[ANGLE_SENSOR][ J2DT[ANGLE_SENSOR][jid] ] ->
         Get( J2D[ANGLE_SENSOR][jid] );
}

//-----------------------------------------------------------------------------
//
void
Driver::TorqueZero(){
  for(int jid=0; jid<JointNum; jid++){
    this->SetTorque( jid, 0);
  }
  this->WriteTorque();
}


