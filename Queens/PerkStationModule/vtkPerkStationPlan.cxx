
#include "vtkPerkStationPlan.h"

#include "vtkObjectFactory.h"


vtkStandardNewMacro( vtkPerkStationPlan );


/**
 * Constructor.
 */
vtkPerkStationPlan
::vtkPerkStationPlan()
{
  for ( int i = 0; i < 3; ++ i )
    {
    this->EntryPointRAS[ i ] = 0;
    this->TargetPointRAS[ i ] = 0;
    this->ValidationEntryPointRAS[ i ] = 0;
    this->ValidationTargetPointRAS[ i ] = 0;
    }
  
  this->Validated = false;
}


/**
 * Destructor.
 */
vtkPerkStationPlan
::~vtkPerkStationPlan(void)
{
}


void vtkPerkStationPlan::PrintSelf( ostream& os, vtkIndent indent )
{
  this->Superclass::PrintSelf(os,indent);

  /* :TODO: implement this
  os << indent << "PlanningVolumeRef:   " << 
   (this->PlanningVolumeRef ? this->PlanningVolumeRef : "(none)") << "\n";
 */
}
