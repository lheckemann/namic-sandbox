/* 
 * common define and strcuture etc.
 */

#ifndef COMMON__H
#define COMMON__H

#include <vxWorks.h>
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
#include <iomanip>
#include <math.h>
#include <memLib.h>
#include <string.h>
#include <iostream.h>
#include <strstream.h>
#include <fstream.h>

#include "drv/pci/pciConfigLib.h"
#include "mcpx800.h"
#include "/usr/local/tornado.ppc/target/config/mcp820/config.h"



#define SERVER 1
#define CLIENT 2

#define IN  1
#define OUT 0

#define ON  1
#define OFF 0

#define START 1
#define STOP  0

#define SLAVE    "SurgicalMotionBase"  // string length must be under 20
#define MASTER  "TypeLambda"

#define MASTER_TO_SLAVE_PORT  50000
#define SLAVE_TO_MASTER_PORT  50010

#define PHANTOM_TO_SLAVE_PORT  20000
#define SLAVE_TO_PHANTOM_PORT  20001

#define revl  360
#define linr    1
#define plus    1
#define minus  -1
#define PI  3.1415926535897932384626433832795

//namespace ctcs{

enum DataConfig{
  ARM_CONF,
  JOINT_NUM_CONF,
  JOINT_CTRL_CONF,
  JOINT_ENC_CONF,
  JOINT_SENSOR_CONF,
  DRIVER_NUM_CONF,    // Number of driver class and each borad
  DRIVER_DA_CONF,
  DRIVER_AD_CONF,
  DRIVER_CNT_CONF
};

enum DataKind{
  DEST_EE_POSITION,
  DEST_TRANSFORM,
  DEST_EE_FORCE,
  DEST_JOINT_ANGLES,
  DEST_JOINT_TORQUES,
  CUR_TIME,
  CUR_EE_POSITION,
  CUR_TRANSFORM,
  CUR_EE_FORCE,
  CUR_JOINT_ANGLES,
  CUR_JOINT_TORQUES,
  EMERGENCY_CMMAND,
  COMMAND
};

//!
typedef struct{
    double maxVolt;     //!< D/A output max volt
    double minVolt;     //!< D/A output min volt
    double Vo;          //!< D/A output min volt
    double speedtoVolt; //!< 
    double revl_linear; //!< 
    double direction;   //!< 
    double pulseNum;    //!< Encoder pulse Number per revolution
}HARDWARE_DATA;

//!
typedef struct{
    double maxVolt;     //!< D/A output max volt
    double minVolt;     //!< D/A output min volt
    double Vo;          //!< 
    double speedtoVolt; //!< 
    double revl_linear; //!< 
    double direction;   //!< 
    double pulseNum;    //!< Encoder pulse Number per revolution
}HARDWARE_DATA_;


//! PID contorl palametor
typedef struct {
    double Ts;  //!< Sampling time
    double Kp;  //!< Proportion gain
    double Ti;  //!< Integral time
    double Td;  //!< Differential time
    double Kv;  //!< Velocity feed Foward gain
    double e;   //!< Current error
    double e_1; //!< Before error1
    double e_2; //!< Before error2
    double de;  //!< Delta current error    
    double de_; //!< Delta before error
    double y;   //!< Out value
    double y_;  //!< Before Out value
    double dy;  //!< Delta Out value
    double u_;  //!< Before destination value
    double v;   //!< destination velocity
    double direction;  //!< Revolution delection
}PID_DATA;

//!for socket Interface (UDP/IP)
typedef struct{
    int outPortNum;   //!< out port number
    int inPortNum;     //!< in port number
    char* clientName;  //!< client name
  char* serverName; //!< server name
    int serverClient;  //!< server/client 
    //int buffSize;       //!< buffer size
    int packetSize;   //!< packet size
    int packetNum;   //!< packet number
}IF_DATA;

typedef struct{
    double x;
    double y;
    double z;
    double alpha;
    double beta;
    double gamma;
    double x_;
    double y_;
    double z_;
    double alpha_;
    double beta_;
    double gamma_;
}EE_POSITION_;

typedef struct{
    double x;
    double y;
    double z;
}EE_DIRECTION;

