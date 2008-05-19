/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

// ****************************************************************************
//
//  $Id: ddiATIDAQFTLowLevelIO.h,v 1.5 2005/09/26 15:41:46 anton Exp $
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


/*!
  \file
  \brief Defines the low level IOs for an ATI Force/Torque sensor with a NiDAQ card 
  \ingroup cisstDeviceInterface
*/


#ifndef _ddiATIDAQFTLowLevelIO_h
#define _ddiATIDAQFTLowLevelIO_h

#include <cisstConfig.h>
#if CISST_HAS_ATIDAQFT || defined DOXYGEN

#include <cisstCommon/cmnGenericObject.h>
#include <cisstCommon/cmnClassRegister.h>
//#include <atlbase.h>

#define ATI_DAQ_MAX_AXES 6
#define ATI_DAQ_MAX_GAUGES 8
#define ATI_DAQ_PI 3.14159265358979

#define ATI_DAQ_TRUE 1
#define ATI_DAQ_FALSE 0

/*! calibration information required for F/T conversions */
struct RTCoefs {
  /*! Number of Channels */
  int NumberChannels;

  /*! Number of Axes */
  int NumberAxes;

  /*! Working martix for computation of F/T from Voltages */
  double working_matrix[ATI_DAQ_MAX_AXES][ATI_DAQ_MAX_GAUGES];

  /*! Not used for out F/T */
  double bias_slopes[ATI_DAQ_MAX_GAUGES];

  /*! Not used for out F/T */
  double gain_slopes[ATI_DAQ_MAX_GAUGES];

  /*! Not used for out F/T */
  double thermistor;

  /*! Bias vector */
  double bias_vector[ATI_DAQ_MAX_GAUGES+1];

  /*! Bias vector */
  double TCbias_vector[ATI_DAQ_MAX_GAUGES];
};

/*! Tool transformation
  Note: tool transforms only supported for 6-axis F/T transducers 
*/
struct Transform {
  /*! displacement/rotation vector dx, dy, dz, rx, ry, rz */
  double TT[6];

  /*! units of dx, dy, dz */
  std::string DistUnits;    
  
  /*! units of rx, ry, rz */
  std::string AngleUnits;
};

/*! settings that can be changed by the user */
struct Configuration {
  /*! force units of output */
  std::string ForceUnits;

  /*! torque units of output */
  std::string TorqueUnits;

  /*! coordinate system transform set by user */
  Transform UserTransform;

  /*! is temperature compensation enabled? */
  int TempCompEnabled;
};

/*! transducer properties read from calibration file */
struct Calibration {
  /*! non-usable matrix; use rt.working_matrix for calculations */
  double BasicMatrix[ATI_DAQ_MAX_AXES][ATI_DAQ_MAX_GAUGES];

  /*! force units of basic matrix, as read from file; constant */
  std::string ForceUnits;

  /*! torque units of basic matrix, as read from file; constant */
  std::string TorqueUnits;

  /*! does this calibration have optional temperature compensation? */
  int TempCompAvailable;

  /*! built-in coordinate transform; for internal use */
  Transform BasicTransform;

  /*! maximum loads of each axis, in units above */
  double MaxLoads[ATI_DAQ_MAX_AXES];

  /*! names of each axis */
  std::string AxisNames[ATI_DAQ_MAX_AXES];

  /*! serial number of transducer (such as "FT4566") */
  std::string Serial;

  /*! transducer's body style (such as "Delta") */
  std::string BodyStyle;

  /*! calibration part number (such as "US-600-3600") */
  std::string PartNumber;

  /*! family of transducer (typ. "DAQ") */
  std::string Family;

  /*! date of calibration */
  std::string CalDate;

  /*! struct containing configurable parameters */
  Configuration cfg;

  /*! struct containing coefficients used in realtime calculations */
  RTCoefs rt;
};

