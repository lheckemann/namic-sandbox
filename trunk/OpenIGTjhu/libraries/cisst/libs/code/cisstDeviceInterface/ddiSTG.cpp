/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

// ****************************************************************************
//
//  $Id: ddiSTG.cpp,v 1.2 2005/09/26 15:41:46 anton Exp $
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


#include <cisstDeviceInterface/ddiSTG.h>
#if CISST_HAS_STG

#include <iostream>

ddiSTG::ddiSTG():
    ddiDeviceInterface("STG") {
  PreReadNameMap["LatchEncoders"] = LATCHENCODERS;

  PreReadOperations.resize(ReadOperations.size() + 2);
  PreReadOperations[LATCHENCODERS] = new ddiCommand<ddiSTG>(&ddiSTG::LatchEncoders, this, "LATCHENC");

  ReadNameMap["Positions"] = POSITIONS;
  ReadNameMap["Info"] = INFO;

  ReadOperations.resize(ReadOperations.size() + 3);
  ReadOperations[POSITIONS] = new ddiCommand<ddiSTG>(&ddiSTG::GetPositions, this, "POS");
  ReadOperations[INFO] = new ddiCommand<ddiSTG>(&ddiSTG::GetInfo, this, "INFO");

  WriteNameMap["MotorVoltages"] = MOTORVOLTAGES;

  WriteOperations.resize(WriteOperations.size() + 2);
  WriteOperations[MOTORVOLTAGES] = new ddiCommand<ddiSTG>(&ddiSTG::SetMotorVoltages, this, "MV");
/*
  PostWriteNameMap["LoadMotorVoltages"] = LOADMOTORVOLTAGES;

  PostWriteOperations.resize(PostWriteOperations.size() + 2);
  PostWriteOperations[LOADMOTORVOLTAGES] = new ddiCommand<ddiSTG>(&ddiSTG::LoadMotorVolatages, this);
*/
  ConversionNameMap["EncodersToDegs"] = ENCODERTODEGS;
  ConversionNameMap["MotorVoltagesToDAC"] = MOTORVOLTAGESTODAC;

  ConversionOperations.resize(ConversionOperations.size() + 3);
  ConversionOperations[ENCODERTODEGS] = new ddiCommand2<ddiSTG>(&ddiSTG::ConvertEncoderCountsToDegs, this, "ENCTODEG");
  ConversionOperations[MOTORVOLTAGESTODAC] = new ddiCommand2<ddiSTG>(&ddiSTG::ConvertMotorVoltagesToDACCounts, this, "MVTODAC");
}


ddiSTG::~ddiSTG() {
}


std::vector<std::string> ddiSTG::Provides (void) {
  std::vector<std::string> *provides = new std::vector<std::string>;
  provides->push_back("Position");
  provides->push_back("Info");
  provides->push_back("MotorVoltage");
  return *provides;
}


void ddiSTG::Configure(const char *filename) {
  BaseAddress = BaseFind();
  /* add a throw here if not found */
  __STG_BaseAddress = BaseAddress;
  MaxAxis = 0x03;
  Init8255();
  EncoderInit();
  for(unsigned int axis = 0; axis <= STG_MAX_AXIS; axis++) {
    RawDAC(axis, 0);
  }
  SetIrq(7);
  SelectInterruptPeriod(_1_MILLISECOND);
}


bool ddiSTG::LatchEncoders(cmnDataObject &obj) {
  EncoderLatch();
  return true;
}


bool ddiSTG::GetPositions(cmnDataObject &obj) {
  cmnStdVectorDataObject<long> *pdata = dynamic_cast<cmnStdVectorDataObject<long> *>(&obj);
  LONGBYTE enc[8];
  EncoderReadAll(enc);
  //for now the axis is 0x03, but later we may want to change it to
  //lower of size of StdVector or max encoders on the device
  //for(unsigned int axis = 0; axis <= MaxAxis && axis <= MaxSize; axis++)
  for (unsigned int axis = 0; axis <= MaxAxis; axis++) {
    //pdata->Data[axis] = enc[axis].Long;
    pdata->Data[axis] = (axis+1)*11;
  }
  return true;
}


bool ddiSTG::GetInfo(cmnDataObject &obj) {
  char page[1024];
  int len=0;
  len += sprintf (page+len, "\t\t---=== STG Device Info ===---\n");
  len += sprintf (page+len, "\t-=== Card Info ===-\n");
  len += sprintf (page+len, "Card Address          : 0x%03x\n", BaseAddress);

  std::cout << page << std::endl;
  return true;
}


bool ddiSTG::SetMotorVoltages(cmnDataObject &obj) {
  //the dynamic range of input is 4096(+10V) to -4095(-10V)
  cmnStdVectorDataObject<short> *pdata = dynamic_cast<cmnStdVectorDataObject<short> *>(&obj);
  for (unsigned int axis = 0; axis <= MaxAxis; axis++) {
    RawDAC(axis, pdata->Data[axis]);
  }
  return true;
}


//OOPS! the STG doesnt allow software to changed the latching of DACs
//It is set by hardware jumper J3 to be done when the encoders latch
//and the DAC values written have to wait till the next cycle.

/*
bool ddiSTG::LoadMotorVolatages(cmnDataObject &obj) {
  return true;
}
*/

bool ddiSTG::ConvertEncoderCountsToDegs(cmnDataObject &from, cmnDataObject &to) {
  cmnStdVectorDataObject<double> *dataTo = dynamic_cast<cmnStdVectorDataObject<double> *>(&to);
  cmnStdVectorDataObject<long> *dataFrom = dynamic_cast<cmnStdVectorDataObject<long> *>(&from);
  for (unsigned int axis = 0; axis <= MaxAxis; axis++) {
    dataTo->Data[axis] = 1.0*(double)(dataFrom->Data[axis])/768.0;
    //todata->Data[axis] = 1.0*(double)(fromdata->Data[axis]-0x007FFFFF)/768.0;
  }
  return true;
}


bool ddiSTG::ConvertMotorVoltagesToDACCounts(cmnDataObject &from, cmnDataObject &to) {
  cmnStdVectorDataObject<short> *dataTo = dynamic_cast<cmnStdVectorDataObject<short> *>(&to);
  cmnStdVectorDataObject<double> *dataFrom = dynamic_cast<cmnStdVectorDataObject<double> *>(&from);
  for (unsigned int axis = 0; axis <= MaxAxis; axis++) {
    dataTo->Data[axis] = (short)(4096.0 * dataFrom->Data[axis]/10.42);
  }
  return true;
}

#endif // CISST_HAS_STG


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: ddiSTG.cpp,v $
// Revision 1.2  2005/09/26 15:41:46  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.1  2004/05/27 17:09:25  anton
// Incorporated code from Ankur for the ATI, MEI, STG and LoPoMoCo devices.
// The build process will have to be redone whenever will will add dynamic
// loading.
//
//
// Revision 1.3  2004/04/08 06:11:47  kapoor
// Added conditional compilation
//
// Revision 1.2  2004/03/24 21:00:28  kapoor
// Return a command object instead of a member function pointer. This was done
// to handle composite devices. The command object contains the information
// on the deivce that has to handle the operation besides just the operation,
// which is needed to resolve who should handle the operaion in case of a
// composite device.
//
// Revision 1.1  2004/03/22 00:30:53  kapoor
// Added new devices.
//
// ****************************************************************************
