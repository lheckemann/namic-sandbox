/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

// ****************************************************************************
//
//  $Id: ddiCompositeDevice.cpp,v 1.6 2006/05/03 01:51:37 kapoor Exp $
//
//  Author(s):  Ankur Kapoor
//  Created on: 2004-04-30
//
//
//              Developed by the Engineering Research Center for
//          Computer-Integrated Surgical Systems & Technology (CISST)
//
//               Copyright(c) 2004, The Johns Hopkins University
//                          All Rights Reserved.
//
//  Experimental Software - Not certified for clinical use.
//  For use only by CISST sponsored research projects.  For use outside of
//  CISST, please contact Dr. Russell Taylor, CISST Director, at rht@cs.jhu.edu
//  
// ****************************************************************************


#include <cisstDeviceInterface/ddiCompositeDevice.h>


ddiCompositeDevice::~ddiCompositeDevice() {
  for (unsigned int i = 0; i < Devices.size(); i++) {
    if (Devices[i]) delete Devices[i];
  }
}


void ddiCompositeDevice::Configure(const char *filename) {
  for (unsigned int i = 0; i < Devices.size(); i++) {
    if (Devices[i]) Devices[i]->Configure(filename);
  }
}


std::vector<std::string> ddiCompositeDevice::Provides(void) {
  std::vector<std::string> *provides = new std::vector<std::string>;
  if (Devices[0]) *provides = Devices[0]->Provides();
  std::vector<std::string> list;
  for (unsigned int i = 1; i < Devices.size(); i++) {
    if (Devices[i]) {
      list = Devices[i]->Provides();
      for (unsigned int j = 0; j < list.size(); j++) {
        bool found = false;
        for (unsigned int k = 0; k < provides->size(); k++) {
          if ((*provides)[k] == list[j]) {
            found = true;
            break;
          }
        }
        if (!found) provides->push_back(list[j]);
      }
    }
  }
  return *provides;
}

void ddiCompositeDevice::AddDevice(ddiDeviceInterface *device) {
    if (device) {
        Devices.push_back(device);
        // reconstructing a new map for the composite device would
        // make lookup operations faster than trying to lookup in
        // number of smaller maps
        ConcatMaps<ddiDeviceInterface::ZeroArgOperationNameMapType, ddiCommand0Base, ddiCompositeCommand0>
            (this->GetDeviceMapZeroArg(), device->GetDeviceMapZeroArg());
        ConcatMaps<ddiDeviceInterface::OneArgOperationNameMapType, ddiCommand1Base, ddiCompositeCommand1>
            (this->GetDeviceMapOneArg(), device->GetDeviceMapOneArg());
        ConcatMaps<ddiDeviceInterface::TwoArgOperationNameMapType, ddiCommand2Base, ddiCompositeCommand2>
            (this->GetDeviceMapTwoArg(), device->GetDeviceMapTwoArg());
    }
}


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: ddiCompositeDevice.cpp,v $
// Revision 1.6  2006/05/03 01:51:37  kapoor
// These compile. For easy tranfer to Anton. No gurantees on functionality.
//
// Revision 1.5  2005/09/26 15:41:46  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.4  2004/05/27 15:05:48  anton
// I must have done a nasty copy/paste, removed implementation of
// 3 methods for ddiCompositeCommand2 which had nothing to do here.
//
// Revision 1.3  2004/05/11 17:55:27  anton
// Switch back to ddiCompositeCommand from ddiCommandComposite.  There has been
// a minor communication problem between Ankur and I.  Sorry.
//
// Revision 1.2  2004/05/06 17:58:26  kapoor
// Renamed ddiCompositeCommand to ddiCommandComposte and ddiCompositeCommand2 to ddiCommand2Composite
//
// Revision 1.1  2004/05/04 20:45:45  anton
// Added code for composite devices
//
//
// Revision 1.3  2004/03/26 06:09:44  kapoor
// Removed keyword const from Execute.
//
// Revision 1.2  2004/03/24 21:00:28  kapoor
// Return a command object instead of a member function pointer. This was done
// to handle composite devices. The command object contains the information
// on the deivce that has to handle the operation besides just the operation,
// which is needed to resolve who should handle the operaion in case of a
// composite device.
//
// Revision 1.1  2004/03/22 00:30:53  kapoor
// Added new devices.
//
// ****************************************************************************
