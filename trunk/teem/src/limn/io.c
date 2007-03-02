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


#include "limn.h"

int
limnObjectDescribe(FILE *file, limnObject *obj) {
  limnFace *face; unsigned int si, fii;
  limnEdge *edge; unsigned int eii;
  limnVertex *vert; unsigned int vii;
  limnPart *part; unsigned int partIdx;
  limnLook *look;
  
  fprintf(file, "parts: %d\n", obj->partNum);
  for (partIdx=0; partIdx<obj->partNum; partIdx++) {
    part = obj->part[partIdx];
    fprintf(file, "part %d | verts: %d ========\n", partIdx, part->vertIdxNum);
    for (vii=0; vii<part->vertIdxNum; vii++) {
      vert = obj->vert + part->vertIdx[vii];
      fprintf(file, "part %d | %d(%d): w=(%g,%g,%g)\n", 
              partIdx, vii, part->vertIdx[vii], 
              vert->world[0], vert->world[1], vert->world[2]);
      /* vert->view[0], vert->view[1], vert->view[2]); */
      /* vert->screen[0], vert->screen[1], vert->screen[2]); */
    }
    fprintf(file, "part %d | edges: %d ========\n", partIdx, part->edgeIdxNum);
    for (eii=0; eii<part->edgeIdxNum; eii++) {
      edge = obj->edge + part->edgeIdx[eii];
      fprintf(file, "part %d==%d | %d(%d): "
              "vert(%d,%d), face(%d,%d)\n", 
              partIdx, edge->partIdx, eii, part->edgeIdx[eii],
              edge->vertIdx[0], edge->vertIdx[1],
              edge->faceIdx[0], edge->faceIdx[1]);
    }
    fprintf(file, "part %d | faces: %d ========\n", partIdx, part->faceIdxNum);
    for (fii=0; fii<part->faceIdxNum; fii++) {
      face = obj->face + part->faceIdx[fii];
      fprintf(file, "part %d==%d | %d(%d): [", 
              partIdx, face->partIdx, fii, part->faceIdx[fii]);
      for (si=0; si<face->sideNum; si++) {
        fprintf(file, "%d", face->vertIdx[si]);
        if (si < face->sideNum-1) {
          fprintf(file, ",");
        }
      }
      fprintf(file, "]; wn = (%g,%g,%g)", face->worldNormal[0],
              face->worldNormal[1], face->worldNormal[2]);
      look = obj->look + face->lookIdx;
      fprintf(file, "; RGB=(%g,%g,%g)", 
              look->rgba[0], look->rgba[1], look->rgba[2]);
      fprintf(file, "\n");
    }
  }

  return 0;
}

int
limnObjectOFFWrite(FILE *file, limnObject *obj) {
  char me[]="limnObjectOFFWrite", err[BIFF_STRLEN];
  unsigned int si;
  limnVertex *vert; unsigned int vii;
  limnFace *face; unsigned int fii;
  limnPart *part; unsigned int partIdx;
  
  if (!( obj && file )) {
    sprintf(err, "%s: got NULL pointer", me);
    biffAdd(LIMN, err); return 1;
  }
  fprintf(file, "OFF # created by Teem/limn\n");
  fprintf(file, "%d %d %d\n", obj->vertNum, obj->faceNum, obj->edgeNum);

  /* write vertices */
  for (partIdx=0; partIdx<obj->partNum; partIdx++) {
    fprintf(file, "### LIMN BEGIN PART %d\n", partIdx);
    part = obj->part[partIdx];
    for (vii=0; vii<part->vertIdxNum; vii++) {
      vert = obj->vert + part->vertIdx[vii];
      fprintf(file, "%g %g %g",
              vert->world[0]/vert->world[3],
              vert->world[1]/vert->world[3],
              vert->world[2]/vert->world[3]);
      /* verts no longer have a lookIdx
      if (vert->lookIdx) {
        fprintf(file, " %g %g %g",
                obj->look[vert->lookIdx].rgba[0],
                obj->look[vert->lookIdx].rgba[1],
                obj->look[vert->lookIdx].rgba[2]);
      }
      */
      fprintf(file, "\n");
    }
  }

  /* write faces */
  for (partIdx=0; partIdx<obj->partNum; partIdx++) {
    fprintf(file, "### LIMN BEGIN PART %d\n", partIdx);
    part = obj->part[partIdx];
    for (fii=0; fii<part->faceIdxNum; fii++) {
      face = obj->face + part->faceIdx[fii];
      fprintf(file, "%d", face->sideNum);
      for (si=0; si<face->sideNum; si++) {
        fprintf(file, " %d", face->vertIdx[si]);
      }
      if (face->lookIdx) {
        fprintf(file, " %g %g %g",
                obj->look[face->lookIdx].rgba[0],
                obj->look[face->lookIdx].rgba[1],
                obj->look[face->lookIdx].rgba[2]);
      }
      fprintf(file, "\n");
    }
  }

  return 0;
}

