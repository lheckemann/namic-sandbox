/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: cmnDataObject.h,v 1.19 2006/06/05 03:11:21 kapoor Exp $
  
  Author(s):  Ankur Kapoor
  Created on: 

--- begin cisst license - do not edit ---

CISST Software License Agreement(c)

Copyright 2005 Johns Hopkins University (JHU) All Rights Reserved.

This software ("CISST Software") is provided by The Johns Hopkins
University on behalf of the copyright holders and
contributors. Permission is hereby granted, without payment, to copy,
modify, display and distribute this software and its documentation, if
any, for research purposes only, provided that (1) the above copyright
notice and the following four paragraphs appear on all copies of this
software, and (2) that source code to any modifications to this
software be made publicly available under terms no more restrictive
than those in this License Agreement. Use of this software constitutes
acceptance of these terms and conditions.

The CISST Software has not been reviewed or approved by the Food and
Drug Administration, and is for non-clinical, IRB-approved Research
Use Only.

IN NO EVENT SHALL THE COPYRIGHT HOLDERS AND CONTRIBUTORS BE LIABLE TO
ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION,
EVEN IF THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE BEEN ADVISED OF
THE POSSIBILITY OF SUCH DAMAGE.

THE COPYRIGHT HOLDERS AND CONTRIBUTORS SPECIFICALLY DISCLAIM ANY
EXPRESS OR IMPLIED WARRANTIES INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
PURPOSE, AND NON-INFRINGEMENT.

THE SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
IS." THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE NO OBLIGATION TO
PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

--- end cisst license ---

*/


#ifndef _cmnDataObject_h
#define _cmnDataObject_h

#include <cisstCommon/cmnTypeTraits.h>
#include <cisstCommon/cmnGenericObject.h>
#include <cisstCommon/cmnClassRegister.h>

#include <ostream>
#include <vector>

// Always include last
#include <cisstCommon/cmnExport.h>

#define PACK_ARRAY_SIZE 20

/*! Extend this to include all builtin types */
union PackedType {
  bool ___b;
  char ___c;
  short ___s;
  int ___i;
  long ___l;
  float ___f;
  double ___d;
  unsigned long ___ul;
};

class PackedTypeArray;

class cmnDataObject : public cmnGenericObject {
public:
    int Index;
  cmnDataObject():Index(-1){};
  virtual ~cmnDataObject(){};

  virtual long TypeId(void) const = 0;
  virtual std::string TypeName(void) const = 0;
  virtual std::string ToString(void) const = 0;
  virtual void ToStream(std::ostream& out) const = 0;
  virtual void pack(PackedTypeArray &packarray) = 0;
  virtual cmnDataObject& unpack(PackedTypeArray &unpackarray) = 0;
  CISST_EXPORT friend std::ostream& operator << (std::ostream& out, const cmnDataObject& data);
};


class PackedTypeArray {
protected:
  int Size;
  PackedType Data[PACK_ARRAY_SIZE];
public:
  PackedTypeArray():Size(0){}
  ~PackedTypeArray(){}
  PackedTypeArray & operator<< (bool ivalue) {
    Data[Size].___b = ivalue;
    Size++;
    return *this;
  }
  PackedTypeArray & operator<< (char ivalue) {
    Data[Size].___c = ivalue;
    Size++;
    return *this;
  }
  PackedTypeArray & operator<< (short ivalue) {
    Data[Size].___s = ivalue;
    Size++;
    return *this;
  }
  PackedTypeArray & operator<< (int ivalue) {
    Data[Size].___i = ivalue;
    Size++;
    return *this;
  }
  PackedTypeArray & operator<< (long ivalue) {
    Data[Size].___l = ivalue;
    Size++;
    return *this;
  }
  PackedTypeArray & operator<< (float ivalue) {
    Data[Size].___f = ivalue;
    Size++;
    return *this;
  }
  PackedTypeArray & operator<< (double ivalue) {
    Data[Size].___d = ivalue;
    Size++;
    return *this;
  }
  PackedTypeArray & operator<< (unsigned long ivalue) {
    Data[Size].___ul = ivalue;
    Size++;
    return *this;
  }
  PackedTypeArray & operator<< (cmnDataObject &obj) {
    obj.pack(*this);
    return *this;
  }
  PackedTypeArray & operator>> (bool &ivalue) {
    if (Size > 0) { ivalue = Data[--Size].___b; }
    return *this;
  }
  PackedTypeArray & operator>> (char &ivalue) {
    if (Size > 0) { ivalue = Data[--Size].___c; }
    return *this;
  }
  PackedTypeArray & operator>> (short &ivalue) {
    if (Size > 0) { ivalue = Data[--Size].___s; }
    return *this;
  }
  PackedTypeArray & operator>> (int &ivalue) {
    if (Size > 0) { ivalue = Data[--Size].___i; }
    return *this;
  }
  PackedTypeArray & operator>> (long &ivalue) {
    if (Size > 0) { ivalue = Data[--Size].___l; }
    return *this;
  }
  PackedTypeArray & operator>> (float &ivalue) {
    if (Size > 0) { ivalue = Data[--Size].___f; }
    return *this;
  }
  PackedTypeArray & operator>> (double &ivalue) {
    if (Size > 0) { ivalue = Data[--Size].___d; }
    return *this;
  }
  PackedTypeArray & operator>> (unsigned long &ivalue) {
    if (Size > 0) { ivalue = Data[--Size].___ul; }
    return *this;
  }
  PackedTypeArray & operator>> (cmnDataObject &obj) {
    obj.unpack(*this);
    return *this;
  }
  const PackedType & operator[](unsigned int index){return Data[index];}
  int size() { return Size; }
  void flush(void) { Size = 0; }
};


