#include "vtkStereoCalibCVClass.h"
#include <stdio.h>              
#include <vector>
 #include <iostream>
std::vector<CvPoint3D32f> objectPoints;
std::vector<int> npoints;
std::vector<CvPoint3D32f> lines[2];
std::vector<uchar> active[2];
std::vector<CvPoint2D32f> points[2]; 

vtkStandardNewMacro(vtkStereoCalibCVClass);                        // 
vtkCxxRevisionMacro(vtkStereoCalibCVClass, "$Revision: 1.0 $");    // 100620-komura


vtkStereoCalibCVClass::vtkStereoCalibCVClass()
{

    _M1 = cvMat(3, 3, CV_64F, M1 );
    _M2 = cvMat(3, 3, CV_64F, M2 );
    _D1 = cvMat(1, 5, CV_64F, D1 );
    _D1_mono = cvMat(1, 4, CV_64F, D1_mono );
    _D2 = cvMat(1, 5, CV_64F, D2 );
    _R = cvMat(3, 3, CV_64F, R );
    _T = cvMat(3, 1, CV_64F, T );
    _E = cvMat(3, 3, CV_64F, E );
    _F = cvMat(3, 3, CV_64F, F );
    n = CORNER_NUMBER;
}
vtkStereoCalibCVClass::~vtkStereoCalibCVClass()
{
    if(mx1 != NULL){
        cvReleaseMat( &mx1 );
    }
    if(my1 != NULL){
        cvReleaseMat( &my1 );
    }
    if(mx2 != NULL){
        cvReleaseMat( &mx2 );
    }
    if(my2 != NULL){
        cvReleaseMat( &my2 );
    }
}

void vtkStereoCalibCVClass::chessLoad(IplImage* frame, int lr){//100603-komura
    int i, j;
    std::vector<CvPoint2D32f> temp(n);
    int count = 0, result=0;
    std::vector<CvPoint2D32f>& pts = points[lr];
    
    IplImage* img = cvCreateImage(cvSize(frame->width,frame->height), IPL_DEPTH_8U, 1); 
    cvCvtColor(frame, img, CV_BGR2GRAY);

    result = cvFindChessboardCorners( img, cvSize(CORNER_WIDTH, CORNER_HEIGHT ),
                                      &temp[0], &count,
                                      CV_CALIB_CB_ADAPTIVE_THRESH |
                                      CV_CALIB_CB_NORMALIZE_IMAGE
                                      );
    N = pts.size();
    pts.resize(N + n, cvPoint2D32f(0,0));
    active[lr].push_back((uchar)result);
    //assert( result != 0 );
    if( result ){
        //Calibration will suffer without subpixel interpolation
        cvFindCornerSubPix( img, &temp[0], count,
                            cvSize(11, 11), cvSize(-1,-1),
                            cvTermCriteria(CV_TERMCRIT_ITER+CV_TERMCRIT_EPS,
                                           30, 0.01) );
        copy( temp.begin(), temp.end(), pts.begin() + N );
    }
    // char test[255];
    // sprintf(test,"/home/masai/Desktop/test%d.bmp",lr);
    // cvSaveImage( test, img );
    cvReleaseImage( &img );
}


