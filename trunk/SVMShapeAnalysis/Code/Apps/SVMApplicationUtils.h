#ifndef SVMApplicationUtils_H
#define SVMApplicationUtils_H

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
 * SVMApplicationUtils contains a number of common utilites for
 * writing applications that use the SVM and related classes.  These
 * utilities include: type definitions, input and output functions,
 * output level mechanism, and specialized parsing of command line
 * options.
 *
 *
 */
class SVMApplicationUtils 
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
  // checking to see if fixed length vectors work...
  // nb: fixed length vectors are NOT recommended
  //typedef itk::Vector<ValueType, 240>                MeasurementVectorType;  
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

  /** the output level determines how much is printed to standard
   *   error
   */
  enum OutputLevel { Silent, Standard, Verbose, Everything };

  /** get output level */
  static OutputLevel GetOutputLevel()
  {
    return outputLevel;
  }

  /** set output level */
  static void SetOutputLevel(const OutputLevel& outputLevelIn)
  {
    outputLevel = outputLevelIn;
  }
  
  /** set output level from command line argument string */
  static void SetOutputLevel(const std::string& levelString);

  /** check to see if the current output level is at least at a
   * certain level.
   */
  static bool OutputLevelAtLeast(const OutputLevel& outputLevelIn) 
  { return outputLevel >= outputLevelIn; }

  //
  // utility functions
  //

  /** load an image as a vector */
  static void
  LoadMeasurementVectorFromImageFile(const std::string& filename,
                                     MeasurementVectorType& mv);

  /** load a vector */
  static void
  LoadMeasurementVectorFromVectorFile(const std::string& filename,
                                      MeasurementVectorType& mv);

  /** write a vector */
  static void
  SaveMeasurementVectorToVectorFile(const std::string& filename,
                                    const MeasurementVectorType& mv,
                                    bool writeDataAsBinary = true);

  /** load a collection of labeled vectors into a MembershipSample */
  static void
  LoadClassifiedVectors(const FilenameList& class1VectorFilenames,
                        const FilenameList& class2VectorFilenames,
                        SampleType::Pointer sample,
                        MembershipSampleType::Pointer membershipSample);

  /** load a collection of vectors into a Sample */
  static void
  LoadUnclassifiedVectors(const FilenameList& vectorFilenames,
                          SampleType::Pointer sample);

  /** parse SVM kerenel from command line arguments */
  static bool
  ParseSVMKernel(const SVMType::Pointer svm, 
                 const std::string& kernelTypeString,
                 const std::vector<std::string>& kernelParamString);

  /** get fileneame (including extension) from a fully qualified
   *  filename
   */
  static std::string
  ExtractFilename(const std::string& fullyQualifiedFilename);

private:
  /** current output level for an application */
  static OutputLevel outputLevel;  
};

#endif // SVMApplicationUtils_H
