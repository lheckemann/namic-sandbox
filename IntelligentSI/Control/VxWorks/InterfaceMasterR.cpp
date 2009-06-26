/***************************************************************************
 * FileName      : InterfaceMasterR.cpp
 * Created       : 2008/2/5
 * LastModified  : 2008/
 * Author        : Hiroaki KOZUKA
 * Aim           : Interface for connecting slave robot
 * OS            : VxWorks 5.5.1
 ***************************************************************************/

#include "InterfaceMasterR.h"

InterfaceBase::IF_MSG_Q_ID InterfaceMasterR::IFMsgQID;
bool InterfaceMasterR::FlagD = false;

//-----------------------------------------------------------------------------
//
InterfaceMasterR::InterfaceMasterR(){
  std::cout<<"Init InterfaceMaster..."<<std::endl;
  InterfaceDataBaseSet IFDBS;
  IFMsgQID = CreateMsgQ( 10, sizeof(BUFF_M) );
  SetRMsgQID( IFDBS.GetRDType("POSITION"), 10, sizeof(BUFF_M),
              500.0,  IFMsgQID);
    SetRMsgQID( IFDBS.GetRDType("TIMESTAMP"), 10, sizeof(BUFF_M),
              500.0,  IFMsgQID);

  tID = CreateThread( "tIFRMsTask", (unsigned int)&Sock, (unsigned int)Read,
                      2, NULL, NULL, 0, 0, 0, 0 );
  FlagD = true;
  std::cout<<"done."<<std::endl;
}

//-----------------------------------------------------------------------------
//
InterfaceMasterR::~InterfaceMasterR(){
  DeleteThread(tID);
  cout<<"End IF_MasterR."<<endl;
}


//-----------------------------------------------------------------------------
//
void
InterfaceMasterR::Delete(){

}

//-----------------------------------------------------------------------------
//
void
InterfaceMasterR::Read(void * ptr){
  //InterfaceMasterR* IF = (InterfaceMasterR*)ptr;
  UDPSock* Sock = (UDPSock*)ptr;
  OpenIGTLink IGTL;
  InterfaceDataBaseSet IFDBS;

  int SockID = Sock->InitSockServer("sumatra", 50000);
  if(SockID == ERROR)
    return;

  SockID = Sock->SetSockOption(SockID);
  Sock->SetRecvBuf( SockID, 51200 );
  Sock->SetSock(SockID);

  MessageQData<Coord_6DoF_N> Buff_M_P;
  Buff_M_P.SetInfo(SERVER, "POSITION");

  MessageQData<unsigned long long[2]> time_d;
  time_d.SetInfo(SERVER, "TIMESTAMP");
  unsigned long long time = 0;

  BUFF_N Buff_N;
  memset(&Buff_N, 0, sizeof(BUFF_N) );

  unsigned long long t_0=0;
  unsigned long long t_1=0;

  int num = 0;
  do{
    num = Sock->Peek();

    if(num>0){
      Sock->Read( (char*)&Buff_N, num );


      if( IGTL.OpenOpenIGTLHeader( &Buff_N.Header, (char*)&Buff_N.Body ) ) {
        if( strstr("POSITION",  Buff_N.Header.name) != NULL ){
          memcpy( &Buff_M_P.Body(), &Buff_N.Body[0], Buff_M_P.BodySize() );

          t_0 = (unsigned long long)(TimeGet()*1000000000.0);

          time_d.Body()[0] = Buff_N.Header.timestamp - time;
          time_d.Body()[1] = t_0 - t_1;
          time = Buff_N.Header.timestamp;
          t_1 = t_0;

          SendMsgQ( IFMsgQID, (char*)&time_d.Data(), time_d.DataSize(), false);
          SendMsgQ(IFMsgQID, (char*)&Buff_M_P.Data(), Buff_M_P.DataSize(), false);

        }
      }
    }
  }while(true);
//  }while(GetEvent() != E_PRGM_EXIT);

}


//
double
InterfaceMasterR::TimeGet(){
  UINT32 tbu, tbl;
  vxTimeBaseGet(&tbu, &tbl);
  return (tbu * 4294967296.0 + tbl)/25000000.0;
}
