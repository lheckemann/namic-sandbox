// -*- C++ -*-
const static char SccsId[] = "@(#)TNetExtractor.cc 1.3 10/31/96";
/****************************************************************************
 *
 *  class TNetExtractor
 *
 ****************************************************************************
 *
 *  File         :  TNetExtractor.cc
 *  Type         :  
 *  Purpose      : 
 *
 ****************************************************************************
 *                 
 *  started      :  4  Jan 95     Robert Schweikert
 *  last change  :  13 Feb 95
 *
 ****************************************************************************/
 
#include "TNetExtractor.h"
#include "util.h"
#include "Ttables.hh"

int TNetExtractor::direct_translate(int dir, int ne)
{
   int trne = tr_direct[dir][ne];
   assertStr(trne >= 0, "TNetExtractor::direct_translate : wrong parameter ne");
   return trne;
}

#define DIR(D1,D2) (( (dir1 == D1) && (dir2 == D2)) || ((dir1 == D2) && (dir2 == D1)) )
int TNetExtractor::diag_translate(int dir1, int dir2, int ne)
{
   // ugly (we could do better)
  int trne;
  
  if DIR(S,E) {
    switch(ne) {
      case 1: trne = 2; break;
      case 5: trne = 6; break;
      case 3: trne = -1; break;
      default: assertStr(TFALSE, "TNetExtractor::diag_translate : assertStrion violated"); break;
    }
  }
  else if DIR(E,N) {
    switch(ne) {
      case 3: trne = 0; break;
      case 7: trne = 4; break;
      default: assertStr(TFALSE, "TNetExtractor::diag_translate : assertStrion violated"); break;
    }
  }
  else if DIR(N,W) {
    switch(ne) {
      case 6: trne = 5; break;
      case 2: trne = 1; break;
      default: assertStr(TFALSE, "TNetExtractor::diag_translate : assertStrion violated"); break;
    }
  }
  else if DIR(W,S) {
    switch(ne) {
      case 4: trne = 7; break;
      case 0: trne = 3; break;
      default: assertStr(TFALSE, "TNetExtractor::diag_translate : assertStrion violated"); break;
    }
  }
  else if DIR(T,S) {
    switch(ne) {
      case 4: trne = 2; break;
      case 5: trne = 3; break;
      default: assertStr(TFALSE, "TNetExtractor::diag_translate : assertStrion violated"); break;
    }
  }
  else if DIR(S,B) {
    switch(ne) {
      case 0: trne = 6; break;
      case 1: trne = 7; break;
      default: assertStr(TFALSE, "TNetExtractor::diag_translate : assertStrion violated"); break;
    }
  }
  else if DIR(B,N) {
    switch(ne) {
      case 2: trne = 4; break;
      case 3: trne = 5; break;
      default: assertStr(TFALSE, "TNetExtractor::diag_translate : assertStrion violated"); break;
    }
  }
  else if DIR(N,T) {
    switch(ne) {
      case 6: trne = 0; break;
      case 7: trne = 1; break;
      default: assertStr(TFALSE, "TNetExtractor::diag_translate : assertStrion violated"); break;
    }
  }
  else if DIR(E,T) {
    switch(ne) {
      case 5: trne = 0; break;
      case 7: trne = 2; break;
      default: assertStr(TFALSE, "TNetExtractor::diag_translate : assertStrion violated"); break;
    }
  }
  else if DIR(T,W) {
    switch(ne) {
      case 4: trne = 1; break;
      case 6: trne = 3; break;
      default: assertStr(TFALSE, "TNetExtractor::diag_translate : assertStrion violated"); break;
    }
  }
  else if DIR(W,B) {
    switch(ne) {
      case 0: trne = 5; break;
      case 2: trne = 7; break;
      default: assertStr(TFALSE, "TNetExtractor::diag_translate : assertStrion violated"); break;
    }
  }
  else if DIR(B,E) {
    switch(ne) {
      case 1: trne = 4; break;
      case 3: trne = 6; break;
      default: assertStr(TFALSE, "TNetExtractor::diag_translate : assertStrion violated"); break;
    }
  }
  return trne;
}

void TNetExtractor::encode(int nx, int ny, int nz, int& number, 
         unsigned char* nbc, numentry* num, int z)
{
   int y, x, i, j;
   unsigned char code;
  
   for(y=0;y<ny;y++) {
      for (x=0;x<nx;x++) {
   code = 0;
   for (i=0;i<8;i++) 
      code |= (*vol_)(x+i%2, y+i/2%2, z+i/4) & 1<<i;
   nbc[x+nx*y]=code;
   numentry* num_act = &num[x+nx*y];

   if (code==126) {
      num_act->nums[3] = num_act->nums[5] = num_act->nums[6] = number;
      number++;
      num_act->nums[1] = num_act->nums[2] = num_act->nums[4] = number;
      number++;
   }
      
   else if (code==189) {
      num_act->nums[0] = num_act->nums[3] = num_act->nums[5] = number;
      number++;
      num_act->nums[2] = num_act->nums[4] = num_act->nums[7] = number;
      number++;
   }

   else if (code==219) {
      num_act->nums[0] = num_act->nums[3] = num_act->nums[6] = number;
      number++;
      num_act->nums[1] = num_act->nums[4] = num_act->nums[7] = number;
      number++;
   }
     
   else if (code==231) {
      num_act->nums[0] = num_act->nums[5] = num_act->nums[6] = number;
      number++;
      num_act->nums[1] = num_act->nums[2] = num_act->nums[7] = number;
      number++;
   }
  
   else {
      for(i=0;i<pt1[code].count;i++) {
         unsigned char nc_act = pt1[code].nbcs[i];
         if (nc_act) {
      for (j=0;j<8;j++) {
         if (nc_act & (1 << j))
      num_act->nums[j] = number;
      }
      number++;
         }
      }
   }
      }
   }
}

