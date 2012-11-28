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
//  DATA = igtlreceive(SD, timeout);
//
//    DATA:    (structure)  Data contents
//    SD  :    (integer)    Socket descriptor (-1 if failed to connect)
//    timeout: (integer/optional) Timeout in milliseconds
//
//  Data fields for IMAGE data
//    DATA.Status:(uint16)     0:Disconnected; -1:Timeout 1:Received 2:Invalid data
//    DATA.Type : (string)     must be 'IMAGE'
//    DATA.Name : (string)     Data name (DEVICE_NAME in OpenIGTLink)
//    DATA.Image: (uint16[][]) Image data
//    DATA.Trans: (real[4][4]) Affine transform matrix (4x4)
//
//  Data fields for TRANSFORM data
//    DATA.Status:(uint16)     0:Disconnected; -1:Timeout 1:Received 2:Invalid data
//    DATA.Type : (string)     must be 'TRANSFORM'
//    DATA.Name : (string)     Data name (DEVICE_NAME in OpenIGTLink)
//    DATA.Trans: (real[4][4]) Affine transform matrix (4x4)
//

#include "mex.h"
#include <math.h>
#include <string.h>

#include "igtlOSUtil.h"
#include "igtlTransformMessage.h"
#include "igtlPositionMessage.h"
#include "igtlImageMessage.h"
#include "igtlMexClientSocket.h"

using namespace std;

//#define pi (3.141592653589793)

#define ARG_ID_SD      0
#define ARG_ID_TIMEOUT 1  // timeout 
#define ARG_ID_NUM     2  // total number of arguments

#define MAX_STRING_LEN 256

//extern void _main();

// -----------------------------------------------------------------
// Function declarations.
//double  getMatlabScalar    (const mxArray* ptr);
//double& createMatlabScalar (mxArray*& ptr);

int checkArguments(int nlhs, mxArray *plhs[],
                   int nrhs, const mxArray *prhs[]);

int waitAndReceiveMessage(int sd, mxArray *plhs[], int timeout);

int receiveTransform(igtl::MexClientSocket::Pointer& socket,
                     igtl::MessageHeader::Pointer& headerMsg,
                     mxArray *plhs[]);

int receivePosition(igtl::MexClientSocket::Pointer& socket,
                    igtl::MessageHeader::Pointer& headerMsg,
                    mxArray *plhs[]);

int receiveImage(igtl::MexClientSocket::Pointer& socket,
                 igtl::MessageHeader::Pointer& headerMsg,
                 mxArray *plhs[]);


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
    return;
    }

  // ---------------------------------------------------------------
  // Set socket descripter
  int  sd;
  sd = (int)*mxGetPr(prhs[ARG_ID_SD]);

  // ---------------------------------------------------------------
  // Set Timeout, if specified
  int  timeout = -1; // -1 means no timeout.
  if (nrhs > ARG_ID_TIMEOUT)
    {
    timeout = (int)*mxGetPr(prhs[ARG_ID_TIMEOUT]);
    }

  // ---------------------------------------------------------------
  // Wait for the message
  waitAndReceiveMessage(sd, plhs, timeout);
}


int checkArguments(int nlhs, mxArray *plhs[],
                   int nrhs, const mxArray *prhs[])
{
  // ---------------------------------------------------------------
  // Check numbers of arguments and outputs
  if (nrhs < 1 || nrhs > ARG_ID_NUM)
    {
    mexErrMsgTxt("Incorrect number of input arguments");
    return 0;
    }

  if (nlhs != 1)
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

  // timeout -- socket descriptor
  if (nrhs > ARG_ID_TIMEOUT)
    {
    if (!mxIsNumeric(prhs[ARG_ID_TIMEOUT]))
      {
      mexErrMsgTxt("timeout argument must be integer.");
      return 0;
      }
    }

  return 1;

}

void createReturnStructureAndSetError(mxArray* plhs[], int ret)
{
  // Get strcutre for returned value
  const char* fnames [] = {
    "Status"
  };
  plhs[0] = mxCreateStructMatrix(1, 1, 1, fnames);
  mxArray* statusString = mxCreateDoubleScalar(ret);
  mxSetField(plhs[0], 0, "Status", statusString);
}

