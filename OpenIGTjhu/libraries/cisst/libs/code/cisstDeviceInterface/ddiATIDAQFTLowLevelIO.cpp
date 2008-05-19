/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

// ****************************************************************************
//
//  $Id: ddiATIDAQFTLowLevelIO.cpp,v 1.4 2005/09/26 15:41:46 anton Exp $
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

#include <cisstConfig.h>
#if CISST_HAS_ATIDAQFT

#include <math.h>              // sin(), cos()
#include <stdio.h>
//#include <windows.h>
//#include <msxml.h>
//#include <objsafe.h>
//#include <objbase.h>
//#include <atlbase.h>
#include <string>

#include <cisstDeviceInterface/ddiATIDAQFTLowLevelIO.h>
#include <cisstCommon/cmnXMLPath.h>
CMN_IMPLEMENT_SERVICES(ddiATIDAQFTLowLevelIO, 5);

short ddiATIDAQFTLowLevelIO::SetToolTransform(Calibration *cal, double Vector[6],const std::string &DistUnits,const std::string &AngleUnits) {
  short i;
  if (cal==NULL) return 1;
  if (DistConv(DistUnits)==0) return 2;
  if (AngleConv(AngleUnits)==0) return 3;
  for(i=0;i<6;i++)
    cal->cfg.UserTransform.TT[i]=Vector[i];
  cal->cfg.UserTransform.DistUnits=DistUnits;
  cal->cfg.UserTransform.AngleUnits=AngleUnits;
  return CalcMatrix(cal);
}


short ddiATIDAQFTLowLevelIO::SetForceUnits(Calibration *cal, const std::string &NewUnits) {
  if (cal==NULL) return 1;
  if (ForceConv(NewUnits)==0) return 2;
  cal->cfg.ForceUnits=NewUnits;
  return CalcMatrix(cal);
}


short ddiATIDAQFTLowLevelIO::SetTorqueUnits(Calibration *cal, const std::string &NewUnits) {
  if (cal==NULL) return 1;
  if (TorqueConv(NewUnits)==0) return 2;
  cal->cfg.TorqueUnits=NewUnits;
  return CalcMatrix(cal);
}


short ddiATIDAQFTLowLevelIO::SetTempComp(Calibration *cal, int TCEnabled) {
  if (cal==NULL) return 1;
  if (TCEnabled) 
    if (cal->TempCompAvailable) {
      cal->cfg.TempCompEnabled=ATI_DAQ_TRUE;
    } else return 2;
  else cal->cfg.TempCompEnabled=ATI_DAQ_FALSE;
  return 0;
}


char* ddiATIDAQFTLowLevelIO::ATI_Strdup(const char *s) {
  size_t t;
  char *p;
  t=strlen(s)+1;  // add one for termination character
  p = (char*)malloc(sizeof(char) * t);
  if (p==NULL) return p;
  *p='\0';
  strcat(p,s);
  return p;
}


unsigned short ddiATIDAQFTLowLevelIO::ATI_FindText(const char *str, unsigned short startPos) {
  int i;
  for(i=startPos;(str[i]=='/t') || (str[i]==' ');i++)
    if (str[i]=='\0')
      return 0;
  return i;
}


unsigned short ddiATIDAQFTLowLevelIO::ATI_FindSpace(const char *str, unsigned short startPos) {
  int i;
  for(i=startPos;(str[i]!='/t') && (str[i]!=' ') && (str[i]!='\0');i++)
    ;
  return i;
}


char* ddiATIDAQFTLowLevelIO::ATI_Strmid(const char *instr, unsigned short startPos, unsigned short length) {
  char *p;
  unsigned short i;
  p=(char*)calloc(length+1,sizeof(char));
  for(i=0;i<length;i++)
    p[i]=instr[i+startPos];
  p[length]='\0';  // terminate string
  return p;
}


/*! parses a space- or tab- delimited string into an array of doubles
  \param valueList String of values to split
  \param results Array of doubles to store the result
  \param numValues Number of values to parse
*/
void ddiATIDAQFTLowLevelIO::ATI_Separate(const char *valueList, double results[], unsigned short numValues) {
    unsigned short i;
    unsigned short startPos, endPos;
  char *word;
    startPos=ATI_FindText(valueList,0);    // find start of first value
  for (i = 0; i < numValues; i++) {
    endPos = ATI_FindSpace(valueList,startPos);    // find end of value
    word = ATI_Strmid(valueList, startPos, (unsigned short)(endPos-startPos));
        results[i] = (double) atof(word);  // store value in array
    free(word);
        startPos = ATI_FindText(valueList,endPos);   // find start of next value
  }
}


