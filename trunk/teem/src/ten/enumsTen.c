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

#include "ten.h"
#include "privateTen.h"

/* -------------------------------------------------------------- */

char
_tenAnisoStr[TEN_ANISO_MAX+1][AIR_STRLEN_SMALL] = {
  "(unknown aniso)",
  "Cl1",
  "Cp1",
  "Ca1",
  "Clpmin1",
  "Cs1",
  "Ct1",
  "Cl2",
  "Cp2",
  "Ca2",
  "Clpmin2",
  "Cs2",
  "Ct2",
  "RA",
  "FA",
  "VF",
  "B",
  "Q",
  "R",
  "S",
  "Skew",
  "Mode",
  "Th",
  "Omega",
  "Det",
  "Tr",
  "eval0",
  "eval1",
  "eval2"
};

airEnum
_tenAniso = {
  "anisotropy metric",
  TEN_ANISO_MAX,
  _tenAnisoStr,  NULL,
  NULL,
  NULL, NULL,
  AIR_FALSE
};
airEnum *
tenAniso = &_tenAniso;

/* --------------------------------------------------------------------- */

char
_tenGageStr[][AIR_STRLEN_SMALL] = {
  "(unknown tenGage)",

  "tensor",
  "confidence",
  
  "trace",
  "B",
  "det",
  "S",
  "Q",
  "FA",
  "R",
  "mode",
  "theta",
  "modew",
  "omega",

  "evals",
  "eval0",
  "eval1",
  "eval2",
  "evecs",
  "evec0",
  "evec1",
  "evec2",

  "delnk2",
  "delnk3",
  "delnr1",
  "delnr2",
  "delnphi1",
  "delnphi2",
  "delnphi3",

  "tensor grad",
  "tensor grad mag",
  "tensor grad mag mag",

  "trace grad vec",
  "trace grad mag",
  "trace normal",

  "B grad vec",
  "B grad mag",
  "B normal",

  "det grad vec",
  "det grad mag",
  "det normal",

  "S grad vec",
  "S grad mag",
  "S normal",

  "Q grad vec",
  "Q grad mag",
  "Q normal",

  "FA grad vec",
  "FA grad mag",
  "FA normal",

  "R grad vec",
  "R grad mag",
  "R normal",

  "mode grad vec",
  "mode grad mag",
  "mode normal",

  "theta grad vec",
  "theta grad mag",
  "theta normal",

  "omega grad vec",
  "omega grad mag",
  "omega normal",

  "invariant K gradients",
  "invariant K gradient mags",
  "invariant R gradients",
  "invariant R gradient mags",
  "rotation tangents",
  "rotation tangent mags",

  "eigenvalue gradients",

  "Cl1",
  "Cp1",
  "Ca1",
  "Clpmin1",
  "Cl2",
  "Cp2",
  "Ca2",
  "Clpmin2",

  "hessian",
  "trace hessian",
  "B hessian",
  "det hessian",
  "S hessian",
  "Q hessian",

  "FA hessian",
  "FA hessian evals",
  "FA hessian eval 0",
  "FA hessian eval 1",
  "FA hessian eval 2",
  "FA hessian evecs",
  "FA hessian evec 0",
  "FA hessian evec 1",
  "FA hessian evec 2",
  "FA ridge surface",
  "FA valley surface",
  "FA laplacian",
  "FA 2nd DD",

  "R hessian",

  "mode hessian",
  "mode hessian evals",
  "mode hessian eval 0",
  "mode hessian eval 1",
  "mode hessian eval 2",
  "mode hessian evecs",
  "mode hessian evec 0",
  "mode hessian evec 1",
  "mode hessian evec 2",

  "omega hessian",
  "omega hessian evals",
  "omega hessian eval 0",
  "omega hessian eval 1",
  "omega hessian eval 2",
  "omega hessian evecs",
  "omega hessian evec 0",
  "omega hessian evec 1",
  "omega hessian evec 2",

  "trace gradvec dot evec0",
  "diffusionAngle(trace)",
  "diffusionFraction(trace)",
  "FA gradvec dot evec0",
  "diffusionAngle(FA)",
  "diffusionFraction(FA)",
  "omega gradvec dot evec0",
  "diffusionAngle(Omega)",
  "diffusionFraction(Omega)",

  "cov",

  "anisotropies"
};

