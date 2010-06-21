#include "vtkppr.h"

vtkStandardNewMacro(vtkppr);                        // 
vtkCxxRevisionMacro(vtkppr, "$Revision: 1.0 $");    // 100620-komura

time_t rawtime;
struct tm *timeinfo;

#define MAX_findo 10

double origP[MAX_findo][3],origI[MAX_findo][3];
double P[MAX_findo][3],I[MAX_findo][3];
double origTRE[MAX_findo],origTRERas[MAX_findo];
char tmatrix[500];

double totalfre;
double* tre2;
double* besttre2;
using namespace TNT;

int omitp[10],omiti[10];
int org_fidno,org_sfidno;

void PrintTime(FILE *fd)
{
  time ( &rawtime );
  timeinfo = localtime ( &rawtime );
  fprintf(fd,"\n %s ",asctime (timeinfo));
} 

void ArraytoMatrix(Array2D <double> &a, Matrix <double> &m, int p, int q)
{
 int i,j; 
 for(i=0;i<p;i++) 
     for(j=0;j<q;j++) 
           m[i][j]=a[i][j];
}

void ArraytoMatrix(Array1D <double> &a, Matrix <double> &m, int p, int q)
{
 int i,j; 
 for(i=0;i<p;i++) 
     for(j=0;j<q;j++) 
           if(p>q)m[i][j]=a[i];
           else m[i][j]=a[j];
}

void MatrixtoArray( Matrix <double> &m,Array2D <double> &a, int p, int q)
{
 int i,j; 
 for(i=0;i<p;i++) 
     for(j=0;j<q;j++) 
           a[i][j]=m[i][j];
}

void Print(char name[],int m,int n,Array2D <double> a)
{
    int i,j; 
    std::cerr << "\n" << name <<" : \n" << std::endl;
     for(i=0;i<m;i++) {
         for(j=0;j<n;j++) {
             char c[252];
             sprintf(c,"%.4lf",a[i][j]);
             std::cerr << c << std::endl;
         }
    std::cerr << "\n" << std::endl;
   }
}

void Print(char name[],int m,int n, Matrix <double> a)
{
    int i,j; 
    std::cerr << "\n" << name << ": \n" << std::endl;
     for(i=0;i<m;i++) {
         for(j=0;j<n;j++) {
             char c[252];
             sprintf(c,"%.4lf",a[i][j]);
         }
    std::cerr << "\n" << std::endl;
   } 
}


