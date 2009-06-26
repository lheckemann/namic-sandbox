/******************************************************************************
 * FileName      : InterfaceBase.cpp
 * Created       : 2008/
 * LastModified  : 2009/
 * Author        : Hiroaki KOZUKA
 * Aim           : Interfaces base
 *
 * OS            : VxWorks 5.5.1
 *****************************************************************************/

#include "InterfaceBase.h"

SEM_ID InterfaceBase::Sm;
std::vector<InterfaceBase::IFMsgQID>* InterfaceBase::CIFMsgQID;
InterfaceBase::IFMsgQID*  InterfaceBase::SIFMsgQID;
InterfaceDataBaseSet InterfaceBase::sIFDBS;


InterfaceBase::InterfaceBase(){
}

InterfaceBase::~InterfaceBase(){
}


#ifdef OS__VXWORKS
//-----------------------------------------------------------------------------
//
InterfaceBase::IF_MSG_Q_ID
InterfaceBase::CreateMsgQ(int maxNum, int maxSize){
  return msgQCreate( maxNum, maxSize, MSG_Q_PRIORITY );
}

//-----------------------------------------------------------------------------
//
void
InterfaceBase::DeleteMsgQ(IF_MSG_Q_ID id){
  msgQDelete(id);
}


//-----------------------------------------------------------------------------
//
void
InterfaceBase::SendMsgQ( IF_MSG_Q_ID id, char* data, int size, bool em ){
  if(em){
    msgQSend(id, data, size, NO_WAIT, MSG_PRI_URGENT );
  }
  else{
    msgQSend(id, data, size, NO_WAIT, MSG_PRI_NORMAL );
  }
}


//-----------------------------------------------------------------------------
//
void
InterfaceBase::ReceiveMsgQ( IF_MSG_Q_ID id, char* data, int size ){
  msgQReceive( id, data, size, WAIT_FOREVER );
}

//-----------------------------------------------------------------------------
//
void
InterfaceBase::ReceiveMsgQ_NW( IF_MSG_Q_ID id, char* data, int size ){
  msgQReceive( id, data, size, NO_WAIT );
}


//-----------------------------------------------------------------------------
//
int
InterfaceBase::GetMsgQNum(IF_MSG_Q_ID id){
  return msgQNumMsgs(id);
}


//-----------------------------------------------------------------------------
//
void
InterfaceBase::Init(){
  Sm = semBCreate( SEM_Q_FIFO, SEM_FULL);
  CIFMsgQID = new vector<IFMsgQID> [ sIFDBS.GetSDTypeNum() ];
  SIFMsgQID = new IFMsgQID [ sIFDBS.GetRDTypeNum() ];

  for( int SType=0; SType<sIFDBS.GetRDTypeNum(); SType++){
    SIFMsgQID[SType].MaxNum = 0;
    SIFMsgQID[SType].Fq     = 0;
    SIFMsgQID[SType].MID    = NULL;
    //      SFlag[SType] = false;
  }
}


//-----------------------------------------------------------------------------
//
void
InterfaceBase::Delete(){
  delete [] CIFMsgQID;
  delete [] SIFMsgQID;
  semDelete(Sm);
}


//-----------------------------------------------------------------------------
//
void
InterfaceBase::SetSMsgQID( int type,  int maxNum, int numSize,
                           double fq, IF_MSG_Q_ID qid){
  semTake(Sm, WAIT_FOREVER);
  IFMsgQID Temp = {maxNum, (int)(1000.0/fq), qid};
  CIFMsgQID[type].push_back(Temp);
  semGive(Sm);
  // cout<<"SetS:"<<type<<" "<<(int)qid<<endl;
}


//-----------------------------------------------------------------------------
//
void
InterfaceBase::SetRMsgQID( int type, int maxNum, int maxSize,
                           double fq, IF_MSG_Q_ID qid ){
  semTake(Sm, WAIT_FOREVER);
  SIFMsgQID[type].MaxNum = maxNum;
  SIFMsgQID[type].Fq = (int)(1000.0/fq);
  SIFMsgQID[type].MID = qid;
  semGive(Sm);
  cout<<"SetR"<<(int)qid<<endl;
}

//-----------------------------------------------------------------------------
//
InterfaceBase::IFMsgQID
InterfaceBase::GetSMsgQID(int type, int num){
  semTake(Sm, WAIT_FOREVER);
  vector<InterfaceBase::IFMsgQID>::iterator itr = CIFMsgQID[ type ].begin();
  IFMsgQID data = itr[num];
  semGive(Sm);
  return data;

}


//-----------------------------------------------------------------------------
//
int
InterfaceBase::GetSMsgQIDNum(int type){
  semTake(Sm, WAIT_FOREVER);
  int  num = (int)CIFMsgQID[ type ].size();
  semGive(Sm);
  return num;
}


//-----------------------------------------------------------------------------
//
InterfaceBase::IFMsgQID
InterfaceBase::GetRMsgQID(int type){
  semTake(Sm, WAIT_FOREVER);
  InterfaceBase::IFMsgQID data = SIFMsgQID[type];
  semGive(Sm);
  return data;
}


#else

#endif //OS__VXWROKS

//-----------------------------------------------------------------------------
//
InterfaceBase::IF_MSG_Q_ID
InterfaceBase::GetMsgQID(){
  return NULL;
}






