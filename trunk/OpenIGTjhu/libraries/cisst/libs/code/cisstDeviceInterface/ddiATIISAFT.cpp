/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

// ****************************************************************************
//
//  $Id: ddiATIISAFT.cpp,v 1.3 2005/09/26 15:41:46 anton Exp $
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


#include <cisstDeviceInterface/ddiATIISAFT.h>
#if CISST_HAS_ATIISAFT

#include <iostream>

CMN_IMPLEMENT_SERVICES(ddiATIISAFT, 5);

ddiATIISAFT::ddiATIISAFT(unsigned int base):
    ddiDeviceInterface("ATIISAFT"),
    BaseAddress(base) {
  __ATIISAFT_BaseAddress = base;
  ReadNameMap["ForceTorques"] = FORCETORQUES;
  ReadNameMap["Info"] = INFO;
    
  ReadOperations.resize(ReadOperations.size() + 3);
  ReadOperations[FORCETORQUES] = new ddiCommand<ddiATIISAFT>(&ddiATIISAFT::GetForcesTorques, this, "FT");
  ReadOperations[INFO] = new ddiCommand<ddiATIISAFT>(&ddiATIISAFT::GetInfo, this, "INFO");
    
  WriteNameMap["Bias"] = BIAS;
    
  WriteOperations.resize(ReadOperations.size() + 2);
  WriteOperations[BIAS] = new ddiCommand<ddiATIISAFT>(&ddiATIISAFT::SetNewBias, this, "BIAS");
    
  ConversionNameMap["FTCountsToFTNewtons"] = FTCOUNTSTOFTNEWTONS;
    
  ConversionOperations.resize(ConversionOperations.size() + 2);
  ConversionOperations[FTCOUNTSTOFTNEWTONS] = new ddiCommand2<ddiATIISAFT>(&ddiATIISAFT::ConvertFTCountsToFTNewtons, this, "FTCTON");
}


ddiATIISAFT::~ddiATIISAFT() {
}


std::vector<std::string> ddiATIISAFT::Provides (void) {
  std::vector<std::string> *provides = new std::vector<std::string>;
  OperationNameMapType::iterator iter;
  for (iter = ReadNameMap.begin(); iter != ReadNameMap.end(); ++iter) {
    provides->push_back(iter->first);
  }
  return *provides;
}


void ddiATIISAFT::Configure(const char *filename) {
  int result;
  result = reset_card();
  if (result == FALSE) {
    /* add throw here */
    std::cout << "ATIFT: Couldnt reset card" << std::endl;
    return;
  }
  result = get_card_data();
  if (result == FALSE) {
    /* add throw here */
    std::cout << "ATIFT: Couldnt get card data" << std::endl;
    return;
  }
  result = get_sensor_data();
  if (result == FALSE) {
    /* add throw here */
    std::cout << "ATIFT: Couldnt get sensor data" << std::endl;
    return;
  }
  result = get_output_info();
  if (result == FALSE) {
    /* add throw here */
    std::cout << "ATIFT: Couldnt output info" << std::endl;
    return;
  }
}


bool ddiATIISAFT::GetForcesTorques(cmnDataObject &obj) {
  cmnFTDataObject *pdata = dynamic_cast<cmnFTDataObject *>(&obj);
  long ftData[8];
  read_ft_point (ftData);
  for (unsigned int i = 0; i < 8; i++) {
    pdata->FTData[i] = (double)ftData[i];
  }
  return true;
}


bool ddiATIISAFT::ConvertFTCountsToFTNewtons(cmnDataObject &from, cmnDataObject &to) {
  cmnFTDataObject *datafrom = dynamic_cast<cmnFTDataObject *>(&from);
  cmnFNTNmDataObject *datato = dynamic_cast<cmnFNTNmDataObject *>(&to);
  datato->Sequence = (long)datafrom->FTData[0];
  datato->Condition = (long)datafrom->FTData[1];
  for (unsigned int i = 0; i < 3; i++) {
    datato->Forces[i] = ((double)datafrom->FTData[i + 2]/(double)__output_info.cnts_per_force)*10.0;
    datato->Torques[i] = ((double)datafrom->FTData[i + 3 + 2]/(double)__output_info.cnts_per_torque)*10.0;
  }
  return true;
}


