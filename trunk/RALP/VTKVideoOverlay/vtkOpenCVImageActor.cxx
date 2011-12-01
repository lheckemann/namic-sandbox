/*=========================================================================

  Program:        OpenCV support for Visualization Toolkit
  Module:         vtkOpenCVImageActor.h
  Contributor(s): Junichi Tokuda (tokuda@bwh.harvard.edu)

  Copyright (c) Brigham and Women's Hospital, All rights reserved.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

// VTK header files
#include "vtkImageActor.h"
#include "vtkImageData.h"
#include "vtkRenderWindow.h"
#include "vtkGraphicsFactory.h"
#include "vtkCamera.h"
#include "vtkOpenCVImageActor.h"

// OpenCV header files
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/types_c.h"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

//----------------------------------------------------------------------------
// Needed when we don't use the vtkStandardNewMacro.
vtkInstantiatorNewMacro(vtkOpenCVImageActor);
//vtkStandardNewMacro(vtkOpenCVImageActor);

//----------------------------------------------------------------------------
vtkOpenCVImageActor::vtkOpenCVImageActor()
{
  this->Actor           = NULL;
  this->VideoImageData  = NULL;
  this->UseCameraMatrix = 0;
  this->SetLayer(0);
}


//----------------------------------------------------------------------------
vtkOpenCVImageActor::~vtkOpenCVImageActor()
{
  if (this->Actor)
    {
    this->Actor->Delete();
    }
  if (this->VideoImageData)
    {
    this->VideoImageData->Delete();
    }
}


//----------------------------------------------------------------------------
// return the correct type of Actor
vtkOpenCVImageActor *vtkOpenCVImageActor::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkGraphicsFactory::CreateInstance("vtkOpenCVImageActor");
  return static_cast<vtkOpenCVImageActor *>(ret);
}


//----------------------------------------------------------------------------
int vtkOpenCVImageActor::SetVideoCapture(cv::VideoCapture * capture)
{

  if( !capture->isOpened() )
    {
    std::cerr << "Could not initialize capturing...\n";
    return 0;
    }

  this->VideoSource = capture;

  // Capture the first frame to check the video size
  cv::Mat frame;
  *(this->VideoSource) >> frame; 
  
  if( frame.empty() )
    {
    std::cerr << "\n\nCouldn't take a picture\n\n" << std::endl;
    return 0;
    }

  this->ImageSize = frame.size();

  this->VideoImageData = vtkImageData::New();
  this->VideoImageData->SetDimensions(this->ImageSize.width, this->ImageSize.height, 1);
  this->VideoImageData->SetExtent(0, this->ImageSize.width-1, 0, this->ImageSize.height-1, 0, 0 );
  this->VideoImageData->SetSpacing(1.0, 1.0, 1.0);
  this->VideoImageData->SetOrigin(0.0, 0.0, 0.0);
  this->VideoImageData->SetNumberOfScalarComponents(3);
  this->VideoImageData->SetScalarTypeToUnsignedChar();
  this->VideoImageData->AllocateScalars();
  this->VideoImageData->Update();

  this->Actor = vtkImageActor::New();
  this->Actor->SetInput(this->VideoImageData);
  this->AddActor(this->Actor);
  this->InteractiveOff();

  this->Actor->Modified();

  // Adjust camera position so that image covers the draw area.
  double origin[3];
  double spacing[3];
  int    extent[6];

  this->VideoImageData->GetOrigin( origin );
  this->VideoImageData->GetSpacing( spacing );
  this->VideoImageData->GetExtent( extent );
  
  vtkCamera* camera = this->GetActiveCamera();
  camera->ParallelProjectionOn();
  double xc = origin[0] + 0.5*(extent[0] + extent[1])*spacing[0];
  double yc = origin[1] + 0.5*(extent[2] + extent[3])*spacing[1];
  double yd = (extent[3] - extent[2] + 1)*spacing[1];
  double d = camera->GetDistance();
  camera->SetParallelScale(0.5*yd);
  camera->SetFocalPoint(xc,yc,0.0);
  camera->SetPosition(xc,yc,d);

  return 1;
}


//----------------------------------------------------------------------------
int vtkOpenCVImageActor::Capture()
{

  cv::Size newImageSize;

  if (!this->VideoSource->isOpened())
    {
    return 0;
    }

  *(this->VideoSource) >> this->CaptureImageTmp;
  if (this->CaptureImageTmp.empty())
    {
    std::cerr << "\n\nCouldn't take a picture\n\n" << std::endl;
    return 0;
    }

  // 5/6/2010 ayamada creating RGB image and capture image
  newImageSize = this->CaptureImageTmp.size();

  // check if the image size is changed
  if (newImageSize.width != this->ImageSize.width||
      newImageSize.height != this->ImageSize.height)
    {
    this->ImageSize.width  = newImageSize.width;
    this->ImageSize.height = newImageSize.height;
    
    // The following code may not be necessary
    this->VideoImageData->SetDimensions(newImageSize.width, newImageSize.height, 1);
    this->VideoImageData->SetExtent(0, newImageSize.width-1, 0, newImageSize.height-1, 0, 0 );
    this->VideoImageData->SetNumberOfScalarComponents(3);
    this->VideoImageData->SetScalarTypeToUnsignedChar();
    this->VideoImageData->AllocateScalars();
    this->VideoImageData->Update();
    
    this->Actor->SetInput(VideoImageData);
    }

  // create rgb image
  cv::flip(this->CaptureImageTmp, this->CaptureImage, 0);
  if (this->UseCameraMatrix)
    {
    //cv::undistort( this->CaptureImage, this->UndistortedImage, cameraMatrix, distCoeffs);
    FastUndistort( this->CaptureImage, this->UndistortedImage, this->Map1Array, this->Map2Array,
                   this->CalibratedImageSize.height, this->CalibratedImageSize.width );
    cv::cvtColor( this->UndistortedImage, this->RGBImage, CV_BGR2RGB);
    cv::cvtColor( this->UndistortedImage, this->GrayImage, CV_BGR2GRAY); 
    }
  else
    {
    cv::cvtColor( this->CaptureImage, this->RGBImage, CV_BGR2RGB);
    cv::cvtColor( this->CaptureImage, this->GrayImage, CV_BGR2GRAY); 
    }

  //
  // OpenCV image process handler here
  //

  if (this->VideoImageData)
    {
    unsigned char* idata;    
    // 5/6/2010 ayamada ok for videoOverlay
    idata = (unsigned char*) this->RGBImage.ptr();
    
    int dsize = this->ImageSize.width*this->ImageSize.height*3;
    memcpy((void*)VideoImageData->GetScalarPointer(), (void*)idata, dsize);
    this->VideoImageData->Modified();

    //
    // VTK image process handler here
    //
    if (this->GetRenderWindow())
      {
      this->GetRenderWindow()->Render();
      }
    }
  
  return 1;
}


//----------------------------------------------------------------------------
int vtkOpenCVImageActor::ImportCameraCalibrationFile(const char * calibFile)
{

  cv::FileStorage fs(calibFile, cv::FileStorage::READ);
  
  fs["image_width"] >> this->CalibratedImageSize.width;
  fs["image_height"] >> this->CalibratedImageSize.height;
  fs["distortion_coefficients"] >> this->DistCoeffs;
  fs["camera_matrix"] >> this->CameraMatrix;
  
  if( this->DistCoeffs.type() != CV_64F )
    {
    this->DistCoeffs = cv::Mat_<double>(this->DistCoeffs);
    }
  if( this->CameraMatrix.type() != CV_64F )
    {
    this->CameraMatrix = cv::Mat_<double>(this->CameraMatrix);
    }
  
  //print out the result
  std::cerr << "Imported camera calibration information:" << std::endl;
  std::cerr << "Image size: "
            << this->CalibratedImageSize.width << ", "
            << this->CalibratedImageSize.height << std::endl;
  std::cerr << "Camera matrix: " << std::endl;
  std::cerr << this->CameraMatrix << std::endl;
  std::cerr << "Distance coefficients: " << std::endl;
  std::cerr << this->DistCoeffs << std::endl;
  
  // Create undistort rectify map
  InitUndistort(this->CalibratedImageSize.height, this->CalibratedImageSize.width, 
                this->CameraMatrix, this->DistCoeffs, this->Map1Array, this->Map2Array);
  
  //this->UseCameraMatrix = 1;
}


//----------------------------------------------------------------------------
void vtkOpenCVImageActor::GetImageSize(unsigned int& width, unsigned int& height)
{

  width  = this->ImageSize.width;
  height = this->ImageSize.height;

}


//----------------------------------------------------------------------------
void vtkOpenCVImageActor::InitUndistort(int rows, int cols, cv::InputArray _cameraMatrix,
                   cv::InputArray _distCoeffs,
                   std::vector< cv::Mat >& _map1_array,
                   std::vector< cv::Mat >& _map2_array )
{
  cv::Mat cameraMatrix = _cameraMatrix.getMat();
  cv::Mat distCoeffs = _distCoeffs.getMat();
  
  int stripe_size0 = std::min(std::max(1, (1 << 12) / std::max(cols, 1)), rows);
  cv::Mat map1(stripe_size0, cols, CV_16SC2), map2(stripe_size0, cols, CV_16UC1);
  
  cv::Mat_<double> A, Ar, I = cv::Mat_<double>::eye(3,3);
  
  cameraMatrix.convertTo(A, CV_64F);
  if( distCoeffs.data )
    distCoeffs = cv::Mat_<double>(distCoeffs);
  else
    {
    distCoeffs.create(5, 1, CV_64F);
    distCoeffs = 0.;
    }
  
  A.copyTo(Ar);
  
  double v0 = Ar(1, 2);

  _map1_array.resize(rows);
  _map2_array.resize(rows);
  
  for( int y = 0; y < rows; y += stripe_size0 )
    {
    int stripe_size = std::min( stripe_size0, rows - y );
    Ar(1, 2) = v0 - y;
    (map1.rowRange(0, stripe_size)).copyTo(_map1_array[y]);
    (map2.rowRange(0, stripe_size)).copyTo(_map2_array[y]);
    
    cv::initUndistortRectifyMap( A, distCoeffs, I, Ar, cv::Size(cols, stripe_size),
                                 _map1_array[y].type(), _map1_array[y], _map2_array[y] );

    }
}


//----------------------------------------------------------------------------
void vtkOpenCVImageActor::FastUndistort( cv::InputArray _src, cv::OutputArray _dst,
                                        std::vector< cv::Mat >& _map1_array,
                                        std::vector< cv::Mat >& _map2_array,
                                        int rows, int cols)
{

  cv::Mat src = _src.getMat();
  _dst.create( src.size(), src.type() );
  cv::Mat dst = _dst.getMat();

  cv::Mat_<double> Ar = cv::Mat_<double>::eye(3,3);

  int stripe_size0 = std::min(std::max(1, (1 << 12) / std::max(cols, 1)), rows);

  for( int y = 0; y < rows; y += stripe_size0 )
    {
    int stripe_size = std::min( stripe_size0, rows - y );
    cv::Mat dst_part = dst.rowRange(y, y + stripe_size);
    cv::remap( src, dst_part, _map1_array[y], _map2_array[y], cv::INTER_LINEAR, cv::BORDER_CONSTANT );
    }

}

