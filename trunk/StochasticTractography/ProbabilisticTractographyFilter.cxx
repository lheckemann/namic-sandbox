#include "itkProbabilisticTractographyFilter.h"
#include "itkVectorImage.h"
#include "itkImageSeriesReader.h"  //this is needed for itk::ExposeMetaData()
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkMetaDataDictionary.h"
#include <iostream>
#include <vector>
#include "ProbabilisticTractographyFilterCLP.h"

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

int main(int argc, char* argv[]){
  PARSE_ARGS;
  //define the input/output types
  typedef itk::VectorImage< unsigned short int, 3 > DWIVectorImageType;
  typedef itk::Image< short int, 3 > ROIImageType;
  typedef itk::Image< unsigned int, 3 > CImageType;
  
  //define reader and writer
  typedef itk::ImageFileReader< DWIVectorImageType > DWIVectorImageReaderType;
  typedef itk::ImageFileReader< ROIImageType > ROIImageReaderType;
  typedef itk::ImageFileWriter< CImageType > CImageWriterType;
  
  //define metadata dictionary types
  typedef itk::MetaDataDictionary DictionaryType;
  typedef DictionaryType::ConstIterator DictionaryIteratorType;

  //define a probabilistic tractography filter type and associated bValue, gradient direction
  //and measurement frame types
  typedef itk::ProbabilisticTractographyFilter< DWIVectorImageType, ROIImageType, 
    CImageType > PTFilterType;
  typedef PTFilterType::bValueContainerType bValueContainerType;
  typedef PTFilterType::GradientDirectionContainerType GDContainerType;
  typedef PTFilterType::MeasurementFrameType MeasurementFrameType;
  
  //parse command line arguments
  if(argc < 3){
    std::cerr << "Usage: " << argv[0];
    std::cerr<< " DWIFile(.nhdr) ROIFile(.nhdr) ConnectivityFile(.nhdr) ";
    std::cerr<< "NumberOfTracts MaxTractLength\n";
    return EXIT_FAILURE;
  }
  //char* dwifilename = argv[1];
  //char* roifilename = argv[2];
  //char* cfilename = argv[3];
  //unsigned int totaltracts = atoi(argv[4]);
  //unsigned int maxtractlength = atoi(argv[5]);
  
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
  
  //set list of directions
  typedef PTFilterType::TractOrientationContainerType TOCType;
  TOCType::Pointer directionsPtr = TOCType::New();
  
  if(SamplingDirections<TOCType>("SD.txt", directionsPtr));
  else return EXIT_FAILURE;
  
   //Setup the PTFilter
  PTFilterType::Pointer ptfilterPtr = PTFilterType::New();
  
  ptfilterPtr->SetDWIImageInput( dwireaderPtr->GetOutput() );
  ptfilterPtr->SetROIImageInput( roireaderPtr->GetOutput() );
  ptfilterPtr->SetbValues(bValuesPtr);
  ptfilterPtr->SetGradients( gradientsPtr );
  ptfilterPtr->SetMeasurementFrame( measurement_frame );
  ptfilterPtr->SetSampleDirections(directionsPtr);
  ptfilterPtr->SetMaxTractLength( maxtractlength );
  ptfilterPtr->SetTotalTracts( totaltracts );
  
  //Write out the Tensor Image
  CImageWriterType::Pointer writerPtr = CImageWriterType::New();
  writerPtr->SetInput( ptfilterPtr->GetOutput() );
  writerPtr->SetFileName( cfilename );
  writerPtr->Update();
  
  return EXIT_SUCCESS;
}