char
_tenGageDesc[][AIR_STRLEN_MED] = {
  "(unknown tenGage item)",
  "tensor",
  "confidence",
  "trace",
  "B",
  "determinant",
  "S",
  "Q",
  "FA",
  "R",
  "mode",
  "theta",
  "warped mode",
  "omega",
  "3 eigenvalues",
  "eigenvalue 0",
  "eigenvalue 1",
  "eigenvalue 2",
  "3 eigenvectors",
  "eigenvector 0",
  "eigenvector 1",
  "eigenvector 2",
  "delnk2",   /* sorry */
  "delnk3",   /* sorry */
  "delnr1",   /* sorry */
  "delnr2",   /* sorry */
  "delnphi1", /* sorry */
  "delnphi2", /* sorry */
  "delnphi3", /* sorry */
  "tensor gradients",
  "tensor gradients magnitudes",
  "tensor gradients magnitude magnitudes",
  "trace grad vec",
  "trace grad mag",
  "trace normal",
  "B grad vec",
  "B grad mag",
  "B normal",
  "determinant grad vec",
  "determinant grad mag",
  "determinant normal",
  "S grad vec",
  "S grad mag",
  "S normal",
  "Q grad vec",
  "Q grad mag",
  "Q normal",
  "FA grad vec",
  "FA grad mag",
  "FA normal",
  "R grad vec",
  "R grad mag",
  "R normal",
  "mode grad vec",
  "mode grad mag",
  "mode normal",
  "theta grad vec",
  "theta grad mag",
  "theta normal",
  "omega grad vec",
  "omega grad mag",
  "omega normal",
  "invariant K gradients",
  "invariant K gradient mags",
  "invariant R gradients",
  "invariant R gradient mags",
  "rotation tangents",
  "rotation tangent mags",
  "eigenvalue gradients",
  "linear anisotropy (1)",
  "planar anisotropy (1)",
  "linear+planar anisotropy (1)",
  "linear anisotropy (2)",
  "planar anisotropy (2)",
  "linear+planar anisotropy (2)",
  "hessian",
  "trace hessian",
  "B hessian",
  "det hessian",
  "S hessian",
  "Q hessian",
  "FA hessian",
  "FA hessian evals",
  "FA hessian eval 0",
  "FA hessian eval 1",
  "FA hessian eval 2",
  "FA hessian evecs",
  "FA hessian evec 0",
  "FA hessian evec 1",
  "FA hessian evec 2",
  "FA ridge surface strength",
  "FA valley surface strength",
  "FA laplacian",
  "FA 2nd DD",
  "R hessian",
  "mode hessian",
  "mode hessian evals",
  "mode hessian eval 0",
  "mode hessian eval 1",
  "mode hessian eval 2",
  "mode hessian evecs",
  "mode hessian evec 0",
  "mode hessian evec 1",
  "mode hessian evec 2",
  "omega hessian",
  "omega hessian evals",
  "omega hessian eval 0",
  "omega hessian eval 1",
  "omega hessian eval 2",
  "omega hessian evecs",
  "omega hessian evec 0",
  "omega hessian evec 1",
  "omega hessian evec 2",
  "trace gradvec dot evec0",
  "diffusion angle of trace",
  "diffusion fraction of trace",
  "FA gradvec dot evec0",
  "diffusion angle of FA",
  "diffusion fraction of FA",
  "omega gradvec dot evec0",
  "diffusion angle of omega",
  "diffusion fraction of omega",
  "covariance",
  "anisotropies"
};

