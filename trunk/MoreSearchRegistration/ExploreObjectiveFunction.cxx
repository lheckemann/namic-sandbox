#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#ifdef __BORLANDC__
#define ITK_LEAN_AND_MEAN
#endif

#include "itkOrientedImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkFRPROptimizer.h"
#include "itkOnePlusOneEvolutionaryOptimizer.h"
#include "itkGradientDescentOptimizer.h"
#include "itkPowellOptimizer.h"
#include "itkNormalVariateGenerator.h"
#include "itkCenteredTransformInitializer.h"
#include "itkResampleImageFilter.h"
#include "itkVersor.h"
#include "itkRecursiveMultiResolutionPyramidImageFilter.h"
#include "itkMattesMutualInformationImageToImageMetric.h"
#include "itkNormalizedMutualInformationHistogramImageToImageMetric.h"
#include "itkNormalizedCorrelationImageToImageMetric.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkImageRegistrationMethod.h"
#include "itkResampleImageFilter.h"

#include "itkRealTimeClock.h"


#include "itkFixedRotationSimilarity3DTransform.h"

#include <queue>
#include <iostream>
#include <iomanip>

using namespace itk;


typedef  short Pixel;
typedef float          ProcessingPixel;
typedef OrientedImage<Pixel, 3> FileImage;
typedef OrientedImage<ProcessingPixel, 3> ProcessingImage;
typedef ImageFileReader<FileImage> Reader;

template< typename T>
void writeimage(const T& image,
                const std::string & filename)
{
    // Write out test image
    typename ImageFileWriter<ProcessingImage>::Pointer writer = ImageFileWriter<ProcessingImage>::New();
    writer->SetFileName(filename);
    writer->SetInput(image);
    writer->Update();
}

template< typename T, typename J>
void writeimage(const T& image,
                const J& transform, 
                const std::string & filename)
{
    typedef ResampleImageFilter<ProcessingImage, FileImage> ResampleImageFilter;
    typename ResampleImageFilter::Pointer resampler = ResampleImageFilter::New();
    // Resample the moving image
    resampler->SetInput(image);
    resampler->SetOutputParametersFromImage(image);
    resampler->SetTransform(transform);
    resampler->Update();

    // Write out test image
    typename ImageFileWriter<FileImage>::Pointer writer = ImageFileWriter<FileImage>::New();
    writer->SetFileName(filename);
    writer->SetInput(resampler->GetOutput());
    writer->Update();
}

class ImageRegistrationViewer
: public Command
{
  public :
    typedef ImageRegistrationViewer     Self;
    typedef Command                Superclass;
    typedef SmartPointer<Self>     Pointer;
  
    itkTypeMacro( ImageRegistrationViewer, Command );

    itkNewMacro( ImageRegistrationViewer );
  
    typedef SingleValuedNonLinearOptimizer  OptimizerType;
  
    itkSetMacro(DontShowParameters, bool);
    itkSetMacro(UpdateInterval, int);
  
    void Execute( Object * caller, const EventObject & event )
      {
      Execute( (const Object *)caller, event );
      }

    void Execute( const Object * object, const EventObject & event )
      {
      if( typeid( event ) != typeid( IterationEvent ) || object == NULL)
        {
        return;
        }
  
      const OptimizerType * opt = dynamic_cast<const OptimizerType *>(object);
  
      if(++m_Iteration % m_UpdateInterval == 0)
        {
        RealTimeClock::TimeStampType t = m_Clock->GetTimeStamp();
        if(!m_DontShowParameters)
          {
          std::cout << "   " << m_Iteration << " : " 
                    << opt->GetCurrentPosition() << " = "
                    << opt->GetValue( opt->GetCurrentPosition() )
                    << "   (" << (t - m_LastTime)/m_UpdateInterval << "s)"
                    << std::endl;
          }
        else
          {
          std::cout << "   " << m_Iteration << " : " 
                    << opt->GetValue( opt->GetCurrentPosition() )
                    << "   (" << (t - m_LastTime)/m_UpdateInterval << "s)"
                    << std::endl;
          }
        m_LastTime = t;
        }
      }

    void Update()
      {
      this->Execute ( (const Object *)NULL, IterationEvent() );
      }

  protected:

    RealTimeClock::Pointer       m_Clock;
    RealTimeClock::TimeStampType m_LastTime;

    int  m_Iteration;
    int  m_UpdateInterval;
    bool m_DontShowParameters;

    ImageRegistrationViewer() 
      { 
      m_Clock = RealTimeClock::New();
      m_LastTime = m_Clock->GetTimeStamp();
      m_Iteration = 0;
      m_UpdateInterval = 1;
      m_DontShowParameters = false; 
      };
    ~ImageRegistrationViewer() {};

};

