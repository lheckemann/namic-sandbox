/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    $RCSfile: igstkPolarisTracker.cxx,v $
  Language:  C++
  Date:      $Date: 2005/12/10 20:48:04 $
  Version:   $Revision: 1.12 $

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more DEBUGrmation.

=========================================================================*/

#if defined(_MSC_VER)
// Warning about: identifier was truncated to '255' characters in the
// debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include "igstkPolarisTracker.h"
#include "igstkTrackerPort.h"

#include <iostream>
#include <fstream>

namespace igstk
{

/** Constructor: Initializes all internal variables. */
PolarisTracker::PolarisTracker(void):m_StateMachine(this)
{
  m_CommandInterpreter = CommandInterpreterType::New();
  m_NumberOfTools = 0;
  for (unsigned int i = 0; i < NumberOfPorts; i++)
    {
    this->m_PortEnabled[i] = 0;
    this->m_PortHandle[i] = 0;
    }

  for (unsigned int j = 0; j < NumberOfPorts; j++)
    { 
    PolarisTrackerToolPointer tool = PolarisTrackerToolType::New();
    TrackerPortPointer port = TrackerPortType::New();
    port->AddTool(tool);
    this->AddPort(port);
    }

  this->SetThreadingEnabled( true );

  m_BufferLock = itk::MutexLock::New();
}

/** Destructor */
PolarisTracker::~PolarisTracker(void)
{
}


/** Helper function for reporting interpreter errors. */
PolarisTracker::ResultType
PolarisTracker::CheckError(CommandInterpreterType *interpreter)
{
  const int errnum = interpreter->GetError();

  if (errnum)
    {
    // convert errnum to a hexidecimal string
    itk::OStringStream os;
    os << "0x";
    os.width(2);
    os.fill('0');
    os << std::hex << std::uppercase << errnum;
    igstkLogMacro( WARNING, "Polaris Error " << os.str() << ": " <<
                   interpreter->ErrorString(errnum) << "\n");

    return FAILURE;
    }

  return SUCCESS;
}


/** Set the communication object, it will be initialized as necessary
  * for use with the Polaris */
void PolarisTracker::SetCommunication( CommunicationType *communication )
{
  igstkLogMacro( DEBUG, "PolarisTracker:: Entered SetCommunication ...\n");
  m_Communication = communication;
  m_CommandInterpreter->SetCommunication( communication );

  // data records are of variable length and end with a carriage return
  if( communication )
    {
    communication->SetUseReadTerminationCharacter( true );
    communication->SetReadTerminationCharacter( '\r' );
    }

  igstkLogMacro( DEBUG, "PolarisTracker:: Exiting SetCommunication ...\n"); 
}

/** Open communication with the tracking device. */
PolarisTracker::ResultType PolarisTracker::InternalOpen( void )
{
  igstkLogMacro( DEBUG, "PolarisTracker::InternalOpen called ...\n");

  m_CommandInterpreter->RESET();
  m_CommandInterpreter->INIT();

  ResultType result = this->CheckError(m_CommandInterpreter);

  if (result == SUCCESS)
    {
    // log information about the device
    m_CommandInterpreter->VER(CommandInterpreterType::NDI_CONTROL_FIRMWARE);
    result = this->CheckError(m_CommandInterpreter);
    }

  return result;
}

/** Close communication with the tracking device. */
PolarisTracker::ResultType PolarisTracker::InternalClose( void )
{
  igstkLogMacro( DEBUG, "PolarisTracker::InternalClose called ...\n");

  // return the device back to its initial comm setttings
  m_CommandInterpreter->COMM(CommandInterpreterType::NDI_9600,
                             CommandInterpreterType::NDI_8N1,
                             CommandInterpreterType::NDI_NOHANDSHAKE);

  return this->CheckError(m_CommandInterpreter);
}

/** Activate the tools attached to the tracking device. */
PolarisTracker::ResultType PolarisTracker::InternalActivateTools( void )
{
  igstkLogMacro( DEBUG, "PolarisTracker::InternalActivateTools called ...\n");

  // load any SROMS that are needed
  for (unsigned int i = 0; i < NumberOfPorts; i++)
    { 
    if (!m_SROMFileNames[i].empty())
      {
      this->LoadVirtualSROM(i, m_SROMFileNames[i]);
      }
    }

  this->EnableToolPorts();

  m_NumberOfTools = 0;

  for(unsigned int i = 0; i < NumberOfPorts; i++)
    { 
    if( this->m_PortEnabled[i] )
      {
      m_NumberOfTools++;
      }
    }

  return SUCCESS;
}

/** Deactivate the tools attached to the tracking device. */
PolarisTracker::ResultType PolarisTracker::InternalDeactivateTools( void )
{
  for (unsigned int i = 0; i < NumberOfPorts; i++)
    { 
    if (!m_SROMFileNames[i].empty() &&
        this->m_PortHandle[i] != 0)
      {
      this->ClearVirtualSROM(i);
      }
    }

  this->DisableToolPorts();

  return SUCCESS;
}

/** Put the tracking device into tracking mode. */
PolarisTracker::ResultType PolarisTracker::InternalStartTracking( void )
{
  igstkLogMacro( DEBUG, "PolarisTracker::InternalStartTracking called ...\n");  

  m_CommandInterpreter->TSTART();

  return this->CheckError(m_CommandInterpreter);
}

/** Take the tracking device out of tracking mode. */
PolarisTracker::ResultType PolarisTracker::InternalStopTracking( void )
{
  igstkLogMacro( DEBUG, "PolarisTracker::InternalStopTracking called ...\n");

  m_CommandInterpreter->TSTOP();

  return this->CheckError(m_CommandInterpreter);
}

/** Reset the tracking device to put it back to its original state. */
PolarisTracker::ResultType PolarisTracker::InternalReset( void )
{
  m_CommandInterpreter->RESET();
  m_CommandInterpreter->INIT();

  ResultType result = this->CheckError(m_CommandInterpreter);

  if (result == SUCCESS)
    {
    // log information about the device
    m_CommandInterpreter->VER(CommandInterpreterType::NDI_CONTROL_FIRMWARE);
    result = this->CheckError(m_CommandInterpreter);
    }

  return result;
}


/** Update the status and the transforms for all TrackerTools. */
PolarisTracker::ResultType PolarisTracker::InternalUpdateStatus()
{
  igstkLogMacro( DEBUG, "PolarisTracker::InternalUpdateStatus called ...\n");

  // these flags are set for tools that can be used for tracking
  const unsigned long mflags = (CommandInterpreterType::NDI_TOOL_IN_PORT |
                                CommandInterpreterType::NDI_INITIALIZED |
                                CommandInterpreterType::NDI_ENABLED);

  m_BufferLock->Lock();

  for (unsigned int port = 0; port < NumberOfPorts; port++) 
    {
    // convert m_StatusBuffer flags from NDI to vtkTracker format
    const int portStatus = m_StatusBuffer[port];

    // only report tools that are enabled
    if ((portStatus & mflags) != mflags) 
      {
      continue;
      }

    // only report tools that are in view
    if (m_AbsentBuffer[port])
      {
      // there should be a method to set that the tool is not in view
      igstkLogMacro( DEBUG, "PolarisTracker::InternalUpdateStatus: " <<
                     "tool " << port << " is not in view\n");
      continue;
      }      

    // create the transform
    TransformType transform;

    typedef TransformType::VectorType TranslationType;
    TranslationType translation;

    translation[0] = m_TransformBuffer[port][4];
    translation[1] = m_TransformBuffer[port][5];
    translation[2] = m_TransformBuffer[port][6];

    typedef TransformType::VersorType RotationType;
    RotationType rotation;
    const double normsquared = 
      m_TransformBuffer[port][0]*m_TransformBuffer[port][0] +
      m_TransformBuffer[port][1]*m_TransformBuffer[port][1] +
      m_TransformBuffer[port][2]*m_TransformBuffer[port][2] +
      m_TransformBuffer[port][3]*m_TransformBuffer[port][3];

    // don't allow null quaternions
    if (normsquared < 1e-6)
      {
      rotation.Set(0.0, 0.0, 0.0, 1.0);
      igstkLogMacro( WARNING, "PolarisTracker::InternUpdateStatus: bad "
                     "quaternion, norm=" << sqrt(normsquared) << "\n");
      }
    else
      {
      // ITK quaternions are in xyzw order, not wxyz order
      rotation.Set(m_TransformBuffer[port][1],
                   m_TransformBuffer[port][2],
                   m_TransformBuffer[port][3],
                   m_TransformBuffer[port][0]);
      }

    // report NDI error value
    typedef TransformType::ErrorType  ErrorType;
    ErrorType errorValue = m_TransformBuffer[port][7];

    typedef TransformType::TimePeriodType TimePeriodType;
    const TimePeriodType validityTime = 100.0;

    transform.SetToIdentity(validityTime);
    transform.SetTranslationAndRotation(translation, rotation, errorValue,
                                        validityTime);

    this->SetToolTransform(port, 0, transform);
    }
  m_BufferLock->Unlock();

  return SUCCESS;
}

/** Update the m_StatusBuffer and the transforms. 
    This function is called by a separate thread. */
PolarisTracker::ResultType PolarisTracker::InternalThreadedUpdateStatus( void )
{
  igstkLogMacro( DEBUG, "PolarisTracker::InternalThreadedUpdateStatus "
                 "called ...\n");

  // get the transforms for all tools from the NDI
  m_CommandInterpreter->TX(CommandInterpreterType::NDI_XFORMS_AND_STATUS);

  ResultType result = this->CheckError(m_CommandInterpreter);

  // lock the buffer
  m_BufferLock->Lock();

  // Initialize transformations to identity.
  // The NDI transform is 8 values:
  // the first 4 values are a quaternion
  // the next 3 values are an x,y,z position
  // the final value is an error estimate in the range [0,1]
  for (unsigned int port = 0; port < NumberOfPorts; port++)
    {
    m_TransformBuffer[port][0] = 1.0;
    m_TransformBuffer[port][1] = 0.0;
    m_TransformBuffer[port][2] = 0.0;
    m_TransformBuffer[port][3] = 0.0;
    m_TransformBuffer[port][4] = 0.0;
    m_TransformBuffer[port][5] = 0.0;
    m_TransformBuffer[port][6] = 0.0;
    m_TransformBuffer[port][7] = 0.0;
    }

  if (result == SUCCESS)
    {
    unsigned long frame[ NumberOfPorts ];
    for (unsigned int port = 0; port < NumberOfPorts; port++)
      { 
      // port handle reported by device
      const int ph = this->m_PortHandle[port];
      m_AbsentBuffer[port] = 0;
      m_StatusBuffer[port] = 0;

      frame[port] = 0;
      if (ph == 0)
        {
        continue;
        }

      double transform[8];
      const int tstatus = m_CommandInterpreter->GetTXTransform(ph, transform);
      const int absent = (tstatus != CommandInterpreterType::NDI_VALID);
      const int status = m_CommandInterpreter->GetTXPortStatus(ph);
      frame[port] = m_CommandInterpreter->GetTXFrame(ph);

      if (!absent)
        {
        for( unsigned int i = 0; i < 8; i++ )
          {
          m_TransformBuffer[port][i] = transform[i];
          }
        }

      m_AbsentBuffer[port] = absent;
      m_StatusBuffer[port] = status;
      }
    }

  // In the original vtkNDITracker code, there was a check at this
  // point in the code to see if any new tools had been plugged in

  // unlock the buffer
  m_BufferLock->Unlock();

  return result;
}

/** Specify an SROM file to be used with a passive or custom tool. */
void PolarisTracker::AttachSROMFileNameToPort( const unsigned int portNum,
                                               std::string fileName )
{
  igstkLogMacro( DEBUG, "PolarisTracker::AttachSROMFileNameToPort called..\n");

  // the first 3 ports are active, don't allow SROMS for them
  if ( (portNum >= 3) && (portNum <= NumberOfPorts) )
    {
    m_SROMFileNames[portNum] = fileName;
    }
}

/** Load a virtual SROM, given the file name of the ROM file */
bool PolarisTracker::LoadVirtualSROM( const unsigned int tool,
                                      const std::string SROMFileName) 
{
  igstkLogMacro( DEBUG, "PolarisTracker::LoadVirtualSROM called...\n");

  if (this->m_PortHandle[tool] != 0)
    {
    igstkLogMacro( WARNING, "An SROM has already been loaded for tool "
                   << tool << "\n");
    return false;
    }

  std::ifstream sromFile;
  sromFile.open(SROMFileName.c_str(), std::ios::binary );

  if (!sromFile.is_open())
    {
    igstkLogMacro( WARNING, "PolarisTracker::LoadVirtualSROM: couldn't "
                   "find SROM file " << SROMFileName << " ...\n");
    return false;
    }

  // most SROM files don't contain the whole 1024 bytes, they only
  // contain whatever is necessary, so the rest should be filled with zero
  char data[1024]; 
  memset( data, 0, 1024 );
  sromFile.read( data, 1024 );
  sromFile.close();

  // the "port" must be set to "**" to support the Vicra
  m_CommandInterpreter->PHRQ("********", // device number
                             "*",        // TIU or SCU
                             "1",        // wired or wireless
                             "**",       // port
                             "**");      // channel

  if (this->CheckError(m_CommandInterpreter) == FAILURE)
    {
    return false;
    }
 
  const int ph = m_CommandInterpreter->GetPHRQHandle();

  for ( unsigned int i = 0; i < 1024; i += 64)
    {
    // holds hexidecimal data to be sent to device
    char hexbuffer[129]; 
    
    // convert data to hexidecimal and write to virtual SROM in
    // 64-byte chunks
    m_CommandInterpreter->HexEncode(hexbuffer, &data[i], 64);
    m_CommandInterpreter->PVWR(ph, i, hexbuffer);
    }

  // set the port handle for the tool
  this->m_PortEnabled[tool] = 0;
  this->m_PortHandle[tool] = ph;

  return true;
}

/** Clear a previously loaded SROM. */
void PolarisTracker::ClearVirtualSROM(const unsigned int tool)
{
  igstkLogMacro( DEBUG, "PolarisTracker::ClearVirtualSROM called...\n");

  const int ph = this->m_PortHandle[tool];
  m_CommandInterpreter->PHF(ph);

  this->m_PortEnabled[tool] = 0;
  this->m_PortHandle[tool] = 0;
}

/** Enable all tool ports that are occupied. */
void PolarisTracker::EnableToolPorts()
{
  igstkLogMacro( DEBUG, "PolarisTracker::EnableToolPorts called...\n");

  // reset our information about the wired ports
  for (unsigned int port = 0; port < 3; port++)
    {
    this->m_PortHandle[port] = 0;
    }
  for (unsigned int port = 0; port < NumberOfPorts; port++)
    {
    this->m_PortEnabled[port] = 0;
    }

  // free ports that are waiting to be freed
  m_CommandInterpreter->PHSR(CommandInterpreterType::NDI_STALE_HANDLES);
  unsigned int ntools = m_CommandInterpreter->GetPHSRNumberOfHandles();
  unsigned int tool;
  for (tool = 0; tool < ntools; tool++)
    {
    const int ph = m_CommandInterpreter->GetPHSRHandle(tool);
    m_CommandInterpreter->PHF(ph);

    // if failed to release handle, print error but continue on
    this->CheckError(m_CommandInterpreter);
    }

  // keep list of tools that fail to initialize, so we don't keep retrying,
  // the largest port handle possible is 0xFF, or 256
  int alreadyAttemptedPINIT[256];
  for (int ph = 0; ph < 256; ph++)
    {
    alreadyAttemptedPINIT[ph] = 0;
    }

  // initialize ports waiting to be initialized,  
  // repeat as necessary (in case multi-channel tools are used) 
  for (int safetyCount = 0; safetyCount < 256; safetyCount++)
    {
    m_CommandInterpreter->PHSR(
      CommandInterpreterType::NDI_UNINITIALIZED_HANDLES);
    
    if (this->CheckError(m_CommandInterpreter) == FAILURE)
      {
      break;
      }

    ntools = m_CommandInterpreter->GetPHSRNumberOfHandles();
    int foundNewTool = 0;

    // try to initialize all port handles
    for (tool = 0; tool < ntools; tool++)
      {
      const int ph = m_CommandInterpreter->GetPHSRHandle(tool);
      // only call PINIT on tools that didn't fail last time
      // (the &0xFF makes sure index is < 256)
      if (!alreadyAttemptedPINIT[(ph & 0xFF)])
        {
        alreadyAttemptedPINIT[(ph & 0xFF)] = 1;
        m_CommandInterpreter->PINIT(ph);
        if (this->CheckError(m_CommandInterpreter) == SUCCESS)
          {
          foundNewTool = 1;
          }
        }
      }

    // exit if no new tools were initialized this round
    if (!foundNewTool)
      {
      break;
      }
    }

  // enable initialized tools
  m_CommandInterpreter->PHSR(CommandInterpreterType::NDI_UNENABLED_HANDLES);

  ntools = m_CommandInterpreter->GetPHSRNumberOfHandles();
  
  for (tool = 0; tool < ntools; tool++)
    {
    const int ph = m_CommandInterpreter->GetPHSRHandle(tool);
    m_CommandInterpreter->PHINF(ph, CommandInterpreterType::NDI_BASIC);

    // tool identity and type information
    char identity[512];
    m_CommandInterpreter->GetPHINFToolInfo(identity);

    // use tool type information to figure out mode for enabling
    int mode = CommandInterpreterType::NDI_DYNAMIC;

    if (identity[1] == CommandInterpreterType::NDI_TYPE_BUTTON)
      { // button-box or foot pedal
      mode = CommandInterpreterType::NDI_BUTTON_BOX;
      }
    else if (identity[1] == CommandInterpreterType::NDI_TYPE_REFERENCE)
      { // reference
      mode = CommandInterpreterType::NDI_STATIC;
      }

    // enable the tool
    m_CommandInterpreter->PENA(ph, mode);

    // print any warnings
    this->CheckError(m_CommandInterpreter);
    }

  // get information for all tools
  m_CommandInterpreter->PHSR(CommandInterpreterType::NDI_ALL_HANDLES);

  ntools = m_CommandInterpreter->GetPHSRNumberOfHandles();

  for (tool = 0; tool < ntools; tool++)
    {
    const int ph = m_CommandInterpreter->GetPHSRHandle(tool);
    m_CommandInterpreter->PHINF(ph,
                                CommandInterpreterType::NDI_PORT_LOCATION |
                                CommandInterpreterType::NDI_PART_NUMBER |
                                CommandInterpreterType::NDI_BASIC );

    if (this->CheckError(m_CommandInterpreter) == FAILURE)
      {
      continue;
      }

    // get the physical port identifier
    char location[512];
    m_CommandInterpreter->GetPHINFPortLocation(location);

    // physical port number
    unsigned int port = 0;

    // the value of m_PortHandle should only be set for wired,
    // for wireless tools it is set in LoadVirtualSROM
    if (location[9] == '0') // wired tool
      {
      unsigned int ndiport = (location[10]-'0')*10 + (location[11]-'0');
      if (ndiport > 0 && ndiport <= NumberOfPorts)
        {
        port = ndiport - 1;
        this->m_PortHandle[port] = ph;
        }
      }
    else // wireless tool
      {
      for (port = 0; port < NumberOfPorts; port++)
        {
        if (this->m_PortHandle[port] == ph)
          {
          break;
          }
        }
      if (port >= NumberOfPorts)
        {
        igstkLogMacro( WARNING, "PolarisTracker::EnableToolPorts: "
                       "cant find port for port handle\n" );
        continue;
        }
      }

    const int status = m_CommandInterpreter->GetPHINFPortStatus();

    this->m_PortEnabled[port] = 
      ((status & CommandInterpreterType::NDI_ENABLED) != 0);
    }
}

/** Disable all tool ports. */
void PolarisTracker::DisableToolPorts( void )
{
  igstkLogMacro( DEBUG, "PolarisTracker::DisableToolPorts called...\n");

  // disable all enabled tools
  m_CommandInterpreter->PHSR(CommandInterpreterType::NDI_ENABLED_HANDLES);
  unsigned int ntools = m_CommandInterpreter->GetPHSRNumberOfHandles();

  for (unsigned int tool = 0; tool < ntools; tool++)
    {
    const int ph = m_CommandInterpreter->GetPHSRHandle(tool);
    m_CommandInterpreter->PDIS( ph );

    // print warning if failed to disable
    this->CheckError(m_CommandInterpreter);
    }

  // disable the enabled ports
  for (unsigned int port = 0; port < NumberOfPorts; port++)
    {
    this->m_PortEnabled[port] = 0;
    }
}


/** Print Self function */
void PolarisTracker::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);

  unsigned int i;

  for( i = 0; i < NumberOfPorts; ++i )
    {
    os << indent << "Port " << i << " Enabled: " << m_PortEnabled[i] 
       << std::endl;
    }
  for( i = 0; i < NumberOfPorts; ++i )
    {
    os << indent << "Port " << i << " Handle: " << m_PortHandle[i] 
       << std::endl;
    }
  os << indent << "Number of tools: " << m_NumberOfTools << std::endl;
  for( i = 0; i < NumberOfPorts; ++i )
    {
    os << indent << "SROM filename " << i << " : " << m_SROMFileNames[i]
       << std::endl;
    }
}


} // end of namespace igstk

