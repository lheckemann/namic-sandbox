
#ifndef __vtkTextActorFlippable_h
#define __vtkTextActorFlippable_h

#include "vtkTextActor.h"

class vtkTextProperty;
class vtkPolyDataMapper2D;
class vtkImageData;
class vtkFreeTypeUtilities;
class vtkTransform;
class vtkPolyData;
class vtkPoints;
class vtkTexture;

class vtkTextActorFlippable : public vtkTextActor
{
public:
  
  static vtkTextActorFlippable *New();
  
  void Flip( int direction );
  
  vtkTypeRevisionMacro( vtkTextActorFlippable, vtkTextActor );
  void PrintSelf( ostream& os, vtkIndent indent );

  virtual void ComputeRectangle( vtkViewport *viewport );

protected:
  
  vtkTextActorFlippable();
  ~vtkTextActorFlippable();

  bool FlipX;
  bool FlipY;

private:
  vtkTextActorFlippable(const vtkTextActorFlippable&);  // Not implemented.
  void operator=(const vtkTextActorFlippable&);  // Not implemented.
};


#endif

