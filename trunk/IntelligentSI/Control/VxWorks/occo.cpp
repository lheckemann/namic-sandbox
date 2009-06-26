#include "occo.h"

STATE  occo::State;
EVENT  occo::Event;
SEM_ID occo::Smph;
unsigned long long occo::Time=0;

#ifdef OS__VXWORKS
//-----------------------------------------------------------------------------
//
occo::occo(){
  sysClkRateSet( RT_HZ );
  LocalSmph = semBCreate( SEM_Q_PRIORITY, SEM_FULL );
  LocalTime = 0;
}

//-----------------------------------------------------------------------------
//
occo::~occo(){
  semDelete(LocalSmph);
}

//-----------------------------------------------------------------------------
//
void
occo::Init(){
  sysClkRateSet( RT_HZ );
  Smph =  semBCreate( SEM_Q_PRIORITY, SEM_FULL );
  Time=0;
}

//-----------------------------------------------------------------------------
//
void
occo::Delete(){
  semDelete(Smph);
}

//-----------------------------------------------------------------------------
//
THRD_ID
occo::GetThreadID(char name[]){
   return taskNameToId(name);
}

//-----------------------------------------------------------------------------
//
THRD_ID
occo::CreateThread( char name[], const unsigned int ptr, const unsigned int func,
                    const unsigned int prty, void* bkup1, void* bkup2,
                   int bkup3, int bkup4, long bkup5, double bkup6 ){

  return taskSpawn( name, prty+90, VX_FP_TASK, 80000, (FUNCPTR)func,
                    ptr,0,0,0,0,0,0,0,0,0 );
}

//-----------------------------------------------------------------------------
//
int
occo::DeleteThread(THRD_ID id){
   return taskDelete( id );
}

//-----------------------------------------------------------------------------
//
int
occo::DeleteAllThread(){
  return 0;
}

void
occo::TaskDelay(unsigned long t){
  int tick = int((double)sysClkRateGet()*((double)t/1000000.0));

  if(tick <= 0)
    tick = 1;

  taskDelay( tick );
};


//-----------------------------------------------------------------------------
//
void
occo::SetEvent(EVENT e){
  semTake( Smph, NO_WAIT );
  Event = e;
  semGive( Smph );
}

//-----------------------------------------------------------------------------
//
void
occo::SetState(STATE s){
  semTake( Smph, NO_WAIT);
  State = s;
  semGive( Smph );
}

//-----------------------------------------------------------------------------
//
EVENT
occo::GetEvent(){
  semTake( Smph, NO_WAIT );
  EVENT e = Event;
  semGive( Smph );
  return e;
}

//-----------------------------------------------------------------------------
//
STATE
occo::GetState(){
  semTake( Smph, NO_WAIT );
  STATE s = State;
  semGive( Smph );
  return s;
}

//-----------------------------------------------------------------------------
//
void
occo::SetEvent_L(EVENT e){
  semTake( LocalSmph, NO_WAIT );
  LocalEvent = e;
  semGive( LocalSmph );
}

//-----------------------------------------------------------------------------
//
void
occo::SetState_L(STATE s){
  semTake( LocalSmph, NO_WAIT );
  LocalState = s;
  semGive( LocalSmph );
}

//-----------------------------------------------------------------------------
//
EVENT
occo::GetEvent_L(){
  semTake( LocalSmph, NO_WAIT );
  EVENT e = LocalEvent;
  semGive( LocalSmph );
  return e;
}

//-----------------------------------------------------------------------------
//
STATE
occo::GetState_L(){
  semTake( LocalSmph, NO_WAIT );
  STATE s = LocalState;
  semGive( LocalSmph );
  return s;
}

//-----------------------------------------------------------------------------
//
const unsigned long long
occo::GetTime(){
  semTake( Smph, NO_WAIT );
  unsigned long long  t = Time;
  semGive( Smph );
  return t;
}

//-----------------------------------------------------------------------------
//
void
occo::CountUpTime(){
  semTake( Smph, NO_WAIT );
  Time++;
  semGive( Smph );
}

//-----------------------------------------------------------------------------
//
const unsigned long long
occo::GetTime_L(){
  semTake( LocalSmph, NO_WAIT );
  unsigned long long  t = LocalTime;
  semGive( LocalSmph );
  return t;
}

