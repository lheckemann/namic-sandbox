//====================================================================
//
// MRI guided robot control system
//
// Copyright (C) 2003-2005 by The University of Tokyo,
// All Right Reserved.
//
//====================================================================
// $RCSfile: main.cpp,v $
// $Revision: 1.3 $ 
// $Author: junichi $
// $Date: 2005/01/24 12:07:28 $
//====================================================================


//====================================================================
// Description: 
//    Main routin for MRI guided robot control system.
//====================================================================

#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include "MrsvrMessageServer.h"
#include "MrsvrMainWindow.h"

int main (int argc, char* argv[]) 
{
  MrsvrMessageServer* mms;
  //  MrsvrLocatorClient* mlc;
  MrsvrMainWindow*    mmw;

  mms  = new MrsvrMessageServer(10005);
  //  mlc  = new MrsvrLocatorClient();

  // Make application
  FXApp application("MR Servo Robot Monitor","ATRE Lab. The Univ. of Tokyo");
  
  // Open display
  application.init(argc,argv);
  application.setBaseColor(FXRGB(80,80,80));
  application.setHiliteColor(FXRGB(120,120,120));
  application.setShadowColor(FXRGB(60,60,60));
  application.setBorderColor(FXRGB(0,0,0));
  application.setBackColor(FXRGB(30,30,30));
  application.setForeColor(FXRGB(240,240,240));

  // Make window
  int w = application.getRootWindow()->getDefaultWidth();
  int h = application.getRootWindow()->getDefaultHeight();
  fprintf(stderr, "w = %d, h = %d\n", w, h);
  mmw = new MrsvrMainWindow(&application, w, h);

  // register messaging server
  mmw->setExtMsgSvr(mms);
  //mmw->setLocClient(mlc);
#ifdef ENABLE_MRTS_CONNECTION
  // register MRTS Connection
  mmw->setMrtsCon(mmc);
#endif //ENABLE_MRTS_CONNECTION


  // Create app
  application.create();

  // Run
  application.run();
}
