#include "itkTensorModelFilter.h"
#include "itkVectorImage.h"
#include "itkDiffusionTensor3D.h"
#include "itkImageSeriesReader.h"  //this is needed for itk::ExposeMetaData()
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkMetaDataDictionary.h"
#include <iostream>
#include <vector>

int main(int argc, char* argv[]){
  //parse command line arguments
  if(argc < 3){
    std::cerr << "Usage: " << argv[0] << " DWIFile(.nhdr) TensorFile(.nhdr)\n";
    return EXIT_FAILURE;
  }
  char* dwifilename = argv[1];
  char* dtfilename = argv[2];
  
  //define the input/output types
  typedef itk::VectorImage< unsigned short int, 3 > DWIVectorImageType;
  typedef itk::Image< itk::DiffusionTensor3D< float >, 3> DTImageType;
  
  //define reader and writer
  typedef itk::ImageFileReader< DWIVectorImageType > DWIVectorImageReaderType;
  typedef itk::ImageFileWriter< DTImageType > DTImageWriterType;
  
  //define metadata dictionary types
  typedef itk::MetaDataDictionary DictionaryType;
  typedef DictionaryType::ConstIterator DictionaryIteratorType;

  //define a tensormodelfilter type and associated bValue, gradient direction
  //and measurement frame types
  typedef itk::TensorModelFilter< DWIVectorImageType, DTImageType > TMFilterType;
  typedef TMFilterType::bValueContainerType bValueContainerType;
  typedef TMFilterType::GradientDirectionContainerType GDContainerType;
  typedef TMFilterType::MeasurementFrameType MeasurementFrameType;
  
  //read in the DWI image
  DWIVectorImageReaderType::Pointer readerPtr = DWIVectorImageReaderType::New();
  readerPtr->SetFileName(dwifilename);
  readerPtr->Update();

  //Obtain bValue, gradient directions and measurement frame from metadata dictionary
  DictionaryType& dictionary = readerPtr->GetMetaDataDictionary();
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
  
   //Setup the TMFilter
  TMFilterType::Pointer tmfilterPtr = TMFilterType::New();
  
  tmfilterPtr->SetInput( readerPtr->GetOutput() );
  tmfilterPtr->SetbValues(bValuesPtr);
  tmfilterPtr->SetGradients( gradientsPtr );
  tmfilterPtr->SetMeasurementFrame( measurement_frame );
  
  //Write out the Tensor Image
  DTImageWriterType::Pointer writerPtr = DTImageWriterType::New();
  writerPtr->SetInput( tmfilterPtr->GetOutput() );
  writerPtr->SetFileName( dtfilename );
  writerPtr->Update();
  
  return EXIT_SUCCESS;
}
