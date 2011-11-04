/*=========================================================================

  Program:   Visualization Toolkit
  Module:    Cube.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// This example shows how to manually create vtkPolyData.


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
#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace cv;
using namespace std;


//vtkRenderer*   BackgroundRenderer;
//vtkImageActor* BackgroundActor;
//int CameraActiveFlag;

//----------------------------------------------------------------
// Video import
//----------------------------------------------------------------
CvCapture* capture;

CvSize        imageSize;
IplImage*     captureImage;
IplImage*     RGBImage;
IplImage*     undistortionImage;      //adding at 09. 12. 15 - smkim

vtkImageData* VideoImageData;

// Optical tracking
int           OpticalFlowTrackingFlag;
IplImage*     GrayImage;
IplImage*     PrevGrayImage;
IplImage*     Pyramid;
IplImage*     PrevPyramid;
IplImage*     SwapTempImage;
int           PyrFlag;

CvPoint2D32f* Points[2];
CvPoint2D32f* SwapPoints;
CvPoint2D32f* GridPoints[2];// = {0,0};
CvPoint2D32f* RVector;
char*         OpticalFlowStatus;

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

vtkRenderWindow * renWin;

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
    //double xd = (extent[1] - extent[0] + 1)*spacing[0];
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
  //if (rwindow)
  //  {
  //  BackgroundRenderer = NULL;
  //  BackgroundActor = NULL;
  //  }

  return 0;

}  

#define NGRID_X  20
#define NGRID_Y  20
#define TIME_POINTS (30*30)

//----------------------------------------------------------------------------
int ProcessMotion(CvPoint2D32f* vector, CvPoint2D32f* position, int n)
{
  float threshold = 5.0;
  CvPoint2D32f mean;

  mean.x = 0.0;
  mean.y = 0.0;

  // Use 10% vectors to calculate translation
  for (int i = 0; i < n; i ++)
    {
    float x = vector[i].x;
    float y = vector[i].y;
    float len = sqrtf(x*x + y*y);
    if (len > threshold)
      {
      mean.x += x;
      mean.y += y;
      }
    }
  mean.x /= (float)n;
  mean.y /= (float)n;

  return 1;
}

//----------------------------------------------------------------------------
// Camera thread / Originally created by A. Yamada
int CameraHandler()
{
  IplImage* captureImageTmp = NULL;
  CvSize   newImageSize;

  if (capture)
    {
    // 5/15/2010 ayamada
    if(NULL == (captureImageTmp = cvQueryFrame( capture )))
      {
      fprintf(stdout, "\n\nCouldn't take a picture\n\n");
      return 0;
      }

    // 5/6/2010 ayamada creating RGB image and capture image
    newImageSize = cvGetSize( captureImageTmp );

    std::cerr << "Size =  " << newImageSize.width << ", " << newImageSize.height << std::endl;

    // check if the image size is changed
    if (newImageSize.width != imageSize.width ||
        newImageSize.height != imageSize.height)
      {
      imageSize.width = newImageSize.width;
      imageSize.height = newImageSize.height;
      captureImage = cvCreateImage(imageSize, IPL_DEPTH_8U,3);
      RGBImage = cvCreateImage(imageSize, IPL_DEPTH_8U, 3);
      undistortionImage = cvCreateImage( imageSize, IPL_DEPTH_8U, 3);

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

      // for optical flow
      Pyramid       = cvCreateImage( imageSize , IPL_DEPTH_8U, 1 );
      PrevPyramid   = cvCreateImage( imageSize , IPL_DEPTH_8U, 1 );
      GrayImage     = cvCreateImage( imageSize , IPL_DEPTH_8U, 1 );
      PrevGrayImage = cvCreateImage( imageSize , IPL_DEPTH_8U, 1 );
      SwapTempImage = cvCreateImage( imageSize , IPL_DEPTH_8U, 1 );

      double gridSpaceX = (double)newImageSize.width / (double)(NGRID_X+1);
      double gridSpaceY = (double)newImageSize.height / (double)(NGRID_Y+1);

      Points[0] = 0;
      Points[1] = 0;
      GridPoints[0] = (CvPoint2D32f*)cvAlloc(NGRID_X*NGRID_Y*sizeof(Points[0][0]));
      GridPoints[1] = (CvPoint2D32f*)cvAlloc(NGRID_X*NGRID_Y*sizeof(Points[0][0]));
      RVector = (CvPoint2D32f*)cvAlloc(NGRID_X*NGRID_Y*sizeof(Points[0][0]));

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
    // 5/6/2010 ayamada for videoOverlay
    cvFlip(captureImageTmp, captureImage, 0);
    
    //cvUndistort2( captureImage, undistortionImage, pGUI->intrinsicMatrix, pGUI->distortionCoefficient );            
    //cvCvtColor( undistortionImage, RGBImage, CV_BGR2RGB);       //comment not to undistort      at 10. 01. 07 - smkim
    cvCvtColor( captureImage, RGBImage, CV_BGR2RGB);       //comment not to undistort      at 10. 01. 07 - smkim
    
    if (OpticalFlowTrackingFlag)
      {
      int win_size = 10;
      int count = NGRID_X*NGRID_Y;
      cvCvtColor(RGBImage, GrayImage, CV_RGB2GRAY );
      cvCalcOpticalFlowPyrLK( PrevGrayImage, GrayImage, PrevPyramid, Pyramid,
                              GridPoints[0], GridPoints[1], count, cvSize(win_size,win_size), 3, OpticalFlowStatus, 0,
                              cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03), PyrFlag );
      PyrFlag |= CV_LKFLOW_PYR_A_READY;
      
      double dx = 0.0;
      double dy = 0.0;
      //int frameCount = 0;

      for(int i =  0; i < count; i++ )
        {
        if( !OpticalFlowStatus[i] )
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
        cvCircle(RGBImage, cvPointFrom32f(GridPoints[0][i]), 3, CV_RGB(0,255,255), -1, 8,0);
        cvLine(RGBImage, cvPointFrom32f(GridPoints[0][i]), cvPointFrom32f(GridPoints[1][i]), CV_RGB(0,255,0), 2);
        }
      
      CV_SWAP( PrevGrayImage, GrayImage, SwapTempImage );
      CV_SWAP( PrevPyramid, Pyramid, SwapTempImage );
      CV_SWAP( Points[0], Points[1], SwapPoints );

      ProcessMotion(RVector, GridPoints[0], count);
      }
    
    unsigned char* idata;    
    // 5/6/2010 ayamada ok for videoOverlay
    idata = (unsigned char*) RGBImage->imageData;
    
    int dsize = imageSize.width*imageSize.height*3;
    memcpy((void*)VideoImageData->GetScalarPointer(), (void*)RGBImage->imageData, dsize);

    if (VideoImageData && BackgroundRenderer)
      {
      VideoImageData->Modified();
      BackgroundRenderer->GetRenderWindow()->Render();
      std::cerr << "rendering " << std::endl;
      }

    }
  
  return 1;
  
}

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
        cout << "calling CameraHandler()" << endl;
        }
      }
    //cout << this->TimerCount << endl;
  }
};
 

int main(int argc, char * argv[])
{

  //TermCriteria termcrit(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03);
  //Size subPixWinSize(10,10), winSize(31,31);

  //--------------------------------------------------
  // Set up OpenCV

  capture      = NULL;
  captureImage = NULL;
  RGBImage     = NULL;
  undistortionImage = NULL;
  imageSize.width = 100;
  imageSize.height = 100;
  
  if (argc > 1) // video file is specified
    {
    const char * path = argv[1];
    capture = cvCaptureFromAVI( path );
    }
  else if (argc == 1)
    {
    int channel = 1;
    capture = cvCaptureFromCAM(channel);
    }
  else 
    {
    return 0;
    }

  if (capture == NULL)
    {
    return 0;
    }

  CameraActiveFlag = 1;

  IplImage* captureImageTmp = NULL;
  CvSize   newImageSize;

  if(NULL == (captureImageTmp = cvQueryFrame( capture )))
    {
    fprintf(stdout, "\n\nCouldn't take a picture\n\n");
    return 0;
    }
  newImageSize = cvGetSize( captureImageTmp );

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
  cvReleaseCapture(&capture);
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
