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

// My headers
#include "itkFixedRotationSimilarity3DTransform.h"
#include "RegistrationMonitor.h"
#include "ImageWriters.h"

#include <queue>
#include <iostream>
#include <iomanip>

using namespace itk;


typedef short                             Pixel;
typedef float                             ProcessingPixel;
typedef OrientedImage<Pixel, 3>           FileImage;
typedef OrientedImage<ProcessingPixel, 3> ProcessingImage;
typedef ImageFileReader<FileImage>        Reader;

// Rotation Grid is how the optimized parameters 
typedef Image<FixedArray<float, 3>, 3>    RotationGrid;
typedef Image<float, 3>                   CostGrid;


RotationGrid::Pointer
createRotationGrid(double max, double resolution)
{
  RotationGrid::Pointer image = RotationGrid::New();

  RotationGrid::RegionType region;

  RotationGrid::IndexType start;
  RotationGrid::SizeType size;

  region.SetSize( size );
  region.SetIndex( start );
  
  for(unsigned int i = 0; i < 3; ++i)
    {
    start[i] = 0;
    size[i] = 2*floor(resolution / max) + 1;
    }

  image->SetRegions( region );
  image->Allocate();

  return image;
}

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

  const int numberoflevels = ceil(log(8.0/ fabs(freader->GetOutput()->GetSpacing()[2]))/log(2));
  std::cout << "nl: " << numberoflevels << std::endl;

  ImagePyramid::Pointer fpyramid = ImagePyramid::New();
  typedef ImagePyramid::ScheduleType Schedule;
  fpyramid->UseShrinkImageFilterOff();
  // hard code for now
  //fpyramid->SetSchedule(schedule);
  fpyramid->SetNumberOfLevels(numberoflevels);
  fpyramid->SetInput(freader->GetOutput());
  fpyramid->Update();

  ImagePyramid::Pointer mpyramid = ImagePyramid::New();
  typedef ImagePyramid::ScheduleType Schedule;
  mpyramid->UseShrinkImageFilterOff();
  // hard code for now
  mpyramid->SetNumberOfLevels(numberoflevels);
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

  writeimage(fpyramid->GetOutput(0), "tmp/dfixed.nrrd");
  writeimage(mpyramid->GetOutput(0), "tmp/dmoving.nrrd");

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

  RotationGenerator rg(M_PI/4, M_PI/16);
  //RotationGenerator rg(M_PI/6, M_PI/16);

  std::cout << "Built " << rg.size() << " trial rotations" << std::endl;

  std::ofstream file("tmp/moresearch.csv");
  if(!file)
    {
    std::cerr  << "cannot open csv file for writing" << std::endl;
    return EXIT_FAILURE;
    }
  file << "metric,i,# its,t0,t1,t2,s" << std::endl;

  std::vector<double> scales;

  int counter = 0;
  for(RotationGenerator::ConstIterator it = rg.begin();
      it != rg.end(); ++it)
    {
    const Versor<double> & rot = *it;
    
    typedef ImageRegistrationMethod<ProcessingImage, ProcessingImage> ImageRegistration;
    ImageRegistration::Pointer reg = ImageRegistration::New();
    reg->SetFixedImage(fpyramid->GetOutput(0));
    reg->SetFixedImageRegion(fpyramid->GetOutput(0)->GetLargestPossibleRegion());
    reg->SetMovingImage(mpyramid->GetOutput(0));

    typedef ImageRegistrationViewer ViewerCommandType;
    ViewerCommandType::Pointer command = ViewerCommandType::New();

    typedef LinearInterpolateImageFunction<ProcessingImage, double> Interpolator;
    Interpolator::Pointer reginterp = Interpolator::New();

    reg->SetInterpolator(reginterp);

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
//    metric->Initialize();

    reg->SetMetric(metric);

    //typedef OnePlusOneEvolutionaryOptimizer OptimizerType;
    //typedef PowellOptimizer OptimizerType;
    typedef FRPROptimizer OptimizerType;
    //typedef GradientDescentOptimizer OptimizerType;
    OptimizerType::Pointer opt = OptimizerType::New(); 
    typedef Optimizer::ScalesType       OptimizerScalesType;
    OptimizerScalesType optimizerScales( 4 );
    // should set this scale based on size of the image
    // optimizerScales[0] = 1.0/ 2500.0;
    // optimizerScales[1] = 1.0/ 2500.0;
    // optimizerScales[2] = 1.0/ 2500.0;
    // optimizerScales[3] = 1.0/ 1.0;
    optimizerScales[0] = 1.0/ 50.0;
    optimizerScales[1] = 1.0/ 50.0;
    optimizerScales[2] = 1.0/ 50.0;
    optimizerScales[3] = 1.0/ 1.0;

    opt->SetScales( optimizerScales );
    opt->SetMaximize(false);
    opt->SetCatchGetValueException( true );
    opt->SetMetricWorstPossibleValue(0.0);
    opt->AddObserver( IterationEvent(), command);

    // For 1 + 1
    //typedef itk::Statistics::NormalVariateGenerator Generator;
    //Generator::Pointer generator = Generator::New();
    //generator->Initialize(12345);

    // For gradient descent
    //opt->SetLearningRate(0.01);
    //opt->SetNumberOfIterations(40);

    // opt->SetNormalVariateGenerator(generator);
    // opt->Initialize(100.0);
    // opt->SetEpsilon(.001);

    // For powell
    // opt->SetStepLength(.5);
    // opt->SetStepTolerance(.0001);
    // opt->SetValueTolerance(.00001);
    // opt->SetMaximumIteration(30);
    // opt->SetMaximumLineIteration(10);
    //opt->SetUseUnitLengthGradient( true );
    //opt->UseGradientOff();

    // For FRPR
    opt->SetStepLength(200.0);
    opt->SetStepTolerance(.001);
    opt->SetValueTolerance(.0001);
    opt->SetMaximumIteration(100);
    opt->SetMaximumLineIteration(30);
    opt->SetUseUnitLengthGradient( true );
    opt->SetToPolakRibiere();

    reg->SetOptimizer(opt);

    // Execute a simplified rotation on the downsampled images
    Transform::Pointer ot = Transform::New();
    ot->SetIdentity();
    ot->SetRotation(rot);
    ot->SetCenter(initt->GetCenter());
    ot->SetTranslation(initt->GetTranslation());
    ot->SetParameters(initt->GetParameters());
    Transform::Pointer nt = Transform::New();
    nt->SetIdentity();
    nt->SetRotation(rot);
    nt->SetCenter(initt->GetCenter());
    nt->SetTranslation(initt->GetTranslation());
    nt->SetParameters(initt->GetParameters());

    //std::cout << "translation: " << nt->GetTranslation() << std::endl;
    
    reg->SetTransform(nt);
    reg->SetInitialTransformParameters( nt->GetParameters() );

    reg->StartRegistration();
    //std::cout << "params" << reg->GetInitialTransformParameters() << std::endl;

    nt->SetParameters( reg->GetLastTransformParameters() );

    std::cout << "Finished grid point " << counter << std::endl;
    // std::cout << "t: " << initt->GetTranslation() << std::endl;
    std::cout << nt->GetParameters() << std::endl;
    std::cout << "matrix: " << nt->GetMatrix() << std::endl;

    double nv = metric->GetValue(nt->GetParameters());

    file << opt->GetValue() << "," <<
      counter << "," <<
      opt->GetCurrentIteration() << "," <<
      nt->GetParameters()[0] << "," <<
      nt->GetParameters()[1] << "," <<
      nt->GetParameters()[2] << "," <<
      nt->GetParameters()[3] << std::endl;

    // std::cout << "Metric: " << opt->GetValue() << std::endl;
    // std::cout << "it: " << opt->GetCurrentIteration() << std::endl;
    // std::cout << "Rotation versor: " << rot << std::endl;
    // std::cout << "p: " << reg->GetLastTransformParameters() << std::endl;
    // std::cout << "scale: " << nt->GetScale() << std::endl;

    std::stringstream ss;
    ss << "tmp/pre" << std::setw(3) << std::setfill('0') << counter << ".nrrd";
    writeimage(mpyramid->GetOutput(0), ot, ss.str());
    ss.clear();
    ss.str("");
    ss << "tmp/post" << std::setw(3) << std::setfill('0') << counter << ".nrrd";
    writeimage(mpyramid->GetOutput(0), nt, ss.str());
    ++counter;
    }

  

  return EXIT_SUCCESS;
}

