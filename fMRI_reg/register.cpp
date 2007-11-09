#include "includes.h"

const int GRID_STEP_X = 8;
const int GRID_STEP_Y = 8;

#define DEBUG_FJ_ 1

//@func registerSlicers
//@desc register the slices for the fmri volume - into the same volume
int registerSlices(FloatVolumeType::Pointer volume, char transform_type,
                   char metric_type, char opt_type, char interp_type=1)
{
    // metrics
    Metric::Pointer metric; //the superclass object
    MSMetric::Pointer ms_metric;
    VW_MIMetric::Pointer vw_mi_metric;
    MattesMetric::Pointer mattes_metric;
    KLMetric::Pointer kl_metric;

    // transforms
    TranslationTransform::Pointer trans_xform;
    AffineTransform::Pointer affine_xform;
    CenteredRigid2DTransform::Pointer centered_xform;
    Transform::Pointer  xform;
    itk::Array<double> xform_scales; //general object

    //optimizers
    RSGDOptimizer::Pointer rsgd_optimizer;
    CGDOptimizer::Pointer cgd_optimizer;
    AmoebaOptimizer::Pointer amoeba_optimizer;
    Optimizer::Pointer optimizer;   //the superclass object

    // the iteration observer classes
    CommandIterationUpdate<RSGDOptimizer>::Pointer rgsd_observer ;
    CommandIterationUpdate<CGDOptimizer>::Pointer cgd_observer ;
    CommandIterationUpdate<AmoebaOptimizer>::Pointer amoeba_observer ;


    //interpolators
    LinearInterpolator::Pointer lerp;
    BSplineInterpolator::Pointer burp;

    bool deformable_flag = false; //true for deformable registration

    // needed for all cases
    SliceExtractorType::Pointer slicer = SliceExtractorType::New();
    ResampleImageFilter::Pointer image_resampler = ResampleImageFilter::New();
    Registration::Pointer registration = Registration::New();;

    // to write out the deformation field image
    ResampleImageFilter::Pointer grid_resampler = ResampleImageFilter::New();
    ImageWriterType::Pointer image_writer = ImageWriterType::New();
    FloatToUShortImageCast::Pointer flt_2_us_cast = FloatToUShortImageCast::New();

    // volume origin
    const FloatVolumeType::PointType& orgn = volume->GetOrigin();
    // spacing
    FloatVolumeType::SpacingType spacing = volume->GetSpacing();
    // size in pixels
    FloatVolumeType::SizeType size_pxl = volume->GetLargestPossibleRegion().GetSize();
    // size in mm
    float size_mm[2];
    size_mm[0] = ceil(size_pxl[0]*spacing[0]); // R in mm
    size_mm[1] = ceil(size_pxl[1]*spacing[1]); // A in mm
    cerr<<"volume size in mm  "<<size_mm;

    //set up the transform
    switch(transform_type){
    case 0:
        cerr<<endl<<"translation transform";
        trans_xform = TranslationTransform::New();
        xform = trans_xform;
        registration->SetTransform( trans_xform );
        // set the scales of the parameter space
        {
            TranslationTransform::ParametersType
                trans_xform_scales(trans_xform->GetNumberOfParameters());
            // the range of 1.0 / the image diag size (in millimeters).
            trans_xform_scales[0] = 1.0/size_mm[0]/1.414;
            trans_xform_scales[1] = 1.0/size_mm[0]/1.414;
            xform_scales = trans_xform_scales;

        }
        break;

    case 1:
        affine_xform = AffineTransform::New();
        xform = affine_xform;
        cerr<<endl<<"affine transform with "
              <<affine_xform->GetNumberOfParameters()
              <<" params";
        registration->SetTransform( affine_xform );
        // set the scales of the parameter space
        {
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
            xform_scales = affine_xform_scales;
        }

        break;

    case 2: //centered transform
        centered_xform = CenteredRigid2DTransform::New();
        xform = centered_xform;
        cerr<<endl<<"centered rigid 2d transform with "
              <<centered_xform->GetNumberOfParameters()
              <<" params";
        registration->SetTransform( centered_xform );

        // set the scales of the parameter space
        {
            CenteredRigid2DTransform::ParametersType
                centered_xform_scales(centered_xform->GetNumberOfParameters());
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
            xform_scales = centered_xform_scales;
        }
        break;

    default:
        cerr<<endl<<"transform option not supported";
        return -1;
    }

    // setup the metric
    switch(metric_type){
    case 0: //wells
        cerr<<endl<<"viola wells metric";
        vw_mi_metric = VW_MIMetric::New();
        registration->SetMetric( vw_mi_metric );
        vw_mi_metric->SetNumberOfSpatialSamples(1024);
        vw_mi_metric->SetMovingImageStandardDeviation(.2);
        vw_mi_metric->SetFixedImageStandardDeviation(.2);
        metric = vw_mi_metric;
        break;

    case 1: //matts
        cerr<<endl<<"mattes metric";
        mattes_metric = MattesMetric::New();
        registration->SetMetric( mattes_metric );
        //set this high in order to over-sample the probability distribution for now
        mattes_metric->SetNumberOfSpatialSamples(2<<12);
        mattes_metric->SetNumberOfHistogramBins(128);
        mattes_metric->ReinitializeSeed();
        metric = mattes_metric;
        break;

    case 2: //mean squares
        cerr<<endl<<"mean squares metric";
        ms_metric = MSMetric::New();
        registration->SetMetric( ms_metric );
        metric = ms_metric;
        break;

    case 3: //kl
        cerr<<endl<<"kl divergence";
        kl_metric = KLMetric::New();
        registration->SetMetric( kl_metric );
        metric = kl_metric;
        // no params as far as i can tell - don't know about histogram size
        break;

    default:
        cerr<<endl<<"metric option not supported";
        return -1;
    }

    /** NOTE: depending upon the metric, either maximize or minimize
     *  maximize for wells, minimize for mattes and rms
     */

    // setup the optimizer
    switch(opt_type){
    case 0: //gradient descent
        cerr<<endl<<"regular step gradient descent optimizer";
        rsgd_optimizer = RSGDOptimizer::New();
        optimizer = rsgd_optimizer;
        registration->SetOptimizer( rsgd_optimizer );
        rgsd_observer = CommandIterationUpdate<RSGDOptimizer>::New();
        rsgd_optimizer->AddObserver( itk::IterationEvent(), rgsd_observer );
        /** set the parameter scales
         *  see http://www.itk.org/Wiki/ITK_Image_Registration
         */
        rsgd_optimizer->SetScales(xform_scales);
        if( metric_type != 0 )
            rsgd_optimizer->MaximizeOff();
        else
            rsgd_optimizer->MaximizeOn();

        break;
    case 1: //conj grad des
        cerr<<endl<<"conjugate gradient descent optimizer";
        cgd_optimizer = CGDOptimizer::New();
        optimizer = cgd_optimizer;
        registration->SetOptimizer( cgd_optimizer );
        cgd_observer = CommandIterationUpdate<CGDOptimizer>::New();
        cgd_optimizer->AddObserver( itk::IterationEvent(), cgd_observer);
        cgd_optimizer->SetScales(xform_scales);
        if( metric_type != 0 )
            cgd_optimizer->MaximizeOff();
        else
            cgd_optimizer->MaximizeOn();
        break;

    case 2: //amoeba

        cerr<<endl<<"conjugate gradient descent optimizer";
        amoeba_optimizer = AmoebaOptimizer::New();
        optimizer = amoeba_optimizer;
        registration->SetOptimizer( amoeba_optimizer );
        amoeba_observer = CommandIterationUpdate<AmoebaOptimizer>::New();
        amoeba_optimizer->AddObserver( itk::IterationEvent(), amoeba_observer);
        amoeba_optimizer->SetScales(xform_scales);
        if( metric_type != 0 )
            amoeba_optimizer->MaximizeOff();
        else
            amoeba_optimizer->MaximizeOn();
        break;

    default:
        cerr<<endl<<"optimizer option not supported";
        return -1;
    }
    cerr<<endl<<"optimizer scales "<<optimizer->GetScales();

    // setup the interpolater
    switch(interp_type){
    case 0:
        cerr<<endl<<"linear interp";
        lerp = LinearInterpolator::New();
        registration->SetInterpolator( lerp );
        break;
    case 1:
        cerr<<endl<<"bspline interp";
        burp = BSplineInterpolator::New();
        registration->SetInterpolator( burp);
        break;

    default:
        cerr<<endl<<"optimizer option not supported";
        return -1;
    }

    // the pointers to the slices
    FloatImageType::Pointer prev_slice, curr_slice, next_slice;


    // extract the slices from the volume
    slicer->SetInput(volume);

    // input region properties
    FloatVolumeType::RegionType input_rgn =
                            volume->GetLargestPossibleRegion();
    FloatVolumeType::SizeType size = input_rgn.GetSize();
    FloatVolumeType::IndexType start = input_rgn.GetIndex();

    unsigned int num_slices = size[2];
    cerr<<endl<<"total number of slices is "<<num_slices;

    // collapse the Z dimension to set slice region
    size[2] = 0;

    int start_slice = start[2];


    // extract the 0th slice
    FloatVolumeType::RegionType desired_rgn;
    desired_rgn.SetSize( size );
    desired_rgn.SetIndex( start );
    slicer->SetExtractionRegion( desired_rgn );
    slicer->Update();
    prev_slice = slicer->GetOutput();

    // -- build an image grid to visualize the deformation field --
    FloatImageType::Pointer grid_image = FloatImageType::New();
    FloatImageType::Pointer deformed_grid_image;
    grid_image->SetRegions(prev_slice->GetLargestPossibleRegion());
    grid_image->Allocate();
    grid_image->SetSpacing(prev_slice->GetSpacing());
    grid_image->SetOrigin(prev_slice->GetOrigin());

    FloatImageIndexedIterator grid_image_it( grid_image,
                                    grid_image->GetRequestedRegion() );
    for ( grid_image_it.GoToBegin(); !grid_image_it.IsAtEnd(); ++grid_image_it){
        FloatImageType::IndexType idx = grid_image_it.GetIndex();
        int x = idx[0];
        int y = idx[1];
        int pixel_value = (!((x%GRID_STEP_X) && (y%GRID_STEP_Y)))*32768;
        grid_image_it.Set( pixel_value );
    }

#if DEBUG_FJ_
    //write out the img
    char *output_file_name = "grid_image.png";
    cerr<<endl<<"--Writing grid image file "<<output_file_name;

    // set the file name
    flt_2_us_cast->SetInput(grid_image);
    image_writer->SetInput(flt_2_us_cast->GetOutput());
    image_writer->SetFileName(output_file_name);

    // write out the image
    cerr<<endl<<"start ...";
    image_writer->Update();
    cerr<<"... done ";

    // test the moving image and fixed image spacing and origin
    cerr<<endl<<"\tFixed Image:"
        <<" Size "<<prev_slice->GetLargestPossibleRegion().GetSize()
        <<" Origin "<<prev_slice->GetOrigin()
        <<" Spacing "<< prev_slice->GetSpacing()<<endl;
#endif

    // forward iteration thru the slices
    for(unsigned int slice_num = start_slice+1;
                     slice_num  < num_slices + start_slice;
                     slice_num ++ ){

        cerr<<endl<<"slc: "<<slice_num;

        // extract the current slice
        start[2] = slice_num;
        desired_rgn.SetIndex( start );
        slicer->SetExtractionRegion( desired_rgn );
        slicer->Update();
        curr_slice = slicer->GetOutput();

#if DEBUG_FJ_
        cerr<<endl<<"\tMoving Image:"
            <<" Size "<<curr_slice->GetLargestPossibleRegion().GetSize()
            <<" Origin "<<curr_slice->GetOrigin()
            <<" Spacing "<< curr_slice->GetSpacing()<<endl;
#endif //DEBUG_FJ_


        //set fixed and moving images
        registration->SetFixedImage( prev_slice );
        registration->SetMovingImage( curr_slice );
        registration->SetFixedImageRegion(
                            prev_slice->GetBufferedRegion() );

        //setup transform parameters
        switch(transform_type){
        case 0:
            trans_xform->SetIdentity();
            registration->SetInitialTransformParameters( trans_xform->GetParameters() );            break;

        case 1:  // affine
            {
            AffineTransform::InputPointType center__;
            AffineTransform::OutputVectorType translation__;
            translation__[0] = 0.0;
            translation__[1] = 0.0;
            center__[0] = orgn[0] + size[0] / 2.0;
            center__[1] = orgn[1] + size[1] / 2.0;
            affine_xform->SetIdentity();
            affine_xform->SetCenter( center__ );
            affine_xform->SetTranslation(translation__);
            registration->SetInitialTransformParameters( affine_xform->GetParameters() );
            }
            break;

        case 2: //centered
            {
            CenteredRigid2DTransform::InputPointType center__;
            CenteredRigid2DTransform::OutputVectorType translation__;
            translation__[0] = 0.0;
            translation__[1] = 0.0;
            center__[0] = orgn[0] + size[0] / 2.0;
            center__[1] = orgn[1] + size[1] / 2.0;
            centered_xform->SetCenter( center__ );
            centered_xform->SetTranslation(translation__);
            centered_xform->SetAngle( 0.0 );
            registration->SetInitialTransformParameters( centered_xform->GetParameters() );
            }
            break;

        }//switch(transform_type)

        //optimizer tuning
        switch(opt_type){
        //[fj] want to parameterize this
        case 0: //gradient descent
            /* The initial step length is defined with the method
               SetMaximumStepLength(), while the tolerance for convergence
               is defined with the method SetMinimumStepLength().
             */
            rsgd_optimizer->SetMaximumStepLength( 1.0 );
            rsgd_optimizer->SetMinimumStepLength( 0.001 );
            rsgd_optimizer->SetNumberOfIterations( 200 );
            break;
        case 1: //conj gradient
            break;

        case 2: //amoeba optimizer
            AmoebaOptimizer::ParametersType simplexDelta(
                                            xform->GetNumberOfParameters());
            // initialize the simplex to 2 times the scale in that parameter
            simplexDelta = xform_scales*2;
            amoeba_optimizer->AutomaticInitialSimplexOff();
            amoeba_optimizer->SetInitialSimplexDelta( simplexDelta );


            /* Adjust the convergence tolerances of the parameters
               to zero since we're dealing with all kinds of parameter types
             */
            amoeba_optimizer->SetParametersConvergenceTolerance( 0 );


            /*  tolerance on the cost function value -- depends heavily on the
                metric. here it is setup for mi metric. may want to change it by                        parameterizing it
             */
            amoeba_optimizer->SetFunctionConvergenceTolerance(0.001);

            amoeba_optimizer->SetMaximumNumberOfIterations( 200 );

            break;

        }//switch(opt_type)

        ProgressDisplay progressDisplay(registration);
        itk::SimpleMemberCommand<ProgressDisplay>::Pointer progressEvent =
            itk::SimpleMemberCommand<ProgressDisplay>::New();
        progressEvent->SetCallbackFunction(&progressDisplay,
                                            &ProgressDisplay::Display);
        registration->AddObserver(itk::ProgressEvent(), progressEvent);
        registration->Update();
        unsigned int num_its ;
        double best_value ;

        switch(opt_type){
        case 0: //gradient descent
            num_its = rsgd_optimizer->GetCurrentIteration();
            best_value = rsgd_optimizer->GetValue();
            break;
        case 1: //conj grad des
            num_its = cgd_optimizer->GetCurrentIteration();
            best_value = cgd_optimizer->GetValue();
            break;
        case 2: //amoeba
            num_its = amoeba_optimizer->GetOptimizer()->get_num_evaluations();
            best_value = amoeba_optimizer->GetValue();
            break;

        default:
            cerr<<endl<<"optimizer option not supported";
            return -1;
        }

        cerr<<" its: "<<num_its;
        cerr<<" error: "<<best_value;

        // rigid registration
        if(!deformable_flag){
            // apply the transform to the moving image
            image_resampler->SetInput(curr_slice);
            image_resampler->SetTransform( registration->GetOutput()->Get() );
            image_resampler->SetSize(
                        prev_slice->GetLargestPossibleRegion().GetSize() );
            image_resampler->SetOutputOrigin( prev_slice->GetOrigin() );
            image_resampler->SetOutputSpacing( prev_slice->GetSpacing() );
            image_resampler->SetDefaultPixelValue( 0 );
            image_resampler->Update();
            curr_slice = image_resampler->GetOutput();

            // apply the transform to the grid image and save it
            grid_resampler->SetInput(grid_image);
            grid_resampler->SetTransform( registration->GetOutput()->Get() );
            grid_resampler->SetSize(
                        prev_slice->GetLargestPossibleRegion().GetSize() );
            grid_resampler->SetOutputOrigin( prev_slice->GetOrigin() );
            grid_resampler->SetOutputSpacing( prev_slice->GetSpacing() );
            grid_resampler->SetDefaultPixelValue( 0 );
            grid_resampler->Update();
            deformed_grid_image = grid_resampler->GetOutput();
        }

        // write the slice out to the image
        volume->SetRequestedRegion(desired_rgn);

        FloatImageType::RegionType image_rgn =
                                curr_slice->GetLargestPossibleRegion();

        FloatVolumeIterator vol_it(volume, desired_rgn);
        FloatImageIterator img_it(curr_slice, image_rgn);
        for ( vol_it.GoToBegin(), img_it.GoToBegin();
              !img_it.IsAtEnd();
              ++vol_it, ++img_it){
            vol_it.Set( img_it.Get() );
        }

        //write out the deformed grid image
        char output_file_name[100];
        sprintf( output_file_name, "deffield_t%01d_m%01d_o%01d_i%01d_%02d.png",
                 transform_type, metric_type, opt_type, interp_type, slice_num);
        flt_2_us_cast->SetInput(deformed_grid_image);
        image_writer->SetInput(flt_2_us_cast->GetOutput());
        image_writer->SetFileName(output_file_name);
        image_writer->Update();
        cerr<<" ... done ";

        //update for next iteration
        prev_slice = curr_slice;
        //curr_slice = next_slice;

    }//for slice_num
    return 0;

}//registerSlices
