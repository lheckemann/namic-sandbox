/*=========================================================================

  Program:   Optotrak - OpenIGTLink Interface
  Module:    $RCSfile: $
  Language:  C++
  Date:      $Date: $
  Version:   $Revision: $

  Copyright (c) Brigham and Women's Hospital. All rights reserved.
  Copyright (c) Nagoya Institute of Technology. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifdef WIN32
#define NOMINMAX
#ifdef USE_RAWINPUT
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif
#endif
#endif

#if HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include <string>
#include <math.h>


//--------------------------------------------------------------------------------
// Optotrak
//--------------------------------------------------------------------------------
#include "ndtypes.h"
#include "ndpack.h"
#include "ndopto.h"

#include "ot_aux.h"


// Rigid body and marker information
#define NUM_MARKERS     10
#define FRAME_RATE      (float)100.0
#define COLLECTION_TIME (float)10.0

#define NUM_RIGID_BODIES    2
#define RIGID_BODY_1        0
#define RIGID_BODY_2        1
#define RIGID_BODY_FILE_1   "reference"
#define RIGID_BODY_FILE_2   "locator"


typedef struct RigidBodyDataStruct
{
    struct OptotrakRigidStruct  pRigidData[NUM_RIGID_BODIES];
    Position3d                  p3dData[NUM_MARKERS];
} RigidBodyDataType;

#define RIGID_BODY_N_MARKER_1 6
#define RIGID_BODY_N_MARKER_2 4

/*
static Position3d
    RigidBody1[RIGID_BODY_N_MARKER_1] =
    {
                {-11.6942F,     -0.0000F,      0.0000F},
                { 92.6282F,     -0.0000F,      0.0000F},
                { 96.4412F,     97.2981F,      0.5193F},
                {  0.0000F,    108.7348F,      0.0000F} 
    };

static Position3d
    RigidBody2[RIGID_BODY_N_MARKER_2] =
    {
                { 273.1690F, -0.0045F,  0.0026 },
                { 223.1803F, -0.0093F, -0.0109 },
                {  -0.0076F, -6.9846F, -0.0162 },
                { -52.1920F, -7.1505F,  0.5218 }
    };
*/
char
   szNDErrorString[MAX_ERROR_STRING_LENGTH + 1];



//--------------------------------------------------------------------------------
// ACE Library (for networking and threading)
//--------------------------------------------------------------------------------
#include <ace/ACE.h>
#include <ace/OS.h>
#include <ace/FILE.h>
#include <ace/Thread.h>
#include <ace/Synch.h>
#include <ace/Thread_Mutex.h>

#include <ace/INET_Addr.h>
#include <ace/Time_Value.h>
#include <ace/SOCK_Stream.h>
#include <ace/SOCK_Connector.h>
#include <ace/Log_Msg.h>

#include <iostream>

#include "igtlMath.h"
#include "igtlTransformMessage.h"
#include "ScannerSim.h"


class ACE_Thread_Mutex;

// for TCP/IP connection
ACE_INET_Addr   address;
ACE_SOCK_Stream sock;
ACE_SOCK_Connector connector;

ACE_Thread_Mutex * mutex;
void * thread;

bool stop;
bool connected;

void PrintMatrix(igtl::Matrix4x4 &matrix)
{
  std::cout << "=============" << std::endl;
  std::cout << matrix[0][0] << ", " << matrix[0][1] << ", " << matrix[0][2] << ", " << matrix[0][3] << std::endl;
  std::cout << matrix[1][0] << ", " << matrix[1][1] << ", " << matrix[1][2] << ", " << matrix[1][3]  << std::endl;
  std::cout << matrix[2][0] << ", " << matrix[2][1] << ", " << matrix[2][2] << ", " << matrix[2][3]  << std::endl;
  std::cout << matrix[3][0] << ", " << matrix[3][1] << ", " << matrix[3][2] << ", " << matrix[3][3]  << std::endl;
  std::cout << "=============" << std::endl;
}


//--------------------------------------------------------------------------------
// Optotrak
//--------------------------------------------------------------------------------

