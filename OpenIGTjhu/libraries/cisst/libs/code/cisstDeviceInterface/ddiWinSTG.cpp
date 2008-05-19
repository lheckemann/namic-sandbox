/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

// ****************************************************************************
//
//  $Id: ddiWinSTG.cpp,v 1.2 2005/09/26 15:41:46 anton Exp $
//
//  Author(s):  Ankur Kapoor
//  Created on: 2004-04-30
//
//
//              Developed by the Engineering Research Center for
//          Computer-Integrated Surgical Systems & Technology (CISST)
//
//               Copyright(c) 2004, The Johns Hopkins University
//                          All Rights Reserved.
//
//  Experimental Software - Not certified for clinical use.
//  For use only by CISST sponsored research projects.  For use outside of
//  CISST, please contact Dr. Russell Taylor, CISST Director, at rht@cs.jhu.edu
//  
// ****************************************************************************


#include <cisstDeviceInterface/ddiWinSTG.h>
#if CISST_HAS_WINSTG


ddiWinSTG::ddiWinSTG()
  :ddiDeviceInterface("WinSTG")
{
  ReadNameMap["IsMoving"] = ISMOVING;
  ReadNameMap["GetAccelerations"] = GETACCELERATIONS;
  ReadNameMap["GetVelocities"] = GETVELOCITIES;
  ReadNameMap["GetError"] = GETERROR;
  ReadNameMap["GetCommand"] = GETCOMMAND;
  ReadNameMap["GetPositions"] = GETPOSITIONS;
  ReadNameMap["GetGains"] = GETGAINS;

  ReadOperations.resize(ReadOperations.size() + 8);
  ReadOperations[ISMOVING] = new ddiCommand< ddiWinSTG >(&ddiWinSTG::IsMoving, this, "IsMoving");
  ReadOperations[GETACCELERATIONS] = new ddiCommand< ddiWinSTG >(&ddiWinSTG::GetAccelerations, this, "GetAccelerations");
  ReadOperations[GETVELOCITIES] = new ddiCommand< ddiWinSTG >(&ddiWinSTG::GetVelocities, this, "GetVelocities");
  ReadOperations[GETERROR] = new ddiCommand< ddiWinSTG >(&ddiWinSTG::GetError, this, "GetError");
  ReadOperations[GETCOMMAND] = new ddiCommand< ddiWinSTG >(&ddiWinSTG::GetCommand, this, "GetCommand");
  ReadOperations[GETPOSITIONS] = new ddiCommand< ddiWinSTG >(&ddiWinSTG::GetPositions, this, "GetPositions");
  ReadOperations[GETGAINS] = new ddiCommand< ddiWinSTG >(&ddiWinSTG::GetGains, this, "GetGains");

  WriteNameMap["SetGains"] = SETGAINS;
  WriteNameMap["EnableAxes"] = ENABLEAXES;
  //WriteNameMap["VelMove"] = VELMOVE;
  WriteNameMap["PosMove"] = POSMOVE;
  WriteNameMap["PosDMove"] = POSDMOVE;

  WriteOperations.resize(WriteOperations.size() + 5);
  WriteOperations[SETGAINS] = new ddiCommand< ddiWinSTG >(&ddiWinSTG::SetGains, this, "SetGains");
  WriteOperations[ENABLEAXES] = new ddiCommand< ddiWinSTG >(&ddiWinSTG::EnableAxes, this, "EnableAxes");
  //WriteOperations[VELMOVE] = new ddiCommand< ddiWinSTG >(&ddiWinSTG::VelMove, this, "VelMove");
  WriteOperations[POSMOVE] = new ddiCommand< ddiWinSTG >(&ddiWinSTG::PosMove, this, "PosMove");
  WriteOperations[POSDMOVE] = new ddiCommand< ddiWinSTG >(&ddiWinSTG::PosDMove, this, "PosDMove");

  // need to maintain a local copy, because IOCTLs could be expensive in terms of time
  for (unsigned int i = 0; i < MAX_AXIS; i++) {
    ClosedLoop[i] = false;
    InterpolateMode[i] = PVA_INTERPOLATE;
  }
}


ddiWinSTG::~ddiWinSTG() {
  for (unsigned int i = 0 ; i < MAX_AXIS; i++) {
    if (ClosedLoop[i]) {
            BoardIO->EndClosedLoopControl((ddiWinSTGLowLevelIO::AxisType)i);
        }
  }
  if (BoardIO) delete BoardIO;
}