int
_tenGageVal[] = {
  tenGageUnknown,
  tenGageTensor,        /* "t", the reconstructed tensor: GT[7] */
  tenGageConfidence,    /* "c", first of seven tensor values: GT[1] */
  tenGageTrace,         /* "tr", trace of tensor: GT[1] */
  tenGageB,             /* "b": GT[1] */
  tenGageDet,           /* "det", determinant of tensor: GT[1] */
  tenGageS,             /* "s", square of frobenius norm: GT[1] */
  tenGageQ,             /* "q", (S - B)/9: GT[1] */
  tenGageFA,            /* "fa", fractional anisotropy: GT[1] */
  tenGageR,             /* "r", 9*A*B - 2*A^3 - 27*C: GT[1] */
  tenGageMode,          /* "mode", sqrt(2)*R/sqrt(Q^3): GT[1] */
  tenGageTheta,         /* "th", arccos(mode/sqrt(2))/AIR_PI: GT[1] */
  tenGageModeWarp,      /* */
  tenGageOmega,         /* */
  tenGageEval,          /* "eval", all eigenvalues of tensor : GT[3] */
  tenGageEval0,         /* "eval0", major eigenvalue of tensor : GT[1] */
  tenGageEval1,         /* "eval1", medium eigenvalue of tensor : GT[1] */
  tenGageEval2,         /* "eval2", minor eigenvalue of tensor : GT[1] */
  tenGageEvec,          /* "evec", major eigenvectors of tensor: GT[9] */
  tenGageEvec0,         /* "evec0", major eigenvectors of tensor: GT[3] */
  tenGageEvec1,         /* "evec1", medium eigenvectors of tensor: GT[3] */
  tenGageEvec2,         /* "evec2", minor eigenvectors of tensor: GT[3] */
  tenGageDelNormK2,
  tenGageDelNormK3,
  tenGageDelNormR1,
  tenGageDelNormR2,
  tenGageDelNormPhi1,
  tenGageDelNormPhi2,
  tenGageDelNormPhi3,
  tenGageTensorGrad,    /* "tg", all tensor component gradients: GT[21] */
  tenGageTensorGradMag,    /* "tgm" */
  tenGageTensorGradMagMag,    /* "tgmm" */
  tenGageTraceGradVec,  /* "trgv": gradient (vector) of trace: GT[3] */
  tenGageTraceGradMag,  /* "trgm": gradient magnitude of trace: GT[1] */
  tenGageTraceNormal,   /* "trn": normal of trace: GT[3] */
  tenGageBGradVec,      /* "bgv", gradient (vector) of B: GT[3] */
  tenGageBGradMag,      /* "bgm", gradient magnitude of B: GT[1] */
  tenGageBNormal,       /* "bn", normal of B: GT[3] */
  tenGageDetGradVec,    /* "detgv", gradient (vector) of Det: GT[3] */
  tenGageDetGradMag,    /* "detgm", gradient magnitude of Det: GT[1] */
  tenGageDetNormal,     /* "detn", normal of Det: GT[3] */
  tenGageSGradVec,      /* "sgv", gradient (vector) of S: GT[3] */
  tenGageSGradMag,      /* "sgm", gradient magnitude of S: GT[1] */
  tenGageSNormal,       /* "sn", normal of S: GT[3] */
  tenGageQGradVec,      /* "qgv", gradient vector of Q: GT[3] */
  tenGageQGradMag,      /* "qgm", gradient magnitude of Q: GT[1] */
  tenGageQNormal,       /* "qn", normalized gradient of Q: GT[3] */
  tenGageFAGradVec,     /* "fagv", gradient vector of FA: GT[3] */
  tenGageFAGradMag,     /* "fagm", gradient magnitude of FA: GT[1] */
  tenGageFANormal,      /* "fan", normalized gradient of FA: GT[3] */
  tenGageRGradVec,      /* "rgv", gradient vector of Q: GT[3] */
  tenGageRGradMag,      /* "rgm", gradient magnitude of Q: GT[1] */
  tenGageRNormal,       /* "rn", normalized gradient of Q: GT[3] */
  tenGageModeGradVec,   /* "mgv", gradient vector of mode: GT[3] */
  tenGageModeGradMag,   /* "mgm", gradient magnitude of mode: GT[1] */
  tenGageModeNormal,    /* "mn", normalized gradient of moe: GT[3] */
  tenGageThetaGradVec,  /* "thgv", gradient vector of theta: GT[3] */
  tenGageThetaGradMag,  /* "thgm", gradient magnitude of theta: GT[1] */
  tenGageThetaNormal,   /* "thn", normalized gradient of theta: GT[3] */
  tenGageOmegaGradVec,  /* */
  tenGageOmegaGradMag,  /* */
  tenGageOmegaNormal,   /* */
  tenGageInvarKGrads,
  tenGageInvarKGradMags,
  tenGageInvarRGrads,
  tenGageInvarRGradMags,
  tenGageRotTans,       /* "rts" */
  tenGageRotTanMags,    /* "rtms" */
  tenGageEvalGrads,     /* "evgs" */
  tenGageCl1,
  tenGageCp1,
  tenGageCa1,
  tenGageClpmin1,
  tenGageCl2,
  tenGageCp2,
  tenGageCa2,
  tenGageClpmin2,
  tenGageHessian,
  tenGageTraceHessian,
  tenGageBHessian,
  tenGageDetHessian,
  tenGageSHessian,
  tenGageQHessian,
  tenGageFAHessian,
  tenGageFAHessianEval,
  tenGageFAHessianEval0,
  tenGageFAHessianEval1,
  tenGageFAHessianEval2,
  tenGageFAHessianEvec,
  tenGageFAHessianEvec0,
  tenGageFAHessianEvec1,
  tenGageFAHessianEvec2,
  tenGageFARidgeSurfaceStrength,
  tenGageFAValleySurfaceStrength,
  tenGageFALaplacian,
  tenGageFA2ndDD,
  tenGageRHessian,
  tenGageModeHessian,
  tenGageModeHessianEval,
  tenGageModeHessianEval0,
  tenGageModeHessianEval1,
  tenGageModeHessianEval2,
  tenGageModeHessianEvec,
  tenGageModeHessianEvec0,
  tenGageModeHessianEvec1,
  tenGageOmegaHessianEvec2,
  tenGageOmegaHessian,
  tenGageOmegaHessianEval,
  tenGageOmegaHessianEval0,
  tenGageOmegaHessianEval1,
  tenGageOmegaHessianEval2,
  tenGageOmegaHessianEvec,
  tenGageOmegaHessianEvec0,
  tenGageOmegaHessianEvec1,
  tenGageOmegaHessianEvec2,
  tenGageTraceGradVecDotEvec0,
  tenGageTraceDiffusionAngle,
  tenGageTraceDiffusionFraction,
  tenGageFAGradVecDotEvec0,
  tenGageFADiffusionAngle,
  tenGageFADiffusionFraction,
  tenGageOmegaGradVecDotEvec0,
  tenGageOmegaDiffusionAngle,
  tenGageOmegaDiffusionFraction,
  tenGageCovariance,
  tenGageAniso
};