bool ddiATIISAFT::GetInfo(cmnDataObject &obj) {
  char page[1024];
  int len=0;
  unsigned short filter_size;
  unsigned short is_bias_on;
  unsigned short is_temp_compensation_on;
  unsigned short is_calibration_on;
  unsigned short is_peak_holding_on;
  unsigned short is_monitoring_on;
  unsigned short is_saturation_check_on;

  len += sprintf (page+len, "\t\t---=== ATI F/T Sensor Device Info ===---\n");
  get_sensor_data ();
  get_card_data ();
  get_output_info ();
  get_condition_flags (&filter_size, &is_bias_on, &is_temp_compensation_on,
      &is_calibration_on, &is_peak_holding_on, &is_monitoring_on, &is_saturation_check_on);
  len += sprintf (page+len, "\t-=== Card Info ===-\n");
  len += sprintf (page+len, "Card Serial No.       : SN-%d\n", __card_data.cd_serial_number);
  len += sprintf (page+len, "Card Model No.        : %d\n", __card_data.cd_model);
  len += sprintf (page+len, "Card uCode Version    : %d\n", __card_data.cd_microcode_version);
  len += sprintf (page+len, "Sample Rate           : %d\n", __card_data.cd_sample_rate);
  len += sprintf (page+len, "Data Condition Flags:\n");
  len += sprintf (page+len, "    Filter Size       : %d\n", filter_size);
  len += sprintf (page+len, "    Software Bias     : %s\n", (is_bias_on) ? "YES" : "NO");
  len += sprintf (page+len, "    Temp Compensation : %s\n", (is_temp_compensation_on) ? "YES" : "NO");
  len += sprintf (page+len, "    Calibration       : %s\n", (is_calibration_on) ? "YES" : "NO");
  len += sprintf (page+len, "    Peak Holding      : %s\n", (is_peak_holding_on) ? "YES" : "NO");
  len += sprintf (page+len, "    Monitoring        : %s\n", (is_monitoring_on) ? "YES" : "NO");
  len += sprintf (page+len, "    Saturation Check  : %s\n", (is_saturation_check_on) ? "YES" : "NO");
  len += sprintf (page+len, "\t-=== Sensor Info ===-\n");
  len += sprintf (page+len, "Sensor Serial No.     : FT%d\n", __sensor_data.sd_serial);
  len += sprintf (page+len, "Maximum Force         : %d\n", __sensor_data.sd_force);
  len += sprintf (page+len, "Maximum Torque        : %d\n", __sensor_data.sd_torque);
  len += sprintf (page+len, "Counts Per Force      : %d\n", __output_info.cnts_per_force);
  len += sprintf (page+len, "Counts Per Torque     : %d\n", __output_info.cnts_per_torque);

  std::cout << page << std::endl;
  return true;
}


bool ddiATIISAFT::SetNewBias(cmnDataObject &obj) {
  int result = set_new_bias();
  if (result == TRUE) {
    return true;
  } else {
    return false;
  }
}


#endif // CISST_HAS_ATIISAFT


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: ddiATIISAFT.cpp,v $
// Revision 1.3  2005/09/26 15:41:46  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.2  2005/07/09 04:49:02  kapoor
// Added CMN_IMPLEMENT_SERVICES
//
// Revision 1.1  2004/05/27 17:09:25  anton
// Incorporated code from Ankur for the ATI, MEI, STG and LoPoMoCo devices.
// The build process will have to be redone whenever will will add dynamic
// loading.
//
//
// Revision 1.3  2004/04/08 06:07:01  kapoor
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