/*!
  \ingroup cisstDeviceInterface
  This class is converted from the ftconfig.* and ftrt.* files provided by
  ATI. Provides some helper routines to manage the newer ATI F/T sensor that
  works with the NI-DAQ cards.
*/
class ddiATIDAQFTLowLevelIO: public cmnGenericObject {
  CMN_DECLARE_SERVICES(ddiATIDAQFTLowLevelIO, CMN_NO_DYNAMIC_CREATION);

  /*! Returns a pointer to a copy of the string s
      \param s String to copy
  */
  char* ATI_Strdup(const char *s);
    
  /*! Finds the first character in a string after startPos that is not a tab or space
      \param str String to search
      \param startPos Starting position in string
  */
  unsigned short ATI_FindText(const char *str, unsigned short startPos);
    
  /*! Finds the first tab or space in a string after startPos
      \param str String to search
      \param startPos Starting position in string
  */
  unsigned short ATI_FindSpace(const char *str, unsigned short startPos);
    
  /*! Returns a string containing a subset of a larger string
      \param instr String to use
      \param startPos Starting position in string
      \param length Length of substring
  */
  char* ATI_Strmid(const char *instr, unsigned short startPos, unsigned short length);
    
  /*! parses a space- or tab- delimited string into an array of doubles
      \param valueList String of values to split
      \param results Array of doubles to store the result
      \param numValues Number of values to parse
  */
  void ATI_Separate(const char *valueList, double results[], unsigned short numValues);
    
  /*! Convert raw voltages into Force/Torques
      \param coefs Coefficient of the conversion matrix
      \param voltages Raw voltages
      \param result Place to store result
      \param tempcomp If true perform temperature compensation
  */
  void RTConvertToFT(RTCoefs *coefs, double voltages[], double result[], int tempcomp);
    
  /*! Remove the bias from raw voltages
      \param coefs Coefficient of the conversion matrix
      \param voltages Raw voltages
  */
  void RTBias(RTCoefs *coefs, double voltages[]);
    
  /*! Used internally */
  void ddiATIDAQFTLowLevelIO::mmult(double *a, unsigned short ra, unsigned short ca, unsigned short dca,
                                      double *b, unsigned short cb, unsigned short dcb,
                                      double *c, unsigned short dcc);
    
  /*! Used internally */
  double TempComp(RTCoefs *coefs, double G, double T, unsigned short i);
    
  /*! Restores all configuration options to their default values
      \param cal Calibration structure
  */
  void ResetDefaults(Calibration *cal);
    
  /*! Computes a working matrix (based on BasicMatrix, tool transforms, and units)
      and stores it in the rt structure
      \param cal Calibration structure
  */
  short CalcMatrix(Calibration *cal);
    
  /*! Calculates a working matrix based on the basic matrix, basic tool transform, 
      user tool transform, and user units, and stores in result.
      \param cal Calibration structure
      \param result Place holder for result
  */
  short GetMatrix(Calibration *cal, double *result);
    
  /*! calculates a tool transform matrix and stores in result
      \param xform User transformation
      \param result Place holder for result
      \param ForceUnits Units for force
      \param TorqueUnits Units for torque
  */
  short TTM(Transform xform, double result[6][6], const std::string &ForceUnits, const std::string &TorqueUnits);
    
  /*! Return a multiplication factor to convert from lb to Units
      \param Units Units for Force
  */
  double ForceConv(const std::string &Units);
    
  /*! Return a multiplication factor to convert from lb-in to Units
      \param Units Units for Torque
  */
  double TorqueConv(const std::string &Units);
    
  /*! Return a multiplication factor to convert from in to Units
      \param Units Units for Distance
  */
  double DistConv(const std::string &Units);
    
  /*! Return a multiplication factor to convert from degrees to Units
      \param Units Units for Angle
  */
  double AngleConv(const std::string &Units);
    
 public:
  /*! Constructor. Does nothing */
  ddiATIDAQFTLowLevelIO() {}
    
  /*! Destructor */
  ~ddiATIDAQFTLowLevelIO() {}
    
