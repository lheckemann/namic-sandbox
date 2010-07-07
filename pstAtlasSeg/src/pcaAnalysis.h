#ifndef _pcaAnalysis_h_
#define _pcaAnalysis_h_


#include <iostream>
#include <list>

#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>

namespace newProstate
{
  template<typename TData>
  class CpcaAnalysis
  {
    /* Although tempalted, TData can only be double. Not even float*/

  public:
    CpcaAnalysis();
    //  ~CpcaAnalysis();

    typedef vnl_vector<TData> TVnlVector;
    typedef vnl_matrix<TData> TVnlMatrix;

    void addData(const TVnlVector& datumVector);
    TVnlVector getData(long i);

    void gogogo();

    void decompose();

    void decenterize();

    TVnlVector getMean();
    TVnlVector getEigenMode( long i);
    TVnlMatrix& getEigenModeMatrix();
  
    long getNumberOfEigenModes() { return m_N>m_d?m_d:m_N; }

    // data
    std::list< TVnlVector > m_dataList;
    TVnlMatrix m_matrixOfAllData; // each column is a long vector of data

    TVnlMatrix m_eigenModes; // each column is a vector of eigen mode, 
    TVnlVector m_eigenValues;  

    TVnlVector m_mean; 



    bool m_centered;
    bool m_decomposed;

    long m_N; // total number of data
    long m_d; // dimension of every datum


  protected:
    void listToMatrix();
    bool m_listHasBeenPortedToMatrix;
  };

} // douher

#include "pcaAnalysis.hpp"


#endif
