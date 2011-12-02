/*=========================================================================

  Program:        OpenCV support for Visualization Toolkit
  Module:         vtkOpenCVRendererDelegate.h
  Contributor(s): Junichi Tokuda (tokuda@bwh.harvard.edu)

  Copyright (c) Brigham and Women's Hospital, All rights reserved.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include <vector>
#include <iostream>

// VTK header files and prototypes
#include "vtkOpenCVOpticalFlowCallback.h"
#include "vtkOpenCVRendererDelegate.h"

// OpenCV stuff
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/types_c.h"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

//----------------------------------------------------------------------------
// We cannot use vtkStandardNewMacro() since vtkCommand is not derived from
// vtkObject.
//vtkStandardNewMacro(vtkOpenCVOpticalFlowCallback);


//----------------------------------------------------------------------------
vtkOpenCVOpticalFlowCallback::vtkOpenCVOpticalFlowCallback()
{
  InitBuffer();
}


//----------------------------------------------------------------------------
vtkOpenCVOpticalFlowCallback::~vtkOpenCVOpticalFlowCallback()
{
}


//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
int vtkOpenCVOpticalFlowCallback::InitBuffer()
{
  BufferIndex = 0;
  MotionFieldBuffer = new cv::Point2f[NGRID_X*NGRID_Y*BUFFER_SIZE];
  FileIndex = 0;
  return (MotionFieldBuffer? 1: 0);
}

  
//----------------------------------------------------------------------------
int vtkOpenCVOpticalFlowCallback::StoreMotionField()
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
int vtkOpenCVOpticalFlowCallback::ProcessMotion(std::vector<cv::Point2f>& vector)
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
void vtkOpenCVOpticalFlowCallback::Execute(vtkObject * caller, unsigned long eventId, void *callData)
{

  cv::Mat * RGBImage = reinterpret_cast< cv::Mat* > (callData);
  vtkOpenCVRendererDelegate * CVRendererDelegate 
    = vtkOpenCVRendererDelegate::SafeDownCast(caller);

  if (eventId == vtkOpenCVRendererDelegate::OpenCVProcessImageEvent && RGBImage)
    {

    unsigned int width, height;
    CVRendererDelegate->GetImageSize(width, height);

    double gridSpaceX = (double)width  / (double)(NGRID_X+1);
    double gridSpaceY = (double)height / (double)(NGRID_Y+1);
    
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
    cv::cvtColor( *RGBImage, this->GrayImage, CV_RGB2GRAY); 
    
    int win_size = 10;
    int count = NGRID_X*NGRID_Y;
    std::vector<uchar> status;
    std::vector<float> err;
    cv::Size subPixWinSize(10,10);
    cv::Size winSize(31,31);
    
    if (PrevGrayImage.empty())
      {
      GrayImage.copyTo(PrevGrayImage);
      }
    
    cv::calcOpticalFlowPyrLK( PrevGrayImage, GrayImage, GridPoints[0], GridPoints[1],
                              status, err, winSize, 3,
                              cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03));
    
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

      cv::circle(*RGBImage, GridPoints[0][i], 3, CV_RGB(0,255,255), -1, 8,0);
      cv::line(*RGBImage, GridPoints[0][i], GridPoints[1][i], CV_RGB(0,255,0), 2);
      }
    
    cv::swap(PrevGrayImage, GrayImage);
    std::swap(Points[1], Points[0]);

    ProcessMotion(RVector);
    }

}