  /*! Loads calibration info for a transducer into a new Calibration struct
      \param CalFilePath The name and path of the calibration file
      \param index The number of the calibration within the file (usually 1)
      \return Return Values:
      NULL: Could not load the desired calibration.
      Notes: For each Calibration object initialized by this function, destroyCalibration 
      must be called for cleanup.
  */
  Calibration *CreateCalibration(const char *CalFilePath, unsigned short index);
    
  /*! Frees memory allocated for Calibration struct by a successful call to 
      createCalibration.  Must be called when Calibration struct is no longer needed.
      \param cal Initialized Calibration struct
  */
  void DestroyCalibration(Calibration *cal);
    
  /*! Performs a 6-axis translation/rotation on the transducer's coordinate system.
      \param cal Initialized Calibration struct
      \param Vector Displacements and rotations in the order Dx, Dy, Dz, Rx, Ry, Rz
      \param DistUnits Units of Dx, Dy, Dz
      \param AngleUnits Units of Rx, Ry, Rz
      \return Return Values:
      0: Successful completion
      1: Invalid Calibration struct
      2: Invalid distance units
      3: Invalid angle units
  */
    short SetToolTransform(Calibration *cal, double Vector[6], const std::string &DistUnits, const std::string &ngleUnits);
    
    /*! Sets the units of force output
      \param cal Initialized Calibration struct
      \param NewUnits Units for force output ("lb","klb","N","kN","g","kg")
      \return Return Values:
      0: Successful completion
      1: Invalid Calibration struct
      2: Invalid force units
    */
    short SetForceUnits(Calibration *cal, const std::string &NewUnits);
    
    /*! Sets the units of torque output
      \param   cal Initialized Calibration struct
      \param  NewUnits Units for torque output  ("in-lb","ft-lb","N-m","N-mm","kg-cm")
      \return Return Values:
      0: Successful completion
      1: Invalid Calibration struct
      2: Invalid torque units
    */
    short SetTorqueUnits(Calibration *cal, const std::string &NewUnits);
    
    /*! Enables or disables temperature compensation, if available
      \param cal Initialized Calibration struct
      \param TCEnabled If 0 = temperature compensation off
      If 1 = temperature compensation on
      \return Return Values:
      0: Successful completion
      1: Invalid Calibration struct
      2: Not available on this transducer system
    */
    short SetTempComp(Calibration *cal, int TCEnabled);
    
    /*! Remove the bias from raw voltages
      \param coefs Coefficient of the conversion matrix
      \param voltages Raw voltages
    */
    void Bias(Calibration *cal, double voltages[]) {
        RTBias(&cal->rt,voltages);
    }
    
    /*! Convert raw voltages into Force/Torques
      \param coefs Coefficient of the conversion matrix
      \param voltages Raw voltages
      \param result Place to store result
      \param tempcomp If true perform temperature compensation
    */
    void ConvertToFT(Calibration *cal, double voltages[],double result[]) {
        RTConvertToFT(&cal->rt,voltages,result,cal->cfg.TempCompEnabled);
    }
    
};

#endif // CISST_HAS_ATIDAQFT

#endif // _ddiATIDAQFTLowLevelIO_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: ddiATIDAQFTLowLevelIO.h,v $
// Revision 1.5  2005/09/26 15:41:46  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.4  2005/08/08 17:13:44  anton
// Minor Doxygen typos.
//
// Revision 1.3  2005/06/19 21:41:17  kapoor
// ddiDeviceInterface: a working version of ati ft daq (windows) interface.
//
// Revision 1.2  2004/05/27 19:34:22  anton
// Added || defined DOXYGEN to all #ifdef CISST_HAS_XYZ so that doxygen can
// create the documentation.
//
// Revision 1.1  2004/05/27 17:09:25  anton
// Incorporated code from Ankur for the ATI, MEI, STG and LoPoMoCo devices.
// The build process will have to be redone whenever will will add dynamic
// loading.
//
//
// Revision 1.1  2004/04/08 06:05:05  kapoor
// Added ATI F/T DAQ DI
//
// ****************************************************************************