void ddiATIDAQFTLowLevelIO::RTConvertToFT(RTCoefs *coefs, double voltages[],double result[],int tempcomp) {
  // perform temp. comp., if applicable
  double cvoltages[ATI_DAQ_MAX_GAUGES];
  unsigned short i;
    
  for (i=0; i<coefs->NumberChannels-1; i++) {
    if (tempcomp==ATI_DAQ_TRUE) {
      cvoltages[i]=TempComp(coefs,voltages[i],voltages[coefs->NumberChannels-1],i) - coefs->TCbias_vector[i];
    } else {
      cvoltages[i]=voltages[i]-coefs->bias_vector[i];
    }
  }
  // perform matrix math
  mmult(*coefs->working_matrix,coefs->NumberAxes,(unsigned short)(coefs->NumberChannels-1),ATI_DAQ_MAX_GAUGES,
    cvoltages,1,1,
    result,1);
}


void ddiATIDAQFTLowLevelIO::RTBias(RTCoefs *coefs, double voltages[]) {
  unsigned short i;
  for (i=0; i<coefs->NumberChannels-1; i++) {
    coefs->bias_vector[i]=voltages[i];
    coefs->TCbias_vector[i]=TempComp(coefs,voltages[i],voltages[coefs->NumberChannels-1],i);
  }
  // store bias thermistor value, although it is no longer needed
  coefs->bias_vector[coefs->NumberChannels-1]=voltages[coefs->NumberChannels-1];
}


void ddiATIDAQFTLowLevelIO::mmult(double *a, unsigned short ra, unsigned short ca, unsigned short dca,
                  double *b, unsigned short cb, unsigned short dcb,
                  double *c, unsigned short dcc)
{
  //multiplies two matrices; must be properly dimensioned
  unsigned short i,j,k;
  for (i=0;i<ra;i++)
    for (j=0;j<cb;j++) {
      c[i*dcc+j]=0;
      for (k=0;k<ca;k++)
        c[i*dcc+j] = c[i*dcc+j] + a[i*dca+k] * b[k*dcb+j];
    }
}


double ddiATIDAQFTLowLevelIO::TempComp(RTCoefs *coefs, double G, double T, unsigned short i) {
    return ((G + coefs->bias_slopes[i] * (T - coefs->thermistor)) / (1 - coefs->gain_slopes[i] * (T - coefs->thermistor)));
}


void ddiATIDAQFTLowLevelIO::DestroyCalibration(Calibration *cal) {
  if (cal==NULL) return;
  free(cal);
  return;
}

