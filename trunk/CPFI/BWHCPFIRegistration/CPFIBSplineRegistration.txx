#include "CommandIterationupdate.h"
#include "itkImageRegistrationMethod.h"
#include "itkMattesMutualInformationImageToImageMetric.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkBSplineDeformableTransform.h"

#include "itkOrientedImage.h"
#include "itkOrientImageFilter.h"
#include "itkResampleImageFilter.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkTransformFileReader.h"
#include "itkTransformFileWriter.h"
#include "itkExtractImageFilter.h"
#include "itkTileImageFilter.h"
#include "itkFlipImageFilter.h"

#include "itkPluginUtilities.h"

#include "itkTimeProbesCollectorBase.h"




namespace {


template<typename T, unsigned int N>
class CPFIBSplineRegistration
{

public:
  int DoIt( int argc, char * argv[])
  {
    PARSE_ARGS;

    // typedefs
    const unsigned int FileImageDimension = 3;
    const unsigned int ImageDimension = N;
    typedef  T  PixelType;
    typedef  T  OutputPixelType;
    typedef typename itk::OrientedImage< PixelType, FileImageDimension >   FileInputImageType;
    typedef typename itk::OrientedImage< PixelType, FileImageDimension >   FileOutputImageType;
    typedef typename itk::OrientedImage< PixelType, ImageDimension >       InputImageType;
    typedef typename itk::OrientedImage< OutputPixelType, ImageDimension > OutputImageType;

    typedef typename itk::ImageFileReader< FileInputImageType > FixedImageReaderType;
    typedef typename itk::ImageFileReader< FileInputImageType > MovingImageReaderType;
    typedef typename itk::ImageFileWriter< FileOutputImageType >  WriterType;

    typedef typename itk::OrientImageFilter<FileInputImageType,FileInputImageType> OrientFilterType;
    typedef typename itk::ExtractImageFilter<FileInputImageType, InputImageType>   InputExtractFilterType;

    typedef typename itk::ResampleImageFilter< 
                              InputImageType, 
                              OutputImageType >    ResampleFilterType;

    typedef typename itk::TileImageFilter<OutputImageType, FileOutputImageType> OutputPasteFilterType;
      
    const unsigned int SpaceDimension = ImageDimension;
    const unsigned int SplineOrder = 3;
    typedef double CoordinateRepType;
    typedef typename itk::ContinuousIndex<CoordinateRepType, ImageDimension> ContinuousIndexType;
  
    typedef typename itk::BSplineDeformableTransform<
                              CoordinateRepType,
                              SpaceDimension,
                              SplineOrder >     TransformType;
    typedef typename itk::AffineTransform<CoordinateRepType, ImageDimension> AffineTransformType;
    typedef typename itk::LBFGSBOptimizer       OptimizerType;
    typedef typename itk::MattesMutualInformationImageToImageMetric< 
                                      InputImageType, 
                                      InputImageType >    MetricType;
    typedef typename itk:: LinearInterpolateImageFunction< 
                                      InputImageType,
                                      double          >    InterpolatorType;
    typedef typename itk::ImageRegistrationMethod< 
                                      InputImageType, 
                                      OutputImageType >    RegistrationType;
  
    typename MetricType::Pointer         metric        = MetricType::New();
    typename OptimizerType::Pointer      optimizer     = OptimizerType::New();
    typename InterpolatorType::Pointer   interpolator  = InterpolatorType::New();
    typename TransformType::Pointer      transform     = TransformType::New();
    typename RegistrationType::Pointer   registration  = RegistrationType::New();
  
    typedef typename TransformType::RegionType RegionType;
    typedef typename TransformType::SpacingType SpacingType;
    typedef typename TransformType::OriginType OriginType;
    typedef typename TransformType::ParametersType     ParametersType;  
  
  
    // Read fixed and moving images
    //
    //
    typename FixedImageReaderType::Pointer  fixedImageReader  = FixedImageReaderType::New();
    typename MovingImageReaderType::Pointer movingImageReader = MovingImageReaderType::New();
  
    fixedImageReader->SetFileName(  FixedImageFileName.c_str() );
    movingImageReader->SetFileName( MovingImageFileName.c_str() );
  
    // If an initial transform was specified, read it
    //
    //
    typedef itk::TransformFileReader TransformReaderType;
    TransformReaderType::Pointer initialTransform;
  
    if (InitialTransform != "")
      {
      initialTransform= TransformReaderType::New();
      initialTransform->SetFileName( InitialTransform );
      try
        {
        initialTransform->Update();
        }
      catch (itk::ExceptionObject &err)
        {
        std::cerr << err << std::endl;
        return  EXIT_FAILURE;
        }
      }
  
    
    // Reorient to axials to avoid issues with registration metrics not
    // transforming image gradients with the image orientation in
    // calculating the derivative of metric wrt transformation
    // parameters.
    //
    // Forcing image to be axials avoids this problem. Note, that
    // reorientation only affects the internal mapping from index to
    // physical coordinates.  The reoriented data spans the same
    // physical space as the original data.  Thus, the registration
    // transform calculated on the reoriented data is also the
    // transform forthe original un-reoriented data. 
    //

    typename OrientFilterType::Pointer fixedOrient = OrientFilterType::New();
    typename OrientFilterType::Pointer movingOrient = OrientFilterType::New();
  
    fixedOrient->UseImageDirectionOn();
    fixedOrient->SetDesiredCoordinateOrientationToAxial();
    fixedOrient->SetInput (fixedImageReader->GetOutput());
  
    movingOrient->UseImageDirectionOn();
    movingOrient->SetDesiredCoordinateOrientationToAxial();
    movingOrient->SetInput (movingImageReader->GetOutput());

    // Add a time probe
    itk::TimeProbesCollectorBase collector;

    collector.Start( "Read fixed volume" );
    itk::PluginFilterWatcher watchOrientFixed(fixedOrient,
                                              "Orient Fixed Image",
                                              CLPProcessInformation,
                                              1.0/3.0, 0.0);
    fixedOrient->Update();
    collector.Stop( "Read fixed volume" );
  
    collector.Start( "Read moving volume" );
    itk::PluginFilterWatcher watchOrientMoving(movingOrient,
                                              "Orient Moving Image",
                                               CLPProcessInformation,
                                              1.0/3.0, 1.0/3.0);
    movingOrient->Update();
    collector.Stop( "Read moving volume" );
    
    typename InputImageType::Pointer fixedImage;
    typename InputImageType::Pointer movingImage;

    typename InputExtractFilterType::Pointer fixedExtract;
    typename InputExtractFilterType::Pointer movingExtract;

    // Extract filter to collapse image dimension (3D->2D in case of 2D registration)
    fixedExtract  = InputExtractFilterType::New();
    movingExtract = InputExtractFilterType::New();
    
    fixedExtract->SetInput(fixedOrient->GetOutput());
    movingExtract->SetInput(movingOrient->GetOutput());
    
    typename InputExtractFilterType::InputImageRegionType region
      = fixedOrient->GetOutput()->GetLargestPossibleRegion();

    int collapseDim = 0;
    
    if (ImageDimension < FileImageDimension)
      {
      typedef itk::Size<FileImageDimension> SizeType;
      SizeType size = region.GetSize();
      for (unsigned int i = 0; i < FileImageDimension; i ++)
        {
        if (size[i] == 1)
          {
          collapseDim = i;
          break;
          }
        }
      size[collapseDim] = 0; // collapse this dimension
      region.SetSize(size);
      }
    
    fixedExtract->SetExtractionRegion(region);
    movingExtract->SetExtractionRegion(region);
    fixedExtract->Update();
    movingExtract->Update();
    
    fixedImage  = fixedExtract->GetOutput();
    movingImage = movingExtract->GetOutput();

    //fixedImage  = fixedOrient->GetOutput();
    //movingImage = movingOrient->GetOutput();

  
    // Setup BSpline deformation
    //
    //  Note that the B-spline computation requires a finite support
    //  region ( 1 grid node at the lower borders and 2 grid nodes at
    //  upper borders).
    RegionType bsplineRegion;
    typename RegionType::SizeType   gridSizeOnImage;
    typename RegionType::SizeType   gridBorderSize;
    typename RegionType::SizeType   totalGridSize;
  
    gridSizeOnImage.Fill( gridSize );
    gridBorderSize.Fill( 3 );    // Border for spline order = 3 ( 1 lower, 2 upper )
    totalGridSize = gridSizeOnImage + gridBorderSize;
  
    bsplineRegion.SetSize( totalGridSize );
  
    SpacingType spacing = fixedImage->GetSpacing();
    OriginType origin   = fixedImage->GetOrigin();;


    typename InputImageType::RegionType fixedRegion =
      fixedImage->GetLargestPossibleRegion();
    
    typename InputImageType::SizeType fixedImageSize =
      fixedRegion.GetSize();
  
    for(unsigned int r=0; r<ImageDimension; r++)
      {
      spacing[r] *= floor( static_cast<double>(fixedImageSize[r] - 1)  / 
                    static_cast<double>(gridSizeOnImage[r] - 1) );
      origin[r]  -=  spacing[r]; 
      }
  

    std::cerr << "spacing = " << spacing << std::endl;

    transform->SetGridSpacing ( spacing );
    transform->SetGridOrigin  ( origin );
    transform->SetGridRegion  ( bsplineRegion );
  
    const unsigned int numberOfParameters =
                 transform->GetNumberOfParameters();
    
    ParametersType parameters( numberOfParameters );
    parameters.Fill( 0.0 );
  
    transform->SetParameters  ( parameters );
  
    // Initialize the transform with a bulk transform using either a
    // transform that aligns the centers of the volumes or a specified
    // bulk transform 
    //
    //
    typename TransformType::InputPointType centerFixed;
    typename InputImageType::RegionType::SizeType sizeFixed 
      = fixedImage->GetLargestPossibleRegion().GetSize();
    // Find the center
    ContinuousIndexType indexFixed;
    for ( unsigned j = 0; j < 3; j++ )
      {
      indexFixed[j] = (sizeFixed[j]-1) / 2.0;
      }
    fixedImage->TransformContinuousIndexToPhysicalPoint ( indexFixed, centerFixed );

  
    typename TransformType::InputPointType centerMoving;
    typename InputImageType::RegionType::SizeType sizeMoving = movingImage->GetLargestPossibleRegion().GetSize();

    // Find the center
    ContinuousIndexType indexMoving;
    for ( unsigned j = 0; j < 3; j++ )
      {
      indexMoving[j] = (sizeMoving[j]-1) / 2.0;
      }
    movingImage->TransformContinuousIndexToPhysicalPoint ( indexMoving, centerMoving );

    typename AffineTransformType::Pointer centeringTransform;
    centeringTransform = AffineTransformType::New();
  
    centeringTransform->SetIdentity();
    centeringTransform->SetCenter( centerFixed );
    centeringTransform->Translate(centerMoving-centerFixed);
    std::cout << "Centering transform: "; centeringTransform->Print( std::cout );
  
    transform->SetBulkTransform( centeringTransform );

  
    // If an initial transformation was provided, then use it instead.
    //
    if (InitialTransform != ""
        && initialTransform->GetTransformList()->size() != 0)
      {
      typename TransformReaderType::TransformType::Pointer initial;
      initial = *(initialTransform->GetTransformList()->begin());
  
      typedef typename TransformType::BulkTransformType BulkTransformType;
      
      //typename TransformType::BulkTransformType::Pointer bulk
      //  = dynamic_cast< TransformType::BulkTransformType* >(initial.GetPointer());
      typename BulkTransformType::Pointer bulk = dynamic_cast< BulkTransformType* >(initial.GetPointer());
  
      if (bulk)
        {
        transform->SetBulkTransform( bulk );
        }
      else
        {
        std::cout << "Initial transform is an unsupported type." << std::endl;
        }
      
      std::cout << "Initial transform: "; initial->Print ( std::cout );
      }
  
    
    // Setup optimizer
    //
    //
    typename OptimizerType::BoundSelectionType boundSelect( transform->GetNumberOfParameters() );
    typename OptimizerType::BoundValueType upperBound( transform->GetNumberOfParameters() );
    typename OptimizerType::BoundValueType lowerBound( transform->GetNumberOfParameters() );
    if (ConstrainDeformation)
      {
      boundSelect.Fill( 2 );
      upperBound.Fill(  MaximumDeformation );
      lowerBound.Fill( -MaximumDeformation );
      }
    else
      {
      boundSelect.Fill( 0 );
      upperBound.Fill( 0.0 );
      lowerBound.Fill( 0.0 );
      }
  
    optimizer->SetBoundSelection( boundSelect );
    optimizer->SetUpperBound    ( upperBound );
    optimizer->SetLowerBound    ( lowerBound );
  
    optimizer->SetCostFunctionConvergenceFactor ( 1e+1 );
    optimizer->SetProjectedGradientTolerance    ( 1e-7 );
    optimizer->SetMaximumNumberOfIterations     ( Iterations );
    optimizer->SetMaximumNumberOfEvaluations    ( 500 );
    optimizer->SetMaximumNumberOfCorrections    ( 12 );
  
    
    // Create the Command observer and register it with the optimizer.
    //
    typename CommandIterationUpdate::Pointer observer = CommandIterationUpdate::New();
    observer->SetRegistration ( registration );
    observer->SetCostFunction( metric );
  
    optimizer->AddObserver( itk::IterationEvent(), observer );
  
    // Setup metric
    //
    //
    metric->ReinitializeSeed( 76926294 );
    metric->SetNumberOfHistogramBins( HistogramBins );
    metric->SetNumberOfSpatialSamples( SpatialSamples );
  
    std::cout << std::endl << "Starting Registration" << std::endl;
  
    // Registration
    //
    //
    registration->SetFixedImage  ( fixedImage  );
    registration->SetMovingImage ( movingImage );
    registration->SetMetric      ( metric       );
    registration->SetOptimizer   ( optimizer    );
    registration->SetInterpolator( interpolator );
    registration->SetTransform   ( transform    );
    registration->SetInitialTransformParameters( transform->GetParameters() );
  
    try 
      { 
      itk::PluginFilterWatcher watchRegistration(registration,
                                                 "Registering",
                                                 CLPProcessInformation,
                                                 1.0/3.0, 2.0/3.0);
      collector.Start( "Registration" );
      registration->Update();
      collector.Stop( "Registration" );
      } 
    catch( itk::ExceptionObject & err ) 
      { 
      std::cerr << "ExceptionObject caught !" << std::endl; 
      std::cerr << err << std::endl; 
      return EXIT_FAILURE;
      } 
    
    typename OptimizerType::ParametersType finalParameters = 
      registration->GetLastTransformParameters();
    std::cout << "Final parameters: " << finalParameters[50] << std::endl;
    transform->SetParameters      ( finalParameters );
  
    if (OutputTransform != "")
      {
      typedef itk::TransformFileWriter TransformWriterType;
      TransformWriterType::Pointer outputTransformWriter;
  
      outputTransformWriter= TransformWriterType::New();
      outputTransformWriter->SetFileName( OutputTransform );
      outputTransformWriter->SetInput( transform );
      outputTransformWriter->AddTransform( transform->GetBulkTransform() );
      try
        {
        outputTransformWriter->Update();
        }
      catch (itk::ExceptionObject &err)
        {
        std::cerr << err << std::endl;
        return EXIT_FAILURE ;
        }
      }
  
    // Resample to the original coordinate frame (not the reoriented
    // axial coordinate frame) of the fixed image
    //
    if (ResampledImageFileName != "")
      {
      typename ResampleFilterType::Pointer resample = ResampleFilterType::New();
      
      itk::PluginFilterWatcher watcher(
        resample,
        "Resample",
        CLPProcessInformation,
        1.0/3.0, 2.0/3.0);
      
      resample->SetTransform( transform );
      resample->SetInput( movingImage );
      resample->SetDefaultPixelValue( DefaultPixelValue );
      resample->SetOutputParametersFromImage ( fixedImage );
      
      collector.Start( "Resample" );
      resample->Update();
      collector.Stop( "Resample" );

      // convert 2D image to 3D, if 2D registration is performed
      typename OutputPasteFilterType::Pointer outputPaste = OutputPasteFilterType::New();
      typename OutputPasteFilterType::LayoutArrayType layout;
      if (ImageDimension == 2)
        {
        layout[0] = 1;
        layout[1] = 1;
        layout[2] = 0;
        }
      else // 3D
        {
        layout[0] = 1;
        layout[1] = 1;
        layout[2] = 1;
        }
      outputPaste->SetLayout(layout);
      outputPaste->SetInput(resample->GetOutput());
      outputPaste->Update();

      typename WriterType::Pointer      writer =  WriterType::New();

      if (ImageDimension == 2)
        {
        // Flip axis
        typedef typename itk::FlipImageFilter< FileOutputImageType > FlipFilterType;
        typename FlipFilterType::Pointer outputFlip = FlipFilterType::New();
        typename FlipFilterType::FlipAxesArrayType flipAxes;
        flipAxes[0] = 0; flipAxes[1] = 1; flipAxes[2] = 0; // is this OK??????
        outputFlip->SetFlipAxes(flipAxes);
        outputFlip->SetInput(outputPaste->GetOutput());
        outputFlip->Update();
        
        
        // Set origin and direction
        typename FileOutputImageType::Pointer outputImage = FileOutputImageType::New();
        outputImage = outputFlip->GetOutput();
        outputImage->Update();
        
        outputImage->SetOrigin(fixedImageReader->GetOutput()->GetOrigin());
        typename FileOutputImageType::DirectionType direction;
        direction = fixedImageReader->GetOutput()->GetDirection();
        outputImage->SetDirection(direction);
        outputImage->Update();
        
        writer->SetFileName( ResampledImageFileName.c_str() );
        writer->SetInput( outputImage   );
        }
      else
        {
        writer->SetFileName( ResampledImageFileName.c_str() );
        writer->SetInput( outputPaste->GetOutput()   );
        }
  
      try
        {
        collector.Start( "Write resampled volume" );
        writer->Update();
        collector.Stop( "Write resampled volume" );
        }
      catch( itk::ExceptionObject & err ) 
        { 
        std::cerr << "ExceptionObject caught !" << std::endl; 
        std::cerr << err << std::endl; 
        return EXIT_FAILURE;
        }
      }
  
    // Report the time taken by the registration
    collector.Report();
  
    return EXIT_SUCCESS;
  }
};


}




