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
//  r = igtlsend(SD, DATA);
//
//    SD  : (integer)    Socket descriptor (-1 if failed to connect)
//    DATA: (structure)  Data contents
//
//  Data fields for IMAGE data
//    DATA.Type : (string)     must be 'IMAGE'
//    DATA.Name : (string)     Data name (DEVICE_NAME in OpenIGTLink)
//    DATA.Image: (uint16[][]) Image data
//    DATA.Trans: (real[4][4]) Affine transform matrix (4x4)
//
//  Data fields for TRANSFORM data
//    DATA.Type : (string)     must be 'TRANSFORM'
//    DATA.NAME : (string)     Data name (DEVICE_NAME in OpenIGTLink)
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
#define ARG_ID_DATA    1  // data
#define ARG_ID_NUM     2  // total number of arguments

#define MAX_STRING_LEN 256

//extern void _main();

// -----------------------------------------------------------------
// Function declarations.
double  getMatlabScalar    (const mxArray* ptr);
double& createMatlabScalar (mxArray*& ptr);

int checkArguments(int nlhs, mxArray *plhs[],
                   int nrhs, const mxArray *prhs[]);

int procTransformData(int sd, const char* name, const mxArray *ptr);
int procImageData(int sd, const char* name, const mxArray *ptr);
template<typename DATATYPE> void procTypedImageData(int sd, const char* name, const mxArray* imField, const mxArray* trField, DATATYPE dtype);

//int checkData(const char* type, const mxArray* prhs);


