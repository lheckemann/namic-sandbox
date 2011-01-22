//====================================================================
//
// MRI guided robot control system
//
// Copyright (C) 2003-2005 by The University of Tokyo,
// All Right Reserved.
//
//====================================================================
// $RCSfile: MrsvrMrtsCon.cpp,v $
// $Revision: 1.2 $ 
// $Author: junichi $
// $Date: 2005/11/15 11:05:33 $
//====================================================================


//====================================================================
// Description: 
//    Message interface to communicate with MRTS.
//====================================================================

#include <iostream.h>
#include "MrsvrMrtsCon.h"

const char* MrsvrMrtsCon::statusStr[] = {
  "Disconnected",
  "Connected"
};


MrsvrMrtsCon::MrsvrMrtsCon() : MrsvrThread()
{
  isRun           = 0;
  //mtc             = new MRTrackingClient();
  mrtsHostname[0] = '\0';
  mrtsPort        = DEFAULT_MRTS_PORT;
  status          = MRTS_DISCONNECTED;
  interval        = DEFAULT_MRTS_INTERVAL;

  setPoint        = new MrsvrRASWriter(SHM_RAS_SETPOINT);
}


MrsvrMrtsCon::~MrsvrMrtsCon()
{
  if (mtc) 
    delete mtc;
}


int MrsvrMrtsCon::connect()
{
  if (status !=  MRTS_DISCONNECTED) {
#ifdef  DEBUG_MRSVR_MRTS_CON
    cerr << ": Disconnect before connect." << endl;
#endif
    return 0;
  }
  if (mtc->connectToServer(mrtsHostname, mrtsPort) == 
      MRTrackingClient::FAILURE) {
#ifdef  DEBUG_MRSVR_MRTS_CON
    cerr << ": Connection failed." << endl;
#endif
    status = MRTS_DISCONNECTED;
    return 0;
  } else {
    status = MRTS_CONNECTED;
    return 1;
  }
}


int MrsvrMrtsCon::disconnect()
{
  if (mtc) {
    mtc->disconnect();
  }
  status = MRTS_DISCONNECTED;
  return 1;
}


int MrsvrMrtsCon::getTrans() 
{
  //mtc->getTransform(&msgTrans);
  //
  //setPoint->setNR(msgTrans.nx);
  //setPoint->setNA(msgTrans.ny);
  //setPoint->setNS(msgTrans.nz);
  //setPoint->setTR(msgTrans.tx);
  //setPoint->setTA(msgTrans.ty);
  //setPoint->setTS(msgTrans.tz);
  //setPoint->setPR(msgTrans.dx);
  //setPoint->setPA(msgTrans.dy);
  //setPoint->setPS(msgTrans.dz);
  //
  //return 1;
}


void MrsvrMrtsCon::process()
  //
  // This process is implemented TEMPORARILY, using ART Linux API.
  //
{
  int count;
  if (status != MRTS_CONNECTED) {
    connect();
  }

#ifdef USE_ART
  if (art_enter(ART_PRIO_MAX, ART_TASK_PERIODIC, interval) == -1) {
    perror("art_enter");
    exit(1);
  }
#endif //USE_ART

  count  = 0;
  isRun = 1;

  while (1) {
#ifdef USE_ART    
    if (art_wait() == -1) {
      perror("art_wait");
      break;
    }
#endif
    if (isRun == 0) {
      break;
    }
    getTrans();
    cout << "GetTrans    " << count << endl;
    count ++;
  }
  
  cout << "MrsvrMrtsCon interval = " << interval << endl;
  disconnect();
#ifdef USE_ART
  if (art_exit() == -1) {
    perror("art_exit");
  }
#endif //USE_ART

}


void MrsvrMrtsCon::quitProcess() 
{
  isRun = 0;
}


const char* MrsvrMrtsCon::getStatusStr()
{
  return statusStr[status];
}