char* getsvd(int ITEM, char* str_i, char* str_p)
{
 char* result= new char[500];
 int i,j,k,m,n;
 double ref,ref2;

 double** I = new double*[ITEM];
 for(i=0;i<ITEM;i++) I[i] = new double [3];
 
 double** P = new double*[ITEM];
 for(i=0;i<ITEM;i++)  P[i] = new double[3];
 
 double Imean[3],Pmean[3];
 
 double** IO = new double*[ITEM];
 for(i=0;i<ITEM;i++) IO[i] = new double[3];
 
 double** PO = new double*[ITEM];
 for(i=0;i<ITEM;i++) PO[i] = new double[3];
 
 double*** h = new double**[ITEM];
 for(i=0;i<ITEM;i++)  h[i] = new double*[3];
 for(i=0;i<ITEM;i++) 
   for(j=0;j<3;j++)
      h[i][j] = new double[3];

        double ta1[3][3],ta2[3][3];
    Array2D< double> U(3,3), V(3,3), S(3,3), H(3,3), T(3,3);
  
    double sumi,sump;

    FILE *temp1;
    temp1=fopen("/home/komura/Desktop/Triple100613-100606StereoCalib-ayamada/100606StereoCalib-ayamada/temporary_1.txt","wt");
    fprintf(temp1,"%s %s ", str_i, str_p);
    fflush(temp1);
    fclose(temp1);

    temp1=fopen("/home/komura/Desktop/Triple100613-100606StereoCalib-ayamada/100606StereoCalib-ayamada/temporary_1.txt","rt");
    for(i=0;i<ITEM;i++)
      for(j=0;j<3;j++)
          fscanf(temp1,"%lf", &I[i][j]);
    
    for(i=0;i<ITEM;i++)
      for(j=0;j<3;j++)
          fscanf(temp1,"%lf", &P[i][j]);
    fclose(temp1);
    

    for(i=0;i<3;i++) {
     sumi=0;
     sump=0;
     for(j=0;j<ITEM;j++)  {
      sumi+=I[j][i];
      sump+=P[j][i];
     }
     Imean[i] = sumi/(double)ITEM;
     Pmean[i] = sump/(double)ITEM;
//     fprintf(stdout,"\n Imean_%d : %.2f Pmean_%d : %.2f",i,Imean[i],i,Pmean[i]);
   }
    
    for(i=0;i<ITEM;i++) {
      for(j=0;j<3;j++) {
       IO[i][j]= I[i][j]-Imean[j];
       PO[i][j]= P[i][j]-Pmean[j];
      }
     }
     
        
    for(i=0;i<3;i++) {
     for(j=0;j<3;j++) {
         H[i][j]=0.0;   
      for(k=0;k<ITEM;k++) {
          h[k][i][j] = IO[k][i]*PO[k][j];
          H[i][j] += h[k][i][j];
         }
        }
    }
  
   JAMA::SVD<double> svdcl(H);
   
   svdcl.getU(U);
   svdcl.getS(S);
   svdcl.getV(V);
   
   Matrix <double> UM(3,3),VM(3,3),SM(3,3),HM(3,3),UMT,VMT,T1,T2,T3,RE,RM,DE(3,3);
   
   ArraytoMatrix(H,HM,3,3); 
   ArraytoMatrix(U,UM,3,3);
   ArraytoMatrix(S,SM,3,3); 
   ArraytoMatrix(V,VM,3,3); 

   VMT=transpose(VM);
   RE=UM*SM*VMT;  // RE must be same as HM
 
   for(i=0;i<3;i++) 
     for(j=0;j<3;j++)
        if(i==j) DE[i][j] =1.0;
        else DE[i][j] = 0;
   
   T3=VM*UM;
   double t3a[3][3];
   for(i=0;i<3;i++) 
      for(j=0;j<3;j++) 
       t3a[i][j]=T3[i][j];

   vtkdete ppp;
   ref = ppp.dete(t3a);
   
   DE(3,3)=ref;
    
   UMT=transpose(UM);
   RM=VM*DE*UMT;

   Matrix <double> IMM(3,1),PMM(3,1),TL,IM(3,1),PM(3,1),TRE,PM_RE;
   double* tre= new double [ITEM];
   double* fre= new double [ITEM];
   double avetre=0.0;
   totalfre=0.0;
   
   IMM(1,1) = Imean[0];
   IMM(2,1) = Imean[1];
   IMM(3,1) = Imean[2];

   PMM(1,1) = Pmean[0];
   PMM(2,1) = Pmean[1];
   PMM(3,1) = Pmean[2];

   TL=PMM-RM*IMM;

   for(i=0;i<ITEM;i++) {
    IM(1,1) = I[i][0];
       IM(2,1) = I[i][1];
       IM(3,1) = I[i][2];
    
       PM(1,1) = P[i][0];
       PM(2,1) = P[i][1];
       PM(3,1) = P[i][2];

       PM_RE=RM*IM+TL;
       TRE=PM-PM_RE;

       tre[i]=0.0;
//       tre2[i]=0.0;
       
       for(j=0;j<3;j++) tre[i]+=TRE(j+1,1)*TRE(j+1,1);
       tre2[i] = fre[i] = sqrt(tre[i]);
       fre[i] = fre[i]*fre[i];
       totalfre += fre[i]; 
   }
       totalfre = sqrt(totalfre/(double)ITEM);
      
   sprintf(result, "%f %f %f %f   \n%f %f %f %f  \n%f %f %f %f \n0.0   0.0   0.0   1.0  \n%f", RM(1,1), RM(1,2), RM(1,3), TL(1,1), RM(2,1), RM(2,2), RM(2,3), TL(2,1),RM(3,1), RM(3,2), RM(3,3), TL(3,1), totalfre);
   sprintf(tmatrix, "%f %f %f %f   \n%f %f %f %f  \n%f %f %f %f \n0.0   0.0   0.0   1.0  \n", RM(1,1), RM(1,2), RM(1,3), TL(1,1), RM(2,1), RM(2,2), RM(2,3), TL(2,1),RM(3,1), RM(3,2), RM(3,3), TL(3,1));

   return(result);  
}


