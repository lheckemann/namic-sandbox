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

#include "vtkOpenCVRendererDelegate.h"
#include "vtkOpenCVOpticalFlowCallback.h"

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

vtkOpenCVRendererDelegate * CVRendererDelegate;

//----------------------------------------------------------------
// Timer
//----------------------------------------------------------------

int TimerFlag;
int TimerInterval;

//----------------------------------------------------------------
// Video import
//----------------------------------------------------------------

vtkRenderer*   BackgroundRenderer;
int CameraActiveFlag;


//----------------------------------------------------------------
// Timer Callback
//----------------------------------------------------------------

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
        CVRendererDelegate->Capture();
        if (BackgroundRenderer)
          {
          BackgroundRenderer->GetRenderWindow()->Render();
          }
        }
      }
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
  
  cv::VideoCapture capture;

  const char * calibrationFile = NULL;
  const char * flowDataFile = NULL;
  const char * videoFile = NULL;
  int channel = 0;

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
  if (videoFile)
    {
    capture.open( videoFile );
    }
  else
    {
    capture.open( channel );
    }

  if( !capture.isOpened() )
    {
    std::cerr << "Could not initialize capturing...\n" << std::endl;
    exit(0);
    }
  
  CVRendererDelegate = vtkOpenCVRendererDelegate::New();
  if (!CVRendererDelegate->SetVideoCapture(&capture))
    {
    std::cerr << "Couldn't set video capture...\n" << std::endl;
    exit(0);
    }

  if (calibrationFile && !CVRendererDelegate->ImportCameraCalibrationFile(calibrationFile))
    {
    std::cerr << "Couldn't import the camera calibration file...\n" << std::endl;
    exit(0);
    }

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

  // Renderer for the foreground (VTK model)
  vtkRenderer *fgrenderer = vtkRenderer::New();
  fgrenderer->AddActor(cubeActor);
  fgrenderer->SetActiveCamera(camera);
  fgrenderer->ResetCamera();
  fgrenderer->SetBackground(1,1,1);

  vtkOpenCVOpticalFlowCallback * OpticalFlow = vtkOpenCVOpticalFlowCallback::New();
  CVRendererDelegate->SetOpenCVProcessImageCallback(OpticalFlow);

  // Renderer for the background (OpenCV image)
  //vtkRenderer *bgrenderer = vtkRenderer::New();
  BackgroundRenderer = vtkRenderer::New();
 
  BackgroundRenderer->SetDelegate(CVRendererDelegate);
  

  BackgroundRenderer->InteractiveOff();

  // Create render window
  vtkRenderWindow * renWin;
  renWin = vtkRenderWindow::New();
  unsigned int width;
  unsigned int height;
  CVRendererDelegate->GetImageSize(width, height);
  renWin->SetSize(width, height);

  renWin->SetNumberOfLayers(2);
  BackgroundRenderer->SetLayer(0);
  renWin->AddRenderer(BackgroundRenderer);
  fgrenderer->SetLayer(1);
  renWin->AddRenderer(fgrenderer);

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

  //OpticalFlowTrackingFlag = 1;
  int timerId = iren->CreateRepeatingTimer(100);
  std::cout << "timerId: " << timerId << std::endl;  

  CameraActiveFlag = 1;
  iren->Start();

  // Stop timer
  TimerFlag = 0;

  //--------------------------------------------------
  // Clean up OpenCV
  CameraActiveFlag = 0;

  //cvReleaseCapture(&capture);
  //vtkSlicerViewerWidget* vwidget = GetApplicationGUI()->GetNthViewerWidget(0);
  //ViewerBackgroundOff(renWin);

  //--------------------------------------------------
  // Clean up VTK
  cube->Delete();
  cubeMapper->Delete();
  cubeActor->Delete();
  camera->Delete();
  renWin->Delete();
  iren->Delete();
  cb->Delete();
  CVRendererDelegate->Delete();
  fgrenderer->Delete();
  BackgroundRenderer->Delete();
  
  return 0;
}