int
limnPolyDataIVWrite(FILE *file, const limnPolyData *pld) {
  char me[]="limnPolyDataIVWrite", err[BIFF_STRLEN];
  unsigned int primIdx, xyzwIdx, rgbaIdx, normIdx, bitFlag,
    baseVertIdx;
  int haveStrips, haveTris, haveElse;
  float xyz[3], norm[3], len;

  if (!(file && pld)) {
    sprintf(err, "%s: got NULL pointer", me);
    biffAdd(LIMN, err); return 1;
  }
  haveStrips = haveTris = haveElse = AIR_FALSE;
  for (primIdx=0; primIdx<pld->primNum; primIdx++) {
    int isTri, isStrip, isElse;
    isTri = limnPrimitiveTriangles == pld->type[primIdx];
    isStrip = limnPrimitiveTriangleStrip == pld->type[primIdx];
    isElse = !(isTri || isStrip);
    haveTris |= isTri;
    haveStrips |= isStrip;
    haveElse |= isElse;
    if (isElse) {
      sprintf(err, "%s: sorry, can only have %s or %s prims (prim[%u] is %s)",
              me, airEnumStr(limnPrimitive, limnPrimitiveTriangles),
              airEnumStr(limnPrimitive, limnPrimitiveTriangleStrip),
              primIdx, airEnumStr(limnPrimitive, pld->type[primIdx]));
      biffAdd(LIMN, err); return 1;
    }
  }
  if (haveStrips && 1 != pld->primNum) {
    sprintf(err, "%s: sorry, can only have a single triangle strip", me);
    biffAdd(LIMN, err); return 1;
  }

  fprintf(file, "#Inventor V2.0 ascii\n");
  fprintf(file, "# written by Teem/limn\n\n");
  fprintf(file, "Separator {\n");
  fprintf(file, "  Coordinate3 {\n");
  fprintf(file, "    point [\n");
  if (haveStrips) {
    unsigned int vii;
    for (vii=0; vii<pld->icnt[0]; vii++) {
      xyzwIdx = (pld->indx)[vii];
      ELL_34V_HOMOG(xyz, pld->xyzw + 4*xyzwIdx);
      fprintf(file, "      %g %g %g%s\n",
              xyz[0], xyz[1], xyz[2],
              vii < pld->icnt[0]-1 ? "," : "");
    }
  } else {
    for (xyzwIdx=0; xyzwIdx<pld->xyzwNum; xyzwIdx++) {
      ELL_34V_HOMOG(xyz, pld->xyzw + 4*xyzwIdx);
      fprintf(file, "      %g %g %g%s\n",
              xyz[0], xyz[1], xyz[2],
              xyzwIdx < pld->xyzwNum-1 ? "," : "");
    }
  }
  fprintf(file, "    ]\n");
  fprintf(file, "  }\n");

  bitFlag = limnPolyDataInfoBitFlag(pld);
  if (bitFlag & (1 << limnPolyDataInfoNorm)) {
    fprintf(file, "  NormalBinding {  value PER_VERTEX_INDEXED }\n");
    fprintf(file, "  Normal {\n");
    fprintf(file, "    vector [\n");
    if (haveStrips) {
      unsigned int vii;
      for (vii=0; vii<pld->icnt[0]; vii++) {
        normIdx = (pld->indx)[vii];
        ELL_3V_SET(norm, 
                   pld->norm[0 + 3*normIdx],
                   pld->norm[1 + 3*normIdx],
                   pld->norm[2 + 3*normIdx]);
        ELL_3V_NORM(norm, norm, len);
        fprintf(file, "      %g %g %g%s\n", norm[0], norm[1], norm[2],
                vii < pld->icnt[0]-1 ? "," : "");
      }
    } else {
      for (normIdx=0; normIdx<pld->normNum; normIdx++) {
        fprintf(file, "      %g %g %g%s\n",
                pld->norm[0 + 3*normIdx],
                pld->norm[1 + 3*normIdx],
                pld->norm[2 + 3*normIdx],
                normIdx < pld->normNum-1 ? "," : "");
      }
    }
    fprintf(file, "    ]\n");
    fprintf(file, "  }\n");
  }
  if (!haveStrips) {
    if (bitFlag & (1 << limnPolyDataInfoRGBA)) {
      fprintf(file, "  MaterialBinding {  value PER_VERTEX_INDEXED }\n");
      fprintf(file, "  Material {\n");
      fprintf(file, "    diffuseColor [\n");
      for (rgbaIdx=0; rgbaIdx<pld->rgbaNum; rgbaIdx++) {
        fprintf(file, "      %g %g %g%s\n",
                pld->rgba[0 + 4*rgbaIdx]/255.0,
                pld->rgba[1 + 4*rgbaIdx]/255.0,
                pld->rgba[2 + 4*rgbaIdx]/255.0,
                rgbaIdx < pld->rgbaNum-1 ? "," : "");
      }
      fprintf(file, "    ]\n");
      fprintf(file, "  }\n");
    }
  }

  if (haveStrips) {
    fprintf(file, "  TriangleStripSet {\n");
    fprintf(file, "    numVertices %u\n", pld->icnt[0]);
    fprintf(file, "  }\n");
  } else {
    fprintf(file, "  IndexedFaceSet {\n");
    fprintf(file, "    coordIndex [\n");
    
    baseVertIdx = 0;
    for (primIdx=0; primIdx<pld->primNum; primIdx++) {
      unsigned int triIdx, triNum, *indx3;
      triNum = pld->icnt[primIdx]/3;
      for (triIdx=0; triIdx<triNum; triIdx++) {
        indx3 = pld->indx + baseVertIdx + 3*triIdx;
        fprintf(file, "      %u, %u, %u, -1%s\n",
                indx3[0], indx3[1], indx3[2],
                triIdx < triNum-1 ? "," : "");
      }
      baseVertIdx += 3*triNum;
    }
    fprintf(file, "    ]\n");
    fprintf(file, "  }\n");
  }

  fprintf(file, "}\n");

  return 0;
}

