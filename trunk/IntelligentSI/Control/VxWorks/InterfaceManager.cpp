/******************************************************************************
 * FileName      : InterfaceManager.cpp
 * Created       : 2007/12/2
 * LastModified  : 2008/
 * Author        : Hiroaki KOZUKA
 * Aim           : management interface classes
 *
 * OS            : VxWorks 5.5.1
 *****************************************************************************/
#include "InterfaceManager.h"

using namespace std;

//-----------------------------------------------------------------------------
//
InterfaceManager::InterfaceManager(){
  cout<<"Init InterfaceManager..."<<flush;

  CFlag = new bool[IFBDS.GetSDTypeNum()];
  SFlag = new bool[IFBDS.GetRDTypeNum()];

  CIFMsgQID = new vector<IFMsgQID> [ IFBDS.GetSDTypeNum() ];
  SIFMsgQID = new IFMsgQID [ IFBDS.GetRDTypeNum() ];

  save = new SAVE( 1000, 0.01, 9, 10 );
  this->Init();

  cout<<"done."<<endl;
}

//-----------------------------------------------------------------------------
//
InterfaceManager::~InterfaceManager(){
  delete save;
  delete [] CFlag;
  delete [] SFlag;
  delete [] CIFMsgQID;
  delete [] SIFMsgQID;
  std::cout<<"End InterfaceManager."<<std::endl;
}

//-----------------------------------------------------------------------------
//
void
InterfaceManager::Init(){
  for( int CType=0; CType<IFBDS.GetSDTypeNum(); CType++){
    CFlag[CType] = false;
  }

  for( int SType=0; SType<IFBDS.GetRDTypeNum(); SType++){
    SIFMsgQID[SType] = GetRMsgQID(SType);
    cout<<"RData:"<<(int)SIFMsgQID[SType].MID<<" type:"<<SType<<endl;
  }

  for( int CType=0; CType<IFBDS.GetSDTypeNum(); CType++){
    for(int num=0; num< GetSMsgQIDNum(CType); num++){
      cout<<"SData:"<<(int)GetSMsgQID(CType, num).MID<<" type:"<<CType<<endl;
      CIFMsgQID[CType].push_back( GetSMsgQID(CType, num) );
    }
  }

  R_TF.SetInfo(SERVER, "TRANSFORM");
  R_Pee.SetInfo(SERVER, "POSITION");
  R_Fee.SetInfo(SERVER, "FORCE");
  R_TIME.SetInfo(SERVER,"TIMESTAMP");

  S_TF.SetInfo(CLIENT,"TRANSFORM");
  S_Pee.SetInfo(CLIENT, "POSITION");
  S_curPee.SetInfo(CLIENT, "C_POSITION");
  S_Fee.SetInfo(CLIENT, "FORCE");
  S_CAG.SetInfo(CLIENT, "CUR_ANGLES");
  S_DAG.SetInfo(CLIENT, "DEST_ANGLES");
  S_TIME.SetInfo(CLIENT, "TIMESTAMP");
  S_TIME_T.SetInfo(CLIENT, "TIME_TRNSCT");
  /*
  memset(&S_DestAngle, 0, sizeof(M_BUFF) );
  memset(&S_CurAngle, 0, sizeof(M_BUFF) );
  memset(&S_Pee, 0, sizeof(M_EE_POSITION));
  memset(&S_Fee, 0, sizeof(M_EE_FORCE));
  memset(&S_Event, 0, sizeof(M_EVENT));
  memset(&S_Tf, 0, sizeof(M_EE_TRANSFORM));
  memset(&S_PosLT, 0, sizeof(M_EE_POSITION_LT) );

  memset(&R_Buff, 0, sizeof(M_BUFF) );
  memset(&R_DestAngle, 0, sizeof(M_BUFF) );
  memset(&R_CurAngle, 0, sizeof(M_BUFF) );
  memset(&R_Pee, 0, sizeof(M_EE_POSITION));
  memset(&R_Fee, 0, sizeof(M_EE_FORCE));
  memset(&R_Event, 0, sizeof(M_EVENT));
  memset(&R_Tf, 0, sizeof(M_EE_TRANSFORM));
  memset(&R_PosLT, 0, sizeof(M_EE_POSITION_LT) );
  */
  R_TF.Body()[0] = R_TF.Body()[4] = R_TF.Body()[8] = 1;
  S_TF.Body()[0] = S_TF.Body()[4] = S_TF.Body()[8] = 1;

//  SetCMsgQID( C_POSITION, save, 200, 100,  save->GetMsgQID() );
//  SetCMsgQID( C_DEST_ANGLE, save, 200, 100,  save->GetMsgQID() );
//  SetCMsgQID( C_CUR_ANGLE, save, 200, 100,  save->GetMsgQID() );
}

