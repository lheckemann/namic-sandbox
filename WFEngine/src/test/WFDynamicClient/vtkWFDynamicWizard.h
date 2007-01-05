#ifndef VTKWFDYNAMICWIZARD_H_
#define VTKWFDYNAMICWIZARD_H_

#include <vtkKWDialog.h>

class vtkWFDynamicWizard : public vtkKWDialog
{
public:
    static vtkWFDynamicWizard *New();
    vtkTypeRevisionMacro(vtkWFDynamicWizard,vtkKWDialog);
    
//    void InitializeDynamicWizard();
    
protected:
    vtkWFDynamicWizard();
    virtual ~vtkWFDynamicWizard();
    
    // Description:
    // Create the widget.
    virtual void CreateWidget();
};

#endif /*VTKWFDYNAMICWIZARD_H_*/
