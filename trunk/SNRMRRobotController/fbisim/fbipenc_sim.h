/* ***************************************************************
   fbipenc.h
   ---------------------------------------------------------------
   Version 1.10-02
   ---------------------------------------------------------------
   Date September 24, 2002
   ---------------------------------------------------------------
   Copyright 2002 Interface Corporation. All rights reserved.
   *************************************************************** */

#if !defined( _FBIPENC_H_ )
#define _FBIPENC_H_

#ifdef __cplusplus
extern "C" {
#endif


// -----------------------------------------------------------------------
//      Callback routine
// -----------------------------------------------------------------------
typedef void (PENCCALLBACKEX)(int nChannel, unsigned long ulEvent, unsigned long ulUser);
typedef PENCCALLBACKEX *LPPENCCALLBACKEX;


// -----------------------------------------------------------------------
//      Open flag identifier
// -----------------------------------------------------------------------
#define PENC_FLAG_NORMAL                0x0000          // Exclusive open
#define PENC_FLAG_SHARE                 0x0002          // Shared open


// -----------------------------------------------------------------------
//      Error identifiers
// -----------------------------------------------------------------------
#define PENC_ERROR_SUCCESS                                      0x00000000      // The operation is successfully completed.
#define PENC_ERROR_NOT_DEVICE                           0xC0000001      // Failed to find the specified device.
#define PENC_ERROR_NOT_OPEN                                     0xC0000002      // The system cannot open the device.
#define PENC_ERROR_INVALID_DEVICE_NUMBER        0xC0000003      // The device number is invalid.
#define PENC_ERROR_ALREADY_OPEN                         0xC0000004      // The device has been already opened.
#define PENC_ERROR_NOT_SUPPORTED                        0xC0000009      // The function is not supported.

#define PENC_ERROR_INITIALIZE_IRQ                       0xC0001000      // Failed to initialize the interrupt function.
#define PENC_ERROR_INVALID_CHANNEL                      0xC0001001      // The specified channel number is invalid.
#define PENC_ERROR_INVALID_MODE                         0xC0001002      // Invalid mode is specified.
#define PENC_ERROR_INVALID_DIRECT                       0xC0001003      // Invalid direction is specified.
#define PENC_ERROR_INVALID_EQUALS                       0xC0001004      // Invalid flag is specified.
#define PENC_ERROR_INVALID_LATCH                        0xC0001005      // Invalid latch is specified."
#define PENC_ERROR_INVALID_COUNTER                      0xC0001006      // The specified counter value is invalid
#define PENC_ERROR_INVALID_COMPARATOR           0xC0001007      // The specified comparator reference value is invalid.
#define PENC_ERROR_INVALID_ZMODE                        0xC0001008      // The specified polarity of phase Z,
                                                                                                                // counter clear condition, and counter latch condition are invalid
#define PENC_ERROR_INVALID_MASK                         0xC0001009      // The specified event mask is invalid.
#define PENC_ERROR_INVALID_ITIMER                       0xC000100A      // The specified interval timer count is invalid.
#define PENC_ERROR_ALREADY_REGISTRATION         0xC000100B      // The event has already been registered.
#define PENC_ERROR_ALREADY_DELETE                       0xC000100C      // The event has already been deleted.
#define PENC_ERROR_MEMORY_NOTALLOCATED          0xC000100D      // Not enough memory    
#define PENC_ERROR_MEMORY_FREE                          0xC000100E      // Failed to release memory.    
#define PENC_ERROR_DRVCAL                                       0xC0001010      // Failed to call the driver.
#define PENC_ERROR_NULL_POINTER                         0xC0001011      // A NULL pointer is passed between the driver and DLL.
#define PENC_ERROR_PARAMETER                            0xC0001012      // The parameter is invalid.


// -----------------------------------------------------------------------
//      API Function identifiers
// -----------------------------------------------------------------------
int PencOpen(int nDevice, unsigned long fulFlags);
int PencClose(int nDevice);
int PencSetMode(int nDevice, int nChannel, unsigned int nMode,
                                int nDirection, int nEqual, int nLatch );
int PencGetMode(int nDevice, int nChannel, unsigned int *pnMode,
                                int *pnDirection, int *pnEqual, int *pnLatch );
int PencSetCounter(int nDevice, int nChannel, unsigned long  ulCounter );
int PencGetCounter(int nDevice, int nChannel, unsigned long *pulCounter );
int PencSetCounterEx(int nDevice, unsigned int nChSel, unsigned long *pulCounter );
int PencGetCounterEx(int nDevice, unsigned int nChSel, unsigned long *pulCounter );
int PencSetComparator(int nDevice, int nChannel, unsigned long  ulComparator );
int PencGetComparator(int nDevice, int nChannel, unsigned long *pulComparator);
int PencSetZMode(int nDevice, int nChannel, unsigned int nZMode );
int PencGetZMode(int nDevice, int nChannel, unsigned int *pnZMode );
int PencEnableCount(int nDevice, unsigned int nChSel, int nEnable );
int PencGetStatus(int nDevice, int nChannel, unsigned int *pnStatus);
int PencGetStatusEx(int nDevice, unsigned int nChSel, unsigned long *pulCounter,
                                        unsigned long *pulStatus );
int PencReset(int nDevice, int nChannel );
int PencSetEventMask(int nDevice, int nChannel, int nEventMask, int nTimerMask);
int PencGetEventMask(int nDevice, int nChannel, int *pnEventMask, int *pnTimerMask);
int PencSetEventEx(int nDevice, LPPENCCALLBACKEX lpEventProcEx, unsigned long ulUser);
int PencKillEvent(int nDevice);
int PencSetTimerConfig(int nDevice, unsigned char ucTimerConfig);
int PencGetTimerConfig(int nDevice, unsigned char *pucTimerConfig);
int PencGetTimerCount(int nDevice, unsigned char *pucTimerCount);

#ifdef __RTL__
int PencOpenEx(unsigned short uDeviceID, unsigned short uSubsystemID,
                           unsigned char ucBoardID, int *pnDevice, unsigned long fulFlags);
#endif


#ifdef __cplusplus
}
#endif

#endif // _FBIPENC_H_


