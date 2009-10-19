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

#include "vtkHybridNavWin32Header.h"
#include "vtkObject.h"
#include "vtkMRMLLinearTransformNode.h"
#include "vtkMatrix4x4.h"
#include "itkPoint.h"
#include <vector>

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
class vtkPivotCalibration : public vtkObject
{

public:

  static vtkPivotCalibration* New();
  vtkTypeRevisionMacro(vtkPivotCalibration, vtkObject);

  /** This method sets the number of transformations required for performing
   *  the pivot calibration and the tracker MRML node information.
   *  It is assumed that the tracker is already in tracking mode and that the
   *  tool is connected to the given port and channel. */
  void Initialize( unsigned int n, vtkMRMLNode* node );

  /* This method is part of the data acquisition before the actual calibration
   * process. We add a transform to the vector which stores the data. When the
   * calibration has been calculated correctly the function returns 1, else 0.
   */
  int AcquireTransform();

  /** This method performs the calibration. It generates
   *  several events */
  void ComputeCalibration();

  /** This method resets the class to its initial state prior to computing a
   *  calibration (all user provided transformations are removed). */
  void ResetCalibration();

  /** This method sets the tolerance for the singular values of the matrix A
   *  below which they are considered to be zero. The default setting is
   *  PivotCalibrationAlgorithm::DEFAULT_SINGULAR_VALUE_THRESHOLD .
   *  The solution exists only if rank(A)=6 (columns are linearly independent).
   *  The rank is evaluated using SVD(A) = USV^T, after zeroing out all
   *  singular values (diagonal entries of S) that are less than the
   *  tolerance. */
  void SetSingularValueThreshold( double threshold );


  /** This method is used to request the calibration transformation.
   *  The method should only be invoked after a successful calibration.
   *  It generates two events: CoordinateSystemTransformToEvent, and
   *  TransformNotAvailableEvent, respectively denoting that a calibration
   *  transform is and isn't available. */
  //void RequestCalibrationTransform();

  /** This method is used to request the pivot point, given in the coordinate
   *  system in which the user supplied transforms were given. It generates two
   *  events: PointEvent, and InvalidRequestErrorEvent, respectively denoting
   *  that the pivot point is and isn't available. */
  //void RequestPivotPoint();

  /** This method is used to request the Root Mean Square Error (RMSE) of the
   *  overdetermined equation system used to perform pivot calibration. It
   *  generates two events: DoubleTypeEvent, and InvalidRequestErrorEvent,
   *  respectively denoting that the RMSE is and isn't available.
   *  \sa PivotCalibrationAlgorithm */
  //void RequestCalibrationRMSE();



protected:

  vtkPivotCalibration ( void );
  virtual ~vtkPivotCalibration ( void );

public:

  vtkMRMLLinearTransformNode* transformNode;        //MRML node which contains tracking information
  vtkMatrix4x4* CalibrationTransform;   //transform from tracked position to tool tip point

private:
//BTX
  //maximal number of retries when acquiring tracking data before
  //it is considered an error
  //static const unsigned int MAXIMAL_RETRIES;

  //--------------------------------------------------------------------------
  //Variables Data Acquisition

  std::vector< vtkMatrix4x4* > m_Transforms;        //vector of acquired tracked transforms
  unsigned int m_RequiredNumberOfTransformations;   //number of transformation we want to acquire

  //----------------------------------------------------------------------------
  //Flags indicating errors
  bool bInitializeError;
  bool bComputationError;

  //----------------------------------------------------------------------------
  //Variables Computation

  itk::Point< double,3 > m_PivotPoint;  //pivot point (tip position)
  itk::Point< double,3 > m_Translation;

  //root mean squared error of the vector [Ax-b]
  double m_RMSE;

  //below this threshold the singular values of SVD(A) are set to zero
  double m_SingularValueThreshold;
  static const double DEFAULT_SINGULAR_VALUE_THRESHOLD;


//ETX

};

#endif //__vtkPivotCalibration_h
