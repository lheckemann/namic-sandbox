//-----------------------------------------------------------------------------
///\file types.h
///\desc Types for this application
//-----------------------------------------------------------------------------

typedef itk::Image<float,3>                 FloatVolumeType;
typedef itk::Image<unsigned short,3>        UShortVolumeType;
typedef itk::Image<unsigned char,3>         BinaryVolumeType;
typedef itk::Image<double,3>                RealVolumeType;

typedef itk::Image<float, 2>                FloatImageType;
typedef itk::Image<unsigned short, 2>       UShortImageType;

// iterator types
typedef itk::ImageRegionIterator
                        <FloatVolumeType>   FloatVolumeIterator;
typedef itk::ImageRegionIterator
                        <FloatImageType>    FloatImageIterator;
typedef itk::ImageRegionIteratorWithIndex
                        <FloatImageType>    FloatImageIndexedIterator;


// unsigned short to float and back
typedef itk::CastImageFilter< UShortVolumeType,
                        FloatVolumeType>    UShortToFloatCast;
typedef itk::CastImageFilter< FloatVolumeType,
                        UShortVolumeType>   FloatToUShortCast;


typedef itk::CastImageFilter< FloatVolumeType,
                        RealVolumeType>     FloatToRealCast;
typedef itk::CastImageFilter< RealVolumeType,
                        FloatVolumeType>    RealToFloatCast;

typedef itk::CastImageFilter< UShortImageType,
                        FloatImageType>     UShortToFloatImageCast;
typedef itk::CastImageFilter< FloatImageType,
                        UShortImageType>    FloatToUShortImageCast;


//file readers and writers
typedef itk::ImageFileWriter<UShortVolumeType>   VolumeWriterType;
typedef itk::ImageFileReader<UShortVolumeType>   VolumeReaderType;
typedef itk::ImageFileWriter<UShortImageType>    ImageWriterType;
typedef itk::ImageFileReader<UShortImageType>    ImageReaderType;
// series reader writers
typedef itk::ImageSeriesReader<UShortVolumeType> ImageSeriesReaderType;
typedef itk::ImageSeriesWriter<UShortVolumeType,
                               UShortImageType > SeriesWriterType;
// filename generator type
typedef itk::NumericSeriesFileNames              NameGeneratorType;

typedef itk::AnalyzeImageIO                      AnalyzeImageIOType;

// extract slices
typedef itk::ExtractImageFilter
            <FloatVolumeType, FloatImageType >  SliceExtractorType;

// Image histogram related types
typedef itk::Statistics::
   ScalarImageToHistogramGenerator<FloatVolumeType> HistogramGeneratorType;
typedef HistogramGeneratorType::HistogramType       HistogramType;
typedef HistogramType::FrequencyType                FrequencyType;




// Resample Image Filter
typedef itk::ResampleImageFilter
                <FloatVolumeType,FloatVolumeType>   ResampleVolumeFilter;

// normalize intensity filter
typedef itk::NormalizeImageFilter
                <FloatVolumeType, FloatVolumeType>  VolumeNormalizeFilter;
typedef itk::RescaleIntensityImageFilter
                <FloatVolumeType, FloatVolumeType>  RescaleIntensityFilter;

//Curvature Anisotropic Diffusion Image Filter
typedef itk::CurvatureAnisotropicDiffusionImageFilter
            <RealVolumeType,
                RealVolumeType>             CurvatureAnisotropicDiffusionFilterType;

//-- Interpolator types --
// Linear Interpolator
typedef itk::LinearInterpolateImageFunction<
                       FloatVolumeType, double >    LinearInterpolatorType;
//// Bspline interpolator
typedef itk::BSplineInterpolateImageFunction
                <FloatVolumeType>                   BSplineVolumeInterpolator;

// Windowed sinc interploation
typedef itk::Function::WelchWindowFunction<1>       WelchWindow;
typedef itk::ConstantBoundaryCondition<
                            FloatVolumeType>        ConstantBoundaryCondition;
typedef itk::WindowedSincInterpolateImageFunction<
                       FloatVolumeType, 1, WelchWindow,
                       ConstantBoundaryCondition,
                       double>                       WindowedSincInterpolator;

