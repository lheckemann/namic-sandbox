/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

// ****************************************************************************
//
//  $Id: rtsStateDataArray.h,v 1.3 2006/06/05 03:11:21 kapoor Exp $
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
  \brief Defines a state data array used in a state table.
  \ingroup cisstRealTime
*/

#ifndef _rtsStateDataArray_h
#define _rtsStateDataArray_h

#include <cisstRealTime/rtsStateDataArrayBase.h>

#include <vector>
#include <typeinfo>

/*!
  \ingroup cisstRealTime

  Individual state data classes can be created from an instance of
  the following template, where _stateDataType represents the type of
  data used by the particular state element. It is assumed that
  _stateDataType is derived from cmnDataObject.
 */
template <class _stateDataType>
class rtsStateDataArray :public rtsStateDataArrayBase {
protected:
  /*! A vector to store the data. These element of the vector
    represents the cell of the state data table. */
  std::vector<_stateDataType> Data;

public:
  /*! Default constructor. Does nothing */
  rtsStateDataArray(int size = 0): Data(size) {};

  /*! Default destructor. */
  virtual ~rtsStateDataArray() {};

  /*! Overloaded [] operator. Returns data at index */
  cmnDataObject& operator[](int index){
        return Data[index];
    }

  virtual const cmnDataObject& operator[](int index) const {
        return Data[index];
    }

  /* Create the array of data. */
  virtual rtsStateDataArrayBase* Create(int size) {
        Data.resize(size);
        return this;
    }

  /*! Copy data from one index to another. */
  virtual void Copy(int to, int from) {
        Data[to] = Data[from];
    }

  /*! Get data from array.  The Get and Set member functions deserve
    special mention because they must overcome a limitation of C++
    -- namely, that it does not fully support containers of
    heterogeneous objects. In particular, we expect the 'obj'
    parameter to be of type _stateDataType& (the derived class)
    rather than cmnDataObject& (the base class). This can be handled
    using C++ Run Time Type Information (RTTI) features such as
    dynamic cast.
   */
  virtual bool Get(int index, cmnDataObject& obj) const;

  /*! Set data in array.  The Get and Set member functions deserve
    special mention because they must overcome a limitation of C++
    -- namely, that it does not fully support containers of
    heterogeneous objects. In particular, we expect the 'obj'
    parameter to be of type _stateDataType& (the derived class)
    rather than cmnDataObject& (the base class). This can be handled
    using C++ Run Time Type Information (RTTI) features such as
    dynamic cast.
   */
  virtual bool Set(int index, const cmnDataObject& obj);
};


#include <iostream>

template <class _stateDataType>
bool rtsStateDataArray<_stateDataType>::Set(int index,  const cmnDataObject &obj) {
  //do some typechecking?? should this be an ASSERT?
  //TODO: check if throw works
  if (typeid(obj) != typeid(_stateDataType)) {
    CMN_LOG(1) << "The passed object is not of the same kind as array. Expected: "
                  << typeid(_stateDataType).name()
                  << "Got: " << typeid(obj).name() << std::endl;
    return false;
  }
  const _stateDataType* pdata = dynamic_cast<const _stateDataType*>(&obj);
  //const _stateDataType* pdata = &obj;
  if (pdata) {
    Data[index] = *pdata;
    return true;
  } else {
    CMN_LOG(1) << "Found NULL element in state data array" << std::endl;
  }
  return false;
}

template <class _stateDataType>
bool rtsStateDataArray<_stateDataType>::Get(int index, cmnDataObject &obj) const {
  //do some typechecking?? should this be an ASSERT?
  if (typeid(obj) != typeid(_stateDataType)) {
    CMN_LOG(1) << "The passed object is not of the same kind as array. Expected: "
                  << typeid(_stateDataType).name() 
                  << "Got: " << typeid(obj).name() << std::endl;
    return false;
  }
  _stateDataType* pdata = dynamic_cast<_stateDataType*>(&obj);
  //_stateDataType* pdata = &obj;
  if (pdata) {
    *pdata = Data[index];
    return true;
  }
  return false;
}


#endif // _rtsStateDataArray_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: rtsStateDataArray.h,v $
// Revision 1.3  2006/06/05 03:11:21  kapoor
// cisstRealTime: Mods to state data table. This mod eliminates the need to
// maintain the enumerations in the class. But this requires one to maintain
// a member variable to hold the current state.
//
// Revision 1.2  2005/09/26 15:41:47  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.1  2004/05/04 14:12:11  anton
// Imported code from Ankur
//
//
// Revision 1.4  2004/03/24 20:46:47  kapoor
// Added lots of documentation.
//
// Revision 1.3  2004/03/16 17:43:29  kapoor
// Changed to cmnDataObject. Added typeid checking for Get/Set Methods, this
// would go away later or would be only for debuging.
//
// Revision 1.2  2004/03/04 07:31:17  kapoor
// lots of fixes.
//
// Revision 1.1.1.1  2004/03/02 04:05:03  kapoor
// Importing source into local tree
//
// ****************************************************************************
