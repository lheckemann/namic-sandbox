#include "vtkTRProstateBiopsyUSBOpticalEncoder.h"
#include "vtkTRProstateBiopsyGUI.h"

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkTRProstateBiopsyUSBOpticalEncoder);
vtkCxxRevisionMacro(vtkTRProstateBiopsyUSBOpticalEncoder, "$Revision: 1.0 $");
//------------------------------------------------------------------------------
/// Constructor
vtkTRProstateBiopsyUSBOpticalEncoder::vtkTRProstateBiopsyUSBOpticalEncoder(void)
{
#ifdef _WIN32
    this->USBDeviceHandle=INVALID_HANDLE_VALUE;
#else /* WIN32 */
    this->USBDeviceHandle=-1;
#endif /* WIN32 */
}


/// Destructor
vtkTRProstateBiopsyUSBOpticalEncoder::~vtkTRProstateBiopsyUSBOpticalEncoder(void)
{
    this->CloseUSBdevice();
}

void vtkTRProstateBiopsyUSBOpticalEncoder::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

/// Tries to initialize the first USB device
bool vtkTRProstateBiopsyUSBOpticalEncoder::OpenUSBdevice(void)
{
    this->CloseUSBdevice();
#ifdef _WIN32
    // Just open all of them and the first successul one is what we'll use
    LPCSTR lsUSBname;
    char USBstring[]={"\\\\.\\USDUSBB0"};
    lsUSBname=USBstring;

    for (int j=0;j<=9;j++) {
        USBstring[7+4]=j+'0';
        this->USBDeviceHandle = CreateFile(lsUSBname, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
        if (this->USBDeviceHandle!=INVALID_HANDLE_VALUE) {
            // make all channels 4x
            for (int k=0;k<4;k++) {
                SetChannelCounterMode4x(k);
            }
            return true;
        }
    }
#endif /* _WIN32 */

    // nothing found
    return false;
}


/// Close the current USB device, if opened
void vtkTRProstateBiopsyUSBOpticalEncoder::CloseUSBdevice(void)
{
#ifdef _WIN32
    if (this->USBDeviceHandle!=INVALID_HANDLE_VALUE) {
        CloseHandle(this->USBDeviceHandle);
        this->USBDeviceHandle=INVALID_HANDLE_VALUE;
    }
#endif /* _WIN32 */
}


/// Get current USB device module address ( -1 means "not found", other: usb device ID )
int vtkTRProstateBiopsyUSBOpticalEncoder::GetModuleAddress(void)
{
#ifdef _WIN32
    if (this->USBDeviceHandle==INVALID_HANDLE_VALUE) {
        return -1;
    }

    BOOL lResult;
    DWORD nBytesReturned;
    unsigned char inBufferAddress;
    unsigned char outBufferAddress;

    inBufferAddress=USDUSB_MODULEADDRESS_COMMAND;

    lResult = DeviceIoControl(USBDeviceHandle, USDUSB_GENERAL_CONTROL,
        &inBufferAddress, sizeof(inBufferAddress),
        &outBufferAddress, sizeof(outBufferAddress),
        &nBytesReturned, NULL);

    if (!lResult || (nBytesReturned!=1)) {
        return -1;
    }
    return outBufferAddress;
#else /* _WIN32 */
    return -1;
#endif /* _WIN32 */
}


/// Set the Channel Counter mode to 4x
bool vtkTRProstateBiopsyUSBOpticalEncoder::SetChannelCounterMode4x(int nChannel)
{
#ifdef _WIN32
    if (this->USBDeviceHandle==INVALID_HANDLE_VALUE) {
        return false;
    }

    BOOL lResult;
    DWORD nBytesReturned;
    USDUSB_COUNTERMODE_IN inBuffer;
    USDUSB_COUNTERMODE_OUT outBuffer;

    inBuffer.Mode[0]=USDUSB_COUNTERMODE_COMMAND;
    inBuffer.Mode[1]=nChannel;
    inBuffer.Mode[2]=62;   // 62=4x  46=1x  52=2x counter resolution
    inBuffer.Mode[3]=0x0E; // 14

    lResult = DeviceIoControl(USBDeviceHandle, USDUSB_GENERAL_CONTROL,
        &inBuffer, sizeof(inBuffer),
        &outBuffer, sizeof(outBuffer),
        &nBytesReturned, NULL);

    if (!lResult || (nBytesReturned!=1)) {
        return false;
    }
    return (outBuffer.Result!=0);
#else /* _WIN32 */
    return false;
#endif /* _WIN32 */
}


/// The the value of a given channel ( -1 = error retrieving data)
int vtkTRProstateBiopsyUSBOpticalEncoder::GetChannelValue(int nChannel)
{
#if _WIN32
    if (this->USBDeviceHandle==INVALID_HANDLE_VALUE) {
        return -1;
    }

    BOOL lResult;
    DWORD nBytesReturned;
    USDUSB_READCHANNEL_IN inBuffer;
    USDUSB_READCHANNEL_OUT outBuffer;

    inBuffer.CommandAndChannel[0]=USDUSB_READCHANNEL_COMMAND;
    inBuffer.CommandAndChannel[1]=nChannel;

    lResult = DeviceIoControl(USBDeviceHandle, USDUSB_GENERAL_CONTROL,
        &inBuffer, sizeof(inBuffer),
        &outBuffer, sizeof(outBuffer),
        &nBytesReturned, NULL);

    if (!lResult || (nBytesReturned!=14)) {
        return -1;
    }

    long nPosition;
    this->DeviceToLong(outBuffer.Response[0],outBuffer.Response[1],outBuffer.Response[2],outBuffer.Response[3], nPosition);
    //DEVICE2LONG(outBuffer.Response[4],outBuffer.Response[5],outBuffer.Response[6],outBuffer.Response[7], nMaxCount); - maximum counter value - wraps!
    //DEVICE2LONG(outBuffer.Response[8],outBuffer.Response[9],outBuffer.Response[10],outBuffer.Response[11], nTimeStamp); - timestamp
    //nCounterMode=outBuffer.Response[12]; - counter mode (62)
    //nStatusFlag=outBuffer.Response[13]; - status ( 0 = Ok)

    // should I check for status!=0 ?
    return nPosition;
#else /* _WIN32 */
    return false;
#endif /* _WIN32 */
}

/// The the max value of a given channel ( -1 = error retrieving data) - the counter is between 0 and max, wraps!
int vtkTRProstateBiopsyUSBOpticalEncoder::GetChannelMaxValue(int nChannel)
{
#if _WIN32
    if (this->USBDeviceHandle==INVALID_HANDLE_VALUE) {
        return -1;
    }

    BOOL lResult;
    DWORD nBytesReturned;
    USDUSB_READCHANNEL_IN inBuffer;
    USDUSB_READCHANNEL_OUT outBuffer;

    inBuffer.CommandAndChannel[0]=USDUSB_READCHANNEL_COMMAND;
    inBuffer.CommandAndChannel[1]=nChannel;

    lResult = DeviceIoControl(USBDeviceHandle, USDUSB_GENERAL_CONTROL,
        &inBuffer, sizeof(inBuffer),
        &outBuffer, sizeof(outBuffer),
        &nBytesReturned, NULL);

    if (!lResult || (nBytesReturned!=14)) {
        return -1;
    }
    long nMaxCount;
    //DEVICE2LONG(outBuffer.Response[0],outBuffer.Response[1],outBuffer.Response[2],outBuffer.Response[3], nPosition); - current position
    this->DeviceToLong(outBuffer.Response[4],outBuffer.Response[5],outBuffer.Response[6],outBuffer.Response[7], nMaxCount);
    //DEVICE2LONG(outBuffer.Response[8],outBuffer.Response[9],outBuffer.Response[10],outBuffer.Response[11], nTimeStamp); - timestamp
    //nCounterMode=outBuffer.Response[12]; - counter mode (62)
    //nStatusFlag=outBuffer.Response[13]; - status ( 0 = Ok)

    // should I check for status!=0 ?
    return nMaxCount;
#else /* _WIN32 */
    return false;
#endif /* _WIN32 */
}


/// Reset this channel
bool vtkTRProstateBiopsyUSBOpticalEncoder::SetChannelValueToZero(int nChannel)
{
#if _WIN32
    if (this->USBDeviceHandle==INVALID_HANDLE_VALUE) {
        return false;
    }

    BOOL lResult;
    DWORD nBytesReturned;
    USDUSB_RESETCHANNEL_IN inBuffer;
    USDUSB_RESETCHANNEL_OUT outBuffer;

    inBuffer.CommandAndChannel[0]=USDUSB_RESETCHANNEL_COMMAND;
    inBuffer.CommandAndChannel[1]=nChannel;

    lResult = DeviceIoControl(USBDeviceHandle, USDUSB_GENERAL_CONTROL,
        &inBuffer, sizeof(inBuffer),
        &outBuffer, sizeof(outBuffer),
        &nBytesReturned, NULL);

    if (!lResult || (nBytesReturned!=1)) {
        return false;
    }
    return (outBuffer.Result!=0);
#else /* _WIN32 */
    return false;
#endif /* _WIN32 */
}
