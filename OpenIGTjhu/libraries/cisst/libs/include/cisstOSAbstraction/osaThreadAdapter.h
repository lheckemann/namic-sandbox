/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: osaThreadAdapter.h,v 1.7 2006/03/18 04:47:13 anton Exp $
  
  Author(s): Ankur Kapoor
  Created on: 2004-04-30

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


/*!
  \file
  \brief Defines a thread adapter 
  \ingroup cisstOSAbstraction
*/

#ifndef _osaThreadAdapter_h
#define _osaThreadAdapter_h

#include <cisstCommon/cmnPortability.h>

#if (CISST_OS == CISST_WINDOWS)
#include <windows.h>
#endif

/*!
  \ingroup cisstOSAbstraction

  This struct is used as base class for all callback adapters.  It
  just declares and initializes the adapters member data, so we don't
  have to do this separately for each single adapter.
*/
template<class _objectType, class _callBackMethodType, class _userDataType>
struct osaCallBackBase {
    /*! Typedef for osaCallBackBase */
    typedef osaCallBackBase osaCallBackBaseType;

    /*! Constructor. Stores the pointer to member function of object
      class _objectType and the user object of class _userDataType.

     \param obj The pointer to the object containing the member
     function to be called.

     \param callBackFunction The pointer to the member function to be
     called.

     \param userData The user data to be passed to the callback member
     function.
    */
    osaCallBackBase(_objectType* obj, _callBackMethodType callBackFunction,
                    _userDataType userData)
        : Obj(obj), CallBackFunction(callBackFunction), UserData(userData) {}


    /*! The pointer to the object containing the member function to be called. */
    _objectType* Obj;

    /*! The pointer to the member function to be called. */
    _callBackMethodType CallBackFunction;

    /*! A copy of the user data to be passed to the callback member function. */
    _userDataType UserData;
};


/*!
  \ingroup cisstOSAbstraction

  Adapter for callback functions with one user defined argument and
  adapter instance created on the heap. The adapter instance is
  automatically destroyed with delete after processing the
  callback. This is used for CreateThread() R MyCallback( A1 arg1 ) is
  redirected to [virtual] R O::MyMemberCallback( U UserData );
 */
template<class _objectType, class _userDataType,
         class _callBackReturnType, class _callBackArgumentType >
struct osaHeapCallBack: public osaCallBackBase<_objectType, _callBackReturnType(_objectType::*)(_userDataType), _userDataType> {

    /*! Creates an object that can be passed as argument of the 'start
      routine'. */
    static osaHeapCallBack* Create(_objectType* obj,
                                   _callBackReturnType (_objectType::*callBackFunction)(_userDataType),
                                   _userDataType userData) {
        return new osaHeapCallBack(obj, callBackFunction, userData);
    }

    typedef osaCallBackBase<_objectType, _callBackReturnType(_objectType::*)(_userDataType), _userDataType> BaseType;

    /*! The static function to be passed as the 'start routine'
      argument of functions such as pthread_create. We use the
      argument to the 'start routine' to send an object that packs the
      member function to be called, along with the receiver object and
      user data.
    
      \param obj This object must be of type _callBackArgumentType,
      which is the same as that of 'start routine' prototype defined
      by the platform.

      \returns The result of the callback method which must be of type
      _callBackReturnType type which is the same as the return type pf
      the 'start routine' defined by the platform. */
#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN)
    static void * CallbackAndDestroy(_callBackArgumentType obj) {
        osaHeapCallBack* _this = (osaHeapCallBack*) obj;
        _callBackReturnType result = (_this->Obj->*_this->CallBackFunction)(_this->UserData);
        //delete _this; // AK 2005-04-16-0121
        return (void*)result;
    }
#endif // CISST_LINUX_RTAI || CISST_LINUX

#if (CISST_OS == CISST_WINDOWS)
    static DWORD WINAPI CallbackAndDestroy(_callBackArgumentType obj) {
        osaHeapCallBack* _this = (osaHeapCallBack*) obj;
        _callBackReturnType result = (_this->Obj->*_this->CallBackFunction)(_this->UserData );
        //delete _this; // AK 2005-04-16-0121
        return (DWORD)result;
    }
#endif // CISST_WINDOWS


protected:
    /*! Constructor is protected to prevent instances on the stack. */
    osaHeapCallBack( _objectType* obj,
                     _callBackReturnType (_objectType::*callBackFunction)(_userDataType),
                     _userDataType userData )
        : BaseType(obj, callBackFunction , userData) {}
};


#endif // _osaThreadAdapter_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: osaThreadAdapter.h,v $
// Revision 1.7  2006/03/18 04:47:13  anton
// cisstOSAbstraction: Added license, replace tabs by spaces.
//
// Revision 1.6  2005/11/24 02:32:38  anton
// cisstOSAbstraction: Added compilation flags for Darwin (see #194).
//
// Revision 1.5  2005/09/26 15:41:47  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.4  2005/06/16 19:13:42  txia
// osaThreadAdapter: Defined BaseType and use it to call base constructor.
// Previous syntax wouldn't compile with gcc 3.4
//
// Revision 1.3  2005/04/16 21:38:04  kapoor
// Removed double delete. Checking in for safe keeping, since I dont want to loose this change.
//
// Revision 1.2  2004/05/28 21:50:21  anton
// Made modifications so that cisstOSAbstraction can compile on Linux with
// gcc and icc.  ThreadBuddy, Mailboxes and Semaphores are still missing.
//
// Revision 1.1  2004/05/04 14:12:11  anton
// Imported code from Ankur
//
//
// Revision 1.6  2004/04/08 05:58:34  kapoor
// Added win32 implementation
//
// Revision 1.5  2004/03/24 20:47:56  kapoor
// Added lots of documentation.
//
// Revision 1.4  2004/03/05 18:12:10  kapoor
// Missed out endif.
//
// Revision 1.3  2004/03/05 17:42:00  kapoor
// Removed extra adapters for _beginthreadx etc. etc. Will put them back in when we port to Windows.
//
// Revision 1.2  2004/03/04 21:26:14  kapoor
// more changes.
//
// Revision 1.1.1.1  2004/03/02 04:05:03  kapoor
// Importing source into local tree
//
// ****************************************************************************