#include <sys/types.h>
#include <sys/stat.h>
Calibration* ddiATIDAQFTLowLevelIO::CreateCalibration(const char *CalFilePath, unsigned short index) {
    
  int i, j;
  Calibration *cal;
  try {
    cal = (Calibration*)calloc(1, sizeof(Calibration));
    cmnXMLPath xmlConfig;
    if (!CalFilePath) {
      CMN_LOG_CLASS(2) << "Warning, could not configure ATIDAQFTLowLevelIO device" << std::endl;
      return 0;
    }
    struct _stat st;
    if (_stat(CalFilePath, &st) < 0) {
      CMN_LOG_CLASS(2) << "Invalid filename!! " << CalFilePath << std::endl;
      return 0;
    }

    xmlConfig.SetInputSource(CalFilePath);
    char path[60];
    std::string context("/FTSensor");

    CMN_LOG_CLASS(3) << "Configuring ATIDAQFTLowLevelIO with \"" << CalFilePath << "\"" << std::endl;
    std::string strValue;
    xmlConfig.GetXMLValue(context.c_str(), "@Serial", cal->Serial);
    xmlConfig.GetXMLValue(context.c_str(), "@BodyStyle", cal->BodyStyle);
    xmlConfig.GetXMLValue(context.c_str(), "@Family", cal->Family);
    xmlConfig.GetXMLValue(context.c_str(), "@NumGages", cal->rt.NumberChannels);
    
    context += "/Calibration";
    xmlConfig.GetXMLValue(context.c_str(), "@PartNumber", cal->PartNumber);
    xmlConfig.GetXMLValue(context.c_str(), "@CalDate", cal->CalDate);
    xmlConfig.GetXMLValue(context.c_str(), "@ForceUnits", cal->ForceUnits);
    xmlConfig.GetXMLValue(context.c_str(), "@TorqueUnits", cal->TorqueUnits);
    xmlConfig.GetXMLValue(context.c_str(), "@DistUnits", cal->BasicTransform.DistUnits);
    cal->cfg.UserTransform.DistUnits = cal->BasicTransform.DistUnits;
        
    //skiATI_DAQ_PIng angle units for now
    cal->BasicTransform.AngleUnits = ATI_Strdup("degrees");
    cal->cfg.UserTransform.AngleUnits = ATI_Strdup("degrees");
    // initialize temp comp variables
    cal->TempCompAvailable=ATI_DAQ_FALSE;
    for (i=0;i<ATI_DAQ_MAX_GAUGES;i++) {
      cal->rt.bias_slopes[i]=0;
      cal->rt.gain_slopes[i]=0;
    }
    cal->rt.thermistor=0;
        
    /*
      These are not read in the original file
      xmlConfig.GetXMLValue(context.c_str(), "@CalFileVersion", strValue);
      xmlConfig.GetXMLValue(context.c_str(), "@OutputMode", strValue);
      xmlConfig.GetXMLValue(context.c_str(), "@OutputRange", strValue);
      xmlConfig.GetXMLValue(context.c_str(), "@HWTempComp", strValue);
      xmlConfig.GetXMLValue(context.c_str(), "@GainMultiplier", strValue);
      xmlConfig.GetXMLValue(context.c_str(), "@OutputBipolar", strValue);
    */
    //Read in basic transform
        xmlConfig.GetXMLValue(context.c_str(), "BasicTransform/@Dx", cal->BasicTransform.TT[0]);
        xmlConfig.GetXMLValue(context.c_str(), "BasicTransform/@Dy", cal->BasicTransform.TT[1]);
        xmlConfig.GetXMLValue(context.c_str(), "BasicTransform/@Dz", cal->BasicTransform.TT[2]);
    xmlConfig.GetXMLValue(context.c_str(), "BasicTransform/@Rx", cal->BasicTransform.TT[3]);
    xmlConfig.GetXMLValue(context.c_str(), "BasicTransform/@Ry", cal->BasicTransform.TT[4]);
    xmlConfig.GetXMLValue(context.c_str(), "BasicTransform/@Rz", cal->BasicTransform.TT[5]);
    /*
      These are not present in the cal file
      xmlConfig.GetXMLValue(context.c_str(), "BiasSlope/@values", strValue);
      ATI_Separate(strValue.c_str(),cal->rt.bias_slopes,(unsigned short)(cal->rt.NumberChannels-1));
      xmlConfig.GetXMLValue(context.c_str(), "GainSlope/@values", strValue);
      ATI_Separate(strValue.c_str(),cal->rt.gain_slopes,(unsigned short)(cal->rt.NumberChannels-1));
      xmlConfig.GetXMLValue(context.c_str(), "Thermistor/@value", cal->rt.thermistor);
      ATI_Separate(strValue.c_str(),cal->rt.gain_slopes,(unsigned short)(cal->rt.NumberChannels-1));
    */
    //Read in the actual matrix
    double scale;
    double temparray[ATI_DAQ_MAX_GAUGES];
    cal->rt.NumberAxes = ATI_DAQ_MAX_AXES; /*! assumes that Fx, Fy etc are in order */
    for (i = 0; i < ATI_DAQ_MAX_AXES; i++) {
      sprintf(path, "Axis[%d]/@Name", i+1);
      xmlConfig.GetXMLValue(context.c_str(), path, cal->AxisNames[i]);
      sprintf(path, "Axis[%d]/@max", i+1);
      xmlConfig.GetXMLValue(context.c_str(), path, cal->MaxLoads[i]);
      sprintf(path, "Axis[%d]/@scale", i+1);
      xmlConfig.GetXMLValue(context.c_str(), path, scale);
      sprintf(path, "Axis[%d]/@values", i+1);
      xmlConfig.GetXMLValue(context.c_str(), path, strValue);
      ATI_Separate(strValue.c_str(),temparray,(unsigned short)(cal->rt.NumberChannels-1));
      for(j=0;j<cal->rt.NumberChannels-1;j++) {
        cal->BasicMatrix[i][j]=temparray[j]/scale;
      }
    }
    return cal;
  }
  
  catch(...)
        {
      CMN_LOG_CLASS(1) << "Exception occurred in Configuring ATIDAQFTLowLevelIO" << std::endl;
            return 0;
        } 
}


void ddiATIDAQFTLowLevelIO::ResetDefaults(Calibration *cal) {
  unsigned short i;  
  cal->cfg.ForceUnits=cal->ForceUnits;  // set output units to calibration file defaults
  cal->cfg.TorqueUnits=cal->TorqueUnits;  
  for (i=0;i<6;i++)                                 // clear user Tool Transform
    cal->cfg.UserTransform.TT[i]=0;
  for (i=0;i < cal->rt.NumberChannels-1;i++) {         // clear bias vectors
    cal->rt.bias_vector[i]=0;
    cal->rt.TCbias_vector[i]=0;
  }
  cal->rt.bias_vector[cal->rt.NumberChannels-1]=0;     // thermistor reading of bias vector
  cal->cfg.TempCompEnabled=cal->TempCompAvailable;  // turn temp comp on by default, if available
  
  CalcMatrix(cal);                                  // compute working matrix and store in rt
}


