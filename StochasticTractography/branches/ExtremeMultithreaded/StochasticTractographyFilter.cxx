#include "itkStochasticTractographyFilter.h"
#include "itkVectorImage.h"
#include "itkImageSeriesReader.h"  //this is needed for itk::ExposeMetaData()
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkMetaDataDictionary.h"
#include "itkAddImageFilter.h"
#include "itkUnaryFunctorImageFilter.h"
#include <iostream>
#include <vector>
#include "itkImageRegionConstIterator.h"
#include "StochasticTractographyFilterCLP.h"

template< class TTOContainerType >
bool SamplingDirections(const char* fn, typename TTOContainerType::Pointer directions){
  char str[100];
  double x,y,z;
  std::ifstream m_file;
  typename TTOContainerType::Element direction;

  //open the file
  m_file.open(fn);
  if(!m_file.is_open()){
    std::cerr<<"Problems opening: "<< fn << std::endl;
    return false;
  }
  else{
    //load file info into valid itk representation
    //assume all entries in the file describe a vector of length 1
    //each line is arranged [x, y, z]
    unsigned int directioncount=0;

    while(!m_file.getline(str, 100, '\n').eof()){
      for(int i=0;i<3;i++){
          sscanf(str,"%lf %lf %lf",&x,&y,&z);
          direction[0]=x;
          direction[1]=y;
          direction[2]=z;
        }
        directions->InsertElement(directioncount++,direction);
    }
  }
  
  //close file
  m_file.close();
  return true;
}


namespace Functor {  
   
template< class TInput, class TOutput>
class ZeroDWITest
{
public:
  ZeroDWITest() {};
  ~ZeroDWITest() {};
  bool operator!=( const ZeroDWITest & ) const
  {
    return false;
  }
  bool operator==( const ZeroDWITest & other ) const
  {
    return !(*this != other);
  }
  inline TOutput operator()( const TInput & A )
  {
    /*for(int i=0;i<A.GetSize();i++){
      if(A[0]<100){
        std::cout<<"Invalid Voxel\n";
        return 0;
      }
    }
    */
    return 10;
  }
}; 
 
}

