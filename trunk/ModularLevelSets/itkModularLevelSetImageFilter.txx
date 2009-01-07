
#ifndef __itkModularLevelSetImageFilter_txx_
#define __itkModularLevelSetImageFilter_txx_

#include "itkModularLevelSetImageFilter.h"

namespace itk {

template <class TSolver, class TLevelSetFunction>
ModularLevelSetImageFilter<TSolver, TLevelSetFunction>
::ModularLevelSetImageFilter()
{
  m_LevelSetFunction = LevelSetFunctionType::New();

//  //TODO: this should be taken care of within the functions.
//  // uncomment to use ModularLevelSetImageFilter with any ITK Level Set function
//  typename LevelSetFunctionType::RadiusType r;
//  r.Fill(1);
//
//  m_LevelSetFunction->Initialize(r);

//TODO: in ITK, add an Initialize() method to all the function with r.Fill(1);
  m_LevelSetFunction->Initialize();

  this->SetDifferenceFunction(m_LevelSetFunction);
}

template <class TSolver, class TLevelSetFunction>
ModularLevelSetImageFilter<TSolver, TLevelSetFunction>
::~ModularLevelSetImageFilter()
{
}

template <class TSolver, class TLevelSetFunction>
void
ModularLevelSetImageFilter<TSolver, TLevelSetFunction>
::Initialize()
{
  Superclass::Initialize();
}

template <class TSolver, class TLevelSetFunction>
void
ModularLevelSetImageFilter<TSolver, TLevelSetFunction>
::GenerateData()
{
  Superclass::GenerateData();
}

template <class TSolver, class TLevelSetFunction>
void
ModularLevelSetImageFilter<TSolver, TLevelSetFunction>
::PrintSelf(std::ostream &os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
  os << "LevelSetFunction: " << m_LevelSetFunction;
}

} // end namespace itk

#endif
