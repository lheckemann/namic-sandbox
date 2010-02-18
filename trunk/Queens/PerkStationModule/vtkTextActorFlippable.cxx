
#include "vtkTextActorFlippable.h"

#include "vtkObjectFactory.h"
#include "vtkViewport.h"
#include "vtkPoints.h"
#include "vtkPointData.h"


vtkCxxRevisionMacro( vtkTextActorFlippable, "$Revision: 1.48 $" );
vtkStandardNewMacro( vtkTextActorFlippable );


/**
 * @param flip True flips the text, False leaves in normal position.
 */
void
vtkTextActorFlippable::FlipAroundX( bool flip )
{
  this->FlipX = flip;
  this->Modified();
}


/**
 * @param flip True flips the text, False leaves in normal position.
 */
void
vtkTextActorFlippable::FlipAroundY( bool flip )
{
  this->FlipY = flip;
  this->Modified();
}


void
vtkTextActorFlippable
::ComputeRectangle( vtkViewport* viewport )
{
  this->Superclass::ComputeRectangle( viewport );
  
  if ( FlipX )
    {
    double p0[ 3 ];
    this->RectanglePoints->GetPoint( 0, p0 );
    double p1[ 3 ];
    this->RectanglePoints->GetPoint( 1, p1 );
    double p2[ 3 ];
    this->RectanglePoints->GetPoint( 2, p2 );
    double p3[ 3 ];
    this->RectanglePoints->GetPoint( 3, p3 );
    this->RectanglePoints->SetPoint( 0, p1 );
    this->RectanglePoints->SetPoint( 1, p0 );
    this->RectanglePoints->SetPoint( 2, p3 );
    this->RectanglePoints->SetPoint( 3, p2 );
    }
  
  if ( FlipY )
    {
    double p0[ 3 ];
    this->RectanglePoints->GetPoint( 0, p0 );
    double p1[ 3 ];
    this->RectanglePoints->GetPoint( 1, p1 );
    double p2[ 3 ];
    this->RectanglePoints->GetPoint( 2, p2 );
    double p3[ 3 ];
    this->RectanglePoints->GetPoint( 3, p3 );
    this->RectanglePoints->SetPoint( 0, p3 );
    this->RectanglePoints->SetPoint( 1, p2 );
    this->RectanglePoints->SetPoint( 2, p1 );
    this->RectanglePoints->SetPoint( 3, p0 );
    }
  
}


vtkTextActorFlippable
::vtkTextActorFlippable()
{
  this->FlipX = false;
  this->FlipY = false;
}


vtkTextActorFlippable
::~vtkTextActorFlippable()
{

}


// ----------------------------------------------------------------------------
void vtkTextActorFlippable::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "Flip around X axis: " << this->FlipX << endl;
  os << indent << "Flip around Y axis: " << this->FlipY << endl;
}
