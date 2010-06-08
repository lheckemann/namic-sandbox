
#include "vtkObject.h"

#include <string>

#include "vtkPerkStationModuleWin32Header.h"


/**
 * Data model for needle insertion.
 * Planning is defined by an entry and a target point, as well as validation.
 */
class
VTK_PERKSTATIONMODULE_EXPORT
vtkPerkStationPlan
: public vtkObject
{
public:

  static vtkPerkStationPlan *New();  
  void PrintSelf( ostream& os, vtkIndent indent );
  
  
  //BTX
  
  void SetName( std::string str ){ this->Name = str; }
  std::string GetName() const { return this->Name; };
  
  void SetPlanningVolumeRef( std::string ref ){ this->PlanningVolumeRef = ref; }
  std::string GetPlanningVolumeRef() const { return this->PlanningVolumeRef; };
  
  vtkSetVector3Macro( EntryPointRAS, double );
  vtkGetVectorMacro( EntryPointRAS, double, 3 );
  
  vtkSetVector3Macro( TargetPointRAS, double );
  vtkGetVectorMacro( TargetPointRAS, double, 3 );
  
  vtkGetMacro( Validated, bool);
  vtkSetMacro( Validated, bool);
  
  void SetValidationVolumeRef( std::string ref ){ this->ValidationVolumeRef = ref; }
  std::string GetValidationVolumeRef() const { return this->ValidationVolumeRef; };
  
  vtkSetVector3Macro( ValidationEntryPointRAS, double );
  vtkGetVectorMacro( ValidationEntryPointRAS, double, 3 );
  
  vtkSetVector3Macro( ValidationTargetPointRAS, double );
  vtkGetVectorMacro( ValidationTargetPointRAS, double, 3 );
  
  //ETX
  
protected:
  
  vtkPerkStationPlan( void );
  ~vtkPerkStationPlan( void );
  
  vtkPerkStationPlan( const vtkPerkStationPlan& );
  void operator=( const vtkPerkStationPlan& );
  
  //BTX
  
  std::string Name;
  
  std::string PlanningVolumeRef;
  double EntryPointRAS[ 3 ];
  double TargetPointRAS[ 3 ];
  
  bool Validated;
  std::string ValidationVolumeRef;
  double ValidationEntryPointRAS[ 3 ];
  double ValidationTargetPointRAS[ 3 ];
  
  //ETX
};
