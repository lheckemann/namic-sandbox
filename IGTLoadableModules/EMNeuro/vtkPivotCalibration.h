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
                          vtkMRMLnode* node );

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

protected:

  PivotCalibration ( void );
  ~PivotCalibration ( void );

  /** Print the object information in a stream. */
  void PrintSelf( std::ostream& os, itk::Indent indent ) const;

private:

  //maximal number of retries when acquiring tracking data before
  //it is considered an error
  static const unsigned int MAXIMAL_RETRIES;

  //transformations used for pivot calibration
  std::vector< PivotCalibrationAlgorithm::TransformType > m_Transforms;
  //tracker used for pivot calibration
  Tracker::Pointer      m_TmpTracker;
  Tracker::Pointer      m_Tracker;
  //number of transformation we want to acquire
  unsigned int m_TmpRequiredNumberOfTransformations;
  unsigned int m_RequiredNumberOfTransformations;

           //the object that actually does all the work
  PivotCalibrationAlgorithm::Pointer m_PivotCalibrationAlgorithm;
};

#endif //__vtkPivotCalibration_h
