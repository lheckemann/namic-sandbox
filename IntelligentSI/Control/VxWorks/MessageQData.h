/******************************************************************************
 * FileName      : InterfaceData.h
 * Created       : 2009/2/17
 * LastModified  : 200/
 * Author        : Hiroaki KOZUKA
 * Aim           : Formanaging interfaces
 *
 * OS            : VxWorks 5.5.1
 *****************************************************************************/
#ifndef MESSAGE_Q_DATA__H
#define MESSAGE_Q_DATA__H


#include "InterfaceDataBaseSet.h"
#include "Common.h"

/// For interface dataes
template <typename T>
class MessageQData : public InterfaceDataBaseSet {
private:

#ifdef OS__VXWORKS
  /// Message queue data
  typedef struct{
    MSGQ_HEADER Header;  ///< Message Q header
    T Body;                             ///< template data
  } _WRS_PACK_ALIGN(1) M_Data;
#else
  #pragma pack(1)
  /// Message queue data
  typedef struct{
    MSGQ_HEADER Header;  ///< Message Q header
    T Body;                             ///< template data
  }M_Data;
  #pragma pack()
#endif

  /// Data for MessageQ
  M_Data data;

  /// Interface type (SERVER/CLIENT)
  IF_TYPE IFType;

public:

  /// A constructor
  MessageQData(){
    this->Init();
  }

  /// A destructor
  ~MessageQData(){
  }

  /// Initialize
  void Init(){
    memset(&data, 0, sizeof(M_Data) );
    data.Header.Type = -1;
  }

  /// Set Number of Joint
  /// \param j Number of Joint
  void SetJointNum(int j){
    data.Header.JNum = j;
  }

  /// Get Number of Joint
  /// \return j Number of Joint
  int GetJointNum(){
    return data.Header.JNum;
  }

  /// Set Time
  /// \param t Time [ms]
  void SetTime(unsigned long long  t){
    data.Header.Time = t;
  }

  /// Get Time
  /// \return t Time [ms]
  unsigned long long  GetTime(){
    return data.Header.Time;
  }

  /// Get pointer
  /// \return data pointer
  M_Data& Data(){
    return data;
  }

  /// Get data body pointer
  /// \return data body pointer
  T& Body(){
    return data.Body;
  }

  /// Set body data
  /// \param d data body pointer
  void SetBody(const T* d ){
    data.Body = *d;
  }

  /// Get data header pointer
  /// \return Data header pointer
  MSGQ_HEADER& Header(){
    return data.Header;
  }

  /// Set header
  /// \param h Header
  void SetHeader(const MSGQ_HEADER& h){
    data.Header = *h;
  }

  /// Get data size
  /// \return Size
  int DataSize(){
    return sizeof(M_Data);
  }

  /// Get data body size
  /// \return Body size
  int BodySize(){
    return ( sizeof(M_Data) - MSGQ_HEADER_SIZE );
  }

  /// Get Interface type (SERVER/CLIENT)
  /// \return SERVER/CLIENT
  IF_TYPE GetIFType(){
    return IFType;
  }

  /// Get data type
  /// \return Data type ( Error:-1, 0~ )
  int GetDataType(){
    return data.Header.Type;
  }

  /// Set data information
  /// \param iftype interface type (SERVER/CLIENT)
  /// \param name Data name
  void SetInfo(IF_TYPE iftype, char name[] ){
    IFType = iftype;
    if(IFType == SERVER)
      data.Header.Type = GetRDType(name);
    else if(IFType == CLIENT)
      data.Header.Type = GetSDType(name);
  }

  /// Get data name
  /// \return Data name pointer
  const char* GetDataName(){
    if(IFType == SERVER)
      return GetRDName(data.Header.Type);
    else if(IFType == CLIENT)
      return GetSDName(data.Header.Type);
    else
      return NULL;
  }

};

#endif // MESSAGE_Q_DATA__H


