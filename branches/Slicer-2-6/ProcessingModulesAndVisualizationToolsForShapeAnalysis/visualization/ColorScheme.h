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
  static void MakeDistanceMap (vtkPolyDataMapper *mapper, double min, double max);
  static void MakeSignificanceFunction(vtkPolyDataMapper *mapper);
  static void MakeSignificanceMap(vtkPolyDataMapper *mapper);
  ColorScheme();
  virtual ~ColorScheme();

};

#endif // !defined(AFX_COLORSCHEME_H__26714073_8062_452A_9507_DCA418650B1E__INCLUDED_)
