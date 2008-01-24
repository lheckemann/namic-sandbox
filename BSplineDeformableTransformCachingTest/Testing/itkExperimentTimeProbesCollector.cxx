
#include "itkExperimentTimeProbesCollector.h"

namespace itk
{

ExperimentTimeProbesCollector
::ExperimentTimeProbesCollector() : m_ExperimentString(" "), m_Delimeter("\t")
{
}

ExperimentTimeProbesCollector
::~ExperimentTimeProbesCollector()
{

}

void 
ExperimentTimeProbesCollector::
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
  os <<  "  Time  " << m_Delimeter;
  os << std::endl;
}

void 
ExperimentTimeProbesCollector::
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
    os <<  probe->second.GetMeanTime();
    os << std::endl;
    probe++;
    }
}

void 
ExperimentTimeProbesCollector::
Report( std::ostream & os ) const
{
  this->WriteReportHeader( os );
  this->WriteReportBody  ( os );
}

void 
ExperimentTimeProbesCollector::
SetExperimentString( std::string& inString )
{
  m_ExperimentString = inString;
}

void 
ExperimentTimeProbesCollector::
SetDelimeterString( std::string& delim )
{
  m_Delimeter = delim;
}

} // end namespace itk