char
_tenGageStrEqv[][AIR_STRLEN_SMALL] = {
  "t", "tensor",
  "c", "conf",
  "tr", "trace",
  "b",
  "det",
  "s",
  "q",
  "fa",
  "r",
  "mode", "m",
  "th", "theta",
  "modew", "mw",
  "omega", "om",
  "eval",
  "eval0",
  "eval1",
  "eval2",
  "evec",
  "evec0",
  "evec1",
  "evec2",
  "delnk2",
  "delnk3",
  "delnr1",
  "delnr2",
  "delnphi1",
  "delnphi2",
  "delnphi3",
  "tg", "tensor grad",
  "tgm", "tensor grad mag",
  "tgmm", "tensor grad mag mag",
  "trgv", "tracegv", "trace grad vec",
  "trgm", "tracegm", "trace grad mag",
  "trn", "tracen", "trace normal",
  "bgv", "b grad vec",
  "bgm", "b grad mag",
  "bn", "b normal",
  "detgv", "det grad vec",
  "detgm", "det grad mag",
  "detn", "det normal",
  "sgv", "s grad vec",
  "sgm", "s grad mag",
  "sn", "s normal",
  "qgv", "q grad vec",
  "qgm", "q grad mag",
  "qn", "q normal",
  "fagv", "fa grad vec",
  "fagm", "fa grad mag",
  "fan", "fa normal",
  "rgv", "r grad vec",
  "rgm", "r grad mag",
  "rn", "r normal",
  "mgv", "mode grad vec",
  "mgm", "mode grad mag",
  "mn", "mode normal",
  "thgv", "th grad vec",
  "thgm", "th grad mag",
  "thn", "th normal",
  "omgv",
  "omgm",
  "omn",
  "ikgs",
  "ikgms",
  "irgs",
  "irgms",
  "rts", "rotation tangents",
  "rtms", "rotation tangent mags",
  "evgs", "eigenvalue gradients",
  "cl1",
  "cp1",
  "ca1",
  "clpmin1",
  "cl2",
  "cp2",
  "ca2",
  "clpmin2",
  "hess",
  "trhess",
  "bhess",
  "dethess",
  "shess",
  "qhess",
  "fahess",
  "fahesseval",
  "fahesseval0",
  "fahesseval1",
  "fahesseval2",
  "fahessevec",
  "fahessevec0",
  "fahessevec1",
  "fahessevec2",
  "farsurf",
  "favsurf",
  "falapl",
  "fa2d", "fa2dd",
  "rhess",
  "mhess",
  "mhesseval",
  "mhesseval0",
  "mhesseval1",
  "mhesseval2",
  "mhessevec",
  "mhessevec0",
  "mhessevec1",
  "mhessevec2",
  "omhess",
  "omhesseval",
  "omhesseval0",
  "omhesseval1",
  "omhesseval2",
  "omhessevec",
  "omhessevec0",
  "omhessevec1",
  "omhessevec2",
  "trgvdotevec0",
  "datr",
  "dftr",
  "fagvdotevec0",
  "dafa",
  "dffa",
  "omgvdotevec0",
  "daom",
  "dfom",
  "cov",
  "an", "aniso", "anisotropies",
  ""
};

