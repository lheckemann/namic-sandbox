/******************************************************************************
 * FileName      : Device.cpp
 * Created       : 2008/11/29
 * LastModified  : 2008//
 * Author        : Hiroaki KOZUKA
 * Aim           : device class
 *****************************************************************************/
#include "Device.h"
#include "BoardBase.h"
using namespace std;
//-----------------------------------------------------------------------------
//
DeviceBase::DeviceBase(){
//  std::cout<<"Start DeviceBase"<<std::endl;
}

//-----------------------------------------------------------------------------
//
DeviceBase::~DeviceBase(){
//  std::cout<<"End DeviceBase"<<std::endl;
}

//-----------------------------------------------------------------------------
//
void
DeviceBase::Read(){
}

//-----------------------------------------------------------------------------
//
void
DeviceBase::Write(){
}

//-----------------------------------------------------------------------------
//
void
DeviceBase::Set(int id, double data){
}

//-----------------------------------------------------------------------------
//
double
DeviceBase::Get( int id ){
  return 0;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
Encoder::Encoder(){
  cout<<"Init encoder class..."<<flush;
  BrdNum = 0;
  Brd = NULL;
  cout<<"done."<<endl;
}

//-----------------------------------------------------------------------------
//
Encoder::~Encoder(){
  delete [] PluseNum;
  delete [] Direct;
  delete [] Pluse;
  delete [] Angle;
  delete [] SpeedRadio;
  if(BrdNum>0)
    delete [] Brd;
  cout<<"End encoder"<<endl;
}

//-----------------------------------------------------------------------------
//
void
Encoder::SetDvcNum(int num){
  Num = num;
  PluseNum = new double[Num];
  Direct = new double[Num];
  Pluse = new long* [Num];
  Angle = new double[Num];
  SpeedRadio = new double[Num];
  NaData = 0;

  memset( &PluseNum[0], 0, sizeof(double)*Num );
  memset( &Direct[0], 0, sizeof(double)*Num );
  memset( &Angle[0], 0, sizeof(double)*Num );
  memset( &SpeedRadio[0], 0, sizeof(double)*Num);
  for(int i=0; i<Num; i++){
    Pluse[i] = &NaData;
  }
}

//-----------------------------------------------------------------------------
//
bool
Encoder::InitParam(int id, int direct, long pluse, double spdr ){
  if(id<Num && id>=0){
    PluseNum[id] = (double)pluse*4;
    Direct[id] = (double)direct;
    SpeedRadio[id] = spdr;
    return true;
  }
  else{
    return false;
  }
}

//-----------------------------------------------------------------------------
//
void
Encoder::Connect(int id, long* ptr){
  Pluse[id] = ptr;
}

//-----------------------------------------------------------------------------
//
void
Encoder::Set(int id, double angle){
  Angle[id] = angle;
}

//-----------------------------------------------------------------------------
//
double
Encoder::Get(int id){
  return Angle[id];
}


//-----------------------------------------------------------------------------
//
void
Encoder::SetBoardNum(int brdNum){
  Brd = new DeviceBase::Board[brdNum];
  memset(Brd, 0, sizeof(DeviceBase::Board)*brdNum );
  BrdNum = brdNum;
}

//-----------------------------------------------------------------------------
//
void
Encoder::SetBoard(int id, BoardBase* ptr, int unitNum){
  Brd[id].BrdPtr = ptr;
  Brd[id].UnitNum = unitNum;
}


//-----------------------------------------------------------------------------
//
void
Encoder::Read(){
  for(int id=0; id<BrdNum; id++){
    Brd[id].BrdPtr->Read(Brd[id].UnitNum, BoardBase::TYPE_CNT);
  }
  for(int i=0; i<Num; i++){
    Angle[i] = 2.0*PI*Direct[i]*(double)(*(Pluse[i])/PluseNum[i])/SpeedRadio[i];
  }
}

//-----------------------------------------------------------------------------
//
void
Encoder::Write(){
  for(int i=0; i<Num; i++){
   *(Pluse[i])  = (long)( Direct[i]*Angle[i]*SpeedRadio[i]*PluseNum[i]/(2.0*PI) );
  }
  for(int id=0; id<BrdNum; id++){
    Brd[id].BrdPtr->Write(Brd[id].UnitNum, BoardBase::TYPE_CNT);
  }

}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//
Motor::Motor(){
  cout<<"Init motor class..."<<flush;
  BrdNum = 0;
  Brd = NULL;
  cout<<"done."<<endl;
}

//-----------------------------------------------------------------------------
//
Motor::~Motor(){
  delete [] MtData;
  if(BrdNum>0)
    delete [] Brd;
  cout<<"End motor"<<endl;
}

//-----------------------------------------------------------------------------
//
void
Motor::SetDvcNum(int num){
  Num = num;
  NaData = 0;

  MtData = new MotorData[Num];

  for(int i=0; i<Num; i++){
    memset( &MtData[i], 0, sizeof(MotorData) );
    MtData[i].Volt = &NaData;
    MtData[i].OutDirect = &NaData;
  }

}


//-----------------------------------------------------------------------------
//
void
Motor::InitParam(int id, double dir, double max, double min, double t2v ,double spdr ){
  if(id<Num && id>=0){
    MtData[id].Direct = dir;
    MtData[id].MaxVolt = max;
    MtData[id].MinVolt = min;
    MtData[id].TorqueToVolt = t2v;
    MtData[id].SpeedRadio = spdr;
  }
}


//-----------------------------------------------------------------------------
//
void
Motor::Connect( int id, double* ptr1, double* ptr2){
  MtData[id].Volt = ptr1;
  if(ptr2 != NULL)
    MtData[id].OutDirect = ptr2;
}


//-----------------------------------------------------------------------------
//
void
Motor::Connect( int id, double* ptr1){
  MtData[id].Volt = ptr1;
}

//-----------------------------------------------------------------------------
//
void
Motor::SetBoardNum(int brdNum){
  Brd = new DeviceBase::Board[brdNum];
  memset(Brd, 0, sizeof(DeviceBase::Board)*brdNum );
  BrdNum = brdNum;
}

//-----------------------------------------------------------------------------
//
void
Motor::SetBoard(int id, BoardBase* ptr, int unitNum){
  Brd[id].BrdPtr = ptr;
  Brd[id].UnitNum = unitNum;
}

//-----------------------------------------------------------------------------
//
void
Motor::RegulatVolt1(int id, double& volt, double& direct){

  if(volt < 0 ){
    volt = -volt;
    direct = 5;
  }
  else{
    direct = 0;
  }

  if( volt >= 0.01 && volt <= MtData[id].MaxVolt ){
    return;
  }
  else if( volt > MtData[id].MaxVolt ){
    volt = MtData[id].MaxVolt;
  }
  else {
    volt = 0;
  }

}


//-----------------------------------------------------------------------------
//
void
Motor::RegulatVolt2(int id, double& volt){

  double OriginVolt = ( MtData[id].MaxVolt - MtData[id].MinVolt )/2;

  if( volt > (OriginVolt - 0.01) && volt < (OriginVolt + 0.01) ){
    volt = OriginVolt;
  }
  else if( volt >= MtData[id].MinVolt && volt<=MtData[id].MaxVolt ){
    return;
  }
  else if( volt < MtData[id].MinVolt ){
    volt = MtData[id].MinVolt;
  }
  else if( volt > MtData[id].MaxVolt ){
    volt = MtData[id].MaxVolt;
  }
  else{
    volt = OriginVolt;
  }

}


//-----------------------------------------------------------------------------
//
void
Motor::Write(){
  for(int id=0; id<Num; id++){
    *(MtData[id].Volt) = MtData[id].Direct *
                         MtData[id].Torque * MtData[id].TorqueToVolt;

    if( MtData[id].OutDirect != &NaData )
      this->RegulatVolt1( id, *(MtData[id].Volt), *(MtData[id].OutDirect) );
    else{
      this->RegulatVolt2( id, *(MtData[id].Volt) );
    }
  }
  for(int id=0; id<BrdNum; id++){
    Brd[id].BrdPtr->Write(Brd[id].UnitNum, BoardBase::TYPE_DA);
  }
}

void
Motor::Set(int id, double t){
  MtData[id].Torque = t;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//
SW::SW(int chNum1, int chNum2, int chNum3){
  cout<<"Init Switch..."<<flush;
  BrdNum = 0;
  Brd = NULL;

  ChNum1 = chNum1;
  ChNum2 = chNum2;
  ChNum3 = chNum3;

  Data[0] = new bool[chNum1];
  Data[1] = new bool[chNum2];
  Data[2] = new bool[chNum3];

  DataPtr[0] = new bool*[chNum1];
  DataPtr[1] = new bool*[chNum2];
  DataPtr[2] = new bool*[chNum3];

  CntSW[0] = new long [chNum2];
  CntSW[1] = new long [chNum3];

  NaData = false;

  for(int i=0; i<chNum1; i++){
    Data[0][i] = false;
    DataPtr[0][i] = &NaData;
  }

  for(int i=0; i<chNum2; i++){
    Data[1][i] = false;
    DataPtr[1][i] = &NaData;
    CntSW[0][i] = 0;
  }

  for(int i=0; i<chNum3; i++){
    Data[2][i] = false;
    DataPtr[2][i] = &NaData;
    CntSW[1][i] = 0;
  }

  cout<<"done."<<endl;

}

//-----------------------------------------------------------------------------
//
SW::~SW(){

  delete [] Data[0];
  delete [] Data[1];
  delete [] Data[2];

  delete [] DataPtr[0];
  delete [] DataPtr[1];
  delete [] DataPtr[2];

  delete [] CntSW[0];
  delete [] CntSW[1];

  if(BrdNum>0)
    delete [] Brd;
}


//-----------------------------------------------------------------------------
//
void
SW::SetDvcNum(int num){

}


//-----------------------------------------------------------------------------
//
void
SW::Connect(int Type, int ch, bool* ptr){
  DataPtr[Type][ch] = ptr;
}

//-----------------------------------------------------------------------------
//
void
SW::SetBoardNum(int brdNum){
  Brd = new DeviceBase::Board[brdNum];
  memset(Brd, 0, sizeof(DeviceBase::Board)*brdNum );
  BrdNum = brdNum;
}

//-----------------------------------------------------------------------------
//
void
SW::SetBoard(int id, BoardBase* ptr, int unitNum){
  Brd[id].BrdPtr = ptr;
  Brd[id].UnitNum = unitNum;
}

//-----------------------------------------------------------------------------
//
bool
SW::SW1(int ch){
  return *DataPtr[0][ch];
}

//-----------------------------------------------------------------------------
//
bool
SW::SW2(int ch){
  if( *(DataPtr[1][ch]) ){
    CntSW[0][ch]++;
  }
  else{
    if( CntSW[0][ch]>150 ){
      if( Data[1][ch] ){
        Data[1][ch] = false;
      }
      else{
        Data[1][ch] = true;
      }
      CntSW[0][ch] = 0;
    }
    else{
      CntSW[0][ch] = 0;
    }
  }

  return Data[1][ch];
}

//-----------------------------------------------------------------------------
//
bool
SW::SW3(int ch){
  if( *(DataPtr[2][ch]) ){
    CntSW[1][ch]++;

    if( CntSW[1][ch]>150 ){
      if( Data[2][ch] ){
        Data[2][ch] = false;
      }
      else{
        Data[2][ch] = true;
      }
    }
  }
  else{
    CntSW[1][ch] = 0;
  }

  return Data[2][ch];
}


