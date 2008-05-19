/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

// ****************************************************************************
//
//  $Id: ddiDeviceInterface.h,v 1.16 2006/06/03 00:27:45 kapoor Exp $
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

#ifndef _ddiDeviceInterface_h
#define _ddiDeviceInterface_h

#include <cisstCommon/cmnGenericObject.h>
#include <cisstCommon/cmnClassRegister.h>
#include <cisstCommon/cmnDataObject.h>
#include <cisstDeviceInterface/ddiTypes.h>
#include <cisstDeviceInterface/ddiCommand0.h>
#include <cisstDeviceInterface/ddiCommand1.h>
#include <cisstDeviceInterface/ddiCommand2.h>

#include <vector>
#include <map>
#include <string>
#include <typeinfo>

// Always include last
#include <cisstDeviceInterface/ddiExport.h>

/*!
  \file
  \brief Declaration of ddiDeviceInterface
  \ingroup cisstDeviceInterface
 */

//forward decleration for the NO OPS device.
class ddiNOPS;

/*!
  \ingroup cisstDeviceInterface

  This will change.
  This class is a bit tricky, and the name isnt obvious (it will change), but
  the idea behind this class is to act as a bridge between the hardware devices
  and the software tasks that can act as devices. An example is that a PID
  controller could be on a hardware device such as MEI and also on a software
  task such as mrcBasicServo which in turn uses a hardware device such as STG
  or LoPoMoCo. The goal is to keep the higher levels free from dependencies
  on the lower levels. Continuing with our example, it means that the trajectory
  task should not care if the lower level positioning is provided by a MEI
  or a task.
  The way to go is to have a unified interface and this class provides that.
  Thus every task or a device would provide common methods for reading and
  writing. For tasks these would translate to read and write to the mailbox,
  and for devices it would translate to a IO to actual hardware.
 */
class CISST_EXPORT ddiDeviceInterface: public cmnGenericObject {
    public:
        /*! Typdef for the pointer to member funtion which takes zero
          argument of the device. */
        typedef bool (ddiDeviceInterface::*ZeroArgOperationPointerBaseType)();

        /*! Typdef for the pointer to member funtion which takes one
          argument of the device. */
        typedef bool (ddiDeviceInterface::*OneArgOperationPointerBaseType)(cmnDataObject &);

        /*! Typedef for the pointer to member funtion which takes two
          arguments of the device. */
        typedef bool (ddiDeviceInterface::*TwoArgOperationPointerBaseType)(cmnDataObject &, cmnDataObject &);

        /*! Typedef for a map of name of zero argument command and name of command. */
        typedef std::map<std::string, ddiCommand0Base*> ZeroArgOperationNameMapType;

        /*! Typedef for a map of name of one argument command and name of command. */
        typedef std::map<std::string, ddiCommand1Base*> OneArgOperationNameMapType;

        /*! Typedef for a map of name of two argument command and name of command. */
        typedef std::map<std::string, ddiCommand2Base*> TwoArgOperationNameMapType;

    private:

    protected:

        /*! A string identifying the 'Name' of the device. */
        std::string Name;

    public:

        /*! Static pointer to the NOPS deivce. */
        static ddiNOPS *NOPSDevice;


        /*! Static pointer to NOPS method.
          These are returned when a lookup on a map fails. They can be used for
          comparison later, hence static
         */
        static ddiCommand0<ddiNOPS> *NOPSCommand0;

        /*! Static pointer to NOPS method.
          These are returned when a lookup on a map fails. They can be used for
          comparison later, hence static
         */
        static ddiCommand1<ddiNOPS> *NOPSCommand1;


        /*! Static pointer to NOPS2 method.
          These are returned when a lookup on a map fails. They can be used for
          comparison later, hence static
         */
        static ddiCommand2<ddiNOPS> *NOPSCommand2;


        /*! Default constructor. Does nothing. */
        ddiDeviceInterface() {}

        /*! Default constructor. Sets the name. */
        ddiDeviceInterface(std::string name):Name(name) {}

        /*! Default destructor. Does nothing. */
        virtual ~ddiDeviceInterface() {}

        /*! Returns the name of the device. */
        std::string GetName(void) {
            return Name;
        }

        /*! The virtual method so that the device or tasks can configure themselves */
        virtual void Configure(const char *filename = NULL) = 0;

        /*! The virtual method so that the device or tasks returns what operations
          they provide */
        virtual std::vector<std::string> Provides(void);

        /*! Get a const reference to the Zero argument
          map of commands indexed by name.
          This is used by the Composite device to contruct its own map, which
          is concatenation of the maps of it constituent devices.
         */
        ZeroArgOperationNameMapType & GetDeviceMapZeroArg();

        /*! Get a const reference to the One argument
          map of commands indexed by name.
          This is used by the Composite device to contruct its own map, which
          is concatenation of the maps of it constituent devices.
         */
        OneArgOperationNameMapType & GetDeviceMapOneArg();

        /*! Get a const reference to the Two argument
          map of commands indexed by name.
          This is used by the Composite device to contruct its own map, which
          is concatenation of the maps of it constituent devices.
         */
        TwoArgOperationNameMapType & GetDeviceMapTwoArg();