// -----------------------------------------------------------------
// Function definitions.
void mexFunction (int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[])
{
  
  // ---------------------------------------------------------------
  // Get reference to return value
  double& retVal = createMatlabScalar(plhs[0]);

  // ---------------------------------------------------------------
  // Check arguments
  if (checkArguments(nlhs, plhs, nrhs, prhs) == 0)
    {
    retVal = 0;
    return;
    }

  // ---------------------------------------------------------------
  // Set header variables
  int  sd;
  char type[MAX_STRING_LEN];
  char name[MAX_STRING_LEN];

  sd = (int)*mxGetPr(prhs[ARG_ID_SD]);

  // Get DATA.type
  mxArray*  typeField = mxGetField(prhs[ARG_ID_DATA], 0, "Type");
  if (typeField == NULL)
    {
    mexErrMsgTxt("No DATA.Type field.");
    retVal = 0;
    return;
    }
  mxGetString(typeField, type, MAX_STRING_LEN);

  
  // Get DATA.name
  mxArray*  nameField = mxGetField(prhs[ARG_ID_DATA], 0, "Name");
  if (nameField == NULL)
    {
    mexErrMsgTxt("No DATA.Name field.");
    retVal = 0;
    return;
    }
  mxGetString(nameField, name, MAX_STRING_LEN);

  
  // ---------------------------------------------------------------
  // Process data field and send through OpenIGTLink connection
  int r;
  if (strcmp(type, "TRANSFORM") == 0)
    {
    r = procTransformData(sd, name, prhs[ARG_ID_DATA]);
    }
  else if (strcmp(type, "IMAGE") == 0)
    {
    r = procImageData(sd, name, prhs[ARG_ID_DATA]);
    }
  
  // ---------------------------------------------------------------
  // Return result to Matlab
  
  retVal = r;
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

  // DATA
  if (!mxIsStruct(prhs[ARG_ID_DATA]))
    {
    mexErrMsgTxt("DATA argument must be structure.");
    return 0;
    }

  return 1;

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


int procImageData(int sd, const char* name, const mxArray *ptr)
{
  mxArray* imageField =  mxGetField(ptr, 0, "Image");
  mxArray* transField =  mxGetField(ptr, 0, "Trans");

  mxClassID DataType;
  DataType = mxGetClassID(imageField);

  switch(DataType)
    {
    case mxINT8_CLASS:    { igtlInt8 dummy = igtl::ImageMessage::TYPE_INT8; procTypedImageData(sd, name, imageField, transField, dummy); break; }
    case mxUINT8_CLASS:   { igtlUint8 dummy = igtl::ImageMessage::TYPE_UINT8; procTypedImageData(sd, name, imageField, transField, dummy); break; } 
    case mxINT16_CLASS:   { igtlInt16 dummy = igtl::ImageMessage::TYPE_INT16; procTypedImageData(sd, name, imageField, transField, dummy); break; }
    case mxUINT16_CLASS:  { igtlUint16 dummy = igtl::ImageMessage::TYPE_UINT16; procTypedImageData(sd, name, imageField, transField, dummy); break; }
    case mxINT32_CLASS:   { igtlInt32 dummy = igtl::ImageMessage::TYPE_INT32; procTypedImageData(sd, name, imageField, transField, dummy); break; }
    case mxUINT32_CLASS:  { igtlUint32 dummy = igtl::ImageMessage::TYPE_UINT32; procTypedImageData(sd, name, imageField, transField, dummy); break; }
    case mxSINGLE_CLASS:  { igtlFloat32 dummy = igtl::ImageMessage::TYPE_FLOAT32; procTypedImageData(sd, name, imageField, transField, dummy); break; }
    case mxDOUBLE_CLASS:  { igtlFloat64 dummy = igtl::ImageMessage::TYPE_FLOAT64; procTypedImageData(sd, name, imageField, transField, dummy); break; }
    default:              { igtlFloat32 dummy = igtl::ImageMessage::TYPE_FLOAT32; procTypedImageData(sd, name, imageField, transField, dummy); break; }          
    }
  
  return 1;

}


template<typename DATATYPE> void procTypedImageData(int sd, const char* name, const mxArray *imField, const mxArray *trField, DATATYPE dtype)
{

  DATATYPE* rdata = (DATATYPE*)mxGetPr(imField);

  int ndim             = mxGetNumberOfDimensions(imField);
  const mwSize*  s     = mxGetDimensions(imField);
  int size[3];
  size[0] = s[0]; size[1] = s[1]; size[2] = (ndim == 3)? s[2] : 1;
  double*     trans    = mxGetPr(trField);

  float norm_i[] = {trans[0], trans[1], trans[2]};
  float norm_j[] = {trans[4], trans[5], trans[6]};
  float norm_k[] = {trans[8], trans[9], trans[10]};
  float pos[]    = {trans[12], trans[13], trans[14]};
  float spacing[3];
  int svoffset[] = {0, 0, 0};

  // calculate spacing
  spacing[0] = sqrt(norm_i[0]*norm_i[0] + norm_i[1]*norm_i[1] + norm_i[2]*norm_i[2]);
  spacing[1] = sqrt(norm_j[0]*norm_j[0] + norm_j[1]*norm_j[1] + norm_j[2]*norm_j[2]);
  spacing[2] = sqrt(norm_k[0]*norm_k[0] + norm_k[1]*norm_k[1] + norm_k[2]*norm_k[2]);

  // normalize
  for (int i = 0; i < 3; i ++)
    {
    norm_i[i] /= spacing[0];
    norm_j[i] /= spacing[1];
    norm_k[i] /= spacing[2];
    }
  
  igtl::Matrix4x4 mat;
  mat[0][0] = norm_i[0]; mat[0][1] = norm_j[0]; mat[0][2] = norm_k[0]; mat[0][3] = pos[0];
  mat[1][0] = norm_i[1]; mat[1][1] = norm_j[1]; mat[1][2] = norm_k[1]; mat[1][3] = pos[1];
  mat[2][0] = norm_i[2]; mat[2][1] = norm_j[2]; mat[2][2] = norm_k[2]; mat[2][3] = pos[2];
  mat[3][0] = 0.0;  mat[3][1] = 0.0;  mat[3][2] = 0.0; mat[3][3] = 1;

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
  imgMsg->SetMatrix(mat);
  imgMsg->SetSpacing(spacing);
  imgMsg->SetEndian(igtl::ImageMessage::ENDIAN_LITTLE);
  imgMsg->SetScalarType(dtype);
  imgMsg->SetDeviceName(name);
  imgMsg->SetSubVolume(size, svoffset);
  imgMsg->AllocateScalars();

  int ni = size[0]; int nj = size[1]; int nk = size[2];
  DATATYPE* dest = (DATATYPE*)imgMsg->GetScalarPointer();
  for (int k = 0; k < nk; k ++)
    {
    int koff = k*ni*nj;
    for (int j = 0; j < nj; j ++)
      {
      for (int i = 0; i < ni; i ++)
        {
        dest[koff + j*ni + i] = (DATATYPE)rdata[koff + i*nj + j];
        }
      }
    }

  // ---------------------------------------------------------------
  // Send image message
  
  imgMsg->Pack();
  socket->Send(imgMsg->GetPackPointer(), imgMsg->GetPackSize());
  
  mexPrintf("The image has been sent.\n");
  
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