int FindFidNumber(int &fid, int &sfid)
{
  FILE *fi, *fp;
  int i,j,ret,rets,ii=0,jj=0;
  
  fi = fopen("/home/komura/Desktop/Triple100613-100606StereoCalib-ayamada/100606StereoCalib-ayamada/ImagePosition.dat","rt");
  fp = fopen("/home/komura/Desktop/Triple100613-100606StereoCalib-ayamada/100606StereoCalib-ayamada/PatientPosition.dat","rt");
  
  if(fi==NULL||fp==NULL) { fprintf(stdout,"No inputfiles for registration"); exit(0); }

  fid=sfid=0;
  for(i=0;i<10;i++) omitp[i]=omiti[i]=0;

  for(i=0;i<MAX_findo;i++) {
//  printf("\n %d",i);
     for(j=0;j<3;j++) 
       if(!feof(fp)) 
       { 
             ret=fscanf(fp,"%lf",&origP[i][j]);
             P[i+ii][j]=origP[i][j];
       }
     if(ret==-1) break; // this is for the omitting the data located last
     fid++;
     if(abs(origP[i][0])>=10000) { omitp[i] = 1; ii--; fid--;  }
     if(feof(fp)) break;
   }

  for(i=0;i<MAX_findo;i++) {
//  printf("\n %d",i);
     for(j=0;j<3;j++)  
         if(!feof(fi)) 
           { 
           ret=fscanf(fi,"%lf",&origI[i][j]);
           I[i+jj][j]=origI[i][j];
       //    printf(" %lf",I[i][j]);
    
        }
     if(ret==-1) break; // this is for the omitting the data located last
     sfid++;
     if(abs(origI[i][0])>10000) {  omiti[i] = 1; jj--; sfid--;  }
     if(feof(fi)) break;
  }
  
//  printf("\n count %d counts %d",count, counts);
  if(fid<3||fid>MAX_findo||sfid<3||sfid>MAX_findo) { std::cerr << "\nFiducial number error" << std::endl; exit(0); }
//  std::cerr << "\n\n stereo calibration finish \n\n" << std::endl;
  std::cerr << "\nPatient fid #" << fid << "Image fid #" << sfid <<std::endl;
  fclose(fi);
  fclose(fp);
  
//  for(i=0;i<10;i++) {
//     printf("\n%d, %d",omitp[i],omiti[i]); 
// }


  return 1;
}

int SortandRankList(double array[],int num,char liststring[])
{
// rank 0 is smallest value 

  int first,search,i,j,rank,top;
  double temp;
  double* cparray = new double [num] ;
  char tempc[4];
  int* list = new int [num];

  
  for(i=0;i<num;i++) cparray[i] = array[i];

  for(first = 0; first < num - 1; first++)
   for(search = first + 1; search < num; search++)
 if(array[search] < array[first]) {
    temp = array[search];
    array[search] = array[first];
    array[first] = temp;
   }

  for(i=0;i<num;i++) 
     for(j=0;j<num;j++) 
       if(cparray[i]==array[j]) {
        list[i] = j;
        if(j==num-1) top = i;
    }
//  fprintf(stdout,"\n(Min:0) ");
//  for(i=0;i<num;i++) fprintf(stdout," %d ", list[i]);
   
  for(i=0;i<num;i++) {
      sprintf(tempc,"%d ", list[i]);
      strcat(liststring,tempc);
  }
  return top;
}

int sfscanf(char buf[],double *result, int reset) {
  char ch;
  char str[20];   // 15 is ok?
  static int i=0;
  int j,ret=0;
//  *result=-10001;
  
  if(reset==1) { memset(str,0,20); i=0; return 0; }
  
//    strcpy(str,"");
    memset(str,0,20);
    j=0;

    while(j<=20) {
     ret=sscanf(buf+i,"%c",&ch);
     if(ret==-1) break;
//     printf("\n%c,%d",ch,ret);
     i++;
     if(j==0&&ch==' ') continue;
     if(ch==' ') break;
      str[j]=ch;
      j++;
    };

    if(j>0)  *result=atof(str); 
    if(j==0&&ret==-1) return -1;
    return 1;
}
 