int
_tenGageValEqv[] = {
  tenGageTensor, tenGageTensor,
  tenGageConfidence, tenGageConfidence,
  tenGageTrace, tenGageTrace,
  tenGageB,
  tenGageDet,
  tenGageS,
  tenGageQ,
  tenGageFA,
  tenGageR,
  tenGageMode, tenGageMode,
  tenGageTheta, tenGageTheta,
  tenGageModeWarp, tenGageModeWarp,
  tenGageOmega, tenGageOmega,
  tenGageEval,
  tenGageEval0,
  tenGageEval1,
  tenGageEval2,
  tenGageEvec,
  tenGageEvec0,
  tenGageEvec1,
  tenGageEvec2,
  tenGageDelNormK2,
  tenGageDelNormK3,
  tenGageDelNormR1,
  tenGageDelNormR2,
  tenGageDelNormPhi1,
  tenGageDelNormPhi2,
  tenGageDelNormPhi3,
  tenGageTensorGrad, tenGageTensorGrad,
  tenGageTensorGradMag, tenGageTensorGradMag,
  tenGageTensorGradMagMag, tenGageTensorGradMagMag,
  tenGageTraceGradVec, tenGageTraceGradVec, tenGageTraceGradVec,
  tenGageTraceGradMag, tenGageTraceGradMag, tenGageTraceGradMag,
  tenGageTraceNormal, tenGageTraceNormal, tenGageTraceNormal,
  tenGageBGradVec, tenGageBGradVec,
  tenGageBGradMag, tenGageBGradMag,
  tenGageBNormal, tenGageBNormal,
  tenGageDetGradVec, tenGageDetGradVec,
  tenGageDetGradMag, tenGageDetGradMag,
  tenGageDetNormal, tenGageDetNormal,
  tenGageSGradVec, tenGageSGradVec,
  tenGageSGradMag, tenGageSGradMag,
  tenGageSNormal, tenGageSNormal,
  tenGageQGradVec, tenGageQGradVec,
  tenGageQGradMag, tenGageQGradMag,
  tenGageQNormal, tenGageQNormal,
  tenGageFAGradVec, tenGageFAGradVec,
  tenGageFAGradMag, tenGageFAGradMag,
  tenGageFANormal, tenGageFANormal,
  tenGageRGradVec, tenGageRGradVec,
  tenGageRGradMag, tenGageRGradMag,
  tenGageRNormal, tenGageRNormal,
  tenGageModeGradVec, tenGageModeGradVec,
  tenGageModeGradMag, tenGageModeGradMag,
  tenGageModeNormal, tenGageThetaNormal,
  tenGageThetaGradVec, tenGageThetaGradVec,
  tenGageThetaGradMag, tenGageThetaGradMag,
  tenGageThetaNormal, tenGageThetaNormal,
  tenGageOmegaGradVec,
  tenGageOmegaGradMag,
  tenGageOmegaNormal,
  tenGageInvarKGrads,
  tenGageInvarKGradMags,
  tenGageInvarRGrads,
  tenGageInvarRGradMags,
  tenGageRotTans, tenGageRotTans,
  tenGageRotTanMags, tenGageRotTanMags,
  tenGageEvalGrads, tenGageEvalGrads,
  tenGageCl1,
  tenGageCp1,
  tenGageCa1,
  tenGageClpmin1,
  tenGageCl2,
  tenGageCp2,
  tenGageCa2,
  tenGageClpmin2,
  tenGageHessian,
  tenGageTraceHessian,
  tenGageBHessian,
  tenGageDetHessian,
  tenGageSHessian,
  tenGageQHessian,
  tenGageFAHessian,
  tenGageFAHessianEval,
  tenGageFAHessianEval0,
  tenGageFAHessianEval1,
  tenGageFAHessianEval2,
  tenGageFAHessianEvec,
  tenGageFAHessianEvec0,
  tenGageFAHessianEvec1,
  tenGageFAHessianEvec2,
  tenGageFARidgeSurfaceStrength,
  tenGageFAValleySurfaceStrength,
  tenGageFALaplacian,
  tenGageFA2ndDD, tenGageFA2ndDD,
  tenGageRHessian,
  tenGageModeHessian,
  tenGageModeHessianEval,
  tenGageModeHessianEval0,
  tenGageModeHessianEval1,
  tenGageModeHessianEval2,
  tenGageModeHessianEvec,
  tenGageModeHessianEvec0,
  tenGageModeHessianEvec1,
  tenGageModeHessianEvec2,
  tenGageOmegaHessian,
  tenGageOmegaHessianEval,
  tenGageOmegaHessianEval0,
  tenGageOmegaHessianEval1,
  tenGageOmegaHessianEval2,
  tenGageOmegaHessianEvec,
  tenGageOmegaHessianEvec0,
  tenGageOmegaHessianEvec1,
  tenGageOmegaHessianEvec2,

  tenGageTraceGradVecDotEvec0,
  tenGageTraceDiffusionAngle,
  tenGageTraceDiffusionFraction,

  tenGageFAGradVecDotEvec0,
  tenGageFADiffusionAngle,
  tenGageFADiffusionFraction,

  tenGageOmegaGradVecDotEvec0,
  tenGageOmegaDiffusionAngle,
  tenGageOmegaDiffusionFraction,

  tenGageCovariance,

  tenGageAniso, tenGageAniso, tenGageAniso
};

