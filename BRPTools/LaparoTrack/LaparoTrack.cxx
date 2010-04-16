/* Demo of modified Lucas-Kanade optical flow algorithm.
   See the printf below */

#ifdef _CH_
#pragma package <opencv>
#endif

#define CV_NO_BACKWARD_COMPATIBILITY

#ifndef _EiC
#include "cv.h"
#include "highgui.h"
#include <stdio.h>
#include <ctype.h>
#include <iostream>
#endif

#define NGRID_X  20
#define NGRID_Y  20

IplImage *image = 0, *grey = 0, *prev_grey = 0, *pyramid = 0, *prev_pyramid = 0, *swap_temp;

int win_size = 10;
const int MAX_COUNT = 500;
CvPoint2D32f* points[2] = {0,0}, *swap_points;
char* status = 0;
int count = 0;
int night_mode = 0;
int flags = 0;
CvPoint pt;

CvPoint2D32f* gridpoints[2] = {0,0};


void OnMouse( int event, int x, int y, int flags, void* param )
{
    if( !image )
        return;

    if( image->origin )
        y = image->height - y;

    if( event == CV_EVENT_LBUTTONDOWN )
    {
        pt = cvPoint(x,y);
    }
}


int main( int argc, char** argv )
{
  CvCapture* capture = 0;
  
  if( argc == 1 || (argc == 2 && strlen(argv[1]) == 1 && isdigit(argv[1][0])))
    capture = cvCaptureFromCAM( argc == 2 ? argv[1][0] - '0' : 0 );
  else if( argc == 2 )
    capture = cvCaptureFromAVI( argv[1] );
  
  if( !capture )
    {
    fprintf(stderr,"Could not initialize capturing...\n");
    return -1;
    }
  
  /* print a welcome message, and the OpenCV version */
  printf ("Welcome to lkdemo, using OpenCV version %s (%d.%d.%d)\n",
          CV_VERSION,
          CV_MAJOR_VERSION, CV_MINOR_VERSION, CV_SUBMINOR_VERSION);
  
  printf( "Hot keys: \n"
          "\tESC - quit the program\n"
          "\tr - auto-initialize tracking\n"
          "\tc - delete all the points\n"
          "\tn - switch the \"night\" mode on/off\n"
          "To add/remove a feature point click it\n" );
  
  cvNamedWindow( "LaparoTrack", 0 );
  cvSetMouseCallback( "LaparoTrack", OnMouse, 0 );
  
  for(;;)
    {
    IplImage* frame = 0;
    int i, k, c;
    
    frame = cvQueryFrame( capture );
    if( !frame )
      break;
    
    if( !image )
      {
      /* allocate all the buffers */
      image = cvCreateImage( cvGetSize(frame), 8, 3 );
      image->origin = frame->origin;
      grey = cvCreateImage( cvGetSize(frame), 8, 1 );
      prev_grey = cvCreateImage( cvGetSize(frame), 8, 1 );
      pyramid = cvCreateImage( cvGetSize(frame), 8, 1 );
      prev_pyramid = cvCreateImage( cvGetSize(frame), 8, 1 );

      status = (char*)cvAlloc(NGRID_X*NGRID_Y);
      flags = 0;

      count = NGRID_X*NGRID_Y;

      gridpoints[0] = (CvPoint2D32f*)cvAlloc(NGRID_X*NGRID_Y*sizeof(points[0][0]));
      gridpoints[1] = (CvPoint2D32f*)cvAlloc(NGRID_X*NGRID_Y*sizeof(points[0][0]));
      
      CvSize CanvasSize = cvGetSize(frame);

      double gridSpaceX = (double)CanvasSize.width / (double)(NGRID_X+1);
      double gridSpaceY = (double)CanvasSize.height / (double)(NGRID_Y+1);
      std::cerr << "gridSpaceX = " << gridSpaceX << std::endl;
      std::cerr << "gridSpaceY = " << gridSpaceY << std::endl;

      for (int i = 0; i < NGRID_X; i ++)
        {
        for (int j = 0; j < NGRID_Y; j ++)
          {
          gridpoints[0][i+j*NGRID_X].x = gridSpaceX*i + gridSpaceX;
          gridpoints[0][i+j*NGRID_Y].y = gridSpaceY*j + gridSpaceY;
          }
        }
      }
    
    cvCopy( frame, image, 0 );
    cvCvtColor( image, grey, CV_BGR2GRAY );
    
    if( night_mode )
      cvZero( image );
    
    if( count > 0 )
      {
      cvCalcOpticalFlowPyrLK( prev_grey, grey, prev_pyramid, pyramid,
                              gridpoints[0], gridpoints[1], count, cvSize(win_size,win_size), 3, status, 0,
                              cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03), flags );

      flags |= CV_LKFLOW_PYR_A_READY;
      for( i = k = 0; i < count; i++ )
        {
        
        if( !status[i] )
          {
          gridpoints[1][i].x = gridpoints[0][i].x;
          gridpoints[1][i].y = gridpoints[0][i].y;
          }
        else
          {
          double dx = gridpoints[0][i].x - gridpoints[1][i].x;
          double dy = gridpoints[0][i].y - gridpoints[1][i].y;

          if (sqrt(dx*dx + dy*dy) > 50.0)
            {
            gridpoints[1][i].x = gridpoints[0][i].x;
            gridpoints[1][i].y = gridpoints[0][i].y;
            }
          }
        
        gridpoints[1][k++] = gridpoints[1][i];
        cvCircle( image, cvPointFrom32f(gridpoints[0][i]), 3, CV_RGB(0,255,255), -1, 8,0);
        //cvCircle( image, cvPointFrom32f(gridpoints[1][i]), 3, CV_RGB(0,255,0), -1, 8,0);
        cvLine(image, cvPointFrom32f(gridpoints[0][i]), cvPointFrom32f(gridpoints[1][i]), CV_RGB(0,255,0), 2);
        }
      }
    
    CV_SWAP( prev_grey, grey, swap_temp );
    CV_SWAP( prev_pyramid, pyramid, swap_temp );
    CV_SWAP( points[0], points[1], swap_points );
    cvShowImage( "LkDemo", image );
    
    c = cvWaitKey(10);
    if( (char)c == 27 )
      break;
    switch( (char) c )
      {
      case 'r':
        //need_to_init = 1;
        break;
      case 'c':
        count = 0;
        break;
      case 'n':
        night_mode ^= 1;
        break;
      default:
        ;
      }
    }
  
  cvReleaseCapture( &capture );
  cvDestroyWindow("LkDemo");
  
  return 0;
}



