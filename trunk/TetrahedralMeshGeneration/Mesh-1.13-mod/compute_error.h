/* $Id: compute_error.h,v 1.33 2004/04/30 07:50:21 aspert Exp $ */


/*
 *
 *  Copyright (C) 2001-2004 EPFL (Swiss Federal Institute of Technology,
 *  Lausanne) This program is free software; you can redistribute it
 *  and/or modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2 of
 *  the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 *  USA.
 *
 *  In addition, as a special exception, EPFL gives permission to link
 *  the code of this program with the Qt non-commercial edition library
 *  (or with modified versions of Qt non-commercial edition that use the
 *  same license as Qt non-commercial edition), and distribute linked
 *  combinations including the two.  You must obey the GNU General
 *  Public License in all respects for all of the code used other than
 *  Qt non-commercial edition.  If you modify this file, you may extend
 *  this exception to your version of the file, but you are not
 *  obligated to do so.  If you do not wish to do so, delete this
 *  exception statement from your version.
 *
 *  Authors : Nicolas Aspert, Diego Santa-Cruz and Davy Jacquet
 *
 *  Web site : http://mesh.epfl.ch
 *
 *  Reference :
 *   "MESH : Measuring Errors between Surfaces using the Hausdorff distance"
 *   in Proceedings of IEEE Intl. Conf. on Multimedia and Expo (ICME) 2002, 
 *   vol. I, pp. 705-708, available on http://mesh.epfl.ch
 *
 */

 /*
  * The original version has been modified so that the dist_pt_surf() can be
  * called externally. The changes are localized to changing corresponding
  * functions from static to external.
  * Author: Andriy Fedorov, College of William and Mary, VA, USA
  * Date: 19 Sept 2005
  */






#ifndef _COMPUTE_ERROR_PROTO
#define _COMPUTE_ERROR_PROTO

/*
 * --------------------------------------------------------------------------*
 *                         External includes                                 *
 * --------------------------------------------------------------------------*
 */

#include <model_analysis.h>
#include <reporting.h>

#ifdef __cplusplus
#define BEGIN_DECL extern "C" {
#define END_DECL }
#else
#define BEGIN_DECL
#define END_DECL
#endif

BEGIN_DECL
#undef BEGIN_DECL

/* --------------------------------------------------------------------------*
 *                       Exported data types                                 *
 * --------------------------------------------------------------------------*/

#ifdef USE_EC_BITMAP /* Use a bitmap */
  /* Type for storing empty cell bitmap */
  typedef int ec_bitmap_t;
  /* The number of bytes used by ec_bitmap_t */
# define EC_BITMAP_T_SZ (sizeof(ec_bitmap_t))
  /* The number of bits used by ec_bitmap_t, and also the divisor to obtain the
   * bitmap element index from a cell index */
# define EC_BITMAP_T_BITS (EC_BITMAP_T_SZ*8)
  /* Bitmask to obtain the bitmap bit index from the cell index. */
# define EC_BITMAP_T_MASK (EC_BITMAP_T_BITS-1)
  /* Macro to test the bit corresponding to element i in the bitmap bm. */
# define EC_BITMAP_TEST_BIT(bm,i) \
   ((bm)[(i)/EC_BITMAP_T_BITS]&(1 << ((i)&EC_BITMAP_T_MASK)))
  /* Macro to set the bit corresponding to element i in the bitmap bm. */
# define EC_BITMAP_SET_BIT(bm,i) \
   ((bm)[(i)/EC_BITMAP_T_BITS] |= 1 << ((i)&EC_BITMAP_T_MASK))
#else /* Fake bitmap macros to access simple type */
  /* Type for marking empty cells. Small type uses less memory, but access to
   * aligned type can be faster (but more memory can cause more cache misses) */
  typedef char ec_bitmap_t;
  /* The number of bytes used by ec_bitmap_t */
# define EC_BITMAP_T_SZ sizeof(ec_bitmap_t)
  /* The number of bits used by ec_bitmap_t. Since here we don't use the
   * individual bits it is 1. */
# define EC_BITMAP_T_BITS 1
  /* Bitmask to obtain the bitmap bit index from the cell index. */
# define EC_BITMAP_T_MASK (EC_BITMAP_T_BITS-1)
  /* Macro to test the element i in the map bm. */
# define EC_BITMAP_TEST_BIT(bm,i) (bm)[i]
  /* Macro to set the element i in the map bm. */
# define EC_BITMAP_SET_BIT(bm,i) ((bm)[i] = 1)
#endif

/* A integer size in 3D */
struct size3d {
  int x; /* Number of elements in the X direction */
  int y; /* Number of elements in the Y direction */
  int z; /* Number of elements in the Z direction */
};

