/*=========================================================================

Program:   Insight Segmentation & Registration Toolkit
Module:    $RCSfile: itkSVMSolver1.txx,v $
Language:  C++
Date:      $Date: 2006/09/15 15:26:15 $
Version:   $Revision: 1.3 $

Copyright (c) Insight Software Consortium. All rights reserved.
See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even 
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __SVMSolver1_txx
#define __SVMSolver1_txx

#include "itkSVMSolver1.h"
#include <iomanip>

namespace itk
{
namespace Statistics
{

// LessThan function for the multimap
struct ltfloat
{
  bool operator()(float s1,float s2) const
  {
    return (s1<s2);
  }
};

/** Constructor */
template<class TVector, class TOutput>
SVMSolver1<TVector,TOutput>
::SVMSolver1()
{
  m_QPSolver = QPSolverType::New();
  m_Epsilon = 0.001;
  m_WorkingSetSize = 10;
  m_Iterations = 500;
  m_Precision = 1e-12;
  m_SigDigits = -log10(m_Precision);
  m_Margin = 1e-4;  
  m_C = 0.0;
  m_Lambda_eq = 0.0;
  m_AlphaThreshold = -1.0;
  m_AlphaThresholdMult = 1e-5;  
}

template<class TVector, class TOutput>
SVMSolver1<TVector,TOutput>
::~SVMSolver1()
{ }
  
template<class TXSample, class TYSample>
void SVMSolver1<TXSample,TYSample>
::Update()
{
  this->GenerateLabels();

  if(m_C==0)
  {
    ComputeDefaultC(); 
  }

#ifdef ITK_SVM_DEBUG_OUTPUT
  std::cerr<<"Default C = "<<m_C<<std::endl;
#endif
  if(Superclass::GetComputeQMatrix() == true &&
     Superclass::GetComputeKMatrix() == true)
  {
    this->ComputeQAndKMatrices();
  }
  else if(Superclass::GetComputeKMatrix() == false &&
           Superclass::GetComputeQMatrix() == true)   
  {
    this->ComputeQMatrix();
  }
  else if(Superclass::GetComputeQMatrix() == false &&
           Superclass::GetComputeKMatrix() == true)   
  {
    this->ComputeKMatrix();
  }
  else
  {
   //dont compute anything,both are set by the user
  }
  
  m_Alphas.set_size(this->m_NumSamples);
  
  // vectors for the QP Solver
  // minimize   c' * x + 1/2 x' * H *x
  // subject to A*x = b
  // low <= x <= up
  // Please see itkSVMpr_loqo.h for more information
  
  vnl_vector<double> g(this->m_NumSamples);    // N x 1 vector of -1s 
  vnl_vector<double> b(1);                     // 1 x 1 = 0
  vnl_vector<double> low(this->m_NumSamples);  // lower bound on alphas
  vnl_vector<double> up(this->m_NumSamples);   // uppder bound on alphas
  
  up.fill(m_C); 
  low.fill(0.0);  
  g.fill(-1.0);
  b(0)=(0.0);  
   
  double* primal = new double[3*this->m_NumSamples];
  double* dual = new double[1+2*this->m_NumSamples];
  
  for(int i=0; i<(3*this->m_NumSamples); i++)
  {
    primal[i]=0;
  }
  for(int i=0; i<1+(2*this->m_NumSamples); i++)
  {
    dual[i]=0;
  }
         
#ifdef ITK_SVM_DEBUG_OUTPUT
  std::cerr << "m_QMatrix = ";
  std::cerr << this->m_QMatrix << std::endl;
#endif
  
  //
  // call pr_loqo solver and then copy alphas
  //
  // (unused variable) int r = 
  m_QPSolver->
    pr_loqo(this->m_NumSamples,               // number of primal vars 
            (int)1,                           // number of constraints
            g.data_block(),                   // c
            this->m_QMatrix.data_block(),     // h_x
            this->m_Labels.data_block(),      // a
            b.data_block(),                   // b
            low.data_block(),                 // l: lower bound for alphas
            up.data_block(),                  // u: upper bound for alphas
            primal,                           // primal
            dual,                             // dual
            QUIET,                            // verbosity level
            m_SigDigits,                      // sigfig_max
            m_Iterations,                     // counter_max
            m_Margin,                         // margin
            up[0]/2.0,                        // bound 
            (int)0);                          // restart
  for(unsigned int i = 0; i < m_Alphas.size(); i++)
  {
    m_Alphas[i] = primal[i]; 
  }
  
  //SelectWorkingSet(); 
  
#ifdef ITK_SVM_DEBUG_OUTPUT
  std::cerr<<"Smola Results"<<std::endl;       
  std::cerr<<r<<std::endl;       
  std::cerr<<"Primal"<<std::endl; 
  for(unsigned int k=0; k<3*this->m_NumSamples; k++)
  {
    std::cerr<<std::setprecision (5)<<primal[k]<<std::endl;
  }
  std::cerr<<"dual"<<std::endl; 
  for(unsigned int k=0; k<1+2*this->m_NumSamples; k++)
  {
    std::cerr<<std::setprecision (5)<<dual[k]<<std::endl;
  }
  std::cerr<<"Class 1 = "<<this->m_NumClass1<<std::endl;
  std::cerr<<"Class 2 = "<<this->m_NumClass2<<std::endl;
  std::cerr<<"Default C = "<<m_C<<std::endl;
#endif
  
  //for(unsigned int i=0; i<this->m_NumSamples; i++)
  // m_Alphas[i] = primal[i];  
         
         
  //3. Check optimality conditions
#ifdef ITK_SVM_DEBUG_OUTPUT
  //   if(CheckOptimality())
  //  std::cerr<<"Alphas Optimal"<<std::endl;
  //else
  //  std::cerr<<"Alphas Not Optimal"<<std::endl;        
  //       
  //std::cerr<<"Lambda Eq = "<<m_Lambda_eq<<std::endl;
  //std::cerr<<"Adding SVs"<<std::endl;
#endif
  
  AddSupportVectors();
#ifdef ITK_SVM_DEBUG_OUTPUT
  std::cerr<<"Done Adding SVs"<<std::endl;
#endif
  this->m_SVM->SetB(dual[0]);
  
  //
  // free memory used by pr_loqo
  //
  delete [] primal;
  delete [] dual;
}

