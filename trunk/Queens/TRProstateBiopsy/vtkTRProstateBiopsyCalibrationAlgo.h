/*=auto=========================================================================

  Portions (c) Copyright 2007 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: $
  Date:      $Date: $
  Version:   $Revision: $

=========================================================================auto=*/
// .NAME vtkTRProstateBiopsyCalibrationAlgo - ...
// .SECTION Description
// vtkTRProstateBiopsyCalibrationAlgo ... TODO: to be completed

#ifndef __vtkTRProstateBiopsyCalibrationAlgo_h
#define __vtkTRProstateBiopsyCalibrationAlgo_h

#include "vtkObject.h"
#include "vtkTRProstateBiopsyWin32Header.h"

class vtkMatrix4x4;
class vtkImageData;
class vtkTRProstateBiopsyTargetDescriptor;
class vtkMRMLTRProstateBiopsyModuleNode;
class vtkPoints;

#include <vector>
#include "itkPoint.h"

const int CALIB_MARKER_COUNT=4;

//BTX
struct TRProstateBiopsyCalibrationFromImageInput
{
  double MarkerInitialPositions[CALIB_MARKER_COUNT][3]; // in RAS coordinates
  double MarkerSegmentationThreshold[CALIB_MARKER_COUNT];
  double MarkerDimensionsMm[3];
  double MarkerRadiusMm;
  double RobotInitialAngle;
  vtkMatrix4x4 *VolumeIJKToRASMatrix;
  vtkImageData *VolumeImageData;
};
//ETX

struct TRProstateBiopsyCalibrationFromImageOutput
{
  bool MarkerFound[CALIB_MARKER_COUNT];
  double MarkerPositions[CALIB_MARKER_COUNT][3]; // in RAS coordinates
};

struct TRProstateBiopsyCalibrationData
{
  bool CalibrationValid;
  double AxesDistance;
  double RobotRegistrationAngleDegrees; // registration angle in degrees
  double AxesAngleDegrees; // angle alpha between two axes in degrees
  double I1[3];
  double I2[3]; 
  double v1[3];
  double v2[3];
};

class VTK_TRPROSTATEBIOPSY_EXPORT vtkTRProstateBiopsyCalibrationAlgo :
  public vtkObject
{
public:

  static vtkTRProstateBiopsyCalibrationAlgo *New();
  vtkTypeRevisionMacro(vtkTRProstateBiopsyCalibrationAlgo,vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  //BTX
  typedef itk::Point<double, 3> PointType;

  // Description
  // ... TODO: to be completed
  bool CalibrateFromImage(const TRProstateBiopsyCalibrationFromImageInput &input, TRProstateBiopsyCalibrationFromImageOutput &output);  
  //ETX

  bool FindTargetingParams(vtkMRMLTRProstateBiopsyModuleNode* node, vtkTRProstateBiopsyTargetDescriptor *target);

  vtkImageData *GetCalibMarkerPreProcOutput(int i);
  void GetAxisCenterpoints(vtkPoints *points, int i);

  const TRProstateBiopsyCalibrationData& GetCalibrationData() { return this->CalibrationData; }

protected:

  bool RotatePoint(double H_before[3], double rotation_rad, double alpha_rad, double mainaxis[3], double I[3], /*out*/double H_after[3]);

  //BTX
  void SegmentAxis(const double initPos1[3], const double initPos2[3], vtkMatrix4x4 *volumeIJKToRASMatrix, vtkImageData* calibVol,
    double thresh1, double thresh2, const double fidDimsMm[3], double radius, double initialAngle, 
    double P1[3], double v1[3], double finalPos1[3], double finalPos2[3], bool &found1, bool &found2, vtkImageData* img1, vtkImageData* img2, std::vector<typename PointType> *CoordinatesVectorAxis);
  // thresh: 0..100, binary threshold value (0 corresponds to min voxel value; 100 corresponds to max voxel value in the image)
  bool SegmentCircle(double markerCenterGuessRas[3],const double normalRas[3],  double thresh, const double fidDimsMm[3], double radius, vtkMatrix4x4 *ijkToRAS, vtkImageData *calibVol, std::vector<PointType> &CoordinatesVector, vtkImageData *preprocOutput=NULL);
  //ETX
  bool CalculateCircleCenter(vtkImageData *inData, unsigned int *tempStorage, int tempStorageSize, double nThersholdVal, double nRadius, double &gx, double &gy, int nVotedNeeded, bool lDebug);  
  bool CalculateCircleCenterMean(vtkImageData *inData, double nRadius, double threshold, double &gx, double &gy);  
  //BTX
  void RemoveOutliners(double P_[3], double v_[3], const double def1[3], const double def2[3], std::vector<typename PointType> &CoordinatesVector);
  //ETX
  bool FindProbe(const double P1[3], const double P2[3], double v1[3], double v2[3], 
    double I1[3], double I2[3], double &axesAngleDegrees, double &axesDistance);
  //BTX
  void Linefinder(double P_[3], double v_[3], std::vector<itk::Point<double,3> > CoordVector);
  //ETX

  bool DoubleEqual(double val1, double val2);

  vtkTRProstateBiopsyCalibrationAlgo();
  virtual ~vtkTRProstateBiopsyCalibrationAlgo();

  //BTX
  std::vector<PointType> CoordinatesVectorAxis1;
  std::vector<PointType> CoordinatesVectorAxis2;
  std::vector<vtkImageData*> CalibMarkerPreProcOutput;
  //ETX

  TRProstateBiopsyCalibrationData CalibrationData;

private:
  vtkTRProstateBiopsyCalibrationAlgo(const vtkTRProstateBiopsyCalibrationAlgo&);  // Not implemented.
  void operator=(const vtkTRProstateBiopsyCalibrationAlgo&);  // Not implemented.
};

#endif