//-----------------------------------------------------------------------------
//
void
occo::CountUpTime_L(){
  semTake( LocalSmph, NO_WAIT );
  LocalTime++;
  semGive( LocalSmph );
}

#else

//-----------------------------------------------------------------------------
//
occo::occo(){
  LocalTime = 0;
}

//-----------------------------------------------------------------------------
//
occo::~occo(){
}

//-----------------------------------------------------------------------------
//
void
occo::Init(){
}

//-----------------------------------------------------------------------------
//
void
occo::Delete(){
}

//-----------------------------------------------------------------------------
//
THRD_ID
occo::GetThreadID(char name[]){
  return NULL;
}

//-----------------------------------------------------------------------------
//
THRD_ID
occo::CreateThread(char name[], const unsigned int ptr, const unsigned int func,
                   const unsigned int prty, void* bkup1, void* bkup2,
                   int bkup3, int bkup4, long bkup5, double bkup6 ){
  return (THRD_ID)_beginthreadex( NULL, 1000, (FUNCPTR)func,
                                            (void*)ptr, 0,  NULL );
}

//-----------------------------------------------------------------------------
//
int
occo::DeleteThread(THRD_ID id){
  CloseHandle( id );
  return 0;
}

//-----------------------------------------------------------------------------
//
int
occo::DeleteAllThread(){

  return 0;
}

//-----------------------------------------------------------------------------
//
void
occo::TaskDelay(unsigned long t){
  Sleep(t/1000);
}

//-----------------------------------------------------------------------------
//
void
occo::SetEvent(EVENT e){
  WaitForSingleObject( Smph, INFINITE );
  Event = e;
  ReleaseMutex( Smph );
}

//-----------------------------------------------------------------------------
//
void
occo::SetState(STATE s){
  WaitForSingleObject( Smph, INFINITE );
  State = s;
  ReleaseMutex( Smph );
}

//-----------------------------------------------------------------------------
//
EVENT
occo::GetEvent(){
  WaitForSingleObject( Smph, INFINITE );
  EVENT e = Event;
  ReleaseMutex( Smph );
  return e;
}

//-----------------------------------------------------------------------------
//
STATE
occo::GetState(){
  WaitForSingleObject( Smph, INFINITE );
  STATE s = State;
  ReleaseMutex( Smph );
  return s;
}

//-----------------------------------------------------------------------------
//
void
occo::SetEvent_L(EVENT e){
  WaitForSingleObject( LocalSmph, INFINITE );
  LocalEvent = e;
  ReleaseMutex( LocalSmph );
}

//-----------------------------------------------------------------------------
//
void
occo::SetState_L(STATE s){
  WaitForSingleObject( LocalSmph, INFINITE );
  LocalState = s;
  ReleaseMutex( LocalSmph );
}

//-----------------------------------------------------------------------------
//
EVENT
occo::GetEvent_L(){
  WaitForSingleObject( LocalSmph, INFINITE );
  EVENT e = LocalEvent;
  ReleaseMutex( LocalSmph );
  return e;
}

//-----------------------------------------------------------------------------
//
STATE
occo::GetState_L(){
  WaitForSingleObject( LocalSmph, INFINITE );
  STATE s = LocalState;
  ReleaseMutex( LocalSmph );
  return s;
}

//-----------------------------------------------------------------------------
//
const unsigned long long
occo::GetTime(){
  WaitForSingleObject( Smph, INFINITE );
  unsigned long long  t = Time;
  ReleaseMutex( Smph );
  return t;
}

//-----------------------------------------------------------------------------
//
void
occo::CountUpTime(){
  WaitForSingleObject( Smph, INFINITE );
  Time++;
  ReleaseMutex( Smph );
}

//-----------------------------------------------------------------------------
//
const unsigned long long
occo::GetTime_L(){
  WaitForSingleObject( LocalSmph, INFINITE );
  unsigned long long  t = LocalTime;
  ReleaseMutex( LocalSmph );
  return t;
}

//-----------------------------------------------------------------------------
//
void
occo::CountUpTime_L(){
  WaitForSingleObject( LocalSmph, INFINITE );
  LocalTime++;
  ReleaseMutex( LocalSmph );
}

#endif

