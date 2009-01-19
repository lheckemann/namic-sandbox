/*=========================================================================

  Program:   AtamaiTracking for VTK
  Module:    $RCSfile: vtkTracker.h,v $
  Creator:   David Gobbi <dgobbi@atamai.com>
  Language:  C++
  Author:    $Author: dgobbi $
  Date:      $Date: 2008/04/10 17:26:35 $
  Version:   $Revision: 1.7 $

==========================================================================

Copyright (c) 2000-2005 Atamai, Inc.

Use, modification and redistribution of the software, in source or
binary forms, are permitted provided that the following terms and
conditions are met:

1) Redistribution of the source code, in verbatim or modified
   form, must retain the above copyright notice, this license,
   the following disclaimer, and any notices that refer to this
   license and/or the following disclaimer.  

2) Redistribution in binary form must include the above copyright
   notice, a copy of this license and the following disclaimer
   in the documentation or with other materials provided with the
   distribution.

3) Modified copies of the source code must be clearly marked as such,
   and must not be misrepresented as verbatim copies of the source code.

THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES PROVIDE THE SOFTWARE "AS IS"
WITHOUT EXPRESSED OR IMPLIED WARRANTY INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE.  IN NO EVENT SHALL ANY COPYRIGHT HOLDER OR OTHER PARTY WHO MAY
MODIFY AND/OR REDISTRIBUTE THE SOFTWARE UNDER THE TERMS OF THIS LICENSE
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, LOSS OF DATA OR DATA BECOMING INACCURATE
OR LOSS OF PROFIT OR BUSINESS INTERRUPTION) ARISING IN ANY WAY OUT OF
THE USE OR INABILITY TO USE THE SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGES.

=========================================================================*/
// .NAME vtkTracker - interfaces VTK with real-time 3D tracking systems
// .SECTION Description
// The vtkTracker is a generic VTK interface to real-time tracking
// systems.  Subclasses to this class implement this interface for
// the POLARIS (Northern Digital Inc., Waterloo, Canada), the
// Flock of Birds (Ascension Technology Corporation), and a few
// other systems.
// Derived classes should override the Probe(), InternalUpdate(),
// InternalStartTracking(), and InternalStopTracking() methods.
// The InternalUpdate() method is called from within a separate
// thread, therefore its contents must be thread safe.  Use the
// vtkPOLARISTracker as a framework for developing subclasses
// for new tracking systems.
// .SECTION see also
// vtkTrackerTool vtkPOLARISTracker vtkFlockTracker

#ifndef __vtkTracker_h
#define __vtkTracker_h

#include "vtkTrackingWin32Header.h"
#include "vtkObject.h"
#include "vtkCriticalSection.h"

class vtkMatrix4x4;
class vtkMultiThreader;
class vtkTrackerTool;
class vtkSocketCommunicator;
class vtkCharArray;
class vtkDataArray;
class vtkDoubleArray;

// several flags which give added info about a transform
enum {
  TR_MISSING       = 0x0001,  // tool or tool port is not available
  TR_OUT_OF_VIEW   = 0x0002,  // cannot obtain transform for tool
  TR_OUT_OF_VOLUME = 0x0004,  // tool is not within the sweet spot of system
  TR_SWITCH1_IS_ON = 0x0010,  // various buttons/switches on tool
  TR_SWITCH2_IS_ON = 0x0020,
  TR_SWITCH3_IS_ON = 0x0040
};

// flags for tool LEDs (specifically for the POLARIS)
enum {
  TR_LED_OFF   = 0,
  TR_LED_ON    = 1,
  TR_LED_FLASH = 2
};

