#include "SVMTestingUtils.h"
#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkMetaArrayWriter.h>
#include <itkIOCommon.h>
#include "itkLinearSVMKernel.h"
#include "itkRBFSVMKernel.h"
#include "itkPolynomialSVMKernel.h"

const SVMTestingUtils::ClassLabelType 
SVMTestingUtils::Class1Label = 0;

const SVMTestingUtils::ClassLabelType
SVMTestingUtils::Class2Label = 1;

const MET_ValueEnumType
SVMTestingUtils::METAValueType = MET_FLOAT;

SVMTestingUtils::OutputLevel SVMTestingUtils::outputLevel = 
SVMTestingUtils::Standard;

void
SVMTestingUtils::
SetOutputLevel(const std::string& outputLevelString)
{
  if (outputLevelString == std::string("silent")) 
  {
    outputLevel = Silent;
  }
  else if (outputLevelString == std::string("standard"))
  {
    outputLevel = Standard;
  }
  else if (outputLevelString == std::string("verbose")) 
  {
    outputLevel = Verbose;
  }
  else if (outputLevelString == std::string("everything")) 
  {
    outputLevel = Everything;
  }
}

//
// Overloaded functions for copying data to and from the META vector
// IO to the measurment vector.  We use overloading since the
// different measurement vector types have different interfaces.
// These functions can't be member functions because MS Visual Studio
// will not compile them.
//
template <class T>
void
GetDataFromMETA(SVMTestingUtils::VectorReaderType::Pointer& vectorReader,
                itk::VariableLengthVector<T>& mv)
{
  vectorReader->
    GetOutput(SVMTestingUtils::METAValueType, &mv, true); 
}

template <class T>
void
GetDataFromMETA(SVMTestingUtils::VectorReaderType::Pointer& vectorReader,
                itk::Array<T>& mv)
{
  vectorReader->GetOutput(SVMTestingUtils::METAValueType, &mv, true);  
}

template <class T, int I>
void
GetDataFromMETA(SVMTestingUtils::VectorReaderType::Pointer& vectorReader,
                itk::Vector<T, I>& mv)
{
  vectorReader->GetOutput(SVMTestingUtils::METAValueType, &mv);  
}

template <class T, int I>
void
GetDataFromMETA(SVMTestingUtils::VectorReaderType::Pointer& vectorReader,
                itk::FixedArray<T, I>& mv)
{
  vectorReader->GetOutput(SVMTestingUtils::METAValueType, &mv);  
}

void
SVMTestingUtils::
LoadMeasurementVectorFromVectorFile(const std::string& filename,
                                    MeasurementVectorType& mv)
{
  //
  // load the vector
  //
  VectorReaderType::Pointer vectorReader = VectorReaderType::New();
  vectorReader->SetFileName(filename.c_str());
  vectorReader->Update();

  //
  // If a variable length measurement vector type is used, then steal
  // the data buffer from the META reader and put it in a vector.  The
  // reader will stop managing the data buffer memory the vector will
  // start managing the data buffer memory.  If a fixed length
  // measurement vector type is used, the data will be copied over.
  // Because the different types of measurement vectors have different
  // interfaces, we use function overloading to decide how to actually
  // get the data.
  //
  GetDataFromMETA(vectorReader, mv);
}

void
SVMTestingUtils::
SaveMeasurementVectorToVectorFile(const std::string& filename,
                                  const MeasurementVectorType& mv,
                                  bool writeDataAsBinary)
{
  //
  // setup the file writer
  //
  VectorWriterType::Pointer vectorWriter = VectorWriterType::New();
  vectorWriter->SetBinary(writeDataAsBinary);
  vectorWriter->SetFileName(filename.c_str());
  vectorWriter->SetInput(METAValueType, &mv);
  vectorWriter->Update();
}

void
SVMTestingUtils::
LoadMeasurementVectorFromImageFile(const std::string& filename,
                                   MeasurementVectorType& mv)
{
  //
  // load the image
  //
  typedef itk::Image<ValueType,2>                    ImageType;
  typedef ImageType::Pointer                ImagePointer;
  typedef itk::ImageFileReader<ImageType>            ImageReaderType;
  ImageReaderType::Pointer imageReader =    ImageReaderType::New();
  imageReader->SetFileName(filename.c_str());
  imageReader->Update();

  //
  // Steal the data buffer from the image and put it in a
  // vector.  The image will stop managing the data buffer memory the
  // vector will start managing the data buffer memory.
  //
  ImageType::Pointer image = imageReader->GetOutput();
  image->GetPixelContainer()->ContainerManageMemoryOff();
  bool letVectorManageData = true;
  mv.SetData(image->GetBufferPointer(), 
             image->GetLargestPossibleRegion().GetNumberOfPixels(), 
             letVectorManageData);
}

