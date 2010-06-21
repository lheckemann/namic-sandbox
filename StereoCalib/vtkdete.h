#ifndef __vtkdete_h
#define __vtkdete_h

#include <stdio.h>
#include <math.h>
#include "vtkSlicerWidget.h"                // 100620-komura

class vtkdete  :
public vtkSlicerWidget                                // 100620-komura
{
    int c;
public:
    vtkTypeRevisionMacro( vtkdete ,vtkSlicerWidget);    // 100620-komura
    static vtkdete *New();
    vtkdete();
    double dete(double in[3][3]);
    void change(double x[10][10],int m,int n);

};

#endif