#if 0
// Number of grids for vector field
#define PI 3.1415926535

#define GRID_GAUSS_SIGMA
#define GRID_WIDTH_X 
#define GRID_WIDTH_Y

// Calculate vector field (2D gaussian interpolation)
GenerateOpticalFlowField(CvPoint2D32f* point0, CvPoint2D32f* points1, int count,
                         const CvSize* gridSize, const CvSize* winSize,
                         CvPoint2D32f* flowField)
{

  int nx = winSize.x / gridSize.x + 1;
  int ny = winSize.y / gridSize.y + 1;
  double sigmax = gridSize.x / 3.0;
  double sigmay = gridSize.y / 3.0;
  double* w = new double[nx*ny];

  for (int i = 0; i < nx*ny; i ++)
    {
    flow
    }

  for (int i = 0; i < count ; i ++)
    {
    int ix = (int) point0[i].x / gridSize.x;
    int iy = (int) point0[i].y / gridSize.y;
    double vx = point1[i].x - point0[i].x;
    double vy = point1[i].y - point0[i].y;
    
    // find the nearest grid point
    if (point0[i].x % gridSize.x > (gridSize.x / 2))
      {
      ix += 1;
      }
    if (point0[i].y % gridSize.y > (gridSize.y / 2))
      {
      iy += 1;
      }

    // make sure that the indeces are in the range
    if (ix < 0) ix = 0;
    if (ix > nx) ix = nx - 1;
    if (iy < 0) iy = 0;
    if (iy > ny) iy = n - 1;
    }
}
#endif


#ifdef _EiC
main(1,"lkdemo.c");
#endif