/* Per face error metrics */
struct face_error {
  double face_area;      /* Area of the face, for error weighting in averages */
  double min_error;      /* The minimum error for the face */
  double max_error;      /* The maximum error for the face */
  double mean_error;     /* The mean error for the face */
  double mean_sqr_error; /* The mean squared error for the face */
  double *serror;        /* The error at each sample of the face. For a
                          * triangle with vertices v0 v1 and v2 (in that
                          * order) the samples (i,j) appear in the following
                          * order. First the errors at samples with i equal 0
                          * and j from 0 to sample_freq-1, followed by errors
                          * at samples i equal 1 and j from 0 to
                          * sample_freq-2, and so on. The index i corresponds
                          * to the v0-v1 direction and j to the v0-v2
                          * direction. If sample_freq is larger than 1, the
                          * error at v0 is serror[0], at v1 is
                          * serror[sample_freq*(sample_freq+1)/2-1] and at v2
                          * is serror[sample_freq-1]. */
  int sample_freq;       /* The sampling frequency for this triangle. If zero,
                          * no error was calculated. The number of samples is
                          * sample_freq*(sample_freq+1)/2. */
};

/* Model and error, plus miscellaneous model properties */
struct model_error {
  double min_error;       /* The minimum error value (at sample) */
  double max_error;       /* The maximum error value (at sample) */
  double mean_error;      /* The mean error value */
  struct model *mesh;     /* The 3D model mesh */
  int n_samples;          /* Number of samples used to calculate the error */
  struct face_error *fe;  /* The per-face error metrics. NULL if not
                           * present. The fe[i].serror arrays are all parts of
                           * one array, starting at fe[0].serror and can thus
                           * be accessed linearly. */
  float *verror;          /* The per vertex error array. NULL if not
                           * present. */
  struct model_info *info;/* The model information. NULL if not present. */
};

/* Statistics from the dist_surf_surf function */
struct dist_surf_surf_stats {
  double st_m1_area;/* Total area of sampled triangles of model 1 */
  double m1_area;   /* Area of model 1 surface */
  double m2_area;   /* Area of model 2 surface */
  double min_dist;  /* Minimum distance from model 1 to model 2 */
  double max_dist;  /* Maximum distance from model 1 to model 2 */
  double mean_dist; /* Mean distance from model 1 to model 2 */
  double rms_dist;  /* Root mean squared distance from model 1 to model 2 */
  double cell_sz;   /* The partitioning cubic cell side length */
  double n_t_p_nec; /* Average number of triangles per non-empty cell */
  int m1_samples;   /* Total number of samples taken on model 1 */
  struct size3d grid_sz; /* The number of cells in the partitioning grid in
                          * each direction X,Y,Z */
  int n_ne_cells;   /* Number of non-empty cells */
};

/* A triangle and useful associated information. AB is always the longest side
 * of the triangle. That way the projection of C on AB is always inside AB. */
struct triangle_info {
  dvertex_t a;         /* The A vertex of the triangle */
  dvertex_t b;         /* The B vertex of the triangle */
  dvertex_t c;         /* The C vertex of the triangle. The projection of C
                        * on AB is always inside the AB segment. */
  dvertex_t ab;        /* The AB vector */
  dvertex_t ca;        /* The CA vector */
  dvertex_t cb;        /* The CB vector */
  double ab_len_sqr;   /* The square of the length of AB */
  double ca_len_sqr;   /* The square of the length of CA */
  double cb_len_sqr;   /* The square of the length of CB */
  double ab_1_len_sqr; /* One over the square of the length of AB */
  double ca_1_len_sqr; /* One over the square of the length of CA */
  double cb_1_len_sqr; /* One over the square of the length of CB */
  dvertex_t normal;    /* The (unit length) normal of the ABC triangle
                        * (orinted with the right hand rule turning from AB to
                        * AC). If the triangle is degenerate it is (0,0,0). */
  dvertex_t nhsab;     /* (unnormalized) normal of the plane trough AB,
                        * perpendicular to ABC and pointing outside of ABC */
  dvertex_t nhsbc;     /* (unnormalized) normal of the plane trough BC,
                        * perpendicular to ABC and pointing outside of ABC */
  dvertex_t nhsca;     /* (unnormalized) normal of the plane trough CA,
                        * perpendicular to ABC and pointing outside of ABC */
  double chsab;        /* constant of the plane equation: <p|npab>=cpab */
  double chsbc;        /* constant of the plane equation: <p|npbc>=cpbc */
  double chsca;        /* constant of the plane equation: <p|npca>=cpca */
  double a_n;          /* scalar product of A with the unit length normal */
  double s_area;       /* The surface area of the triangle */
  int obtuse_at_c;     /* Flag indicating if the angle at C is larger than 90
                        * degrees (i.e. obtuse) */
};