        ddiCommand0Base* GetMethodZeroArg(std::string operation);
        ddiCommand1Base* GetMethodOneArg(std::string operation);
        ddiCommand2Base* GetMethodTwoArg(std::string operation);

        template <class __DeviceType>
            inline void AddMethodZeroArg(ddiCommandBase::ReturnType (__DeviceType::*action)(),
                    __DeviceType *dev, const char *name) {
                ZeroArgOperations[name] = new ddiCommand0<__DeviceType>(action, dev, name);
            }

        template <class __DeviceType>
            inline void AddMethodOneArg(ddiCommandBase::ReturnType (__DeviceType::*action)(cmnDataObject &),
                    __DeviceType *dev, const char *name, const std::type_info *info, const cmnClassServicesBase *classInputTypeServices) {
                OneArgOperations[name] = new ddiCommand1<__DeviceType>(action, dev, name, info, classInputTypeServices);
            }

        template <class __DeviceType>
            inline void AddMethodTwoArg(ddiCommandBase::ReturnType (__DeviceType::*action)(cmnDataObject &, cmnDataObject &),
                    __DeviceType *dev, const char *name, const std::type_info *infoFrom, const std::type_info *infoTo) {
                TwoArgOperations[name] = new ddiCommand2<__DeviceType>(action, dev, name, infoFrom, infoTo);
            }

protected:

        ZeroArgOperationNameMapType ZeroArgOperations;
        OneArgOperationNameMapType  OneArgOperations;
        TwoArgOperationNameMapType  TwoArgOperations;

};


#endif // _ddiDeviceInterface_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: ddiDeviceInterface.h,v $
// Revision 1.16  2006/06/03 00:27:45  kapoor
// cisstDeviceInterface: Attempt to come with a self describing command object.
// We now also store the they of input/output data type for the cmnDataObject
// class and its derivatives. Moreover some of these could be dynamically created.
// See sine7 (the new example in realtime tutorial task).
//
// Revision 1.15  2006/05/13 21:26:04  kapoor
// cisstDeviceInterface: Missed Ampersand (not passed by reference) in concat maps
// lead to a silly bug.
//
// Revision 1.14  2006/05/09 03:29:47  kapoor
// cisstDeviceInterface: Added RTTI typeinfo of expected input/output to
// ddiCommand object. This allows checking for correct type in the mailbox
// command.
//
// Revision 1.13  2006/05/09 02:26:58  kapoor
// cisstDeviceInterface and IRE: wrapping of the ddiCommand[0-2]Base class.
//
// Revision 1.12  2006/05/07 05:19:55  kapoor
// ddiDeviceInterface and IRE: Wrapping of ddiCommand[0-2] objects. Added
// a new macro, which returns the exact type of command instead of base type.
// This needs to be changed, not the most optimal way.
//
// Revision 1.11  2006/05/05 05:07:37  kapoor
// cisstDeviceInterface: Provides method is no longer pure virtual. It can be
// overloaded to provide a non-default behaviour. By default it return, the
// list of name strings of all commands.
//
// Revision 1.10  2006/05/04 05:46:45  kapoor
// Extended command0,1 to tasks. Appears to work. More testing needed.
//
// Revision 1.9  2006/05/03 02:28:34  kapoor
// These compile. For easy tranfer to Anton. No gurantees on functionality.
//
// Revision 1.8  2006/05/03 01:49:59  kapoor
// These compile. For easy tranfer to Anton. No gurantees on functionality.
//
// Revision 1.7  2006/05/02 20:37:01  anton
// ddi and rts: Modified #include to use newly created ddiTypes.h
//
// Revision 1.6  2005/09/26 15:41:46  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.5  2005/06/19 21:42:10  kapoor
// ddiDeviceInterface: removed unnecessary #include of iostream.
//
// Revision 1.4  2005/06/17 22:45:25  anton
// cisst libraries: Added some CISST_EXPORT required to compile DLLs
//
// Revision 1.3  2005/04/16 21:34:42  kapoor
// Now uses dynamic vectors instead of std vector. Still retainins the name
// because changing the name would lead to HUGE modifications in real time
// code.
//
// Revision 1.2  2005/02/28 22:43:34  anton
// cisstDevices and real-time:  Added Log (see #130).
//
// Revision 1.1  2004/05/04 14:12:11  anton
// Imported code from Ankur
//
//
// Revision 1.7  2004/04/08 06:01:24  kapoor
// map has problem with const std::string on windows. changed to std::string
//
// Revision 1.6  2004/03/26 06:03:49  kapoor
// Removed keyword const from ddiCommand*
//
// Revision 1.5  2004/03/24 20:48:32  kapoor
// Added lots of documentation.
//
// Revision 1.4  2004/03/22 00:31:20  kapoor
// Added new devices.
//
// Revision 1.3  2004/03/19 16:00:57  kapoor
// More comments.
//
// Revision 1.2  2004/03/17 16:46:58  kapoor
// Added full functionality.
//
// Revision 1.1  2004/03/16 17:58:07  kapoor
// Started work on Device Interface.
//
// ****************************************************************************