int main(int argc, char* argv[]){
  PARSE_ARGS;
  //define the input/output types
  typedef itk::VectorImage< unsigned short int, 3 > DWIVectorImageType;
  typedef itk::Image< unsigned short int, 3 > MaskImageType;
  typedef itk::Image< short int, 3 > ROIImageType;
  typedef itk::Image< unsigned int, 3 > CImageType;
  
  //define an iterator for the ROI image
  typedef itk::ImageRegionConstIterator< ROIImageType > ROIImageIteratorType;
  
  //define reader and writer
  typedef itk::ImageFileReader< DWIVectorImageType > DWIVectorImageReaderType;
  typedef itk::ImageFileReader< ROIImageType > ROIImageReaderType;
  typedef itk::ImageFileWriter< CImageType > CImageWriterType;
  typedef itk::ImageFileWriter< MaskImageType > MaskImageWriterType;
  
  //define metadata dictionary types
  typedef itk::MetaDataDictionary DictionaryType;
  typedef DictionaryType::ConstIterator DictionaryIteratorType;

  //define a probabilistic tractography filter type and associated bValue,
  //gradient direction, and measurement frame types
  typedef itk::StochasticTractographyFilter< DWIVectorImageType, MaskImageType,
    CImageType > 
    PTFilterType;
  typedef PTFilterType::bValueContainerType bValueContainerType;
  typedef PTFilterType::GradientDirectionContainerType GDContainerType;
  typedef PTFilterType::MeasurementFrameType MeasurementFrameType;
  
  //define a filter to generate a mask image that excludes zero dwi values
  typedef Functor::ZeroDWITest< DWIVectorImageType::PixelType, MaskImageType::PixelType >
    ZeroDWITestType;
  typedef itk::UnaryFunctorImageFilter< DWIVectorImageType, MaskImageType, 
    ZeroDWITestType > ExcludeZeroDWIFilterType;
    
  //define AddImageFilterType to accumulate the connectivity maps of the pixels in the ROI
  typedef itk::AddImageFilter< CImageType, CImageType, CImageType> AddImageFilterType;
  
  //parse command line arguments
  if(argc < 3){
    std::cerr << "Usage: " << argv[0];
    std::cerr<< " DWIFile(.nhdr) ROIFile(.nhdr) ConnectivityFile(.nhdr) ";
    std::cerr<< "LabelNumber NumberOfTracts MaxTractLength MaxLikelihoodCacheSize\n";
    return EXIT_FAILURE;
  }
  /*
  char* dwifilename = argv[1];
  char* roifilename = argv[2];
  char* cfilename = argv[3];
  unsigned int labelnumber = atoi(argv[4]);
  unsigned int totaltracts = atoi(argv[5]);
  unsigned int maxtractlength = atoi(argv[6]);
  unsigned int maxlikelihoodcachesize = atoi(argv[7]);
  */
  //read in the DWI image
  DWIVectorImageReaderType::Pointer dwireaderPtr = DWIVectorImageReaderType::New();
  dwireaderPtr->SetFileName(dwifilename);
  dwireaderPtr->Update();
  
  //Obtain bValue, gradient directions and measurement frame from metadata dictionary
  DictionaryType& dictionary = dwireaderPtr->GetMetaDataDictionary();
  bValueContainerType::Pointer bValuesPtr = bValueContainerType::New();
  MeasurementFrameType measurement_frame;
  GDContainerType::Pointer gradientsPtr = GDContainerType::New();
  bValueContainerType::Element scaling_bValue = 0;
  GDContainerType::Element g_i;

  //bad choice of variable names: dictit->first refers to the key in the Map
  //dictit->second refers to the Value associated with that Key
  for(DictionaryIteratorType dictit = dictionary.Begin();
      dictit!=dictionary.End();
      ++dictit){
    if(dictit->first.find("DWMRI_gradient") != std::string::npos){
      std::string metaDataValue;
      itk::ExposeMetaData< std::string > (dictionary, dictit->first, metaDataValue);
      sscanf(metaDataValue.c_str(), "%lf %lf %lf\n", &g_i[0], &g_i[1], &g_i[2]);
      gradientsPtr->InsertElement( gradientsPtr->Size(), g_i );
    }
    else if(dictit->first.find("DWMRI_b-value") != std::string::npos){
      std::string metaDataValue;
      itk::ExposeMetaData< std::string >(dictionary, dictit->first, metaDataValue);
      scaling_bValue = atof(metaDataValue.c_str());
    }
    else if(dictit->first.find("NRRD_measurement frame") != std::string::npos){
      std::vector< std::vector < double > > metaDataValue;
      itk::ExposeMetaData< std::vector< std::vector<double> > >
        (dictionary, dictit->first, metaDataValue);
      for(int i=0;i<3;i++){
        for(int j=0;j<3;j++)
          measurement_frame(i,j) = metaDataValue[j][i];
      }
    }
    else{
      std::cout << dictit->first << std::endl;
    }
  }
  
  //check to see if bValue, gradients, or measurement frame is missing
  if(scaling_bValue == 0){
    std::cerr << "scaling_bValue should never be 0, possibly not found in Nrrd header\n";
    return EXIT_FAILURE;
  }
  else if(gradientsPtr->Size() == 0){
    std::cerr <<"no gradients were found!";
    return EXIT_FAILURE;
  }
  else if(measurement_frame.size() == 0){
    std::cerr <<"no measurement frame was found!";
    return EXIT_FAILURE;
  }
  //correct the measurement frame
  
  std::cout << scaling_bValue << std::endl;
  for(unsigned int i=0; i<gradientsPtr->Size(); i++)
    std::cout << gradientsPtr->GetElement(i) << std::endl;
  
  //std::cout << measurement_frame <<std::endl;
  
  for(unsigned int i=0; i<measurement_frame.rows(); i++){
    for(unsigned int j=0; j<measurement_frame.columns(); j++){
      std::cout<<measurement_frame(i,j) << " ";
    }
    std::cout << std::endl;
  }
 
  //correct the measurement frame since the image is now in LPS from RAS frame
  //NRRDIO should do this, but we do it here as a work around
  
  //fill up bValue container with the scaling_bValue;
  for(unsigned int i=0; i<gradientsPtr->Size() ;i++)
    bValuesPtr->InsertElement(i, scaling_bValue);
  //the first b value is zero for the non diffusion weighted image
  //this matters in the calculations for the constrained model but not the tensor model
  //since a gradient direction of all zeros handles it
  bValuesPtr->SetElement( 0, 0.0);
  
  //setup the ROI image reader
  ROIImageReaderType::Pointer roireaderPtr = ROIImageReaderType::New();
  roireaderPtr->SetFileName(roifilename);
  roireaderPtr->Update();
  
  /*
  //set list of directions
  typedef PTFilterType::TractOrientationContainerType TOCType;
  TOCType::Pointer directionsPtr = TOCType::New();
  
  if(SamplingDirections<TOCType>("SD.txt", directionsPtr));
  else return EXIT_FAILURE;
  */
  /*
  //write out the directions
  std::ofstream outfile("crapola.txt");
  
 
  for(int i=0; i<directionsPtr->Size(); i++){
    TOCType::Element dir = directionsPtr->GetElement(i);
    outfile<<"{"<<dir[0]<<", "<<dir[1]<<", "<<dir[2]<<"},"<<std::endl;
  }
  outfile.close();
  */
  
  //Create a default Mask Image which 
  //excludes DWI pixels which contain values that are zero
  ExcludeZeroDWIFilterType::Pointer ezdwifilter = ExcludeZeroDWIFilterType::New();
  ezdwifilter->SetInput( dwireaderPtr->GetOutput() );
  std::cout<<"Start mask image\n";  
  //write out the mask image
  MaskImageWriterType::Pointer maskwriterPtr = MaskImageWriterType::New();
  maskwriterPtr->SetInput( ezdwifilter->GetOutput() );
  maskwriterPtr->SetFileName( "maskimage.nhdr" );
  maskwriterPtr->Update();
  std::cout<<"Finish the mask image\n";

  std::cout<<"Create PTFilter\n";
  //Setup the PTFilter
  PTFilterType::Pointer ptfilterPtr = PTFilterType::New();
  ptfilterPtr->SetInput( dwireaderPtr->GetOutput() );
  ptfilterPtr->SetMaskImageInput( ezdwifilter->GetOutput() );
  ptfilterPtr->SetbValues(bValuesPtr);
  ptfilterPtr->SetGradients( gradientsPtr );
  ptfilterPtr->SetMeasurementFrame( measurement_frame );
  //ptfilterPtr->SetSampleDirections(directionsPtr);
  ptfilterPtr->SetMaxTractLength( maxtractlength );
  ptfilterPtr->SetTotalTracts( totaltracts );
  ptfilterPtr->SetMaxLikelihoodCacheSize( maxlikelihoodcachesize );
  ptfilterPtr->SetNumberOfThreads( 8 );
  
  //Setup the AddImageFilter
  AddImageFilterType::Pointer addimagefilterPtr = AddImageFilterType::New();
  
  //Create a zeroed Connectivity Image
  CImageType::Pointer accumulatedcimagePtr = CImageType::New();
  accumulatedcimagePtr->CopyInformation( dwireaderPtr->GetOutput() );
  accumulatedcimagePtr->SetBufferedRegion( dwireaderPtr->GetOutput()->GetBufferedRegion() );
  accumulatedcimagePtr->SetRequestedRegion( dwireaderPtr->GetOutput()->GetRequestedRegion() );
  accumulatedcimagePtr->Allocate();
  accumulatedcimagePtr->FillBuffer(0);
  
  //graft this onto the output of the addimagefilter
  addimagefilterPtr->GraftOutput(accumulatedcimagePtr);
  addimagefilterPtr->SetInput1(ptfilterPtr->GetOutput());
  addimagefilterPtr->SetInput2(addimagefilterPtr->GetOutput());
  
  ROIImageIteratorType ROIImageIt( roireaderPtr->GetOutput(),
    roireaderPtr->GetOutput()->GetRequestedRegion() );
  for(ROIImageIt.GoToBegin(); !ROIImageIt.IsAtEnd(); ++ROIImageIt){
    if(ROIImageIt.Get() == labelnumber){
      std::cout << "PixelIndex: "<< ROIImageIt.GetIndex() << std::endl;
      ptfilterPtr->SetSeedIndex( ROIImageIt.GetIndex() );
      addimagefilterPtr->Update();
//      break;
    }
  }        

  //Write out the Connectivity Map
  CImageWriterType::Pointer writerPtr = CImageWriterType::New();
  writerPtr->SetInput( accumulatedcimagePtr );
  writerPtr->SetFileName( cfilename );
  writerPtr->Update();
  
  return EXIT_SUCCESS;
}