void ddiWinSTG::Configure(const char *filename) {
  int axis;
  BoardIO = new ddiWinSTGLowLevelIO();
  if (BoardIO->OpenStgDriver() != STG_SUCCESS) {
    std::cout << "Couldnt open device" << std::endl;
  }
  /* all these would be read from a CISST config file */
  long PGain[] = {250, 250, 250, 250};
  long IGain[] = {100, 100, 100, 100};
  long DGain[] = {100, 100, 100, 100};
  long ILimit[] = {1000, 1000, 1000, 1000};
  long OScale[] = {1, 1, 1, 1};
  long IMode[] = {PV_INTERPOLATE, PV_INTERPOLATE, PV_INTERPOLATE, PV_INTERPOLATE};
  for (axis = 0; axis < MAX_AXIS; axis++) {
    DefaultVelocities[axis] = 500; // counts/sample;
    DefaultAccelerations[axis] = 100; // counts/sample^2;
  }  
    
  for (axis = 0; axis < 4; axis++) {
    // set the gains for each
    BoardIO->SetGainAll((ddiWinSTGLowLevelIO::AxisType)axis, PGain[axis], IGain[axis],
                            DGain[axis], ILimit[axis], OScale[axis]);
    // set interpolation method - could be a PV_INTERPOLATE - default is trapezoidal aka PVA
      BoardIO->SetInterpolateOne((ddiWinSTGLowLevelIO::AxisType)axis, IMode[axis]);
      BoardIO->LoadParameters((ddiWinSTGLowLevelIO::AxisType)axis);
    InterpolateMode[axis] = IMode[axis];
    // this should be done when the first time a motion cmd is called. maybe??
    //BoardIO->BeginClosedLoopControl((ddiWinSTGLowLevelIO::AxisType)axis);
  }
}


std::vector<std::string> ddiWinSTG::Provides (void) {
  //For now we return all the read methods provided by this task
  //TODO: Add your code here
  std::vector<std::string> *provides = new  std::vector<std::string>;
  _OperationNameMapType::iterator iter;
  for (iter = ReadNameMap.begin(); iter != ReadNameMap.end(); ++iter) {
    provides->push_back(iter->first);
  }
  return *provides;
}



/* Protected methods that are executed for the corresponsding command */

bool ddiWinSTG::IsMoving(cmnDataObject &obj) {
  cmnStdVectorDataObject<bool> *pdata = dynamic_cast< cmnStdVectorDataObject<bool> *>(&obj);
  BoardIO->PollMoveDoneAll();
  for (int i = 0; i < pdata->size() && i < MAX_AXIS; i++) {
    if (BoardIO->GetPollResult((ddiWinSTGLowLevelIO::AxisType)i)) {
      (*pdata)[i] = true;
    } else {
      (*pdata)[i] = false;
    }
  }
  return true;
}


bool ddiWinSTG::GetAccelerations(cmnDataObject &obj) {
  cmnStdVectorDataObject<double> *pdata = dynamic_cast< cmnStdVectorDataObject<double> *>(&obj);
  return true;
}


bool ddiWinSTG::GetVelocities(cmnDataObject &obj) {
  cmnStdVectorDataObject<double> *pdata = dynamic_cast< cmnStdVectorDataObject<double> *>(&obj);
  long lVelocities[MAX_AXIS];
  BoardIO->GetVelocityAll(lVelocities);
  for (int i = 0; i < pdata->size() && i < MAX_AXIS; i++) {
    (*pdata)[i] = (double)lVelocities[i];
  }
  return true;
}


bool ddiWinSTG::GetError(cmnDataObject &obj) {
  cmnStdVectorDataObject<double> *pdata = dynamic_cast< cmnStdVectorDataObject<double> *>(&obj);
  long lError[MAX_AXIS];
  BoardIO->GetErrorAll(lError);
  for (int i = 0; i < pdata->size() && i < MAX_AXIS; i++) {
    (*pdata)[i] = (double)lError[i];
  }
  return true;
}


bool ddiWinSTG::GetCommand(cmnDataObject &obj) {
  cmnStdVectorDataObject<double> *pdata = dynamic_cast< cmnStdVectorDataObject<double> *>(&obj);
  for (unsigned int i = 0; i < (unsigned int)pdata->size() && i < MAX_AXIS; i++) {
    BoardIO->GetAllInfoOne((ddiWinSTGLowLevelIO::AxisType)i);
    (*pdata)[i] = (double)BoardIO->StgInfoSetPoint(); 
  }
  return true;
}


bool ddiWinSTG::GetPositions(cmnDataObject &obj) {
  cmnStdVectorDataObject<long> *pdata = dynamic_cast< cmnStdVectorDataObject<long> *>(&obj);
  long lEncoder[MAX_AXIS];
  BoardIO->GetEncoderAll(lEncoder);
  for (int i = 0; i < pdata->size() && i < MAX_AXIS; i++) {
    (*pdata)[i] = (long)lEncoder[i];
  }
  return true;
}


bool ddiWinSTG::GetGains(cmnDataObject &obj) {
  cmnStdVectorDataObject<cmnGainData> *pdata = dynamic_cast< cmnStdVectorDataObject<cmnGainData> *>(&obj);
  for (unsigned int i = 0; i < (unsigned int)pdata->size() && i < MAX_AXIS; i++) {
    BoardIO->GetAllInfoOne((ddiWinSTGLowLevelIO::AxisType)i);
    (*pdata)[i].PGain = (double)BoardIO->StgInfoPgain(); 
    (*pdata)[i].IGain = (double)BoardIO->StgInfoIgain();
    (*pdata)[i].DGain = (double)BoardIO->StgInfoDgain();
    (*pdata)[i].ILimit = (double)BoardIO->StgInfoIlimit();
    (*pdata)[i].OScale = (double)BoardIO->StgInfoOgain();
  }
  return true;
}


