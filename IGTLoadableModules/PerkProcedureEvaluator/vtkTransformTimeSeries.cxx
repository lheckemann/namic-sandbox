
#include "vtkTransformTimeSeries.h"


#include "vtkObjectFactory.h"



vtkStandardNewMacro( vtkTransformTimeSeries );



void
vtkTransformTimeSeries
::PrintSelf( ostream& os, vtkIndent indent )
{
  Superclass::PrintSelf( os, indent );
}



int
vtkTransformTimeSeries
::GetNumberOfRecords() const
{
  return this->Data.size();
}



double
vtkTransformTimeSeries
::GetTimeAtIndex( int index ) const
{
  if ( index < 0  ||  (unsigned int)index >= this->Data.size() )
    {
      // TODO: Exception.
    return 0;
    }
  
  return this->Data[ index ].first;
}



void
vtkTransformTimeSeries
::AddRecord( double time, vtkTransform* transform )
{
  vtkTransform* tr = vtkTransform::New();
  tr->DeepCopy( transform );
  std::pair< double, vtkTransform* > p( time, tr );
  
  if ( time < this->MinTime )
    {
    this->Data.insert( this->Data.begin(), p );
    this->MinTime = time;
    }
  
  else if ( time > this->MaxTime )
    {
    this->Data.push_back( p );
    this->MaxTime = time;
    }
    
  else
    {
    for ( DataIteratorType it = this->Data.begin(); it < this->Data.end(); ++ it )
      {
      if ( (*it).first > time )
        {
        this->Data.insert( ( -- it ), p );
        break;
        }
      }
    }
}



void
vtkTransformTimeSeries
::Clear()
{
  for ( DataIteratorType it = this->Data.begin(); it < this->Data.end(); ++ it )
    {
    (*it).second->Delete();
    (*it).second = NULL;
    }
  
  this->Data.clear();
}



vtkTransformTimeSeries
::vtkTransformTimeSeries()
{
  this->MinTime = 0;
  this->MaxTime = 0;
}



vtkTransformTimeSeries
::~vtkTransformTimeSeries()
{
  this->Clear();
}