typedef struct{
    double x;
    double y;
    double z;
    double alpha;
    double beta;
    double gamma;
  void init(){
    x = y = z = 0;
    alpha = beta = gamma = 0;
  }
}EE_POSITION;

typedef struct{
    float x;
    float y;
    float z;
    float alpha;
    float beta;
    float gamma;
  void init(){
    x = y = z = 0;
    alpha = beta = gamma = 0;
  }
}EE_POSITION_S;

//
typedef struct{
    EE_POSITION_ Pee_P;
    EE_DIRECTION Pee_D;
    EE_POSITION_ Pee_V;
    EE_POSITION  Pee_A;
}ARM_DATA;

//
typedef struct{
    double angle;
    double angle_;
    double speed;
    double speed_;
    double accel;
}JOINT_DATA;

//
typedef struct{
    double outVolt;
    double inVolt;
    long paluse;
    int  HWStat;
}DRIVER_STAT_DATA;

typedef struct {
    double Kp;  //!< Proportion gain
    double Ti;  //!< Integral time
    double Td;  //!< Differential time  
}PID_DEFAULT_DATA;

//! for motor test
typedef struct{
    static const int allSin = 1;
    static const int SinCos = 2;
    static const int step   = 3; 
    static const int  aaa   = 4;
    double time;
    int Ts;
    int endtime;
    int sinTsNum;
    int cosTsNum;
    int sinEndTsNum;
    int cosEndTsNum;
}DEG_COMPUTE_DATA;

#if defined OS__64__BIT
struct IGTL_HEADER{
  unsigned short V;
  char TYPE[8];
  char DEVICE_NAME[20];
    unsigned long TIME_STAMP;
    unsigned long BODY_SIZE;
    unsigned long CRC;
};
#else
struct IGTL_HEADER{
  unsigned short V;
  char TYPE[8];
  char DEVICE_NAME[20];
    unsigned long long TIME_STAMP;
    unsigned long long BODY_SIZE;
    unsigned long long CRC;
};
#endif

typedef struct{
    IGTL_HEADER   header;
    EE_POSITION_S body;
}IGTL_POSITION;

/*
typedef struct{
  int index,sw;
  double scale;
  double x;
  double y;
  double Fx;
  double Fy;
}MASTER_PHANTOM_DATA;
*/

struct IGTL_PHANTOM{
  struct IGTL_HEADER header;
  EE_POSITION_S Pee;
  void init(){
    header.V = 0;
    for(int i=0; i<8; i++){
      header.TYPE[i] = 0;
    }
    for(int i=0; i<20; i++){
      header.DEVICE_NAME[i] = 0;
    }
    header.TIME_STAMP = header.BODY_SIZE = header.CRC = 0;
    Pee.init();
  }

  void init( unsigned short v,
         char *type,
         char *device_name,
#if defined OS__64__BIT
         unsigned long time_stamp,
         unsigned long body_size,
         unsigned long crc){
#else
         unsigned long long time_stamp,
         unsigned long long body_size,
         unsigned long long crc){
#endif
    header.V = v;
    for(int i=0; i<8; i++){
      header.TYPE[i] = type[i];
    }
    for(int i=0; i<20; i++){
      header.DEVICE_NAME[i] = device_name[i];
    }
    header.TIME_STAMP = time_stamp;
    header.BODY_SIZE = body_size;
    header.CRC = crc;
    Pee.init();
  }
};


typedef enum EVENT{
  E_UNKNOWN,
  E_PRGM_EXIT_,
  E_EMERGENCY_0,
  E_EMERGENCY_1,
  E_EMERGENCY_2,
  E_PRGM_EXIT,
  E_PRGM_START,
  E_CTRL_STOP_START,
  E_CTRL_STOP_STOP,
  E_CTRL_RUN_START,
  E_CTRL_RUN_STOP
};

typedef enum STATE{
  S_UNKNOWN,
  S_EMERGENCY_0,
  S_EMERGENCY_1,
  S_EMERGENCY_2,
  S_PRGM_EXIT,
  S_PRGM_ENTRANCE,
  S_CTRL_READY,
  S_CTRL_STOP,
  S_CTRL_RUN
};


typedef struct{
  STATE state;
  STATE state_1;
  EVENT event;
}STATE_MACHINE;


typedef double TRANSFORM[4][4];

//} // End of namespace ctcs
 
#endif //COMMON__H