typedef union {
  int **i;
  void **v;
} _ippu;

int
limnObjectOFFRead(limnObject *obj, FILE *file) {
  char me[]="limnObjectOFFRead", err[BIFF_STRLEN];
  double vert[6];
  char line[AIR_STRLEN_LARGE];  /* HEY: bad Gordon */
  int lineCount, lookIdx, partIdx, idxTmp, faceNum, faceGot, got;
  unsigned int vertGot,vertNum;
  unsigned int ibuff[1024]; /* HEY: bad Gordon */
  float fbuff[1024];  /* HEY: bad bad Gordon */
  float lastRGB[3]={-1,-1,-1}; int lastLook;
  unsigned int lret;
  
  int *vertBase;
  airArray *vertBaseArr, *mop;
  _ippu u;

  if (!( obj && file )) {
    sprintf(err, "%s: got NULL pointer", me);
    biffAdd(LIMN, err); return 1;
  }
  vertBase = NULL;
  u.i = &vertBase;
  vertBaseArr = airArrayNew(u.v, NULL, sizeof(int), 128);
  mop = airMopNew();
  airMopAdd(mop, vertBaseArr, (airMopper)airArrayNuke, airMopAlways);
  got = 0;
  lineCount = 0;
  do {
    if (!airOneLine(file, line, AIR_STRLEN_LARGE)) {
      sprintf(err, "%s: hit EOF before getting #vert #face #edge line", me);
      biffAdd(LIMN, err); airMopError(mop); return 1;
    }
    lineCount++;
    got = airParseStrUI(ibuff, line, AIR_WHITESPACE, 3);
  } while (3 != got);
  vertNum = ibuff[0];
  faceNum = ibuff[1];
  
  /* read all vertex information */
  lastLook = -1;
  partIdx = limnObjectPartAdd(obj);
  vertGot = 0;
  airArrayLenIncr(vertBaseArr, 1);
  vertBase[partIdx] = vertGot;
  while (vertGot < vertNum) {
    do {
      lret = airOneLine(file, line, AIR_STRLEN_LARGE);
      lineCount++;
    } while (1 == lret);
    if (!lret) {
      sprintf(err, "%s: (near line %d) hit EOF trying to read vert %d (of %d)",
              me, lineCount, vertGot, vertNum);
      biffAdd(LIMN, err); airMopError(mop); return 1;
    }
    if (1 == sscanf(line, "### LIMN BEGIN PART %d", &idxTmp)) {
      if (idxTmp != 0) {
        partIdx = limnObjectPartAdd(obj);
        if (idxTmp != partIdx) {
          sprintf(err, "%s: got signal to start part %d, not %d", 
                  me, idxTmp, partIdx);
          biffAdd(LIMN, err); airMopError(mop); return 1;
        }
        airArrayLenIncr(vertBaseArr, 1);
        vertBase[partIdx] = vertGot;
      }
      continue;
    }
    if (3 != airParseStrD(vert, line, AIR_WHITESPACE, 3)) {
      sprintf(err, "%s: couldn't parse 3 doubles from \"%s\" "
              "for vert %d (of %d)",
              me, line, vertGot, vertNum);
      biffAdd(LIMN, err); airMopError(mop); return 1;
    }
    if (6 == airParseStrD(vert, line, AIR_WHITESPACE, 6)) {
      /* we could also parse an RGB color */
      if (-1 == lastLook || !ELL_3V_EQUAL(lastRGB, vert+3)) {
        lookIdx = limnObjectLookAdd(obj);
        ELL_4V_SET(obj->look[lookIdx].rgba,
                   AIR_CAST(float, vert[3]),
                   AIR_CAST(float, vert[4]),
                   AIR_CAST(float, vert[5]),
                   1);
        lastLook = lookIdx;
        ELL_3V_COPY_TT(lastRGB, float, vert+3);
      } else {
        lookIdx = lastLook;
      }
    } else {
      lookIdx = 0;
    }
    /*
    fprintf(stderr, "line %d: vertGot = %d; lookIdx = %d; partIdx = %d\n",
            lineCount, vertGot, lookIdx, partIdx);
    */
    limnObjectVertexAdd(obj, partIdx,
                        AIR_CAST(float, vert[0]),
                        AIR_CAST(float, vert[1]),
                        AIR_CAST(float, vert[2]));
    vertGot++;
  }
  /* read face information */
  partIdx = 0;
  faceGot = 0;
  while (faceGot < faceNum) {
    do {
      lret = airOneLine(file, line, AIR_STRLEN_LARGE);
      lineCount++;
    } while (1 == lret);
    if (!lret) {
      sprintf(err, "%s: (near line %d) hit EOF trying to read face %d (of %d)",
              me, lineCount, faceGot, faceNum);
      biffAdd(LIMN, err); airMopError(mop); return 1;
    }
    if (1 == sscanf(line, "### LIMN BEGIN PART %d", &idxTmp)) {
      if (idxTmp != 0) {
        partIdx += 1;
        if (idxTmp != partIdx) {
          sprintf(err, "%s: (near line %d) got signal to start "
                  "part %d, not %d", 
                  me, lineCount, idxTmp, partIdx);
          biffAdd(LIMN, err); airMopError(mop); return 1;
        }
      }
      continue;
    }
    if (1 != sscanf(line, "%u", &vertNum)) {
      sprintf(err, "%s: (near line %d) can't get first int "
              "(#verts) from \"%s\" for face %d (of %d)",
              me, lineCount, line, faceGot, faceNum);
      biffAdd(LIMN, err); airMopError(mop); return 1;
    }
    if (vertNum+1 != airParseStrUI(ibuff, line, AIR_WHITESPACE, vertNum+1)) {
      sprintf(err, "%s: (near line %d) couldn't parse %d ints from \"%s\" "
              "for face %d (of %d)",
              me, lineCount, vertNum+1, line, faceGot, faceNum);
      biffAdd(LIMN, err); airMopError(mop); return 1;
    }
    if (vertNum+1+3 == airParseStrF(fbuff, line,
                                    AIR_WHITESPACE, vertNum+1+3)) {
      /* could also parse color */
      if (-1 == lastLook || !ELL_3V_EQUAL(lastRGB, fbuff+vertNum+1)) {
        lookIdx = limnObjectLookAdd(obj);
        ELL_4V_SET(obj->look[lookIdx].rgba, fbuff[vertNum+1+0],
                   fbuff[vertNum+1+1], fbuff[vertNum+1+2], 1);
        lastLook = lookIdx;
        ELL_3V_COPY(lastRGB, fbuff+vertNum+1);
      } else {
        lookIdx = lastLook;
      }
    } else {
      lookIdx = 0;
    }
    /*
    fprintf(stderr, "line %d: faceGot = %d; lookIdx = %d; partIdx = %d\n",
            lineCount, faceGot, lookIdx, partIdx);
    */
    limnObjectFaceAdd(obj, partIdx, lookIdx, vertNum, ibuff+1);
    faceGot++;
  }

  airMopOkay(mop); 
  return 0;
}