class RotationGenerator
{
public:
  typedef Versor<double> Rotation;
private:
  typedef std::list<Versor<double> > Container;
public:
  typedef Container::iterator Iterator;
  typedef Container::const_iterator ConstIterator;
      
  explicit RotationGenerator(double max, double resolution)
  {
    Versor<double> identity;
    identity.SetIdentity();
    rotations.push_back(identity);

    for(double a = 0; a < max; a += resolution)
      {
      Versor<double> posi;
      Versor<double> negi;

      posi.SetRotationAroundX(a);
      negi.SetRotationAroundX(-a);
      
      for(double b = 0; b < max; b += resolution)
        {
        Versor<double> posj;
        Versor<double> negj;

        posj.SetRotationAroundY(b);
        negj.SetRotationAroundY(-b);

        for(double c = 0; c < max; c += resolution)
          {
          Versor<double> posk;
          Versor<double> negk;

          posk.SetRotationAroundZ(c);
          negk.SetRotationAroundZ(-c);

          Versor<double> pos = posk * posj * posi;
          Versor<double> neg = negk * negj * negi;
          
          if( !(a == 0 && b == 0 && c == 0) )
            {
            rotations.push_back(pos);
            rotations.push_back(neg);
            }
          }
        }
      }
  }

  virtual ~RotationGenerator()
  {
  }

  Iterator begin() { return this->rotations.begin(); }
  Iterator end() { return this->rotations.end(); }
  ConstIterator begin() const { return this->rotations.begin(); }
  ConstIterator end() const { return this->rotations.end(); }
  
  size_t size() const { return this->rotations.size(); }

private:
  std::list<Versor<double> > rotations;
};

