/*=========================================================================

  Program:   VTK-OpenCV Video Image Overlay Example
  Module:    $HeadURL: http://svn.na-mic.org/NAMICSandBox/trunk$
  Language:  C++
  Date:      $Date: $
  Version:   $Revision: 6525 $

  Copyright (c) Junichi Tokuda. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

// VTK stuff
#include "vtkActor.h"
#include "vtkCamera.h"
#include "vtkCellArray.h"
#include "vtkFloatArray.h"
#include "vtkPointData.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkImageData.h"
#include "vtkCommand.h"
#include "vtkCellData.h"
#include "vtkImageActor.h"

#include <vtkSmartPointer.h>


// OpenCV stuff
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/types_c.h"

#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace std;

//----------------------------------------------------------------
// Video import
//----------------------------------------------------------------
//CvCapture* capture;
cv::VideoCapture capture;
cv::Size         imageSize;
cv::Mat          captureImage;
cv::Mat          RGBImage;
cv::Mat          undistortionImage;

vtkImageData*    VideoImageData;

// Optical tracking
int           OpticalFlowTrackingFlag;
cv::Mat       GrayImage;
cv::Mat       PrevGrayImage;
cv::Mat       SwapTempImage;
int           PyrFlag;

std::vector< cv::Point2f >  Points[2];
std::vector< cv::Point2f >  SwapPoints;
std::vector< cv::Point2f >  GridPoints[2];// = {0,0};
std::vector< cv::Point2f >  RVector;

char*         OpticalFlowStatus;

// Camera parameter
int     fUseCameraMatrix;
cv::Mat cameraMatrix, distCoeffs;
cv::Size calibratedImageSize;
cv::Mat map1, map2;

cv::Mat newCameraMatrix;
std::vector< cv::Mat > map1_array;
std::vector< cv::Mat > map2_array;



//----------------------------------------------------------------
// Timer
//----------------------------------------------------------------

int TimerFlag;
int TimerInterval;

//----------------------------------------------------------------
// Video import
//----------------------------------------------------------------

vtkRenderer*   BackgroundRenderer;
vtkImageActor* BackgroundActor;
int CameraActiveFlag;

//----------------------------------------------------------------
// Recording
//----------------------------------------------------------------

#define NGRID_X  20
#define NGRID_Y  20
#define TIME_POINTS (30*30)

cv::Point2f * MotionFieldBuffer;
int           BufferIndex;
int           FileIndex;

vtkRenderWindow * renWin;



void initUndistort(int rows, int cols, cv::InputArray _cameraMatrix,
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

void fastUndistort( cv::InputArray _src, cv::OutputArray _dst,
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
    remap( src, dst_part, _map1_array[y], _map2_array[y], cv::INTER_LINEAR, cv::BORDER_CONSTANT );
    }
}





//----------------------------------------------------------------------------
int ViewerBackgroundOn(vtkRenderWindow* rwindow, vtkImageData* imageData)
{
  if (rwindow)
    {
    BackgroundRenderer = vtkRenderer::New();
    BackgroundActor = vtkImageActor::New();
    BackgroundActor->SetInput(imageData);
    BackgroundRenderer->AddActor(BackgroundActor);
    BackgroundRenderer->InteractiveOff();
    BackgroundRenderer->SetLayer(0);
    BackgroundActor->Modified();
    rwindow->AddRenderer(BackgroundRenderer);
    
    // Adjust camera position so that image covers the draw area.
    double origin[3];
    double spacing[3];
    int extent[6];
    VideoImageData->GetOrigin( origin );
    VideoImageData->GetSpacing( spacing );
    VideoImageData->GetExtent( extent );

    vtkCamera* camera = BackgroundRenderer->GetActiveCamera();
    camera->ParallelProjectionOn();
    double xc = origin[0] + 0.5*(extent[0] + extent[1])*spacing[0];
    double yc = origin[1] + 0.5*(extent[2] + extent[3])*spacing[1];
    double yd = (extent[3] - extent[2] + 1)*spacing[1];
    double d = camera->GetDistance();
    camera->SetParallelScale(0.5*yd);
    camera->SetFocalPoint(xc,yc,0.0);
    camera->SetPosition(xc,yc,d);
 
    // Render again to set the correct view
    rwindow->Render();

    return 1;
    }
  return 0;
}


//----------------------------------------------------------------------------
int ViewerBackgroundOff(vtkRenderWindow* rwindow)
{
  return 0;

}  

#define BUFFER_SIZE 300

int InitBuffer()
{
  BufferIndex = 0;
  MotionFieldBuffer = new cv::Point2f[NGRID_X*NGRID_Y*BUFFER_SIZE];
  FileIndex = 0;
  return (MotionFieldBuffer? 1: 0);
}


int StoreMotionField()
{
  std::stringstream ss;

  ss << "video_" << setfill ('0') << setw (3) << FileIndex << ".csv";

  std::ofstream of(ss.str().c_str());
  cv::Point2f * p = &MotionFieldBuffer[0];
  
  for (int i = 0; i < BUFFER_SIZE; i ++)
    {
    for (int i = 0; i < NGRID_X*NGRID_Y-1; i ++)
      {
      of << p->x << ", " << p->y << ", ";
      p ++;
      }
    of << p->x << ", " << p->y << std::endl;
    }

  FileIndex ++;

  of.close();
}


//----------------------------------------------------------------------------
int ProcessMotion(std::vector<cv::Point2f>& vector)
{
  
  if (MotionFieldBuffer == NULL)
    {
    return 0;
    }

  if (BufferIndex >= NGRID_X*NGRID_Y*BUFFER_SIZE)
    {
    StoreMotionField();
    BufferIndex = 0;
    }

  cv::Point2f * p = &MotionFieldBuffer[BufferIndex];
  
  std::vector<cv::Point2f>::iterator iter;
  for (iter = vector.begin(); iter != vector.end(); iter ++)
    {
    memcpy(p, &(*iter), sizeof(cv::Point2f));
    p ++;
    }

  BufferIndex += NGRID_X*NGRID_Y;

  return 1;
}

//----------------------------------------------------------------------------
// Camera thread / Originally created by A. Yamada
int CameraHandler()
{
  //IplImage* captureImageTmp = NULL;
  cv::Mat captureImageTmp;
  //CvSize   newImageSize;
  cv::Size newImageSize;

  if (capture.isOpened())
    {
    capture >> captureImageTmp;
    if (captureImageTmp.empty())
      {
      fprintf(stdout, "\n\nCouldn't take a picture\n\n");
      return 0;
      }

    // 5/6/2010 ayamada creating RGB image and capture image
    newImageSize = captureImageTmp.size();

    // check if the image size is changed
    if (newImageSize.width != imageSize.width||
        newImageSize.height != imageSize.height)
      {
      imageSize.width  = newImageSize.width;
      imageSize.height = newImageSize.height;

      // The following code may not be necessary
      VideoImageData->SetDimensions(newImageSize.width, newImageSize.height, 1);
      VideoImageData->SetExtent(0, newImageSize.width-1, 0, newImageSize.height-1, 0, 0 );
      VideoImageData->SetNumberOfScalarComponents(3);
      VideoImageData->SetScalarTypeToUnsignedChar();
      VideoImageData->AllocateScalars();
      VideoImageData->Update();

      //vtkSlicerViewerWidget* vwidget = GetApplicationGUI()->GetNthViewerWidget(0);
      BackgroundActor->SetInput(VideoImageData);
      //ViewerBackgroundOff(renWin);
      //ViewerBackgroundOn(renWin, VideoImageData);

      double gridSpaceX = (double)newImageSize.width  / (double)(NGRID_X+1);
      double gridSpaceY = (double)newImageSize.height / (double)(NGRID_Y+1);

      GridPoints[0].resize(NGRID_X*NGRID_Y);
      GridPoints[1].resize(NGRID_X*NGRID_Y);
      RVector.resize(NGRID_X*NGRID_Y);

      for (int i = 0; i < NGRID_X; i ++)
        {
        for (int j = 0; j < NGRID_Y; j ++)
          {
          GridPoints[0][i+j*NGRID_X].x = gridSpaceX*i + gridSpaceX;
          GridPoints[0][i+j*NGRID_Y].y = gridSpaceY*j + gridSpaceY;
          }
        }

      OpticalFlowStatus = (char*)cvAlloc(NGRID_X*NGRID_Y);
      }
    
    // create rgb image
    //cvFlip(captureImageTmp, captureImage, 0);
    //captureImageTmp.copyTo(captureImage);
    cv::flip(captureImageTmp, captureImage, 0);

    if (fUseCameraMatrix)
      {
      //cv::undistort( captureImage, undistortionImage, cameraMatrix, distCoeffs);
      fastUndistort( captureImage, undistortionImage, map1_array, map2_array,
                     calibratedImageSize.height, calibratedImageSize.width );

      cv::cvtColor( undistortionImage, RGBImage, CV_BGR2RGB);
      cv::cvtColor( undistortionImage, GrayImage, CV_BGR2GRAY); 
      }
    else
      {
      cv::cvtColor( captureImage, RGBImage, CV_BGR2RGB);
      cv::cvtColor( captureImage, GrayImage, CV_BGR2GRAY); 
      }
      
    if (OpticalFlowTrackingFlag)
      {
      int win_size = 10;
      int count = NGRID_X*NGRID_Y;
      vector<uchar> status;
      vector<float> err;
      cv::Size subPixWinSize(10,10);
      cv::Size winSize(31,31);

      if (PrevGrayImage.empty())
        {
        GrayImage.copyTo(PrevGrayImage);
        }

      cv::calcOpticalFlowPyrLK( PrevGrayImage, GrayImage, GridPoints[0], GridPoints[1],
                                status, err, winSize, 3, cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03));

      double dx = 0.0;
      double dy = 0.0;

      for(int i =  0; i < GridPoints[1].size(); i++ )
        {
        if( !status[i] )
          {
          GridPoints[1][i].x = GridPoints[0][i].x;
          GridPoints[1][i].y = GridPoints[0][i].y;
          }
        dx = GridPoints[1][i].x - GridPoints[0][i].x;
        dy = GridPoints[1][i].y - GridPoints[0][i].y;
        
        if (sqrt(dx*dx + dy*dy) > 50.0)
          {
          GridPoints[1][i].x = GridPoints[0][i].x;
          GridPoints[1][i].y = GridPoints[0][i].y;
          dx = 0.0;
          dy = 0.0;
          }
        
        RVector[i].x = dx;
        RVector[i].y = dy;
        cv::circle(RGBImage, GridPoints[0][i], 3, CV_RGB(0,255,255), -1, 8,0);
        cv::line(RGBImage, GridPoints[0][i], GridPoints[1][i], CV_RGB(0,255,0), 2);
        }
      
      cv::swap(PrevGrayImage, GrayImage);
      std::swap(Points[1], Points[0]);

      ProcessMotion(RVector);
      }
    
    unsigned char* idata;    
    // 5/6/2010 ayamada ok for videoOverlay
    idata = (unsigned char*) RGBImage.ptr();
    
    int dsize = imageSize.width*imageSize.height*3;
    memcpy((void*)VideoImageData->GetScalarPointer(), (void*)idata, dsize);

    if (VideoImageData && BackgroundRenderer)
      {
      VideoImageData->Modified();
      BackgroundRenderer->GetRenderWindow()->Render();
      }

    }
  
  return 1;
  
}


//void Camera(vtkCamera *NaviCamera, double *Matrix, double FOV, int xx )
//{
//  this->focal_length = (cvmGet(this->intrinsicMatrix[xx], 0, 0) + cvmGet(this->intrinsicMatrix[xx], 1, 1)) / 2.0;
//
//   double F = focal_length;
//   double * m = Matrix;
//   double pos[3] = {m[3], m[7], m[11]};
//
//   this->focal_point_x = (VIEW_SIZE_X / 2.0) - cvmGet(this->intrinsicMatrix[xx], 0, 2);
//   this->focal_point_y = (VIEW_SIZE_Y / 2.0) - cvmGet(this->intrinsicMatrix[xx], 1, 2);
//
//   double focal[3] = {    pos[0] + m[0] * focal_point_x + m[1] * focal_point_y + m[2] * (F),
//       pos[1] + m[4] * focal_point_x + m[5] * focal_point_y + m[6] * (F),
//       pos[2] + m[8] * focal_point_x + m[9] * focal_point_y + m[10] * (F)    };
//
//   double viewup[3] = {-m[1], -m[5], -m[9]};
//
//   NaviCamera->SetPosition(pos);
//   NaviCamera->SetFocalPoint(focal);
//   NaviCamera->SetViewUp(viewup);
//   NaviCamera->SetClippingRange(0, 5 * F);
//   NaviCamera->SetViewAngle(FOV);
//}


class vtkTimerCallback : public vtkCommand
{
public:
  static vtkTimerCallback *New()
  {
    vtkTimerCallback *cb = new vtkTimerCallback;
    return cb;
  }
  
  virtual void Execute(vtkObject *vtkNotUsed(caller), unsigned long eventId,
                       void *vtkNotUsed(callData))
  {
    if (vtkCommand::TimerEvent == eventId && TimerFlag)
      {
      if (CameraActiveFlag)
        {
        CameraHandler();
        }
      }
    //cout << this->TimerCount << endl;
  }
};


int printHelp(const char* name)
{
  std::cerr << "Usage: " << name << " [options]" << std::endl;
  std::cerr << "  Options (input)" << std::endl;
  std::cerr << "    -c <file>  : Camera calibration file" << std::endl;
  std::cerr << "    -v <file>  : Video file" << std::endl;
  std::cerr << "    -C <file>  : Camera channel" << std::endl;
  std::cerr << std::endl;
  std::cerr << "  Options (output)" << std::endl;
  std::cerr << "    -o <file>  : Flow data file" << std::endl;
  std::cerr << std::endl;
  std::cerr << "  Options (others)" << std::endl;
  std::cerr << "    -h         : Show help" << std::endl;

}

int main(int argc, char * argv[])
{
  
  const char * calibrationFile = NULL;
  const char * flowDataFile = NULL;
  const char * videoFile = NULL;
  int channel = 0;

  fUseCameraMatrix = 0;

  //--------------------------------------------------
  // Parse arguments
  for (int i = 1; i < argc; i ++)
    {
    if (strcmp(argv[i], "-c") == 0 && i+1 < argc)
      {
      // Camera calibration file name
      i ++;
      calibrationFile = argv[i];
      }
    else if (strcmp(argv[i], "-o") == 0 && i+1 < argc)
      {
      // Flow data file
      i ++;
      flowDataFile = argv[i];
      }
    else if (strcmp(argv[i], "-v") == 0 && i+1 < argc)
      {
      // Video data
      i ++;
      videoFile = argv[i];
      }
    else if (strcmp(argv[i], "-C") == 0 && i+1 < argc)
      {
      // Channel
      i ++;
      channel = atoi(argv[i]);
      }
    else if (strcmp(argv[i], "-h") == 0)
      {
      printHelp(argv[0]);
      exit(0);
      }
    }

  //--------------------------------------------------
  // Set up OpenCV
  if (calibrationFile)
    {
    cv::FileStorage fs(calibrationFile, cv::FileStorage::READ);
    fs["image_width"] >> calibratedImageSize.width;
    fs["image_height"] >> calibratedImageSize.height;
    fs["distortion_coefficients"] >> distCoeffs;
    fs["camera_matrix"] >> cameraMatrix;
    
    if( distCoeffs.type() != CV_64F )
      distCoeffs = cv::Mat_<double>(distCoeffs);
    if( cameraMatrix.type() != CV_64F )
      cameraMatrix = cv::Mat_<double>(cameraMatrix);

    //print out the result
    std::cerr << "Imported camera calibration information:" << std::endl;
    std::cerr << "Image size: " << calibratedImageSize.width << ", " << calibratedImageSize.height << std::endl;
    std::cerr << "Camera matrix: " << std::endl;
    std::cerr << cameraMatrix << std::endl;
    std::cerr << "Distance coefficients: " << std::endl;
    std::cerr << distCoeffs << std::endl;

    // Create undistort rectify map
    initUndistort(calibratedImageSize.height, calibratedImageSize.width, 
                  cameraMatrix, distCoeffs, map1_array, map2_array);

    fUseCameraMatrix = 1;
    }

  if (videoFile)
    {
    capture.open( videoFile );
    }
  else
    {
    capture.open( channel );
    }


  if (argc > 1) // video file is specified
    {
    const char * path = argv[1];
    //capture = cvCaptureFromAVI( path );

    }
  else if (argc == 1)
    {
    int channel = 0;
    //capture = cvCaptureFromCAM(channel);
    capture.open(channel);
    }
  else 
    {
    return 0;
    }

  if( !capture.isOpened() )
    {
      cout << "Could not initialize capturing...\n";
      return 0;
    }
  
  CameraActiveFlag = 1;

  cv::Size   newImageSize;

  cv::Mat frame;
  capture >> frame;
  if( frame.empty() )
    {
    fprintf(stdout, "\n\nCouldn't take a picture\n\n");
    return 0;
    }

  newImageSize = frame.size();

  VideoImageData = vtkImageData::New();
  VideoImageData->SetDimensions(newImageSize.width, newImageSize.height, 1);
  VideoImageData->SetExtent(0, newImageSize.width-1, 0, newImageSize.height-1, 0, 0 );
  VideoImageData->SetSpacing(1.0, 1.0, 1.0);
  VideoImageData->SetOrigin(0.0, 0.0, 0.0);
  VideoImageData->SetNumberOfScalarComponents(3);
  VideoImageData->SetScalarTypeToUnsignedChar();
  VideoImageData->AllocateScalars();
  VideoImageData->Update();

  //--------------------------------------------------
  // Set up motion vector field recorder
  InitBuffer();
  
  //--------------------------------------------------
  // Set up VTK
  int i;
  static float x[8][3]={{0,0,0}, {1,0,0}, {1,1,0}, {0,1,0},
                        {0,0,1}, {1,0,1}, {1,1,1}, {0,1,1}};
  static vtkIdType pts[6][4]={{0,1,2,3}, {4,5,6,7}, {0,1,5,4},
                        {1,2,6,5}, {2,3,7,6}, {3,0,4,7}};
  
  // We'll create the building blocks of polydata including data attributes.
  vtkPolyData *cube = vtkPolyData::New();
  vtkPoints *points = vtkPoints::New();
  vtkCellArray *polys = vtkCellArray::New();
  vtkFloatArray *scalars = vtkFloatArray::New();

  // Load the point, cell, and data attributes.
  for (i=0; i<8; i++) points->InsertPoint(i,x[i]);
  for (i=0; i<6; i++) polys->InsertNextCell(4,pts[i]);
  for (i=0; i<8; i++) scalars->InsertTuple1(i,i);

  // We now assign the pieces to the vtkPolyData.
  cube->SetPoints(points);
  points->Delete();
  cube->SetPolys(polys);
  polys->Delete();
  cube->GetCellData()->SetScalars(scalars);
  scalars->Delete();

  // Now we'll look at it.
  vtkPolyDataMapper *cubeMapper = vtkPolyDataMapper::New();
      cubeMapper->SetInput(cube);
      cubeMapper->SetScalarRange(0,7);
  vtkActor *cubeActor = vtkActor::New();
      cubeActor->SetMapper(cubeMapper);

  // The usual rendering stuff.
  vtkCamera *camera = vtkCamera::New();
  camera->SetPosition(1,1,1);
  camera->SetFocalPoint(0,0,0);
  camera->SetClippingRange(0.04, 0.1);

  vtkRenderer *renderer = vtkRenderer::New();
  renderer->AddActor(cubeActor);
  renderer->SetActiveCamera(camera);
  renderer->ResetCamera();
  renderer->SetBackground(1,1,1);
  
  renWin = vtkRenderWindow::New();
  renWin->SetSize(newImageSize.width, newImageSize.height);

  renWin->SetNumberOfLayers(2);
  ViewerBackgroundOn(renWin, VideoImageData);

  renderer->SetLayer(1);
  renWin->AddRenderer(renderer);

  vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();
  iren->Initialize();
  iren->SetRenderWindow(renWin);

  // interact with data
  renWin->Render();

  TimerFlag = 1;

  // Sign up to receive TimerEvent
  //vtkSmartPointer<vtkTimerCallback> cb = 
  //  vtkSmartPointer<vtkTimerCallback>::New();
  vtkTimerCallback* cb = vtkTimerCallback::New();
  iren->AddObserver(vtkCommand::TimerEvent, cb);

  OpticalFlowTrackingFlag = 1;
  int timerId = iren->CreateRepeatingTimer(100);
  std::cout << "timerId: " << timerId << std::endl;  

  iren->Start();

  // Stop timer
  TimerFlag = 0;

  //--------------------------------------------------
  // Clean up OpenCV
  CameraActiveFlag = 0;

  //cvReleaseCapture(&capture);
  //vtkSlicerViewerWidget* vwidget = GetApplicationGUI()->GetNthViewerWidget(0);
  ViewerBackgroundOff(renWin);


  //--------------------------------------------------
  // Clean up VTK
  cube->Delete();
  cubeMapper->Delete();
  cubeActor->Delete();
  camera->Delete();
  renderer->Delete();
  renWin->Delete();
  iren->Delete();

  cb->Delete();
  return 0;
}