//Danielsson Distance Map
typedef
itk::SignedDanielssonDistanceMapImageFilter
        < FloatVolumeType, RealVolumeType >         DanielssonDistanceMapFilter;

// 4th order anisotropic diffusion
typedef itk::AnisotropicFourthOrderLevelSetImageFilter
    <RealVolumeType,RealVolumeType>             TasidzenDiffusion;


// connected component detection
typedef itk::ConnectedComponentImageFilter
        <UShortVolumeType, UShortVolumeType,
                          UShortVolumeType>     ConnectedComponentFilter;

//--- itk to vtk connections
typedef itk::VTKImageExport<FloatVolumeType>    VolumeExportType;

struct CallPacket
{
    FloatVolumeType::Pointer volume;
    char *output_file_name;
};

//--- registration types
typedef itk::TranslationTransform< double, 2 >      TranslationTransform;
typedef itk::AffineTransform< double, 2 >           AffineTransform;
typedef itk::CenteredRigid2DTransform<double>       CenteredRigid2DTransform;
typedef itk::Euler2DTransform<double>               Euler2DTransform;
typedef itk::Transform< double, 2, 2>               Transform;

typedef itk::RegularStepGradientDescentOptimizer    RSGDOptimizer;
typedef itk::ConjugateGradientOptimizer             CGDOptimizer;
typedef itk::AmoebaOptimizer                        AmoebaOptimizer;
typedef itk::SingleValuedNonLinearOptimizer         Optimizer;

typedef itk::ImageToImageMetric
        <FloatImageType, FloatImageType >           Metric;
typedef itk::MeanSquaresImageToImageMetric
        <FloatImageType, FloatImageType >           MSMetric;
typedef itk::MutualInformationImageToImageMetric
        <FloatImageType, FloatImageType >           VW_MIMetric;
typedef itk::MattesMutualInformationImageToImageMetric
        <FloatImageType, FloatImageType >           MattesMetric;
typedef itk::KullbackLeiblerCompareHistogramImageToImageMetric
        <FloatImageType, FloatImageType >           KLMetric;


typedef itk::ImageRegistrationMethod
        <FloatImageType, FloatImageType>            Registration;
typedef itk::ResampleImageFilter
                <FloatImageType,FloatImageType>     ResampleImageFilter;
typedef itk:: LinearInterpolateImageFunction
        <FloatImageType, double >                   LinearInterpolator;
typedef itk::BSplineInterpolateImageFunction
        <FloatImageType, double >                   BSplineInterpolator;

/**
 * This will be setup as a callback for a progress event on an ITK
 * filter.
 */
struct ProgressDisplay
{
  ProgressDisplay(itk::ProcessObject* process): m_Process(process),
                                                old_progress(0.0)
  {}


  void Display()
  {
    float progress = m_Process->GetProgress()*100.0;
    if( progress > old_progress + 5 ){
        std::cout<<" "<< progress << "%" ;
        old_progress = progress;
    }
  }

  itk::ProcessObject::Pointer m_Process;
  float old_progress;
};

/**
 * @class observe registration progress
 */

template <typename Optimizer>
class CommandIterationUpdate : public itk::Command
{
public:
    typedef CommandIterationUpdate Self;
    typedef itk::Command Superclass;
    typedef itk::SmartPointer<Self> Pointer;

    itkNewMacro( Self );        //New() method

protected:
    CommandIterationUpdate() {}; //constructor
    //the optimzer pointer
    typedef const Optimizer *OptimizerPointer;

    //the callback
    void Execute(itk::Object *caller, const itk::EventObject & event)
    {
        Execute( (const itk::Object *)caller, event);
    }

    //@param object: pointer to object that invoked callback
    //@param event: event that was invoked
    void Execute(const itk::Object * object, const itk::EventObject & event)
    {
        OptimizerPointer optimizer = dynamic_cast<OptimizerPointer>( object );

        //compare the type of the received event with an IterationEvent
        if( ! itk::IterationEvent().CheckEvent( &event ) ){
            std::cerr<<endl<<__FILE__<<__LINE__<<"::bad event received";
            return;
        }
        std::cerr<<".";
//      std::cerr<<endl<<optimizer->GetCurrentIteration()<< " = ";
//      std::cerr<<optimizer->GetValue()<< " : ";
//      std::cerr<<optimizer->GetCurrentPosition()<<endl;
    }

};

