/*=========================================================================

  Program:   Matlab Open IGT Link Interface -- igtlreceive
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
//  [STATUS, DATA] = igtlreceive(SD);
//
//    STATUS:    (integer)    Status: 0: connection lost;  1: sucessful; -1: unknown data type/read error
//    DATA  :    (structure)  Data contents
//    SD    :    (integer)    Socket descriptor (-1 if failed to connect)
//
//  Data fields for IMAGE data
//    DATA.Type : (string)     must be 'IMAGE'
//    DATA.Name : (string)     Data name (DEVICE_NAME in OpenIGTLink)
//    DATA.Image: (uint16[][]) Image data
//    DATA.Trans: (real[4][4]) Affine transform matrix (4x4)
//
//  Data fields for TRANSFORM data
//    DATA.Type : (string)     must be 'TRANSFORM'
//    DATA.Name : (string)     Data name (DEVICE_NAME in OpenIGTLink)
//    DATA.Trans: (real[4][4]) Affine transform matrix (4x4)
//

#include "mex.h"
#include <math.h>
#include <string.h>

#include "igtlOSUtil.h"
#include "igtlImageMessage.h"
#include "igtlTransformMessage.h"
#include "igtlMexClientSocket.h"

using namespace std;

//#define pi (3.141592653589793)

#define ARG_ID_SD      0
//#define ARG_ID_TIMEOUT 1  // timeout : future work
#define ARG_ID_NUM     1  // total number of arguments

#define RET_ID_STATUS  0
#define RET_ID_DATA    1
#define RET_ID_NUM     2

#define MAX_STRING_LEN 256

//extern void _main();

// -----------------------------------------------------------------
// Function declarations.
//double  getMatlabScalar    (const mxArray* ptr);
double& createMatlabScalar (mxArray*& ptr);

int checkArguments(int nlhs, mxArray *plhs[],
                   int nrhs, const mxArray *prhs[]);
void receiveError(mxArray *plhs[], int status, const char* name, const char* type);
int waitAndReceiveMessage(int sd, mxArray *plhs[]);
int receiveTransform(igtl::MexClientSocket::Pointer& socket,
                         igtl::MessageHeader::Pointer& headerMsg,
                         mxArray *plhs[]);

//int procTransformData(int sd, const char* name, const mxArray *ptr);
//int procImageData(int sd, const char* name, const mxArray *ptr);
//int checkData(const char* type, const mxArray* prhs);


// -----------------------------------------------------------------
// Function definitions.
void mexFunction (int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[])
{

  // ---------------------------------------------------------------
  // Check arguments
  if (checkArguments(nlhs, plhs, nrhs, prhs) == 0)
    {
    //double& retVal = createMatlabScalar(plhs[0]);
    //retVal = 0;
    receiveError(plhs, -1, "", "");
    return;
    }

  // ---------------------------------------------------------------
  // Set socket descripter and timeout
  int  sd;
  sd = (int)*mxGetPr(prhs[ARG_ID_SD]);

  // ---------------------------------------------------------------
  // Wait for the message
  waitAndReceiveMessage(sd, plhs);

}


int checkArguments(int nlhs, mxArray *plhs[],
                   int nrhs, const mxArray *prhs[])
{
  // ---------------------------------------------------------------
  // Check numbers of arguments and outputs
  if (nrhs != ARG_ID_NUM)
    {
    mexErrMsgTxt("Incorrect number of input arguments");
    return 0;
    }

  if (nlhs != RET_ID_NUM)
    {
    mexErrMsgTxt("Incorrect number of output arguments");
    return 0;
    }

  // ---------------------------------------------------------------
  // Check types of arguments

  // SD -- socket descriptor
  if (!mxIsNumeric(prhs[ARG_ID_SD]))
    {
    mexErrMsgTxt("SD argument must be integer.");
    return 0;
    }

  return 1;

}


int waitAndReceiveMessage(int sd, mxArray *plhs[])
{
  int r;

  // ---------------------------------------------------------------
  // Create a message buffer to receive data

  printf("int waitAndReceiveMessage(int sd, mxArray *plhs[])\n");

  igtl::MessageHeader::Pointer headerMsg;
  headerMsg = igtl::MessageHeader::New();

  // ---------------------------------------------------------------
  // Set up OpenIGTLink Connection
  igtl::MexClientSocket::Pointer socket;
  socket = igtl::MexClientSocket::New();
  r = socket->SetDescriptor(sd);
  if (r != 0)
    {
    mexErrMsgTxt("Invalid socket descriptor.");
    receiveError(plhs, 0, "", "");
    return 0;
    }

  // ---------------------------------------------------------------
  // Receive generic header from the socket
  headerMsg->InitPack();
  r = socket->Receive(headerMsg->GetPackPointer(), headerMsg->GetPackSize());
  if (r == 0)
    {
    mexErrMsgTxt("Connection lost.");
    socket->CloseSocket();
    receiveError(plhs, 0, "", "");
    return 0;
    }
  if (r != headerMsg->GetPackSize())
    {
    mexErrMsgTxt("Invalid data size.");
    receiveError(plhs, -1, "", "");
    return 0;
    }

  // Deserialize the header
  headerMsg->Unpack();

  // ---------------------------------------------------------------
  // Check data type and receive data body

  if (strcmp(headerMsg->GetDeviceType(), "TRANSFORM") == 0)
    {
    printf("procReceiveTransform(socket, headerMsg, plhs);\n");
    receiveTransform(socket, headerMsg, plhs);
    }
  //else if (strcmp(headerMsg->GetDeviceType(), "IMAGE") == 0)
  //  {
  //  procReceiveImage(socket, headerMsg);
  //  }
  //else if (strcmp(headerMsg->GetDeviceType(), "POSITION") == 0)
  //  {
  //  procReceivePosition(socket, headerMsg);
  //  }
  //else if (strcmp(headerMsg->GetDeviceType(), "STATUS") == 0)
  //  {
  //  procReceiveStatus(socket, headerMsg);
  //  }
  else
    {
    socket->Skip(headerMsg->GetBodySizeToRead(), 0);
    receiveError(plhs, -1, "", "");
    }
}


void receiveError(mxArray *plhs[], int status, const char* name, const char* type)
  // status=0: connection lost
  // status=-1: invalid data size / no data handler
{

  double& retVal = createMatlabScalar(plhs[RET_ID_STATUS]);

  // Get strcutre for returned value
  const char* fnames [] = {
    "Type", "Name", "Trans"
  };
  plhs[RET_ID_DATA] = mxCreateStructMatrix(1, 1, 3, fnames);
  
  // Set type string
  mxArray* typeString = mxCreateString(type);
  mxSetField(plhs[RET_ID_DATA], 0, "Type", typeString);
  
  // Set device name string
  mxArray* nameString = mxCreateString(name);
  mxSetField(plhs[RET_ID_DATA], 0, "Name", nameString);
  
  retVal = status;

}


int receiveTransform(igtl::MexClientSocket::Pointer& socket,
                     igtl::MessageHeader::Pointer& headerMsg,
                     mxArray *plhs[])
{
  std::cerr << "Receiving TRANSFORM data type." << std::endl;
  
  // Create a message buffer to receive transform data
  igtl::TransformMessage::Pointer transMsg;
  transMsg = igtl::TransformMessage::New();
  transMsg->SetMessageHeader(headerMsg);
  transMsg->AllocatePack();
  
  // Receive transform data from the socket
  socket->Receive(transMsg->GetPackBodyPointer(), transMsg->GetPackBodySize());
  
  // Deserialize the transform data
  // If you want to skip CRC check, call Unpack() without argument.
  int c = transMsg->Unpack(1);

  double& retVal = createMatlabScalar(plhs[RET_ID_STATUS]);
  
  // Get strcutre for returned value
  const char* fnames [] = {
    "Type", "Name", "Trans"
  };
  plhs[RET_ID_DATA] = mxCreateStructMatrix(1, 1, 3, fnames);
  
  if (c & igtl::MessageHeader::UNPACK_BODY) // if CRC check is OK
    {
    // Retrive the transform data
    igtl::Matrix4x4 mat;
    transMsg->GetMatrix(mat);
    igtl::PrintMatrix(mat);

    // Set type string
    mxArray* typeString = mxCreateString("TRANSFORM");
    mxSetField(plhs[RET_ID_DATA], 0, "Type", typeString);

    // Set device name string
    mxArray* nameString = mxCreateString(transMsg->GetDeviceName());
    mxSetField(plhs[RET_ID_DATA], 0, "Name", nameString);

    // Set transform
    mxArray* transMatrix = mxCreateDoubleMatrix(4, 4, mxREAL);
    double*  trans       = mxGetPr(transMatrix);
    trans[0] = mat[0][0];  trans[4] = mat[0][1];  trans[8]  = mat[0][2]; trans[12] = mat[0][3];
    trans[1] = mat[1][0];  trans[5] = mat[1][1];  trans[9]  = mat[1][2]; trans[13] = mat[1][3];
    trans[2] = mat[2][0];  trans[6] = mat[2][1];  trans[10] = mat[2][2]; trans[14] = mat[2][3];
    trans[3] = mat[3][0];  trans[7] = mat[3][1];  trans[11] = mat[3][2]; trans[15] = mat[3][3];

    mxSetField(plhs[RET_ID_DATA], 0, "Trans", transMatrix);
    retVal = 1;
    return 1;
    }
  else
    {
    retVal = -1;
    return 0;
    }

}


int procTransformData(int sd, const char* name, const mxArray *ptr)
{
  mxArray*    transField = mxGetField(ptr, 0, "Trans");

  double*    trans       = mxGetPr(transField);
  const mwSize* s        = mxGetDimensions(transField);
  
  igtl::Matrix4x4 mat;
  mat[0][0] = trans[0];  mat[0][1] = trans[4];  mat[0][2] = trans[8];  mat[0][3] = trans[12];
  mat[1][0] = trans[1];  mat[1][1] = trans[5];  mat[1][2] = trans[9];  mat[1][3] = trans[13];
  mat[2][0] = trans[2];  mat[2][1] = trans[6];  mat[2][2] = trans[10]; mat[2][3] = trans[14];
  mat[3][0] = trans[3];  mat[3][1] = trans[7];  mat[3][2] = trans[11]; mat[3][3] = trans[15];
  
  float norm_i[] = {mat[0][0], mat[1][0], mat[2][0]};
  float norm_j[] = {mat[0][1], mat[1][1], mat[2][1]};
  float norm_k[] = {mat[0][2], mat[1][2], mat[2][2]};
  float pos[]    = {mat[0][3], mat[1][3], mat[2][3]};

  float a[3];
  // calculate absolutes
  a[0] = sqrt(norm_i[0]*norm_i[0] + norm_i[1]*norm_i[1] + norm_i[2]*norm_i[2]);
  a[1] = sqrt(norm_j[0]*norm_j[0] + norm_j[1]*norm_j[1] + norm_j[2]*norm_j[2]);
  a[2] = sqrt(norm_k[0]*norm_k[0] + norm_k[1]*norm_k[1] + norm_k[2]*norm_k[2]);

  // normalize
  for (int i = 0; i < 3; i ++)
    {
    norm_i[i] /= a[i];
    norm_j[i] /= a[i];
    norm_k[i] /= a[i];
    }
  
  // print variables
  mexPrintf("Data Name  : %s\n", name);
  mexPrintf("Transform  : [%1.6f, %1.6f %1.6f %1.6f]\n", mat[0][0], mat[0][1], mat[0][2], mat[0][3]);
  mexPrintf("             [%1.6f, %1.6f %1.6f %1.6f]\n", mat[1][0], mat[1][1], mat[1][2], mat[1][3]);
  mexPrintf("             [%1.6f, %1.6f %1.6f %1.6f]\n", mat[2][0], mat[2][1], mat[2][2], mat[2][3]);
  mexPrintf("             [%1.6f, %1.6f %1.6f %1.6f]\n", mat[3][0], mat[3][1], mat[3][2], mat[3][3]);

  // ---------------------------------------------------------------
  // Set up OpenIGTLink Connection
  igtl::MexClientSocket::Pointer socket;
  socket = igtl::MexClientSocket::New();
  int r = socket->SetDescriptor(sd);
  if (r != 0)
    {
    mexErrMsgTxt("Invalid socket descriptor.");
    }

  // ---------------------------------------------------------------
  // Prepare Transform message
  igtl::TransformMessage::Pointer transMsg = igtl::TransformMessage::New();
  transMsg->SetDeviceName(name);
  transMsg->SetMatrix(mat);
  transMsg->Pack();
  socket->Send(transMsg->GetPackPointer(), transMsg->GetPackSize());

  mexPrintf("The transform has been sent.\n");

  return 1;

}

/*
int procImageData(int sd, const char* name, const mxArray *ptr)
{
  mxArray* imageField =  mxGetField(ptr, 0, "Image");
  mxArray* transField =  mxGetField(ptr, 0, "Trans");
  
  double*     rdata    = mxGetPr(imageField);
  int ndim             = mxGetNumberOfDimensions(imageField);
  const int*  s        = mxGetDimensions(imageField);
  int size[3];
  size[0] = s[0]; size[1] = s[1]; size[2] = (ndim == 3)? s[2]:1;
  double*     trans    = mxGetPr(transField);

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
  socket->CloseSocket();

  mexPrintf("The image has been sent.\n");

  return 1;

}


double getMatlabScalar (const mxArray* ptr) {

  // Make sure the input argument is a scalar in double-precision.
  if (!mxIsDouble(ptr) || mxGetNumberOfElements(ptr) != 1)
    mexErrMsgTxt("The input argument must be a double-precision scalar");

  return *mxGetPr(ptr);
}
*/

double& createMatlabScalar (mxArray*& ptr)
{
  ptr = mxCreateDoubleMatrix(1,1,mxREAL);
  return *mxGetPr(ptr);
}

