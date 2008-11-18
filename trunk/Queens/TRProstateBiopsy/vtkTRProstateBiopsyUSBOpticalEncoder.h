/*=auto=========================================================================

  Portions (c) Copyright 2007 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: $
  Date:      $Date: $
  Version:   $Revision: $

=========================================================================auto=*/
// .NAME vtkTRProstateBiopsyUSBOpticalEncoder - provides access to USB optical encoder
// .SECTION Description
// vtkTRProstateBiopsyUSBOpticalEncoder provides communication to USB optical encoder

#ifndef __vtkTRProstateBiopsyUSBOpticalEncoder_h
#define __vtkTRProstateBiopsyUSBOpticalEncoder_h

/// This is a MS Windows specific module to access the USB driver
#ifdef _WIN32
#include <windows.h>
#else /* _WIN32 */
typedef int HANDLE;
#endif /* _WIN32 */
#include "vtkTRProstateBiopsyWin32Header.h"


class vtkTRProstateBiopsyGUI;

class VTK_TRPROSTATEBIOPSY_EXPORT vtkTRProstateBiopsyUSBOpticalEncoder
{
public:
    static vtkTRProstateBiopsyUSBOpticalEncoder *New();

    // Description: 
    // Get/Set GUI
    vtkTRProstateBiopsyGUI* GetGUI(){return this->GUI;};
    virtual void SetGUI(vtkTRProstateBiopsyGUI *gui){ this->GUI = gui;};

    // Description:
    // Get device handle
    HANDLE GetDeviceHandle(){return this->USBDeviceHandle;};

    // Description:
    // Open/close device connection; return value tells whether it succeeded or failed
    bool OpenUSBdevice(void);
    void CloseUSBdevice(void);

    // Description
    // Get channel value 
    int GetChannelValue(int nChannel);

    // Description
    // Get channel max value 
    int GetChannelMaxValue(int nChannel);

    // Description
    // Set channel max value  to zero
    bool SetChannelValueToZero(int nChannel);


protected:
    
    vtkTRProstateBiopsyUSBOpticalEncoder(void);
    ~vtkTRProstateBiopsyUSBOpticalEncoder(void);


    void DeviceToLong(unsigned char b0, unsigned char b1, unsigned char b2, unsigned char b3, long & val)
    {
    val = b3 + (long(b2)<<8) + (long(b1)<<16) + (long(b0)<<24); 
    }

    void LongToDevice(long val, unsigned char & b0, unsigned char & b1, unsigned char & b2, unsigned char & b3)
    {
    b3= val & 0xFF;
    b2=(val>>8) & 0xFF;
    b1=(val>>16) & 0xFF;
    b0=(val>>24) & 0xFF;
    }
    
    int GetModuleAddress(void); 
    bool SetChannelCounterMode4x(int nChannel);
    

    vtkTRProstateBiopsyGUI *GUI;
    HANDLE  USBDeviceHandle;
private:
  vtkTRProstateBiopsyUSBOpticalEncoder(const vtkTRProstateBiopsyUSBOpticalEncoder&);
  void operator=(const vtkTRProstateBiopsyUSBOpticalEncoder&);

};



// US Digital USB1 device dependent codes
// general control command
#define USDUSB_GENERAL_CONTROL  0x222084
// module address in/out command
#define USDUSB_MODULEADDRESS_COMMAND  0x26
// Read channel data
#define USDUSB_READCHANNEL_COMMAND 0x01
// Reset channel position
#define USDUSB_RESETCHANNEL_COMMAND 0x03
// counter mode
#define USDUSB_COUNTERMODE_COMMAND 0x07

typedef struct {
    unsigned char CommandAndChannel[2];
} USDUSB_READCHANNEL_IN;
typedef struct {
    unsigned char Response[14];
} USDUSB_READCHANNEL_OUT;
typedef struct {
    unsigned char CommandAndChannel[2];
} USDUSB_RESETCHANNEL_IN;
typedef struct {
    unsigned char Result;
} USDUSB_RESETCHANNEL_OUT;
typedef struct {
    unsigned char Mode[4];
} USDUSB_COUNTERMODE_IN;
typedef struct {
    unsigned char Result;
} USDUSB_COUNTERMODE_OUT;

#endif // _vtkTRProstateBiopsyUSBOpticalEncoder_h
