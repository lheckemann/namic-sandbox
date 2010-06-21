#ifndef _ppr10_h
#define _ppr10_h

#include <iostream>
#include <string.h>
#include "jama_svd.h"
#include "tnt_cmat.h"
#include "vtkdete.h"
#include "vtkpuc.h"
#include <time.h>
#include "vtkSlicerWidget.h"    // 100620-komura


    void PrintTime(FILE *fd);
    
    void ArraytoMatrix(Array2D <double> &a, Matrix <double> &m, int p, int q);

    void ArraytoMatrix(Array1D <double> &a, Matrix <double> &m, int p, int q);

    void MatrixtoArray( Matrix <double> &m,Array2D <double> &a, int p, int q);

    void Print(char name[],int m,int n,Array2D <double> a);

    void Print(char name[],int m,int n, Matrix <double> a);

    char* getsvd(int ITEM, char* str_i, char* str_p);

    int FindFidNumber(int &fid, int &sfid);

    int SortandRankList(double array[],int num,char liststring[]);

    int sfscanf(char buf[],double *result, int reset);

    int GetOriginalOrder(char bestpair[], int order);

    int GetOriginalOrderRas(char bestpair[], int order);

class vtkppr :
public vtkSlicerWidget      // 100620-komura
{
public:
    vtkTypeRevisionMacro( vtkppr ,vtkSlicerWidget);   // 100620-komura
    static vtkppr *New();

    int ppr(int);
};
#endif
