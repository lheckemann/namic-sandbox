/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

// ****************************************************************************
//
//  $Id: ddiCompositeDevice.h,v 1.8 2006/05/13 21:26:04 kapoor Exp $
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


/*!
  \file
  \brief Defines a composite device 
  \ingroup cisstDeviceInterface
*/


#ifndef _ddiCompositeDevice_h
#define _ddiCompositeDevice_h

#include <cisstCommon/cmnDataObject.h>
#include <cisstDeviceInterface/ddiDeviceInterface.h>
#include <cisstDeviceInterface/ddiCompositeCommand0.h>
#include <cisstDeviceInterface/ddiCompositeCommand1.h>
#include <cisstDeviceInterface/ddiCompositeCommand2.h>
#include <vector>
#include <map>
#include <string>
#include <iostream>

// Always include last
#include <cisstDeviceInterface/ddiExport.h>

/*!
  \ingroup cisstDeviceInterface

  This class has a vector of pointers to devices. It concatenates the
  operation on two or more devices into one, such that it appears a
  single operation to a user of the composite device. As an example
  consider two LoPoMoCo cards as constituents of a composite device. A
  user of this composite device has to issue only one position read to
  get eight positions. The composite device automagically routes them
  to the correct device, in this case each constituent device fills
  the corresponding entry position vector.
 */
class CISST_EXPORT ddiCompositeDevice : public ddiDeviceInterface {
 protected:
  /*! Vector of pointers to devices that constitute this composite
    device. */
  std::vector<ddiDeviceInterface *> Devices;

  /*! The derived device can use this to concatinate Operation
    maps.  AddDevice and this method should be overloaded if any
    specific behaviour is required for the new composite device. An
    example where these need to be overloaded is a chain of STG or
    LoPoMoCo. */
    template <class __MapType, class __CommandType, class __CommandTypeComposite>
  void ConcatMaps(__MapType &devNameMap, __MapType &concatNameMap);

  /*! The derived devices add devices to this list using this
    method */
  void AddDevice(ddiDeviceInterface *device);

  /*! This return the ith device */
  ddiDeviceInterface* GetDevice(unsigned int deviceNumber) {return Devices[deviceNumber];}

  /*! The constructor is protected so that the no object gets created */
  ddiCompositeDevice(std::string name): ddiDeviceInterface(name) {}

public:
  /*! Default destructor. Does nothing. */
  ~ddiCompositeDevice();

  /*! The default Configure and Provides are provided. The execute
    the Configure and Provides of the constituent devices in order
    they are added */
  virtual void Configure(const char *filename=NULL);

  /*! Concat the 'provides' string to constituent devices. */
  virtual std::vector<std::string> Provides(void);
};


template <class __MapType, class __CommandType, class __CommandTypeComposite>
void ddiCompositeDevice::ConcatMaps(__MapType &concatNameMap, __MapType &devNameMap) {
    typename __MapType::const_iterator iter;
    typename __MapType::iterator iterComposite;
    for (iter = devNameMap.begin(); iter != devNameMap.end(); ++iter) {
        iterComposite = concatNameMap.end();
        iterComposite = concatNameMap.find(iter->first);
        if (iterComposite == concatNameMap.end()) {
            concatNameMap.insert(std::pair<std::string, __CommandType*> (iter->first, iter->second));
        } else {
            // new device operation was already there in composite
            // device. remove it and add as composite
            __CommandTypeComposite* compositeCmd = new __CommandTypeComposite();
            compositeCmd->AddCommand(iterComposite->second);
            compositeCmd->AddCommand(iter->second);
            concatNameMap[iter->first] = compositeCmd;
        }
    }
}

#endif // _ddiCompositeDevice_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: ddiCompositeDevice.h,v $
// Revision 1.8  2006/05/13 21:26:04  kapoor
// cisstDeviceInterface: Missed Ampersand (not passed by reference) in concat maps
// lead to a silly bug.
//
// Revision 1.7  2006/05/03 01:49:59  kapoor
// These compile. For easy tranfer to Anton. No gurantees on functionality.
//
// Revision 1.6  2005/09/26 15:41:46  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.5  2005/06/17 22:45:25  anton
// cisst libraries: Added some CISST_EXPORT required to compile DLLs
//
// Revision 1.4  2004/10/30 00:58:26  kapoor
// Added method to obtain a pointer to device.
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
// Revision 1.2  2004/03/24 20:48:32  kapoor
// Added lots of documentation.
//
// Revision 1.1  2004/03/22 00:31:20  kapoor
// Added new devices.
//
// ****************************************************************************
