// -*- C++ -*-
/***************************************************************************
 *
 *  
 *
 ****************************************************************************
 *
 *  File         : util.cc
 *  Type         : 
 *  Purpose      : 
 *
 ****************************************************************************
 *                 
 *  started      :  15 Nov 94
 *  last change  :  10 Feb 95     Robert Schweikert
 *
 ****************************************************************************/

#include "util.h"

const float  PI    = 3.14159265359;


void assertStr(int condition, char* comment)
{
  if ( !condition ) 
     throw AssertWarning(comment);
}

void assertStrFatal(int condition, char* comment)
{
  if ( !condition ) 
     throw AssertFatal(comment);
}

void assertStrWarning(int condition, char* comment)
{
  if ( !condition ) 
     throw AssertWarning(comment);
}

// ******************** progress reporting (from ~brech) ********************************

static int report_func_default(char* msg, int percent)
{
  //cerr << "--st-- " << msg << " : " << percent << "%\n";
  return(0);
}

static report_fncT report_func = report_func_default;

report_fncT set_report_func(report_fncT new_func)
{
  report_fncT old_func = report_func;
  report_func = new_func;
  return old_func;
}

void report_progress(char* message, int percent)
{
  (*report_func)(message, percent);
}
