/***********************************************************
 * FileName      : Console.h
 * Created       : 2009/01/
 * LastModified  :
 * Author        : Hiroaki KOZUKA
 * Aim           : Key board console
  ***********************************************************/
#ifndef CONSOLE_W__H
#define CONSOLE_W__H

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

#include "Common.h"
#include "InterfaceBase.h"
#include "ConsoleOrder.h"
#include "MessageQData.h"

#ifdef OS__VXWORKS

#else
#include <process.h>
#include <windows.h>
#endif

/// Key board console
class ConsoleW : public InterfaceBase {
private:

  /// Display help messages
  /// \return OK
  int helpMessage();

  MessageQData<Coord_6DoF> P;
  MessageQData<TF_Matrix> TF;
  /// key board handler
  /// \param flag Event
  /// \return Event
  EVENT keyHandler(EVENT flag, std::string& buffer);

  /// Number of order
  int OrderNum;

  /// Order pointer
  ConsoleOrder<EVENT>* Order;

  /// Thread 1
  static THRD_FUNC_RETURN ThreadFunc(void* ptr);

  /// Thread 2
  static THRD_FUNC_RETURN ThreadFunc2(void* ptr);

  THRD_ID tr, tr2;

public:
  /// A constructor
  ConsoleW();

  /// A destructor
  ~ConsoleW();

};

#endif // CONSOLE_W__H

