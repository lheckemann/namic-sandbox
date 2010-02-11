#ifndef pcaAnalysis_hpp_
#define pcaAnalysis_hpp_


#include "pcaAnalysis.h"

#include <vnl/vnl_math.h>
#include <vnl/algo/vnl_symmetric_eigensystem.h>

#include <csignal>

namespace douher
{
  ////////////////////////////////////////////////////////////
  template<typename TData>
  CpcaAnalysis<TData>::CpcaAnalysis()
  {
    m_N = 0;
    m_d = 0;

    m_centered = false;

    m_listHasBeenPortedToMatrix = false;
  }

  ////////////////////////////////////////////////////////////
  template<typename TData>
  void 
  CpcaAnalysis<TData>::addData(const TVnlVector& datumVector)
  {
    if (m_listHasBeenPortedToMatrix)
      {
        std::cerr<<"Error: Data in the list has already been ported to matrix. Do not accept more data.\n";
        raise(SIGABRT);
      }

    if ( !m_dataList.empty() && (unsigned long)m_d != datumVector.size() )
      {
        std::cerr<<"Error: size of the new vector doesn't match those in the list.\n";
        raise(SIGABRT);
      }

    m_d = datumVector.size();
    if ( m_d == 0 )
      {
        std::cerr<<"Error: dimension of data is 0.\n";
        raise(SIGABRT);
      }

    m_dataList.push_back(datumVector);


    ++m_N;
  }


  ////////////////////////////////////////////////////////////
  template<typename TData>
  void 
  CpcaAnalysis<TData>::decompose()
  {
    if ( m_matrixOfAllData.empty() )
      {
        /*==================================================
          This just test if data matrix has been allocated, it
          may be possible that the data has not been filled in. But
          it's not easy to test that without prior knowledge.

          So, it's the user's responsibility to make sure fill in the
          data matrix then run this.
        */
        std::cerr<<"data matrix not ready. abort. \n";
        raise(SIGABRT);
      }

    if (!m_centered)
      {
        std::cerr<<"Warning: data matrix not de-centered. \n";
        raise(SIGABRT);
      }


    if (m_N < m_d)
      {
        /*==================================================
          If m_N < m_d
    
          Construct the covariant matrix E = Mdecenter' * Mdecenter
          where Mdecenter' is Mdecenter transpose
    
          Note, E is of size N*N, not d*d like C=Mdecenter*Mdecenter', this reduces the
          computation. For the relation between E, C and eig vct of E
          and C, see work log(paper log, not the workLog file) #5 P6~7'
        */

        TVnlMatrix E = ( m_matrixOfAllData.transpose() )*m_matrixOfAllData/(double)m_N;
        vnl_symmetric_eigensystem<double>  eig(E);

        m_eigenValues = eig.D.diagonal();
        m_eigenValues.flip(); // now biggest value at beginning

        m_eigenModes = m_matrixOfAllData*(eig.V); // each column is a vector of eigen mode
        m_eigenModes.fliplr(); // now the eig-vector corresponding to the biggest value is at beginning

        /*============================================================
         * After eig.V is multiplicated with the m_matrixOfAllData to
         * get m_eigenModes above, the columns of m_eigenModes are not
         * normalized (l2 sense). The actual eigen vector (same size as
         * the actual data one), u_i, is obtained by normalizing by its
         * l2 norm, or it can be written as u_i =
         * (X*v_i)/sqrt(N*lambda_i).
         */
      
        /* Furthermore, eigen vectors are multiplied by their
         * corresponding sqrt(eigen value). Thus we don't need to store
         * eigen values somewhere else.
         *
         * But this is arguable, since the output of the eigen vector
         * should be "pure" eigen vector whose 2 norm is 1. Otherwise
         * when reusing this, this point may cause confusion.

         * So now it the sqrt(eigVal) is NOT multiplied  */
        for (int i = 0; i < m_N; ++i)
          {
            TVnlVector u = m_eigenModes.get_column(i);
            double l2 = u.two_norm();

            m_eigenModes.set_column(i, u/l2 );
            // m_eigenModes.set_column(i, sqrt(m_eigenValues[i])*u/l2 );
          }
      }
    else
      {
        TVnlMatrix E = m_matrixOfAllData*( m_matrixOfAllData.transpose() )/(double)m_N;
        //      std::cout<<E<<std::endl;

        vnl_symmetric_eigensystem<double>  eig(E);

        m_eigenValues = eig.D.diagonal();
        m_eigenValues.flip(); // now biggest value at beginning

        m_eigenModes = eig.V; // each column is a vector of eigen mode
        m_eigenModes.fliplr(); // now the eig-vector corresponding to the biggest value is at beginning
      }

    m_decomposed = true;
  }


  ////////////////////////////////////////////////////////////
  template<typename TData>
  void 
  CpcaAnalysis<TData>::decenterize()
  {
    if (m_centered)
      {
        // already decentered
        return;
      }

    if ( m_matrixOfAllData.empty() )
      {
        /*
          This just test if data matrix has been allocated, it
          may be possible that the data has not been filled in. But
          it's not easy to test that without prior knowledge.

          So, it's the user's responsibility to make sure fill in the
          data matrix then run this.
        */
        std::cerr<<"data matrix not ready. abort. \n";
        raise(SIGABRT);
      }


    /*
      step 1. get the mean
    */
    m_mean = m_matrixOfAllData.get_column(0);
    for (long i = 1; i < m_N; ++i)
      {
        m_mean += m_matrixOfAllData.get_column(i);
      }
    m_mean /= (double)m_N;


    /*
      step 2. subtract mean from each column
    */
    for (long i = 0; i < m_N; ++i)
      {
        m_matrixOfAllData.set_column(i, m_matrixOfAllData.get_column(i) - m_mean );
      }

    m_centered = true;
  }


