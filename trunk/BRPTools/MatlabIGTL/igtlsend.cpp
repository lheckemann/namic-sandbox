/*=========================================================================

  Program:   Matlab Open IGT Link Interface -- igtlsend
  Module:    $RCSfile: $
  Language:  C++
  Date:      $Date: $
  Version:   $Revision: $

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

//
//  r = igtlsend(SD, NAME, DATA, TRANSFORM);
//
//    SD  : (integer)    Socket descriptor (-1 if failed to connect)
//    NAME: (string)     Data name (DEVICE_NAME in OpenIGTLink)
//    DATA: (uint16[][]) Image data
//    TRANS:(real[4][4]) Affine transform matrix (4x4)
//

#include "mex.h"
#include <math.h>

#include "igtlOSUtil.h"
#include "igtlImageMessage.h"
#include "igtlMexClientSocket.h"

using namespace std;

//#define pi (3.141592653589793)

//#define ARG_ID_HOST    0  // host
//#define ARG_ID_PORT    1  // port
#define ARG_ID_SD      0
#define ARG_ID_NAME    1  // name
#define ARG_ID_DATA    2  // data
#define ARG_ID_TRANS   3  // transform
#define ARG_ID_NUM     4  // total number of arguments

#define MAX_STRING_LEN 256

//extern void _main();

// -----------------------------------------------------------------
// Function declarations.
double  getMatlabScalar    (const mxArray* ptr);
double& createMatlabScalar (mxArray*& ptr);

int checkArguments(int nlhs, mxArray *plhs[],
                   int nrhs, const mxArray *prhs[]);

// -----------------------------------------------------------------
// Function definitions.
void mexFunction (int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[]) {
  
  checkArguments(nlhs, plhs, nrhs, prhs);

  // ---------------------------------------------------------------
  // Set variables

  char hostname[MAX_STRING_LEN];
  char name[MAX_STRING_LEN];
  
  //mxGetString(prhs[ARG_ID_HOST], hostname, MAX_STRING_LEN);
  mxGetString(prhs[ARG_ID_NAME], name, MAX_STRING_LEN);

  //int         port     = (int)*mxGetPr(prhs[ARG_ID_PORT]);
  int         sd       = (int)*mxGetPr(prhs[ARG_ID_SD]);
  double*     rdata    = mxGetPr(prhs[ARG_ID_DATA]);
  int ndim             = mxGetNumberOfDimensions(prhs[ARG_ID_DATA]);
  const int*  s        = mxGetDimensions(prhs[ARG_ID_DATA]);
  int size[3];
  size[0] = s[0]; size[1] = s[1]; size[2] = (ndim == 3)? s[2]:1;
  double*     trans    = mxGetPr(prhs[ARG_ID_TRANS]);

  igtl::Matrix4x4 mat;
  mat[0][0] = trans[0];  mat[0][1] = trans[4];  mat[0][2] = trans[8];  mat[0][3] = trans[12];
  mat[1][0] = trans[1];  mat[1][1] = trans[5];  mat[1][2] = trans[9];  mat[1][3] = trans[13];
  mat[2][0] = trans[2];  mat[2][1] = trans[6];  mat[2][2] = trans[10]; mat[2][3] = trans[14];
  mat[3][0] = trans[3];  mat[3][1] = trans[7];  mat[3][2] = trans[11]; mat[3][3] = trans[15];

  float norm_i[] = {mat[0][0], mat[1][0], mat[2][0]};
  float norm_j[] = {mat[0][1], mat[1][1], mat[2][1]};
  float norm_k[] = {mat[0][2], mat[1][2], mat[2][2]};
  float pos[]      = {mat[0][3], mat[1][3], mat[2][3]};
  float spacing[3];
  int svoffset[] = {0, 0, 0};

  // calculate spacing
  spacing[0] = sqrt(norm_i[0]*norm_i[0] + norm_i[1]*norm_i[1] + norm_i[2]*norm_i[2]);
  spacing[1] = sqrt(norm_j[0]*norm_j[0] + norm_j[1]*norm_j[1] + norm_j[2]*norm_j[2]);
  spacing[2] = sqrt(norm_k[0]*norm_k[0] + norm_k[1]*norm_k[1] + norm_k[2]*norm_k[2]);

  // normalize
  for (int i = 0; i < 3; i ++)
    {
    norm_i[i] /= spacing[i];
    norm_j[i] /= spacing[i];
    norm_k[i] /= spacing[i];
    }

  // print variables
  //mexPrintf("Hostname   : %s\n", hostname);
  //mexPrintf("Port #     : %d\n", port);
  mexPrintf("Data Name  : %s\n", name);
  mexPrintf("Size       : (%d, %d, %d)\n", size[0], size[1], size[2]);
  mexPrintf("Spacing    : (%f, %f, %f)\n", spacing[0], spacing[1], spacing[2]);
  mexPrintf("Transform  : [%1.6f, %1.6f %1.6f %1.6f]\n", mat[0][0], mat[0][1], mat[0][2], mat[0][3]);
  mexPrintf("             [%1.6f, %1.6f %1.6f %1.6f]\n", mat[1][0], mat[1][1], mat[1][2], mat[1][3]);
  mexPrintf("             [%1.6f, %1.6f %1.6f %1.6f]\n", mat[2][0], mat[2][1], mat[2][2], mat[2][3]);
  mexPrintf("             [%1.6f, %1.6f %1.6f %1.6f]\n", mat[3][0], mat[3][1], mat[3][2], mat[3][3]);


  // ---------------------------------------------------------------
  // Set up OpenIGTLink Connection
  igtl::MexClientSocket::Pointer socket;
  socket = igtl::MexClientSocket::New();

  int r = socket->SetDescriptor(sd);

//  int r = socket->ConnectToServer(hostname, port);
//
  if (r != 0)
    {
    mexErrMsgTxt("Invalid socket descriptor.");
    }

  // ---------------------------------------------------------------
  // Prepare image message
  igtl::ImageMessage::Pointer imgMsg = igtl::ImageMessage::New();
  imgMsg->SetDimensions(size);
  imgMsg->SetSpacing(spacing);
  imgMsg->SetNormals(norm_i, norm_j, norm_k);
  imgMsg->SetScalarType(igtl::ImageMessage::TYPE_UINT16);
  imgMsg->SetDeviceName(name);
  imgMsg->SetSubVolume(size, svoffset);
  imgMsg->AllocateScalars();

  //int npixel = size[0]*size[1]*size[2];
  int ni = size[0]; int nj = size[1]; int nk = size[2];
  igtlUint16* dest = (igtlUint16*)imgMsg->GetScalarPointer();
  for (int k = 0; k < nk; k ++)
    {
    int koff = k*ni*nj;
    for (int j = 0; j < nj; j ++)
      {
      for (int i = 0; i < ni; i ++)
        {
        dest[koff + j*ni + i] = (igtlUint16)rdata[koff + i*nj + j];
        }
      }
    }

  // ---------------------------------------------------------------
  // Send image message

  imgMsg->Pack();
  socket->Send(imgMsg->GetPackPointer(), imgMsg->GetPackSize());

  mexPrintf("The image has been sent.\n");

  socket->CloseSocket();

  double& p = createMatlabScalar(plhs[0]);

  p = 1;
}


int checkArguments(int nlhs, mxArray *plhs[],
                   int nrhs, const mxArray *prhs[])
{
  // ---------------------------------------------------------------
  // Check numbers of arguments and outputs
  if (nrhs != ARG_ID_NUM)
    {
    mexErrMsgTxt("Incorrect number of input arguments");
    }

  if (nlhs != 1)
    {
    mexErrMsgTxt("Incorrect number of output arguments");
    }

  // ---------------------------------------------------------------
  // Check types of arguments

//  // HOST
//  if (!mxIsChar(prhs[ARG_ID_HOST]))
//    {
//    mexErrMsgTxt("HOST argument must be string.");
//    }
//
//  // PORT
//  if (!mxIsNumeric(prhs[ARG_ID_PORT]))
//    {
//    mexErrMsgTxt("PORT argument must be integer.");
//    }

  // SD -- socket descriptor
  if (!mxIsNumeric(prhs[ARG_ID_SD]))
    {
    mexErrMsgTxt("SD argument must be integer.");
    }

  // NAME
  if (!mxIsChar(prhs[ARG_ID_NAME]))
    {
    mexErrMsgTxt("NAME argument must be string.");
    }

  // DATA
  if (!(mxIsNumeric(prhs[ARG_ID_DATA]) && 
        (mxGetNumberOfDimensions(prhs[ARG_ID_DATA]) == 2 ||
         mxGetNumberOfDimensions(prhs[ARG_ID_DATA]) == 3)))
    {
    mexErrMsgTxt("DATA argument must be numeric and 2 or 3 dimensions.");
    }

  // TRANS
  if (!(mxIsNumeric(prhs[ARG_ID_TRANS]) && mxGetNumberOfDimensions(prhs[ARG_ID_TRANS]) == 2))
    {
    mexErrMsgTxt("TRANS argument must be numeric and 2-dimensional array.");
    }
  const int* s = mxGetDimensions(prhs[ARG_ID_TRANS]);
  if (!(s[0] == 4 && s[1] == 4))
    {
    mexErrMsgTxt("TRANS argument must be 4x4 2-dimensional array.");
    }

}


double getMatlabScalar (const mxArray* ptr) {

  // Make sure the input argument is a scalar in double-precision.
  if (!mxIsDouble(ptr) || mxGetNumberOfElements(ptr) != 1)
    mexErrMsgTxt("The input argument must be a double-precision scalar");

  return *mxGetPr(ptr);
}

double& createMatlabScalar (mxArray*& ptr) { 
  ptr = mxCreateDoubleMatrix(1,1,mxREAL);
  return *mxGetPr(ptr);
}
