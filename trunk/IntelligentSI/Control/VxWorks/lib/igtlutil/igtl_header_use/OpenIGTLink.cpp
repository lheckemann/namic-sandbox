/******************************************************************************
 * FileName      : OpenIGTLink.cpp
 * Created       : 2008/2/5
 * LastModified  : 2008/
 * Author        : Nagoya Institute of Technology
 * Aim           : OpenIGTLink
 *
 * OS            : VxWorks 5.5.1
 *****************************************************************************/
#include "OpenIGTLink.h"

//-----------------------------------------------------------------------------
//! Constractor
OpenIGTLink::OpenIGTLink(){
}

//-----------------------------------------------------------------------------
//! Destractor
OpenIGTLink::~OpenIGTLink(){
}

//-----------------------------------------------------------------------------
//! Initialize OpenIGTLink Header. Return IGTLink Header
//! InitOpenIGTHeader( "DataType", "DeviceName",
//!                    Time stamp, size )

igtl_header
OpenIGTLink::InitOpenIGTHeader( char* dataType, char* deviceName,
                unsigned long long  timeStamp, int dataSize ){
  igtl_header igtlHeader;
  igtl_uint64 crc = crc64(0, 0, 0LL);
  igtlHeader.version = 1;

  strncpy(igtlHeader.name, dataType, 12);
  strncpy(igtlHeader.device_name, deviceName, 20);

  igtlHeader.timestamp = timeStamp;
  igtlHeader.body_size = dataSize;
  igtlHeader.crc = crc;

  return igtlHeader;
}

//-----------------------------------------------------------------------------
//! Pack OpenIGTLink Header(for sending header)
//! PackOpenIGTLHeader( OpenIGTLink header pointer,
//!                     (char*) & data pointer, time stamp )
void
OpenIGTLink::PackOpenIGTLHeader( igtl_header* igtlHeader, char* data,
                             unsigned long long  timeStamp ){
  igtl_uint64 crc = crc64(0, 0, 0LL);
  igtlHeader->crc = crc64( (unsigned char*)data, igtlHeader->body_size, crc);
  igtlHeader->timestamp = timeStamp;
  igtl_header_convert_byte_order(igtlHeader);
}

//-----------------------------------------------------------------------------
//! Open OpenIGTLink Header(for receiving header) Return bool
//! OpenOpenIGTLHeader( OpenIGTLink header pointer, (char*) & data pointer )
bool
OpenIGTLink::OpenOpenIGTLHeader( igtl_header* igtlHeader, char* data ){
  //
  igtl_header_convert_byte_order(igtlHeader);
  igtl_uint64 crc = crc64(0, 0, 0LL);
  //
  if( igtlHeader->crc != crc64( (unsigned char*)data,
                                igtlHeader->body_size, crc) ){
    return false;
  }
  return true;
}


