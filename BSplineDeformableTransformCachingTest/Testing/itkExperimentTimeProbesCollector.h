
#ifndef ExperimentTimeProbesCollector_h
#define ExperimentTimeProbesCollector_h

#include "itkTimeProbesCollectorBase.h"

#include <string>

namespace itk
{
class ExperimentTimeProbesCollector : public TimeProbesCollectorBase
{
public:
  
  ExperimentTimeProbesCollector();

  ~ExperimentTimeProbesCollector();

  /** Report the summary of results from the time probes */
  virtual void Report( std::ostream & os = std::cout ) const;

  void SetExperimentString( const std::string& );
  void SetDelimeterString( const std::string& );

protected:
  void WriteReportHeader( std::ostream & os = std::cout ) const;
  void WriteReportBody  ( std::ostream & os ) const;

  typedef std::string StringType;
  StringType m_ExperimentString;
  StringType m_Delimeter;
};
}

#endif // ExperimentTimeProbesCollector_h

