
#include <vector>

namespace itk
{

template <class ValueType>
class SparseVector 
{
public:
  typedef unsigned int  IndexType;
  typedef std::pair<IndexType, ValueType> PairType;

  // Fast access. Allow non-unique elements.
  typedef std::vector< PairType > ContainerType;

  SparseVector() : m_Container()
    {
    m_DefaultValue = 0;
    }

  void Set( IndexType index, ValueType& val )
    {
    PairType pair( index, val );
    this->InsertIntoContainer( pair );
    }

  /*
  bool IsInContainer( IndexType index )
    {
    ContainerType::iterator iter = m_Container.find( index );
    if ( iter == m_Container.end() )
      {
      return false;
      }
    else
      {
      return true;
      }
    }
  */

  template <class NonSparseVectorType>
  void ConvertToNonSparse( NonSparseVectorType& inVect )
    {
    // Won't work for non-zero fill
    inVect.Fill( 0.0 );
    this->AddToNonSparse( inVect );
    }

  template <class NonSparseVectorType>
  void AddToNonSparse( NonSparseVectorType& inVec )
    {
    for (ContainerType::iterator iter = m_Container.begin();
          iter != m_Container.end();
          ++iter)
      {
      inVect[ (*iter).first ] += (*iter).second;
      }    
    }

  template <class NonSparseVectorType>
  void AddToNonSparseWithWeight( NonSparseVectorType& inVec, double weight )
    {
    for (ContainerType::iterator iter = m_Container.begin();
          iter != m_Container.end();
          ++iter)
      {
      inVect[ (*iter).first ] += ( (*iter).second * weight );
      }    
    }

  template <class NonSparseVectorType>
  void SubtractFromNonSparse( NonSparseVectorType& inVect )
    {
    for (ContainerType::iterator iter = m_Container.begin();
          iter != m_Container.end();
          ++iter)
      {
      inVect[ (*iter).first ] -= (*iter).second;
      }    
    }

  template <class NonSparseVectorType>
  void SubtractFromNonSparseWithWeight( NonSparseVectorType& inVect, double weight )
    {
    for (ContainerType::iterator iter = m_Container.begin();
          iter != m_Container.end();
          ++iter)
      {
      inVect[ (*iter).first ] -= ( (*iter).second * weight );
      }    
    }

protected:
  ValueType       m_DefaultValue;
  ContainerType   m_Container;

  void InsertIntoContainer( PairType& pair )
    {
    this->m_Container.push_back( pair );
    }

  class IndexCompare
    { 
    public:
      IndexCompare( IndexType searchIndex )
        {
        m_SearchIndex = searchIndex;
        }

      bool operator()( PairType& p1 )
        {
        if (p1 == m_SearchIndex)
          {
          return true;
          }
        return false;
        }

      void SetSearchIndex( IndexType searchIndex )
        {
        m_SearchIndex = searchIndex;
        }

    private:
      IndexType m_SearchIndex;
    };

public:
  SparseVector( const SparseVector& in ) 
    {
    //sampleADerivatives.resize( m_NumberOfSpatialSamples );
    m_DefaultValue = in.m_DefaultValue;
    m_Container    = in.m_Container;
    }

  SparseVector& operator=( const SparseVector& in )
    {
    if ( &in != this )
      {
      this->m_DefaultValue = in.m_DefaultValue;
      this->m_Container    = in.m_Container;
      }
    return *this;
    }

}; // SparseVector

} // namespace itk
