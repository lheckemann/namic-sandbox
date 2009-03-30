#ifndef __itkLevelSetTerm_h_
#define __itkLevelSetTerm_h_

#include "itkFiniteDifferenceFunction.h"

#include <map>

#define itkRegisterGlobalDataMacro \
typedef GlobalDataStruct GlobalDataType; \
virtual void *GetGlobalDataPointer() const \
{ \
  GlobalDataStruct *ans = new GlobalDataStruct(); \
  return ans; \
} \
virtual void ReleaseGlobalDataPointer(void *GlobalData) const \
{ delete (GlobalDataStruct*) GlobalData; }

namespace itk {

template <class TImage>
class ITK_EXPORT LevelSetTerm
  : public FiniteDifferenceFunction<TImage>
{
public:
  /** Standard class typedefs. */
  typedef LevelSetTerm Self;
  typedef FiniteDifferenceFunction<TImage> Superclass;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self> ConstPointer;

  /** Run-time type information (and related methods) */
  itkTypeMacro(LevelSetTerm, FiniteDifferenceFunction);

  /** Extract some parameters from the superclass. */
  itkStaticConstMacro(ImageDimension, unsigned int, Superclass::ImageDimension);

  /** Convenient typedefs. */
  typedef double TimeStepType;
  typedef typename Superclass::ImageType  ImageType;
  typedef typename Superclass::PixelType  PixelType;
  typedef                      PixelType  ScalarValueType;
  typedef typename Superclass::PixelRealType  PixelRealType;
  typedef typename Superclass::RadiusType RadiusType;
  typedef typename Superclass::NeighborhoodType NeighborhoodType;
  typedef typename Superclass::NeighborhoodScalesType NeighborhoodScalesType;
  typedef typename Superclass::FloatOffsetType FloatOffsetType;

  typedef std::map<std::string,Pointer> DependencyMapType;

  typedef typename DependencyMapType::const_iterator DependencyMapConstIterator;
  typedef typename DependencyMapType::iterator DependencyMapIterator;
  
  typedef std::map<std::string,void*> DependencyGlobalDataMapType;

  struct GlobalDataStruct
  {
    GlobalDataStruct()
    {
      m_MaxChange = NumericTraits<TimeStepType>::Zero;
    }
    bool m_Computed;
    ScalarValueType m_MaxChange;
    DependencyGlobalDataMapType m_DependencyGlobalDataMap;
  };

  typedef GlobalDataStruct GlobalDataType;

  virtual void Initialize()
  {
    NeighborhoodType it;
    it.SetRadius(this->GetRadius());

    m_Center =  it.Size() / 2;

    for(unsigned int i = 0; i < ImageDimension; i++)
    {  m_xStride[i] = it.GetStride(i); }
  }

  virtual void Initialize(const RadiusType &r) 
  {
    this->SetRadius(r);
    this->Initialize();
  }

  bool IsCached()
  {
    return m_Cached;
  }

  virtual void SetWeight(const ScalarValueType w)
  { m_Weight = w; }

  ScalarValueType GetWeight() const
  { return m_Weight; }

  virtual PixelType ComputeUpdate(const NeighborhoodType &neighborhood,
                                  void *globalData,
                                  const FloatOffsetType& offset) = 0;

  virtual TimeStepType ComputeGlobalTimeStep(void *GlobalData) const = 0;

  virtual void *GetGlobalDataPointer() const = 0;

  virtual void ReleaseGlobalDataPointer(void *GlobalData) const = 0;

  DependencyMapType m_DependencyMap;

protected:
  LevelSetTerm() 
  {
    RadiusType r;
    r.Fill(1);
    this->SetRadius(r);

    m_Cached = false;
  }

  virtual ~LevelSetTerm() {}

  void PrintSelf(std::ostream &s, Indent indent) const {}

  void AddDependency(Self* term)
  {
    if (this->IsCached() && !term->IsCached())
      {
      ExceptionObject exception(__FILE__, __LINE__);
      std::string errorMessage = "Error, Cannot set non-cached term as a dependency for a cached term.";
      exception.SetDescription(errorMessage.c_str());
      exception.SetLocation(ITK_LOCATION);
      throw exception;
      }
    this->m_DependencyMap[term->GetNameOfClass()] = term;
  }

  ScalarValueType m_Weight;

  bool m_Cached;

  ::size_t m_Center;

  ::size_t m_xStride[itkGetStaticConstMacro(ImageDimension)];

private:
  LevelSetTerm(const Self&); //purposely not implemented
  void operator=(const Self&);   //purposely not implemented
};

} // end namespace itk

#endif
