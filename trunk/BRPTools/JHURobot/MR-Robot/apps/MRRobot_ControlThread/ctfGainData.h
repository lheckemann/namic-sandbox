/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

// $Id$

#ifndef _ctfGainData_h
#define _ctfGainData_h


#include <cisstMultiTask/mtsGenericObject.h>
#include <cisstCommon/cmnClassRegister.h>

/* class to store and pass the PID++etc around mailbox, python etc */
class ctfGainData : public mtsGenericObject {
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);
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

    ctfGainData():
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
    ~ctfGainData(){}

    void Assign(const ctfGainData &that) {
        AxisNumber = that.AxisNumber;
        PGain = that.PGain; IGain = that.IGain; DGain = that.DGain;
        AccelerationFF = that.AccelerationFF; VelocityFF = that.VelocityFF;
        ILimit = that.ILimit; Offset = that.Offset;
        DACLimit = that.DACLimit; OScale = that.OScale;
        FrictionFF = that.FrictionFF;
        ErrorLimit = that.ErrorLimit;
    }

    ctfGainData & operator= (const ctfGainData &that) { this->Assign(that); return *this; }

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
};

CMN_DECLARE_SERVICES_INSTANTIATION(ctfGainData);

#endif // _ctfGainData_h
