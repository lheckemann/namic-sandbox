
#include "malloc.h"

#define MAX_CLUST_NO (256*256-2)

struct cldsc {
  int clsize;
  unsigned short clid;
  int rank;
};

int clcomp(struct cldsc *first,struct cldsc *second)
{

  if(first->clsize > second->clsize) {
    return(-1);
  }
  else if(first->clsize < second->clsize) {
    return(1);
  }
  else {
    return(0);
  }
}

int clback(struct cldsc *first,struct cldsc *second)
{

  if(first->clid > second->clid) {
    return(1);
  }
  else if(first->clid < second->clid) {
    return(-1);
  }
  else {
    return(0);
  }
}

int clmerge(unsigned short *clustno,int *clid,int nclust,int ind1,int ind2)
{
  int cl1, cl2;
  int i;

  cl1 = clustno[ind1];
  cl2 = clustno[ind2];
  if(clid[cl1-1]) {
    cl1 = clid[cl1-1];
  }
  if(clid[cl2-1]) {
    cl2 = clid[cl2-1];
  }
  if(cl1 == cl2) {
    return;
  }
  if(clid[cl1-1] || clid[cl2-1]) {
    printf("cluster bookkeeping error\n");
    exit(-122);
  }
  clid[cl2-1] = cl1;
  for(i=0;i<nclust;i++) {
    if(clid[i] == cl2) {
      clid[i] = cl1;
    }
  }
  return;
}

int clanal(int  npts,char *in,unsigned short  *out,int  width,int  height,int metric,char **answer)
{
  int nclust, nrclust;
  int i, j, k, depth;
  int clust;
  int *clsize;
  int *clid;
  struct cldsc *clusters;
  int offset[13];
  int upper;
  char buf[128];
  char *answ;
  int uplim;
  int proc;
  char status[64];

  if(!(clid=(int *) malloc(MAX_CLUST_NO*sizeof(int)))) {
    return(-2);
  }
  if(!(clsize=(int *) malloc(MAX_CLUST_NO*sizeof(int)))) {
    free(clid);
    return(-2);
  }
  if(!(clusters=(struct cldsc *) malloc(MAX_CLUST_NO*sizeof(struct cldsc)))) {
    free(clid);
    free(clsize);
    return(-2);
  }

  /*** set border of input image to zero ***/

  depth=npts/(width*height);
  for (i=0;i<width;i++) {
    for (j=0;j<height;j++) {
      in[i+j*width]=0;
      in[i+j*width+(depth-1)*width*height]=0;
    }
  }
  for (i=0;i<width;i++) {
    for (k=0;k<depth;k++) {
      in[i+k*width*height]=0;
      in[i+(height-1)*width+k*width*height]=0;
    }
  }
  for (j=0;j<height;j++) {
    for (k=0;k<depth;k++) {
      in[j*width+k*width*height]=0;
      in[width-1+j*width+k*width*height]=0;
    }
  }
  for(i=0;i<npts;i++) {
    out[i] = 0;
  }
  offset[0] = 1;
  offset[1] = width;
  offset[2] = width*height;
  offset[3] = 1-width*height;
  offset[4] = 1+width*height;
  offset[5] = 1+width;
  offset[6] = width-width*height;
  offset[7] = width+width*height;
  offset[8] = -1+width;
  offset[9] = 1+width-width*height;
  offset[10] = 1+width+width*height;
  offset[11] = -1+width-width*height;
  offset[12] = -1+width+width*height;
  upper = metric / 2;
  for(i=0;i<MAX_CLUST_NO;i++) {
    clsize[i] = 0;
    clid[i] = 0;
  }
  nclust = 0;
  proc = 1;
  uplim = (int) ((float) proc * (float) npts / 100.0);
  for(i=0;i<npts;i++) {
    if(i > uplim) {
      proc++;
      uplim = (int) ((float) proc * (float) npts / 100.0);
    }
    if (in[i]) { 
    if(!out[i]) {
      clust = (-1);
      for(j=0;j<upper;j++) {
        if(out[i+offset[j]]) {
          if(clust >= 0) {
            clmerge(out,clid,nclust,clust,i+offset[j]);
          }
          else {
            clust = i+offset[j];
          }
        }
        if(out[i-offset[j]]) {
          if(clust >= 0) {
            clmerge(out,clid,nclust,clust,i-offset[j]);
          }
          else {
            clust = i-offset[j];
          }
        }
      }
      if(clust < 0) {
        nclust++;
        if(nclust > MAX_CLUST_NO) {
          free(clid);
          free(clsize);
          free(clusters);
          return(-1);
        }
        out[i] = nclust;
      }
      else {
        out[i] = out[clust];
      }
    }
  }
  }
  for(i=0,nrclust=0;i<nclust;i++) {
    if(!clid[i]) {
      clid[i] = -(++nrclust);
    }
  }
  proc = 1;
  uplim = (int) ((float) proc * (float) npts / 100.0);
  for(i=0;i<npts;i++) {
    if(i > uplim) {
      proc++;
      uplim = (int) ((float) proc * (float) npts / 100.0);
    }
    clust = out[i];
    if(clust) {
      if(clid[clust-1] > 0) {
        clust = clid[clust-1];
      }
      if(clid[clust-1] >= 0) {
        free(clid);
        free(clsize);
        free(clusters);
        return(-3);
      }
      out[i] = -clid[clust-1];
    };
  }
  for(i=0;i<nrclust;i++) {
    clsize[i] = 0;
  }
  for(i=0;i<npts;i++) {
    if (out[i]) {
      clsize[out[i]-1]++;
    }
  }
  for(i=0;i<nrclust;i++) {
    clusters[i].clid = i + 1;
    clusters[i].clsize = clsize[i];
    clusters[i].rank = 0;
  }
  qsort((char *) clusters,nrclust,sizeof(struct cldsc),clcomp);
  for(i=0;i<nrclust;i++) {
    clusters[i].rank = i + 1;
  }
  if(!(answ=(char *) malloc(nrclust*128*sizeof(char)))) {
    free(clid);
    free(clsize);
    free(clusters);
    return(-2);
  }
  answ[0] = '\0';
  for(i=0;i<nrclust;i++) {
    sprintf(buf,"%d. cluster %d\n", i+1, clusters[i].clsize);
    strcat(answ,buf);
  }
  qsort((char *) clusters,nrclust,sizeof(struct cldsc),clback);
  proc = 1;
  uplim = (int) ((float) proc * (float) npts / 100.0);
  for(i=0;i<npts;i++) {
    if(i > uplim) {
      proc++;
      uplim = (int) ((float) proc * (float) npts / 100.0);
    }
    if (out[i]) {
      out[i] = clusters[out[i]-1].rank;
    }
  }
  free(clid);
  free(clsize);
  free(clusters);
  *answer = answ;
  return(nrclust);
}