/* A list of cells */
struct cell_list {
  int *cell;   /* The array of the linear indices of the cells in the list */
  int n_cells; /* The number of elemnts in the array */
};

/* A list of cells for different distances */
struct dist_cell_lists {
  struct cell_list *list; /* list[k]: the list of cells at distance k in the
                           * X, Y or Z direction. */
  int n_dists;            /* The number of elements in list */
};

/* A list of triangles with their associated information */
struct triangle_list {
  struct triangle_info *triangles; /* The triangles */
  int n_triangles;                 /* The number of triangles */
  double area;                     /* The total triangle area */
};

/* List of triangles intersecting each cell */
struct t_in_cell_list {
  int ** triag_idx;         /* The list of the indices of the triangles
                             * intersecting each cell, terminated by -1
                             * (triag_idx[i] is the list for the cell with
                             * linear index i). If cell i is empty,
                             * triag_idx[i] is NULL. */
  int n_cells;              /* The number of cells in triag_idx */
  ec_bitmap_t *empty_cell;  /* A bitmap indicating which cells are empty. If
                             * cell i is empty, the bit (i&EC_BITMAP_T_MASK)
                             * of empty_cell[i/EC_BITMAP_T_BITS] is
                             * non-zero. */
  int n_ne_cells;           /* The number of non-empty cells */
  double n_t_per_ne_cell;   /* Average number of triangles per non-empty cell */
};

/* --------------------------------------------------------------------------*
 *                       Exported functions                                  *
 * --------------------------------------------------------------------------*/

/* Calculates the distance from model me1->mesh (m1) to model m2. The
 * triangles of m1 are sampled so that the sampling density (number of samples
 * per unit surface) is sampling_density. If min_sample_freq is non-zero, all
 * triangles must have at least min_sample_freq as their sample frequency,
 * even if the specified sampling density is too low for that. The per face
 * (of m1) error metrics are returned in a new array (of length m1->num_faces)
 * allocated at me1->fe. The overall distance metrics and other statistics are
 * returned in stats. Optionally, if calc_normals is non-zero and m2 has no
 * normals or face normals, the normals will be calculated and added to m2
 * (only normals, not face normals). The normals are calculated assuming that
 * the model m2 is oriented, if it is not the case the resulting normals can
 * be incorrect. Information already used to calculate the distance is reused
 * to compute the normals, so it is very fast. If prog in not NULL it is used
 * for reporting progress. The memory allocated at me1->fe should be freed by
 * calling free_face_error(me1->fe). Note that non-zero values for
 * min_sample_freq distort the uniform distribution of error samples. */
void dist_surf_surf(struct model_error *me1, struct model *m2, 
  double sampling_density, int min_sample_freq,
                    struct dist_surf_surf_stats *stats, int calc_normals,
                    struct prog_reporter *prog);


/* Frees the memory allocated by dist_surf_surf() for the per face error
 * metrics. */
void free_face_error(struct face_error *fe);

/* Stores the error values at each vertex in the me->verror array (realloc'ed
 * to the correct size), given the per face error metrics in me->fe. A
 * negative error (special flag) is assigned to vertices for which there are
 * no sample points. The number of vertices and faces without error samples is
 * returned in *nv_empty and *nf_empty, respectively. */
void calc_vertex_error(struct model_error *me, int *nv_empty, int *nf_empty);

/* The following functions were 'static' in the original code */
void init_triangle(const vertex_t *a, const vertex_t *b, 
                          const vertex_t *c, struct triangle_info *t);

struct triangle_list* model_to_triangle_list(const struct model *m);
  
double get_cell_size(const struct triangle_list *tl,
                            const dvertex_t *bbox_min,
                            const dvertex_t *bbox_max, struct size3d *grid_sz);
  
struct t_in_cell_list* 
triangles_in_cells(const struct triangle_list *tl,
                   struct size3d grid_sz,
                   double cell_sz,
                   dvertex_t bbox_min);

double dist_pt_surf(dvertex_t p, const struct triangle_list *tl,
                           const struct t_in_cell_list *fic,
#ifdef DO_DIST_PT_SURF_STATS
                           struct dist_pt_surf_stats *stats,
#endif
                           struct size3d grid_sz, double cell_sz,
                           dvertex_t bbox_min, struct dist_cell_lists *dcl,
                           const dvertex_t *prev_p, double prev_d,
                           int **dcl_buf, int *dcl_buf_sz);

END_DECL
#undef END_DECL

#endif /* _COMPUTE_ERROR_PROTO */
