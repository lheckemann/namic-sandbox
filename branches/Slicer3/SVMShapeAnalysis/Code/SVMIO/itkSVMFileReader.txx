/*=========================================================================

Program:   Insight Segmentation & Registration Toolkit
Module:    $RCSfile: itkSVMFileReader.txx,v $
Language:  C++
Date:      $Date: 2006/09/08 13:36:31 $
Version:   $Revision: 1.3 $

Copyright (c) Insight Software Consortium. All rights reserved.
See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even 
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _SVMFileReader_txx
#define _SVMFileReader_txx

#include <itksys/ios/sstream>
#include "itkSVMFileReader.h"
#include <typeinfo>

namespace itk
{
  /** Constructor */
  template<class TVector, class TOutput>
  SVMFileReader<TVector,TOutput>
  ::SVMFileReader()
  {
    m_FileName = "";
  }

  template<class TVector, class TOutput>
  typename SVMFileReader<TVector,TOutput>::SVMType*
  SVMFileReader<TVector,TOutput>
  ::GetSVM() 
  {
    return m_SVM.GetPointer();
  }

  /** Destructor */
  template<class TVector, class TOutput>
  SVMFileReader<TVector,TOutput>
  ::~SVMFileReader()
  {
  }


  template<class TVector, class TOutput>
  void
  SVMFileReader<TVector,TOutput>
  ::PrintSelf( std::ostream& os, Indent indent ) const 
  { 
    Superclass::PrintSelf( os, indent ); 
  } 


  /** Update the Reader */
  template<class TVector, class TOutput>
  void
  SVMFileReader<TVector,TOutput>
  ::M_SetupReadFields(void)
  {
  

  }

  template<class T>
  void
  UpdateSize(itk::VariableLengthVector<T>& v, unsigned int dim)
  {
    v.SetSize(dim);
  }

  template<class T>
  void
  UpdateSize(itk::Array<T>& v, unsigned int dim)
  {
    v.SetSize(dim);
  }

  template<class T, unsigned int I>
  void
  UpdateSize(itk::Vector<T, I>& v, unsigned int dim)
  {
    // do nothing, vectors are not resizable
  }

  template<class T, unsigned int I>
  void
  UpdateSize(itk::FixedArray<T, I>& v, unsigned int dim)
  {
    // do nothing, FixedArrays are not resizable
  }

  template<class T, unsigned int I>
  void
  UpdateSize(itk::Point<T, I>& v, unsigned int dim)
  {
    // do nothing, Points are not resizable
  }


  template<class TVector, class TOutput>
  bool
  SVMFileReader<TVector,TOutput>
  ::MeasurementVectorResizable()
  {
    std::string str = typeid(TVector).name(); 
    std::cerr << "NAME OF TYPE: " << str << std::endl;
    if(str.find("Array")== std::string::npos
       || str.find("VariableLength")== std::string::npos)
      return false;
  
    return true;
  }
  /** Update the Reader */
  template<class TVector, class TOutput>
  void
  SVMFileReader<TVector,TOutput>
  ::Update()
  {  
    std::ifstream in;
    in.open ( m_FileName.c_str(), std::ios::binary );
    unsigned int num_sv = 0;
    float svm_b = 0;
    char* valuetype = new char[255]; //ASCII, Binary
    char* elementdatafile = new char[255];
    char* kernel_type = new char[255];
    itkNotUsed(int kernelparam = 0;);
    unsigned int dim;
  
    MET_FieldRecordType * mF;
  
    mF = new MET_FieldRecordType;
    MET_InitReadField(mF, "ObjectType", MET_STRING, true);
    m_Fields.push_back(mF);
  
    mF = new MET_FieldRecordType;
    MET_InitReadField(mF, "NumSVs", MET_INT, true);
    m_Fields.push_back(mF);
  
    mF = new MET_FieldRecordType;
    MET_InitReadField(mF, "Dim", MET_INT, true);
    m_Fields.push_back(mF);
  
    mF = new MET_FieldRecordType;
    MET_InitReadField(mF, "SVM_B", MET_FLOAT, true);
    m_Fields.push_back(mF);
  
    mF = new MET_FieldRecordType;
    MET_InitReadField(mF, "ValuesType", MET_STRING, true);
    m_Fields.push_back(mF);

    mF = new MET_FieldRecordType;
    MET_InitReadField(mF, "KernelType", MET_STRING, true);
    mF->terminateRead=true; 
    m_Fields.push_back(mF);
  
    if(!MET_Read(in, & m_Fields,'='))
    {
      std::cout << "MetaObject: Read: MET_Read Failed" << std::endl;
      //return false;
    }
    mF = MET_GetFieldRecord("ObjectType", &m_Fields);

    if(!strcmp((char *)(mF->value),"SupportVectorMachine"))
    {   
      m_SVM= SVMType::New();
      //std::cout<<"SVM instantiated"<<std::endl;
    }

    mF = MET_GetFieldRecord("NumSVs", &m_Fields);
    num_sv = (int) mF->value[0];

    mF = MET_GetFieldRecord("Dim", &m_Fields);
    dim = (int) mF->value[0];

    mF = MET_GetFieldRecord("SVM_B", &m_Fields);
    svm_b = (float) mF->value[0];
    
    mF = MET_GetFieldRecord("ValuesType", &m_Fields);
    strcpy(valuetype,(char*)mF->value);
 
    mF = MET_GetFieldRecord("KernelType", &m_Fields);
    strcpy(kernel_type, (char*)mF->value);
  
    m_SVM->SetB(svm_b);
    m_SVM->SetNumberOfSupportVectors(num_sv);
  
    if(!strcmp(kernel_type,"RBFSVMKernel"))
    {
      typedef Statistics::RBFSVMKernel<TVector, double> KernelType;
      typename KernelType::Pointer kernel=KernelType::New();
      for (unsigned int i = 0; i < m_Fields.size(); ++i)
      {
        delete m_Fields[i];
      }
      m_Fields.clear();
      mF = new MET_FieldRecordType;
      MET_InitReadField(mF, "Gamma", MET_FLOAT, true);
      mF->terminateRead=true; 
      m_Fields.push_back(mF);  
      if(!MET_Read(in, & m_Fields,'='))
      {
        std::cout << "MetaObject: Read: MET_Read Failed" << std::endl;
      }
      mF = MET_GetFieldRecord("Gamma", &m_Fields);
      float gamma = (float) mF->value[0];
      kernel->SetGamma(gamma);
      m_SVM->SetKernel(kernel);
    }
    else if(!strcmp(kernel_type,"PolynomialSVMKernel"))
    {  
      typedef Statistics::PolynomialSVMKernel<TVector, double> KernelType;
      typename KernelType::Pointer kernel=KernelType::New();
      for (unsigned int i = 0; i < m_Fields.size(); ++i)
      {
        delete m_Fields[i];
      }
      m_Fields.clear();
      mF = new MET_FieldRecordType;
      MET_InitReadField(mF, "Gamma", MET_FLOAT, true);
      m_Fields.push_back(mF);

      mF = new MET_FieldRecordType;
      MET_InitReadField(mF, "R", MET_FLOAT, true);
      m_Fields.push_back(mF);

      mF = new MET_FieldRecordType;
      MET_InitReadField(mF, "Power", MET_FLOAT, true);
      mF->terminateRead=true; 
      m_Fields.push_back(mF);
  
      if(!MET_Read(in, & m_Fields,'='))
      {
        std::cout << "MetaObject: Read: MET_Read Failed" << std::endl;
      }
      mF = MET_GetFieldRecord("Gamma", &m_Fields);
      float gamma = (float) mF->value[0];
      mF = MET_GetFieldRecord("R", &m_Fields);
      float R = (float) mF->value[0];
      mF = MET_GetFieldRecord("Power", &m_Fields);
      float power = (float) mF->value[0];

      kernel->SetGamma(gamma);
      kernel->SetR(R);
      kernel->SetPower(power);
      m_SVM->SetKernel(kernel);
    }
    else if (!strcmp(kernel_type,"LinearSVMKernel"))
    {
      typedef Statistics::LinearSVMKernel<TVector, double> KernelType;
      typename KernelType::Pointer kernel=KernelType::New();
      m_SVM->SetKernel(kernel);
    }
    else
    {
      itkExceptionMacro(<< "Invalid Kernel Type" << kernel_type);
    }
    for (unsigned int i = 0; i < m_Fields.size(); ++i)
    {
      delete m_Fields[i];
    }
    m_Fields.clear();
    mF = new MET_FieldRecordType;
    MET_InitReadField(mF, "ElementDataFile", MET_STRING, true);
    mF->terminateRead=true; 
    m_Fields.push_back(mF);  
    if(!MET_Read(in, & m_Fields,'='))
    {
      std::cout << "MetaObject: Read: MET_Read Failed" << std::endl;
    }
    mF = MET_GetFieldRecord("ElementDataFile", &m_Fields);
    strcpy(elementdatafile, (char*)mF->value);
    for (unsigned int i = 0; i < m_Fields.size(); ++i)
    {
      delete m_Fields[i];
    }
    m_Fields.clear();
    // read Support Vectors, alphas, labels
    double* sv_array;
    double* alphas;
    double* labels;
  
    if(!strcmp(elementdatafile, "Local")) //read from same file
    {
      sv_array = new double[num_sv*dim];
      if(!strcmp(valuetype,"ASCII"))
      {
        mF = new MET_FieldRecordType;  
        MET_InitReadField(mF, "Alphas",MET_DOUBLE_ARRAY, true,-1,
                          num_sv);
        m_Fields.push_back(mF);
       
        mF = new MET_FieldRecordType;  
        MET_InitReadField(mF, "Labels",MET_DOUBLE_ARRAY, true,-1,
                          num_sv);
        mF->terminateRead=true; 
        m_Fields.push_back(mF);
              
        if(!MET_Read(in, & m_Fields,'='))
        {
          std::cout << "MetaObject: Read: MET_Read Failed missing Alphas and Labels"<< std::endl;
          return;
        } 
        mF = MET_GetFieldRecord("Alphas", &m_Fields);
        alphas = mF->value;
     
        mF = MET_GetFieldRecord("Labels", &m_Fields);
        labels = mF->value;

        char tmpChar = ' ';
        while (tmpChar != '=')
        {
          in >> tmpChar;
        }

        if (in.eof())
        {
          std::cout<< "Read failed while reading Support Vectors"<<std::endl;
        }

        for (unsigned int scount = 0; scount < num_sv*dim; ++scount)
        {
          in >> sv_array[scount];
        }

      }
      else      //read binary data values 
      {
        alphas = new double[num_sv];
        labels = new double[num_sv];
        in.read((char *)alphas, num_sv*sizeof(double));
        in.read((char *)labels, num_sv*sizeof(double));
        in.read((char *)sv_array, num_sv*dim*sizeof(double));
      }      
     
      int count =0;
      for(unsigned int i=0; i<num_sv; i++)
      {
        TVector tempSV;
        // this is a hack: if we are using a vector type that is
        // resizable we need to resize it.  If we are using a vector
        // type that is of fixed dimension, like itk::Vector, there
        // is no SetSize function.  Since these different vector
        // types do not derive from the same base class, we use
        // function overloading to decide if SetSize should be
        // called.  If you find this while debugging a compile
        // error, try writing an UpdateSize function for the
        // container that you are using (look at the UpdateSize
        // functions defined above).
        UpdateSize(tempSV, dim);

        for(unsigned int k=0; k<dim; k++)
        {
          tempSV[k]= (float) sv_array[count];
          count++;          
        }  
        m_SVM->SetSupportVector(i,tempSV);
        m_SVM->SetLabel(i,(float)labels[i]);
        m_SVM->SetAlpha(i, (double)alphas[i]);
      }
      if (strcmp(valuetype,"ASCII"))
      {
        delete [] labels;
        delete [] alphas;
      }
    }
    else // read from the listed file/s
    {
  
    }
  
    //
    // delete temporary buffers
    //
    for (unsigned int i = 0; i < m_Fields.size(); ++i)
    {
      delete m_Fields[i];
    }
    m_Fields.clear();
    delete [] valuetype;
    delete [] elementdatafile;
    delete [] kernel_type;
    delete [] sv_array;
    in.close();
  } // namespace itk
}

#endif