airEnum
_tenGage = {
  "tenGage",
  TEN_GAGE_ITEM_MAX+1,
  _tenGageStr, _tenGageVal,
  _tenGageDesc,
  _tenGageStrEqv, _tenGageValEqv,
  AIR_FALSE
};
airEnum *
tenGage = &_tenGage;

/* --------------------------------------------------------------------- */

char
_tenFiberTypeStr[][AIR_STRLEN_SMALL] = {
  "(unknown tenFiberType)",
  "evec0",
  "evec1",
  "evec2",
  "tensorline",
  "pureline",
  "zhukov"
};

char
_tenFiberTypeDesc[][AIR_STRLEN_MED] = {
  "unknown tenFiber type",
  "simply follow principal eigenvector",
  "follow medium eigenvector",
  "follow minor eigenvector",
  "Weinstein-Kindlmann tensorlines",
  "based on tensor multiplication only",
  "Zhukov\'s oriented tensors"
};

char
_tenFiberTypeStrEqv[][AIR_STRLEN_SMALL] = {
  "ev0", "evec0",
  "ev1", "evec1",
  "ev2", "evec2",
  "tline", "tensorline",
  "pline", "pureline",
  "z", "zhukov",
  ""
};

int
_tenFiberTypeValEqv[] = {
  tenFiberTypeEvec0, tenFiberTypeEvec0,
  tenFiberTypeEvec1, tenFiberTypeEvec1,
  tenFiberTypeEvec2, tenFiberTypeEvec2,
  tenFiberTypeTensorLine, tenFiberTypeTensorLine,
  tenFiberTypePureLine, tenFiberTypePureLine,
  tenFiberTypeZhukov, tenFiberTypeZhukov
};

