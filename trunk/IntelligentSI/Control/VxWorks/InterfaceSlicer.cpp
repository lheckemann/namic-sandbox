/***************************************************************************
 * FileName      : InterfaceSlicer.cpp
 * Created       : 2008/2/5
 * LastModified  : 2008/
 * Author        : Hiroaki KOZUKA
 * Aim           : Interface for connecting slave robot
 * OS            : VxWorks 5.5.1
 ***************************************************************************/

#include "InterfaceSlicer.h"

bool InterfaceSlicer::FlagD = false;
InterfaceBase::IF_MSG_Q_ID InterfaceSlicer::IFMsgQID;

//-----------------------------------------------------------------------------
//
InterfaceSlicer::InterfaceSlicer(){
  std::cout<<"Init InterfaceMaster..."<<std::endl;
  InterfaceDataBaseSet IFDBS;


  IFMsgQID = CreateMsgQ(10, sizeof(BUFF_M));

  SetSMsgQID( IFDBS.GetSDType("TRANSFORM"), 10, sizeof(BUFF_M), 10.0, IFMsgQID);

  cout<<"Slicer:"<<(int)IFMsgQID<<endl;

  tID = CreateThread( "tSlicerSv", (unsigned int)&Sock, (unsigned int)Write,
                      4, NULL, NULL, 0, 0, 0, 0 );

  FlagD = true;
  std::cout<<"done."<<std::endl;
}

//-----------------------------------------------------------------------------
//
InterfaceSlicer::~InterfaceSlicer(){
  DeleteThread(tID);
  cout<<"End IF_Slicer."<<endl;
}

//-----------------------------------------------------------------------------
//
void
InterfaceSlicer::Delete(){

}


//-----------------------------------------------------------------------------
//
void
InterfaceSlicer::Write(void* ptr){
  TCPSock* Sock = (TCPSock*)ptr;
  InterfaceDataBaseSet IFDBS;
  OpenIGTLink IGTL;

  BUFF_M Buff_M_P;
  BUFF_N Buff_N_T;

  memset( &Buff_M_P, 0, sizeof(BUFF_M) );
  memset( &Buff_N_T, 0, sizeof(BUFF_N) );

  int SockID = Sock->sockClientInit("slicer", 18944);
  if( SockID == ERROR ){

  }
  else{
    Sock->sockSetOption(SockID);
    Sock->SetSock(SockID);
  }


  // Open IGTLink header init
  Buff_N_T.Header = IGTL.InitOpenIGTHeader( "TRANSFORM", ROBOT_NAME, 0,
                                            IFDBS.GetSDSize("TRANSFORM") );

  do{
      // Get messageQ
      ReceiveMsgQ( IFMsgQID, (char*)&Buff_M_P, sizeof(Buff_M_P) );

      //
      if(Buff_M_P.Header.Type == IFDBS.GetSDType("TRANSFORM") ){
        memcpy(&Buff_N_T.Body, &Buff_M_P.Body, IFDBS.GetSDSize("TRANSFORM") );
        IGTL.PackOpenIGTLHeader( &Buff_N_T.Header, (char*)&Buff_N_T.Body, 0 );

      }

      // Send
      Sock->Write( (char*)&Buff_N_T, IFDBS.GetSDSize("TRANSFORM")+58 );
  }while(true);
//  }while(GetEvent() != E_PRGM_EXIT);

}

