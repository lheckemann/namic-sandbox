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
//  igtlclose(SD);
//
//    SD  : (integer)    Socket descriptor (-1 if failed to connect)
//

#include "mex.h"
#include <math.h>

#include "igtlOSUtil.h"
#include "igtlImageMessage.h"
#include "igtlMexClientSocket.h"

using namespace std;

//#define pi (3.141592653589793)

#define ARG_ID_SD      0  // socket descriptor
#define ARG_ID_NUM     1  // total number of arguments

#define MAX_STRING_LEN 256

//extern void _main();

// -----------------------------------------------------------------
// Function declarations.
double& createMatlabScalar (mxArray*& ptr);

// -----------------------------------------------------------------
// Function definitions.
void mexFunction (int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[]) {
  
  // ---------------------------------------------------------------
  // Check numbers of arguments and outputs
  if (nrhs != ARG_ID_NUM)
    {
    mexErrMsgTxt("Incorrect number of input arguments");
    }

  //if (nlhs != 1)
  //  {
  //  mexErrMsgTxt("Incorrect number of output arguments");
  //  }

  // ---------------------------------------------------------------
  // Check types of arguments

  if (!mxIsNumeric(prhs[ARG_ID_SD]))
    {
    mexErrMsgTxt("PORT argument must be integer.");
    }

  // ---------------------------------------------------------------
  // Set variable

  int sd = (int)*mxGetPr(prhs[ARG_ID_SD]);

  // ---------------------------------------------------------------
  // Set up OpenIGTLink Connection
  igtl::MexClientSocket::Pointer socket;
  socket = igtl::MexClientSocket::New();

  int r = socket->SetDescriptor(sd);

  socket->CloseSocket();

}



