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
#include "itkTensorFractionalAnisotropyImageFilter.h"
#include "itkPathIterator.h"
#include <string>
#include <fstream>
#include "vtkPolyDataWriter.h"
#include "vtkPolyData.h"
#include "vtkCellArray.h"
#include "vtkPoints.h"

std::string stripExtension(const std::string& fileName){
  const int length = fileName.length();
  for (int i=0; i!=length; ++i){
    if (fileName[i]=='.'){
      return fileName.substr(0,i);
    }
  } 
  return fileName; 
}


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

template< class TractContainerType >
bool WriteTractContainerToFile( const char* fn, TractContainerType* tractcontainer ){
  std::ofstream tractfile( fn, std::ios::out | std::ios::binary );
  tractfile.seekp(0);
  if(tractfile.is_open()){
    for(int i=0; i<tractcontainer->Size(); i++ ){
      typename TractContainerType::Element tract =
        tractcontainer->GetElement(i);
      
      typename TractContainerType::Element::ObjectType::VertexListType::ConstPointer vertexlist = 
        tract->GetVertexList();
      
      for(int j=0; j<vertexlist->Size(); j++){
        typename TractContainerType::Element::ObjectType::VertexListType::Element vertex =
          vertexlist->GetElement(j);
        tractfile.write((char*)&vertex[0], sizeof(vertex[0]));
        tractfile.write((char*)&vertex[1], sizeof(vertex[1]));
        tractfile.write((char*)&vertex[2], sizeof(vertex[2]));
      }
      double endoftractsymbol = 0.0;
      tractfile.write((char*)&endoftractsymbol, sizeof(endoftractsymbol));
      tractfile.write((char*)&endoftractsymbol, sizeof(endoftractsymbol));
      tractfile.write((char*)&endoftractsymbol, sizeof(endoftractsymbol));
    }
    
    tractfile.close();
    return true;
  }
  else{
    std::cerr<<"Problems opening file to write tracts\n";
    return false;
  }
}
template< class FAContainerType >
bool WriteScalarContainerToFile( const char* fn, FAContainerType* facontainer ){
  std::ofstream fafile( fn, std::ios::out );
  if(fafile.is_open()){
    for(int i=0; i<facontainer->Size(); i++){
      fafile<<facontainer->GetElement(i)<<std::endl;
    }
    fafile.close();
    return true;
  }
  else{
    std::cerr<<"Problems opening file to write FA value\n";
    return false;
  }
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

template< class TInput, class TOutput>
class ScalarMultiply
{
public:
  ScalarMultiply() {};
  ~ScalarMultiply() {};
  bool operator!=( const ScalarMultiply & ) const
  {
    return false;
  }
  bool operator==( const ScalarMultiply & other ) const
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
    return A*1000;
  }
}; 

}