template<class TXSample, class TYSample>
typename SVMSolver1<TXSample, TYSample>::VnlMatrixType
SVMSolver1<TXSample,TYSample>
::SelectWorkingSet()
{
  VnlMatrixType ws; //working set
  std::multimap<float,int,ltfloat> alphamap;
  m_G.set_size(this->m_QMatrix.rows());
  m_G.fill(-1); 
  m_G += (this->m_QMatrix*m_Alphas);
  
  for(unsigned int i=0; i<m_Alphas.size(); i++)
  {
    alphamap.insert(std::pair<float,int>(this->m_Labels(0,i)*m_G(i), i));
  }   
  
  int count=0; 
  std::multimap<float, int, ltfloat>::iterator it;
  
  
  //Pick m_WorkingSetSize/2 elements from top of alphamap
  //for which 0 < alpha(t) < C
#ifdef ITK_SVM_DEBUG_OUTPUT
  std::cerr<<"Top q/2"<<std::endl;
  count = 0;
  it = alphamap.begin();
  while(count<= (m_WorkingSetSize/2) && it != alphamap.end())
  {
    if((*it).first > 0 && (*it).first < m_C)   
      std::cerr << "In the working set =" << (*it).first << " " 
                << (*it).second << std::endl;
    it++;
    count++;
  }
  
  
  //Pick m_WorkingSetSize/2 elements from bottom of alphamap
  //for which  0 < alpha(t) < C
  it = alphamap.end();
  count =0;
  std::cerr<<"Bottom q/2"<<std::endl;
  while(count<= (m_WorkingSetSize/2) && it != alphamap.begin())
  {
    if((*it).first > 0 && (*it).first < m_C)   
      std::cerr << "In the working set =" << (*it).first << " " 
                << (*it).second<<std::endl;
    it--;
    count++;
  }
#endif       
  return ws;
}
template<class TXSample, class TYSample>
void SVMSolver1<TXSample,TYSample>
::AddSupportVectors()
{ 
  //
  // decide how many support vectors we'll have
  //
  unsigned int svCount = 0;
  if(m_AlphaThreshold < 0)
  {
    double temp = 0;
    for(unsigned int i =0; i < m_Alphas.size(); i++)
    {
      if(temp < fabs(m_Alphas[i]))
        temp = fabs(m_Alphas[i]);
    } 
    m_AlphaThreshold = temp * m_AlphaThresholdMult;
  }
 
  for (unsigned int i = 0; i < m_Alphas.size(); ++i)
  {
    if (fabs(m_Alphas[i]) > m_AlphaThreshold)
    {
      ++svCount;
    }
  }

  //
  // set the support vectors and associated label information
  //
  this->m_SVM->SetNumberOfSupportVectors(svCount);
  typename TXSample::ConstIterator iter1 = this->m_C1SamplePointer->Begin();
  typename TXSample::ConstIterator iter2 = this->m_C2SamplePointer->Begin();
  for(unsigned int i = 0, nextSVIndex = 0; i < m_Alphas.size(); ++i)
  {
    if (m_Alphas[i] > m_AlphaThreshold)
    {
      this->m_SVM->SetAlpha(nextSVIndex, this->m_Alphas[i]);
      this->m_SVM->SetLabel(nextSVIndex, this->m_Labels(0,i));
      
      if (i < this->m_C1SamplePointer->Size())
      {
        this->m_SVM->SetSupportVector(nextSVIndex, 
                                      iter1.GetMeasurementVector());      
        this->m_SupportVectorInstanceIdentifiers.
          push_back(iter1.GetInstanceIdentifier());
      }
      else
      {
        this->m_SVM->SetSupportVector(nextSVIndex, 
                                      iter2.GetMeasurementVector());      
        this->m_SupportVectorInstanceIdentifiers.
          push_back(iter2.GetInstanceIdentifier());
      }
      ++nextSVIndex;
    }
    
    if (i < this->m_C1SamplePointer->Size())
    {
      ++iter1;
    }
    else
    {
      ++iter2;
    }
  }
}

