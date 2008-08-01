/*=========================================================================

  Program:   Open IGT Link -- Performance Test Prgram
  Module:    $RCSfile: $
  Language:  C++
  Date:      $Date: $
  Version:   $Revision: $

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


#ifndef __igtlLogger_h
#define __igtlLogger_h

#include "igtlObject.h"
#include "igtlObjectFactory.h"
#include "igtlMacro.h"

#include <string>

namespace igtl
{

#define MAX_ROWS      10000
#define MAX_COLUMNS   20

class Logger : public Object
{
public:
  /** Standard class typedefs. */
  typedef Logger                    Self;
  typedef Object                    Superclass;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  igtlNewMacro(Self);
  igtlTypeMacro(FastMutexLock,Object);

  void        SetAppendMode(int s) { this->m_Append = s; };

  void        ClearData();
  
  void        SetFileName(const char* name);
  const char* GetFileName();

  int         SetSize(int rows, int columns);
  void        GetSize(int* rows, int* columns);

  int         SetData(int row, int column, double data);
  int         GetData(int row, int column, double* data);

  int         PushData(double* data);

  void        Flush();

protected:
  Logger();
  ~Logger();

private:

  void        DeleteData();

  //BTX
  std::string m_FileName;
  //ETX
  int         m_Columns;
  int         m_Rows;
  double**    m_Data;
  int         m_Counter;
  int         m_Append;
};

}

#endif //__igtlLogger_h