int ActivateOptotrak()
{
  int fLoadSystem;
  ACE_Time_Value timeVal(0, (int)(1000)*1000 ); // 1 sec
  
  fLoadSystem = 1;
  if(fLoadSystem)
    {
         //Load the system of processors.
      std::cout << "Loading System Transputer ... ";
      if( TransputerLoadSystem( "system" ) != OPTO_NO_ERROR_CODE )
                {
                        std::cout << "failed." << std::endl;
                        return 0;
            }
          ACE_OS::sleep( timeVal );
          std::cout << "done." << std::endl;
        }


  // Initialize the processors system.
  std::cout << "Initializing ... ";
  if( TransputerInitializeSystem( OPTO_LOG_ERRORS_FLAG | OPTO_LOG_MESSAGES_FLAG ) )
    {
                std::cout << "failed." << std::endl;
        return 0;
    }
  std::cout << "done." << std::endl;

    // Set optional processing flags (this overides the settings in Optotrak.INI).
  std::cout << "Setting processing flags ...";
  if( OptotrakSetProcessingFlags(   OPTO_LIB_POLL_REAL_DATA |
                                    OPTO_CONVERT_ON_HOST |
                                    OPTO_RIGID_ON_HOST ) )
    {
                std::cout << "failed." << std::endl;
        return 0;
    }
  std::cout << "done." <<std::endl;

  // Load the standard camera parameters.
  std::cout << "Loading standard camera parameters ... ";
  if( OptotrakLoadCameraParameters( "standard" ) )
    {
                std::cout << "failed." << std::endl;
        return 0;
    }
  std::cout << "done." << std::endl;


  // Set up a collection for the Optotrak.
  std::cout << "Setting up a collection for the Optotrak ... ";
  if( OptotrakSetupCollection(
            NUM_MARKERS,        /* Number of markers in the collection. */
            FRAME_RATE,         /* Frequency to collect data frames at. */
            (float)2500.0,      /* Marker frequency for marker maximum on-time. */
            30,                 /* Dynamic or Static Threshold value to use. */
            160,                /* Minimum gain code amplification to use. */
            1,                  /* Stream mode for the data buffers. */
            (float)0.4,         /* Marker Duty Cycle to use. */
            (float)7.5,         /* Voltage to use when turning on markers. */
            COLLECTION_TIME,    /* Number of seconds of data to collect. */
            (float)0.0,         /* Number of seconds to pre-trigger data by. */
            OPTOTRAK_NO_FIRE_MARKERS_FLAG | OPTOTRAK_BUFFER_RAW_FLAG ) )
    {
                std::cout << "failed." << std::endl;
        return 0;
    }

  // Wait one second to let the camera adjust.
  ACE_OS::sleep( timeVal );
  std::cout << "done." << std::endl;

  // Activate the markers.
  std::cout << "Activating the markers ... ";
  if( OptotrakActivateMarkers() )
    {
                std::cout << "failed.";
        return 0;
    }

  ACE_OS::sleep( timeVal );

  std::cout << "done." << std::endl;

  // Add rigid bodies for tracking to the Optotrak system from a .RIG file.
  std::cout << "Adding rigid bodies ... ";
  if( RigidBodyAddFromFile(
            RIGID_BODY_1,           /* ID associated with this rigid body.*/
            1,                      /* First marker in the rigid body.*/
            RIGID_BODY_FILE_1,      /* RIG file containing rigid body coordinates.*/
            OPTOTRAK_QUATERN_RIGID_FLAG | OPTOTRAK_RETURN_QUATERN_FLAG) )
    {
        std::cout << "failed to load RIGID_BODY_1." << std::endl;
        return 0;
    }

   if( RigidBodyAddFromFile(
            RIGID_BODY_2,           /* ID associated with this rigid body.*/
            1+RIGID_BODY_N_MARKER_1,/* First marker in the rigid body.*/
            RIGID_BODY_FILE_2,      /* RIG file containing rigid body coordinates.*/
            OPTOTRAK_QUATERN_RIGID_FLAG | OPTOTRAK_RETURN_QUATERN_FLAG) )
    {
        std::cout << "failed to load RIGID_BODY_2." << std::endl;
        return 0;
    }

#if 0   // Rigid body can be defined by using Position3d array.
  if( RigidBodyAdd(
            RIGID_BODY_1,           /* ID associated with this rigid body. */
            1,                      /* First marker in the rigid body. */
            RIGID_BODY_N_MARKER_1,  /* Number of markers in the rigid body. */
            (float *)RigidBody1,    /* 3D coords for each marker in the body. */
            NULL,                   /* no normals for this rigid body. */
                        OPTOTRAK_QUATERN_RIGID_FLAG | OPTOTRAK_RETURN_QUATERN_FLAG ) )
    {
                std::cout << "failed to load RIGID_BODY_1." << std::endl;
        return 0;
    }

  if( RigidBodyAdd(
            RIGID_BODY_2,           /* ID associated with this rigid body. */
            1+RIGID_BODY_N_MARKER_1,/* First marker in the rigid body. */
            RIGID_BODY_N_MARKER_2,  /* Number of markers in the rigid body. */
            (float *)RigidBody2,    /* 3D coords for each marker in the body. */
            NULL,                   /* no normals for this rigid body. */
            OPTOTRAK_QUATERN_RIGID_FLAG | OPTOTRAK_RETURN_QUATERN_FLAG) )
    {
        std::cout << "failed to load RIGID_BODY_2." << std::endl;
        return 0;
    }

#endif

   std::cout << "done." << std::endl;

  // Change the default frame of reference to be defined by rigid body one.
  std::cout << "Setting constant rigid flag for RIGID_BODY_1 ... ";
  if( RigidBodyChangeFOR( RIGID_BODY_1, OPTOTRAK_CONSTANT_RIGID_FLAG ) )
    {
                std::cout << "failed." << std::endl;
        return 0;
    }
  std::cout << "done." << std::endl;
  return 1;

}


