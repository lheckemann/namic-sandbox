/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */


/*
  $Id: ddiTypes.h,v 1.8 2006/07/13 22:08:02 pkaz Exp $
  
  Author(s):  Ankur Kapoor, Anton Deguet
  Created on:  2006-05-05

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


#ifndef _ddiTypes_h
#define _ddiTypes_h

#include <cisstCommon/cmnTypeTraits.h>
#include <cisstCommon/cmnDataObject.h>
#include <cisstDeviceInterface/ddiDynamicVectorDataObject.h>
#include <cisstDeviceInterface/ddiFixedSizeVectorDataObject.h>
#include <cisstDeviceInterface/ddiBasicTypeDataObject.h>

// Always include last!
#include <cisstDeviceInterface/ddiExport.h>

typedef ddiDynamicVectorDataObject<double, 16> ddiVecDouble16;
typedef ddiDynamicVectorDataObject<double, 8> ddiVecDouble8;
typedef ddiDynamicVectorDataObject<double, 4> ddiVecDouble4;
typedef ddiDynamicVectorDataObject<long, 16> ddiVecLong16;
typedef ddiDynamicVectorDataObject<long, 8> ddiVecLong8;
typedef ddiDynamicVectorDataObject<long, 4> ddiVecLong4;
typedef ddiDynamicVectorDataObject<int, 16> ddiVecInt16;
typedef ddiDynamicVectorDataObject<int, 8> ddiVecInt8;
typedef ddiDynamicVectorDataObject<int, 4> ddiVecInt4;
typedef ddiDynamicVectorDataObject<int, 2> ddiVecInt2;  //GSF
typedef ddiDynamicVectorDataObject<short, 16> ddiVecShort16;
typedef ddiDynamicVectorDataObject<short, 8> ddiVecShort8;
typedef ddiDynamicVectorDataObject<short, 4> ddiVecShort4;
typedef ddiDynamicVectorDataObject<bool, 16> ddiVecBool16;
typedef ddiDynamicVectorDataObject<bool, 8> ddiVecBool8;
typedef ddiDynamicVectorDataObject<bool, 4> ddiVecBool4;


typedef ddiBasicTypeDataObject<double> ddiDouble;
typedef ddiBasicTypeDataObject<long> ddiLong;
typedef ddiBasicTypeDataObject<int> ddiInt;
typedef ddiBasicTypeDataObject<short> ddiShort;
typedef ddiBasicTypeDataObject<bool> ddiBool;

CMN_DECLARE_SERVICES_INSTANTIATION(ddiVecDouble16);
CMN_DECLARE_SERVICES_INSTANTIATION(ddiVecDouble8);
CMN_DECLARE_SERVICES_INSTANTIATION(ddiVecDouble4);
CMN_DECLARE_SERVICES_INSTANTIATION(ddiVecLong16);
CMN_DECLARE_SERVICES_INSTANTIATION(ddiVecLong8);
CMN_DECLARE_SERVICES_INSTANTIATION(ddiVecLong4);
CMN_DECLARE_SERVICES_INSTANTIATION(ddiVecInt16);
CMN_DECLARE_SERVICES_INSTANTIATION(ddiVecInt8);
CMN_DECLARE_SERVICES_INSTANTIATION(ddiVecInt4);
CMN_DECLARE_SERVICES_INSTANTIATION(ddiVecInt2);    //GSF
CMN_DECLARE_SERVICES_INSTANTIATION(ddiVecShort16);
CMN_DECLARE_SERVICES_INSTANTIATION(ddiVecShort8);
CMN_DECLARE_SERVICES_INSTANTIATION(ddiVecShort4);
CMN_DECLARE_SERVICES_INSTANTIATION(ddiVecBool16);
CMN_DECLARE_SERVICES_INSTANTIATION(ddiVecBool8);
CMN_DECLARE_SERVICES_INSTANTIATION(ddiVecBool4);

CMN_DECLARE_SERVICES_INSTANTIATION(ddiDouble);
CMN_DECLARE_SERVICES_INSTANTIATION(ddiLong);
CMN_DECLARE_SERVICES_INSTANTIATION(ddiInt);
CMN_DECLARE_SERVICES_INSTANTIATION(ddiShort);
CMN_DECLARE_SERVICES_INSTANTIATION(ddiBool);

/*! Temp will be deleted. */
class CISST_EXPORT ddiFTDataObject : public cmnDataObject {
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, 5);
    public:
        /*! Temp will be deleted. */
        double FTData[8];
        /*! Temp will be deleted. */
        ddiFTDataObject(){}
        /*! Temp will be deleted. */
        ~ddiFTDataObject(){}
        /*! Temp will be deleted. */
        virtual long TypeId(void) const {return 167;}
        virtual std::string TypeName(void) const { return "ddiFTDataObject"; }

    std::string ToString(void) const {
        std::stringstream outputStream;
        ToStream(outputStream);
        return outputStream.str();
    }
        /*! Temp will be deleted. */
        virtual void ToStream(std::ostream &out) const {
            out << " { ";
            for(unsigned int i = 0; i < 7; i++) {
                out << FTData[i] << ", ";
            }
            out << FTData[7] << " } ";
        }
        /*! Temp will be deleted. */
        virtual void pack(PackedTypeArray &packarray) {
            for (unsigned int i = 0; i < 7; i++) {
                packarray << FTData[i];
            }
        }
        /*! Temp will be deleted. */
        virtual cmnDataObject& unpack(PackedTypeArray &unpackarray) {
            for (unsigned int i = 7; i > 0; i--) {
                unpackarray >> FTData[i-1];
            }
            return *this;
        }
};
CMN_DECLARE_SERVICES_INSTANTIATION(ddiFTDataObject);

