// -*- C++ -*-
/****************************************************************************
 *
 *  util.h
 *
 ****************************************************************************
 *
 *  File         :  util.h
 *  Type         :  
 *  Purpose      :  
 *
 ****************************************************************************
 *                 
 *  started      :  15 Nov 94   Robert Schweikert
 *  last change  :  19 Jan 95  
 *
 ****************************************************************************/

#ifndef _UTIL_H
#define  _UTIL_H

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stream.h>

enum { TFALSE = 0, TTRUE = 1 };

#define MAX(A,B) ((A)>(B) ? (A):(B))
#define MIN(A,B) ((A)<(B) ? (A):(B))

extern const float PI;

// ************************* error handling ************************************

class AssertFatal {
public:
   AssertFatal(char* msg) { msg_ = new char[strlen(msg) + 1]; strcpy(msg_, msg); }
   ~AssertFatal()         { delete [] msg_; }
   char* msg_;
};

class AssertWarning {
public:
   AssertWarning(char* msg) { msg_ = new char[strlen(msg) + 1]; strcpy(msg_, msg); }
   ~AssertWarning()         { delete [] msg_; }
   char* msg_;
};

void assertStr(int, char*);
void assertStrFatal(int, char*);
void assertStrWarning(int, char*);

// *********************** progress reporting **************************************

typedef int (*report_fncT) (char*, int);
report_fncT set_report_func(report_fncT new_func);
void report_progress(char* comment, int percent);


#endif
