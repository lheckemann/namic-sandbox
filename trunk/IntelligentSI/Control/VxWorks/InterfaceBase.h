/******************************************************************************
 * FileName      : InterfaceBase.h
 * Created       : 2008/
 * LastModified  : 2009/
 * Author        : Hiroaki KOZUKA
 * Aim           : Interfaces base
 *
 * OS            : VxWorks 5.5.1
 *****************************************************************************/

#ifndef INTERFACE_BASE__H
#define INTERFACE_BASE__H

#include "igtl_header.h"
#include "Common.h"
#include "occo.h"
#include "InterfaceDataBaseSet.h"

#define MSGQ_HEADER_SIZE 16

/// For interface base
class
InterfaceBase : public occo{
public:
#ifdef OS__VXWORKS
  /// Message Queue ID
  typedef MSG_Q_ID IF_MSG_Q_ID;
#else
  /// Message Queue ID
  typedef int IF_MSG_Q_ID;
#endif


private:
  ///Struct for the infomation of Message Queue ID and so on.
  struct IFMsgQID{
    int MaxNum;          ///< The number of max messeges
    int Fq;              ///< Send/receive freqency
    IF_MSG_Q_ID MID;     ///< Message Queue ID
  };

  ///
  static SEM_ID Sm;

  /// Message queue information
  static std::vector<IFMsgQID>* CIFMsgQID;

  /// Message queue information
  static IFMsgQID*  SIFMsgQID;

  ///
  static InterfaceDataBaseSet sIFDBS;


public:
  /// A constructor
  InterfaceBase();

  /// A destructor
  virtual ~InterfaceBase();


#ifdef OS__VXWORKS

  /// Create message queue
  /// \param maxNum  Max number of messages
  /// \param maxSize Max message size
  /// \return MessageQ ID
  static IF_MSG_Q_ID CreateMsgQ(int maxNum, int maxSize);

  /// Delete message queue
  /// \param id messageQ ID
  static void DeleteMsgQ(IF_MSG_Q_ID id);

  /// Send message queue
  /// \param id   MessageQ ID
  /// \param data Send data
  /// \param size Data size
  /// \param em Emargency ture/false

  static void SendMsgQ( IF_MSG_Q_ID id, char* data, int size, bool em );

  /// Receive message queue
  /// \param id   MessageQ ID
  /// \param data Send data
  /// \param size Data size
  static void ReceiveMsgQ( IF_MSG_Q_ID id, char* data, int size );

  /// Receive message queue
  /// \param id   MessageQ ID
  /// \param data Send data
  /// \param size Data size
  static void ReceiveMsgQ_NW( IF_MSG_Q_ID id, char* data, int size );

  /// Get message queue
  /// \param id MessageQ ID
  /// \return Number of messages
  static int GetMsgQNum(IF_MSG_Q_ID id);

  ///
  static void Init();

  ///
  static void Delete();

  ///
  static void  SetSMsgQID( int type,  int maxNum, int numSize,
                           double fq, IF_MSG_Q_ID qid);

  ///
  static void  SetRMsgQID( int type, int maxNum, int maxSize,
                           double fq, IF_MSG_Q_ID qid );

  ///
  static IFMsgQID GetSMsgQID(int type, int num);

  ///
  static int GetSMsgQIDNum(int type);

  ///
  static IFMsgQID GetRMsgQID(int type);

#else
  ///
  static void Init();

  ///
  static void Delete();

  /// Create message queue
  /// \param maxNum  Max number of messages
  /// \param maxSize Max message size
  /// \return MessageQ ID
  IF_MSG_Q_ID CreateMsgQ(int maxNum, int maxSize);

  /// Delete message queue
  /// \param id messageQ ID
  void DeleteMsgQ(IF_MSG_Q_ID id);

  /// Send message queue
  /// \param id   MessageQ ID
  /// \param data Send data
  /// \param size Data size
  /// \param em Emargency ture/false
  void SendMsgQ( IF_MSG_Q_ID id, char* data, int size, bool em );

  /// Receive message queue
  /// \param id   MessageQ ID
  /// \param data Send data
  /// \param size Data size
  void ReceiveMsgQ( IF_MSG_Q_ID id, char* data, int size );

  /// Get message queue
  /// \param id MessageQ ID
  /// \return Number of messages
  int GetMsgQNum(IF_MSG_Q_ID id);


#endif //OS__VXWROKS
  virtual IF_MSG_Q_ID GetMsgQID();

};

#endif // INTERFACE_BASE__H


