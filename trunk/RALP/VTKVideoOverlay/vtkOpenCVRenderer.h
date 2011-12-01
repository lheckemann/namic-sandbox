/*=========================================================================

  Program:        OpenCV support for Visualization Toolkit
  Module:         vtkOpenCVRenderer.h
  Contributor(s): Junichi Tokuda (tokuda@bwh.harvard.edu)

  Copyright (c) Brigham and Women's Hospital, All rights reserved.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkOpenCVActor - abstract specification for renderers
// .SECTION Description

// .SECTION See Also
// vtkActor vtkRenderWindow vtkActor vtkCamera vtkLight vtkVolume

#ifndef __vtkOpenCVActor_h
#define __vtkOpenCVActor_h

// VTK header files and prototypes
#include "vtkActor.h"
class vtkImageActor;
class vtkImageData;
class vtkRenderWindow;

// OpenCV header files and protoptyes
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/types_c.h"
#include "opencv2/highgui/highgui.hpp"

class VTK_RENDERING_EXPORT vtkOpenCVActor : public vtkActor
{

public:

  vtkTypeMacro(vtkOpenCVActor,vtkActor);
  void PrintSelf(ostream& os, vtkIndent indent);

  static vtkOpenCVActor *New();

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

  // Description:
  // Get the size of camera image
  void GetImageSize(unsigned int& width, unsigned int& height);

  // Description:
  // Create an image. Subclasses of vtkActor must implement this method.
  virtual void DeviceRender() {};

//BTX
protected:
  
  vtkOpenCVActor();
  ~vtkOpenCVActor();
  
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

  vtkOpenCVActor(const vtkOpenCVActor&);  // Not implemented.
  void operator=(const vtkOpenCVActor&);  // Not implemented.

  // Description:
  // To improve the speed of undistortion process, the OpenCVActor call
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