airEnum
_tenFiberType = {
  "tenFiberType",
  TEN_FIBER_TYPE_MAX,
  _tenFiberTypeStr, NULL,
  _tenFiberTypeDesc,
  _tenFiberTypeStrEqv, _tenFiberTypeValEqv,
  AIR_FALSE
};
airEnum *
tenFiberType = &_tenFiberType;

/* ----------------------------------------------------------------------- */

char
_tenFiberStopStr[][AIR_STRLEN_SMALL] = {
  "(unknown tenFiberStop)",
  "aniso",
  "length",
  "steps",
  "confidence",
  "radius",
  "bounds",
  "stub"
};

char
_tenFiberStopStrEqv[][AIR_STRLEN_SMALL] = {
  "aniso",
  "length", "len",
  "steps",
  "confidence", "conf", "c",
  "radius",
  "bounds",
  "stub",
  ""
};

int
_tenFiberStopValEqv[] = {
  tenFiberStopAniso,
  tenFiberStopLength, tenFiberStopLength,
  tenFiberStopNumSteps,
  tenFiberStopConfidence, tenFiberStopConfidence, tenFiberStopConfidence,
  tenFiberStopRadius,
  tenFiberStopBounds,
  tenFiberStopStub,
};

char
_tenFiberStopDesc[][AIR_STRLEN_MED] = {
  "unknown tenFiber stop",
  "anisotropy went below threshold",
  "fiber length exceeded normalcy bounds",
  "number of steps along fiber too many",
  "tensor \"confidence\" value too low",
  "radius of curvature of path got too small",
  "fiber went outside bounding box",
  "neither fiber half got anywhere; fiber has single vert"
};

airEnum
_tenFiberStop = {
  "fiber stopping criteria",
  TEN_FIBER_STOP_MAX,
  _tenFiberStopStr, NULL,
  _tenFiberStopDesc,
  _tenFiberStopStrEqv, _tenFiberStopValEqv, 
  AIR_FALSE
};
airEnum *
tenFiberStop = &_tenFiberStop;

/* ----------------------------------------------------------------------- */

char
_tenFiberIntgStr[][AIR_STRLEN_SMALL] = {
  "(unknown tenFiberIntg)",
  "euler",
  "midpoint",
  "rk4"
};

char
_tenFiberIntgStrEqv[][AIR_STRLEN_SMALL] = {
  "euler",
  "midpoint", "rk2",
  "rk4",
  ""
};