/*
http://www.npr.org/templates/story/story.php?storyId=4531695
*/

#define LMPD_MAGIC "LIMN0001"
#define DEMARK_STR "====== "
#define DEMARK_CHAR '='
#define NUM_STR "num:"
#define INFO_STR "info:"
#define TYPE_STR "type:"
#define ICNT_STR "icnt:"
#define INDX_STR "indx:"
#define XYZW_STR "xyzw:"

int
limnPolyDataLMPDWrite(FILE *file, const limnPolyData *pld) {
  char me[]="limnPolyDataLMPDWrite", err[BIFF_STRLEN], infoS[AIR_STRLEN_MED];
  unsigned int primIdx, infoNum, flag, bit;
  Nrrd *nrrd;
  airArray *mop;
  
  if (!(file && pld)) {
    sprintf(err, "%s: got NULL pointer", me);
    biffAdd(LIMN, err); return 1;
  }

  mop = airMopNew();
  
  fprintf(file, "%s\n", LMPD_MAGIC);
  fprintf(file, "%s%s %u %u %u\n", DEMARK_STR, NUM_STR,
          pld->xyzwNum, pld->indxNum, pld->primNum);

  flag = limnPolyDataInfoBitFlag(pld);
  infoNum = 0;
  bit = 0;
  strcpy(infoS, "");
  while (flag) {
    if (flag & 1) {
      infoNum += 1;
      strcat(infoS, airEnumStr(limnPolyDataInfo, bit));
      strcat(infoS, "\n");
    }
    flag /= 2;
    bit += 1;
  }
  fprintf(file, "%s%s %u\n%s", DEMARK_STR, INFO_STR, infoNum, infoS);

  fprintf(file, "%s%s\n", DEMARK_STR, TYPE_STR);
  for (primIdx=0; primIdx<pld->primNum; primIdx++) {
    fprintf(file, "%s\n", airEnumStr(limnPrimitive, pld->type[primIdx]));
  }
  fprintf(file, "%s%s\n", DEMARK_STR, ICNT_STR);
  for (primIdx=0; primIdx<pld->primNum; primIdx++) {
    fprintf(file, "%u\n", pld->icnt[primIdx]);
  }
  nrrd = nrrdNew();
  airMopAdd(mop, nrrd, (airMopper)nrrdNix, airMopAlways); /* nix, not nuke */

  fprintf(file, "%s%s\n", DEMARK_STR, INDX_STR);
  if (nrrdWrap_va(nrrd, pld->indx, nrrdTypeUInt, 1, pld->indxNum)
      || nrrdWrite(file, nrrd, NULL)) {
    sprintf(err, "%s: problem saving indx array", me);
    biffMove(LIMN, err, NRRD); airMopError(mop); return 1;
  }
  fprintf(file, "\n");

  fprintf(file, "%s%s\n", DEMARK_STR, XYZW_STR);
  if (nrrdWrap_va(nrrd, pld->xyzw, nrrdTypeFloat, 2, 4, pld->xyzwNum)
      || nrrdWrite(file, nrrd, NULL)) {
    sprintf(err, "%s: problem saving xyzw array", me);
    biffMove(LIMN, err, NRRD); airMopError(mop); return 1;
  }
  fprintf(file, "\n");

  if (infoNum) {
    flag = limnPolyDataInfoBitFlag(pld);
    bit = 0;
    while (flag) {
      if (flag & 1) {
        int E;
        fprintf(file, "%s%s %s\n", DEMARK_STR, INFO_STR,
                airEnumStr(limnPolyDataInfo, bit));
        switch (bit) {
        case limnPolyDataInfoRGBA:
          E = nrrdWrap_va(nrrd, pld->rgba, nrrdTypeUChar, 2, 4, pld->rgbaNum);
          break;
        case limnPolyDataInfoNorm:
          E = nrrdWrap_va(nrrd, pld->norm, nrrdTypeFloat, 2, 3, pld->normNum);
          break;
        case limnPolyDataInfoTex2D:
          E = nrrdWrap_va(nrrd, pld->tex2D, nrrdTypeFloat, 2, 2,pld->tex2DNum);
          break;
        default:
          sprintf(err, "%s: info %d (%s) not handled", me, bit,
                  airEnumStr(limnPolyDataInfo, bit));
          biffAdd(LIMN, err); airMopError(mop); return 1;
          break;
        }
        if (E || nrrdWrite(file, nrrd, NULL)) {
          sprintf(err, "%s: problem saving %s info",
                  me, airEnumStr(limnPolyDataInfo, bit));
          biffMove(LIMN, err, NRRD); airMopError(mop); return 1;
        }
        fprintf(file, "\n");
      }
      flag /= 2;
      bit += 1;
    }
  }

  airMopOkay(mop);
  return 0;
}

