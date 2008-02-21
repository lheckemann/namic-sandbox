
#ifndef ExperimentMemoryProbesCollector_h
#define ExperimentMemoryProbesCollector_h

#include "MemoryUsage.h"

#include <string>

namespace itk
{
class ExperimentMemoryProbesCollector : public MemoryUseCollector
{
public:
  
  ExperimentMemoryProbesCollector();

  ~ExperimentMemoryProbesCollector();

  /** Report the summary of results from the time probes */
  virtual void Report( std::ostream & os = std::cout ) const;

  void SetExperimentString( std::string& );
  void SetDelimeterString( std::string& );

protected:
  void WriteReportHeader( std::ostream & os = std::cout ) const;
  void WriteReportBody  ( std::ostream & os ) const;

  typedef std::string StringType;
  StringType m_ExperimentString;
  StringType m_Delimeter;
};
}

#endif // ExperimentMemoryProbesCollector_h

