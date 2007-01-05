#ifndef SVMTestingUtils_H
#define SVMTestingUtils_H

#include <list>
#include <string>
#include <itkMembershipSample.h>
#include <itkListSample.h>
#include <itkMetaArrayReader.h>
#include <itkMetaArrayWriter.h>
#include <itkVector.h>
#include <itkVariableLengthVector.h>
#include <itkArray.h>
#include <itkFixedArray.h>
#include "itkSupportVectorMachine.h"
#include "itkSVMSolver1.h"
#include "itkSVMFileReader.h"
#include "itkSVMFileWriter.h"

/**
 * This class provides a number of utilities for writing tests for the
 * SVM related classes.  These utilities are mainly: type definitions,
 * input and output funcions, output level, specialized parsing of
 * command line options.
 *
 */

class SVMTestingUtils 
{
public:
  //
  // typedefs
  //

  /* class labels */
  typedef unsigned int                               ClassLabelType;  
  static const ClassLabelType                        Class1Label;
  static const ClassLabelType                        Class2Label;

  /* measurement vector */
  typedef float                                      ValueType;
  static const MET_ValueEnumType                     METAValueType;
  typedef itk::VariableLengthVector<ValueType>       MeasurementVectorType;  
  typedef itk::MetaArrayReader                       VectorReaderType;
  typedef itk::MetaArrayWriter                       VectorWriterType;

  /* filenames */
  typedef std::list<std::string>                     FilenameList;
  typedef std::list<std::string>::const_iterator     FilenameListConstIter;

  /* samples */
  typedef itk::Statistics::ListSample<MeasurementVectorType> SampleType;
  typedef itk::Statistics::MembershipSample<SampleType>   MembershipSampleType;

  /* svm and svm kernel */
  typedef itk::Statistics::
  SupportVectorMachine<MeasurementVectorType>        SVMType;  
  typedef itk::Statistics::SVMKernelBase<MeasurementVectorType, double>
  SVMKernelType;

  /* svm solver */
  typedef itk::Statistics::
  SVMSolver1<MembershipSampleType::ClassSampleType,
             MembershipSampleType::ClassSampleType>
  SVMSolverType;

  /* svm IO */
  typedef itk::VariableLengthVector<double>          TargetVectorType;
  typedef itk::SVMFileReader<MeasurementVectorType, TargetVectorType>
  SVMReaderType;
  typedef itk::SVMFileWriter<MeasurementVectorType, TargetVectorType>
  SVMWriterType;
  
  //
  // output level mechanism
  //
  enum OutputLevel { Silent, Standard, Verbose, Everything };
  static OutputLevel GetOutputLevel()
  {
    return outputLevel;
  }

  static void SetOutputLevel(const OutputLevel& outputLevelIn)
  {
    outputLevel = outputLevelIn;
  }
  
  static void SetOutputLevel(const std::string& levelString);

  static bool OutputLevelAtLeast(const OutputLevel& outputLevelIn)
  {
    return outputLevel >= outputLevelIn;
  }

  //
  // utility functions
  //
  static void
  LoadMeasurementVectorFromImageFile(const std::string& filename,
                                     MeasurementVectorType& mv);

  static void
  LoadMeasurementVectorFromVectorFile(const std::string& filename,
                                      MeasurementVectorType& mv);

  static void
  SaveMeasurementVectorToVectorFile(const std::string& filename,
                                    const MeasurementVectorType& mv,
                                    bool writeDataAsBinary = true);

  static void
  LoadClassifiedVectors(const FilenameList& class1VectorFilenames,
                        const FilenameList& class2VectorFilenames,
                        SampleType::Pointer sample,
                        MembershipSampleType::Pointer membershipSample);

  static void
  LoadUnclassifiedVectors(const FilenameList& vectorFilenames,
                          SampleType::Pointer sample);

  static bool
  ParseSVMKernel(SVMType::Pointer svm, 
                 const std::string& kernelTypeString,
                 const std::vector<std::string>& kernelParamString);

private:
  static OutputLevel outputLevel;  
};

#endif // SVMTestingUtils_H
