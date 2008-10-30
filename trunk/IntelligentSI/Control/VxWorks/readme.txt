------------------------------------------------------------------------------
Open core control software source code (tested by Vxworks5.5.1)
Author : Hiroaki kozuka, Jumpei Arata ( Nagoya Institute of technology )
mail   : jumpei@nitech.ac.jp
------------------------------------------------------------------------------

<1.Environment>
OS
  VxWorks5.5.1

A housing
  AVAL-900

CPU board
  Motorola MCP820 (500MHz MPC7410 processor)
CPCI board
  AVAL-DATA ACP-560 D/A     4Ch
  AVAL-DATA ACP-550 A/D     4Ch  
  AVAL-DATA ACP-420 Counter 4Ch

<2.Files>
  main.cpp               // main function
  main.h
  robot.cpp              // for controlling robot
  robot.h                
  frame.cpp              // including kinematics models
  frame.h 
  driver.cpp             // for management board driver 
  driver.h
  joint.cpp              // joint control (PID cotrol)
  joint.h
  IFmanager.cpp          // management interface
  IFmanager.h
  SlicerInterface.cpp    // interface for communicating 3D Slicer by using OpenIGTLink
  SlicerInterface.h
  PNTM_interface.cpp     //interface for communicating robot by using OpenIGTLink
  PNTM_interface.h
  sockUDP.cpp            // UDP/IP socket 
  sockUDP.h
  
  common.h               // structure etc.

  acp560.cpp             // D/A
  acp560.h
  acp550.cpp             // A/D
  acp550.h
  acp420.cpp             //counter
  acp420.h

