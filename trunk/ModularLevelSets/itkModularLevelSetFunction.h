#ifndef __itkModularLevelSetFunction_h_
#define __itkModularLevelSetFunction_h_

#include "itkFiniteDifferenceFunction.h"
#include "itkLevelSetTerm.h"

#include <map>

namespace itk {

template <class TImageType>
class ITK_EXPORT ModularLevelSetFunction
  : public FiniteDifferenceFunction<TImageType>
{
public:
  /** Standard class typedefs. */
  typedef ModularLevelSetFunction Self;
  typedef FiniteDifferenceFunction<TImageType> Superclass;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self> ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods) */
  itkTypeMacro(ModularLevelSetFunction, FiniteDifferenceFunction);

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

  typedef LevelSetTerm<ImageType> LevelSetTermType;
  typedef SmartPointer<LevelSetTermType> LevelSetTermPointer;
  typedef typename LevelSetTermType::GlobalDataType LevelSetTermGlobalDataType;

  typedef std::map<std::string, LevelSetTermPointer> FunctionTermMapType;
  typedef typename FunctionTermMapType::const_iterator FunctionTermMapConstIterator;
  typedef typename FunctionTermMapType::iterator FunctionTermMapIterator;

  /** A global data type for this class of equations.  Used to store
   * values that are needed in calculating the time step and other intermediate
   * products such as derivatives that may be used by virtual functions called
   * from ComputeUpdate.  Caching these values here allows the ComputeUpdate
   * function to be const and thread safe.*/
  struct GlobalDataStruct
  {
  // TODO: the global data struct must be a list (or map) of void* to GlobalDataStructs belonging to each term.
  // Each struct is always a GlobalDataStruct. Each term has to have a m_MaxChange inside its GlobalDataStruct.
  };

//    ScalarValueType m_MaxAdvectionChange;
//    ScalarValueType m_MaxPropagationChange;
//    ScalarValueType m_MaxCurvatureChange;
//  
//    /** Hessian matrix */
//    vnl_matrix_fixed<ScalarValueType,
//                     itkGetStaticConstMacro(ImageDimension),
//                     itkGetStaticConstMacro(ImageDimension)> m_dxy;
//    
//    /** Array of first derivatives*/
//    ScalarValueType m_dx[itkGetStaticConstMacro(ImageDimension)];
//
//    ScalarValueType m_dx_forward[itkGetStaticConstMacro(ImageDimension)];
//    ScalarValueType m_dx_backward[itkGetStaticConstMacro(ImageDimension)];
//
//    ScalarValueType m_GradMagSqr;

  /** Compute the equation value. */
  virtual PixelType ComputeUpdate(const NeighborhoodType &neighborhood,
                                  void *globalData,
                                  const FloatOffsetType& = FloatOffsetType(0.0));

  /** Computes the time step for an update given a global data structure.
    * The data used in the computation may take different forms depending on
    * the nature of the equations.  This global data cannot be kept in the
    * instance of the equation object itself since the equation object must
    * remain stateless for thread safety.  The global data is therefore managed
    * for each thread by the finite difference solver filters. */
  virtual TimeStepType ComputeGlobalTimeStep(void *GlobalData) const;

  /** Returns a pointer to a global data structure that is passed to this
   * object from the solver at each calculation.  The idea is that the solver
   * holds the state of any global values needed to calculate the time step,
   * while the equation object performs the actual calculations.  The global
   * data should also be initialized in this method.  Global data can be used
   * for caching any values used or reused by the FunctionObject.  Each thread
   * should receive its own global data struct. */
  virtual void *GetGlobalDataPointer() const
  {
    GlobalDataStruct *ans = new GlobalDataStruct();
    return ans;
  }

  /** This method creates the appropriate member variable operators for the
   * level-set calculations.  The argument to this function is a the radius
   * necessary for performing the level-set calculations. */
  virtual void Initialize();
  /** For backwards compatibility. */
  virtual void Initialize(const RadiusType &r) 
  {
//    this->SetRadius(r);
//    this->Initialize();
  }

  /** When the finite difference solver filter has finished using a global
   * data pointer, it passes it to this method, which frees the memory.
   * The solver cannot free the memory because it does not know the type
   * to which the pointer points. */
  virtual void ReleaseGlobalDataPointer(void *GlobalData) const
    { delete (GlobalDataStruct *) GlobalData; }

  void AddTerm(LevelSetTermType* term)
  {
    m_FunctionTermMap[term->GetNameOfClass()] = term;
  }

  LevelSetTermPointer GetTerm(const char* name)
  {
    return m_FunctionTermMap[name];
  }

protected:
  ModularLevelSetFunction() 
  {
    RadiusType r;
    r.Fill(1);
    this->SetRadius(r);
  }

  virtual ~ModularLevelSetFunction() {}

  void PrintSelf(std::ostream &s, Indent indent) const;

  FunctionTermMapType m_FunctionTermMap;

private:
  ModularLevelSetFunction(const Self&); //purposely not implemented
  void operator=(const Self&);   //purposely not implemented
};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkModularLevelSetFunction.txx"
#endif

#endif
