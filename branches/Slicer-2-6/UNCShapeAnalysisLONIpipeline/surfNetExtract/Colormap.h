// -*- C++ -*-
/****************************************************************************
 *
 *  Colormap
 *
 ****************************************************************************
 *
 *  File         :  Colormap.h
 *  Type         :  from tkoller
 *  Purpose      :  convert AVS colormap (HSV) to RGB
 *
 ****************************************************************************
 *                 
 *  started      :  12 Jan 95     Robert Schweikert
 *  last change  :  12 Jan 95
 *
 ****************************************************************************/

#ifndef COLORMAP_H
#define  COLORMAP_H

struct RGBcolormap {
   int    size;              // number of entries in each array 
   float  lower;             // 0th entry maps to this value
   float  upper;             // size-th entry maps to this value
   float* r;
   float* g;
   float* b;
   float* alpha;
};

#define RGBcmapIndex(cmap, val)                        \
   ((val) <= (cmap).lower ? 0 :                        \
    ((val) >= (cmap).upper ? (cmap).size-1 :           \
     (int) ((((val) - (cmap).lower) /                  \
             ((cmap).upper - (cmap).lower)) * (((cmap).size)-1))))
   
#ifndef NO_AVS
#include "common_avs.h"
void ConvertColormap(AVScolormap*, RGBcolormap*);
#endif NO_AVS

#endif
