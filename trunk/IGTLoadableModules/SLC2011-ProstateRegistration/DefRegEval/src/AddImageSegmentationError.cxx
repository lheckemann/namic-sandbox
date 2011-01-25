// disable warnings for sprintf
#define _CRT_SECURE_NO_WARNINGS
// disable warnings for std::copy
#define _SCL_SECURE_NO_WARNINGS

#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include "DefRegEval.h"

typedef itk::Image <unsigned char, 3> OutputImageType;

#include "itkResampleImageFilter.h"
#include "itkLinearInterpolateImageFunction.h"

#include "itkBSplineDeformableTransform.h"

#include <fstream>

//  The following section of code implements a Command observer
//  used to monitor the evolution of the registration process.
//
#include "itkCommand.h"
class CommandProgressUpdate : public itk::Command 
{
public:
  typedef  CommandProgressUpdate   Self;
  typedef  itk::Command             Superclass;
  typedef itk::SmartPointer<Self>  Pointer;
  itkNewMacro( Self );
protected:
  CommandProgressUpdate() {};
public:
  void Execute(itk::Object *caller, const itk::EventObject & event)
  {
    Execute( (const itk::Object *)caller, event);
  }

  void Execute(const itk::Object * object, const itk::EventObject & event)
  {
    const itk::ProcessObject * filter = 
      dynamic_cast< const itk::ProcessObject * >( object );
    if( ! itk::ProgressEvent().CheckEvent( &event ) )
    {
      return;
    }
    std::cout << filter->GetProgress() << std::endl;
  }
};