short ddiATIDAQFTLowLevelIO::CalcMatrix(Calibration *cal) {
  return GetMatrix(cal,*cal->rt.working_matrix);
}


short ddiATIDAQFTLowLevelIO::GetMatrix(Calibration *cal, double *result) {
  double UserTTM[6][6];           // the User tool transform matrix 
    double BasicTTM[6][6];          // basic (built-in) tool transform matrix
  double result1[6][ATI_DAQ_MAX_GAUGES];  // temporary intermediate result
  double FConv, TConv;            // unit conversion factors
  unsigned short i, j;           // loop variables
  unsigned short NumGauges=cal->rt.NumberChannels-1;  // number of strain gages
  short sts;                     // return value 
    
  if (cal->rt.NumberAxes==6) {
    sts=TTM(cal->BasicTransform,BasicTTM,cal->ForceUnits,cal->TorqueUnits);
    if (sts!=0) return 1;      // error in tool transform units
    sts=TTM(cal->cfg.UserTransform,UserTTM,cal->ForceUnits,cal->TorqueUnits);
    if (sts!=0) return 1;      // error in tool transform units
    mmult(*BasicTTM,6,6,6,
              *cal->BasicMatrix,NumGauges,ATI_DAQ_MAX_GAUGES,
              *result1,ATI_DAQ_MAX_GAUGES);
    mmult(*UserTTM,6,6,6,                // compute working matrix
              *result1,NumGauges,ATI_DAQ_MAX_GAUGES,
              result,ATI_DAQ_MAX_GAUGES);
  } else {
    // No transforms allowed except for 6-axis transducers
    result=*cal->BasicMatrix;
  }
  //Apply units change
  FConv = ForceConv(cal->cfg.ForceUnits) / ForceConv(cal->ForceUnits);
  TConv = TorqueConv(cal->cfg.TorqueUnits) / TorqueConv(cal->TorqueUnits);
  for(i=0;i<cal->rt.NumberAxes;i++)  //forces
    for(j=0;j<NumGauges;j++)
      if ((cal->AxisNames[i])[0]=='F') {
        result[i*ATI_DAQ_MAX_GAUGES+j] = result[i*ATI_DAQ_MAX_GAUGES+j] * FConv;
        if (FConv==0) return 2;
      } else {
        result[i*ATI_DAQ_MAX_GAUGES+j] = result[i*ATI_DAQ_MAX_GAUGES+j] * TConv;
        if (TConv==0) return 2;
      }
  return 0;
}