//-----------------------------------------------------------------------------
//
void
InterfaceManager::SetJointNum(int jNum){
  JointNum = jNum;
  R_TF.Header().JNum = R_Pee.Header().JNum = R_Fee.Header().JNum = JointNum;
  S_TF.Header().JNum = S_Pee.Header().JNum = S_Fee.Header().JNum = JointNum;
}



//-----------------------------------------------------------------------------
//
void
InterfaceManager::SendData(){
  unsigned long long Time = GetTime();
  int typeNum = IFBDS.GetSDTypeNum();

  for(int Type=0; Type<typeNum; Type++){
    vector<IFMsgQID>::iterator itr = CIFMsgQID[ Type].begin();
    vector<IFMsgQID>::iterator itrEnd = CIFMsgQID[ Type ].end();

    if(Type == IFBDS.GetSDType("TRANSFORM") &&
       CFlag[IFBDS.GetSDType("TRANSFORM")] ){

      for(; itr != itrEnd; itr++ ){
        if( Time%itr->Fq == 0  ){
          S_Pee.Header().Type = Type;
          S_Pee.Header().Time = Time;
          SendMsgQ(itr->MID, (char*)&S_TF.Data(), S_TF.DataSize(), false );
        }
      }
      CFlag[Type] = false;
    }
    if(Type == IFBDS.GetSDType("POSITION") &&
       CFlag[IFBDS.GetSDType("POSITION")] ){

      for(; itr != itrEnd; itr++ ){
        if( Time%itr->Fq == 0  ){
          S_Pee.Header().Type = Type;
          S_Pee.Header().Time = Time;
          SendMsgQ(itr->MID, (char*)&S_Pee.Data(), S_Pee.DataSize(), false );
        }
      }
      CFlag[Type] = false;
    }
    if(Type == IFBDS.GetSDType("C_POSITION") &&
       CFlag[IFBDS.GetSDType("C_POSITION")] ){

      for(; itr != itrEnd; itr++ ){
        if( Time%itr->Fq == 0  ){
          S_curPee.Header().Type = Type;
          S_curPee.Header().Time = Time;
          SendMsgQ(itr->MID, (char*)&S_curPee.Data(), S_curPee.DataSize(), false );
        }
      }
      CFlag[Type] = false;
    }
    if(Type == IFBDS.GetSDType("CUR_ANGLES") &&
       CFlag[IFBDS.GetSDType("CUR_ANGLES")] ){

      for(; itr != itrEnd; itr++ ){
        if( Time%itr->Fq == 0  ){
          S_CAG.Header().Type = Type;
          S_CAG.Header().Time = Time;
          SendMsgQ(itr->MID, (char*)&S_CAG.Data(), S_CAG.DataSize(), false );
        }
      }
      CFlag[Type] = false;
    }
    if(Type == IFBDS.GetSDType("DEST_ANGLES") &&
       CFlag[IFBDS.GetSDType("DEST_ANGLES")] ){

      for(; itr != itrEnd; itr++ ){
        if( Time%itr->Fq == 0  ){
          S_DAG.Header().Type = Type;
          S_DAG.Header().Time = Time;
          SendMsgQ(itr->MID, (char*)&S_DAG.Data(), S_DAG.DataSize(), false );

        }
      }
      CFlag[Type] = false;

    }
    /*
    if(Type == IFBDS.GetSDType("TIMESTAMP") &&
       CFlag[IFBDS.GetSDType("TIMESTAMP")] ){

      for(; itr != itrEnd; itr++ ){
        if( Time%itr->Fq == 0  ){
          S_TIME.Header().Type = Type;
          S_TIME.Header().Time = Time;
          SendMsgQ(itr->MID, (char*)&S_TIME.Data(), S_TIME.DataSize(), false );
        }
      }
      CFlag[Type] = false;
    }

    if(Type == IFBDS.GetSDType("TIME_TRNSCT") &&
       CFlag[IFBDS.GetSDType("TIME_TRNSCT")] ){

      for(; itr != itrEnd; itr++ ){
        if( Time%itr->Fq == 0  ){
          S_TIME_T.Header().Type = Type;
          S_TIME_T.Header().Time = Time;
          SendMsgQ(itr->MID, (char*)&S_TIME_T.Data(), S_TIME_T.DataSize(), false );
        }
      }
      CFlag[Type] = false;
    }
    */
  }
}

