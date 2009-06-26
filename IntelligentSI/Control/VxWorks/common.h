
#ifndef COMMON__H
#define COMMON__H

// For VxWorks
#define OS__VXWORKS

#ifdef OS__VXWORKS
#include <vxWorks.h>
#include <strstream.h>
#include <fstream.h>
#include <iomanip.h>
#include <taskLib.h>
#include <kernelLib.h>
#include <stdio.h>
#include <stdlib.h>
#include <semLib.h>
#include <sysLib.h>
#include <usrLib.h>
#include <msgQLib.h>
#include <logLib.h>
#include <intLib.h>
#include <ioLib.h>
#include <pipeDrv.h>
#include <vxLib.h>
#include <math.h>
#include <memLib.h>
#include <string.h>
#include <wdLib.h>
#include <tickLib.h>

#else

#endif

#include "igtl_header.h"

///  PI
#define PI 3.1415926535897932384626433832795

/// Square
#define sq(x) ((x)*(x))

/// Device name
#define ROBOT_NAME "jawa"

/// Server/Client
enum IF_TYPE{
  SERVER = 1,
  CLIENT = 2
};


/// Event
typedef enum EVENT{
  E_UNKNOWN = -1,    ///< Unknown event
  E_PRGM_START,      ///< Start program
  E_PRGM_EXIT_,      ///< Exit program
  E_EMERGENCY_0,     ///< Emergency 0
  E_EMERGENCY_1,     ///< Emergency 1
  E_EMERGENCY_2,     ///< Emergency 2
  E_PRGM_EXIT,       ///< Exit program
  E_INIT,            ///< Initialize
  E_CTRL_STOP_START, ///< Start control stop mode
  E_CTRL_STOP_STOP,  ///< Stop control stop mode
  E_CTRL_RUN_START,  ///< Start control mode
  E_CTRL_RUN_STOP    ///< Stop control mode
};

/// State
typedef enum STATE{
  S_UNKNOWN = -1,  ///< Unknown state
  S_PRGM_ENTRANCE, ///< Program entrance
  S_CTRL_READY,    ///< Ready control
  S_EMERGENCY_0,   ///< Emergency 0
  S_EMERGENCY_1,   ///< Emergency 1
  S_EMERGENCY_2,   ///< Emergency 2
  S_PRGM_EXIT,     ///< Exit program
  S_INIT,          ///< Initialize
  S_CTRL_STOP,     ///< Control stop mode
  S_CTRL_RUN       ///< Control mode
};


/// Server interaface data type
enum SIFDataType{
  S_POSITION = 0,   ///< End effector position
  S_FORCE,          ///< End effector force
  S_EVENT,          ///< Event
  S_TRANSFORM,      ///< Transform
  S_POSITION_LT,
  SIF_DATA_TYPE_NUM ///< Number of data type
};

/// Client interaface data type
enum CIFDataType{
  C_POSITION = 0,   ///< End effector position
  C_FORCE,          ///< End effector force
  C_EVENT,          ///< Event
  C_TRANSFORM,      ///< Transform
  C_DEST_ANGLE,     ///< Destination joint angles
  C_CUR_ANGLE,      ///< Current joint angles
  C_POSITION_LT,
  C_POSITION_LT_M,
  CIF_DATA_TYPE_NUM ///< Number of data type
};


struct Coord_6DoF{
  double x;     ///< coordinate x
  double y;     ///< coordinate y
  double z;     ///< coordinate z
  double alpha; ///< lotation alpha
  double beta;  ///< lotation beta
  double gamma; ///< lotation gamma
};

struct Coord_6DoF_N{
  float x;     ///< coordinate x
  float y;     ///< coordinate y
  float z;     ///< coordinate z
  float alpha; ///< lotation alpha
  float beta;  ///< lotation beta
  float gamma; ///< lotation gamma
};

/// Robot end effector position
typedef struct{
  unsigned long long Time; // latency
  double x;     ///< coordinate x
  double y;     ///< coordinate y
  double z;     ///< coordinate z
  double alpha; ///< lotation alpha
  double beta;  ///< lotation beta
  double gamma; ///< lotation gamma
}EE_POSITION_LT;

typedef float  TF_Matrix_N[12];
typedef double TF_Matrix[12];

typedef Coord_6DoF EE_POSITION;
typedef Coord_6DoF EE_FORCE;

typedef Coord_6DoF_N EE_POSITION_N;
typedef Coord_6DoF_N EE_FORCE_N;

/// Sphere header (for OpenIGTLink)
typedef struct {
    unsigned short v;   ///< Version
    unsigned short n;   ///< Number of sphere
    double h;           ///< Hardness
}SPHERE_HEADER;

/// Sphere data (for OpenIGTLink body)
typedef struct {
    double x; ///< Position x
    double y; ///< Position y
    double z; ///< Position z
    double r; ///< Radius
}SPHERE_N_B;

typedef struct {
    unsigned long long Time;
    float x;
    float y;
    float z;
    float alpha;
    float beta;
    float gamma;
}EE_POSITION_LT_N_B;


#ifdef OS__VXWORKS
/// Buffer for Socket
  typedef struct {
    igtl_header Header; ///< OpenIGTLink header
    char Body[1024];    ///< Buffer
  } _WRS_PACK_ALIGN(1) BUFF_N;

  /// Message queue header
  typedef struct{
    int Type;                 ///< Data type
    unsigned long long Time;  ///< Time stamp
    int JNum;                 ///< The Number of joints
  } MSGQ_HEADER;

  /// Buffer for the meaaege queue
  typedef struct{
    MSGQ_HEADER Header; ///< Message queue header
    char Body[1024];    ///< Buffer
  } _WRS_PACK_ALIGN(1)  BUFF_M;

  /// Interface state
  typedef struct{
    int Type;
    bool IFSockErr;
    bool IFSockSizeErr;
    bool MsgQNumErr;
    bool MsgQSizeErr;
    bool MsgQOtherErr;
  } _WRS_PACK_ALIGN(1) IF_STATE;
#else

  /// Buffer for Socket
  #pragma pack(1)
  typedef struct {
    igtl_header Header; ///< OpenIGTLink header
    char Body[1024];    ///< Buffer
  }BUFF_N;
  #pragma pack()

  /// Message queue header
  typedef struct{
    int Type;                 ///< Data type
    unsigned long long Time;  ///< Time stamp
    int JNum;                 ///< The Number of joints
  } MSGQ_HEADER;

  /// Buffer for the meaaege queue
  #pragma pack(1)
  typedef struct{
    MSGQ_HEADER Header; ///< Message queue header
    char Body[1024];    ///< Buffer
  } BUFF_M;
  #pragma pack()

  /// Interface state
  struct IF_STATE{
    bool IFSockErr;
    bool IFSockSizeErr;
    bool MsgQNumErr;
    bool MsgQSizeErr;
    bool MsgQOtherErr;
  };

#endif


#endif // COMMON__H

