/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/

#ifndef __vtkPivotCalibrationAlgorithm_h
#define __vtkPivotCalibrationAlgorithm_h

#include <vector>
#include "vtkObject.h"
#include "itkPoint.h"

class vtkMatrix4x4;

/** \class PivotCalibrationAlgorithm
 *
 *  \brief This class implements the pivot calibration algorithm, locating a
 *         tools tip relative to a tracked coordinate system.
 *
 *  We are interested in the location of a tool's tip relative to a tracked
 *  coordinate system. By rotating the tool while its tip location remains
 *  fixed we compute both the tip location relative to the tracked coordinate
 *  system and the fixed point's coordinates in the tracker's coordinate
 *  system. Every transformation \f$[R_i,\mathbf{t_i}]\f$ acquired by the
 *  tracker during the rotation yields three equations in six unknowns,
 *  \f$R_i \mathbf{t_{tip}} + \mathbf{t_i} = \mathbf{t_{fixed}}\f$, with the
 *  unknown 3x1 vectors \f$\mathbf{t_{tip}}\f$ and \f$\mathbf{t_{fixed}}\f$.
 *  The solution is obtained in a least squares manner by acquiring multiple
 *  transformation while rotating the tool around its tip resulting in the
 *  following overdetermined equation system:
 *  \f$$ \left[
 *       \begin{array}{ccc}
 *         R_0 &        & -I \\
 *             & \vdots &    \\
 *         R_n &        & -I
 *       \end{array}
 *     \right]
 *     \left[
 *       \begin{array}{c}
 *         \bf{t_{tip}}\\
 *         \bf{t_{fixed}}
 *       \end{array}
 *     \right]
 *     =
 *     \left[
 *       \begin{array}{c}
 *         \bf{-t_0}\\
 *         \vdots\\
 *         \bf{-t_n}
 *       \end{array}
 *     \right] $$\f
 * Which is solved using the pseudoinverse (singular value decomposition).
 */
class vtkPivotCalibrationAlgorithm : public vtkObject
{

public:

  /** Macro with standard traits declarations (Self, SuperClass, State
   *  Machine etc.). */
  static vtkPivotCalibrationAlgorithm* New();
  vtkTypeRevisionMacro(vtkPivotCalibrationAlgorithm, vtkObject);

//BTX
  typedef itk::Point< double, 3 >    PointType;
//ETX
  /** This method adds the given transforms to those used to perform the pivot
   *  calibration. The method should only be invoked before calibration is
   *  performed or after it has been reset or it will generate an
   *  InvalidRequestErrorEvent.
   *  NOTE: The transforms are assumed to be valid. */
//BTX
  void AddTransformsVector( std::vector< vtkMatrix4x4* > t );
//ETX

  /** This method resets the class to its initial state prior to computing a
   *  calibration (all user provided transformations are removed). */
  void ResetCalibration();

  /** This method performs the calibration. It generates two events:
   *  CalibrationSuccessEvent, and CalibrationFailureEvent, denoting success
   *  or failure of the computation. Note that invoking this method prior to
   *  setting transfromations is considered valid, it will simply generate a
   *  CalibraitonFaliureEvent. In this way all sets of degenerate
   *  transformations are treated alike (empty set, data with low variability
   *  for example the same transform repeated many times).*/
  void ComputeCalibration();

  /** This method is used to request the calibration transformation.
   *  The method should only be invoked after a successful calibration.
   *  It generates two events: CoordinateSystemTransformToResult, and
   *  TransformNotAvailableEvent, respectively denoting that a calibration
   *  transform is and isn't available. */
  void GetCalibrationTransform(vtkMatrix4x4* trans);

  /** This method is used to request the pivot point, given in the coordinate
   *  system in which the user supplied transforms were given.
   *  It generates two events: PointEvent, and InvalidRequestErrorEvent,
   *  respectively denoting that the pivot point is and isn't available. */
  void GetPivotPoint();

  /** This method is used to request the Root Mean Square Error (RMSE) of the
   *  linear equation system used to compute the pivot point and calibration
   *  transformation (RMSE of the vector Ax-b, see class description for more
   *  details). It generates two events: DoubleTypeEvent, and
   *  InvalidRequestErrorEvent, respectively denoting that the RMSE is and
   *  isn't available. */
  void GetCalibrationRMSE();

  /** This method sets the tolerance for the singular values of the matrix A
   *  below which they are considered to be zero. The default setting is
   *  PivotCalibrationAlgorithm::DEFAULT_SINGULAR_VALUE_THRESHOLD .
   *  The solution exists only if rank(A)=6 (columns are linearly independent).
   *  The rank is evaluated using SVD(A) = USV^T, after zeroing out all
   *  singular values (diagonal entries of S) that are less than the
   *  tolerance. */
  void SetSingularValueThreshold( double threshold );

  /** Default threshold value under which singular values are considered to be
   *  zero. */
  static const double DEFAULT_SINGULAR_VALUE_THRESHOLD;

  /*
   * This method checks to see if the Calibration method executed correctly
   */
  bool GetErrorStatus();

protected:

  vtkPivotCalibrationAlgorithm  ( void );
  virtual ~vtkPivotCalibrationAlgorithm ( void );

  /** Print the object information in a stream. */
  //void PrintSelf( std::ostream& os, itk::Indent indent ) const;

private:

  //transformations used for pivot calibration
  //BTX
  std::vector <vtkMatrix4x4*> m_Transforms;
  //ETX

  //computed translation between tracked coordinate system and pivot point
  vtkMatrix4x4* m_Transform;

  //computed pivot point in the tracker's coordinate system
  //BTX
  itk::Point< double,3 > m_PivotPoint;
  itk::Point< double,3 > m_Translation;
  //ETX

  //root mean squared error of the vector [Ax-b]
  double m_RMSE;

  //below this threshold the singular values of SVD(A) are set to zero

  //change these variables for return functions
  double m_SingularValueThreshold;
  bool bCalibrationComputationError;

};

#endif //__vtkPivotCalibrationAlgorithm_h
