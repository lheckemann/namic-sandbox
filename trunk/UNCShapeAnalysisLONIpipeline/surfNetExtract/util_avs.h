// -*- C++ -*-
/****************************************************************************
 *
 *  util_avs.h
 *
 ****************************************************************************
 *
 *  File         :  util_avs.h
 *  Type         :  
 *  Purpose      :  
 *
 ****************************************************************************
 *                 
 *  started      :  07 Juli 94    Georg Israel
 *  adapted      :  15 Nov  94    Martin Berger
 *  last change  :  28 Feb  95    Robert Schweikert (deleted (lots of) stuff)
 *
 ****************************************************************************/

#ifndef _UTIL_AVS_H
#define  _UTIL_AVS_H

#ifdef NO_AVS
typedef void GEOMedit_list;
#else
#include "common_avs.h"
#include "util.h"

int  WhichChoice(char const * which,
     char const * const choices[]);

void GenerateChoice(char * OutString,
         const char * const Choices[],
         const char * Seperator);


#endif NO_AVS

#endif
