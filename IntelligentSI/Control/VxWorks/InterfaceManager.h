/******************************************************************************
 * FileName      : InterfaceManager.h
 * Created       : 2007/11/21
 * LastModified  : 2007/
 * Author        : Hiroaki KOZUKA
 * Aim           : Formanaging interfaces
 *
 * OS            : VxWorks 5.5.1
 *****************************************************************************/
#ifndef INTERFACE_MANAGER__H
#define INTERFACE_MANAGER__H

#include "InterfaceBase.h"
#include "Common.h"
#include "save24.h"
#include "MessageQData.h"

/// For managing interfaces
class InterfaceManager : public InterfaceBase {
private:
  /// The number of joint
  int JointNum;

  MessageQData<Coord_6DoF_N> S_Pee;
  MessageQData<Coord_6DoF_N> R_Pee;
  MessageQData<Coord_6DoF_N> S_curPee;
  MessageQData<Coord_6DoF_N> S_Fee;
  MessageQData<Coord_6DoF_N> R_Fee;
  MessageQData<BUFF_M> S_Buff;
  MessageQData<BUFF_M> R_Buff;

  MessageQData<float[12]> S_TF;
  MessageQData<float[12]> R_TF;

  MessageQData<float[3]> S_CAG;
  MessageQData<float[3]> S_DAG;

  MessageQData<unsigned long long[2]> S_TIME;
  MessageQData<unsigned long long[2]> R_TIME;
  MessageQData<float> S_TIME_T;


  /*
  /// The end effector position
  M_EE_POSITION S_Pee, R_Pee;

  /// The end effector force
  M_EE_FORCE S_Fee, R_Fee;

  /// Event
  M_EVENT S_Event, R_Event;

  /// Transfrom (4x4)
  M_EE_TRANSFORM S_Tf, R_Tf;

  /// The buffer of message queue
  M_BUFF R_Buff;

  /// The destination joint angles
  M_BUFF S_DestAngle, R_DestAngle;

  /// The current joint angles
  M_BUFF S_CurAngle, R_CurAngle;

  M_EE_POSITION_LT S_PosLT, R_PosLT;
  */

  bool* CFlag;
  bool* SFlag;

  std::vector<IFMsgQID>* CIFMsgQID;
  IFMsgQID* SIFMsgQID;
  /// logging class pointer
  SAVE* save;

  InterfaceDataBaseSet IFBDS;

  void FilterMovingAvrg(Coord_6DoF_N& Pee);

  void MovingAvrg(int num, Coord_6DoF_N& data, Coord_6DoF_N* data_);

 public:
  /// A constructor
  InterfaceManager();

  /// A destructor
  ~InterfaceManager();

  /// Initialize
  void Init();

  /// Set number of the joint
  /// \param jNum Number of the joint
  void SetJointNum(int jNum);

  /// Set Message Queue ID for the client interface
  /// \param type Send data type
  /// \param id Interface pointer
  /// \param max Number of max messages
  /// \param fq Send freqency [Hz]
  /// \param qid Mwssage Queue ID
  void SetCMsgQID( CIFDataType type, InterfaceBase* id,
                   int max, double fq, IF_MSG_Q_ID qid );

  /// Set Message Queue ID for the server interface
  /// \param type Receive data type
  /// \param id Interface pointer
  /// \param max Number of max messages
  /// \param fq Receive freqency [Hz]
  /// \param qid Mwssage Queue ID
  void SetSMsgQID( SIFDataType type, InterfaceBase* id,
                   int max, double fq, IF_MSG_Q_ID qid );

  /// Set Message Queue ID for the server interface
  /// \param type Receive data type
  /// \param id Interface pointer
  /// \param max Number of max messages
  /// \param qid Message Queue ID
  void SetSMsgQID( SIFDataType type, InterfaceBase* id,
                   int max, IF_MSG_Q_ID qid );

  /// Receive all data from message queue
  void ReceiveData();

  /// Send all data to message queue
  void SendData();

  /// Send all data to message queue
  void SendcurData(Coord_6DoF& data);

  /// Timestamp send
  void SendTime();

  ///
  void SendTime_T(double t);

  /// Receive position of end effector
  /// \param p Robot tip of position
  void SendData(Coord_6DoF& data);
/*
  /// Receive position of end effector
  /// \param p Robot tip of position
  void SendDataLT(Coord_6DoF& data);
*/
  /// Send current joint angle
  /// \param angle Joint angles
  void SendDestAngle(double* angle );

  /// Send current joint angle
  /// \param angle Joint angles
  void SendCurAngle(double* angle );
/*
  /// Send current joint angle
  /// \param jid Joint ID
  /// \param angle Joint angle
  void SendCurAngle(int jid, double angle );
*/
  /// Send the position of the end effector
  /// \param data position
  void SendDataTf(EE_POSITION& data);

  /// Send force of end effector
  /// \param data Robot tip of force
//  void SendData(Coord_6DoF& data);

  /// Receive position of end effector
  /// \param p Robot tip of position
  void ReceiveData(Coord_6DoF& p);

  /// Receive position of end effector
  /// \param p Robot tip of position
//  void ReceiveDataLT(Coord_6DoF& p);

  /// Read robot force
  /// \param f Robot tip of force
//  void ReceiveData(Coord_6DoF& f);

  /// Read event
  /// \param e Event
  void ReceiveData(EVENT& e);
/*
  /// Set time [ms]
  /// \param t Time
  void SetTime(unsigned long long t);
*/
};

#endif // INTERFACE_MANAGER__H


