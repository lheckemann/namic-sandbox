/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

#include "devRMM1612.h"

#include <cisstCommon/cmnXMLPath.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>
#include <limits.h>


CMN_IMPLEMENT_SERVICES(devRMM1612);


devRMM1612::devRMM1612(const std::string & deviceName, bool doInit):
    mtsDevice(deviceName)
{
    mtsInterfaceProvided *provided = AddInterfaceProvided("MainInterface");
    if (provided) {
        provided->AddCommandWrite(&devRMM1612::SetOutputAll, this,
                                 "SetOutputAll", mtsDoubleVec(16));
    }

    //GSF
    // Only once per program, not per board
    if (doInit)
        dscInit( DSC_VERSION );
}

devRMM1612::~devRMM1612() {
    free(dscdacs.output_codes);
    dscFree();
}

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


void devRMM1612::Configure(const std::string &filename) {
    
    cmnXMLPath xmlConfig;
    if (filename == "") {
        CMN_LOG_CLASS_INIT_WARNING << "Warning, could not configure RMM1612 device" << std::endl;
        return;
    }
    
    struct stat st;
    if (stat(filename.c_str(), &st) < 0) {
        CMN_LOG_CLASS_INIT_WARNING << "Invalid filename!! " << filename << std::endl;
        return;
    }
    
    xmlConfig.SetInputSource(filename);
    std::string context("/Config/Device[@Name=\"");
    context = context + GetName() + "\"]";

    CMN_LOG_CLASS_INIT_VERBOSE << "Configuring RMM1612 Board with \"" << filename << "\"" << std::endl;
    //read in the base address and axis factors
    xmlConfig.GetXMLValue(context.c_str(), "@BaseAddress", BaseAddress);
    
    //int SelectorSwitchPosition;
    //if (ret == false) {
    //    // try switch selector
    //    xmlConfig.GetXMLValue(context.c_str(), "@SelectorSwitchPosition", SelectorSwitchPosition);
    //    BaseAddress = 0x200 + SelectorSwitchPosition * 0x20;
    //}
    
    DACRangeLow = 0;
    DACRangeHigh = 10;
    CMN_LOG_CLASS_INIT_VERBOSE << "BaseAddress: " << BaseAddress << std::endl;
    CMN_LOG_CLASS_INIT_VERBOSE << "DAC Min. Voltage: " << DACRangeLow << " ,  DAC Max. Voltage: " << DACRangeHigh << std::endl;
    
    /////
    // Retrieve these settings from config file
    /////
    // Initialize board
    dsccb.boardtype = DSC_RMM1612;
    //dsccb.base_address = 0x340;
    dsccb.base_address = BaseAddress;
    //dsccb.int_level = 3;
    dsccb.RMM_external_trigger = 1;
    
    dscInitBoard(DSC_RMM1612, &dsccb, &dscb);

    dscdacs.output_codes = (DSCDACODE*)malloc( sizeof(DSCDACODE) * 16 );

    for ( int i=0; i<16; i++){
        dscdacs.channel_enable[i] = TRUE;
        dscdacs.output_codes[i] = 0;
    }

    CMN_LOG_CLASS_INIT_VERBOSE << "Configured RMM1612" << std::endl;
}

//GSF
void devRMM1612::SetOutputAll(const mtsDoubleVec &output)
{
    int limit = 16;
    if (output.size() > limit)
        CMN_LOG_CLASS_INIT_WARNING << "SetOutputAll: output vector too large, size = " << output.size() << std::endl;
    else if (output.size() < limit) {
        CMN_LOG_CLASS_INIT_WARNING << "SetOutputAll: output vector too small, size = " << output.size() << std::endl;
        limit = output.size();
    }

    // NEED TO CONVERT VOLTAGES -> DAC OUTPUT VALUES BASED ON RANGE IN CONFIG FILE & SET BY JUMPERS
    
    double DAC_voltage[16];
    unsigned long DAC_raw[16];
    for(int i=0; i<limit; i++){
        // Get command voltages
        DAC_voltage[i] = output[i];

        // Check range of command and saturate at max/min
        if (DAC_voltage[i]>DACRangeHigh)
            DAC_voltage[i] = DACRangeHigh;
        if (DAC_voltage[i]<DACRangeLow)
            DAC_voltage[i] = DACRangeLow;

        //Determine corresponding DAC raw output
        DAC_raw[i] = static_cast<unsigned long>(floor(( DAC_voltage[i] / (DACRangeHigh - DACRangeLow) ) * 4095 + 0.5));

        // Set raw output values, Not transfered to DAC yet
        dscdacs.output_codes[i] = DAC_raw[i];
    }

    // Start conversion on DAC
    dscDAConvertScan(dscb,&dscdacs);
}      
