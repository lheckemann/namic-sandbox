
#include "vtkpuc.h"


vtkStandardNewMacro(vtkpuc);                        // 
vtkCxxRevisionMacro(vtkpuc, "$Revision: 1.0 $");    // 100620-komura


vtkpuc::vtkpuc(){
}


vtkpuc::vtkpuc(int n, int o)
{
 int i;
 num=n; out=o;
 fac=1;

 for(i=0;(num-i)>0;i++) fac=fac*(num-i);

 flag = new int [num];
 number = new char [num];
 tmp = new int[num];

 pu = new int* [fac];
 for(i=0;i<fac;i++)  pu[i] = new int [num];
 count=0;
 execute=0;
 for(i=0 ; i<num ; i++) flag[i]=0;
 
// printf("initialize ok");

}

vtkpuc::~vtkpuc()
{
// delete [] flag;
// delete [] number;
// delete [] pu;
}

int vtkpuc::ch(int n)         // ?? ??? ?? ??? n?? ??? ??
{

 int i,c=0;
 for(i=1 ; i<=num ; i++) tmp[i] =0;

 for(i=1 ; i<=num ; i++)
  if(!flag[i-1])
   tmp[c++]=i;
 
 return(tmp[n]);

}

void vtkpuc::rec(int depth)      // ???? ??
{
 int i, k;
 if(depth==0) {       // ??? ???? ?????
  for(i=0 ; i<num ; i++) {
//   printf(" %d", number[i]);   // ???
   pu[count][i]= number[i];
}
//  putchar('\n');
  count++;
  return;                        // ???? ??
 }

 for(i=0 ; i<depth ; i++) {
  k=ch(i);
  flag[k-1]=1;                // flag? ???? ????.
  number[num-depth]=k;
  rec(depth-1);
  flag[k-1]=0;                // flag????
 }
 execute=1;
}

void vtkpuc::display()
{
  int i,j;
 if(execute==0) rec(num);
 
  for(i=0 ; i<fac ; i++) {
    for(j=0;j<num;j++)
        printf(" %d", pu[i][j]);
  putchar('\n');
}

 
}

int vtkpuc::getpu(int m,int n)
{
 if(execute==0) rec(num);
 
 return pu[m][n];
}