void
SVMTestingUtils::
LoadClassifiedVectors(const FilenameList& class1VectorFilenames,
                      const FilenameList& class2VectorFilenames,
                      SampleType::Pointer sample,
                      MembershipSampleType::Pointer membershipSample)
{
  unsigned int measurementVectorSize = 0;
  bool alreadyHaveFirstSize = false;

  //
  // add class 1 vectors to sample
  //
  std::cerr << "Loading class 1 measurement vectors";
  SampleType::InstanceIdentifier instanceID = 0;
  SampleType::MeasurementVectorType mv;
  typedef std::list<std::string>::const_iterator FilenameIter;
  for (FilenameIter iter = class1VectorFilenames.begin();
       iter != class1VectorFilenames.end();
       ++iter)
  {
    std::cerr << "." << instanceID;

    SVMTestingUtils::LoadMeasurementVectorFromVectorFile(*iter, mv);

    // test for consistent vector size
    if (alreadyHaveFirstSize)
    {
      unsigned int newMeasurementVectorSize = mv.Size();
      if (newMeasurementVectorSize != measurementVectorSize)
      {
        std::cerr << "Inconsistent measurment vector size; "
                  << "expected " << measurementVectorSize
                  << ", found " << newMeasurementVectorSize
                  << "; file: " << *iter << std::endl;
        exit(0);
      }
    }
    else
    {
      measurementVectorSize = mv.Size();
      sample->SetMeasurementVectorSize(measurementVectorSize);
      membershipSample->SetSample(sample);
      membershipSample->SetNumberOfClasses(2);
      alreadyHaveFirstSize = true;
    }

    // this makes a copy!!! of the data pointed to by the vector
    sample->PushBack(mv);
    membershipSample->AddInstance(Class1Label, instanceID++);
  }
  std::cerr << std::endl;

  //
  // add class 2 vectors to sample
  //
  std::cerr << "Loading class 2 measurement vectors";
  for (FilenameIter iter = class2VectorFilenames.begin();
       iter != class2VectorFilenames.end();
       ++iter)
  {
    std::cerr << "." << instanceID;

    SVMTestingUtils::LoadMeasurementVectorFromVectorFile(*iter, mv);

    // test for consistent vector size
    unsigned int newMeasurementVectorSize = mv.Size();
    if (newMeasurementVectorSize != measurementVectorSize)
    {
      std::cerr << "Inconsistent measurment vector size; "
                << "expected " << measurementVectorSize
                << ", found " << newMeasurementVectorSize
                << "; file: " << *iter << std::endl;
      exit(0);
    }

    sample->PushBack(mv);
    membershipSample->AddInstance(Class2Label, instanceID++);
  }
    std::cerr << std::endl;
}

void
SVMTestingUtils::
LoadUnclassifiedVectors(const FilenameList& vectorFilenames,
                        SampleType::Pointer sample)
{
  unsigned int measurementVectorSize = 0;
  bool alreadyHaveFirstSize = false;

  //
  // add class vectors to sample
  //
  std::cerr << "Loading measurement vectors";
  SampleType::InstanceIdentifier instanceID = 0;
  SampleType::MeasurementVectorType mv;
  typedef std::list<std::string>::const_iterator FilenameIter;
  for (FilenameIter iter = vectorFilenames.begin();
       iter != vectorFilenames.end();
       ++iter)
  {
    std::cerr << "." << instanceID++;

    SVMTestingUtils::LoadMeasurementVectorFromVectorFile(*iter, mv);

    // test for consistent vector size
    if (alreadyHaveFirstSize)
    {
      unsigned int newMeasurementVectorSize = mv.Size();
      if (newMeasurementVectorSize != measurementVectorSize)
      {
        std::cerr << "Inconsistent measurment vector size; "
                  << "expected " << measurementVectorSize
                  << ", found " << newMeasurementVectorSize
                  << "; file: " << *iter << std::endl;
        exit(0);
      }
    }
    else
    {
      measurementVectorSize = mv.Size();
      sample->SetMeasurementVectorSize(measurementVectorSize);
      alreadyHaveFirstSize = true;
    }

    // this makes a copy!!! of the data pointed to by the vector
    sample->PushBack(mv);
  }
  std::cerr << std::endl;
}

bool
SVMTestingUtils::
ParseSVMKernel(SVMType::Pointer svm, 
               const std::string& kernelTypeString,
               const std::vector<std::string>& kernelParameters)
{
  if (kernelTypeString == "linear")
  {
    typedef itk::Statistics::LinearSVMKernel<MeasurementVectorType,double>
      LinearKernelType;
    svm->SetKernel(LinearKernelType::New());
    return true;
  }
  else if (kernelTypeString == "polynomial")
  {
    typedef itk::Statistics::
      PolynomialSVMKernel<MeasurementVectorType,double> PolynomialKernelType;
    PolynomialKernelType::Pointer polynomialKernel = 
      PolynomialKernelType::New();

    if (kernelParameters.size() != 3)
    {
      return false;
    }
    polynomialKernel->SetGamma(atof(kernelParameters[0].c_str()));
    polynomialKernel->SetR(atof(kernelParameters[1].c_str()));
    polynomialKernel->SetPower(atof(kernelParameters[2].c_str()));
    svm->SetKernel(polynomialKernel);
    return true;
  }
  else if (kernelTypeString == "rbf")
  {
    typedef itk::Statistics::RBFSVMKernel<MeasurementVectorType,double>
      RBFKernelType;
    RBFKernelType::Pointer rbfKernel = RBFKernelType::New();

    if (kernelParameters.size() != 1)
    {
      return false;
    }
    rbfKernel->SetGamma(atof(kernelParameters[0].c_str()));
    svm->SetKernel(rbfKernel);
    return true;
  }
  return false;
}