TVertexList* TNetExtractor::extractnet()
{
   char progressMessage[80];

   int i,j, k, x, y, z, xy;
   int number = 0;
   int nx = vol_->Nx();
   int ny = vol_->Ny();
   int nz = vol_->Nz();

   int z_tab[]  = { 0,0,0,0,-1,1 };
   int xy_tab[] = { -1, 1, -nx, nx, 0, 0 };  

//    pt1 = new t1entry[256];
//    pt2 = new t2entry[256];

//    // read in tables:
//    ifstream t1stream("/usr/goomba/home2/brech/stud/schweikert/avs/source/table1.dat");
//    for (j = 0; j<256; j++) {
//       t1stream>>pt1[j].nbc;
//       t1stream>>pt1[j].count;
//       for (i = 0; i<pt1[j].count;i++)
//    t1stream>>pt1[j].nbcs[i];
//    }
  
//    int dirmath;
//    ifstream t2stream("/usr/goomba/home2/brech/stud/schweikert/avs/source/table2.dat");
//    for (j = 0; j<256; j++) {
//       t2stream>>pt2[j].nbc;
//       t2stream>>pt2[j].count;
//       for (i = 0; i<pt2[j].count;i++) {
//    t2stream>>dirmath;
//    pt2[j].dir[i] = dirmath-1; // account for Mathematica indices here
//       }
//       for (i = 0; i<pt2[j].count;i++)
//    t2stream>>pt2[j].ne[i];
//    }
  
   // allocate memory:  
   numentry* num[3];
   num[0] = new numentry[nx*ny*3];
   num[1] = num[0] + nx*ny;
   num[2] = num[1] + nx*ny;
   numentry* num_remember = num[0];
  
   // dbg
   for (i=0;i<nx*ny*3;i++)
      for (j=0;j<8;j++)
   num[0][i].nums[j] = -1;

   unsigned char *nbc_this, *nbc_next, *nbc_remember;
   nbc_this = new unsigned char[nx*ny*2];
   nbc_remember =  nbc_this;
   nbc_next = nbc_this + nx*ny;

   encode(nx, ny, nz, number, nbc_next, num[2], 0);

   TVertexList* vl = new TVertexList; // collect vertices in this list

   for (z=0; z<nz-1; z++) {
      sprintf(progressMessage, "%s%i", "Schicht:", z+1);
      report_progress(progressMessage,((nz-2)==0)? 50 : 100*z/(nz-2));

      numentry* help_i=num[0]; num[0]=num[1]; num[1]=num[2]; num[2]=help_i;
      unsigned char* help_u=nbc_this; nbc_this=nbc_next; nbc_next=help_u;
      
      if (z<nz-2)
   encode(nx,ny,nz,number,nbc_next,num[2],z+1);
      
      for (y=0;y<ny-1;y++) {
   for (x=0;x<nx-1;x++) {

      // nc is the nbc as encounterd in the input data volume
      unsigned char nc;
      if (nc = nbc_this[xy = x+nx*y]) {
            
         for(i=0;i<pt1[nc].count;i++) {

      // according  to table1, we split nc into pieces to get an "actual" nbc
      unsigned char nc_act = pt1[nc].nbcs[i];
      if (nc_act) {
         int wh_act[3];
         int count_act = 0;
         int neighb_act[14];
         wh_act[0] = x+1; wh_act[1] = y+1; wh_act[2] = z+1;
        
         int count2 = pt2[nc_act].count;
         assertStr(count2 > 0, "TNetExtractor::extractnet : count entry in table2.dat is <= 0");

         // the values where to look for the neighbor's numbers in the num array are precomputed
         // and stored in table pt2
         for(k=0;k<count2;k++) {

      int dir1 = pt2[nc_act].dir[k];
      int dir2 = pt2[nc_act].dir[(k+1)%count2];
      int ne1  = pt2[nc_act].ne[k];
      int ne2  = pt2[nc_act].ne[(k+1)%count2];

      int t_ne1 = direct_translate(dir1,ne1);
      int num_act = num[1+z_tab[dir1]][xy+xy_tab[dir1]].nums[t_ne1];
      assertStr(num_act >= 0, "TNetExtractor::extractnet : nums number choice (direct) is invalid");
      neighb_act[count_act++] = num_act;

      int t_ne2 = diag_translate(dir1,dir2,ne2);
      numentry dbg = num[1+z_tab[dir1]+z_tab[dir2]][xy+xy_tab[dir1]+xy_tab[dir2]];
      num_act = num[1+z_tab[dir1]+z_tab[dir2]]
         [xy+xy_tab[dir1]+xy_tab[dir2]].nums[t_ne2];
      assertStr(num_act >= 0, "TNetExtractor::extractnet : nums number choice (diagonal)  is invalid");
      neighb_act[count_act++] = num_act;
         }

         // save vertex
         TVertex v(wh_act, count_act, neighb_act);
         vl->append(v);

      }// if(nc_act)
         }
      }
   }
      }
   }
   
   delete [] nbc_remember;
   delete [] num_remember;;
//    delete [] pt1;
//    delete [] pt2;

   return vl;
}
