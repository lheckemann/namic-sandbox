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

//namespace ctcs{

#define VX__DRIVER
//#define ART__DRIVER

//#define FORCE__CTRL
#define POSITION__CTRL

#define SERVER 1
#define CLIENT 2

#define IN  1
#define OUT 0

#define ON  1
#define OFF 0

//#define OK  1
//#define NG -1

#define START 1
#define STOP  0

//#define READ  1
//#define WRITE 0

#define JOINT_OUT   0
#define JOINT_IN    1
#define DRIVER_K    2
#define ARM_K       3
#define POSITION_K  4

#define MASTER_TO_SLAVE_PORT  50000
#define SLAVE_TO_MASTER_PORT  50010

#define PHANTOM_TO_SLAVE_PORT  20000
#define SLAVE_TO_PHANTOM_PORT  20001

#define revl  360
#define linr    1
#define plus    1
#define minus  -1
#define PI  3.1416

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
    int outPortNum; //!< out port number
    int inPortNum;  //!< in port number
    char* hostName; //!< server/client name
    char* myName;
    int S_C;        //!< server/client  1/2
    int buffSize;   //!< buffer size
    int packetSize; //!< packet size
    int packetNum;  //!< packet number
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


typedef struct{
    int state;
    int state_1;
    int event;
}STATE_MACHINE;

typedef struct{
    static const int UNKNOWN = -1;
    static const int EMERGENCY_0     = 0;
    static const int EMERGENCY_1     = 1;
    static const int EMERGENCY_2     = 2;
    static const int PRGM_EXIT       = 100;
    static const int PRGM_START      = 101;
    static const int CTRL_STOP_START = 102;
    static const int CTRL_STOP_STOP  = 103;
    static const int CTRL_RUN_START  = 104;
    static const int CTRL_RUN_STOP   = 105;
}EVENT;

typedef struct{
    static const int UNKNOWN = -1;
    static const int EMERGENCY_0   = 0;
    static const int EMERGENCY_1   = 1;
    static const int EMERGENCY_2   = 2;
    static const int PRGM_EXIT     = 100;
    static const int PRGM_ENTRANCE = 101;
    static const int CTRL_READY    = 102;
    static const int CTRL_STOP     = 103;
    static const int CTRL_RUN      = 104;
}STATE;

typedef double TRANSFORM[4][4];

//}//End of namespace ctcs
 
#endif COMMON__H

