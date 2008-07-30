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

#include <math.h>
#include <iostream>
#include <fstream>

#include "igtlLogger.h"

namespace igtl
{

Logger::Logger() :
  Object()
{
  this->m_Columns = 0;
  this->m_Rows    = 0;
  this->m_Data    = NULL;

  this->m_Counter = 0;
}


Logger::~Logger()
{
  this->DeleteData();
}


void Logger::DeleteData()
{
  if (this->m_Data && this->m_Columns > 0 && this->m_Rows > 0)
    {
    for (int i = 0; i < m_Rows; i ++)
      {
      delete [] this->m_Data[i];
      }
    delete [] this->m_Data;
    }
  this->m_Columns = 0;
  this->m_Rows    = 0;
}


void Logger::ClearData()
{
  for (int i = 0; i < this->m_Rows; i ++)
    {
    for (int j = 0; j < this->m_Columns; j ++)
      {
      this->m_Data[i][j] = 0.0;
      }
    }
}

void Logger::SetFileName(const char* name)
{
  this->m_FileName = name;
}


const char* Logger::GetFileName()
{
  return this->m_FileName.c_str();
}


int Logger::SetSize(int rows, int columns)
{

  if (columns > 0 && columns <= MAX_COLUMNS &&
      rows > 0 && rows <= MAX_ROWS)
    {
    this->DeleteData();

    this->m_Columns = columns;
    this->m_Rows    = rows;
    this->m_Counter = 0;
    
    this->m_Data = new double*[rows];
    for (int i = 0; i < rows; i ++)
      {
      this->m_Data[i] = new double[columns];
      }

    this->ClearData();
    return 1;
    }
  else // Illegal size is specified.
    {
    return 0;
    }

}
      

void Logger::GetSize(int* rows, int* columns)
{
  *columns = this->m_Columns;
  *rows    = this->m_Rows;
}


int Logger::SetData(int row, int column, double data)
{
  if (this->m_Data != NULL &&
      column >= 0 && column < this->m_Columns &&
      row    >= 0 && row    < this->m_Rows)
    {
    this->m_Data[row][column] = data;
    if (row > this->m_Counter)
      {
      this->m_Counter = row;
      }
    return 1;
    }
  else
    {
    return 0;
    }
}


int Logger::GetData(int row, int column, double* data)
{

  if (this->m_Data != NULL &&
      column >= 0 && column < this->m_Columns &&
      row    >= 0 && row    < this->m_Rows)
    {
    *data = this->m_Data[row][column];
    return 1;
    }
  else  // data does not exist
    {
    return 0;
    }

}


int Logger::PushData(double* data)
{

  if (this->m_Counter < this->m_Rows)
    {
    for (int j = 0; j < this->m_Columns; j ++)
      {
      this->m_Data[this->m_Counter][j] = data[j];
      }
    this->m_Counter ++;
    }

}


void Logger::Flush()
{

  if (this->m_FileName.size() > 0)
    {
    std::ofstream fout(this->m_FileName.c_str(), std::ios::app);
    //fout.precision(9);

    int rows       = this->m_Rows;
    int columns_m1 = this->m_Columns - 1;

    for (int i = 0; i < this->m_Counter; i ++)
      {
      for (int j = 0; j < columns_m1; j ++)
        {
        fout << this->m_Data[i][j] << ", ";
        }
      fout <<  this->m_Data[i][columns_m1] << std::endl;
      }
    fout.close();
    }
  this->m_Counter = 0;
  this->ClearData();

}

} // namespace igtl
