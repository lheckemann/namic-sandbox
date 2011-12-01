/*=========================================================================

  Program:        OpenCV support for Visualization Toolkit
  Module:         vtkOpenCVRenderer.h
  Contributor(s): Junichi Tokuda (tokuda@bwh.harvard.edu)

  Copyright (c) Brigham and Women's Hospital, All rights reserved.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkOpenCVRenderer - abstract specification for renderers
// .SECTION Description

// .SECTION See Also
// vtkRenderer vtkRenderWindow vtkActor vtkCamera vtkLight vtkVolume

#ifndef __vtkOpenCVRenderer_h
#define __vtkOpenCVRenderer_h

// VTK header files and prototypes
#include "vtkRenderer.h"
class vtkImageActor;
class vtkImageData;
class vtkRenderWindow;

// OpenCV header files and protoptyes
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/types_c.h"
#include "opencv2/highgui/highgui.hpp"

class VTK_RENDERING_EXPORT vtkOpenCVRenderer : public vtkRenderer
{

public:

  vtkTypeMacro(vtkOpenCVRenderer,vtkRenderer);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  // Description:
  static vtkOpenCVRenderer *New();

  // Description:
  // SetUseCameraMatrix(1) activates the use of camera calibration matrix.
  // Note that the camera calibration is not automatically activated
  // after calling ImportCameraCalibrationFile()
  vtkSetMacro(UseCameraMatrix, int);
  vtkGetMacro(UseCameraMatrix, int);
  
  // Description:
  // Set VideoCapture class as video source
  // 'capture' must be opened prior to SetVideoCapture() call.
  int SetVideoCapture(cv::VideoCapture * capture);

  // Description:
  // Capture image and render. If image process handler is set,
  // it will be called after importing the captured image.
  int Capture();

  // Description:
  // Import camera calibration matrix from a camera calibration file in OpenCV's
  // format (*.yml).
  int ImportCameraCalibrationFile(const char * calibFile);


//BTX
protected:
  
  vtkOpenCVRenderer();
  ~vtkOpenCVRenderer();
  
  vtkImageActor *           Actor;
  vtkImageData *            VideoImageData;

  cv::Ptr<cv::VideoCapture> VideoSource;
  cv::Mat                   CaptureImage;
  cv::Mat                   CaptureImageTmp;
  cv::Mat                   RGBImage;
  cv::Mat                   GrayImage;
  cv::Mat                   UndistortedImage;
  cv::Size                  ImageSize;

  // Camera parameter
  int                       UseCameraMatrix;
  cv::Mat                   CameraMatrix, DistCoeffs;
  cv::Size                  CalibratedImageSize;
  cv::Mat                   Map1, Map2;

  std::vector< cv::Mat >    Map1Array;
  std::vector< cv::Mat >    Map2Array;
  
private:

  vtkOpenCVRenderer(const vtkOpenCVRenderer&);  // Not implemented.
  void operator=(const vtkOpenCVRenderer&);  // Not implemented.

  // Description:
  // To improve the speed of undistortion process, the OpenCVRenderer call
  // cv::initUndistortRectifyMap() once as an initialization process and 
  // cv::remap() in each capture, instead of executing both functions in each
  // capture by calling cv::undistort().
  void InitUndistort(int rows, int cols, cv::InputArray _cameraMatrix,
                     cv::InputArray _distCoeffs,
                     std::vector< cv::Mat >& _map1_array,
                     std::vector< cv::Mat >& _map2_array );

  void FastUndistort( cv::InputArray _src, cv::OutputArray _dst,
                      std::vector< cv::Mat >& _map1_array,
                      std::vector< cv::Mat >& _map2_array,
                      int rows, int cols);
  
//ETX
};

#endif