int
DoConnectedComponentLabeling(unsigned char * image, int *dim, int metric) 
  // calls the connected component labeling code
  // modifies image (result is stored in image)
  // metric is either 6,18,or 26
{

  unsigned short *tempo;
  unsigned char *tempi;
  char *reslist;
  int metr;
  int i, j, k;
  int ret, res;
  int extlng;
  int indi, indo;

  if (metric != 6 && metric !=18 &&  metric)
    metric = 6;
  
  extlng = (dim[0]+2)*(dim[1]+2)*(dim[2]+2);
  if(!(tempi = (unsigned char *) malloc(extlng*sizeof(unsigned char)))) {
    printf("Memory allocation error"); return(-1);
  }
  if(!(tempo = (unsigned short *) malloc(extlng*sizeof(unsigned short)))) {
    printf("Memory allocation error"); return(-1);
  }
  for(i=0;i<extlng;i++) {
    tempi[i] = (unsigned char) 0;
  }
  for(k=0;k<dim[2];k++) {
    for(j=0;j<dim[1];j++) {
      indi = 1+(dim[0]+2)*(j+1+(dim[1]+2)*(k+1));
      indo = dim[0]*(j+dim[1]*k);
      for(i=0;i<dim[0];i++) {
        tempi[i+indi] = image[i+indo];
      }
    }
  }
  ret = clanal(extlng, tempi, tempo, dim[0]+2, dim[1]+2, metric, &reslist);
  switch(ret) {
    case 0: printf("Input field is empty (no component)"); return(-1);
    case -1: printf("Too many components (max. is 20000)"); return(-1);
    case -2: printf("Memory allocation error"); return(-1);
    case -3: printf("Internal program error in class handling"); return(-1);
    default: break;
  }
  for(k=0;k<dim[2];k++) {
    for(j=0;j<dim[1];j++) {
      indi = 1+(dim[0]+2)*(j+1+(dim[1]+2)*(k+1));
      indo = dim[0]*(j+dim[1]*k);
      for(i=0;i<dim[0];i++) {
        res = tempo[i+indi];
        if(res > 254) {
          res = 255;
        }
        image[i+indo] = (unsigned char) res;
      }
    }
  }
  
  return 0;
}
