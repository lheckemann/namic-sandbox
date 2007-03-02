/*
  Teem: Tools to process and visualize scientific data and images
  Copyright (C) 2006, 2005  Gordon Kindlmann
  Copyright (C) 2004, 2003, 2002, 2001, 2000, 1999, 1998  University of Utah

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public License
  (LGPL) as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  The terms of redistributing and/or modifying this software also
  include exceptions to the LGPL that facilitate static linking.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef TEN_PRIVATE_HAS_BEEN_INCLUDED
#define TEN_PRIVATE_HAS_BEEN_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#define TEND_CMD(name, info) \
unrrduCmd tend_##name##Cmd = { #name, info, tend_##name##Main }

/* USAGE, PARSE: both copied verbatim from unrrdu/privateUnrrdu.h */

#define USAGE(info) \
  if (!argc) { \
    hestInfo(stderr, me, (info), hparm); \
    hestUsage(stderr, hopt, me, hparm); \
    hestGlossary(stderr, hopt, hparm); \
    airMopError(mop); \
    return 2; \
  }

#define PARSE() \
  if ((pret=hestParse(hopt, argc, argv, &perr, hparm))) { \
    if (1 == pret) { \
      fprintf(stderr, "%s: %s\n", me, perr); free(perr); \
      hestUsage(stderr, hopt, me, hparm); \
      airMopError(mop); \
      return 2; \
    } else { \
      /* ... like tears ... in rain. Time ... to die. */ \
      exit(1); \
    } \
  }


/* For 2-tensor estimation */
extern void qball( const double b, const int gradcount, const double svals[], const double grads[], double qvals[] );
extern void segsamp2( const int gradcount, const double qvals[], const double grads[], const double qpoints[], unsigned int seg[], double dists[] );
extern void calcdists( const int centcount, const double centroid[6], const int gradcount, const double qpoints[], double dists[] );
extern void initcent2( const int gradcount, const double qvals[], const double grads[], double centroids[6] );
extern int calccent2( const int gradcount, const double qpoints[], const double dists[], double centroid[6], unsigned int seg[] );
extern void seg2weights( const int gradcount, const int seg[], const int segcount, double weights[] );
extern void qvals2points( const int gradcount, const double qvals[], const double grads[], double qpoints[] );
extern double pldist( const double point[], const double line[] );

/* For 2-tensor tracking */
extern tenFiberContext *tenFiberContextDwiNew(const Nrrd *dtvol);

#ifdef __cplusplus
}
#endif

#endif /* TEN_PRIVATE_HAS_BEEN_INCLUDED */