int waitAndReceiveMessage(int sd, mxArray *plhs[], int timeout)
{
  int r;

  // ---------------------------------------------------------------
  // Create a message buffer to receive data
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
    createReturnStructureAndSetError(plhs, 0);
    return 0;
    }

  // ---------------------------------------------------------------
  // Set timeout if timeout >= 0
  if (timeout >= 0)
    {
    socket->SetReceiveTimeout(timeout);
    }
  else
    {
    socket->SetReceiveTimeout(-1);
    }

  // ---------------------------------------------------------------
  // Receive generic header from the socket
  headerMsg->InitPack();
  r = socket->Receive(headerMsg->GetPackPointer(), headerMsg->GetPackSize());
  if (r == 0)
    {
    socket->CloseSocket();
    createReturnStructureAndSetError(plhs, 0);
    return 0;
    }
  if (r < 0)
    {
    createReturnStructureAndSetError(plhs, -1);
    return -1;
    }
  if (r != headerMsg->GetPackSize())
    {
    createReturnStructureAndSetError(plhs, 2);
    return 2;
    }

  // Deserialize the header
  headerMsg->Unpack();

  // ---------------------------------------------------------------
  // Check data type and receive data body

  if (strcmp(headerMsg->GetDeviceType(), "TRANSFORM") == 0)
    {
    // Get strcutre for returned value
    const char* fnames [] = {
      "Status", "Type", "Name", "Trans"
    };
    plhs[0] = mxCreateStructMatrix(1, 1, 4, fnames);
    printf("procReceiveTransform(socket, headerMsg, plhs);\n");
    receiveTransform(socket, headerMsg, plhs);
    }
  else if (strcmp(headerMsg->GetDeviceType(), "IMAGE") == 0)
    {
    // Get strcutre for returned value
    const char* fnames [] = {
      "Status", "Type", "Name", "Trans", "Image"
    };
    plhs[0] = mxCreateStructMatrix(1, 1, 5, fnames);
    receiveImage(socket, headerMsg, plhs);
    }
  else if (strcmp(headerMsg->GetDeviceType(), "POSITION") == 0)
    {
    // Get strcutre for returned value
    const char* fnames [] = {
      "Status", "Type", "Name", "Pos", "Quat"
    };
    plhs[0] = mxCreateStructMatrix(1, 1, 5, fnames);
    receivePosition(socket, headerMsg, plhs);
    }
  //else if (strcmp(headerMsg->GetDeviceType(), "STATUS") == 0)
  //  {
  //  procReceiveStatus(socket, headerMsg);
  //  }
  else
    {
    socket->Skip(headerMsg->GetBodySizeToRead(), 0);
    createReturnStructureAndSetError(plhs, 2);
    return 2;
    }

  mxArray* statusString = mxCreateDoubleScalar(1);
  mxSetField(plhs[0], 0, "Status", statusString);

  return 1;
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
  
  if (c & igtl::MessageHeader::UNPACK_BODY) // if CRC check is OK
    {
    // Retrive the transform data
    igtl::Matrix4x4 mat;
    transMsg->GetMatrix(mat);
    igtl::PrintMatrix(mat);

    // Set type string
    mxArray* typeString = mxCreateString("TRANSFORM");
    mxSetField(plhs[0], 0, "Type", typeString);

    // Set device name string
    mxArray* nameString = mxCreateString(transMsg->GetDeviceName());
    mxSetField(plhs[0], 0, "Name", nameString);

    // Set transform
    mxArray* transMatrix = mxCreateDoubleMatrix(4, 4, mxREAL);
    double*  trans       = mxGetPr(transMatrix);
    trans[0] = mat[0][0];  trans[4] = mat[0][1];  trans[8]  = mat[0][2]; trans[12] = mat[0][3];
    trans[1] = mat[1][0];  trans[5] = mat[1][1];  trans[9]  = mat[1][2]; trans[13] = mat[1][3];
    trans[2] = mat[2][0];  trans[6] = mat[2][1];  trans[10] = mat[2][2]; trans[14] = mat[2][3];
    trans[3] = mat[3][0];  trans[7] = mat[3][1];  trans[11] = mat[3][2]; trans[15] = mat[3][3];

    mxSetField(plhs[0], 0, "Trans", transMatrix);
    return 1;
    }
  else
    {
    return 0;
    }

}


