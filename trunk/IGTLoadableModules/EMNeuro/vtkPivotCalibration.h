/*=========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

=========================================================================*/

#ifndef __vtkPivotCalibration_h
#define __vtkPivotCalibration_h

#include "vtkPivotCalibrationAlgorithm.h"

/** PivotCalibration
 *
 *  This class encapsulates the pivot calibration algorithm and tracking
 *         data acquisition for performing tool tip calibration.
 *
 *  This class enables you to perform pivot calibration (tool tip calibration).
 *  The class is responsible for acquisition of tracking data and computation of
 *  the pivot calibration. You set the tracker of the specific tool and number
 *  of required transformations using the RequestInitialize() method. The class
 *  expects the tracker to be in Active tracking state and will generate an error if
 *  this is not the case. Once initialized the RequestComputeCalibration()
 *  method will start data acquisition and perform calibration. If data
 *  acquisition fails multiple times (e.g. line of sight is blocked for optical
 *  tracker) the calibration will fail and the appropriate event is generated.
 */
class PivotCalibration
{

public:

  /** This method sets the number of transformations required for performing
   *  the pivot calibration and the tracker information.
   *  It is assumed that the tracker is already in tracking mode and that the
   *  tool is connected to the given port and channel. */
  void RequestInitialize( unsigned int n,
                          igstk::Tracker * tracker,
                          igstk::TrackerTool * trackerTool );

  /** This method performs the data acquisition and calibration. It generates
   *  several events: CalibrationSuccessEvent, CalibrationFailureEvent,
   *  DataAcquistionStartEvent, DataAcquisitionEvent, and
   *  DataAcquisitionEndEvent.
   *  They denote success or failure of the acquisition and computation, the
   *  fact that acquisition of tracking data has started, data was acquired
   *  (contains the percentage out of the required tracking data), and that the
   *  acquisition is done. */
  void RequestComputeCalibration();

  /** This method is used to request the calibration transformation.
   *  The method should only be invoked after a successful calibration.
   *  It generates two events: CoordinateSystemTransformToEvent, and
   *  TransformNotAvailableEvent, respectively denoting that a calibration
   *  transform is and isn't available. */
  void RequestCalibrationTransform();

  /** This method is used to request the pivot point, given in the coordinate
   *  system in which the user supplied transforms were given. It generates two
   *  events: PointEvent, and InvalidRequestErrorEvent, respectively denoting
   *  that the pivot point is and isn't available. */
  void RequestPivotPoint();

  /** This method is used to request the Root Mean Square Error (RMSE) of the
   *  overdetermined equation system used to perform pivot calibration. It
   *  generates two events: DoubleTypeEvent, and InvalidRequestErrorEvent,
   *  respectively denoting that the RMSE is and isn't available.
   *  \sa PivotCalibrationAlgorithm */
  void RequestCalibrationRMSE();

   /** This event is generated if the initialization succeeds. */
  igstkEventMacro( InitializationSuccessEvent, IGSTKEvent );

   /** This event is generated if the initialization fails (e.g. given tracker
    *  object is not in tracking state). */
  igstkEventMacro( InitializationFailureEvent, IGSTKEvent );

   /** This event is generated if the pivot calibration computation succeeds. */
  igstkEventMacro( CalibrationSuccessEvent, IGSTKEvent );

  /** This event is generated if the pivot calibration fails, either due to data
   *  acquisition problems or computation failure. The event object contains a
   *  message (std::string) describing the exact reason for failure.*/
  igstkLoadedEventMacro( CalibrationFailureEvent,
                         IGSTKEvent,
                         EventHelperType::StringType );

  /** This event is generated when data acquisition starts. */
  igstkEventMacro( DataAcquisitionStartEvent, IGSTKEvent );

  /** This event is generated when a transformation is acquired from the
   *  tracker. It contains the percentage of acquired data. */
  igstkEventMacro( DataAcquisitionEvent, DoubleTypeEvent );

    /** This event is generated when data acquisition ends. */
  igstkEventMacro( DataAcquisitionEndEvent, IGSTKEvent );

protected:

  PivotCalibrationNew( void );
  ~PivotCalibrationNew ( void );

  /** Print the object information in a stream. */
  void PrintSelf( std::ostream& os, itk::Indent indent ) const;

private:
             //maximal number of retries when acquiring tracking data before
             //it is considered an error
  static const unsigned int MAXIMAL_RETRIES;