int main(int argc, char* argv[]){
  PARSE_ARGS;
  //define the input/output types
  typedef itk::VectorImage< unsigned short int, 3 > DWIVectorImageType;
  typedef itk::Image< float, 3 > WMPImageType;
  typedef itk::Image< short int, 3 > ROIImageType;
  typedef itk::Image< unsigned int, 3 > CImageType;

  //define an iterator for the ROI image
  typedef itk::ImageRegionConstIterator< ROIImageType > ROIImageIteratorType;
  
  //define reader and writer
  typedef itk::ImageFileReader< DWIVectorImageType > DWIVectorImageReaderType;
  typedef itk::ImageFileReader< WMPImageType > WMPImageReaderType;
  typedef itk::ImageFileReader< ROIImageType > ROIImageReaderType;
  typedef itk::ImageFileWriter< CImageType > CImageWriterType;
  typedef itk::ImageFileWriter< WMPImageType > WMPImageWriterType;
  
  //define metadata dictionary types
  typedef itk::MetaDataDictionary DictionaryType;
  typedef DictionaryType::ConstIterator DictionaryIteratorType;
    
  //define a probabilistic tractography filter type and associated bValue,
  //gradient direction, and measurement frame types
  typedef itk::StochasticTractographyFilter< DWIVectorImageType, WMPImageType,
    CImageType > 
    PTFilterType;
  typedef PTFilterType::bValueContainerType bValueContainerType;
  typedef PTFilterType::GradientDirectionContainerType GDContainerType;
  typedef PTFilterType::MeasurementFrameType MeasurementFrameType;
  
  //define a filter to generate a mask image that excludes zero dwi values
  typedef Functor::ZeroDWITest< DWIVectorImageType::PixelType, WMPImageType::PixelType >
    ZeroDWITestType;
  typedef itk::UnaryFunctorImageFilter< DWIVectorImageType, WMPImageType, 
    ZeroDWITestType > ExcludeZeroDWIFilterType;
  
  //FA stuff
  typedef itk::Image< double, 3 > FAImageType;
  typedef itk::TensorFractionalAnisotropyImageFilter< PTFilterType::OutputTensorImageType,
    FAImageType > FAFilterType;
  typedef itk::ImageFileWriter< FAImageType > FAImageWriterType;
    
  //define a filter to multiply a FA image by 1000
  typedef Functor::ScalarMultiply< FAImageType::PixelType, FAImageType::PixelType >
    ScalarMultiplyType;
  typedef itk::UnaryFunctorImageFilter< FAImageType, FAImageType,
    ScalarMultiplyType > MultiplyFAFilterType;
    
  //define AddImageFilterType to accumulate the connectivity maps of the pixels in the ROI
  typedef itk::AddImageFilter< CImageType, CImageType, CImageType> AddImageFilterType;

  //setup output stat files
  //std::string fafilename = outputprefix + "_CONDFAValues.txt";
  //sprintf( fafilename, "%s_CONDFAValues.txt", outputprefix.c_str() );
  //std::string lengthfilename = outputprefix + "_CONDLENGTHValues.txt";
  //sprintf( lengthfilename, "%s_CONDLENGTHValues.txt", outputprefix.c_str() );
  
  //open these files
  //std::ofstream fafile( fafilename.c_str() );
  //if(!fafile.is_open()){
  //  std::cerr<<"Could not open FA file!\n";
  //  return EXIT_FAILURE;
  //}
  
  //std::ofstream lengthfile( lengthfilename.c_str() );
  //if(!lengthfile.is_open()){
  //  std::cerr<<"Could not open Length file!\n";
  //  return EXIT_FAILURE;
 // }
  
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
      //normalize the gradient vector
      gradientsPtr->InsertElement( gradientsPtr->Size(), g_i.normalize() );
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
  for(unsigned int i=0; i<gradientsPtr->Size() ;i++){
    if(gradientsPtr->GetElement(i).squared_magnitude() == 0){
      bValuesPtr->InsertElement(i, 0);
    }
    else{
      //this matters in the calculations for the constrained model but not the tensor model
      //since a gradient direction of all zeros handles it
      bValuesPtr->InsertElement(i, scaling_bValue);
    }
  }
  
  //setup the ROI image reader
  ROIImageReaderType::Pointer roireaderPtr = ROIImageReaderType::New();
  roireaderPtr->SetFileName(roifilename);
  roireaderPtr->Update();
  
  //setup the white matter probability image reader
  WMPImageReaderType::Pointer wmpreader = WMPImageReaderType::New();
  wmpreader->SetFileName(wmpfilename);
  wmpreader->Update();
  
  //optionally set the origins of these images to be the same as the DWI
  if(recenteroriginswitch==true){
    roireaderPtr->GetOutput()->SetOrigin( dwireaderPtr->GetOutput()->GetOrigin() );
    wmpreader->GetOutput()->SetOrigin( dwireaderPtr->GetOutput()->GetOrigin() );
  }
 
  std::cout<<"Create PTFilter\n";
  //Setup the PTFilter
  PTFilterType::Pointer ptfilterPtr = PTFilterType::New();
  ptfilterPtr->SetInput( dwireaderPtr->GetOutput() );
  //ptfilterPtr->SetMaskImageInput( ezdwifilter->GetOutput() );
  ptfilterPtr->SetWhiteMatterProbabilityImageInput( wmpreader->GetOutput() );
  ptfilterPtr->SetbValues(bValuesPtr);
  ptfilterPtr->SetGradients( gradientsPtr );
  ptfilterPtr->SetMeasurementFrame( measurement_frame );
  //ptfilterPtr->SetSampleDirections(directionsPtr);
  ptfilterPtr->SetMaxTractLength( maxtractlength );
  ptfilterPtr->SetTotalTracts( totaltracts );
  ptfilterPtr->SetMaxLikelihoodCacheSize( maxlikelihoodcachesize );
  ptfilterPtr->SetStepSize( stepsize );
  ptfilterPtr->SetGamma( gamma );
  if(totalthreads!=0) ptfilterPtr->SetNumberOfThreads( totalthreads );
  
  //allocate the VTK Polydata to output the tracts
  vtkPolyData* vtktracts = vtkPolyData::New();
  vtkPoints* points = vtkPoints::New();
  vtkCellArray* vtktractarray = vtkCellArray::New();
  
  ROIImageIteratorType ROIImageIt( roireaderPtr->GetOutput(),
    roireaderPtr->GetOutput()->GetRequestedRegion() );
  for(ROIImageIt.GoToBegin(); !ROIImageIt.IsAtEnd(); ++ROIImageIt){
    if(ROIImageIt.Get() == labelnumber){
      std::cout << "PixelIndex: "<< ROIImageIt.GetIndex() << std::endl;
      ptfilterPtr->SetSeedIndex( ROIImageIt.GetIndex() );
      //ptfilterPtr->GenerateTractContainerOutput();
      //write out the tract info

      //WriteTractContainerToFile( filename, ptfilterPtr->GetOutputTractContainer() );
      ptfilterPtr->Update();

      PTFilterType::TractContainerType::Pointer tractcontainer = 
        ptfilterPtr->GetOutputDiscreteTractContainer();
        
      for(int i=0; i<tractcontainer->Size(); i++ ){
        PTFilterType::TractContainerType::Element tract =
          tractcontainer->GetElement(i);
        
        //std::cout<<i<<":"<<tract<<std::endl;
        PTFilterType::TractContainerType::Element::ObjectType::VertexListType::ConstPointer vertexlist = 
          tract->GetVertexList();
        
        //create a new cell
        vtktractarray->InsertNextCell(vertexlist->Size());
        //std::cout<<tract->EndOfInput()<<std::endl;
        for(int j=0; j<vertexlist->Size(); j++){
          
          PTFilterType::TractContainerType::Element::ObjectType::VertexListType::Element vertex =
            vertexlist->GetElement(j);
          
          points->InsertNextPoint(vertex[0], vertex[1], vertex[2]);
          vtktractarray->InsertCellPoint(points->GetNumberOfPoints()-1);
        }
      }
      
    }
  }        
  
  //finish up the vtk polydata
  vtktracts->SetPoints( points );
  vtktracts->SetLines( vtktractarray );
  //output the vtk tract container
  vtkPolyDataWriter* vtktractswriter = vtkPolyDataWriter::New();
  vtktractswriter->SetInput( vtktracts );
  std::string tractsfilename = outputprefix + "_TRACTS.vtk";
  vtktractswriter->SetFileName( tractsfilename.c_str() );
  vtktractswriter->Write();
  
  
  std::cout<<"DWI image origin:"<< dwireaderPtr->GetOutput()->GetOrigin() <<std::endl;
  std::cout<<"ROI image origin:"<< roireaderPtr->GetOutput()->GetOrigin() <<std::endl;
  std::cout<<"wmp image origin:"<< wmpreader->GetOutput()->GetOrigin() <<std::endl;

  return EXIT_SUCCESS;
}
