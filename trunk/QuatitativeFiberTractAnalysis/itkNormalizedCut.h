#ifndef __itkNormalizedCut_h
#define __itkNormalizedCut_h

#include <vector>

#include "itkObject.h"
#include "itkArray.h"
#include "itkArray2D.h"
#include "itkObjectFactory.h"

//#include "vxl/core/vnl/vnl_sparse_matrix.h"

namespace itk {

/** \class NormalizedCut
 * \brief Initial implementation of normalized cuts that operate on an affinity matrix.
 *
 * For now this class uses a generalized eigensolver on a user-defined affinity matrix.  Future work
 * should use sparse solvers to decrease runtime.
 *
 * This algorithm is based on the work by Shi & Malik.
 */
template<class TElementType>
class ITK_EXPORT NormalizedCut : public Object
{
 public:
  /** Standard typedefs. */
  typedef NormalizedCut<TElementType> Self;
  typedef Object Superclass;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self> ConstPointer;

  typedef Array2D<TElementType> AffinityMatrixType;
  typedef Array2D<TElementType> DiagonalMatrixType;
//  typedef vnl_diag_matrix<TElementType>  DiagonalMatrixType;
  //typedef vnl_sparse_matrix<TElementType> AffinityMatrixType;

  /** Method for creation */
  itkNewMacro(Self);

  /** Run-time type information */
  itkTypeMacro(Self,Object);

  itkSetMacro(NumberOfClusters, unsigned int);
  itkGetMacro(NumberOfClusters, unsigned int);

  /** Set the affinity matrix.
   * Sets the affintiy matrix for the clustering algorithm.
   */
  void SetAffinityMatrix(const AffinityMatrixType & am) {m_AffinityMatrix = am;}
  /** Get reference to the clustering indicies.  The vector returned
   * by this class is of length == # rows in affinity matrix.  Each entry
   * is the class 0 <= a[i] < NumberOfClusters.*/
  std::vector<unsigned int>& GetClasses() {return m_Classes;}
  const std::vector<unsigned int>& GetClasses() const {return m_Classes;}

  /** Method to cause the data to be generated. */
  void Evaluate();
  
 protected:
  NormalizedCut();
  virtual ~NormalizedCut() {};
  void PrintSelf(std::ostream& os, Indent indent) const;

  TElementType CutValue(const DiagonalMatrixType& D,
                        const AffinityMatrixType& W,
                        const std::vector<bool>& cutting) const;
  std::vector<bool> Recursive2WayCutBase(const std::vector<int>&) const;
  void Recursive2WayCut(unsigned int, unsigned int);
  TElementType Recursive2WayCutTest(unsigned int, unsigned int) const;

 private:
  NormalizedCut(const Self&);
  void operator=(const Self&);

  /** The matrix for the system */
  AffinityMatrixType m_AffinityMatrix;

  /** Number of cuts */
  unsigned int m_NumberOfClusters;
  
  std::vector<unsigned int> m_Classes;
  std::vector<TElementType> m_CutValue;

};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkNormalizedCut.txx"
#endif


#endif