int DeactivateOptotrak()
{
  std::cout << "Deactivating markers ...";
  if( OptotrakDeActivateMarkers() )
    {
                std::cout << "failed." << std::endl;
                return 0;
    }
  std::cout << "done." << std::endl;

  // Shutdown the processors message passing system.
  std::cout << "Shutting down Transputer System ... ";
  if( TransputerShutdownSystem() )
    {
                std::cout << "failed." << std::endl;
        return 0;
    }
  std::cout << "done." << std::endl;

  return 1;
}


int GetTransform(float* position, float* quaternion)
{
  unsigned int
   uFlags,
   uElements,
   uFrameCnt,
   uRigidCnt,
   uMarkerCnt,
   uFrameNumber;

  RigidBodyDataType
   RigidBodyData;

  if( DataGetLatestTransforms( &uFrameNumber, &uElements, &uFlags,
                                     &RigidBodyData ) )
    {
      return 0;
    }

  int id = RIGID_BODY_2;
  if (RigidBodyData.pRigidData[id].flags & OPTOTRAK_UNDETERMINED_FLAG)
  {
          // Marker is not detected.
          return 0;
  }
  position[0] = RigidBodyData.pRigidData[id].transformation.quaternion.translation.x;
  position[1] = RigidBodyData.pRigidData[id].transformation.quaternion.translation.y;
  position[2] = RigidBodyData.pRigidData[id].transformation.quaternion.translation.z;

  quaternion[3] = RigidBodyData.pRigidData[id].transformation.quaternion.rotation.q0;
  quaternion[0] = RigidBodyData.pRigidData[id].transformation.quaternion.rotation.qx;
  quaternion[1] = RigidBodyData.pRigidData[id].transformation.quaternion.rotation.qy;
  quaternion[2] = RigidBodyData.pRigidData[id].transformation.quaternion.rotation.qz;

  // Display Marker positions (for debugging)
  /*
  for(int uMarkerCnt = 0; uMarkerCnt < NUM_MARKERS; ++uMarkerCnt )
    {
          DisplayMarker( uMarkerCnt + 1, RigidBodyData.p3dData[uMarkerCnt] );
    }
  */
  return 1;

}


void PrintOptotrakError()
{
  // Indicate that an error has occurred
  fprintf( stdout, "\nAn error has occurred during execution of the program.\n" );
  if( OptotrakGetErrorString( szNDErrorString,
                                MAX_ERROR_STRING_LENGTH + 1 ) == 0 )
    {
        fprintf( stdout, szNDErrorString );
    }
  fprintf( stdout, "\n\n...TransputerShutdownSystem\n" );
  OptotrakDeActivateMarkers();
  TransputerShutdownSystem();
}



bool CheckAndConnect()
{
  ACE_Time_Value timeOut(5,0);
  int ret;
  
  mutex->acquire();  //lock
  if (!connected)
    {
      ret = connector.connect(sock, address, &timeOut);
      if(ret == -1 && errno != ETIME && errno != 0 )
        {
          ACE_DEBUG((LM_ERROR, ACE_TEXT("Error %d : Cannot connect to Slicer Daemon!\n"), errno));
        }
      else
        {
          connected = true;
        }
    }
  mutex->release();  //unlock
  return connected;
}