int main( int argc, char * argv[] )
{
  // Read in image
  if(argc != 3)
    {
    std::cerr << "Usage: " << argv[0] << " <fixed image> <moving image>" << std::endl;
    return EXIT_FAILURE;
    }
  

  Reader::Pointer freader = Reader::New();
  Reader::Pointer mreader = Reader::New();
//  Reader::Pointer maskreader = Reader::New();

  freader->SetFileName(argv[1]);
  //maskreader->SetFileName(argv[2]);
  mreader->SetFileName(argv[2]);

  freader->Update();
  //maskreader->Update();
  mreader->Update();

  // Resample both images to 8x8x8
  typedef RecursiveMultiResolutionPyramidImageFilter<FileImage, ProcessingImage> ImagePyramid;

  // const unsigned int mr = 4;
  // const unsigned int Dimension = 3;

  // Array2D<unsigned int> schedule(mr, Dimension);
  // for(unsigned int r = 0; r < mr; ++r)
  //   {
  //   for(unsigned int d = 0; d < Dimension; ++d)
  //     {
  //     schedule[r][d] = pow(2, mr - 1 - r);
  //     }
  //   }
  
  // std::cout << schedule << std::endl;

  ImagePyramid::Pointer fpyramid = ImagePyramid::New();
  typedef ImagePyramid::ScheduleType Schedule;
  fpyramid->UseShrinkImageFilterOff();
  // hard code for now
  //fpyramid->SetSchedule(schedule);
  fpyramid->SetNumberOfLevels(4);
  fpyramid->SetInput(freader->GetOutput());
  fpyramid->Update();

  ImagePyramid::Pointer mpyramid = ImagePyramid::New();
  typedef ImagePyramid::ScheduleType Schedule;
  mpyramid->UseShrinkImageFilterOff();
  // hard code for now
  mpyramid->SetNumberOfLevels(4);
  mpyramid->SetInput(mreader->GetOutput());
  mpyramid->Update();

  std::cout << "schedule: " << fpyramid->GetSchedule() << std::endl;

  // 0 is the downsampled image
  std::cout << "pyramid[0]: " << fpyramid->GetOutput(0)->GetLargestPossibleRegion().GetSize() << std::endl;
  std::cout << "pyramid[1]: " << fpyramid->GetOutput(1)->GetLargestPossibleRegion().GetSize() << std::endl;
  
  typedef FixedRotationSimilarity3DTransform<double> Transform;
  typedef CenteredTransformInitializer< Transform,
    ProcessingImage,
    ProcessingImage>   TransformInitializer;

  Transform::Pointer initt = Transform::New();
  initt->SetIdentity();

  TransformInitializer::Pointer tinit = TransformInitializer::New();
  tinit->SetTransform(initt);
  tinit->SetFixedImage(fpyramid->GetOutput(0));
  tinit->SetMovingImage(mpyramid->GetOutput(0));
  tinit->MomentsOn();
  //tinit->GeometryOn();

  writeimage(fpyramid->GetOutput(0), "dfixed.nrrd");
  writeimage(mpyramid->GetOutput(0), "dmoving.nrrd");

  try
    {
    tinit->InitializeTransform();
    }
  catch( itk::ExceptionObject e)
    {
    std::cerr << "Error initalizing transform" << std::endl;
    return EXIT_FAILURE;
    }

  std::cout << "Initialized transform: " << std::endl;
  //std::cout << initt << std::endl;

  // Generate a set of rotations
  // Parameter is n where number of samples per dimensions is 2*n+1
  // We have (2*n+1)^3 rotaions.  So this number must be very very small.
  // Setup registration framework
  
  // std::priority_queue<std::pair<float, int>,
  //   std::vector<std::pair<float, int> >,
  //   std::greater<std::pair<float, int> > > pq;
  //RotationGenerator rg(M_PI/6, M_PI/16);

  std::ofstream file("objfunction.csv");
  if(!file)
    {
    std::cerr  << "cannot open csv file for writing" << std::endl;
    return EXIT_FAILURE;
    }

  typedef MattesMutualInformationImageToImageMetric<ProcessingImage, ProcessingImage> Metric;
//   typedef NormalizedMutualInformationHistogramImageToImageMetric<ProcessingImage, ProcessingImage> Metric;
//    typedef NormalizedCorrelationImageToImageMetric<ProcessingImage, ProcessingImage> Metric;
  Metric::Pointer metric = Metric::New();
  metric->SetNumberOfHistogramBins(256/8);
  //    Metric::HistogramSizeType hsize;
  //    hsize[0] = 256/8;
  //    hsize[1] = 256/8;
  //    metric->SetHistogramSize(hsize);
  //Contains only 16384 pixels total   
  //metric->SetUseAllPixels(true);
  // metric->SetFixedImageMask(maskreader->GetOutput());
  metric->SetNumberOfThreads(4);
  metric->SetNumberOfSpatialSamples(15000);
  metric->SetFixedImageSamplesIntensityThreshold(50);
  metric->SetUseExplicitPDFDerivatives(true);

  // Execute a simplified rotation on the downsampled images
  Transform::Pointer t = Transform::New();
  t->SetIdentity();
  t->SetCenter(initt->GetCenter());
  t->SetTranslation(initt->GetTranslation());
  //std::cout << "translation: " << nt->GetTranslation() << std::endl;
  Transform::ParametersType p(4);
  p[0] = 0;
  p[1] = 0;
  p[2] = 0;
  p[3] = 1.0;
  t->SetParameters(p);

  typedef LinearInterpolateImageFunction<ProcessingImage, double> Interpolator;
  Interpolator::Pointer reginterp = Interpolator::New();

  metric->SetTransform(t);
  metric->SetInterpolator(reginterp);

  metric->SetFixedImage(fpyramid->GetOutput(0));
  metric->SetFixedImageRegion(fpyramid->GetOutput(0)->GetLargestPossibleRegion());
  metric->SetMovingImage(mpyramid->GetOutput(0));

  metric->Initialize();

  double m = 0.0;
  itk::Array<double> g(4);
  g.Fill(0.0);
  for(double x = -10; x < 12.01; x += 1)
    {
    for(double y = -15; y < 5.01; y += 1)
      {
      for(double z = -13; z < 1.01; z += 1.0)
        {
        for(double s = .8; s < 1.201; s += .05)
          {
          p[0] = x;
          p[1] = y;
          p[2] = z;
          p[3] = s;
          metric->GetValueAndDerivative(p, m, g );
          
          file << x << ", " 
               << y << ", " 
               << z << ", "
               << s << ", "
               << g[0] << ", " 
               << g[1] << ", " 
               << g[2] << ", "
               << g[3] << ", "
               << m << std::endl;
          }
        }
      }
    }
  

  // std::cout << "t: " << initt->GetTranslation() << std::endl;
  
  // std::cout << "Metric: " << opt->GetValue() << std::endl;
  // std::cout << "it: " << opt->GetCurrentIteration() << std::endl;
  // std::cout << "Rotation versor: " << rot << std::endl;
  // std::cout << "p: " << reg->GetLastTransformParameters() << std::endl;
  // std::cout << "scale: " << nt->GetScale() << std::endl;

  return EXIT_SUCCESS;
}

