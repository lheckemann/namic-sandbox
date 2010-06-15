#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <malloc.h>
#include <math.h>

#include <igtlOSUtil.h>
#include <igtlTransformMessage.h>
#include <igtlImageMessage.h>
#include <igtlClientSocket.h>

#include <opencv/cv.h>
#include <opencv/highgui.h>

int main()
{
  CvCapture* capture = 0;
  IplImage * frame = 0;

  char key = NULL;

  // Automatic detection of the webcam device
  capture = cvCreateCameraCapture(-1); 

  if(capture)
  {
    std::cout << "Webcam Stream Opened." << std::endl; 
  }
  else
  {
    std::cout << "Failed to open Webcam Stream." << std::endl;
    return 1;
  }

  cvNamedWindow("Webcam",CV_WINDOW_AUTOSIZE);

  while(key != 'q')
  {
    frame = cvQueryFrame(capture);
    cvShowImage("Stream from Webcam",frame);
    key = cvWaitKey(10);
  } 


  std::cout << "Leaving." << std::endl;


  // Remove all windows
  cvDestroyAllWindows();

  // Reset frame pointer (no need to destroy image)
  frame = NULL;

  // Release capture
  cvReleaseCapture(&capture);

  return 0;
}
