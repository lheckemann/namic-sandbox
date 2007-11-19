/// code for NA-MIC sandbox
/// logic for affine registration with rms, wells, and mattes mi metrics. grad-desc, conj-grad-desc and downhill-simplex optimizers


#include "includes.h"

// the fractional percentage of the total number of pixel to normalize against
const double NORMALIZATION_FRACTION = 0.99;

//register.cpp
int registerSlices(FloatVolumeType::Pointer volume, char transform_type,
                   char metric_type, char opt_type, char interp_type=1);

int main(int argc, char *argv[])
{
    cout<<"Build date "<<__DATE__<<" and time "<<__TIME__<<endl<<endl;

    if( 1+7 != argc  ){
        cerr<<endl<<"Insufficient number of arguments";
        cerr<<endl<<argv[0]
            <<endl<<"   -input "<<"<input hdr file name>"
            <<endl<<"   -output "<<"<output hdr file name>"
            <<endl<<"   -reg <metric> <optimizer>"
            <<endl<<"        <metric> 0-wells/1-matts/2-mean squares/3-kl"
            <<endl<<"        <optimizer> 0-gradient_descent/1-cong gradient/2-amoeba";
        return EXIT_FAILURE;
    }

    // parameters
    char *input_file_name = argv[2];
    char *output_file_name = argv[4];
    unsigned char metric_type = atoi(argv[6]);
    unsigned char opt_type = atoi(argv[7]);

try{

    /** NOTES:
        All dimensions are in mms
     **/

    FloatVolumeType::Pointer current_vol, registered_vol;
    UShortVolumeType::Pointer input_vol, output_vol;

    VolumeReaderType::Pointer volume_reader = VolumeReaderType::New();
    VolumeWriterType::Pointer volume_writer = VolumeWriterType::New();
    ImageSeriesReaderType::Pointer series_reader ;
    SeriesWriterType::Pointer series_writer;
    AnalyzeImageIOType::Pointer image_io = AnalyzeImageIOType::New();

    ResampleVolumeFilter::Pointer resamp_volume = ResampleVolumeFilter::New();
    BSplineVolumeInterpolator::Pointer bspline_volume =
                                            BSplineVolumeInterpolator::New();

    FloatToUShortCast::Pointer flt_2_us_cast = FloatToUShortCast::New() ;
    UShortToFloatCast::Pointer us_2_flt_cast = UShortToFloatCast::New();
    RealToFloatCast::Pointer r2f_cast ;

    VolumeNormalizeFilter::Pointer normalizer = VolumeNormalizeFilter::New();
    RescaleIntensityFilter::Pointer rescaler = RescaleIntensityFilter::New();
    // the value against which to normalize the volume.
    double normalization_value = 0;


    // -- input the volume --
         cerr<<endl<<"--Reading analyze file "<<input_file_name;

        // set the file name
        volume_reader->SetFileName(input_file_name);
        us_2_flt_cast->SetInput(volume_reader->GetOutput());

        // read in the images
        cerr<<endl<<"start ...";
        us_2_flt_cast->Update();
        cerr<<"... done ";

        input_vol =  volume_reader->GetOutput();
        current_vol = us_2_flt_cast->GetOutput();

        cerr<<endl<<"\t-- Volume Information --"
            <<endl<<"Size "
                    <<current_vol->GetLargestPossibleRegion().GetSize()
            <<endl<<"Origin "<<current_vol->GetOrigin()
            <<endl<<"Spacing "<< current_vol->GetSpacing()<<endl;

    //--resample to 1mm dimensions--
        cerr<<"\n+ Resampling to 1mm in axial plane{  ";

        resamp_volume->SetInput(current_vol);

        // the identity transform
        typedef itk::IdentityTransform<double, 3> IdentityTransform;
        IdentityTransform::Pointer eye = IdentityTransform::New();
        // identity transform
        resamp_volume->SetTransform(eye);
        //set the interpolator
        resamp_volume->SetInterpolator( bspline_volume );
        resamp_volume->SetDefaultPixelValue( 0 );

        resamp_volume->SetOutputOrigin( current_vol->GetOrigin() );

        FloatVolumeType::SpacingType spacing = current_vol->GetSpacing();

        typedef FloatVolumeType::SizeType  SizeType;

        SizeType size = current_vol->GetLargestPossibleRegion().GetSize();

        typedef SizeType::SizeValueType   SizeValueType;

        // number of pixels along X and Y respectively
        size[0] = static_cast< SizeValueType >( ceil(size[0]*spacing[0]) ); 
        size[1] = static_cast< SizeValueType >( ceil(size[1]*spacing[1]) ); 
        resamp_volume->SetSize( size );

        spacing[0] = 1.0; // pixel spacing in millimeters along X
        spacing[1] = 1.0; // pixel spacing in millimeters along Y
        resamp_volume->SetOutputSpacing( spacing );

        ProgressDisplay progressDisplay(resamp_volume);
        itk::SimpleMemberCommand<ProgressDisplay>::Pointer progressEvent =
                        itk::SimpleMemberCommand<ProgressDisplay>::New();
        progressEvent->SetCallbackFunction(&progressDisplay,
                                            &ProgressDisplay::Display);
        resamp_volume->AddObserver(itk::ProgressEvent(), progressEvent);

        resamp_volume->Update();
        current_vol = resamp_volume->GetOutput();

        cerr<<"\n } END Resampling -  ";


        //--normalzie the volume --
            normalizer->SetInput( current_vol );
            normalizer->Update();
            current_vol = normalizer->GetOutput();

        //-- perform slice to slice registration
            cerr<<endl<<"--Registration operation ";
            cerr<<endl<<endl<<"\t translation transform ";
            registerSlices(current_vol, 0, metric_type, opt_type);
//          cerr<<endl<<endl<<"\t centered 2d rigid transform ";
//          registerSlices(current_vol, 2, metric_type, opt_type);
            cerr<<endl<<endl<<"\t affine transform ";
            registerSlices(current_vol, 1, metric_type, opt_type);
            cerr<<endl<<"registration done --";

        //--undo the normalization of the volume --
            rescaler->SetInput( current_vol );
            rescaler->SetOutputMinimum( 1 );
            rescaler->SetOutputMaximum( 65535 );
            rescaler->Update();
            current_vol = rescaler->GetOutput();

        //-- write out the result

            cerr<<endl<<"--Writing analyze file "<<output_file_name;
            // set the file name
            flt_2_us_cast->SetInput(current_vol);
            volume_writer->SetInput(flt_2_us_cast->GetOutput());
            volume_writer->SetFileName(output_file_name);
            volume_writer->SetImageIO( image_io );
            // write out the volume
            cerr<<endl<<"start ...";
            volume_writer->Update();
            cerr<<"... done ";

}// try
catch( itk::ExceptionObject & err ) {
    cerr << "EXCEPTION!" << endl;
    cerr << err << endl;
    return EXIT_FAILURE;
}

    return EXIT_SUCCESS;
}




