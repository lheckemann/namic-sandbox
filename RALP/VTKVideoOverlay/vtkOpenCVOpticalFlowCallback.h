/*=========================================================================

  Program:        OpenCV support for Visualization Toolkit
  Module:         vtkOpenCVRendererDelegate.h
  Contributor(s): Junichi Tokuda (tokuda@bwh.harvard.edu)

  Copyright (c) Brigham and Women's Hospital, All rights reserved.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkOpenCVRendererDelegate - abstract specification for actor
// .SECTION Description

// .SECTION See Also
// vtkRendererDelegate vtkRenderWindow vtkRendererDelegate vtkCamera vtkLight vtkVolume

#ifndef __vtkOpenCVOpticalFlowCallback_h
#define __vtkOpenCVOpticalFlowCallback_h


// VTK header files and prototypes
#include "vtkObject.h"
#include "vtkMatrix4x4.h"
#include "vtkOpenCVCallback.h"

class vtkCamera;


// OpenCV header files and protoptyes
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/types_c.h"
#include "opencv2/highgui/highgui.hpp"

#define BUFFER_SIZE 300
#define NGRID_X  20
#define NGRID_Y  20
#define TIME_POINTS (30*30)

class VTK_COMMON_EXPORT vtkOpenCVOpticalFlowCallback : public vtkOpenCVCallback
{
  
public:
  vtkTypeMacro(vtkOpenCVOpticalFlowCallback, vtkOpenCVCallback);

  static vtkOpenCVOpticalFlowCallback *New()
  {
    return new vtkOpenCVOpticalFlowCallback;
  };
  
  // Description:
  // SetCameraTransform() sets the current position and orientation of the
  // camera in the coordinate system in vtkMatrix4x4 format.
  vtkSetObjectMacro(CameraTransform, vtkMatrix4x4);
  vtkGetObjectMacro(CameraTransform, vtkMatrix4x4);

  int InitBuffer();
  int StoreMotionField();
  int ProcessMotion(std::vector<cv::Point2f>& vector);
  virtual void Execute(vtkObject *vtkNotUsed(caller), unsigned long eventId, void *callData);

  //BTX
  void SetCameraMatrix(cv::Mat & mat);
  void SetCameraImageSize(cv::Size & size);
  //ETX

  int UpdateModelCamera(vtkCamera *camera, double rheight);
  
  //BTX
protected:
  vtkOpenCVOpticalFlowCallback();
  ~vtkOpenCVOpticalFlowCallback();
  
  // Description:
  // RotationalMotionModel() is used to generate a motion vector field based
  // on a model used in Park S.C. et al, Pattern Recognition 2004; 37:767-779.
  // The following argments must be specified:
  //   xy[2]: 2D position of the point on the camera image
  //   Om[3]: Camera rotation around X, Y and Z axes
  //   T[3]:  Camera translation along X, Y and Z axes
  //   f:     Focul length
  //   zoom:  Camera zoom
  //   uv[2]: Output vector
  inline void OpticalFlowModel(double * xy, double * Om, double * T, double f, double zoom, double * uv);
  
  //int           OpticalFlowTrackingFlag;
  cv::Mat GrayImage;
  cv::Mat PrevGrayImage;
  cv::Mat SwapTempImage;

  std::vector< cv::Point2f >  Points[2];
  std::vector< cv::Point2f >  GridPoints[2];// = {0,0};
  std::vector< cv::Point2f >  RVector;
  
  char*         OpticalFlowStatus;
  cv::Point2f * MotionFieldBuffer;
  int           BufferIndex;
  int           FileIndex;

  // Current camera position and orientation
  vtkMatrix4x4 *            CameraTransform;

  // Camera parameters
  cv::Mat                   CameraMatrix;
  cv::Size                  CalibratedImageSize;
 //ETX
  
};


#endif
