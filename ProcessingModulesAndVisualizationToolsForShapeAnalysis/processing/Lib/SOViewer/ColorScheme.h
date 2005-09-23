// ColorScheme.h: interface for the ColorScheme class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COLORSCHEME_H__26714073_8062_452A_9507_DCA418650B1E__INCLUDED_)
#define AFX_COLORSCHEME_H__26714073_8062_452A_9507_DCA418650B1E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vtkPolyDataMapper.h>
#include <vtkLookupTable.h>
#include <vtkColorTransferFunction.h>
#include <assert.h>

class ColorScheme  
{
public:
  void MakeDistanceMap (vtkPolyDataMapper *mapper);
  void MakeSignificanceFunction(vtkPolyDataMapper *mapper);
  void MakeSignificanceMap(vtkPolyDataMapper *mapper);
  void SignificanceSettings ( float t, float h0, float h1, float h2 ) ;
  ColorScheme();
  virtual ~ColorScheme();
  
private:
  float pValueThreshold, pValueHue0, pValueHue1, pValueHue2 ;
  void HSV2RGB ( float hue, double &r, double &g, double &b ) ;

};

#endif // !defined(AFX_COLORSCHEME_H__26714073_8062_452A_9507_DCA418650B1E__INCLUDED_)