int
_tenFiberIntgValEqv[] = {
  tenFiberIntgEuler,
  tenFiberIntgMidpoint, tenFiberIntgMidpoint,
  tenFiberIntgRK4
};

char
_tenFiberIntgDesc[][AIR_STRLEN_MED] = {
  "unknown tenFiber intg",
  "plain Euler",
  "midpoint method, 2nd order Runge-Kutta",
  "4rth order Runge-Kutta"
};

airEnum
_tenFiberIntg = {
  "fiber integration method",
  TEN_FIBER_INTG_MAX,
  _tenFiberIntgStr, NULL,
  _tenFiberIntgDesc,
  _tenFiberIntgStrEqv, _tenFiberIntgValEqv, 
  AIR_FALSE
};
airEnum *
tenFiberIntg = &_tenFiberIntg;

/* ----------------------------------------------------------------------- */

char
_tenGlyphTypeStr[][AIR_STRLEN_SMALL] = {
  "(unknown tenGlyphType)",
  "box",
  "sphere",
  "cylinder",
  "superquad"
};

#define BOX tenGlyphTypeBox
#define SPH tenGlyphTypeSphere
#define CYL tenGlyphTypeCylinder
#define SQD tenGlyphTypeSuperquad

char
_tenGlyphTypeStrEqv[][AIR_STRLEN_SMALL] = {
  "b", "box",
  "s", "sph", "sphere",
  "c", "cyl", "cylind", "cylinder",
  "q", "superq", "sqd", "superquad", "superquadric",
  ""
};

int
_tenGlyphTypeValEqv[] = {
  BOX, BOX,
  SPH, SPH, SPH,
  CYL, CYL, CYL, CYL,
  SQD, SQD, SQD, SQD, SQD
};

char
_tenGlyphTypeDesc[][AIR_STRLEN_MED] = {
  "unknown tenGlyph type",
  "box/cube (rectangular prisms)",
  "sphere (ellipsoids)",
  "cylinders aligned along major eigenvector",
  "superquadric (superellipsoids)"
};

airEnum
_tenGlyphType = {
  "tenGlyphType",
  TEN_GLYPH_TYPE_MAX,
  _tenGlyphTypeStr, NULL,
  _tenGlyphTypeDesc,
  _tenGlyphTypeStrEqv, _tenGlyphTypeValEqv, 
  AIR_FALSE
};
airEnum *
tenGlyphType = &_tenGlyphType;

/* ---------------------------------------------- */

char
_tenEstimate1MethodStr[][AIR_STRLEN_SMALL] = {
  "(unknown tenEstimate1Method)",
  "LLS",
  "WLS",
  "NLS",
  "MLE"
};

char
_tenEstimate1MethodDesc[][AIR_STRLEN_MED] = {
  "unknown tenEstimate1Method",
  "linear least-squares fit of log(DWI)",
  "weighted least-squares fit of log(DWI)",
  "non-linear least-squares fit of DWI",
  "maximum likelihood estimate from DWI"
};

airEnum
_tenEstimate1Method = {
  "single-tensor-estimation",
  TEN_ESTIMATE_1_METHOD_MAX,
  _tenEstimate1MethodStr, NULL,
  _tenEstimate1MethodDesc,
  NULL, NULL,
  AIR_FALSE
};
airEnum *
tenEstimate1Method= &_tenEstimate1Method;

/* ---------------------------------------------- */

char
_tenEstimate2MethodStr[][AIR_STRLEN_SMALL] = {
  "(unknown tenEstimate2Method)",
  "QSegLLS"
};

char
_tenEstimate2MethodDesc[][AIR_STRLEN_MED] = {
  "unknown tenEstimate2Method",
  "Q-ball segmentation"
};

airEnum
_tenEstimate2Method = {
  "two-tensor-estimation",
  TEN_ESTIMATE_2_METHOD_MAX,
  _tenEstimate2MethodStr, NULL,
  _tenEstimate2MethodDesc,
  NULL, NULL,
  AIR_FALSE
};
airEnum *
tenEstimate2Method= &_tenEstimate2Method;
