/******************************************************************************
 * FileName      : Device.h
 * Created       : 2008/11/29
 * LastModified  : 2008//
 * Author        : Hiroaki KOZUKA
 * Aim           : device class
 *****************************************************************************/
#ifndef DEVICE__H
#define DEVICE__H

#include <math.h>
#include <iostream>

#include "Common.h"
#include "occo.h"

/// Board base class
class BoardBase;

//-----------------------------------------------------------------------------
/// Device base class
class DeviceBase : public occo {
private:
  /// Numver of device
  int DvcNum;

public:
  /// A constructor
  DeviceBase();

  ///
  struct Board{
    BoardBase* BrdPtr;
    int UnitNum;
  };

  /// A destructor
  virtual ~DeviceBase();

  /// Input to device
  virtual void Read();

  /// Output to device
  virtual void Write();

  /// Set data to device
  /// \param id Device ID
  /// \param data Set data
  virtual void Set(int id, double data);

  /// Get data from device
  /// \param id Device ID
  /// \return data
  virtual double Get(int id);

};

//-----------------------------------------------------------------------------
/// Encoder class
class Encoder : public DeviceBase{
 private:
  /// Encoder parameter struct
  struct EncParam{
    int ID;        ///< Encoder ID
    long PluseNum; ///< Number of pluse [num/r]
    int Direct;    ///< Direction (1 or -1)
  };

  /// Angle [rad]
  double* Angle;

  /// Encoder pluse
  long**  Pluse;

  /// Not available data
  long NaData;

  /// Speed radio (Gear radio)
  double* SpeedRadio;

  /// Number of encoder pluse  [num/r]
  double* PluseNum;

  /// Revolution direction (1 or -1)
  double* Direct;

  /// Number of device
  int Num;

  /// Baord information
  DeviceBase::Board* Brd;

  /// Number of board
  int BrdNum;

 public:
  /// A constructor
  Encoder();

  /// A destructor
  ~Encoder();

  /// Set number of  device
  /// \param num Number of encoder
  void SetDvcNum(int num);

  /// Initialize parameter
  /// \param num Device id
  /// \param direct Direction
  /// \param pluse number of encoder paluse
  /// \param spdr Speed Radio
  bool InitParam(int num, int direct, long pluse, double spdr );

  /// Angle output from encoder
  void Read();

  /// Angle input into encoder
  void Write();

  /// Set angle
  /// \param id Device ID
  /// \param angle Input angle
  void Set(int id, double angle);

  /// Get anlge
  /// \param id Device ID
  /// \return Output angle
  double Get(int id);

  /// Connect board and device
  /// \param id Device ID
  /// \param ptr Data pointer from board
  void Connect(int id, long* ptr);

  ///
  void SetBoardNum(int brdNum);

  ///
  void SetBoard(int id, BoardBase* ptr, int unitNum);

};




//-----------------------------------------------------------------------------
/// Motor class (For toruqe output)
class Motor : public DeviceBase{
 private:
  /// Motor information
  struct MotorData{
    double Torque;       ///< Input torque
    double* Volt;        ///< Output voltage
    double* OutDirect;   ///< Direction
    double Direct;       ///< Direction (1 or -1)
    double MaxVolt;      ///< Max voltage of DA board
    double MinVolt;      ///< Min voltage of DA board
    double TorqueToVolt; ///< Constant of Converting torque to voltage
    double SpeedRadio;   ///< Speed radio
  };

  /// motor information struct
  MotorData* MtData;

  /// Number of device
  int Num;

  /// Not available data
  double NaData;

  /// Regulate volt
  void RegulatVolt1(int id, double& volt, double& direct);

  /// Regulate volt
  void RegulatVolt2(int id, double& volt);

  /// Baord information
  DeviceBase::Board* Brd;

  /// Number of board
  int BrdNum;

public:
  /// A constructor
  Motor();

  /// A destructor
  ~Motor();

  /// Set number of  device
  /// \param num number of motor
  void SetDvcNum(int num);

  /// Connect motor volatage ouput and board
  /// \param id
  /// \param ptr1 Voltage ouput port
  /// \param ptr2 Direction ouput port
  void Connect(int id, double* ptr1, double* ptr2);

  /// Connect motor volatage ouput and board
  /// \param id Device ID
  /// \param ptr Voltage ouput port
  void Connect(int id, double* ptr);

  /// Initialize parameter
  /// \param id  Device ID
  /// \param dir Direction
  /// \param max Max voltage
  /// \param min Min voltage
  /// \param t2v Constant of Converting torque to voltage
  /// \param spdr Speed radio
  void InitParam( int id, double dir, double max, double min, double t2v,
                  double spdr);

  /// Input torque to motor
  void Write();

  /// Set each motor toruqe
  /// \param id Device ID
  /// \param torque Set torque
  void Set(int id, double torque);

  ///
  void SetBoardNum(int brdNum);

  ///
  void SetBoard(int id, BoardBase* ptr, int unitNum);

};


//-----------------------------------------------------------------------------
/// Switch class
class SW : public DeviceBase{
private:
  /// Switch data
  bool* Data[3];

  /// Board data
  bool** DataPtr[3];

  /// Switch counter
  long* CntSW[2];

  /// Not available data
  bool NaData;

  /// Number of ch 1
  int ChNum1;

  /// Number of ch 2
  int ChNum2;

  /// Number of ch 3
  int ChNum3;

  /// Baord information
  DeviceBase::Board* Brd;

  /// Number of board
  int BrdNum;

public:
  /// A constructor
  /// \param chNum1 Number of ch1
  /// \param chNum2 Number of ch2
  /// \param chNum3 Number of ch3
  SW(int chNum1, int chNum2, int chNum3);

  /// A destrcutor
  ~SW();

  /// Set number of  device
  /// \param num Number of SW
  void SetDvcNum(int num);

  /// Connect data
  /// \param type ch1, ch2, ch3
  /// \param ch ch
  /// \param ptr Data pointer form board
  void Connect(int type, int ch, bool* ptr);

  /// on/off
  /// \param ch ch
  bool SW1(int ch);

  /// on/off
  /// \param ch ch
  bool SW2(int ch);

  /// on/off
  /// \param ch ch
  bool SW3(int ch);

  ///
  void SetBoardNum(int brdNum);

  ///
  void SetBoard(int id, BoardBase* ptr, int unitNum);

};


#endif //DEVICE__H


