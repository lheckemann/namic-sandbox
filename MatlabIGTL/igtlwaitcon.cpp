/*=========================================================================

  Program:   Matlab Open IGT Link Interface -- igtlwaitcon
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
//  cfd = igtlwaitcon(SD, DATA);
//
//    SD      : (integer)   Server Socket descriptor (-1 if failed to connect)
//    TIMEOUT : (integer)   Time out in millisecond
//    cfd     : (integer)   Client file descriptor
//


#include "mex.h"
#include <math.h>
#include <string.h>

#include "igtlOSUtil.h"
#include "igtlMexServerSocket.h"
#include "igtlMexClientSocket.h"

using namespace std;

//#define pi (3.141592653589793)

#define ARG_ID_SD      0
#define ARG_ID_TIMEOUT 1  // time out
#define ARG_ID_NUM     2  // total number of arguments

#define MAX_STRING_LEN 256

//extern void _main();

// -----------------------------------------------------------------
// Function declarations.
double& createMatlabScalar (mxArray*& ptr);

int checkArguments(int nlhs, mxArray *plhs[],
                   int nrhs, const mxArray *prhs[]);

int checkData(const char* type, const mxArray* prhs);


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
  int  timeOut;
  char type[MAX_STRING_LEN];
  char name[MAX_STRING_LEN];

  sd = (int)*mxGetPr(prhs[ARG_ID_SD]);
  timeOut = (int)*mxGetPr(prhs[ARG_ID_TIMEOUT]);

  igtl::MexServerSocket::Pointer serverSocket;
  serverSocket = igtl::MexServerSocket::New();
  serverSocket->SetDescriptor(sd);

  igtl::MexSocket::Pointer socket;
  socket = serverSocket->WaitForConnection(timeOut);

  // ---------------------------------------------------------------
  // Return result to Matlab
  if (socket.IsNotNull()) // if client connected
    {
    // Create a message buffer to receive header
    retVal = socket->GetDescriptor();
    }
  else
    {
    retVal = -1;
    }


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

  // TIMEOUT -- time out
  if (!mxIsNumeric(prhs[ARG_ID_TIMEOUT]))
    {
    mexErrMsgTxt("DATA argument must be integer.");
    return 0;
    }

  return 1;

}


double& createMatlabScalar (mxArray*& ptr) { 
  ptr = mxCreateDoubleMatrix(1,1,mxREAL);
  return *mxGetPr(ptr);
}