//-----------------------------------------------------------------------------
//
void
InterfaceManager::SendData(Coord_6DoF& data){
  S_Pee.Body().x = (float)data.x;
  S_Pee.Body().y = (float)data.y;
  S_Pee.Body().z = (float)data.z;
  S_Pee.Body().alpha = (float)data.alpha;
  S_Pee.Body().beta = (float)data.beta;
  S_Pee.Body().gamma = (float)data.gamma;
  CFlag[ IFBDS.GetSDType("POSITION") ] = true;
}

//-----------------------------------------------------------------------------
//
void
InterfaceManager::SendcurData(Coord_6DoF& data){
  S_curPee.Body().x = (float)data.x;
  S_curPee.Body().y = (float)data.y;
  S_curPee.Body().z = (float)data.z;
  S_curPee.Body().alpha = (float)data.alpha;
  S_curPee.Body().beta = (float)data.beta;
  S_curPee.Body().gamma = (float)data.gamma;
  CFlag[ IFBDS.GetSDType("C_POSITION") ] = true;
}

/*
//-----------------------------------------------------------------------------
//
void
InterfaceManager::SendData(EE_FORCE& data){
  S_Fee.Body().x = (float)data.x;
  S_Fee.Body().y = (float)data.y;
  S_Fee.Body().z = (float)data.z;
  S_Fee.Body().alpha = (float)data.alpha;
  S_Fee.Body().beta = (float)data.beta;
  S_Fee.Body().gamma = (float)data.gamma;
  CFlag[IFBDS.GetSDType("FOCE")] = true;
}
*/

//-----------------------------------------------------------------------------
//
void
InterfaceManager::SendDataTf(EE_POSITION& data){
  S_TF.Body()[9] = (float)data.x;
  S_TF.Body()[10] = (float)data.y;
  S_TF.Body()[11] = (float)data.z;
  CFlag[IFBDS.GetSDType("TRANSFORM")] = true;
}

//-----------------------------------------------------------------------------
//
void
InterfaceManager::SendTime(){
  S_TIME.Body()[0] = R_TIME.Body()[0];
  S_TIME.Body()[1] = R_TIME.Body()[1];
  CFlag[IFBDS.GetSDType("TIMESTAMP")] = true;
}

//-----------------------------------------------------------------------------
//
void
InterfaceManager::SendTime_T(double t){
  S_TIME_T.Body() = (float)t;
  CFlag[IFBDS.GetSDType("TIME_TRNSCT")] = true;
}

/*
//-----------------------------------------------------------------------------
//
void
InterfaceManager::SendDataLT(EE_POSITION& data ){
  S_Pee.Body().x = (float)data.x;
  S_Pee.Body().y = (float)data.y;
  S_Pee.Body().z = (float)data.z;
  S_Pee.Body().alpha = (float)data.alpha;
  S_Pee.Body().beta = (float)data.beta;
  S_Pee.Body().gamma = (float)data.gamma;
  CFlag[C_POSITION_LT] = true;
}

*/
//-----------------------------------------------------------------------------
//
void
InterfaceManager::SendDestAngle(double* angle){
  for(int i=0; i<3; i++){
    memcpy(&S_DAG.Body()[i], &((float)angle[i]), sizeof(float) );
  }
  CFlag[IFBDS.GetSDType("DEST_ANGLES")] = true;
}

//-----------------------------------------------------------------------------
//
void
InterfaceManager::SendCurAngle(double* angle){
  for(int i=0; i<3; i++){
    memcpy(&S_CAG.Body()[i], &((float)angle[i]), sizeof(float) );
  }
  CFlag[IFBDS.GetSDType("CUR_ANGLES")] = true;
}
/*
//-----------------------------------------------------------------------------
//
void
InterfaceManager::SendCurAngle(int jid, double angle ){
  memcpy( &S_CurAngle.body[jid*4], &((float)angle), sizeof(float) );
  CFlag[C_CUR_ANGLE] = true;
}
*/