int receivePosition(igtl::MexClientSocket::Pointer& socket,
                    igtl::MessageHeader::Pointer& headerMsg,
                    mxArray *plhs[])
{
  std::cerr << "Receiving POSITION data type." << std::endl;
  
  // Create a message buffer to receive transform data
  igtl::PositionMessage::Pointer posMsg;
  posMsg = igtl::PositionMessage::New();
  posMsg->SetMessageHeader(headerMsg);
  posMsg->AllocatePack();
  
  // Receive transform data from the socket
  socket->Receive(posMsg->GetPackBodyPointer(), posMsg->GetPackBodySize());
  
  // Deserialize the transform data
  // If you want to skip CRC check, call Unpack() without argument.
  int c = posMsg->Unpack(1);
  
  if (c & igtl::MessageHeader::UNPACK_BODY) // if CRC check is OK
    {
    // Retrive position and quaternion data
    float position[3];
    float quaternion[4];

    posMsg->GetPosition(position);
    posMsg->GetQuaternion(quaternion);

    // print position and quaternion
    std::cerr << "position = (" 
              << position[0] << "," 
              << position[1] << "," 
              << position[2] << ")" << std::endl;
    std::cerr << "quaternion = (" 
              << quaternion[0] << "," 
              << quaternion[1] << "," 
              << quaternion[2] << "," 
              << quaternion[3] << ")" << std::endl;

    // Set type string
    mxArray* typeString = mxCreateString("POSITION");
    mxSetField(plhs[0], 0, "Type", typeString);

    // Set device name string
    mxArray* nameString = mxCreateString(posMsg->GetDeviceName());
    mxSetField(plhs[0], 0, "Name", nameString);

    // Set position and quaternion
    mxArray* posMatrix  = mxCreateDoubleMatrix(3, 1, mxREAL);
    double*  pos        = mxGetPr(posMatrix);
    mxArray* quatMatrix = mxCreateDoubleMatrix(4, 1, mxREAL);
    double*  quat       = mxGetPr(quatMatrix);
    pos[0]  = position[0];
    pos[1]  = position[1];
    pos[2]  = position[2];
    mxSetField(plhs[0], 0, "Pos", posMatrix);

    quat[0] = quaternion[0];
    quat[1] = quaternion[1];
    quat[2] = quaternion[2];
    quat[3] = quaternion[3];
    mxSetField(plhs[0], 0, "Quat", quatMatrix);
    return 1;
    }
  else
    {
    return 0;
    }

}




