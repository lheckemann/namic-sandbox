/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkSVMFileWriter.txx,v $
  Language:  C++
  Date:      $Date: 2006/09/08 13:36:31 $
  Version:   $Revision: 1.2 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _SVMFileWriter_txx
#define _SVMFileWriter_txx

#include <itksys/ios/sstream>
#include "itkSVMFileWriter.h"

namespace itk
{
/** Constructor */
template<class TVector, class TOutput>
SVMFileWriter<TVector,TOutput>
::SVMFileWriter()
{
  m_FileName = "";
  m_WriteASCII = false; //Default: binary output
}

template<class TVector, class TOutput>
void
SVMFileWriter<TVector,TOutput>
::SetInput(SVMType* svm) 
{
  m_SVM = svm; 
}

template<class TVector, class TOutput>
typename SVMFileWriter<TVector,TOutput>::SVMType*
SVMFileWriter<TVector,TOutput>
::GetSVM() 
{
 return m_SVM.GetPointer();
}

/** Destructor */
template<class TVector, class TOutput>
SVMFileWriter<TVector,TOutput>
::~SVMFileWriter()
{
}


template<class TVector, class TOutput>
void
SVMFileWriter<TVector,TOutput>
::PrintSelf( std::ostream& os, Indent indent ) const 
{ 
  Superclass::PrintSelf( os, indent ); 
} 


/** Update the Writer */
template<class TVector, class TOutput>
void
SVMFileWriter<TVector,TOutput>
::Update()
{  
  //std::ofstream of;
  svmOutputfile.open(m_FileName.c_str(), std::ios::binary | std::ios::out);
  
  if(!svmOutputfile.is_open())
  {
    std::cout << "SVMFileReader Write: Cannot open file" << std::endl;
    return;
    }
 
  MET_FieldRecordType * mF;
    
  mF = new MET_FieldRecordType;
  MET_InitWriteField(mF, "ObjectType", MET_STRING,
                     strlen(m_SVM->GetNameOfClass()),m_SVM->GetNameOfClass()); 
  m_Fields.push_back(mF);
  
  mF = new MET_FieldRecordType;
  MET_InitWriteField(mF, "NumSVs", MET_UINT,m_SVM->GetNumberOfSupportVectors());
  m_Fields.push_back(mF);

  mF = new MET_FieldRecordType;
  MET_InitWriteField(mF, "Dim", MET_UINT,m_SVM->GetMeasurementVectorSize());
  m_Fields.push_back(mF);
 
  mF = new MET_FieldRecordType;
  MET_InitWriteField(mF, "SVM_B", MET_FLOAT,m_SVM->GetB());
  m_Fields.push_back(mF);
 
  mF = new MET_FieldRecordType;
  std::string writemode;
  if(m_WriteASCII == true)
    writemode = "ASCII";
  else
    writemode = "Binary";
  MET_InitWriteField(mF, "ValuesType", MET_STRING,
                  writemode.size(),writemode.c_str());
  m_Fields.push_back(mF);
 
  mF = new MET_FieldRecordType;
  MET_InitWriteField(mF, "KernelType", MET_STRING,
      strlen(m_SVM->GetKernel()->GetNameOfClass()),
      m_SVM->GetKernel()->GetNameOfClass());
  m_Fields.push_back(mF);
 
  // if the kernel is RBF, write out the gamma value
  if(!strcmp(m_SVM->GetKernel()->GetNameOfClass(),"RBFSVMKernel"))
  {
    RBFKernelPointer kernel = (RBFKernelType*) m_SVM->GetKernel();
    mF = new MET_FieldRecordType;
    MET_InitWriteField(mF, "Gamma", MET_FLOAT, kernel->GetGamma());
    m_Fields.push_back(mF);
  }
  else if(!strcmp(m_SVM->GetKernel()->GetNameOfClass(),"PolynomialSVMKernel"))
  {
    PolynomialKernelPointer kernel = (PolynomialKernelType*) m_SVM->GetKernel();
    mF = new MET_FieldRecordType;
    MET_InitWriteField(mF, "Gamma", MET_FLOAT, kernel->GetGamma());
    m_Fields.push_back(mF);
    mF = new MET_FieldRecordType;
    MET_InitWriteField(mF, "R", MET_FLOAT, kernel->GetR());
    m_Fields.push_back(mF);
    mF = new MET_FieldRecordType;
    MET_InitWriteField(mF, "Power", MET_FLOAT, kernel->GetPower());
    m_Fields.push_back(mF);
  }
 
  mF = new MET_FieldRecordType;
  MET_InitWriteField(mF, "ElementDataFile", MET_STRING,
  strlen("Local"), "Local");
  mF->terminateRead=true;
  m_Fields.push_back(mF);
  if(!MET_Write(svmOutputfile, & m_Fields,'='))
  {
    std::cout << "MetaObject: Write: MET_Write Failed" << std::endl;
    //return false;
  }
  for (unsigned int i = 0; i < m_Fields.size(); ++i)
  {
    delete m_Fields[i];
  }
  m_Fields.clear();
  double* alphas = new double[m_SVM->GetNumberOfSupportVectors()];
  double* labels = new double[m_SVM->GetNumberOfSupportVectors()];
  double* sv     = new double[m_SVM->GetNumberOfSupportVectors()
                            * m_SVM->GetMeasurementVectorSize()];

  int scount = 0;                       
  for(unsigned int z=0; z<m_SVM->GetNumberOfSupportVectors(); z++)
  {
     alphas[z] = m_SVM->GetAlpha(z);
     labels[z] = m_SVM->GetLabel(z);
     for(unsigned int y = 0; y<m_SVM->GetMeasurementVectorSize(); y++)
     {
        sv[scount] = m_SVM->GetSupportVector(z)[y];
        scount++;
     }
  }  
  //Write Alphas, SupportVectors and corresponding Labels
  if(m_WriteASCII == true) //ASCII mode
  {

    mF = new MET_FieldRecordType;
    MET_InitWriteField(mF, "Alphas", MET_DOUBLE_ARRAY,
                                  m_SVM->GetNumberOfSupportVectors(),
                                                             alphas);
    m_Fields.push_back(mF);

    mF = new MET_FieldRecordType;
    MET_InitWriteField(mF, "Labels", MET_DOUBLE_ARRAY,
                                  m_SVM->GetNumberOfSupportVectors(),
                                                               labels);
    mF->terminateRead=true;
    m_Fields.push_back(mF); 
  
   if(!MET_Write(svmOutputfile, & m_Fields,'='))
   {
    std::cout << "MetaObject: Write: MET_Write Failed" << std::endl;
   }
  
   for (unsigned int i = 0; i < m_Fields.size(); ++i)
   {
     delete m_Fields[i];
   }
   m_Fields.clear();
  
   svmOutputfile << "SupportVectors=\n";
   scount=0;
   for(unsigned int n = 0; n < m_SVM->GetNumberOfSupportVectors(); n++)
   {
     for(unsigned int n1=0; n1<m_SVM->GetMeasurementVectorSize(); n1++)
     {
       svmOutputfile << sv[scount] << " ";
       scount++;
     }
     svmOutputfile << "\n";
   }
   
  }
  else //Binary mode
  {
   svmOutputfile.write( (char *)alphas,
                           m_SVM->GetNumberOfSupportVectors()* sizeof(double)); 
   svmOutputfile.write( (char *)labels,
                           m_SVM->GetNumberOfSupportVectors()* sizeof(double));    svmOutputfile.write( (char *)sv,
                           m_SVM->GetMeasurementVectorSize()*
                           m_SVM->GetNumberOfSupportVectors()* sizeof(double)); 

  
  }
  delete [] alphas;
  delete [] labels;
  delete [] sv;
  svmOutputfile.close();
}


} // namespace itk

#endif
