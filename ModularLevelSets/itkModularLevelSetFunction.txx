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
// TODO: uncomment this to go the singletons way
//  FunctionTermMapIterator termIt;
//  typename LevelSetTermType::DependencyMapIterator depIt;
//
//  FunctionTermMapType dependencyMap;
//
//  for (termIt = m_FunctionTermMap.begin(); termIt != m_FunctionTermMap.end(); ++termIt)
//    {
//    std::string name = (*termIt).first;
//    LevelSetTermPointer term = (*termIt).second;
//
//    for (depIt = term->m_DependencyMap.begin(); depIt != term->m_DependencyMap.end(); ++depIt)
//      {
//      std::string dependencyName = (*depIt).first;
//      LevelSetTermPointer dependencyTerm = (*depIt).second;
//      if (m_FunctionTermMap.count(dependencyName) == 0)
//        {
////        std::cout<<"Adding dependency term for "<<name<<": "<<dependencyName<<"."<<std::endl;
//        dependencyMap[dependencyName] = dependencyTerm;
//        }
//      }
//    }
//  
//  for (termIt = dependencyMap.begin(); termIt != dependencyMap.end(); ++termIt)
//    {
//    m_FunctionTermMap[(*termIt).first] = (*termIt).second;
//    }

  FunctionTermMapIterator termIt;
  typename LevelSetTermType::DependencyMapIterator depIt;

  for (termIt = m_FunctionTermMap.begin(); termIt != m_FunctionTermMap.end(); ++termIt)
    {
    LevelSetTermPointer term = (*termIt).second;
    for (depIt = term->m_DependencyMap.begin(); depIt != term->m_DependencyMap.end(); ++depIt)
      {
      LevelSetTermPointer dependencyTerm = (*depIt).second;
      dependencyTerm->Initialize();
      }
    std::cout<<"Initializing "<<(*termIt).first<<"."<<std::endl;
    term->Initialize();
    std::cout<<"Done."<<std::endl;
    }
}

template< class TImageType >
typename ModularLevelSetFunction< TImageType >::PixelType
ModularLevelSetFunction< TImageType >
::ComputeUpdate(const NeighborhoodType &it, void *globalData,
                const FloatOffsetType& offset)
{
  FunctionTermMapIterator termIt;
  typename LevelSetTermType::DependencyMapIterator depIt;

  PixelType functionValue = NumericTraits<PixelType>::Zero;

  for (termIt = m_FunctionTermMap.begin(); termIt != m_FunctionTermMap.end(); ++termIt)
    {
    std::string name = (*termIt).first;
    LevelSetTermPointer term = (*termIt).second;
    LevelSetTermGlobalDataType* termGlobalData = (LevelSetTermGlobalDataType*)term->GetGlobalDataPointer();
//TODO: solve problem here: what if dependencies have dependencies?
    for (depIt = term->m_DependencyMap.begin(); depIt != term->m_DependencyMap.end(); ++depIt)
      {
      std::string dependencyName = (*depIt).first;
      LevelSetTermPointer dependencyTerm = (*depIt).second;
      LevelSetTermGlobalDataType* dependencyGlobalData = (LevelSetTermGlobalDataType*)dependencyTerm->GetGlobalDataPointer();
      dependencyTerm->ComputeUpdate(it,dependencyGlobalData,offset);
      termGlobalData->m_DependencyGlobalDataMap[dependencyName] = dependencyGlobalData;
      }

    functionValue += term->ComputeUpdate(it,termGlobalData,offset);

    for (depIt = term->m_DependencyMap.begin(); depIt != term->m_DependencyMap.end(); ++depIt)
      {
      std::string dependencyName = (*depIt).first;
      LevelSetTermPointer dependencyTerm = (*depIt).second;
      dependencyTerm->ReleaseGlobalDataPointer(termGlobalData->m_DependencyGlobalDataMap[dependencyName]);
      }

    term->ReleaseGlobalDataPointer(termGlobalData);
    }

  return functionValue;
}

} // end namespace itk

#endif
