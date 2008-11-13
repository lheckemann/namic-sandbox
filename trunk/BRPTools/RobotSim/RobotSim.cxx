/*=========================================================================

  Program:   Open IGT Link -- Simple Robot Simulator
  Module:    $RCSfile: $
  Language:  C++
  Date:      $Date: $
  Version:   $Revision: $

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include <iostream>
#include <stdlib.h>
#include <math.h>

#include "igtlOSUtil.h"
#include "igtlMessageHeader.h"
#include "igtlTransformMessage.h"
#include "igtlImageMessage.h"
#include "igtlServerSocket.h"
#include "igtlStatusMessage.h"

using namespace std;
using namespace ot;

//----------------------------------------------------------------------
// Constants

// Workphases
#define WP_START   0
#define WP_PLAN    1
#define WP_CALIB   2
#define WP_TARGET  3
#define WP_MANUAL  4
#define WP_EMERG   5

// Loop rate
#define DEFAULT_LOOP_RATE    100


//----------------------------------------------------------------------
// Functions

int  checkChangeWorkphase(int nextwp);
bool procStartUp(bool init);
bool procPlanning(bool init);
bool procCalibration(bool init);
bool procTargeting(bool init);
bool procManual(bool init);
bool procEmergency(bool init);


//----------------------------------------------------------------------
// Parameters

int    fcycle;
double range;
double cycle;
int    dir;
double sign;
double speed;

//----------------------------------------------------------------------
// Current position

std::vector<float> cpos(3, 0.0);
std::vector<float> cquat(4, 0.0);

std::vector<float> pos(3, 0.0);
std::vector<float> quat(4, 0.0);


//----------------------------------------------------------------------
// Main

int main(int argc, char **argv)
{
  char* xmlfile;
  long  rate;

  fcycle = 0;
  sign   = 1;

  // set parameters based on arguments
  if (argc <= 1) {
    cerr << "Usage: " << argv[0] << " [<rate in ms> [<range (mm)> <cycle (ms)> <dir (0, 1 or 2))>]]"  << endl;
    exit(-1);
  }
  if (argc > 1) {
    rate = atoi(argv[1]) * 1000;
  } else {
    rate = DEFAULT_LOOP_RATE * 1000;
  }
  if (argc > 5) {
    fcycle = 1;
    range = atof(argv[2]);
    cycle = atof(argv[3]);
    dir   = atoi(argv[4]);
  }

  cerr << "Loop rate: " << (rate/1000) << "ms" << endl;
  if (fcycle) {
    if (dir >= 0 && dir < 3) {
      cerr << "range : " << range << "mm" << endl;
      cerr << "cycle : " << cycle << "ms" << endl;
      cerr << "dir   : " << dir   << endl;
    } else {
      cerr << "Invalid dir." << endl;
      exit(0);
    }
    speed = ((double)rate/1000.0) * (range*2)/cycle;
  }


  // Start OpenIGTLink Server
  


  // context loop

  cquat[3] = 1.0;
  quat[3]  = 1.0;

  int  stopflag             = 0;
  int  workphase            = WP_STARTUP;
  bool allowChangeWorkphase = false;
  bool fWorkphaseChanged    = true;

  


  while (stopflag == 0) {
    
    stopflag = context.loopOnce();

    int nextwp = robotSink->getLastWorkPhaseCmd();

    if (allowChangeWorkphase) {
      int prev_workphase = workphase;
      workphase = checkChangeWorkphase(nextwp);
      if (prev_workphase != workphase) {
        sleep(1);
        fWorkphaseChanged = true;
        stopflag = context.loopOnce();
        usleep(rate);
      }
    }
    if (robotModule) {
      robotModule->sendCurrentWorkphase(workphase);
    }

    switch(workphase) {
    case BRPRobotModule::WP_STARTUP:
      //case WP_START:
      allowChangeWorkphase = procStartUp(fWorkphaseChanged);
      break;
    case BRPRobotModule::WP_PLANNING:
      //case WP_PLAN:
      allowChangeWorkphase = procPlanning(fWorkphaseChanged);
      break;
    case BRPRobotModule::WP_CALIBRATION:
      //case WP_CALIB:
      allowChangeWorkphase = procCalibration(fWorkphaseChanged);
      break;
    case BRPRobotModule::WP_TARGETING:
      //case WP_TARGET:
      allowChangeWorkphase = procTargeting(fWorkphaseChanged);
      break;
    case BRPRobotModule::WP_MANUAL:
      //case WP_MANUAL:
      allowChangeWorkphase = procManual(fWorkphaseChanged);
      break;
    case BRPRobotModule::WP_EMERGENCY:
      //case WP_EMERG:
      allowChangeWorkphase = procEmergency(fWorkphaseChanged);
      break;
    default:
      cerr << "Illegal workphase specified." << endl;
      exit(-1);
      break;
    }

    if (robotModule) {
      cout << "Robot pos= ("
           << cpos[0] << ", " << cpos[1] << ", " << cpos[2] << "),  ";
      cout << "ori = ("
           << cquat[0] << ", " << cquat[1] << ", " << cquat[2] << ", "
           << cquat[3] << ")"  << std::endl;
      robotModule->setTracker(cpos, cquat);
    }
    
    usleep(rate);
    fWorkphaseChanged = false;
  }

  context.close();

}


int  checkChangeWorkphase(int nextwp)
{
  // Check if it is possible to change workphase from current one to
  // the next, regrading to workphase transition diagram.

  return nextwp;
}


bool procStartUp(bool init)
{
  // dummy bootup sequence
  static int step;

  if (init) {
    step = 0;
    std::cerr << "==== START UP ==== " << std::endl;
  }


  step ++;

  if (step == 1)
    {
      std::cerr << "Initializing Robot control software..." << std::endl;
    }
  else if (step == 10)
    {
      std::cerr << "Wating for device activation..." << std::endl;
    }
  else if (step == 45)
    {
      std::cerr << "Initilization completed." << std::endl;
    }

  if (step > 50) {
    return true;
  } else {
    return false;
  }
}


bool procPlanning(bool init)
{
  if (init) {
    std::cerr << "==== PLANNING ==== " << std::endl;
  }

  return true;
}


bool procCalibration(bool init)
{
  static std::vector<float> pos(3, 0.0);
  static std::vector<float> quat(4, 0.0);
  quat[3] = 1.0;
  
  if (init) {
    std::cerr << "==== CALIBRATION ==== " << std::endl;
  }

  if (robotSink) {
    //robotSink->getZTracker(pos, quat);
    std::cerr << "ZFRAME: pos = ( "
              << pos[0]  << ", " << pos[1]  << ", " << pos[2] << " )"
              << " ori = ( " << quat[0] << ", " << quat[1] << ", " 
              << quat[2] << ", " << quat[3] << " )" << std::endl;
  }

  return true;
}


bool procTargeting(bool init)
{

  if (init) {
    std::cerr << "==== TARGETING ==== " << std::endl;
    for (int i = 0; i < 3; i ++) {
      pos[i]   = 0.0;
      cpos[i]  = 0.0;
    }
    for (int i = 0; i < 4; i ++) {
      quat[i]  = 0.0;
      cquat[i] = 0.0;
    }
    quat[3] = 1.0;
    cquat[3] = 1.0;
  }

  if (robotSink) {
    robotSink->getTracker(pos, quat);
    cout << "Navigation pos= ("
         << pos[0] << ", " << pos[1] << ", " << pos[2] << "),  ";
    cout << "ori = ("
         << quat[0] << ", " << quat[1] << ", " << quat[2] << ", "
         << quat[3] << ")"  << std::endl;
  }

  if (fcycle == 0) {
    /* smooth motion */

    for (int i = 0; i < 3; i ++) {
      cpos[i] = cpos[i] + (pos[i] - cpos[i])/50.0;
    }
    for (int i = 0; i < 4; i ++) {
      cquat[i] = cquat[i] + (quat[i] - cquat[i])/50.0;
    }

    /*
    for (int i = 0; i < 3; i ++) {
      cpos[i] = pos[i];
    }
    for (int i = 0; i < 4; i ++) {
      cquat[i] = quat[i];
    }
    */

  } else {
    cout << "speed = " << speed << endl;
    cpos[dir] = cpos[dir] + speed * sign;
    if (cpos[dir] > range/2 || cpos[dir] < -range/2) {
      sign = sign * -1.0;
      cpos[dir] = cpos[dir] - 2 * (cpos[dir] + sign * (range/2));
    }
    cquat[0] = cquat[1] = cquat[2] = 0.0;
    cquat[3] = 1.0;
  }
    
    
  return true;
}


bool procManual(bool init)
{

  if (init) {
    std::cerr << "==== MANUAL ==== " << std::endl;
    for (int i = 0; i < 3; i ++) cpos[i]  = -10.0;
    for (int i = 0; i < 4; i ++) cquat[i] = -10.0;
  }

  for (int i = 0; i < 3; i ++) {
    cpos[i] = cpos[i] + 0.1;
    if (cpos[i] > 10.0)
      cpos[i] = -10.0;
  }

  /*
  if (robotModule) {
    cout << "Robot pos= ("
         << cpos[0] << ", " << cpos[1] << ", " << cpos[2] << "),  ";
    cout << "ori = ("
         << cquat[0] << ", " << cquat[1] << ", " << cquat[2] << ", "
         << cquat[3] << ")"  << std::endl;
  }
  */


  return true;
}


bool procEmergency(bool init)
{
  if (init) {
    std::cerr << "==== EMERGENCY ==== " << std::endl;
  }

  return true;
}