short ddiATIDAQFTLowLevelIO::TTM(Transform xform, double result[6][6], const std::string &ForceUnits, const std::string &TorqueUnits) {
    
  double R[3][3];  // rotation matrix
  double sx, sy, sz, cx, cy, cz, dx, dy, dz;
  unsigned short i, j;  // loop variables
  double DC,AC;  // distance conversion factor and angle conversion factor
    
  if ((DistConv(xform.DistUnits)==0) || (AngleConv(xform.AngleUnits)==0)) return 1; // invalid units
  DC = TorqueConv(TorqueUnits) / (ForceConv(ForceUnits) * DistConv(xform.DistUnits));
  AC = (double) 1.0/AngleConv(xform.AngleUnits);
  

  // calculate sin & cos of angles
  sx = (double) sin(ATI_DAQ_PI/180 * xform.TT[3] * AC);
  cx = (double) cos(ATI_DAQ_PI/180 * xform.TT[3] * AC);
  
  sy = (double) sin(ATI_DAQ_PI/180 * xform.TT[4] * AC);
  cy = (double) cos(ATI_DAQ_PI/180 * xform.TT[4] * AC);

  sz = (double) sin(ATI_DAQ_PI/180 * xform.TT[5] * AC);
  cz = (double) cos(ATI_DAQ_PI/180 * xform.TT[5] * AC);


  // calculate Dx, Dy, Dz
  dx = xform.TT[0]*DC;
  dy = xform.TT[1]*DC;
  dz = xform.TT[2]*DC;

  // calculate rotation matrix
  R[0][0] =  cy * cz;
  R[0][1] =  sx * sy * cz + cx * sz;
  R[0][2] =  sx * sz - cx * sy * cz;
  R[1][0] = -cy * sz;
  R[1][1] = -sx * sy * sz + cx * cz;
  R[1][2] =  sx * cz + cx * sy * sz;
  R[2][0] =  sy;
  R[2][1] = -sx * cy;
  R[2][2] =  cx * cy;

  // calculate transformation matrix
  for(i=0;i<=5;i++)
    for(j=0;j<=5;j++)
      result[i][j]=0;

  result[0][0] = R[0][0];
  result[0][1] = R[0][1];
  result[0][2] = R[0][2];

  result[1][0] = R[1][0];
  result[1][1] = R[1][1];
  result[1][2] = R[1][2];

  result[2][0] = R[2][0];
  result[2][1] = R[2][1];
  result[2][2] = R[2][2];

  result[3][0] = R[0][2] * dy - R[0][1] * dz;
  result[3][1] = R[0][0] * dz - R[0][2] * dx;
  result[3][2] = R[0][1] * dx - R[0][0] * dy;
  result[3][3] = R[0][0];
  result[3][4] = R[0][1];
  result[3][5] = R[0][2];

  result[4][0] = R[1][2] * dy - R[1][1] * dz;
  result[4][1] = R[1][0] * dz - R[1][2] * dx;
  result[4][2] = R[1][1] * dx - R[1][0] * dy;
  result[4][3] = R[1][0];
  result[4][4] = R[1][1];
  result[4][5] = R[1][2];

  result[5][0] = R[2][2] * dy - R[2][1] * dz;
  result[5][1] = R[2][0] * dz - R[2][2] * dx;
  result[5][2] = R[2][1] * dx - R[2][0] * dy;
  result[5][3] = R[2][0];
  result[5][4] = R[2][1];
  result[5][5] = R[2][2];
  return 0;
}


double ddiATIDAQFTLowLevelIO::ForceConv(const std::string &units) {
  if (units == "lb" || units == "lbf") {
      return (double) 1;
  } else if (units == "klb" || units == "klbf") {
    return (double) 0.001;
  } else if (units == "N") {
        return (double) 4.44822161526;
  } else if (units == "kN") {
        return (double) 0.00444822161526;
  } else if (units == "kg") {
        return (double) 0.45359237;
  } else if (units == "g") {
        return (double) 453.59237;
  } else return (double) 0;
}


double ddiATIDAQFTLowLevelIO::TorqueConv(const std::string &units) {
  if (units == "in-lb" || units == "in-lbf" || units == "lb-in" || units == "lbf-in") {
    return (double) 1;
  } else if (units == "ft-lb" || units == "lb-ft" || units == "ft-lbf" || units == "lbf-ft") {
        return (double) 0.08333333333;
  } else if (units == "N-m" || units == "Nm") {
        return (double) 0.112984829028;
  } else if (units == "N-mm" || units == "Nmm") {
        return (double) 112.984829028;
  } else if (units == "kg-cm" || units == "kgcm") {
        return (double) 1.1521246198;
  } else return (double) 0;
}


double ddiATIDAQFTLowLevelIO::DistConv(const std::string &units) {
  if (units == "in", 2) {
        return (double) 1;
  } else if (units == "m") {
        return (double) 0.0254;
  } else if (units == "cm") {
        return (double) 2.54;
  } else if (units == "mm") {
        return (double) 25.4;
  } else if (units == "ft") {
        return (double) 0.08333333333;
  } else return (double) 0;
}


double ddiATIDAQFTLowLevelIO::AngleConv(const std::string &units) {
  if (units == "deg" || units == "degrees" || units == "degree") {
    return (double) 1;
  } else if (units == "rad" || units == "radians" || units == "radian") {
    return (double) ATI_DAQ_PI / 180;
  } else return (double) 0;
}


#endif // CISST_HAS_ATIDAQFT


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
//  $Log: ddiATIDAQFTLowLevelIO.cpp,v $
//  Revision 1.4  2005/09/26 15:41:46  anton
//  cisst: Added modelines for emacs and vi.
//
//  Revision 1.3  2005/09/07 08:22:09  kapoor
//  ddiDeviceInterface: Minor fix in ATI DAQ interface. Added SI to Enc and Enc To SI conversion for MEI.
//
//  Revision 1.2  2005/06/19 21:39:32  kapoor
//  ddiDeviceInterface: a working version of ati ft daq (windows) interface.
//
//  Revision 1.1  2004/05/27 17:09:25  anton
//  Incorporated code from Ankur for the ATI, MEI, STG and LoPoMoCo devices.
//  The build process will have to be redone whenever will will add dynamic
//  loading.
//
//
// ****************************************************************************
