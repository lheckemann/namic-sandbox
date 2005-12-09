#ifndef _itkMesh3DProcrustesAlignFilter_h
#define _itkMesh3DProcrustesAlignFilter_h

#include "itkProcessObject.h"
#include "itkAffineTransform.h"
#include "itkTransformMeshFilter.h"


namespace itk
{
  /** \class Mesh3DProcrustesAlignFilter
  *   \brief Class for groupwise aligning a set of 3D meshes.
  * 
  * All input meshes must have the same number of points (with corresponding 
  * indices).
  * Default mode: Iterative Generalized Procrustes alignment with initialization
  * from first object
  * Option: UseInitialAverageOn/Off() Use  average structure as initialization (off by default)
  *   Only appropriate if objects are already pre-aligned 
  * Option: UseSingleIterationOn/Off() Only run one iteration (off by default) 
  * 
  * All output meshes will be centered at the origin and scaled to 
  * match a mean shape with norm 1.
  * Option: UseScalingOn/Off() disables scaling normalization (off by default)
  *
  * GetTransform() can be used to query the employed transformation for each 
  * input mesh.
  *
  * \author Tobias Heimann. Division Medical and Biological Informatics, 
  *         German Cancer Research Center, Heidelberg, Germany.
  * change  Martin Styner, UNC support for single template and initialization with average
  */
  template <class TInputMesh, class TOutputMesh>
  class Mesh3DProcrustesAlignFilter : public ProcessObject
  {

  public:

    /** Standard typedefs. */
    typedef Mesh3DProcrustesAlignFilter   Self;
    typedef ProcessObject                 Superclass;
    typedef SmartPointer<Self>            Pointer;
    typedef SmartPointer<const Self>      ConstPointer;

    /** Convenient typedefs. */
    typedef TInputMesh                                  InputMeshType;
    typedef TOutputMesh                                 OutputMeshType;
    typedef typename InputMeshType::Pointer             InputMeshPointer;
    typedef typename OutputMeshType::Pointer            OutputMeshPointer;
    typedef typename InputMeshType::PointType           InputPointType;
    typedef typename OutputMeshType::PointType          OutputPointType;
    typedef typename OutputMeshType::PointsContainer    OutputPointsContainer;
    typedef typename OutputMeshType::PointsContainerPointer OutputPointsContainerPointer;
    typedef DataObject::Pointer                         DataObjectPointer;
    typedef typename OutputMeshType::CoordRepType       CoordRepType;
    typedef vnl_matrix<CoordRepType>                    MatrixType;
    typedef AffineTransform<CoordRepType, 3>            TransformType;
    typedef typename TransformType::Pointer             TransformPointer;
    typedef TransformMeshFilter <OutputMeshType, OutputMeshType, TransformType>   TransformMeshType;
    typedef typename TransformMeshType::Pointer         TransformMeshPointer;
    typedef std::vector<TransformMeshPointer>           TransformMeshArray;
    typedef typename TransformType::OffsetType          TranslationType;
    typedef typename TransformType::MatrixType          RotationType;        
    
    /** Method for creation through the object factory. */
    itkNewMacro(Self);

    /** Run-time type information (and related methods). */
    itkTypeMacro(Mesh3DProcrustesAlignFilter, ProcessObject);

    /** Sets the number of input meshes that have to be aligned.
    * Call this before any other methods.
    */
    void SetNumberOfInputs( unsigned int num );

    /** Sets one input mesh (starting with idx=0). */
    void SetInput( unsigned int idx, InputMeshPointer mesh );

    /** Returns one of the input meshes (starting with idx=0). */
    InputMeshType* GetInput( unsigned int idx )
    {
      return static_cast<InputMeshType*>(this->ProcessObject::GetInput( idx ));
    }

    /** Gets one transformed output mesh (starting with idx=0). */
    OutputMeshType* GetOutput(unsigned int idx);

    /** Gets the transformed mean of all aligned meshes. */
    itkGetConstObjectMacro( Mean, OutputMeshType );

    /** Returns the transform used to align a mesh (starting with idx=0). */
    TransformType* GetTransform( unsigned int idx )
    {
      return m_MeshTransform[idx]->GetTransform();
    } 

    /** Get/Set the convergence value that determines when the iterative
    * calculation of alignment is stopped. The smaller the value, the higher
    * the accuracy (the default should be sufficient for all applications).
    */
    itkGetConstMacro( Convergence, double );
    itkSetMacro (Convergence, double );

    /** Creates an ouput object. */
    virtual DataObjectPointer MakeOutput(unsigned int idx);

    /** Normalization with Scaling on (default)*/
    void SetUseScalingOn()
    { this->SetUseScaling(true); }
    
    /** Normalization with Scaling off */
    void SetUseScalingOff()
    { this->SetUseScaling(false); }
    
    /** Set/Get whether or not the filter will use Scaling normalization or not */
    itkSetMacro(UseScaling, bool);
    itkGetMacro(UseScaling, bool);

    /** use average as reference for initialization */
    void SetUseInitialAverageOn()
    { this->SetUseInitialAverage(true); }
    
    /** do not use average as reference for initialization. The first surface will be used instead.(default) */
    void SetUseInitialAverageOff()
    { this->SetUseInitialAverage(false); }
    
    /** Set/Get whether or not the average is used as initialization */
    itkSetMacro(UseInitialAverage, bool);
    itkGetMacro(UseInitialAverage, bool);

    /** Only do one iteration */
    void SetUseSingleIterationOn()
    { this->SetUseSingleIteration(true); }
    
    /** Iterate until convergence (default)*/
    void SetUseSingleIterationOff()
    { this->SetUseSingleIteration(false); }
    
    /** Set/Get whether or not only one iteration should be run */
    itkSetMacro(UseSingleIteration, bool);
    itkGetMacro(UseSingleIteration, bool);

  protected:

    /** Standard constructor. */
    Mesh3DProcrustesAlignFilter();

    /** Standard destructor. */
    ~Mesh3DProcrustesAlignFilter();

    /** Performs the alignment. */
    virtual void GenerateData();

    /** Calculates the center coordinates of the specified input mesh. */
    TranslationType GetMeshCenter( unsigned int idx );

    /** Uses the current transformations to calculate a new mean.*/
    void CalculateMean();

    /** Returns the best transform to fit input mesh idx, translated to  
     * zero origin by using the values in m_Center, to the given targetMesh. 
     * targetMesh has to be centered at zero origin as well!
     */
    TransformPointer GetProcrustesMatch( unsigned int idx, OutputMeshPointer targetMesh );
    
  private:

    /** When the difference between two consecutive mean calculations gets
    * samller than m_Convergence, the alignment is terminated. */
    double                        m_Convergence;
    /** Holds the transforms for all input meshes.*/
    TransformMeshArray            m_MeshTransform;
    /** Holds the center coordinates for all input meshes.*/
    std::vector<TranslationType>  m_Center;
    /** The mean of all transformed meshes. */
    OutputMeshPointer             m_Mean;
    /** The mean of all transformed meshes from the preceding iteration. */
    OutputMeshPointer             m_OldMean;

    /** Scaling normalization on/off */
    bool                          m_UseScaling;

    /** Use Average for initialization on/off */
    bool                          m_UseInitialAverage;

    /** Only run one single iteration on/off */
    bool                          m_UseSingleIteration;
    
  };

  
}

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkMesh3DProcrustesAlignFilter.txx"
#endif

#endif