void vtkStereoCalibCVClass::stereoCalib(CvMat &_M12, CvMat &_M22, CvMat &_D12, CvMat &_D22)//100603-komura
{
    bool isVerticalStereo = false;//OpenCV can handle left-right
    //or up-down camera arrangements
    const float squareSize = 1.f; //Set this to your actual square size
    int i, j;
    std::vector<CvPoint3D32f> objectPoints;
    std::vector<int> npoints;
    // ARRAY AND VECTOR STORAGE:
    // HARVEST CHESSBOARD 3D OBJECT POINT LIST:
    

    nframes = active[0].size();//Number of good chessboads found
    objectPoints.resize(nframes*n);
    for( i = 0; i < CORNER_HEIGHT; i++ ){
        for( j = 0; j < CORNER_WIDTH; j++ ){
            objectPoints[i*CORNER_WIDTH + j] = cvPoint3D32f(i*squareSize, j*squareSize, 0);
        }
    }
    for( i = 1; i < nframes; i++ ){
        copy( objectPoints.begin(), objectPoints.begin() + n,
              objectPoints.begin() + i*n );
    }
    npoints.resize(nframes,n);
    N = nframes*n;
    _objectPoints = cvMat(1, N, CV_32FC3, &objectPoints[0] );
    _imagePoints1 = cvMat(1, N, CV_32FC2, &points[0][0] );
    _imagePoints2 = cvMat(1, N, CV_32FC2, &points[1][0] );
    _npoints = cvMat(1, npoints.size(), CV_32S, &npoints[0] );
    cvSetIdentity(&_M1);
    cvSetIdentity(&_M2);
    cvZero(&_D1);
    cvZero(&_D2);

// CALIBRATE THE STEREO CAMERAS
    std::cerr << "\n\nRunning stereo calibration ...\n\n" << std::endl;
    cvStereoCalibrate( &_objectPoints, &_imagePoints1,
         &_imagePoints2, &_npoints,
         &_M1, &_D1, &_M2, &_D2,
         imageSize, &_R, &_T, &_E, &_F,
         cvTermCriteria(CV_TERMCRIT_ITER+
          CV_TERMCRIT_EPS, 100, 1e-5),
         CV_CALIB_FIX_ASPECT_RATIO +
         CV_CALIB_ZERO_TANGENT_DIST +
         CV_CALIB_SAME_FOCAL_LENGTH );
    std::cerr << "\n\n stereo calibration finish \n\n" << std::endl;
    cvCopy(&_M1, &_M12);
    cvCopy(&_M2, &_M22);
    cvCopy(&_D1, &_D12);
    cvCopy(&_D2, &_D22);

    // 100621-komura
    objectPoints.clear();
    npoints.clear();
    for(int n=0;n<2;n++){
        lines[n].clear();
        active[n].clear();
        points[n].clear(); 
    }

    
}

void vtkStereoCalibCVClass::monoCalib(CvMat &_M12, CvMat &_D12)//100603-komura
{
    bool isVerticalStereo = false;//OpenCV can handle left-right
    //or up-down camera arrangements
    const float squareSize = 1.f; //Set this to your actual square size
    int i, j;
    std::vector<CvPoint3D32f> objectPoints;
    std::vector<int> npoints;
    // ARRAY AND VECTOR STORAGE:
    // HARVEST CHESSBOARD 3D OBJECT POINT LIST:
    

    nframes = active[0].size();//Number of good chessboads found
    objectPoints.resize(nframes*n);
    for( i = 0; i < CORNER_HEIGHT; i++ ){
        for( j = 0; j < CORNER_WIDTH; j++ ){
            objectPoints[i*CORNER_WIDTH + j] = cvPoint3D32f(i*squareSize, j*squareSize, 0);
        }
    }
    for( i = 1; i < nframes; i++ ){
        copy( objectPoints.begin(), objectPoints.begin() + n,
              objectPoints.begin() + i*n );
    }
    npoints.resize(nframes,n);
    N = nframes*n;
    _objectPoints = cvMat(1, N, CV_32FC3, &objectPoints[0] );
    _imagePoints1 = cvMat(1, N, CV_32FC2, &points[0][0] );
    _npoints = cvMat(1, npoints.size(), CV_32S, &npoints[0] );
    cvSetIdentity(&_M1);
    cvSetIdentity(&_M2);
    cvZero(&_D1);
    cvZero(&_D2);

// CALIBRATE THE STEREO CAMERAS
    std::cerr << "\n\nRunning mono calibration ...\n\n" << std::endl;

    cvCalibrateCamera2 (&_objectPoints, &_imagePoints1,
                        &_npoints, imageSize,
                        &_M1, &_D1_mono);

    std::cerr << "\n\n mono calibration finish \n\n" << std::endl;
    cvCopy(&_M1, &_M12);
    cvCopy(&_D1_mono, &_D12); 

    // 100621-komura
    objectPoints.clear();
    npoints.clear();
    for(int n=0;n<2;n++){
        lines[n].clear();
        active[n].clear();
        points[n].clear(); 
    } 
}
 