int GetOriginalOrder(char bestpair[], int order)
{
    int i,j;
    double pa[3];
  
   
   sfscanf(bestpair,&pa[0],1);

    
   for(i=0;i<order+1;i++) {
     sfscanf(bestpair,&pa[0],0);
     sfscanf(bestpair,&pa[1],0);
     sfscanf(bestpair,&pa[2],0);
  }
  
     
   for(i=0;i<MAX_findo;i++) {
    if(pa[0]==origP[i][0]&&pa[1]==origP[i][1]&&pa[2]==origP[i][2])   return i;
 }
     
  return -1;
   
}


int GetOriginalOrderRas(char bestpair[], int order)
{
    int i,j;
    double im[3];
  
   
   sfscanf(bestpair,&im[0],1);
    
   for(i=0;i<order+1;i++) {
     sfscanf(bestpair,&im[0],0);
     sfscanf(bestpair,&im[1],0);
     sfscanf(bestpair,&im[2],0);
  }
  
       
   for(i=0;i<MAX_findo;i++) {
    if(im[0]==origI[i][0]&&im[1]==origI[i][1]&&im[2]==origI[i][2])   return i;
 }
     
  return -1;
   
}




int vtkppr::ppr(int argc)
{

  int i,j,k,m;
  int ii,jj;

  char *str1,*str2;
  char *result,bestresult[500],transformmatrix[500];
  char temp1[15],temp2[15],tre2ch[5];
  double bestfre=200,cutofffre=-1;
  char bestpair_I[500], bestpair_P[500]; // if number of marker increase, it may need to get bigger array
  int min_NUM,autofinding=1;


  FILE *fr;
  int fidno,sfidno,major,pass;
  int p,q;
  int range1,range2;
  int besti,bestj;
//  PrintTime(stdout);

  FindFidNumber(fidno,sfidno);
  org_fidno=fidno;
  org_sfidno=sfidno;
  min_NUM = (fidno < sfidno) ? fidno : sfidno;

  if(min_NUM==sfidno)  major = 1; // patient is major
  else major=0; // image is major

  vtkpuc pa(fidno,4),im(sfidno,4);  // 4 looks to have no meaning

  range1=pa.fac;
  range2=im.fac;

  if(argc<=0) {
      autofinding=0;
      fprintf(stdout,"\nAutomatic Matching Off, Ordered Matching\n"); 
  }
  if(fidno<sfidno) range1=1;
  else range2=1;
  
  
  if(autofinding==0) range1=range2=1;

  std::cerr << "\nMIN Marker #: "<< min_NUM << " Case(p) #: " << range1 << "  Case(i) #: " << range2 << std::endl;
//  printf("\nMIN Marker #: %d, Case(p) #: %d Case(i) #: %d",min_NUM, range1, range2);
  
  double** ras = new double* [min_NUM];
  for(i=0;i<min_NUM;i++) ras[i] = new double [3];

  double** patient = new double* [min_NUM];
  for(i=0;i<min_NUM;i++) patient[i] = new double [3];

  char* ranklist = new char [min_NUM*4];

  tre2 = new double [min_NUM];
  besttre2 = new double [min_NUM];

  fr = fopen("/home/komura/Desktop/Triple100613-100606StereoCalib-ayamada/100606StereoCalib-ayamada/RegistrationMatrix.dat","wt");


  for(i=0;i<range1;i++) {
      for(j=0;j<range2;j++)  {
      
          //printf("\n P  I *********************");
          
          for(k=0;k<min_NUM;k++) {
              pass=0;

              if(range1!=1&&range2!=1) {
                  if(major==0&&k!=0) if(im.getpu(j,k-1)>im.getpu(j,k)) { pass=1; break;  }
                  if(major==1&&k!=0) if(pa.getpu(i,k-1)>pa.getpu(i,k)) { pass=1; break; }
              }
              
              for(m=0;m<3;m++) {
                  p=pa.getpu(i,k)-1;
                  q=im.getpu(j,k)-1;
                  patient[k][m] = P[p][m];
                  ras[k][m] = I[q][m];
              }

//         printf("\n %d  %d",pa.getpu(i,k), im.getpu(j,k));
          }

          
          if(pass==1) continue;
          
          str1 = new char [MAX_findo*40];
          str2 = new char [MAX_findo*40];
          strcpy(str1,"");    
          strcpy(str2,"");
          
          for(ii=0;ii<min_NUM;ii++)
              for(jj=0;jj<3;jj++)  {   
                  sprintf(temp1," %f",ras[ii][jj]);
                  strcat(str1,temp1);
                  sprintf(temp2," %f",patient[ii][jj]);
                  strcat(str2,temp2);
              }
          
          //usleep(100000);
          
          result = getsvd(min_NUM,str2,str1);
          
          if(totalfre < bestfre) {
              
//           printf("\n\nFRE: %f",totalfre);
              bestfre=totalfre;
              sprintf(transformmatrix,"%s",tmatrix);
              strcpy(bestresult,result);
              
              
              sprintf(bestpair_I,"%s",str1); 
              sprintf(bestpair_P,"%s",str2);
              
              for(int r=0;r<min_NUM;r++) besttre2[r]=tre2[r];

//           printf("\n Patient:");
//           for(m=0;m<min_NUM;m++)  printf(" %d",pa.getpu(i,m));
              //          printf("\n Image:  ");
              //          for(m=0;m<min_NUM;m++)  printf(" %d",im.getpu(j,m));
//           printf("\npass %d i %d j %d",pass,i,j);
              besti=i;
              bestj=j;
          }   
          
//       delete [] str1;
//       delete [] str2;

          if(bestfre<=cutofffre) break;
      
      }
//        delete [] I;
//        delete [] P;
  }

  std::cerr << "\n\nFRE: " <<  bestfre << std::endl;
  std::cerr << "\n Patient:" << std::endl;
  for(m=0;m<min_NUM;m++)  fprintf(stdout, " %d", pa.getpu(besti,m) );
  std::cerr << "\n Image:  " << std::endl;
  for(m=0;m<min_NUM;m++)  fprintf(stdout, " %d", im.getpu(bestj,m) );


  double* sortarray = new double[min_NUM];
//     for(int ab=0;ab<min_NUM;ab++) sortarray[ab] = besttre2[ab];

  int topnum;
  char topnumch[2];

 
  int a,b;
  int c;
    
  for(a=0;a<MAX_findo;a++) origTRE[a] = origTRERas[a] = -1.0;
    
 
  for(a=0;a<min_NUM;a++) {
      b=GetOriginalOrder(bestpair_P,a);
      origTRE[b]=besttre2[a];
      c=GetOriginalOrderRas(bestpair_I,a);
      origTRERas[c]=besttre2[a];
       
  }
 

  for(int ab=0;ab<min_NUM;ab++) sortarray[ab] = origTRE[ab];
    
  topnum=SortandRankList(sortarray,min_NUM,ranklist);
     
  strcat(bestresult,"\n"); 

  for(a=0;a<MAX_findo;a++) { 
      sprintf(tre2ch,"%f ",origTRE[a]); 
      strcat(bestresult,tre2ch);  
  }

  strcat(bestresult,"\n"); 
//     for(int ab=0;ab<min_NUM;ab++)  { sprintf(tre2ch,"%f ",besttre2[ab]); strcat(bestresult,tre2ch);  }
//     for(int ab=min_NUM;ab<MAX_findo;ab++) { sprintf(tre2ch,"-1.0 "); strcat(bestresult,tre2ch); }
  for(a=0;a<MAX_findo;a++) { 
      sprintf(tre2ch,"%f ",origTRERas[a]); 
      strcat(bestresult,tre2ch);  
  
  }   
  sprintf(topnumch,"%d",topnum);
  strcat(bestresult,"\n"); 
  strcat(bestresult,topnumch); 
  
  strcat(bestresult,"\n");
  strcat(bestresult,ranklist);
 
  fprintf(fr,"%s",bestresult);

  std::cerr << "\nRegistration Matrix Found.\n" << transformmatrix << "\n" << std::endl;

  fclose(fr);
//   PrintTime(stdout);
  std::cerr << "\n-----Calculation End.------\n"  << std::endl;
}

