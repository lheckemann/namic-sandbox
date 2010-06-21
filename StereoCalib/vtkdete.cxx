
#include "vtkdete.h"

vtkStandardNewMacro(vtkdete);                       // 
vtkCxxRevisionMacro(vtkdete, "$Revision: 1.0 $");   // 100620-komura


vtkdete::vtkdete(){
    c=0;
}


double vtkdete::dete(double in[3][3])
{
  double a,det=1;
  double x[10][10];
   int i,j,l,m,n;

   n=3;

    
        for(i=1;i<=n;i++){
        for(j=1;j<=n;j++)
            x[i][j]=in[i-1][j-1];

    }

//    printf("\ninput matrix");
//    for(i=0;i<3;i++){ printf("\n"); for(j=0;j<3;j++){  printf(" %.5f ",x[i+1][j+1]); } }

    for(m=1;m<=n;m++){

        l=m+1;

        if(x[m][m]==0) change(x,m,n);

        if(det==0) break;

//        for(i=1; i<=n; i++){

//        for(j=1; j<=n; j++)

//            printf(" %12.5f  ", x[i][j]);              

//        printf("\n");

//        }

//        printf("\n");

        for(i=l;i<=n;i++){

            a=x[i][m]/x[m][m];

            for(j=1;j<=n;j++)

                x[i][j]-=a*x[m][j];

        }

    det*=pow((double)-1,(double)c)*x[m][m];

    }

//    printf("\nreturn value = %12.5f\n\n",det);
  
    return(det);

}

void vtkdete::change(double x[10][10], int m, int n)

{

int i,j, ii;

float temp;

    c++;

    for(i=m;i<=n;i++){

        ii = i+1 ;

        if(x[i+1][m] != 0) break;

 //       else printf("det is 0. \n");

    }

    for(j=1; j<=n; j++){

        temp = x[m][j];

        x[m][j] = x[ii][j];

        x[ii][j] = temp;

    }

}