class VTK_TRACKING_EXPORT vtkTracker : public vtkObject
{
public:
  static vtkTracker *New();
  vtkTypeMacro(vtkTracker,vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Probe to see to see if the tracking system is connected to the
  // computer.  Returns 1 if the tracking system was found and is working.
  // Do not call this method while the system is Tracking.  This method
  // should be overridden in subclasses. 
  virtual int Probe();

  // Description:
  // Start the tracking system.  The tracking system is brought from
  // its ground state (i.e. on but not necessarily initialized) into
  // full tracking mode.  This method calls InternalStartTracking()
  // after doing a bit of housekeeping.
  virtual void StartTracking();

  // Description:
  // Stop the tracking system and bring it back to its ground state.
  // This method calls InternalStopTracking().
  virtual void StopTracking();

  // Description:
  // Test whether or not the system is tracking.
  virtual int IsTracking() { return this->Tracking; };

  // Description:
  // This method will call Update() on each of the tools.  Note that
  // this method does not call the InternalUpdate() method, which
  // is called by a separate thread.
  virtual void Update();
  
  // Description:
  // Get the internal update rate for this tracking system.  This is
  // the number of transformations sent by the tracking system per
  // second per tool.
  double GetInternalUpdateRate() { return this->InternalUpdateRate; };

  // Description:
  // Get the tool object for the specified port.  The first tool is
  // retrieved by GetTool(0).  See vtkTrackerTool for more information.
  vtkTrackerTool *GetTool(int port);

  // Description:
  // Get the number of available tool ports.  This is the maxiumum that a
  // particular tracking system can support, not the number of tools
  // that are actually connected to the system.  In order to determine
  // how many tools are connected, you must call Update() and then
  // check IsMissing() for each tool between 0 and NumberOfTools-1.
  vtkGetMacro(NumberOfTools, int);

  // Description:
  // Get the timestamp for the last time that Update() was called, in
  // seconds since 1970 (i.e. the UNIX epoch).  This method is not a
  // good method of getting timestamps for tracking information,
  // you should use the vtkTrackerTool GetTimeStamp() method to get
  // the timestamp associated with each transform.  This method is
  // only valuable for determining e.g. how old the transforms were
  // before the Update method was called.
  vtkGetMacro(UpdateTimeStamp,double);

  // Description:
  // Set one of the ports to be a reference, i.e. track other
  // tools relative to this one.  Set this to -1 (the default)
  // if a reference tool is not desired.
  vtkSetMacro(ReferenceTool, int);
  vtkGetMacro(ReferenceTool, int);
  
  // Description:
  // In addition to the default mode of operation of the tracker
  // it can also operate in client/server mode where the server
  // is on the machine that has the tracker attached to it, and
  // the client is on another machine that talks to the server
  // via TCP/IP.  Set ServerMode to 1 in order to create a "server"
  // version of the tracker object, or set a RemoteAddress in
  // order to create a "client" version.
  vtkSetMacro(ServerMode, int);
  vtkGetMacro(ServerMode, int);
  
  // Description:
  // This method is only used when you are operating the tracker
  // in client/server mode.  Set a numerical network port for
  // communication between the client and the server (default: 1111).  
  vtkSetMacro(NetworkPort, int);
  vtkGetMacro(NetworkPort, int);

  // Description:
  // This method is only used when you are operating the tracker
  // in client/server mode.  Set the IP address of the server
  // that the client will talk to.
  vtkSetStringMacro(RemoteAddress);
  vtkGetStringMacro(RemoteAddress);

  // Description:
  // Get the socket communicator that is used for communication
  // between the server and the client.
  vtkSocketCommunicator* GetSocketCommunicator() {
    return this->SocketCommunicator; };

  // Description:
  // Set the transformation matrix between tracking-system coordinates
  // and the desired world coordinate system.  You can use 
  // vtkLandmarkTransform to create this matrix from a set of 
  // registration points.  Warning: the matrix is copied,
  // not referenced.
  void SetWorldCalibrationMatrix(vtkMatrix4x4* vmat);
  vtkMatrix4x4 *GetWorldCalibrationMatrix();

  // Description:
  // Make the unit emit a string of audible beeps.  This is
  // supported by the POLARIS.
  void Beep(int n);
  
  // Description:
  // Turn one of the LEDs on the specified tool on or off.  This
  // is supported by the POLARIS.
  void SetToolLED(int tool, int led, int state);

  // Description:
  // The subclass will do all the hardware-specific update stuff
  // in this function.   It should call ToolUpdate() for each tool.
  // Note that vtkTracker.cxx starts up a separate thread after
  // InternalStartTracking() is called, and that InternalUpdate() is
  // called repeatedly from within that thread.  Therefore, any code
  // within InternalUpdate() must be thread safe.  You can temporarily
  // pause the thread by locking this->UpdateMutex->Lock() e.g. if you
  // need to communicate with the device from outside of InternalUpdate().
  // A call to this->UpdateMutex->Unlock() will resume the thread.
  virtual void InternalUpdate() {};

//BTX
  // These are used by static functions in vtkTracker.cxx, and since
  // VTK doesn't generally use 'friend' functions they are public
  // instead of protected.  Do not use them anywhere except inside
  // vtkTracker.cxx.
  vtkCriticalSection *UpdateMutex;
  vtkCriticalSection *RequestUpdateMutex;
  vtkTimeStamp UpdateTime;
  double InternalUpdateRate;  
  //ETX
  
  // Description:
  // The ServerTracker should call only this function. This creates
  // a thread that allows it to wait till a client connects. 
  void Connect();
  void Disconnect();
  void StartServer();
  void InterpretCommands(char *message);

  // Description:
  // helper function that converts all the buffer info into a DoubleArray
  void ConvertBufferToMessage( int tool, vtkMatrix4x4 *matrix, 
             long flags, double ts,
             double *msg );
  // Description:
  // helper function that converts all the buffer info into a DoubleArray
  void ConvertMessageToBuffer( double *msg, 
             double*vals, vtkMatrix4x4 *matrix); 
             //long flags, double ts );
  
  void ServerToolUpdate( int tool, 
       vtkMatrix4x4 *matrix, 
       long flags, double ts );

protected:
  vtkTracker();
  ~vtkTracker();

  // Description:
  // This function is called by InternalUpdate() so that the subclasses
  // can communicate information back to the vtkTracker base class, which
  // will in turn relay the information to the appropriate vtkTrackerTool.
  void ToolUpdate(int tool, vtkMatrix4x4 *matrix, long flags, 
      double timestamp);

  // Description:
  // Set the number of tools for the tracker -- this method is
  // only called once within the constructor for derived classes.
  void SetNumberOfTools(int num);

  // Description:
  // These methods should be overridden in derived classes: 
  // InternalStartTracking() should initialize the tracking device, and
  // InternalStopTracking() should free all resources associated with
  // the device.  These methods should return 1 if they are successful,
  // or 0 if they are not.
  virtual int InternalStartTracking() { return 1; };
  virtual int InternalStopTracking() { return 1; };
  virtual void InternalInterpretCommand( char * c) { };
  // Description:
  // This method should be overridden in derived classes that can make
  // an audible beep.  The return value should be zero if an error
  // occurred while the request was being processed.
  virtual int InternalBeep(int n) { return 1; };

  // Description:
  // This method should be overridden for devices that have one or more LEDs
  // on the tracked tools. The return value should be zero if an error
  // occurred while the request was being processed.
  virtual int InternalSetToolLED(int tool, int led, int state) { return 1; };

  vtkMatrix4x4 *WorldCalibrationMatrix;
  int NumberOfTools;
  vtkTrackerTool **Tools;
  int ReferenceTool;
  int Tracking;
  
  double UpdateTimeStamp;
  unsigned long LastUpdateTime;

  vtkMultiThreader *Threader;
  int ThreadId;

  int ServerMode;
  int NetworkPort;
  int ClientConnected;
  char *RemoteAddress;
  vtkSocketCommunicator *SocketCommunicator;
  
private:
  vtkTracker(const vtkTracker&);
  void operator=(const vtkTracker&);  
};

#endif

