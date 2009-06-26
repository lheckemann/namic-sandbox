
#ifndef OCCO__H
#define OCCO__H

#include <vector>
#include <iostream>
#include <stdio.h>
#include <string>
#include <math.h>

#include "Common.h"
//#include "../include/Common.h"

#ifdef OS__VXWORKS

#define THRD_FUNC_RETURN int
#define THRD_ID int
#define RT_HZ 1000

#else
#include <process.h>
#include <windows.h>


#define FUNCPTR unsigned int (__stdcall* )(void *)
#define THRD_FUNC_RETURN unsigned __stdcall
#define SEM_ID HANDLE
#define THRD_ID HANDLE

#endif

class occo{
private:
  ///
  static STATE State;
  ///
  static EVENT Event;
  ///
  static SEM_ID Smph;
  ///
  static unsigned long long Time;
  ///
  STATE LocalState;
  ///
  EVENT LocalEvent;
  ///
  SEM_ID LocalSmph;
  ///
  unsigned long long LocalTime;


public:
  ///
  occo();

  ///
  ~occo();

  ///
  static void Init();

  ///
  static void Delete();

  ///
  THRD_ID GetThreadID(char name[]);

  ///
  THRD_ID CreateThread( char name[], const unsigned int ptr, const unsigned int func,
                        const unsigned int prty,void* bkup1, void* bkup2,
                        int bkup3, int bkup4, long bkup5, double bkup6 );

  ///
  int DeleteThread(THRD_ID id);

  ///
  int DeleteAllThread();

  /// t[us]
  static void TaskDelay(unsigned long );

  ///
  static void SetEvent(EVENT e);

  ///
  static void SetState(STATE s);

  ///
  static EVENT GetEvent();

  ///
  static STATE GetState();

  ///
  void SetEvent_L(EVENT e);

  ///
  void SetState_L(STATE s);

  ///
  EVENT GetEvent_L();

  ///
  STATE GetState_L();

  ///
  static const unsigned long long GetTime();

  ///
  static void CountUpTime();

  ///
  const unsigned long long GetTime_L();

  ///
  void CountUpTime_L();

};

#endif // OCCO__H