/*! Temp will be deleted. */
class CISST_EXPORT ddiFNTNmDataObject : public cmnDataObject {
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, 5);
    public:
        /*! Temp will be deleted. */
        unsigned long Sequence, Condition;
        /*! Temp will be deleted. */
        double Forces[3];
        /*! Temp will be deleted. */
        double Torques[3];
        /*! Temp will be deleted. */
        ddiFNTNmDataObject(){}
        /*! Temp will be deleted. */
        ~ddiFNTNmDataObject(){}
        /*! Temp will be deleted. */
        virtual long TypeId(void) const {return 176;}
        virtual std::string TypeName(void) const { return "ddiFNTNmDataObject"; }
        /*! Temp will be deleted. */

    std::string ToString(void) const {
        std::stringstream outputStream;
        ToStream(outputStream);
        return outputStream.str();
    }
        virtual void ToStream(std::ostream &out) const {
            out << "< { " << Sequence << ", " << Condition << " }, ";
            out << "{ " << Forces[0] << ", " << Forces[1] << ", " << Forces[2] << " }, ";
            out << "{ " << Torques[0] << ", " << Torques[1] << ", " << Torques[2] << " } >";
        }
        /*! Temp will be deleted. */
        virtual void pack(PackedTypeArray &packarray) {
            packarray << Sequence << Condition;
            packarray << Forces[0] << Forces[1] << Forces[2];
            packarray << Torques[0] << Torques[1] << Torques[2];
        }
        /*! Temp will be deleted. */
        virtual cmnDataObject& unpack(PackedTypeArray &unpackarray) {
            unpackarray >> Torques[2]; unpackarray >> Torques[1]; unpackarray >> Torques[0];
            unpackarray >> Forces[2]; unpackarray >> Forces[1]; unpackarray >> Forces[0];
            unpackarray >> Condition;
            unpackarray >> Sequence;
            return *this;
        }
};
CMN_DECLARE_SERVICES_INSTANTIATION(ddiFNTNmDataObject);