template<class TXSample, class TYSample>
bool SVMSolver1<TXSample,TYSample>
::CheckOptimality() 
{ 
  //check KKT conditions   
  itkNotUsed(double cond1=0;);
  itkNotUsed(double cond2=0;);
  itkNotUsed(double cond3=0;);
  
  for(unsigned int i=0; i<m_Alphas.size(); i++)
  {
    if(m_Alphas[i]>0 && m_Alphas[i]<m_C)
    {
      double temp=this->m_Labels(0,i);
      for(unsigned int i1=0; i1<this->m_NumSamples; i1++)
      {
        temp+= m_Alphas[i1]*this->m_Labels(0,i1)* this->m_KMatrix(i,i1);
        //this->m_Kernel->Evaluate(this->m_InputSamples[i],this->m_InputSamples[i1]); 
      }
      if(temp<(m_Lambda_eq-m_Epsilon) || temp>(m_Lambda_eq+m_Epsilon))
        return false;
    }
    else if(m_Alphas[i]==0)
    {
      double temp=0;
      for(unsigned int i1 = 0; i1<this->m_NumSamples; i1++)
      {
        temp+=m_Alphas[i1]*this->m_Labels(0,i1)* this->m_KMatrix(i,i1);
        //this->m_Kernel->Evaluate(this->m_InputSamples[i],this->m_InputSamples[i1]);
      }
      if((this->m_Labels(0,i)*temp +m_Lambda_eq)< (1 - m_Epsilon))
        return false;
    }
    else if(m_Alphas[i]==m_C)
    {
      double temp=0;
      for(unsigned int i1 = 0; i1<this->m_NumSamples; i1++)
      {
        temp+=m_Alphas[i1]*this->m_Labels(0,i1)* this->m_KMatrix(i,i1);
        // this->m_Kernel->Evaluate(this->m_InputSamples[i],this->m_InputSamples[i1]);
      }
      if((this->m_Labels(0,i)*temp +m_Lambda_eq)> (1 + m_Epsilon))
        return false;
    }
    
  }
  
  return true;
}

template<class TXSample, class TYSample>
void SVMSolver1<TXSample,TYSample>
::ComputeLambdaEq() 
{
  
  InternalVectorType tempvec(m_Alphas.size());
  tempvec.fill(0.0);
  for(unsigned int i=0; i<m_Alphas.size(); i++)
  {  
    if(m_Alphas[i]>0 && m_Alphas[i]<m_C)
    {
      tempvec[i]=m_Alphas[i];
      m_Lambda_eq+=this->m_Labels(0,i);
      for(unsigned int j=0; j<m_Alphas.size(); j++)
      {
        m_Lambda_eq-=
          (m_Alphas[j]*this->m_Labels(0,j) * 
           this->m_SVM->GetKernel()->
           Evaluate(this->m_InputSamples[i],this->m_InputSamples[j]));
      }    
    }
  }
  m_Lambda_eq/=(double)tempvec.magnitude();
}

template<class TXSample, class TYSample>
void SVMSolver1<TXSample,TYSample>
::ComputeDefaultC() 
{
  m_C=0;
  typename TXSample::ConstIterator iter1 = this->m_C1SamplePointer->Begin();
  typename TXSample::ConstIterator iter2 = this->m_C2SamplePointer->Begin();
  while(iter1 != this->m_C1SamplePointer->End())
  {
    XVectorType x= iter1.GetMeasurementVector();
    vnl_vector<double> t1(x.Size());
    for(unsigned int j=0; j<x.Size(); j++)
      t1[j]=x[j];
    m_C +=inner_product(t1,t1); // m_Kernel->Evaluate(m_InputSamples[i],m_InputSamples[i]);
    ++iter1; 
  }
  while(iter2 != this->m_C2SamplePointer->End())
  {
    XVectorType x= iter2.GetMeasurementVector();
    vnl_vector<double> t1(x.Size());
    for(unsigned int j=0; j<x.Size(); j++)
      t1[j]=x[j];
    m_C +=inner_product(t1,t1); // m_Kernel->Evaluate(m_InputSamples[i],m_InputSamples[i]);
    ++iter2; 
  }
  
  m_C/=(double)this->m_NumSamples;
  m_C = pow(m_C,-1);
}


/** Print the object */
template<class TXSample, class TYSample>
void SVMSolver1<TXSample,TYSample>
::PrintSelf( std::ostream& os, Indent indent ) const 
{ 
  Superclass::PrintSelf( os, indent ); 
} 

}      // namespace Statistics
}      // namespace itk
#endif // __SVMSolver1_txx

