/*=========================================================================

  Program:   Robin Relay -- Communication link between the RM Endoscout and Slicer3
  Module:    $RCSfile: $
  Language:  C++
  Date:      $Date: $
  Version:   $Revision: $

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

//OpenIGTLink includes
#include <iostream>
#include <math.h>
#include "igtlOSUtil.h"
#include "igtlTransformMessage.h"
#include "igtlClientSocket.h"

//Robin Relay
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <cstdlib>
#include <unistd.h>
#include <errno.h>
#include <string>
#include <sstream>
#include <vector>

//Time (random)
#include <time.h>

//Custom
#include "RobinIF.h"

using namespace std;

void GetRandomTestMatrix(igtl::Matrix4x4& matrix,std::vector<double> pos,std::vector<double> ori);

int main(int argc, char* argv[])
{
  /*** Parse Arguments ***/

  if (argc != 7) // check number of arguments
    {
    // If not correct, print usage
    std::cerr << "Usage: " << argv[0] << " <hostname> <port> <fps> <robinhost> <robinport> <# devices>"     << std::endl;
    std::cerr << "   <slicerhost> : Slicer PC IP or host name"          << std::endl;
    std::cerr << "   <port>       : Port # (18944 in Slicer default)"   << std::endl;
    std::cerr << "   <fps>        : Frequency (fps) to send coordinate" << std::endl;
    std::cerr << "   <robinhost>  : Robin PC IP or host name"           << std::endl;
    std::cerr << "   <robinport>  : Robin PC port"                      << std::endl;
    std::cerr << "   <#devices>    : Number of RM devices connected"    << std::endl;
    exit(0);
    }

  char*  slicerhost = argv[1];        // Hostname/IP of host running slicer
  int    slicerport = atoi(argv[2]);  // Port of OpenIGTLink running on slicerhost
  double fps        = atof(argv[3]);  // Frames per second to poll for new data
  char*  robinhost  = argv[4];        // Hostname/IP of RM Endoscout system
  int    robinport  = atoi(argv[5]);  // Port of Endoscout Server
  int    devices    = atoi(argv[6]);  // Number of Tracking devices (only 1 or 2)

  int    interval = (int) (1000.0 / fps);

  /** Device Check **/
  if (devices < 1 || devices > 2){
    std::cerr << "Number of devices not supported (1-2)" << std::endl;
    exit(0);
  }


  /*** Connect to Hosts ***/  
  /** Slicer **/
  igtl::ClientSocket::Pointer socket;
  socket = igtl::ClientSocket::New();
  int r = socket->ConnectToServer(slicerhost, slicerport);
  if (r != 0){
    std::cerr <<"Cannot connect to the OpenIGTLink in Slicer.." << std::endl;
    std::cerr <<"Please make sure OpenIGTLink server is active." << std::endl;
    exit(0);
  }
  cout << "Connected Successfully to Slicer Host!" << endl;
  

  /** Robin **/
  RobinIF myRMIF = RobinIF(robinhost,robinport);
  if(!myRMIF.isAlive()){
    std::cerr << "Connection to Robin Host refused!" << endl;
    std::cerr << "Please make sure endoscout is active and the .ini port is set." << endl;
    exit(0);
  }
  cout << "Connected Successfully to Robin Host!" << endl;
  
  //------------------------------------------------------------
  // Allocate Transform Message Class

  igtl::TransformMessage::Pointer transMsg;
  transMsg = igtl::TransformMessage::New();
  
  //------------------------------------------------------------
  // loop

  for (int i = 0; i < 1000000; i ++)
    {
      myRMIF.dataFormat();      // Request Data in quaternion form
      myRMIF.dataRead();        // Read from input buffer and set position and orientation
      
      igtl::Matrix4x4 matrix;
      transMsg->SetDeviceName("Tracker");
      GetRandomTestMatrix(matrix,myRMIF.getPosition(0),myRMIF.getOrientation(0));       // Build a new matrix
      transMsg->SetMatrix(matrix);                                                
      transMsg->Pack();                                                                 // Pack it
      socket->Send(transMsg->GetPackPointer(), transMsg->GetPackSize());                // Ship it to Slicer
      
      if(devices==2){                                                                   // Look for device #2
        transMsg->SetDeviceName("Sensor 2");                                           
        GetRandomTestMatrix(matrix,myRMIF.getPosition(1),myRMIF.getOrientation(1));     // Build a new matrix
        transMsg->SetMatrix(matrix);
        transMsg->Pack();                                                               // Pack it
        socket->Send(transMsg->GetPackPointer(), transMsg->GetPackSize());              // Ship to Slicer
      }
      
      igtl::Sleep(interval); // wait
      
    }
  
  //------------------------------------------------------------
  // Close connection
  
  transMsg->Delete();
  socket->CloseSocket();
  socket->Delete();

}

//------------------------------------------------------------
// Function to generate random matrix.


void GetRandomTestMatrix(igtl::Matrix4x4& matrix,std::vector<double> pos,std::vector<double> ori)
{
  float position[3];
  float orientation[4];

  srand(time(NULL));

  // random position
  position[0] = pos[0];
  position[1] = pos[1];
  position[2] = pos[2];
  
  cout << "Position X:" << pos[0] <<" Y:" << pos[1] << " Z:" << pos[2] << endl; 
  
  // random orientation
  orientation[0]= ori[0];
  orientation[1]= ori[1];
  orientation[2]= ori[2];
  orientation[3]= ori[3];

  //igtl::Matrix4x4 matrix;
  igtl::QuaternionToMatrix(orientation, matrix);

  matrix[0][3] = position[0];
  matrix[1][3] = position[1];
  matrix[2][3] = position[2];
  
  igtl::PrintMatrix(matrix);
}