#endif // _cmnDataObject_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: cmnDataObject.h,v $
// Revision 1.19  2006/06/05 03:11:21  kapoor
// cisstRealTime: Mods to state data table. This mod eliminates the need to
// maintain the enumerations in the class. But this requires one to maintain
// a member variable to hold the current state.
//
// Revision 1.18  2006/06/03 00:27:45  kapoor
// cisstDeviceInterface: Attempt to come with a self describing command object.
// We now also store the they of input/output data type for the cmnDataObject
// class and its derivatives. Moreover some of these could be dynamically created.
// See sine7 (the new example in realtime tutorial task).
//
// Revision 1.17  2006/05/07 04:45:26  kapoor
// cisstDeviceInterface and IRE: Fixed printing of dataobjects through python.
//
// Revision 1.16  2006/05/05 05:05:46  kapoor
// cisstDeviceInterface wrapping: Moved GainData to ddiTypes (to be split later),
// wrappers for basic types (double, int etc) to be passed through mailbox.
//
// Revision 1.15  2006/05/05 03:32:04  kapoor
// cisstCommon: Default constructor for cmnGainData.
//
// Revision 1.14  2006/05/02 21:18:38  anton
// Data object (cmn and ddi): Renamed StrOut to ToString for consistency.
//
// Revision 1.13  2005/09/26 15:41:46  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.12  2005/06/19 22:13:52  kapoor
// cmnDataObject: Added ErrorLimit to gains data
//
// Revision 1.11  2005/06/19 21:47:41  kapoor
// cmnDataObject: BUG FIX. Missing virtual method TypeName in some derived classes of cmnDataObject.
//
// Revision 1.10  2005/06/17 20:30:50  alamora
// cisstOSAbstraction and cisstRealTime: Added some required
// CISST_EXPORT to compile DLLs.
//
// Revision 1.9  2005/05/19 19:29:00  anton
// cisst libs: Added the license to cisstCommon and cisstVector
//
// Revision 1.8  2005/04/25 15:01:59  kapoor
// osaMailBox: Moved windows code for mailbox lying in my folders to main repository. This might change in future. Some limitations exists in using mailbox of windows, the main concern being the size of data transferred is limited to 454 bytes in windows.
//
// Revision 1.7  2005/04/24 16:50:13  kapoor
// cmnDataObject: value was passed as parameter, but was not used!
//
// Revision 1.6  2005/04/16 21:34:42  kapoor
// Now uses dynamic vectors instead of std vector. Still retainins the name
// because changing the name would lead to HUGE modifications in real time
// code.
//
// Revision 1.5  2005/02/28 22:42:26  anton
// cmnDataObject: Changed default size of vector, use template parameter now
// (it was set to 4 before).  This is a bad solution which needs to be changed.
//
// Revision 1.4  2004/05/27 15:11:07  anton
// Added TypeName method for cmnGainData
//
// Revision 1.3  2004/05/11 17:52:21  anton
// Added TypeName() method for the cmnBasicTypeDataObject class.  Uses
// cmnTypeTraits::TypeName().  This is becomming kind of redundant.
//
// Revision 1.2  2004/05/11 16:34:43  kapoor
// Checked in **PREMATURE** dynamic object creation code. NO flames PLEASE
//
// Revision 1.1  2004/05/04 14:12:11  anton
// Imported code from Ankur
//
//
// Revision 1.4  2004/04/08 06:18:54  kapoor
// Packed array was added
//
// Revision 1.3  2004/03/26 06:10:04  kapoor
// Added pack and unpack.
//
// Revision 1.2  2004/03/17 16:45:58  kapoor
// Changed to cmnDataObject.
//
// Revision 1.1  2004/03/16 18:02:15  kapoor
// Added cmnDataObject.
//
// ****************************************************************************