int main( int argc, char * argv[] )
{

  std::string inputFilename = "input.mha";
  std::string outputFilename = "output.mha";
  std::string outputDeformationFieldFilename;

  std::vector<double> deformationPosition; // {67, 58, 30} LPS
  std::vector<double> deformationVector; // {0, -5, 0}
  std::vector<double> deformationRegionSize; // {20, 20, 20}

  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);

  args.AddArgument("--input", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputFilename, "Source image");
  args.AddArgument("--output", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputFilename, "Deformed image");

  args.AddArgument("--outputDeformationField", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputDeformationFieldFilename, "Deformed image");

  args.AddArgument("--deformationPosition", vtksys::CommandLineArguments::MULTI_ARGUMENT, &deformationPosition, "Position of the deformed point"); 
  args.AddArgument("--deformationVector", vtksys::CommandLineArguments::MULTI_ARGUMENT, &deformationVector, "Deformation vector"); 
  args.AddArgument("--deformationRegionSize", vtksys::CommandLineArguments::MULTI_ARGUMENT, &deformationRegionSize, "Spacing of the deformation grid (region size of the deformation). Deformation will be 0 if distance from the deformed point is >2*region size"); 

  if ( !args.Parse() )
  {
    std::cerr << "Problem parsing arguments" << std::endl;
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }

  if (deformationPosition.size()!=3)
  {
    std::cerr << "Invalid deformationPosition" << std::endl;
    exit(EXIT_FAILURE);
  }
  if (deformationVector.size()!=3)
  {
    std::cerr << "Invalid deformationVector" << std::endl;
    exit(EXIT_FAILURE);
  }
  if (deformationRegionSize.size()!=3)
  {
    std::cerr << "Invalid deformationRegionSize" << std::endl;
    exit(EXIT_FAILURE);
  }

  const     unsigned int   ImageDimension = 3;

  ExternalImageReaderType::Pointer fixedReader = ExternalImageReaderType::New();
  fixedReader->SetFileName( inputFilename );

  try
  {
    fixedReader->Update();
  }
  catch( itk::ExceptionObject & excp )
  {
    std::cerr << "Exception thrown " << std::endl;
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
  }


  typedef itk::ResampleImageFilter< ExternalImageType, ExternalImageType  >  FilterType;
  FilterType::Pointer resampler = FilterType::New();

  typedef itk::LinearInterpolateImageFunction<ExternalImageType, double >  InterpolatorType;
  InterpolatorType::Pointer interpolator = InterpolatorType::New();

  resampler->SetInterpolator( interpolator );

  ExternalImageType::ConstPointer fixedImage = fixedReader->GetOutput();
  ExternalImageType::SpacingType   fixedSpacing    = fixedImage->GetSpacing();
  ExternalImageType::PointType     fixedOrigin     = fixedImage->GetOrigin();
  ExternalImageType::DirectionType fixedDirection  = fixedImage->GetDirection();

  resampler->SetOutputSpacing( fixedSpacing );
  resampler->SetOutputOrigin(  fixedOrigin  );
  resampler->SetOutputDirection(  fixedDirection  );


  ExternalImageType::RegionType fixedRegion = fixedImage->GetBufferedRegion();
  ExternalImageType::SizeType   fixedSize =  fixedRegion.GetSize();
  resampler->SetSize( fixedSize );
  resampler->SetOutputStartIndex(  fixedRegion.GetIndex() );


  resampler->SetInput( fixedReader->GetOutput() );  

  const unsigned int SpaceDimension = ImageDimension;
  const unsigned int SplineOrder = 3;
  const int SplineSupportRegionSize=SplineOrder-1;
  typedef double CoordinateRepType;

  typedef itk::BSplineDeformableTransform<CoordinateRepType,SpaceDimension,SplineOrder >     TransformType;  

  TransformType::Pointer bsplineTransform = TransformType::New();

  typedef TransformType::RegionType RegionType;
  RegionType bsplineRegion;
  RegionType::SizeType   gridSize; // number of grid nodes
  TransformType::SpacingType gridSpacing; // grid spacing in mm
  TransformType::OriginType gridOrigin; // in LPS coordinate space

  itk::Vector< int, ImageDimension >  deformationPositionNodeIndices;

  {
    typedef double NumericType;
    typedef itk::Matrix<NumericType,SpaceDimension+1,SpaceDimension+1>          MatrixType;
    typedef itk::Vector<NumericType,SpaceDimension+1>            VectorType;
    typedef itk::Point<NumericType,SpaceDimension+1>             PointType;

    // Create a matrix that transforms the image to the world origin and align
    // the axes with world xyz (LPS)
    MatrixType imageToLps;    
    imageToLps.SetIdentity();
    for (int row=0; row<SpaceDimension; row++)
    {
      for (int col=0; col<SpaceDimension; col++)
      {
        imageToLps(row,col)=fixedDirection[row][col];
      }
      imageToLps(row,SpaceDimension)=fixedOrigin[row];      
    }
    MatrixType lpsToImage;
    lpsToImage=imageToLps.GetInverse();

    PointType deformationPosition_LPS;
    deformationPosition_LPS[SpaceDimension]=1; // it's a point
    for (int row=0; row<SpaceDimension; row++)
    {
      deformationPosition_LPS[row]=deformationPosition[row];
    }    
    PointType deformationPosition_Image;
    deformationPosition_Image=lpsToImage*deformationPosition_LPS;

    int numberOfGridNodesToTheLeft=0;
    int numberOfGridNodesToTheRight=0;
    const int numberOfGridNodesOutsideTheImageSupportLeft=SplineSupportRegionSize;
    const int numberOfGridNodesOutsideTheImageSupportRight=SplineSupportRegionSize;
    PointType gridOrigin_Image;
    gridOrigin_Image[SpaceDimension]=1; // it's a point
    for (int row=0; row<SpaceDimension; row++)
    {
      double imageLeft=0;
      double imageRight=fixedSize[row]*fixedSpacing[row];
      gridSpacing[row]=deformationRegionSize[row];
      numberOfGridNodesToTheLeft=floor(fabs(deformationPosition_Image[row]-imageLeft)/gridSpacing[row]) + numberOfGridNodesOutsideTheImageSupportLeft;
      numberOfGridNodesToTheRight=floor(fabs(deformationPosition_Image[row]-imageRight)/gridSpacing[row]) + numberOfGridNodesOutsideTheImageSupportRight;
      gridOrigin_Image[row]=deformationPosition_Image[row]-numberOfGridNodesToTheLeft*gridSpacing[row];      
      gridSize[row]=numberOfGridNodesToTheLeft+1+numberOfGridNodesToTheRight;
      deformationPositionNodeIndices[row]=numberOfGridNodesToTheLeft;
    }

    PointType gridOrigin_LPS;
    gridOrigin_LPS=imageToLps*gridOrigin_Image;
    for (int row=0; row<SpaceDimension; row++)
    {
      gridOrigin[row]=gridOrigin_LPS[row];
    }  

  }

  bsplineRegion.SetSize( gridSize );
  bsplineTransform->SetGridSpacing( gridSpacing );
  bsplineTransform->SetGridOrigin( gridOrigin );
  bsplineTransform->SetGridRegion( bsplineRegion );
  bsplineTransform->SetGridDirection( fixedDirection );


  typedef TransformType::ParametersType     ParametersType;

  const unsigned int numberOfParameters = bsplineTransform->GetNumberOfParameters();
  const unsigned int numberOfNodes = numberOfParameters / SpaceDimension;

  ParametersType parameters( numberOfParameters );

  // Need to multiply the weight so that the specified point has the required value
  // (alternatively, additional points could be added with the same coordinate location, but that is not possible
  // because we use a grid with equal spacing)
  const double weightMultiplier=1.0/0.29622130393981934;

  // parameter indices
  int xParamInd=0;
  int yParamInd=0;
  int zParamInd=0;
  int deformationNodeIndex=
    (((deformationPositionNodeIndices[2]*gridSize[1])+
    deformationPositionNodeIndices[1])*gridSize[0])+
    deformationPositionNodeIndices[0];
  for( unsigned int n=0; n < numberOfNodes; n++ )
  {
    xParamInd=numberOfNodes*0+n;
    yParamInd=numberOfNodes*1+n;
    zParamInd=numberOfNodes*2+n;
    if (n==deformationNodeIndex)
    {
      parameters[xParamInd]=deformationVector[0]*weightMultiplier;
      parameters[yParamInd]=deformationVector[1]*weightMultiplier;
      parameters[zParamInd]=deformationVector[2]*weightMultiplier;
    }
    else
    {
      parameters[xParamInd]=0;
      parameters[yParamInd]=0;
      parameters[zParamInd]=0;
    }
  } 

  bsplineTransform->SetParameters( parameters );

  CommandProgressUpdate::Pointer observer = CommandProgressUpdate::New();

  resampler->AddObserver( itk::ProgressEvent(), observer );
  resampler->SetTransform( bsplineTransform );

  ExternalImageWriterType::Pointer deformedWriter = ExternalImageWriterType::New();
  deformedWriter->SetFileName( outputFilename );
  deformedWriter->SetInput( resampler->GetOutput() );
  try
  {
    deformedWriter->Update();
  }
  catch( itk::ExceptionObject & excp )
  {
    std::cerr << "Exception thrown " << std::endl;
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
  }


  if( !outputDeformationFieldFilename.empty() )
  {
    typedef itk::Point<  float, ImageDimension >  PointType;
    typedef itk::Vector< float, ImageDimension >  VectorType;
    typedef itk::Image< VectorType, ImageDimension >  DeformationFieldType;

    DeformationFieldType::Pointer field = DeformationFieldType::New();
    field->SetRegions( fixedRegion );
    field->SetOrigin( fixedOrigin );
    field->SetSpacing( fixedSpacing );
    field->SetDirection( fixedDirection );
    field->Allocate();

    typedef itk::ImageRegionIterator< DeformationFieldType > FieldIterator;
    FieldIterator fi( field, fixedRegion );

    fi.GoToBegin();

    TransformType::InputPointType  fixedPoint;
    TransformType::OutputPointType movingPoint;
    DeformationFieldType::IndexType index;

    VectorType displacement;

    while( ! fi.IsAtEnd() )
    {
      index = fi.GetIndex();
      field->TransformIndexToPhysicalPoint( index, fixedPoint );
      movingPoint = bsplineTransform->TransformPoint( fixedPoint );
      displacement = movingPoint - fixedPoint;
      fi.Set( displacement );
      ++fi;
    }

    typedef itk::ImageFileWriter< DeformationFieldType >  FieldWriterType;
    FieldWriterType::Pointer fieldWriter = FieldWriterType::New();

    fieldWriter->SetInput( field );

    fieldWriter->SetFileName( outputDeformationFieldFilename );
    try
    {
      fieldWriter->Update();
    }
    catch( itk::ExceptionObject & excp )
    {
      std::cerr << "Exception thrown " << std::endl;
      std::cerr << excp << std::endl;
      return EXIT_FAILURE;
    }
  }

  return EXIT_SUCCESS;
}