bool ddiWinSTG::SetGains(cmnDataObject &obj) {
  cmnStdVectorDataObject<cmnGainData> *pdata = dynamic_cast< cmnStdVectorDataObject<cmnGainData> *>(&obj);
  for (unsigned int i = 0; i < (unsigned int)pdata->size() && i < MAX_AXIS; i++) {
    BoardIO->SetGainAll((ddiWinSTGLowLevelIO::AxisType)i, 
      (long)(*pdata)[i].PGain, 
      (long)(*pdata)[i].IGain,
      (long)(*pdata)[i].DGain, 
      (long)(*pdata)[i].ILimit, 
      (long)(*pdata)[i].OScale);
  }
  return true;
}


bool ddiWinSTG::EnableAxes(cmnDataObject &obj) {
  cmnStdVectorDataObject<bool> *pdata = dynamic_cast< cmnStdVectorDataObject<bool> *>(&obj);
  return true;
}


bool ddiWinSTG::PosDMove(cmnDataObject &obj) {
  cmnStdVectorDataObject<long> *pdata = dynamic_cast< cmnStdVectorDataObject<long> *>(&obj);
  for (unsigned int i = 0; i < (unsigned int)pdata->size() && i < MAX_AXIS; i++) {
    if (!ClosedLoop[i]) BoardIO->BeginClosedLoopControl((ddiWinSTGLowLevelIO::AxisType)i);
    if (InterpolateMode[i] != P_INTERPOLATE) {
      BoardIO->SetInterpolateOne((ddiWinSTGLowLevelIO::AxisType)i, P_INTERPOLATE);
      InterpolateMode[i] = P_INTERPOLATE;
    }
  }
  long lEncoder[MAX_AXIS];
  BoardIO->GetEncoderAll(lEncoder);
  for (unsigned int i = 0; i < (unsigned int)pdata->size() && i < MAX_AXIS; i++) {
    BoardIO->SetDemandPosOne((ddiWinSTGLowLevelIO::AxisType)i, lEncoder[i]+(long)(*pdata)[i]);
    // the lower level velocity units are 65536*counts/sample. read pg 32 of manual
    BoardIO->SetDemandVelOne((ddiWinSTGLowLevelIO::AxisType)i, DefaultVelocities[i] << 16);
    BoardIO->SetDemandAccOne((ddiWinSTGLowLevelIO::AxisType)i, DefaultAccelerations[i]);
  }
  // start all of them together
  BoardIO->SetStartStopAll(1);
  return true;
}


bool ddiWinSTG::PosMove(cmnDataObject &obj) {
  /* for now this takes a double, later it would take a position object,
  which might have vector of positions, velocities, accelerations and 
  vector of flags indicating if the positions, velociites, accelerations
  are valid or not. eg if isVelocityValid[3] == false then use default
  velocities
  */
  cmnStdVectorDataObject<long> *pdata = dynamic_cast< cmnStdVectorDataObject<long> *>(&obj);
  for (unsigned int i = 0; i < (unsigned int)pdata->size() && i < MAX_AXIS; i++) {
    if (!ClosedLoop[i]) BoardIO->BeginClosedLoopControl((ddiWinSTGLowLevelIO::AxisType)i);
    if (InterpolateMode[i] != PVA_INTERPOLATE) {
      BoardIO->SetInterpolateOne((ddiWinSTGLowLevelIO::AxisType)i, PVA_INTERPOLATE);
      InterpolateMode[i] = PVA_INTERPOLATE;
    }
    BoardIO->SetDemandPosOne((ddiWinSTGLowLevelIO::AxisType)i, (long)(*pdata)[i]);
    // the lower level velocity units are 65536*counts/sample. read pg 32 of manual
    BoardIO->SetDemandVelOne((ddiWinSTGLowLevelIO::AxisType)i, DefaultVelocities[i] << 16);
    BoardIO->SetDemandAccOne((ddiWinSTGLowLevelIO::AxisType)i, DefaultAccelerations[i]);
  }
  // start all of them together
  BoardIO->SetStartStopAll(1);
  return true;
}


#endif // CISST_HAS_WINSTG


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
//  $Log: ddiWinSTG.cpp,v $
//  Revision 1.2  2005/09/26 15:41:46  anton
//  cisst: Added modelines for emacs and vi.
//
//  Revision 1.1  2004/05/27 17:09:25  anton
//  Incorporated code from Ankur for the ATI, MEI, STG and LoPoMoCo devices.
//  The build process will have to be redone whenever will will add dynamic
//  loading.
//
//
// ****************************************************************************
