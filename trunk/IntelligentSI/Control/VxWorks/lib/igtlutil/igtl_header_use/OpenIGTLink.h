/***************************************************************************
 * FileName      : OpenIGTLinkHeader.h
 * Created       : 2008/2/5
 * LastModified  : 2008/
 * Author        : Hiroaki KOZUKA
 * Aim           : conect other device
 *
 * OS            : VxWorks 5.5.1
 ***************************************************************************/
#ifndef OPEN_IGT_LINK__H
#define OPEN_IGT_LINK__H

#include <string.h>
#include <iostream>

#include "igtl_header.h"
#include "igtl_util.h"
#include "igtl_transform.h"

//! OpenIGTLink header class
class OpenIGTLink {
public:
  //!
  OpenIGTLink();
  //!
  ~OpenIGTLink();
  //!
  igtl_header InitOpenIGTHeader( char* dataType, char* deviceName,
                             unsigned long long  timeStamp, int dataSize );
  //!
  void PackOpenIGTLHeader( igtl_header* igtlHeader, char* data, unsigned long long timeStamp );
  //!
  bool OpenOpenIGTLHeader( igtl_header* igtlHeader, char* data );

};

#endif // OPEN_IGT_LINK__H
