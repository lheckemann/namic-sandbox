/** simplified pipeline for 2-slice fmri registration
 **/ 

#include "includes.h"


const int GRID_STEP_X = 8;
const int GRID_STEP_Y = 8;

int main(int argc, char *argv[])
{
  std::cout<<"Build date "<<__DATE__<<" and time "<<__TIME__<<endl<<endl;

  if( 1+3 != argc  ){
     std::cerr<<endl<<"Insufficient number of arguments" 
              <<endl<<argv[0]
              <<endl<<"   <fixed_img> "<<"<moving_img>"
          <<"   <output_img>";
        return EXIT_FAILURE;
    }
  
  typedef itk::ResampleImageFilter
                <FloatImageType,FloatImageType>   ImageResampleFilter;
  typedef itk::NormalizeImageFilter
                <FloatImageType, FloatImageType>  ImageNormalizeFilter;
  typedef itk::RescaleIntensityImageFilter
                <FloatImageType, FloatImageType>  ImageRescaleIntensityFilter;
  typedef itk::BSplineInterpolateImageFunction
                <FloatImageType, double>          ImageBSplineInterpolator;
    typedef itk::IdentityTransform<double, 2> IdentityTransform;
    typedef FloatImageType::SizeType SizeType;
    typedef SizeType::SizeValueType   SizeValueType;


try{
  // read in the image file
  ImageReaderType::Pointer fixed_img_reader = ImageReaderType::New();
  ImageReaderType::Pointer moving_img_reader = ImageReaderType::New();

  fixed_img_reader->SetFileName(  argv[1] );
  moving_img_reader->SetFileName( argv[2] );

  UShortToFloatImageCast::Pointer fixed_us_2_flt_cast 
                = UShortToFloatImageCast::New();
  UShortToFloatImageCast::Pointer moving_us_2_flt_cast
                = UShortToFloatImageCast::New() ;

    ImageWriterType::Pointer image_writer = ImageWriterType::New();
    FloatToUShortImageCast::Pointer flt_2_us_cast = FloatToUShortImageCast::New();

  fixed_us_2_flt_cast->SetInput(fixed_img_reader->GetOutput());
  moving_us_2_flt_cast->SetInput(moving_img_reader->GetOutput());

  fixed_us_2_flt_cast->Update();
  moving_us_2_flt_cast->Update();

  FloatImageType::Pointer fixed_img = fixed_us_2_flt_cast->GetOutput();
  FloatImageType::Pointer moving_img = moving_us_2_flt_cast->GetOutput();


  // -- put optional code to do resampling here --
    ImageBSplineInterpolator::Pointer bspline_fixed_img =
                                            ImageBSplineInterpolator::New();
    ImageBSplineInterpolator::Pointer bspline_moving_img =
                                            ImageBSplineInterpolator::New();


  // -- register the slices --

  unsigned int num_its ;
    double best_value ;

  // volume origin
    const FloatImageType::PointType& orgn = fixed_img->GetOrigin();
    // spacing
    FloatImageType::SpacingType spacing = fixed_img->GetSpacing();
    // size in pixels
    FloatImageType::SizeType size_pxl = 
          fixed_img->GetLargestPossibleRegion().GetSize();
    // size in mm
    float size_mm[2];
    size_mm[0] = ceil(size_pxl[0]*spacing[0]); // R in mm
    size_mm[1] = ceil(size_pxl[1]*spacing[1]); // A in mm
    std::cerr<<"image size in mm  "<<size_mm;


  ResampleImageFilter::Pointer image_resampler = ResampleImageFilter::New();

  // -- build an image grid to visualize the deformation field --
    FloatImageType::Pointer grid_image = FloatImageType::New();
    FloatImageType::Pointer deformed_grid_image;
    grid_image->SetRegions(fixed_img->GetLargestPossibleRegion());
    grid_image->Allocate();
    grid_image->SetSpacing(fixed_img->GetSpacing());
    grid_image->SetOrigin(fixed_img->GetOrigin());

    FloatImageIndexedIterator grid_image_it( grid_image,
                                    grid_image->GetRequestedRegion() );
    for ( grid_image_it.GoToBegin(); !grid_image_it.IsAtEnd(); ++grid_image_it){
        FloatImageType::IndexType idx = grid_image_it.GetIndex();
        int x = idx[0];
        int y = idx[1];
        int pixel_value = (!((x%GRID_STEP_X) && (y%GRID_STEP_Y)))*32768;
        grid_image_it.Set( pixel_value );
    }
  // to write out the deformation field image
    ResampleImageFilter::Pointer grid_resampler = ResampleImageFilter::New();

  Registration::Pointer registration = Registration::New();
  // progress update
    ProgressDisplay progressDisplay_reg(registration);
    itk::SimpleMemberCommand<ProgressDisplay>::Pointer progressEvent_reg =
        itk::SimpleMemberCommand<ProgressDisplay>::New();
    progressEvent_reg->SetCallbackFunction(&progressDisplay_reg,
                                        &ProgressDisplay::Display);
    registration->AddObserver(itk::ProgressEvent(), progressEvent_reg);


  BSplineInterpolator::Pointer burp= BSplineInterpolator::New();
    registration->SetInterpolator( burp);

  CGDOptimizer::Pointer cgd_optimizer= CGDOptimizer::New();  
    registration->SetOptimizer( cgd_optimizer );
    CommandIterationUpdate<CGDOptimizer>::Pointer cgd_observer =  
                    CommandIterationUpdate<CGDOptimizer>::New();
  cgd_optimizer->AddObserver( itk::IterationEvent(), cgd_observer);


  MSMetric::Pointer ms_metric = MSMetric::New();
    registration->SetMetric( ms_metric );
    
  
  // -- Apply Translation Transform --
  TranslationTransform::Pointer trans_xform = TranslationTransform::New();
  registration->SetTransform( trans_xform );

  // setup transform parameters
  trans_xform->SetIdentity();
    registration->SetInitialTransformParameters( trans_xform->GetParameters() ); 

  // set the scales of the parameter space
  TranslationTransform::ParametersType trans_xform_scales(
                      trans_xform->GetNumberOfParameters());
    // the range of 1.0 / the image diag size (in millimeters).
    trans_xform_scales[0] = 1.0;
    trans_xform_scales[1] = 1.0;
    cgd_optimizer->SetScales(trans_xform_scales); //optimizer scale
  
  //set fixed and moving images
    registration->SetFixedImage( fixed_img );
    registration->SetMovingImage( moving_img );
    registration->SetFixedImageRegion(
                            fixed_img->GetBufferedRegion() );

  // do registration
  registration->Update();

  num_its = cgd_optimizer->GetCurrentIteration();
    best_value = cgd_optimizer->GetValue();

  std::cerr<<endl<<" After translation transform";
  std::cerr<<" its: "<<num_its;
    std::cerr<<" error: "<<best_value;

  // apply the transform to the moving image
    image_resampler->SetInput(moving_img);
    image_resampler->SetTransform( registration->GetOutput()->Get() );
    image_resampler->SetSize(
                moving_img->GetLargestPossibleRegion().GetSize() );
    image_resampler->SetOutputOrigin( moving_img->GetOrigin() );
    image_resampler->SetOutputSpacing( moving_img->GetSpacing() );
    image_resampler->SetDefaultPixelValue( 0 );
    image_resampler->Update();
    moving_img = image_resampler->GetOutput();

    // apply the transform to the grid image and save it
    grid_resampler->SetInput(grid_image);
    grid_resampler->SetTransform( registration->GetOutput()->Get() );
    grid_resampler->SetSize(
                moving_img->GetLargestPossibleRegion().GetSize() );
    grid_resampler->SetOutputOrigin( grid_image->GetOrigin() );
    grid_resampler->SetOutputSpacing( grid_image->GetSpacing() );
    grid_resampler->SetDefaultPixelValue( 0 );
    grid_resampler->Update();
    grid_image = grid_resampler->GetOutput();

  // -- Apply Centered Rigid 2D Transform --
    CenteredRigid2DTransform::Pointer centered_xform
                      = CenteredRigid2DTransform::New();
    registration->SetTransform( centered_xform );
  // set initial parameters of transform
    CenteredRigid2DTransform::InputPointType center__;
    CenteredRigid2DTransform::OutputVectorType translation__;
    translation__[0] = 0.0;
    translation__[1] = 0.0;
    center__[0] = orgn[0] + size_pxl[0] / 2.0;
    center__[1] = orgn[1] + size_pxl[1] / 2.0;
    centered_xform->SetCenter( center__ );
    centered_xform->SetTranslation(translation__);
    centered_xform->SetAngle( 0.0 );
    registration->SetInitialTransformParameters( centered_xform->GetParameters() );

  //setup optimizer scales
    CenteredRigid2DTransform::ParametersType centered_xform_scales(
                    centered_xform->GetNumberOfParameters());
    // The first parameter is the angle in  radians. Second and third are the
    // center of rotation coordinates and the last two parameters are the
    // translation in each dimension.
    centered_xform_scales[0] = 1.0;
    // ra plane center - in space coordinates
    centered_xform_scales[1] = 1.0/size_mm[0]/1.414;
    centered_xform_scales[2] = 1.0/size_mm[0]/1.414;
    //translation terms
    //the range of 1.0 / the image diag size (in millimeters).
    centered_xform_scales[3] = 1.0/size_mm[0]/1.414;
    centered_xform_scales[4] = 1.0/size_mm[0]/1.414;
  cgd_optimizer->SetScales(centered_xform_scales); //optimizer scale

  //set fixed and moving images
    registration->SetFixedImage( fixed_img );
    registration->SetMovingImage( moving_img );
    registration->SetFixedImageRegion(
                            fixed_img->GetBufferedRegion() );

  // do registration
  registration->Update();

  num_its = cgd_optimizer->GetCurrentIteration();
    best_value = cgd_optimizer->GetValue();

  std::cerr<<endl<<" After translation transform";
  std::cerr<<" its: "<<num_its;
    std::cerr<<" error: "<<best_value;

  // apply the transform to the moving image
    image_resampler->SetInput(moving_img);
    image_resampler->SetTransform( registration->GetOutput()->Get() );
    image_resampler->SetSize(
                moving_img->GetLargestPossibleRegion().GetSize() );
    image_resampler->SetOutputOrigin( moving_img->GetOrigin() );
    image_resampler->SetOutputSpacing( moving_img->GetSpacing() );
    image_resampler->SetDefaultPixelValue( 0 );
    image_resampler->Update();
    moving_img = image_resampler->GetOutput();

    // apply the transform to the grid image and save it
    grid_resampler->SetInput(grid_image);
    grid_resampler->SetTransform( registration->GetOutput()->Get() );
    grid_resampler->SetSize(
                moving_img->GetLargestPossibleRegion().GetSize() );
    grid_resampler->SetOutputOrigin( grid_image->GetOrigin() );
    grid_resampler->SetOutputSpacing( grid_image->GetSpacing() );
    grid_resampler->SetDefaultPixelValue( 0 );
    grid_resampler->Update();
    grid_image = grid_resampler->GetOutput();

  // -- Apply Affine Transform --
    AffineTransform::Pointer affine_xform = AffineTransform::New();

  registration->SetTransform( affine_xform );
  // setup transform parameters
    AffineTransform::InputPointType aff_center__;
    AffineTransform::OutputVectorType aff_translation__;
    aff_translation__[0] = 0.0;
    aff_translation__[1] = 0.0;
    aff_center__[0] = orgn[0] + size_pxl[0] / 2.0;
    aff_center__[1] = orgn[1] + size_pxl[1] / 2.0;
    affine_xform->SetIdentity();
    affine_xform->SetCenter( aff_center__ );
    affine_xform->SetTranslation(aff_translation__);
    registration->SetInitialTransformParameters( affine_xform->GetParameters() );

  // set the scales of the parameter space
  AffineTransform::ParametersType
    affine_xform_scales(affine_xform->GetNumberOfParameters());
  affine_xform_scales[0] = 1.0;  //rotation,scaling,shear terms
  affine_xform_scales[1] = 1.0;
  affine_xform_scales[2] = 1.0;
  affine_xform_scales[3] = 1.0;
  //translation terms
  //the range of 1.0 / the image diag size (in millimeters).
  affine_xform_scales[4] = 1.0/size_mm[0]/1.414;
  affine_xform_scales[5] = 1.0/size_mm[0]/1.414;
  cgd_optimizer->SetScales(affine_xform_scales); //optimizer scale

  //set fixed and moving images
    registration->SetFixedImage( fixed_img );
    registration->SetMovingImage( moving_img );
    registration->SetFixedImageRegion(
                            fixed_img->GetBufferedRegion() );

  // do registration
  registration->Update();

  num_its = cgd_optimizer->GetCurrentIteration();
    best_value = cgd_optimizer->GetValue();

  std::cerr<<endl<<" After translation transform";
  std::cerr<<" its: "<<num_its;
    std::cerr<<" error: "<<best_value;

  // apply the transform to the moving image
    image_resampler->SetInput(moving_img);
    image_resampler->SetTransform( registration->GetOutput()->Get() );
    image_resampler->SetSize(
                moving_img->GetLargestPossibleRegion().GetSize() );
    image_resampler->SetOutputOrigin( moving_img->GetOrigin() );
    image_resampler->SetOutputSpacing( moving_img->GetSpacing() );
    image_resampler->SetDefaultPixelValue( 0 );
    image_resampler->Update();
    moving_img = image_resampler->GetOutput();

    // apply the transform to the grid image and save it
    grid_resampler->SetInput(grid_image);
    grid_resampler->SetTransform( registration->GetOutput()->Get() );
    grid_resampler->SetSize(
                moving_img->GetLargestPossibleRegion().GetSize() );
    grid_resampler->SetOutputOrigin( grid_image->GetOrigin() );
    grid_resampler->SetOutputSpacing( grid_image->GetSpacing() );
    grid_resampler->SetDefaultPixelValue( 0 );
    grid_resampler->Update();
    grid_image = grid_resampler->GetOutput();



    //write out the deformed grid image
    char output_file_name[100];
    sprintf( output_file_name, "deformation_field.png");
    flt_2_us_cast->SetInput(grid_image);
    image_writer->SetInput(flt_2_us_cast->GetOutput());
    image_writer->SetFileName(output_file_name);
    image_writer->Update();

  // write out the moving image
  flt_2_us_cast->SetInput(moving_img);
    image_writer->SetInput(flt_2_us_cast->GetOutput());
    image_writer->SetFileName(argv[3]);
    image_writer->Update();

    std::cerr<<endl<<" ... done ";

}// try
catch( itk::ExceptionObject & err ) {
    std::cerr << "EXCEPTION!" << endl;
    std::cerr << err << endl;
    return EXIT_FAILURE;
}



}