  ////////////////////////////////////////////////////////////
  template<typename TData>
  vnl_vector< TData> 
  CpcaAnalysis<TData>::getMean()
  {
    if ( !m_mean.empty() )
      {
        // case 1. m_mean has already been computed
        return m_mean;
      }

    if ( !m_listHasBeenPortedToMatrix )
      {
        /* case 2.
           data still in list, not in matrix, then compute here,
           and NOT assign to m_mean. Otherwise next time when mean is
           requested, will fall into case 1 and return a wrong value.
        */

        if ( m_dataList.empty() )
          {
            std::cerr<<"data list empty. abort. \n";
            raise(SIGABRT);
          }

        typename std::list< TVnlVector >::const_iterator it = m_dataList.begin();
        TVnlVector temMean = *it;
        ++it;
        for (; it != m_dataList.end(); ++it)
          {
            temMean += (*it);
          }
        return temMean/(double)m_N;
      }

    /* case 3.
       data IS in matrix, then compute mean and store in m_mean.
    */
    if ( m_matrixOfAllData.empty() )
      {
        std::cerr<<"data matrix empty. abort. \n";
        raise(SIGABRT);
      }

    m_mean = m_matrixOfAllData.get_column(0);
    for (long i = 1; i < m_N; ++i)
      {
        m_mean += m_matrixOfAllData.get_column(i);
      }
    m_mean /= (double)m_N;


    return m_mean;
  }

  ////////////////////////////////////////////////////////////
  template<typename TData>
  vnl_matrix<TData>& 
  CpcaAnalysis<TData>::getEigenModeMatrix()
  {
    if (!m_decomposed)
      {
        std::cerr<<"not decomposed yet. Abort.\n";
        raise(SIGABRT);
      }

    return m_eigenModes;
  }


  ////////////////////////////////////////////////////////////
  template<typename TData>
  vnl_vector<TData> 
  CpcaAnalysis<TData>::getEigenMode(long i)
  {
    if (!m_decomposed)
      {
        std::cerr<<"not decomposed yet. Abort.\n";
        raise(SIGABRT);
      }


    if ( i >= (long)m_eigenModes.columns() )
      {
        std::cerr<<"index i exceed total number of eigen modes. Abort.\n";
        raise(SIGABRT);
      }

    return m_eigenModes.get_column(i);
  }



  ////////////////////////////////////////////////////////////
  template<typename TData>
  void 
  CpcaAnalysis<TData>::listToMatrix()
  {
    if ( m_dataList.empty() )
      {
        std::cerr<<"list is empty. abort.\n";
        raise(SIGABRT);
      }

    if ((long)m_dataList.front().size() != m_d)
      {
        std::cerr<<"size of data don't match. abort.\n";
        raise(SIGABRT);
      }

    if ((long)m_dataList.size() != m_N)
      {
        std::cerr<<"number of data doesn't match with size of the list. abort.\n";
        raise(SIGABRT);
      }


    m_matrixOfAllData.set_size(m_d, m_N );

    for (long i = 0; !m_dataList.empty(); ++i)
      {
        m_matrixOfAllData.set_column(i, m_dataList.front() );
        m_dataList.pop_front();
      }

    m_listHasBeenPortedToMatrix = true;
  }


  ////////////////////////////////////////////////////////////
  template<typename TData>
  void 
  CpcaAnalysis<TData>::gogogo()
  {
    /* 
       Step 1. 
       Port data in list to the big data matrix. */
    listToMatrix();

    if ( m_matrixOfAllData.empty() )
      {
        /*
          This just test if data matrix has been allocated, it
          may be possible that the data has not been filled in. But
          it's not easy to test that without prior knowledge.

          So, it's the user's responsibility to make sure fill in the
          data matrix then run this.
        */
        std::cerr<<"data matrix not ready. abort. \n";
        raise(SIGABRT);
      }

    if ( !m_dataList.empty() )
      {
        std::cerr<<"still data in list, why?  abort. \n";
        raise(SIGABRT);
      }


    /*
      Step 2. 
      De-center the data  */

    // debug
    decenterize();


    /*
      Step 3. 
      decompose covariance matrix */
    decompose();
  }

  ////////////////////////////////////////////////////////////
  template<typename TData>
  vnl_vector<TData> 
  CpcaAnalysis<TData>::getData(long i)
  {
    if (!m_listHasBeenPortedToMatrix)
      {
        if ( i >= m_N )
          {
            std::cerr<<"idx exceed number of data, abort.\n";
            raise(SIGABRT);
          }

        typename std::list< TVnlVector >::const_iterator it = m_dataList.begin();
        for (long ii = 0; ii < i; ++ii)
          {
            ++it;
          }
      
        return *it;
      }
    else
      {
        // data have already been in matrix form
        if ( i >= m_N )
          {
            std::cerr<<"idx exceed number of data, abort.\n";
            raise(SIGABRT);
          }

        return m_matrixOfAllData.get_column(i);
      }
  }

}// douher


#endif