void Disconnect()
{
  mutex->acquire();  //lock
  if (connected)
    {
      connected = false;
      sock.close();
    }
  mutex->release();  //unlock
}

void ServerMonitor()
{
  int ret;
  char buf[256];
  ACE_Time_Value timeOut(10,0);
  int tbytes;
  
  while (!stop) {
    if (CheckAndConnect()) {
      ret = sock.recv_n(buf, 256, &timeOut, (size_t*)&tbytes);
      if (ret > 0) {
        ACE_DEBUG((LM_ERROR, ACE_TEXT("Received: %s\n"), buf));
      } else if (ret == 0) {
        Disconnect();
      }
      else if (ret == -1 ) {
        if( errno != ETIME && errno != 0 ) {
          ACE_DEBUG((LM_ERROR, ACE_TEXT("ot:Error %d!\n"), errno));
          Disconnect();
        }
        // else -- time out
      }
    }
    else // wait for a while
      {
        ACE_Time_Value timeVal(0, (int)(1000)*1000 );
        ACE_OS::sleep( timeVal );
      }
  }
}


static void ThreadFunc( void * data )
{
  ServerMonitor();
}

int ConnectToSlicer(const char* hostname, int port)
{
  address.set(port, hostname);
  CheckAndConnect();
  
  stop = false;
  thread = new ACE_thread_t;
  ACE_Thread::spawn((ACE_THR_FUNC)ThreadFunc, 
                    NULL, 
                    THR_NEW_LWP | THR_JOINABLE,
                    (ACE_thread_t *)thread );
  return 1;
}


int Session(int intv_ms)
{

  ActivateOptotrak();
  while (1) {
    float position[3];
    float orientation[4];

    GetTransform(position, orientation);
    
    if (connected) {
      // Get position/orientation from the NaviTrack stream and
      // convert them for Slicer Daemon
      igtl::Matrix4x4 matrix;
      igtl::QuaternionToMatrix(orientation, matrix);
      igtl::TransformMessage::Pointer transform = igtl::TransformMessage::New();
      transform->SetDeviceName("Tracker");
      
      matrix[0][3] = position[0];
      matrix[1][3] = position[1];
      matrix[2][3] = position[2];
          
          matrix[3][0] = 0.0;
          matrix[3][1] = 0.0;
          matrix[3][2] = 0.0;
          matrix[3][3] = 1.0;


      PrintMatrix(matrix);
      transform->SetMatrix(matrix);
      transform->Pack();

      int ret;
      ACE_Time_Value timeOut(1,0);
  
      /*ret = sock.send_n(&header, IGTL_HEADER_SIZE, &timeOut); */
      ret = sock.send_n(transform->GetPackPointer(), transform->GetPackSize(), &timeOut);
      transform->Delete();

      if (ret <= 0) {
        Disconnect();
        ACE_DEBUG((LM_ERROR, ACE_TEXT("Error %d : Connection Lost!\n"), errno));
      }
    }

    ACE_Time_Value timeVal(0, (int)(intv_ms)*1000 );
    ACE_OS::sleep( timeVal );
  }
  DeactivateOptotrak();
}


int main(int argc, char **argv)
{

  if (argc < 2) {
    std::cerr << "Invalid arguments!" << std::endl;
    std::cerr << "Usage: " << argv[0] << " <fps> <hostname>" << std::endl;
    std::cerr << "    <fps>           : Frame rate (frames per second)." << std::endl;
    std::cerr << "    <hostname>      : hostname (port# is fixed to 18944)" << std::endl;
    exit(-1);
  }

  float fps  = atof(argv[1]);
  char* host = argv[2];
  int interval_ms = (int) (1000 / fps);

  stop      = true;
  connected = false;
  mutex = new ACE_Thread_Mutex;

  std::cerr << "Connecting to the 3D Slicer..." << std::endl;
  ConnectToSlicer(host, 18944);
  std::cerr << "Starting Session..." << std::endl;
  Session(interval_ms);

  // Stop thread
#ifdef WIN32
  ACE_Thread::join( (ACE_thread_t*)thread );
#else
  ACE_Thread::join( (ACE_thread_t)thread );
#endif
  delete ((ACE_thread_t *)thread);

  Disconnect();
  delete mutex;

}


