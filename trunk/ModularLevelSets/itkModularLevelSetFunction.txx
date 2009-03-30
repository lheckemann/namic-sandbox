#ifndef __itkModularLevelSetFunction_txx_
#define __itkModularLevelSetFunction_txx_
  
#include "itkModularLevelSetFunction.h"

namespace itk {

template <class TImageType>
void
ModularLevelSetFunction<TImageType>::
PrintSelf(std::ostream& os, Indent indent) const
{
}

template< class TImageType >
typename ModularLevelSetFunction< TImageType >::TimeStepType
ModularLevelSetFunction<TImageType>
::ComputeGlobalTimeStep(void *GlobalData) const
{
//  return NumericTraits<TimeStepType>::One;
  return 0.001;
}

template< class TImageType >
void
ModularLevelSetFunction< TImageType>
::Initialize()
{
  LevelSetTermMapIterator termIt;

  m_CachedLevelSetTermMap.erase(m_CachedLevelSetTermMap.begin(),m_CachedLevelSetTermMap.end());

  for (termIt = m_LevelSetTermMap.begin(); termIt != m_LevelSetTermMap.end(); ++termIt)
    {
    std::string name = (*termIt).first;
    LevelSetTermPointer term = (*termIt).second;
    if (m_UseCaching && term->IsCached())
      {
      m_CachedLevelSetTermMap[name] = term;
      std::cout<<"Cached "<<name<<" "<<term<<std::endl;
      }
    std::cout<<"Initializing dependencies of "<<name<<"."<<std::endl;
    this->InitializeDependency(term);
    std::cout<<"Initializing "<<name<<"."<<std::endl;
    term->Initialize();
    std::cout<<"Done."<<std::endl;
    }
}

template< class TImageType >
void
ModularLevelSetFunction< TImageType >
::InitializeDependency(LevelSetTermPointer term)
{
  typename LevelSetTermType::DependencyMapIterator depIt;
  for (depIt = term->m_DependencyMap.begin(); depIt != term->m_DependencyMap.end(); ++depIt)
    {
    std::string dependencyName = (*depIt).first;
    LevelSetTermPointer dependencyTerm = (*depIt).second;
    if (m_UseCaching && dependencyTerm->IsCached())
      {
      m_CachedLevelSetTermMap[dependencyName] = dependencyTerm;
      std::cout<<"Cached "<<dependencyName<<" "<<dependencyTerm<<std::endl;
      }
    std::cout<<"Initializing dependencies of "<<dependencyName<<"."<<std::endl;
    this->InitializeDependency(dependencyTerm);
    std::cout<<"Initializing "<<dependencyName<<"."<<std::endl;
    dependencyTerm->Initialize();
    std::cout<<"Done."<<std::endl;
    }
}

template< class TImageType >
void
ModularLevelSetFunction< TImageType >
::ComputeDependencyUpdate(const NeighborhoodType &it, 
                          LevelSetTermPointer term, 
                          void *globalData,
                          LevelSetTermGlobalDataType *termGlobalData, 
                          const FloatOffsetType& offset)
{
  CachedGlobalDataMapType& cachedGlobalDataMap = ((GlobalDataType*)globalData)->m_CachedGlobalDataMap;
  typename LevelSetTermType::DependencyMapIterator depIt;
  for (depIt = term->m_DependencyMap.begin(); depIt != term->m_DependencyMap.end(); ++depIt)
    {
    std::string dependencyName = (*depIt).first;
    LevelSetTermPointer dependencyTerm = (*depIt).second;
    LevelSetTermGlobalDataType* dependencyGlobalData = NULL;
    if (m_UseCaching && dependencyTerm->IsCached() && cachedGlobalDataMap.find(dependencyName) != cachedGlobalDataMap.end())
      {
      dependencyGlobalData = cachedGlobalDataMap[dependencyName]; 
      }
    else
      {
      dependencyGlobalData = (LevelSetTermGlobalDataType*)dependencyTerm->GetGlobalDataPointer();
      this->ComputeDependencyUpdate(it,dependencyTerm,globalData,dependencyGlobalData,offset);
      dependencyTerm->ComputeUpdate(it,dependencyGlobalData,offset);
      if (m_UseCaching && dependencyTerm->IsCached())
        {
        cachedGlobalDataMap[dependencyName] = dependencyGlobalData;
        }
      }
    termGlobalData->m_DependencyGlobalDataMap[dependencyName] = dependencyGlobalData;
    }
}

template< class TImageType >
void
ModularLevelSetFunction< TImageType >
::ReleaseDependencyGlobalDataPointer(LevelSetTermPointer term, 
                                     LevelSetTermGlobalDataType *termGlobalData) 
{
  typename LevelSetTermType::DependencyMapIterator depIt;
  for (depIt = term->m_DependencyMap.begin(); depIt != term->m_DependencyMap.end(); ++depIt)
    {
    std::string dependencyName = (*depIt).first;
    LevelSetTermPointer dependencyTerm = (*depIt).second;
    if (m_UseCaching && dependencyTerm->IsCached())
      {
      continue;
      }
    LevelSetTermGlobalDataType* dependencyGlobalData = (LevelSetTermGlobalDataType*)termGlobalData->m_DependencyGlobalDataMap[dependencyName];
    this->ReleaseDependencyGlobalDataPointer(dependencyTerm,dependencyGlobalData);
    dependencyTerm->ReleaseGlobalDataPointer(termGlobalData->m_DependencyGlobalDataMap[dependencyName]);
    }
}

template< class TImageType >
typename ModularLevelSetFunction< TImageType >::PixelType
ModularLevelSetFunction< TImageType >
::ComputeUpdate(const NeighborhoodType &it, 
                void *globalData,
                const FloatOffsetType& offset)
{
  LevelSetTermMapIterator termIt;

  PixelType functionValue = NumericTraits<PixelType>::Zero;

  CachedGlobalDataMapType& cachedGlobalDataMap = ((GlobalDataType*)globalData)->m_CachedGlobalDataMap;

  for (termIt = m_LevelSetTermMap.begin(); termIt != m_LevelSetTermMap.end(); ++termIt)
    {
    std::string name = (*termIt).first;
    LevelSetTermPointer term = (*termIt).second;
    LevelSetTermGlobalDataType* termGlobalData = (LevelSetTermGlobalDataType*)term->GetGlobalDataPointer();
    this->ComputeDependencyUpdate(it,term,globalData,termGlobalData);
    functionValue += term->ComputeUpdate(it,termGlobalData,offset);
    this->ReleaseDependencyGlobalDataPointer(term,termGlobalData);
    term->ReleaseGlobalDataPointer(termGlobalData);
    }

  for (termIt = m_CachedLevelSetTermMap.begin(); termIt != m_CachedLevelSetTermMap.end(); ++termIt)
    {
    std::string name = (*termIt).first;
    LevelSetTermPointer term = (*termIt).second;
    LevelSetTermGlobalDataType* termGlobalData = cachedGlobalDataMap[name];
    //std::cout<<"Releasing "<<name<<" "<<termGlobalData<<" "<<termGlobalData->m_MaxChange<<std::endl;
// dealloc problem here
    term->ReleaseGlobalDataPointer(termGlobalData);
//    std::cout<<"Done releasing "<<name<<std::endl;
    }
  cachedGlobalDataMap.erase(cachedGlobalDataMap.begin(),cachedGlobalDataMap.end());

  return functionValue;
}

} // end namespace itk

#endif
