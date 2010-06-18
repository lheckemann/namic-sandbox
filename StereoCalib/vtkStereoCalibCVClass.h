
#ifndef __vtkStereoCalibCVClass_h
#define __vtkStereoCalibCVClass_h


#include <cv.h>
#include <cxcore.h>
#include <highgui.h>


#include <vector>               // 
#include <string>               // 
#include <algorithm>            // 
#include <stdio.h>              // 
#include <ctype.h>              // 100603-komura



//chessLoad 100603-komura
#define ADAPTIVE_THRESH    0                        
#define NORMALIZE_IMAGE    0                        
#define FILTER_QUADS    0                               
#define CORNER_WIDTH    9                               
#define CORNER_HEIGHT 6                                  
#define CORNER_NUMBER    ( CORNER_WIDTH * CORNER_HEIGHT ) 

class vtkStereoCalibCVClass
{
public:
                static vtkStereoCalibCVClass *New(); 

                int n;
                int N;
                int nframes;

                double M1[3][3];
                double M2[3][3];
                double D1[5];
                double D1_mono[4];
                double D2[5];
                double R[3][3];
                double T[3];
                double E[3][3];
                double F[3][3];
 
    CvMat _M1;
    CvMat _M2;
    CvMat _D1;
                CvMat _D1_mono;
    CvMat _D2;
    CvMat _R;
    CvMat _T;
    CvMat _E;
    CvMat _F;
                CvMat _L1;
    CvMat _L2;
    CvMat _objectPoints;
    CvMat _imagePoints1;
    CvMat _imagePoints2;
    CvMat _npoints;
    CvSize        imageSize;

                CvMat* mx1;
                CvMat* my1;
                CvMat* mx2;
                CvMat* my2;

                void displayChessboard(IplImage*);
                int createFindChessboardCornersFlag();
                void displayStereoCalib(CvMat*, CvMat*, CvMat*, CvMat*);
                void stereoCalib(CvMat&, CvMat&, CvMat&, CvMat&);
                void monoCalib(CvMat&, CvMat&); // 100608-komura
                void chessLoad(IplImage* frame, int lr);
                vtkStereoCalibCVClass();
                ~vtkStereoCalibCVClass(); 

private:
};

#endif
