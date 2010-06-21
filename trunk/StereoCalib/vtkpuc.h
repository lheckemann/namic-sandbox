#ifndef __vtkpuc_h
#define __vtkpuc_h


#include <stdio.h>
#include "vtkSlicerWidget.h"    // 100620-komura

class vtkpuc  :
public vtkSlicerWidget      // 100620-komura
{

public:
    vtkTypeRevisionMacro( vtkpuc ,vtkSlicerWidget);   // 100620-komura
    static vtkpuc *New();
    int num;
    int out;
    int fac;
    int *flag;
    int *tmp;
    char *number;
    int **pu;
    int count;
    int execute;
    
    vtkpuc();
    vtkpuc(int,int);
    ~vtkpuc();
    int ch(int);
    void rec(int);
    int getpu(int,int);
    void display();

};

#endif
