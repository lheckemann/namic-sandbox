/** @file
  \brief Redefined BRPtprControl for this specific robot - contains the control logic
*/ 

#ifndef _ctfControlBase_h_
#define _ctfControlBase_h_

#include <vector>
#include "BRPtprControl.h"
#include "BRPtprOpenTracker.h"
#include "BRPtprMessages.h"

typedef std::vector<float> FloatVectorType;

#ifdef _CISST_LIBRARY
 #define DEBUG_MESSAGE CMN_LOG(3)
#else
 #define DEBUG_MESSAGE std::cout
#endif

/*
#define ctf_LoadData(data_from, data_to, data_size) { \
          float _tmp[data_size]; \
          data_from(_tmp);  \
          for (int _i=0;_i<data_size;_i++) { \
            data_to[_i] = _tmp[_i]; \
          } \
        }

#define ctf_SaveData(data_from, data_to, data_size) { \
          float _tmp[data_size]; \
          for (int _i=0;_i<data_size;_i++) { \
            _tmp[_i]=(float)data_from[_i]; \
          } \
          data_to(_tmp);\
        }
*/

class ctfControlBase: public BRPtprControl
{
public:
 // Object initialization
 ctfControlBase(int nRobotJointNumbers);
 virtual ~ctfControlBase(void);
 void SetOpenTracker(BRPtprOpenTracker *ot);
 
 // Incoming commands from the navigation software ------------------
 virtual void WorkphaseSTART_UP(void);
 virtual void WorkphasePLANNING(void);
 virtual void WorkphaseCALIBRATION(void);
 virtual void WorkphaseTARGETING(void);
 virtual void WorkphaseMANUAL(void);
 virtual void WorkphaseEMERGENCY(void);

 void ZFrame(float position[3], float orientation[4]);
 virtual void GoToCoordinates(float position[3], float orientation[4]);

 virtual void InitializeRobot(void);
 virtual void Home(void);


 virtual void GetActualCoordinates(void);
 void GetStatus(void);

 void SetJointPositions(float pos[]); // "nRobotJointNumbers" long all the time!
 void GetNumberOfJoints(void);
 void GetActualJointPositions(void);
 void LockRobot(void);
 void UnlockRobot(void);
 // -------------------------------------------------------------------


 // Internal functions to set/get the robot status ---------------------

 // stores "p" and "o" into ActualRobotPositions/ActualRobotOrientation (global coord system)
 void SaveActualRobotPosition(const float p[3], const float o[4], const float d[3]);

 // stores "pos" into ActualJointPositions variable (local coord system)
 void SaveActualJointPositions(const float pos[]);
 void SaveActualJointPositions(const double * pos);

 // stores "pos" into ActualRawEncoderPositions variable (local coord system)
 void SaveActualRawEncoderPositions(const float pos[]);
 void SaveActualRawEncoderPositions(const long int * pos);

 // 2x: stores "valve" into Actual2VoltageOutputs variable (local coord system)
 void SaveActual2VoltageOutputs(const float valve[]);
 void SaveActual2VoltageOutputs(const double * valve);

 // 2x: stores "valve" into Actual2RawSensorInputs variable (local coord system)
 void SaveActual2RawSensorInputs(const float valve[]);
 void SaveActual2RawSensorInputs(const double * valve);

 // 2x: stores "valve" into Actual2SensorValues variable (local coord system)
 void SaveActual2SensorValues(const float valve[]);
 void SaveActual2SensorValues(const double * valve);
 
 bool IsThereNewTargetJointPositions(void);  // did TargetJointPositions change since last time?
 void LoadTargetJointPositions(float pos[]); // copies TargetJointPositions into "pos" and flips the flag
 void LoadTargetJointPositions(double *pos); // copies TargetJointPositions into "pos" and flips the flag
 //void LoadTargetJointPositions(double_it start, double_it end); // copies TargetJointPositions into "pos" and flips the flag

 bool IsThereNewTargetVoltageOutputs(void);  // did TargetVoltageOutputs change since last time?
 void LoadTargetVoltageOutputs(float pos[]); // copies TargetVoltageOutputs into "pos" and flips the flag

 bool IsThereNewTargetRobotCoordinates(void);  // did TargetRobotCoordinates change since last time?
 void LoadTargetRobotCoordinates(float p[3], float o[4]); // copies TargetRobotPosition/TargetRobotOrientation into "p/o" and flips the flag

protected:
 BRPtprOpenTracker *OpenTracker;        ///< The OpenTracker object for sending commands out
 

 // ----------- From here: the current position and status of the robot (Actual) ------------------------

 // Global coordinate system (GC) ---------
 bool  lRobotCoordinatesInitialized; ///< GlobCoord: ActualRobotPosition, ActualRobotOrientation & ActualDeptVector
 float ActualRobotPosition[3];          ///< GC:
 float ActualRobotOrientation[4];       ///< GC:
 float ActualDeptVector[3];                     ///< GC:

 bool  lZFrameInitialized; ///< ZFramePosition & ZFrameOrientation
 float ZFramePosition[3];               ///< GC:
 float ZFrameOrientation[4];    ///< GC:

 BRPTPRstatusType mCurrentStatus; ///< the current workphase of the robot

 // Local (Robot) coordinate system (LC) ----------
 int nTotalJointNr; ///< The number of the robot joints (likely 8)
 int nTotalValveNr; ///< The number of valves, usually 2*nTotalJointNr (likely 16)
 bool lActualJointPositions; ///< LocalCoord: ActualJointPositions, ActualRawEncoderPositions, Actual2VoltageOutputs, Actual2RawSensorInputs & Actual2SensorValues
 FloatVectorType ActualJointPositions;          ///< LC: The current joint positions
 FloatVectorType ActualRawEncoderPositions;     ///< LC: encoder count
 FloatVectorType Actual2VoltageOutputs;         ///< LC: 2x: command send to valves
 FloatVectorType Actual2RawSensorInputs;        ///< LC: 2x: voltage input
 FloatVectorType Actual2SensorValues;           ///< LC: 2x: pressure (voltage input x something)


 // ----------- From here: the desired position and status of the robot (Target) --------------------------

 bool  IsTargetJointPositionsChanged;   ///< TargetJointPositions
 FloatVectorType TargetJointPositions;  ///< GC: Here should we move the robot (command) - joint positions
 
 bool  IsTargetVoltageOutputsChanged;   ///< TargetVoltageOutputs
 FloatVectorType TargetVoltageOutputs;  ///< ?

 // Global coordinate system
 bool  lTargetRobotCoordinates;         ///< TargetRobotPosition & TargetRobotOrientation
 float TargetRobotPosition[3];          ///< GC: Here should we go with the robot (command) - position
 float TargetRobotOrientation[4];       ///< GC: Here should we go with the robot (command) - orientation

};

#endif // _ctfControlBase_h_

