
#ifndef __itkModularLevelSetImageFilter_h_
#define __itkModularLevelSetImageFilter_h_

namespace itk {

template <class TSolver, class TLevelSetFunction>
class ITK_EXPORT ModularLevelSetImageFilter :
  public TSolver
{
public:
  typedef ModularLevelSetImageFilter Self;

  typedef TSolver Superclass;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self> ConstPointer;

  typedef TSolver SolverType;

  typedef TLevelSetFunction LevelSetFunctionType;
  typedef typename LevelSetFunctionType::Pointer LevelSetFunctionPointer;

  itkTypeMacro(ModularLevelSetImageFilter, SolverType);

  itkNewMacro(Self);

  itkGetConstReferenceObjectMacro(LevelSetFunction,LevelSetFunctionType);

protected:
  ModularLevelSetImageFilter();
  virtual ~ModularLevelSetImageFilter();

  virtual void PrintSelf(std::ostream& os, Indent indent) const;

  virtual void Initialize();
  virtual void InitializeIteration();

  virtual void GenerateData();

  //TODO: override halting criterion and force it to ignore RMS change for the moment
  virtual bool Halt()
    {
      if (this->GetElapsedIterations() == this->GetNumberOfIterations()) return true;
      else return false;
    }

private:
  ModularLevelSetImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  LevelSetFunctionPointer m_LevelSetFunction;
};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkModularLevelSetImageFilter.txx"
#endif

#endif