/* class to store and pass the PID++etc around mailbox, python etc */
class ddiGainData : public cmnDataObject {
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, 5);
    public:
        int AxisNumber;
        double PGain;
        double IGain;
        double DGain;
        double AccelerationFF;
        double VelocityFF;
        double ILimit;
        double ErrorLimit;
        double Offset;
        double DACLimit;
        double OScale;
        double FrictionFF;
        ddiGainData():
            AxisNumber(0),
            PGain(0),
            IGain(0),
            DGain(0),
            AccelerationFF(0),
            VelocityFF(0),
            ILimit(0),
            ErrorLimit(0),
            Offset(0),
            DACLimit(0),
            OScale(0),
            FrictionFF(0) {
            }
        /*! Temp will be deleted. */
        ~ddiGainData(){}
        /*! Temp will be deleted. */
        virtual long TypeId(void) const {return 678;}
        virtual std::string TypeName(void) const { return "ddiGainData"; }
        void Assign(const ddiGainData &that) {
            AxisNumber = that.AxisNumber;
            PGain = that.PGain; IGain = that.IGain; DGain = that.DGain;
            AccelerationFF = that.AccelerationFF; VelocityFF = that.VelocityFF;
            ILimit = that.ILimit; Offset = that.Offset;
            DACLimit = that.DACLimit; OScale = that.OScale;
            FrictionFF = that.FrictionFF;
            ErrorLimit = that.ErrorLimit;
        }
        ddiGainData & operator= (const ddiGainData &that) { this->Assign(that); return *this; }
        /*! Temp will be deleted. */

    std::string ToString(void) const {
        std::stringstream outputStream;
        ToStream(outputStream);
        return outputStream.str();
    }
        virtual void ToStream(std::ostream &out) const {
            out << AxisNumber
                << ", " << PGain
                << ", " << IGain
                << ", " << DGain
                << ", " << AccelerationFF
                << ", " << VelocityFF
                << ", " << ILimit
                << ", " << ErrorLimit
                << ", " << Offset
                << ", " << DACLimit
                << ", " << OScale
                << ", " << FrictionFF;
        }
        /*! Temp will be deleted. */
        virtual void pack(PackedTypeArray &packarray) {
            packarray << PGain
                << IGain
                << DGain
                << AccelerationFF
                << VelocityFF
                << ILimit
                << ErrorLimit
                << Offset
                << DACLimit
                << OScale
                << FrictionFF
                << AxisNumber;
        }
        /*! Temp will be deleted. */
        virtual cmnDataObject& unpack(PackedTypeArray &unpackarray) {
            unpackarray >> AxisNumber;
            unpackarray >> FrictionFF;
            unpackarray >> OScale;
            unpackarray >> DACLimit;
            unpackarray >> Offset;
            unpackarray >> ILimit;
            unpackarray >> ErrorLimit;
            unpackarray >> VelocityFF;
            unpackarray >> AccelerationFF;
            unpackarray >> DGain;
            unpackarray >> IGain;
            unpackarray >> PGain;
            return *this;
        }
};
CMN_DECLARE_SERVICES_INSTANTIATION(ddiGainData);

template<>
CISST_EXPORT
std::string cmnTypeTraits<ddiGainData>::TypeName();


#endif // _rtsDataObject_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: ddiTypes.h,v $
// Revision 1.8  2006/07/13 22:08:02  pkaz
// ddiDeviceInterface: added CISST_EXPORT for Windows.
//
// Revision 1.7  2006/06/03 00:27:45  kapoor
// cisstDeviceInterface: Attempt to come with a self describing command object.
// We now also store the they of input/output data type for the cmnDataObject
// class and its derivatives. Moreover some of these could be dynamically created.
// See sine7 (the new example in realtime tutorial task).
//
// Revision 1.6  2006/05/11 20:11:46  anton
// ddiTypes: Required modifications for Visual C++ DLLs.
//
// Revision 1.5  2006/05/07 04:45:26  kapoor
// cisstDeviceInterface and IRE: Fixed printing of dataobjects through python.
//
// Revision 1.4  2006/05/05 05:05:46  kapoor
// cisstDeviceInterface wrapping: Moved GainData to ddiTypes (to be split later),
// wrappers for basic types (double, int etc) to be passed through mailbox.
//
// Revision 1.3  2006/05/03 05:33:42  kapoor
// MACROS for DDI_COMMAND* declaration, implementation etc.
//
// Revision 1.2  2006/05/02 21:18:38  anton
// Data object (cmn and ddi): Renamed StrOut to ToString for consistency.
//
// Revision 1.1  2006/05/02 20:35:22  anton
// Data objects for ddi and rts: Moved data definition to the device interface
// level and renamed the two classes of "vectors".  Updated LoPoMoCo to verify
// compilation.
//
//
// ****************************************************************************