/*
******** limnPolyDataLMPDRead
**
** reads a limnPolyData from an LMPD file
**
** HEY: this was written in a hurry, is pretty hacky, and so it 
** needs some serious clean-up
*/
int
limnPolyDataLMPDRead(limnPolyData *pld, FILE *file) {
  char me[]="limnPolyDataLMPDRead", err[BIFF_STRLEN],
    line[AIR_STRLEN_MED], name[AIR_STRLEN_MED], *tmp;
  unsigned int vertNum, indxNum, primNum, primIdx, lineLen,
    infoNum, infoIdx, info, flag;
  Nrrd *nrrd;
  airArray *mop;
  int hackhack, tmpChar;

  if (!(pld && file)) {
    sprintf(err, "%s: got NULL pointer", me);
    biffAdd(LIMN, err); return 1;
  }
  
  sprintf(name, "magic");
  lineLen = airOneLine(file, line, AIR_STRLEN_MED);
  if (!lineLen) {
    sprintf(err, "%s: didn't get %s line", me, name);
    biffAdd(LIMN, err); return 1;
  }
  if (strcmp(line, LMPD_MAGIC)) {
    sprintf(err, "%s: %s line \"%s\" not expected \"%s\"", 
            me, name, line, LMPD_MAGIC);
    biffAdd(LIMN, err); return 1;
  }

  sprintf(name, "nums");
  lineLen = airOneLine(file, line, AIR_STRLEN_MED);
  if (!lineLen) {
    sprintf(err, "%s: didn't get %s line", me, name);
    biffAdd(LIMN, err); return 1;
  }
  if (strncmp(line, DEMARK_STR NUM_STR, strlen(DEMARK_STR NUM_STR))) {
    sprintf(err, "%s: %s line \"%s\" didn't start w/ expected \"%s\"", 
            me, name, line, NUM_STR);
    biffAdd(LIMN, err); return 1;
  }
  tmp = line + strlen(DEMARK_STR NUM_STR);
  if (3 != sscanf(tmp, " %u %u %u", &vertNum, &indxNum, &primNum)) {
    sprintf(err, "%s: couldn't parse \"%s\" as 3 uints on %s line",
            me, tmp, name);
    biffAdd(LIMN, err); return 1;
  }

  sprintf(name, "info");
  lineLen = airOneLine(file, line, AIR_STRLEN_MED);
  if (!lineLen) {
    sprintf(err, "%s: didn't get %s line", me, name);
    biffAdd(LIMN, err); return 1;
  }
  if (strncmp(line, DEMARK_STR INFO_STR, strlen(DEMARK_STR INFO_STR))) {
    sprintf(err, "%s: %s line \"%s\" didn't start w/ expected \"%s\"", 
            me, name, line, DEMARK_STR INFO_STR);
    biffAdd(LIMN, err); return 1;
  }
  tmp = line + strlen(DEMARK_STR INFO_STR);
  if (1 != sscanf(tmp, " %u", &infoNum)) {
    sprintf(err, "%s: couldn't parse \"%s\" as 1 uints on %s line",
            me, tmp, name);
    biffAdd(LIMN, err); return 1;
  }
  flag = 0;
  for (infoIdx=0; infoIdx<infoNum; infoIdx++) {
    lineLen = airOneLine(file, line, AIR_STRLEN_MED);
    if (!lineLen) {
      sprintf(err, "%s: didn't get %s line %u/%u", me, name, infoIdx, infoNum);
      biffAdd(LIMN, err); return 1;
    }
    info = airEnumVal(limnPolyDataInfo, line);
    if (!info) {
      sprintf(err, "%s: couldn't parse \"%s\" %s line %u/%u", 
              me, line, name, infoIdx, infoNum);
      biffAdd(LIMN, err); return 1;
    }
    flag |= (1 << info);
  }

  /* finally, allocate the polydata */
  if (limnPolyDataAlloc(pld, flag, vertNum, indxNum, primNum)) {
    sprintf(err, "%s: couldn't allocate polydata", me);
    biffAdd(LIMN, err); return 1;
  }
  /* actually, caller owns pld, so we don't register it with mop */

  sprintf(name, "type");
  lineLen = airOneLine(file, line, AIR_STRLEN_MED);
  if (!lineLen) {
    sprintf(err, "%s: didn't get %s line", me, name);
    biffAdd(LIMN, err); return 1;
  }
  if (strcmp(line, DEMARK_STR TYPE_STR)) {
    sprintf(err, "%s: %s line \"%s\" not expected \"%s\"", 
            me, name, line, DEMARK_STR TYPE_STR);
    biffAdd(LIMN, err); return 1;
  }
  for (primIdx=0; primIdx<primNum; primIdx++) {
    lineLen = airOneLine(file, line, AIR_STRLEN_MED);
    if (!lineLen) {
      sprintf(err, "%s: didn't get %s line %u/%u", me, name, primIdx, primNum);
      biffAdd(LIMN, err); return 1;
    }
    pld->type[primIdx] = airEnumVal(limnPrimitive, line);
    if (!(pld->type[primIdx])) {
      sprintf(err, "%s: couldn't parse \"%s\" %s line %u/%u", 
              me, line, name, primIdx, primNum);
      biffAdd(LIMN, err); return 1;
    }
  }

  sprintf(name, "icnt");
  lineLen = airOneLine(file, line, AIR_STRLEN_MED);
  if (!lineLen) {
    sprintf(err, "%s: didn't get %s line", me, name);
    biffAdd(LIMN, err); return 1;
  }
  if (strcmp(line, DEMARK_STR ICNT_STR)) {
    sprintf(err, "%s: %s line \"%s\" not expected \"%s\"", 
            me, name, line, DEMARK_STR ICNT_STR);
    biffAdd(LIMN, err); return 1;
  }
  for (primIdx=0; primIdx<primNum; primIdx++) {
    lineLen = airOneLine(file, line, AIR_STRLEN_MED);
    if (!lineLen) {
      sprintf(err, "%s: didn't get %s line %u/%u", me, name, primIdx, primNum);
      biffAdd(LIMN, err); return 1;
    }
    if (1 != sscanf(line, "%u", &(pld->icnt[primIdx]))) {
      sprintf(err, "%s: couldn't parse \"%s\" %s line %u/%u", 
              me, line, name, primIdx, primNum);
      biffAdd(LIMN, err); return 1;
    }
  }

  sprintf(name, "indx");
  lineLen = airOneLine(file, line, AIR_STRLEN_MED);
  if (!lineLen) {
    sprintf(err, "%s: didn't get %s line", me, name);
    biffAdd(LIMN, err); return 1;
  }
  if (strcmp(line, DEMARK_STR INDX_STR)) {
    sprintf(err, "%s: %s line \"%s\" not expected \"%s\"", 
            me, name, line, DEMARK_STR ICNT_STR);
    biffAdd(LIMN, err); return 1;
  }

  /* NOW its finally time to create the mop */
  mop = airMopNew();
  nrrd = nrrdNew();
  airMopAdd(mop, nrrd, (airMopper)nrrdNuke, airMopAlways);
  /* HEY HEY HEY HOLY CRAP!
  ** why the hell isn't the verbosity level a field in NrrdIoState ?!?!
  ** THIS NEES TO BE FIXED (in nrrd) ASAP!
  */
  hackhack = nrrdStateVerboseIO;
  nrrdStateVerboseIO = 0;
  if (nrrdRead(nrrd, file, NULL)) {
    sprintf(err, "%s: trouble reading %s data", me, name);
    biffMove(LIMN, err, NRRD); airMopError(mop); return 1;
  }
  if (!(nrrdTypeUInt == nrrd->type
        && 1 == nrrd->dim
        && indxNum == nrrd->axis[0].size)) {
    sprintf(err, "%s: didn't get 1-D %s-type %u-sample array "
            "(got %u-D %s-type %u-by-? array)", me, 
            airEnumStr(nrrdType, nrrdTypeUInt),
            AIR_CAST(unsigned int, indxNum),
            nrrd->dim,
            airEnumStr(nrrdType, nrrd->type),
            AIR_CAST(unsigned int, nrrd->axis[0].size));
    biffAdd(LIMN, err); airMopError(mop); return 1;
  }
  /* now copy the data */
  memcpy(pld->indx, nrrd->data, nrrdElementSize(nrrd)*nrrdElementNumber(nrrd));
  do {
    tmpChar = getc(file);
    if (EOF == tmpChar) {
      sprintf(err, "%s: hit EOF seeking to begin next line", me);
      biffAdd(LIMN, err); airMopError(mop); return 1;
    }
  } while (DEMARK_CHAR != tmpChar);
  ungetc(tmpChar, file);

  sprintf(name, "xyzw");
  lineLen = airOneLine(file, line, AIR_STRLEN_MED);
  if (!lineLen) {
    sprintf(err, "%s: didn't get %s line", me, name);
    biffAdd(LIMN, err); return 1;
  }
  if (strcmp(line, DEMARK_STR XYZW_STR)) {
    sprintf(err, "%s: %s line \"%s\" not expected \"%s\"", 
            me, name, line, DEMARK_STR XYZW_STR);
    biffAdd(LIMN, err); return 1;
  }
  if (nrrdRead(nrrd, file, NULL)) {
    sprintf(err, "%s: trouble reading %s data", me, name);
    biffMove(LIMN, err, NRRD); airMopError(mop); return 1;
  }
  if (!(nrrdTypeFloat == nrrd->type
        && 2 == nrrd->dim
        && 4  == nrrd->axis[0].size
        && vertNum  == nrrd->axis[1].size)) {
    sprintf(err, "%s: didn't get 2-D %s-type 4-by-%u array "
            "(got %u-D %s-type %u-by-%u array)", me, 
            airEnumStr(nrrdType, nrrdTypeFloat),
            AIR_CAST(unsigned int, vertNum),
            nrrd->dim,
            airEnumStr(nrrdType, nrrd->type),
            AIR_CAST(unsigned int, nrrd->axis[0].size),
            AIR_CAST(unsigned int, nrrd->axis[1].size));
    biffAdd(LIMN, err); airMopError(mop); return 1;
  }
  /* now copy the data */
  memcpy(pld->xyzw, nrrd->data, nrrdElementSize(nrrd)*nrrdElementNumber(nrrd));

  if (infoNum) {
    int wantType;
    unsigned int wantSize;
    void *data;
    for (infoIdx=0; infoIdx<infoNum; infoIdx++) {
      do {
        tmpChar = getc(file);
        if (EOF == tmpChar) {
          sprintf(err, "%s: hit EOF seeking to begin next line", me);
          biffAdd(LIMN, err); airMopError(mop); return 1;
        }
      } while (DEMARK_CHAR != tmpChar);
      ungetc(tmpChar, file);
      lineLen = airOneLine(file, line, AIR_STRLEN_MED);
      if (!lineLen) {
        sprintf(err, "%s: didn't get %s line %u/%u", me,
                INFO_STR, infoIdx, infoNum);
        biffAdd(LIMN, err); return 1;
      }
      if (strncmp(line, DEMARK_STR INFO_STR, strlen(DEMARK_STR INFO_STR))) {
        sprintf(err, "%s: %s line \"%s\" not expected \"%s\"", 
                me, INFO_STR, line, DEMARK_STR INFO_STR);
        biffAdd(LIMN, err); return 1;
      }
      tmp = line + strlen(DEMARK_STR INFO_STR) + strlen(" ");
      info = airEnumVal(limnPolyDataInfo, tmp);
      if (!info) {
        sprintf(err, "%s: couldn't parse \"%s\" as %s in %s line \"%s\"",
                me, tmp, limnPolyDataInfo->name, INFO_STR, line);
        biffAdd(LIMN, err); return 1;
      }
      if (nrrdRead(nrrd, file, NULL)) {
        sprintf(err, "%s: trouble reading %s %s data", me, INFO_STR, tmp);
        biffMove(LIMN, err, NRRD); airMopError(mop); return 1;
      }
      switch (info) {
      case limnPolyDataInfoRGBA:
        wantType = nrrdTypeUChar;
        wantSize = 4;
        data = pld->rgba;
        break;
      case limnPolyDataInfoNorm:
        wantType = nrrdTypeFloat;
        wantSize = 3;
        data = pld->norm;
        break;
      case limnPolyDataInfoTex2D:
        wantType = nrrdTypeFloat;
        wantSize = 2;
        data = pld->tex2D;
        break;
      default:
        sprintf(err, "%s: info %d (%s) not handled", me, info,
                airEnumStr(limnPolyDataInfo, info));
        biffAdd(LIMN, err); airMopError(mop); return 1;
        break;
      }
      if (!(wantType == nrrd->type
            && 2 == nrrd->dim
            && wantSize  == nrrd->axis[0].size
            && vertNum  == nrrd->axis[1].size)) {
        sprintf(err, "%s: didn't get 2-D %s-type %u-by-%u array "
                "(got %u-D %s-type %u-by-%u-by-? array)", me, 
                airEnumStr(nrrdType, wantType),
                wantSize, AIR_CAST(unsigned int, vertNum),
                nrrd->dim,
                airEnumStr(nrrdType, nrrd->type),
                AIR_CAST(unsigned int, nrrd->axis[0].size),
                AIR_CAST(unsigned int, nrrd->axis[1].size));
        biffAdd(LIMN, err); airMopError(mop); return 1;
      }
      /* now copy the data */
      memcpy(data, nrrd->data, nrrdElementSize(nrrd)*nrrdElementNumber(nrrd));
    }
  }

  airMopOkay(mop);
  nrrdStateVerboseIO = hackhack;
  return 0;
}