   /** List of state machine states */
  igstkDeclareStateMacro( Idle );
  igstkDeclareStateMacro( AttemptingToInitialize );
  igstkDeclareStateMacro( Initialized );
  igstkDeclareStateMacro( AttemptingToComputeCalibration );
  igstkDeclareStateMacro( CalibrationComputed );


  /** List of state machine inputs */
  igstkDeclareInputMacro( Initialize );
  igstkDeclareInputMacro( Failed  );
  igstkDeclareInputMacro( Succeeded  );
  igstkDeclareInputMacro( ComputeCalibration );
  igstkDeclareInputMacro( GetTransform  );
  igstkDeclareInputMacro( GetPivotPoint  );
  igstkDeclareInputMacro( GetRMSE  );
  //igstkDeclareInputMacro( CalibrationComputationSuccess  );
  //igstkDeclareInputMacro( CalibrationComputationFailure  );

  /**List of state machine actions*/
  void ReportInvalidRequestProcessing();
  void InitializeProcessing();
  void ReportInitializationFailureProcessing();
  void ReportInitializationSuccessProcessing();
  void ComputeCalibrationProcessing();
  void ReportCalibrationComputationSuccessProcessing();
  void ReportCalibrationComputationFailureProcessing();
  void GetTransformProcessing();
  void GetPivotPointProcessing();
  void GetRMSEProcessing();

  class ErrorObserver : public itk::Command
  {
  public:
    typedef ErrorObserver                    Self;
    typedef ::itk::Command                   Superclass;
    typedef ::itk::SmartPointer<Self>        Pointer;
    typedef ::itk::SmartPointer<const Self>  ConstPointer;

    igstkNewMacro(Self)
    igstkTypeMacro(ErrorObserver, itk::Command)

    /** When an error occurs in an IGSTK component it will invoke this method
     *  with the appropriate error event object as a parameter.*/
    virtual void Execute(itk::Object *caller,
                         const itk::EventObject & event) throw (std::exception);

    /** When an error occurs in an IGSTK component it will invoke this method
     *  with the appropriate error event object as a parameter.*/
    virtual void Execute(const itk::Object *caller,
                         const itk::EventObject & event) throw (std::exception);

    /**Clear the current error.*/
    void ClearError() {this->m_ErrorOccured = false;
                       this->m_ErrorMessage.clear();}
    /**If an error occurs in one of the observed IGSTK components this method
     * will return true.*/
    bool ErrorOccured() {return this->m_ErrorOccured;}
    /**Get the error message associated with the last IGSTK error.*/
    void GetErrorMessage(std::string &errorMessage)
    {
      errorMessage = this->m_ErrorMessage;
    }

  protected:

    /**Construct an error observer for all the possible errors that occur in
     * IGSTK components.*/
    ErrorObserver();
    virtual ~ErrorObserver(){}
  private:
    bool m_ErrorOccured;
    std::string m_ErrorMessage;
    std::map<std::string,std::string> m_ErrorEvent2ErrorMessage;

             //purposely not implemented
    ErrorObserver(const Self&);
    void operator=(const Self&);
  };

  //definitions of the observer classes of events generated by the
  //PivotCalibrationAlgorithm
  igstkObserverMacro( CalibrationTransform,
                      CoordinateSystemTransformToEvent,
                      CoordinateSystemTransformToResult )

  igstkObserverMacro( PivotPoint, PointEvent, EventHelperType::PointType )

  igstkObserverMacro( CalibrationRMSE,
                      DoubleTypeEvent,
                      EventHelperType::DoubleType )


  CalibrationTransformObserver::Pointer m_GetCalibrationTransformObserver;
  PivotPointObserver::Pointer m_GetPivotPointObserver;
  CalibrationRMSEObserver::Pointer m_GetCalibrationRMSEObserver;

  ErrorObserver::Pointer m_ErrorObserver;
  std::string m_ReasonForCalibrationFailure;

             //transformations used for pivot calibration
  std::vector< PivotCalibrationAlgorithm::TransformType > m_Transforms;
              //tracker used for pivot calibration
  Tracker::Pointer      m_TmpTracker;
  Tracker::Pointer      m_Tracker;
             //tool we want to calibrate
  TrackerTool::Pointer  m_TmpTrackerTool;
  TrackerTool::Pointer  m_TrackerTool;
          //number of transformation we want to acquire
  unsigned int m_TmpRequiredNumberOfTransformations;
  unsigned int m_RequiredNumberOfTransformations;

           //the object that actually does all the work
  PivotCalibrationAlgorithm::Pointer m_PivotCalibrationAlgorithm;
};

#endif //__vtkPivotCalibration_h