int receiveImage(igtl::MexClientSocket::Pointer& socket,
                 igtl::MessageHeader::Pointer& headerMsg,
                 mxArray *plhs[])
{
  // Create a message buffer to receive transform data
  igtl::ImageMessage::Pointer imgMsg;
  imgMsg = igtl::ImageMessage::New();
  imgMsg->SetMessageHeader(headerMsg);
  imgMsg->AllocatePack();
  
  // Receive transform data from the socket
  socket->Receive(imgMsg->GetPackBodyPointer(), imgMsg->GetPackBodySize());
  
  // Deserialize the transform data
  // If you want to skip CRC check, call Unpack() without argument.
  int c = imgMsg->Unpack(1);
  
  if (c & igtl::MessageHeader::UNPACK_BODY) // if CRC check is OK
    {
    // Retrive the image data
    int   size[3];          // image dimension
    float spacing[3];       // spacing (mm/pixel)
    int   svsize[3];        // sub-volume size
    int   svoffset[3];      // sub-volume offset
    int   scalarType;       // scalar type
    igtl::Matrix4x4 mat;    // Image origin and orientation matrix

    scalarType = imgMsg->GetScalarType();
    imgMsg->GetDimensions(size);
    imgMsg->GetSpacing(spacing);
    imgMsg->GetSubVolume(svsize, svoffset);
    imgMsg->GetMatrix(mat);

    // Set type string
    mxArray* typeString = mxCreateString("IMAGE");
    mxSetField(plhs[0], 0, "Type", typeString);

    // Set device name string
    mxArray* nameString = mxCreateString(imgMsg->GetDeviceName());
    mxSetField(plhs[0], 0, "Name", nameString);

    // Set transform
    mxArray* transMatrix = mxCreateDoubleMatrix(4, 4, mxREAL);
    double*  trans       = mxGetPr(transMatrix);

    mat[0][0] *= spacing[0];  mat[0][1] *= spacing[1];  mat[0][2] *= spacing[2];
    mat[1][0] *= spacing[0];  mat[1][1] *= spacing[1];  mat[1][2] *= spacing[2];
    mat[2][0] *= spacing[0];  mat[2][1] *= spacing[1];  mat[2][2] *= spacing[2];

    trans[0] = mat[0][0];  trans[4] = mat[0][1];  trans[8]  = mat[0][2]; trans[12] = mat[0][3];
    trans[1] = mat[1][0];  trans[5] = mat[1][1];  trans[9]  = mat[1][2]; trans[13] = mat[1][3];
    trans[2] = mat[2][0];  trans[6] = mat[2][1];  trans[10] = mat[2][2]; trans[14] = mat[2][3];
    trans[3] = mat[3][0];  trans[7] = mat[3][1];  trans[11] = mat[3][2]; trans[15] = mat[3][3];

    mxSetField(plhs[0], 0, "Trans", transMatrix);

    // Set Image
    mwSize dims[3];
    dims[0] = size[0];
    dims[1] = size[1];
    dims[2] = size[2];

    printf("size[] = {%d, %d, %d} \n", dims[0], dims[1], dims[2]);    


    //mxArray*    imageMatrix = mxCreateNumericArray(3, dims, mxDOUBLE_CLASS, mxREAL);
    //double*     rdata       = (double*) mxGetData(imageMatrix);

    mxArray* imageMatrix;

    int ni = size[0]; int nj = size[1]; int nk = size[2];
    if (scalarType == igtl::ImageMessage::TYPE_INT8)
      {
      imageMatrix = mxCreateNumericArray(3, dims, mxINT8_CLASS, mxREAL);
      igtlInt8* rdata       = (igtlInt8*) mxGetData(imageMatrix);

      printf("scalarType = TYPE_INT8\n");
      igtlInt8* src = (igtlInt8*)imgMsg->GetScalarPointer();
      for (int k = 0; k < nk; k ++)
        {
        int koff = k*ni*nj;
        for (int j = 0; j < nj; j ++)
          {
          for (int i = 0; i < ni; i ++)
            {
            rdata[koff + i*nj + j] = src[koff + j*ni + i];
            }
          }
        }
      }
    else if (scalarType == igtl::ImageMessage::TYPE_UINT8)
      {
      imageMatrix = mxCreateNumericArray(3, dims, mxUINT8_CLASS, mxREAL);
      igtlUint8* rdata       = (igtlUint8*) mxGetData(imageMatrix);

      printf("scalarType = TYPE_UINT8\n");
      igtlUint8* src = (igtlUint8*)imgMsg->GetScalarPointer();
      for (int k = 0; k < nk; k ++)
        {
        int koff = k*ni*nj;
        for (int j = 0; j < nj; j ++)
          {
          for (int i = 0; i < ni; i ++)
            {
            rdata[koff + i*nj + j] =  src[koff + j*ni + i];
            }
          }
        }
      }
    else if (scalarType == igtl::ImageMessage::TYPE_INT16)
      {
      imageMatrix = mxCreateNumericArray(3, dims, mxINT16_CLASS, mxREAL);
      igtlInt16* rdata       = (igtlInt16*) mxGetData(imageMatrix);

      printf("scalarType = TYPE_INT16\n");
      igtlInt16* src = (igtlInt16*)imgMsg->GetScalarPointer();
      for (int k = 0; k < nk; k ++)
        {
        int koff = k*ni*nj;
        for (int j = 0; j < nj; j ++)
          {
          for (int i = 0; i < ni; i ++)
            {
            rdata[koff + i*nj + j] =  src[koff + j*ni + i];
            }
          }
        }
      }
    else if (scalarType == igtl::ImageMessage::TYPE_UINT16)
      {
      imageMatrix = mxCreateNumericArray(3, dims, mxUINT16_CLASS, mxREAL);
      igtlUint16* rdata       = (igtlUint16*) mxGetData(imageMatrix);

      printf("scalarType = TYPE_UINT16\n");
      igtlUint16* src = (igtlUint16*)imgMsg->GetScalarPointer();
      for (int k = 0; k < nk; k ++)
        {
        int koff = k*ni*nj;
        for (int j = 0; j < nj; j ++)
          {
          for (int i = 0; i < ni; i ++)
            {
            rdata[koff + i*nj + j] =  src[koff + j*ni + i];
            }
          }
        }
      }
    else if (scalarType == igtl::ImageMessage::TYPE_INT32)
      {
      imageMatrix = mxCreateNumericArray(3, dims, mxINT32_CLASS, mxREAL);
      igtlInt32* rdata       = (igtlInt32*) mxGetData(imageMatrix);

      printf("scalarType = TYPE_INT32\n");
      igtlInt32* src = (igtlInt32*)imgMsg->GetScalarPointer();
      for (int k = 0; k < nk; k ++)
        {
        int koff = k*ni*nj;
        for (int j = 0; j < nj; j ++)
          {
          for (int i = 0; i < ni; i ++)
            {
            rdata[koff + i*nj + j] =  src[koff + j*ni + i];
            }
          }
        }
      }
    else if (scalarType == igtl::ImageMessage::TYPE_UINT32)
      {
      imageMatrix = mxCreateNumericArray(3, dims, mxUINT32_CLASS, mxREAL);
      igtlUint32* rdata       = (igtlUint32*) mxGetData(imageMatrix);

      printf("scalarType = TYPE_UINT32\n");
      igtlUint32* src = (igtlUint32*)imgMsg->GetScalarPointer();
      for (int k = 0; k < nk; k ++)
        {
        int koff = k*ni*nj;
        for (int j = 0; j < nj; j ++)
          {
          for (int i = 0; i < ni; i ++)
            {
            rdata[koff + i*nj + j] =  src[koff + j*ni + i];
            }
          }
        }
      }
    else if (scalarType == igtl::ImageMessage::TYPE_FLOAT32)
      {
      imageMatrix = mxCreateNumericArray(3, dims, mxSINGLE_CLASS, mxREAL);
      igtlFloat32* rdata       = (igtlFloat32*) mxGetData(imageMatrix);

      printf("scalarType = TYPE_FLOAT32\n");
      igtlFloat32* src = (igtlFloat32*)imgMsg->GetScalarPointer();
      for (int k = 0; k < nk; k ++)
        {
        int koff = k*ni*nj;
        for (int j = 0; j < nj; j ++)
          {
          for (int i = 0; i < ni; i ++)
            {
            rdata[koff + i*nj + j] =  src[koff + j*ni + i];
            }
          }
        }
      }
    else if (scalarType == igtl::ImageMessage::TYPE_FLOAT64)
      {
      imageMatrix = mxCreateNumericArray(3, dims, mxDOUBLE_CLASS, mxREAL);
      igtlFloat64* rdata       = (igtlFloat64*) mxGetData(imageMatrix);

      printf("scalarType = TYPE_FLOAT64\n");
      igtlFloat64* src = (igtlFloat64*)imgMsg->GetScalarPointer();
      for (int k = 0; k < nk; k ++)
        {
        int koff = k*ni*nj;
        for (int j = 0; j < nj; j ++)
          {
          for (int i = 0; i < ni; i ++)
            {
            rdata[koff + i*nj + j] =  src[koff + j*ni + i];
            }
          }
        }
      }
    mxSetField(plhs[0], 0, "Image", imageMatrix);

    return 1;
    }

  return 0;


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


double& createMatlabScalar (mxArray*& ptr) { 
  ptr = mxCreateDoubleMatrix(1,1,mxREAL);
  return *mxGetPr(ptr);
}
*/