int
_limnHestPolyDataLMPDParse(void *ptr, char *str, char err[AIR_STRLEN_HUGE]) {
  char me[] = "_limnHestPolyDataLMPDParse", *nerr;
  limnPolyData **lpldP;
  airArray *mop;
  FILE *file;
  
  if (!(ptr && str)) {
    sprintf(err, "%s: got NULL pointer", me);
    return 1;
  }
  file = fopen(str, "rb");
  if (!file) {
    sprintf(err, "%s: couldn't open \"%s\" for reading", me, str);
    return 1;
  }
  mop = airMopNew();
  airMopAdd(mop, file, (airMopper)airFclose, airMopAlways);
  lpldP = (limnPolyData **)ptr;
  *lpldP = limnPolyDataNew();
  airMopAdd(mop, *lpldP, (airMopper)limnPolyDataNix, airMopOnError);
  if (limnPolyDataLMPDRead(*lpldP, file)) {
    airMopAdd(mop, nerr = biffGetDone(LIMN), airFree, airMopOnError);
    strncpy(err, nerr, AIR_STRLEN_HUGE-1);
    airMopError(mop);
    return 1;
  }
  airMopOkay(mop);
  return 0;
}

hestCB
_limnHestPolyDataLMPD = {
  sizeof(limnPolyData *),
  "polydata",
  _limnHestPolyDataLMPDParse,
  (airMopper)limnPolyDataNix
}; 

hestCB *
limnHestPolyDataLMPD = &_limnHestPolyDataLMPD;

