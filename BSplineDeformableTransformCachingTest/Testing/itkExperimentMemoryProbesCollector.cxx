
#include "itkExperimentMemoryProbesCollector.h"

namespace itk
{

ExperimentMemoryProbesCollector
::ExperimentMemoryProbesCollector() : m_ExperimentString(" "), m_Delimeter("\t")
{
}

ExperimentMemoryProbesCollector
::~ExperimentMemoryProbesCollector()
{

}

void 
ExperimentMemoryProbesCollector::
WriteReportHeader( std::ostream & os ) const
{
  unsigned int experimentWidth = m_ExperimentString.length()+5;
  os << "#";
  os.width(experimentWidth);
  os <<  " Experiment name " << m_Delimeter;
  os.width(20);
  os <<  " Probe Tag " << m_Delimeter;
  os.width(10);
  os <<  " Starts " << m_Delimeter;
  os.width(10);
  os <<  " Stops  " << m_Delimeter;
  os.width(15);
  os <<  "  Memory (kB) " << m_Delimeter;
  os << std::endl;
}

void 
ExperimentMemoryProbesCollector::
WriteReportBody( std::ostream & os ) const
{
  MapType::const_iterator probe = m_Probes.begin();
  MapType::const_iterator end   = m_Probes.end();

  unsigned int experimentWidth = m_ExperimentString.length()+5;

  while( probe != end )
    {
    os.width(experimentWidth);
    os << m_ExperimentString << m_Delimeter;
    os.width(20);
    os <<  probe->first.c_str() << m_Delimeter;
    os.width(10);
    os <<  probe->second.GetNumberOfStarts() << m_Delimeter;
    os.width(10);
    os <<  probe->second.GetNumberOfStops() << m_Delimeter;
    os.width(15);
    os <<  probe->second.GetMeanMemoryChange();
    os << std::endl;
    probe++;
    }
}

void 
ExperimentMemoryProbesCollector::
Report( std::ostream & os ) const
{
  this->WriteReportHeader( os );
  this->WriteReportBody  ( os );
}

void 
ExperimentMemoryProbesCollector::
SetExperimentString( const std::string& inString )
{
  m_ExperimentString = inString;
}

void 
ExperimentMemoryProbesCollector::
SetDelimeterString( const std::string& delim )
{
  m_Delimeter = delim;
}

} // end namespace itk

