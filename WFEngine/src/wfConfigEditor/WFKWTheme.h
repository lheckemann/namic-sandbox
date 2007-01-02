#ifndef WFKWTHEME_H_
#define WFKWTHEME_H_

#include "vtkKWTheme.h"
#include "WFKWColors.h"

class WFKWTheme : public vtkKWTheme
{
public:
 
 static WFKWTheme *New();
 
 //Description:
    // Usual vtk functions
    vtkTypeRevisionMacro ( WFKWTheme, vtkKWTheme );
//    void PrintSelf ( ostream& os, vtkIndent indent );

    // Description:
    // Get method for SlicerColor class
    vtkGetObjectMacro ( WFColors, WFKWColors );

 
 virtual void Install ( );

// typedef vtkKWTheme Superclass;
 
protected:
 WFKWTheme();
 virtual ~WFKWTheme();
 WFKWColors *WFColors;
 
private:

};

#endif /*WFKWTHEME_H_*/