//-----------------------------------------------------------------------------
//
void
InterfaceManager::ReceiveData(){
  int num=0;
  int typeNum= IFBDS.GetRDTypeNum();

  for( int SType=0; SType<typeNum; SType++){

    if(SIFMsgQID[SType].MID != NULL){
      num = GetMsgQNum(SIFMsgQID[SType].MID);

      for( ; num>0; num--){
        ReceiveMsgQ_NW( SIFMsgQID[SType].MID, (char*)&R_Buff.Data(),
                        R_Buff.DataSize() );

        if(R_Buff.Header().Type == IFBDS.GetRDType("POSITION") ){
          memcpy( &R_Pee.Data(), &R_Buff.Data(), R_Pee.DataSize() );
        }
        if(R_Buff.Header().Type == IFBDS.GetRDType("TIMESTAMP") ){
          memcpy( &R_TIME.Data(), &R_Buff.Data(), R_TIME.DataSize() );
        }

/*
        else if(R_Buff.Header().Type == IFBDS.GetRDType("POSITION") ){
          memcpy( &R_Fee.Data(), &R_Buff.Data(), R_Fee.DataSize() );
        }
        else if(R_Buff.header.Type == S_EVENT){
          memcpy( &R_Event, &R_Buff, sizeof(M_EVENT) );
        }
        else if(R_Buff.header.Type == S_POSITION_LT){
          memcpy( &R_Pee, &R_Buff, sizeof(M_EE_POSITION) );
        }
*/
      }

    }

  }
}

//-----------------------------------------------------------------------------
//
void
InterfaceManager::FilterMovingAvrg(Coord_6DoF_N& Pee){
  static Coord_6DoF_N Pee_[5] = {{0,0,0,0,0,0},{0,0,0,0,0,0},
                                 {0,0,0,0,0,0},{0,0,0,0,0,0},
                                 {0,0,0,0,0,0}};
  MovingAvrg(5, Pee, Pee_ );
}

void
InterfaceManager::MovingAvrg(int num, Coord_6DoF_N& data, Coord_6DoF_N* data_){
  //
  for(int i=0;i<num;i++){
    data.x += data_[i].x;
    data.y += data_[i].y;
    data.z += data_[i].z;
    data.alpha += data_[i].alpha;
    data.beta  += data_[i].beta;
    data.gamma += data_[i].gamma;
  }

  //
  num++;
  data.x = data.x/((double)num);
  data.y = data.y/((double)num);
  data.z = data.z/((double)num);
  data.alpha = data.alpha/((double)num);
  data.beta  = data.beta/((double)num);
  data.gamma = data.gamma/((double)num);

  //
  for(int i=num-2; i>0; i--){
    data_[i] = data_[i-1];
  }
  data_[0] = data;

}

//-----------------------------------------------------------------------------
//
void
InterfaceManager::ReceiveData(Coord_6DoF& p){
  FilterMovingAvrg( R_Pee.Body() );

  p.x = (double)R_Pee.Body().x;
  p.y = (double)R_Pee.Body().y;
  p.z = (double)R_Pee.Body().z;
  p.alpha = (double)R_Pee.Body().alpha;
  p.beta = (double)R_Pee.Body().beta;
  p.gamma = (double)R_Pee.Body().gamma;

}
/*
//-----------------------------------------------------------------------------
//
void
InterfaceManager::ReceiveDataLT(EE_POSITION& p){
  p.x = (double)R_Pee.Body().x;
  p.y = (double)R_Pee.Body().y;
  p.z = (double)R_Pee.Body().z;
  p.alpha = (double)R_Pee.Body().alpha;
  p.beta = (double)R_Pee.Body().beta;
  p.gamma = (double)R_Pee.Body().gamma;
}

//-----------------------------------------------------------------------------
//
void
InterfaceManager::ReceiveData(EE_FORCE& f){
  f.x = (double)R_Fee.Body().x;
  f.y = (double)R_Fee.Body().y;
  f.z = (double)R_Fee.Body().z;
  f.alpha = (double)R_Fee.Body().alpha;
  f.beta = (double)R_Fee.Body().beta;
  f.gamma = (double)R_Fee.Body().gamma;

}
*/
/*
//-----------------------------------------------------------------------------
//
void
InterfaceManager::ReceiveData(EVENT& e){
  e = R_Event.body;
}
*/