void vtkStereoCalibCVClass::displayStereoCalib(CvMat* _mx1_, CvMat* _mx2_, CvMat* _my1_, CvMat* _my2_){
    int  i;
    printf(" done\n");
    // CALIBRATION QUALITY CHECK
    // because the output fundamental matrix implicitly
    // includes all the output information,
    // we can check the quality of calibration using the
    // epipolar geometry constraint: m2^t*F*m1=0
    std::vector<CvPoint3D32f> lines[2];
    points[0].resize(N);
    points[1].resize(N);
    _imagePoints1 = cvMat(1, N, CV_32FC2, &points[0][0] );
    _imagePoints2 = cvMat(1, N, CV_32FC2, &points[1][0] );
    lines[0].resize(N);
    lines[1].resize(N);
    CvMat _L1 = cvMat(1, N, CV_32FC3, &lines[0][0]);
    CvMat _L2 = cvMat(1, N, CV_32FC3, &lines[1][0]);
//Always work in undistorted space
    cvUndistortPoints( &_imagePoints1, &_imagePoints1,
         &_M1, &_D1, 0, &_M1 );
    cvUndistortPoints( &_imagePoints2, &_imagePoints2,
         &_M2, &_D2, 0, &_M2 );
    cvComputeCorrespondEpilines( &_imagePoints1, 1, &_F, &_L1 );
    cvComputeCorrespondEpilines( &_imagePoints2, 2, &_F, &_L2 );
    double avgErr = 0;
    for( i = 0; i < N; i++ )
    {
        double err = fabs(points[0][i].x*lines[1][i].x +
     points[0][i].y*lines[1][i].y + lines[1][i].z)
            + fabs(points[1][i].x*lines[0][i].x +
     points[1][i].y*lines[0][i].y + lines[0][i].z);
        avgErr += err;
    }
    printf( "avg err = %g\n", avgErr/(nframes*n) );
    //COMPUTE AND DISPLAY RECTIFICATION
    mx1 = cvCreateMat( imageSize.height,
                       imageSize.width, CV_32F );
    my1 = cvCreateMat( imageSize.height,
                       imageSize.width, CV_32F );
    mx2 = cvCreateMat( imageSize.height,         
                       imageSize.width, CV_32F );
    my2 = cvCreateMat( imageSize.height,
                       imageSize.width, CV_32F );

    double R1[3][3], R2[3][3], P1[3][4], P2[3][4];
    CvMat _R1 = cvMat(3, 3, CV_64F, R1);
    CvMat _R2 = cvMat(3, 3, CV_64F, R2);
    // IF BY CALIBRATED (BOUGUET'S METHOD)
    CvMat _P1 = cvMat(3, 4, CV_64F, P1);
    CvMat _P2 = cvMat(3, 4, CV_64F, P2);
    cvStereoRectify( &_M1, &_M2, &_D1, &_D2, imageSize,
                     &_R, &_T,
                     &_R1, &_R2, &_P1, &_P2, 0,
                     0 );//CV_CALIB_ZERO_DISPARITY
    //Precompute maps for cvRemap()
    cvInitUndistortRectifyMap(&_M1,&_D1,&_R1,&_P1,mx1,my1);
    cvInitUndistortRectifyMap(&_M2,&_D2,&_R2,&_P2,mx2,my2);

    cvCopy(mx1, _mx1_);
    cvCopy(mx2, _mx2_);
    cvCopy(my1, _my1_);
    cvCopy(my2, _my2_);    
}

int vtkStereoCalibCVClass::createFindChessboardCornersFlag() {
   int flag = 0;
   
   if ( ADAPTIVE_THRESH != 0 ) {
       flag = flag | CV_CALIB_CB_ADAPTIVE_THRESH;
   }
   if ( NORMALIZE_IMAGE != 0 ) {
       flag = flag | CV_CALIB_CB_NORMALIZE_IMAGE;
   }
   if ( FILTER_QUADS != 0 ) {
       flag = flag | CV_CALIB_CB_FILTER_QUADS;
   }
   
   return flag;
}

void vtkStereoCalibCVClass::displayChessboard(IplImage* frame){//100603-komura
  CvPoint2D32f corners[CORNER_NUMBER];
  // IplImage *grayImage = cvCreateImage( cvGetSize( frame ), IPL_DEPTH_8U, 1 );
  int cornerCount;               // number of finding coners  
  int findChessboardCornersFlag; // cvFindChessboardCorners of flag
  int findFlag;                  //  flag of finding all coners
  
  //create flag for cvChessboardCorners
  findChessboardCornersFlag = createFindChessboardCornersFlag();
  IplImage *halfImage = cvCreateImage (cvSize (frame->width / 2, frame->height / 2), 
                                       frame->depth, frame->nChannels);
  cvResize (frame, halfImage, CV_INTER_AREA);
  
  // find coner
  findFlag=cvFindChessboardCorners(
                                   halfImage,
                                   cvSize( CORNER_WIDTH, CORNER_HEIGHT ),
                                   corners,
                                   &cornerCount,
                                   findChessboardCornersFlag
                                   );
  for(int i=0;i<CORNER_NUMBER;i++){
    corners[i].x = corners[i].x * 2;
    corners[i].y = corners[i].y * 2;
  }
  cvDrawChessboardCorners( frame, cvSize( CORNER_WIDTH, CORNER_HEIGHT ), corners, cornerCount, findFlag );
  cvReleaseImage(&halfImage);
}
