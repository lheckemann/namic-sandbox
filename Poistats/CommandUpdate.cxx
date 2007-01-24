#include <iostream>
#include <string>

// I need to include this for ofstream to work for some reason...
#include <itkImageSeriesReader.h>

#include "CommandUpdate.h"

#include "PoistatsEvents.h"

void
CommandUpdate::Execute( itk::Object *caller, const itk::EventObject & event ){
  Execute( (const itk::Object *) caller, event);
}

void 
CommandUpdate::Execute(const itk::Object * object, 
  const itk::EventObject & event){

  if( itk::IterationEvent().CheckEvent( &event ) ) {
    
    PoistatsFilterPointer filter =
      dynamic_cast< PoistatsFilterPointer >( object );
      
    const int currentIteration = filter->GetCurrentIteration();
    const int currentLull = filter->GetCurrentLull();
    const double energyDifference = filter->GetCurrentEnergyDifference();
    const double meanOfEnergies = filter->GetCurrentMeanOfEnergies();
    const double minOfEnergies = filter->GetCurrentMinOfEnergies();
    const double globalMin = filter->GetGlobalMinEnergy();
    const int exchanges = filter->GetExchanges();
    const double elapsedTime = filter->GetElapsedTime();
    
//    std::string message = currentIteration + "   lull: " + currentLull + "  denergy: " + energyDifference + "  mean: " + meanOfEnergies + "  min: " + minOfEnergies + "  global min: " +  globalMin + "  exchs: " + exchanges + "  time: " + elapsedTime + "\n";
    
    std::ostringstream output;
    output << currentIteration << "   lull: " << currentLull << "  denergy: " << energyDifference << "  mean: " << meanOfEnergies << "  min: " << minOfEnergies << "  global min: " <<  globalMin << "  exchs: " << exchanges << "  time: " << elapsedTime << "\n";
    
    PostMessage( output.str() );
    
  } else if( itk::ProgressEvent().CheckEvent( &event ) ) {

    if( itk::PoistatsOdfCalculationStartEvent().CheckEvent( &event ) ) {
      PostMessage( "constructing odf list\n" );
    } else if( itk::PoistatsOdfCalculationProgressEvent().CheckEvent( &event ) ){
      PostMessage( ". " );
    } else if( itk::PoistatsOdfCalculationEndEvent().CheckEvent( &event ) ) {
      PostMessage( "\nfinished\n" );
    }

  }
  
}


void 
CommandUpdate::PostMessage( const std::string message ){
  
  std::cerr << message;
  
  std::string fileName = m_OutputDirectory + "/" + m_LogFileName;
  WriteMessage( message, fileName );
  
}

void 
CommandUpdate::WriteMessage( const std::string message, 
  const std::string fileName ) {

  std::ofstream logFile( fileName.c_str() );
  
  logFile << message;
  
  logFile.close();
    
}
