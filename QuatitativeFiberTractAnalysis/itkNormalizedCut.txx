#ifndef __itkNormalizedCut_txx_
#define __itkNormalizedCut_txx_

#include <iostream>
#include <fstream>

#include "itkNormalizedCut.h"

#include "itkArray2D.h"
#include "itkArray.h"
#include "vxl/core/vnl/algo/vnl_generalized_eigensystem.h"
// #include "vxl/core/vnl/algo/vnl_symmetric_eigensystem.h"
// #include "vxl/core/vnl/algo/vnl_sparse_symmetric_eigensystem.h"
// #include "vxl/core/vnl/algo/vnl_matrix_inverse.h"
// #include "vxl/core/vnl/vnl_sparse_matrix.h"

namespace itk {

template <class TElementType>
NormalizedCut<TElementType>::NormalizedCut()
{
  m_NumberOfClusters = 2;
}

template <class TElementType>
void 
NormalizedCut<TElementType>::
PrintSelf(std::ostream& os, Indent indent) const
{ 
  os << "Normalized Cut" << std::endl;
}

template <class TElementType>
void
NormalizedCut<TElementType>::
Evaluate() 
{
  const int r = m_AffinityMatrix.rows();
  m_Classes.resize(r,0);
  
  // Use -1 as a flag for classes that we dont know the cut value
  // for 
  m_CutValue.resize(r,-1);

  // Split into two clusters
  Recursive2WayCut(0,1);

  // For any additional cluster to be cut find the most unstable
  // cluster by looking at an attempted cut of each
  for(unsigned int c = 2; c < m_NumberOfClusters; ++c)
    {
    double minCut;
    unsigned int minGroup;
    minCut = std::numeric_limits<double>::max();
    minGroup = std::numeric_limits<unsigned int>::max();

    // Find the most unstable cluster
    for(unsigned int i = 0; i <= c-1; ++i)
      {

      TElementType test;
      if(m_CutValue[i] < 0) 
        {
        test = Recursive2WayCutTest(i,c);
        m_CutValue[i] = test;
        }
      else
        {
        test = m_CutValue[i];
        }
      if(test < minCut)
        {
        minCut = test;
        minGroup = i;
        }
      }

    m_CutValue[minGroup] = -1;
    Recursive2WayCut(minGroup,c);
    }
  
} // End Execute()

template <class TElementType>
TElementType
NormalizedCut<TElementType>::
CutValue(const Array2D<TElementType>& D,
         const Array2D<TElementType>& W,
         const std::vector<bool>& cutting) const
{
  const int r = W.rows();

  // Compute ncut by 
  TElementType topA = 0;
  TElementType bottomA = 0;
  TElementType topB = 0;
  TElementType bottomB = 0;
  for(unsigned int i = 0; i != r; ++i)
    {
    if(cutting[i]) {bottomA += D(i,i);}
    else {bottomB += D(i,i);}  

    for(unsigned int j = 0; j != r; ++j)
      {
      if(!cutting[i] && cutting[j]) {topA +=  W(i,j);}
      if(cutting[i] && !cutting[j]) {topB +=  W(i,j);} 
      }
    }
  
  return topA/bottomA + topB/bottomB;

}

template <class TElementType>
std::vector<bool>
NormalizedCut<TElementType>::
Recursive2WayCutBase(const std::vector<int>& indicies) const
{
  const int r = indicies.size();
  AffinityMatrixType W(r,r);
  AffinityMatrixType D(r,r);

  D.fill(0);

  // Compute the D, W matrices from the indicies and the stored affinity
  // matrix, W is the masked affinity matrix.  D is a diagonal matrix
  // where the value on the diagonal at i,i is the sum of row i in W
  for(int i = 0; i != indicies.size(); ++i)
    {
    TElementType sum = 0;
    for(int j = 0; j != indicies.size(); ++j)
      {
      W(i,j) = m_AffinityMatrix(indicies[i],indicies[j]);
      sum += W(i,j);
      }
    D(i,i) = sum;
    }

  // Solve the generalized eigensystem of (D-W)y = \lambda Dy.  This a
  // relaxed version of the normalized cut problem
  vnl_generalized_eigensystem eigsys(D - W,D);
          
  // The second smallest eigenvector gives the partitioning of the data
  vnl_vector<TElementType> continuous_cut(eigsys.V.get_column(1));
  //std::cout << eigsys::n << std::endl;

  // Comput the mean
  TElementType mean  = 0;
//   TElementType max = -1000.0;
//   TElementType min = 1000.0;
  for(unsigned int i = 0; i < r; ++i)
    {
    mean += continuous_cut[i];
// #ifdef _MSC_VER
//     min = std::_cpp_min(min,continuous_cut[i]);
//     max = std::_cpp_max(max,continuous_cut[i]);
// #else
//     min = std::min(min,continuous_cut[i]);
//     max = std::max(max,continuous_cut[i]);
// #endif
    }
  mean /= r;

  // Try to perform the splitting at zero and at the mean
  // of the vector
  // TODO: sample more possible splittings
  std::vector<bool> discrete_cutZero(r,false);
  std::vector<bool> discrete_cutMean(r,false);
  for(unsigned int i = 0; i < r; ++i)
    {
    if(continuous_cut[i] > mean) 
      {
      discrete_cutMean[i] = true;

      }
    if(continuous_cut[i] > 0) 
      {
      discrete_cutZero[i] = true;

      }
    }

  // Choose the zero or the mean splitting depeneding on which
  // produces the better value of the Normalized Cut
  TElementType mCut = this->CutValue(D,W,discrete_cutMean);
  TElementType zCut = this->CutValue(D,W,discrete_cutZero);
//   TElementType cCut = this->CutValue(indicies,discrete_cutC);

   if(mCut <= zCut){ return discrete_cutMean;}
   else{ return discrete_cutZero;}
}

template <class TElementType>
void
NormalizedCut<TElementType>::
Recursive2WayCut(unsigned int classe, unsigned int newClass) 
{
  std::vector<int> indicies;
  for(unsigned int i = 0; i != m_Classes.size(); ++i)
    {
      if (m_Classes[i] == classe) {indicies.push_back(i);}
    }
  
  std::vector<bool> discrete_cut(this->Recursive2WayCutBase(indicies));
  for(unsigned int i = 0; i < discrete_cut.size(); ++i)
    {
    if(discrete_cut[i]) {m_Classes[indicies[i]] = newClass;}
    }

}
template <class TElementType>
TElementType
NormalizedCut<TElementType>::
Recursive2WayCutTest(unsigned int classe, unsigned int newClass) const
{
  std::vector<int> indicies;
  for(unsigned int i = 0; i != m_Classes.size(); ++i)
    {
      if (m_Classes[i] == classe) {indicies.push_back(i);}
    }
  
  const int r = indicies.size();
  AffinityMatrixType W(r,r);
  AffinityMatrixType D(r,r);

  D.fill(0);

  // Compute the D, W matrices from the indicies and the stored affinity
  // matrix, W is the masked affinity matrix.  D is a diagonal matrix
  // where the value on the diagonal at i,i is the sum of row i in W
  for(int i = 0; i != indicies.size(); ++i)
    {
    TElementType sum = 0;
    for(int j = 0; j != indicies.size(); ++j)
      {
      W(i,j) = m_AffinityMatrix(indicies[i],indicies[j]);
      sum += W(i,j);
      }
    D(i,i) = sum;
    }

  return this->CutValue(D,W,this->Recursive2WayCutBase(indicies));
}

} // itk::

#endif
