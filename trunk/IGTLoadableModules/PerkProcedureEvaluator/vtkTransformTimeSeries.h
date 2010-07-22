
#ifndef VTKTRANSFORMTIMESERIES_H
#define VTKTRANSFORMTIMESERIES_H


#ifdef WIN32
#include "vtkPerkProcedureEvaluatorWin32Header.h"
#endif

#include <iostream>
#include <utility>
#include <vector>

#include "vtkObject.h"

#include "vtkTransform.h"


class
VTK_PerkProcedureEvaluator_EXPORT
vtkTransformTimeSeries
: public vtkObject
{

public:
  
  static vtkTransformTimeSeries *New();  
  void PrintSelf( ostream& os, vtkIndent indent );
  vtkTypeMacro( vtkTransformTimeSeries, vtkObject );
  
  int GetNumberOfRecords() const;
  double GetTimeAtIndex( int index ) const;
  vtkTransform* GetTransformAtIndex( int index ) const;
  
  void AddRecord( double time, vtkTransform* transform );
  void Clear();
  
  
  //BTX
  typedef std::vector< std::pair< double, vtkTransform* > > DataType;
  typedef DataType::iterator DataIteratorType;
  //ETX
  

protected:

  vtkTransformTimeSeries();
  ~vtkTransformTimeSeries();


private:
  
  vtkTransformTimeSeries( const vtkTransformTimeSeries& ); // Not implemented.
  void operator=( const vtkTransformTimeSeries& ); // Not implemented.
  
  
  DataType Data; // std::vector
  double MinTime;
  double MaxTime;
  
};


#endif
