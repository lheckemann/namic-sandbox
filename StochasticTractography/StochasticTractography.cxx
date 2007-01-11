#include "itkVectorImage.h"
#include "itkNrrdImageIO.h"
#include "itkMetaDataDictionary.h"
#include "itkImageSeriesReader.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIterator.h"
#include "itkDiffusionTensor3D.h"
#include "itkImageAdaptor.h"
#include <iostream>

#include "itkProbabilisticTractographyFilter.h"
#include "vnl/vnl_vector.h"

template< class TInternalType, class TExternalType >
class CastPixelAccessor{
public:
  typedef TInternalType InternalType;
  typedef TExternalType ExternalType;
  
  static void Set(InternalType& output, const ExternalType& input){
    output = static_cast< InternalType >(input);
  }
  static ExternalType Get( const InternalType& input){
    return static_cast< ExternalType >(input);
  }
};

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
int main( int argc, char *argv[] ){
  if(argc < 3){
    std::cerr << "Usage: " << argv[0] << " NrrdFileName(.nhdr) "
    << " SeedPointXcoor SeedPointYcorr SeedPointZcorr ConnectivityFilename " 
    <<  "TotalTracts MaxTractLength  " << std::endl;
    std::cerr << "\tExample args: xt_dwi.nhdr 10 10 10 CImage.nhdr 100 100" << std::endl;
    return EXIT_FAILURE;
  }

  const unsigned int Dimension = 3;
  unsigned int numberOfImages = 0;
  bool readb0 = false;
  double b0 = 0;
  
    //define DWIVectorImageType
  typedef unsigned short int  PixelType;
  typedef itk::VectorImage<PixelType, 3> DWIVectorImageType;
  typedef DWIVectorImageType::PointType PointType;
  
  //define desired double image type
  typedef itk::VectorImage< double, 3> DoubleVectorImageType;
  
  //define image adaptor type
  typedef itk::ImageAdaptor< DWIVectorImageType, 
      CastPixelAccessor< DWIVectorImageType::PixelType, DoubleVectorImageType::PixelType> > 
        ImageAdaptorType;

    //define connectivity image
  typedef itk::Image< unsigned int, 3 > ConnectivityImageType;
  //define ProbabilisticTractographyFilterType
  typedef itk::ProbabilisticTractographyFilter< ImageAdaptorType, ConnectivityImageType >
    PTFilterType;
  typedef PTFilterType::Pointer PTFilterPointer;
  //define related gradient direction and container types
  typedef PTFilterType::GradientDirectionType 
    GradientDirectionType;
  typedef PTFilterType::GDContainerType
    GradientDirectionContainerType;
  //define bvalues and container types
  typedef PTFilterType::bValueContainerType bValueContainerType;
  typedef bValueContainerType::Element bValueType;
  
  ImageAdaptorType::Pointer adaptor = ImageAdaptorType::New();
  
  itk::ImageFileReader<DWIVectorImageType>::Pointer reader 
    = itk::ImageFileReader<DWIVectorImageType>::New();

  DWIVectorImageType::Pointer shortimg;
  shortimg= reader->GetOutput();
  adaptor->SetImage( shortimg );
  
    /** Parse Arguments **/
  char* dwifilename = argv[1];
  PointType seedpoint;
  seedpoint[0]=atof(argv[2]);
  seedpoint[1]=atof(argv[3]);
  seedpoint[2]=atof(argv[4]);
  char* cimagefilename = argv[5];
  unsigned int totaltracts = atoi(argv[6]);
  unsigned int maxtractlength = atoi(argv[7]);
  
  // Set the properties for NrrdReader
  reader->SetFileName(dwifilename);
  reader->Update();
  // Read in the nrrd data. The file contains the reference image and the gradient
  // images.
  //try{
  //  adaptor->Update();
  //}
  //catch (itk::ExceptionObject &ex){
  //  std::cout << ex << std::endl;
  //  return EXIT_FAILURE;
  //}

  // -------------------------------------------------------------------------
  // Parse the Nrrd headers to get the B value and the gradient directions used
  // for diffusion weighting. 
  // 
  // The Nrrd headers should look like :
  // The tags specify the B value and the gradient directions. If gradient 
  // directions are (0,0,0), it indicates that it is a reference image. 
  //
  // DWMRI_b-value:=800
  // DWMRI_gradient_0000:= 0 0 0
  // DWMRI_gradient_0001:=-1.000000       0.000000        0.000000
  // DWMRI_gradient_0002:=-0.166000       0.986000        0.000000
  // DWMRI_gradient_0003:=0.110000        0.664000        0.740000
  // ...
  // 
  itk::MetaDataDictionary imgMetaDictionary = shortimg->GetMetaDataDictionary();    
  std::vector<std::string> imgMetaKeys = imgMetaDictionary.GetKeys();
  std::vector<std::string>::const_iterator itKey = imgMetaKeys.begin();
  std::string metaString;
  
  GradientDirectionType vect3d;
  GradientDirectionContainerType::Pointer DiffusionVectors = 
    GradientDirectionContainerType::New();
  
  //fully parse the NRRD header extract diffusion vectors
  //have to put DiffusionVectors into the itk:Vectors instead of vnl vectors
  //use marc's code for this later
  
  for (; itKey != imgMetaKeys.end(); itKey ++){
    double x,y,z;

    itk::ExposeMetaData<std::string> (imgMetaDictionary, *itKey, metaString);
    if (itKey->find("DWMRI_gradient") != std::string::npos){ 
      std::cout << *itKey << " ---> " << metaString << std::endl;      
      sscanf(metaString.c_str(), "%lf %lf %lf\n", &x, &y, &z);
      vect3d[0] = x; vect3d[1] = y; vect3d[2] = z;

      DiffusionVectors->InsertElement( numberOfImages, vect3d );
      ++numberOfImages;
    }
    else if (itKey->find("DWMRI_b-value") != std::string::npos){
      std::cout << *itKey << " ---> " << metaString << std::endl;      
      readb0 = true;
      b0 = atof(metaString.c_str());
    }
  }
  std::cout << "Number Of Gradient images: " << numberOfImages << std::endl;
  if(!readb0){
    std::cerr << "BValue not specified in header file" << std::endl;
    return EXIT_FAILURE;
  }

  /*
  std::cout<<"Writing DWI File: "<< cimagefilename <<std::endl;
  typedef itk::ImageFileWriter< DWIVectorImageType > ConnectivityWriterType;
  ConnectivityWriterType::Pointer cimageWriter = ConnectivityWriterType::New();
  cimageWriter->SetFileName( cimagefilename );
  cimageWriter->SetInput( reader->GetOutput() );
  cimageWriter->Update();
  */
  
  //instantiate probabilisticfilter
  PTFilterPointer ptFilterPtr = PTFilterType::New();

  //create an adaptor to cast the image to doubles
  
  //load in DWI images
  ptFilterPtr->SetInput(adaptor);

  //set gradients
  ptFilterPtr->SetGradients( DiffusionVectors );

  //set bValues
  bValueContainerType::Pointer bValues = bValueContainerType::New();
  for(unsigned int i=0; i<DiffusionVectors->Size() ;i++)
    bValues->InsertElement(i, b0);
  
  //the first b value is zero for the non diffusion weighted image
  //this matters in the calculations for the constrained model but not the tensor model
  //since a gradient direction of all zeros handles it
  bValues->SetElement(0,0.0);
  
  ptFilterPtr->SetbValues(bValues);
  
  //set list of directions
  PTFilterType::TOContainerPointer directions = PTFilterType::TOContainerType::New();
  
  if(SamplingDirections<PTFilterType::TOContainerType>
    ("SD.txt", directions));
  else return EXIT_FAILURE;
  
  ptFilterPtr->SetSampleDirections(directions);
  
  //set seedpoint
  ptFilterPtr->SetSeedPoint(seedpoint);

  //set maxtractlength
  ptFilterPtr->SetMaxTractLength( maxtractlength );
  
  //set totaltracts
  ptFilterPtr->SetTotalTracts( totaltracts );

  std::cout<<"Writing Connectivity File: "<< cimagefilename <<std::endl;
  typedef itk::ImageFileWriter< ConnectivityImageType > ConnectivityWriterType;
  ConnectivityWriterType::Pointer cimageWriter = ConnectivityWriterType::New();
  cimageWriter->SetFileName( cimagefilename );
  cimageWriter->SetInput( ptFilterPtr->GetOutput() );
  cimageWriter->Update();

  return EXIT_SUCCESS;
}

