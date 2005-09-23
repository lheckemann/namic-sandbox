// ColorScheme.txx: implementation of the ColorScheme class.
//
//////////////////////////////////////////////////////////////////////

#include <math.h>
#include "ColorScheme.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ColorScheme::ColorScheme()
{
  pValueThreshold = 0.05 ;
  pValueHue0 = 0 ;
  pValueHue1 = 0.25 ;
  pValueHue2 = 0.67 ;
}

ColorScheme::~ColorScheme()
{

}

void ColorScheme::MakeSignificanceMap(vtkPolyDataMapper *mapper)
{
  vtkLookupTable *lut = vtkLookupTable::New () ;
  lut->SetHueRange (pValueHue0, pValueHue1) ;
  lut->SetTableRange (0.0, pValueThreshold) ;
  lut->SetRampToLinear () ;

  lut->Build() ;
  int n = lut->GetNumberOfColors() ;

  double r, g, b ;
  HSV2RGB ( pValueHue2*360, r, g, b ) ;
  lut->SetTableValue (n-1, r, g, b ) ;

  //float color[3] ;
  //lut->GetColor ( 0.03, color ) ;
  //vtkGenericWarningMacro(<<color[0]) ;

  mapper->SetColorModeToMapScalars () ;
  
  mapper->SetLookupTable (lut) ;
  mapper->SetScalarRange(0, pValueThreshold) ;
}

void ColorScheme::MakeSignificanceFunction(vtkPolyDataMapper *mapper)
{
  /* not used any more */
  vtkColorTransferFunction *lut = vtkColorTransferFunction::New () ;
  lut->AddHSVSegment (0.0, 0, 1, 1, 0.05, 0.25, 1, 1) ;
  lut->AddHSVSegment (0.05, 0.6, 1, 0.5, 1, 0.6, 1, 0.5) ;

  mapper->SetColorModeToMapScalars () ;
  mapper->SetLookupTable (lut) ;
}

void ColorScheme::MakeDistanceMap(vtkPolyDataMapper *mapper)
{ // ignore this completely! not working yet!!!
  /*double limit ;
  limit = fabs ( min ) ;
  if ( max > limit ) 
    limit = max ;

  vtkLookupTable *lutPositive = vtkLookupTable::New () ;
  lutPositive->SetHueRange (0, 0) ;
  lutPositive->SetSaturationRange (0, 1) ;
  lutPositive->SetValueRange ( 1, 1 ) ;
  lutPositive->SetTableRange (0, limit) ;
  lutPositive->SetRampToLinear () ;
  lutPositive->Build() ;
  
  vtkLookupTable *lutNegative = vtkLookupTable::New () ;
  lutNegative->SetHueRange (0.6, 0.6) ;
  lutNegative->SetSaturationRange (1, 0) ;
  lutNegative->SetTableRange (-limit, 0) ;
  lutNegative->SetRampToLinear () ;
  lutNegative->Build() ;

  vtkLookupTable *lut = vtkLookupTable::New () ;
  lut->Allocate ( 512 ) ;
  lut->SetTableRange ( -limit, limit ) ;
  
  int i ;
  double color[3] ;

  for ( i = 0 ; i < 256 ; i++ )
  {
    lutPositive->GetColor ( i/256.0, color ) ;
    lut->SetTableValue ( (vtkIdType) i/256.0, color ) ;
  }
  for ( i = 0 ; i < 256 ; i++ )
  {
    lutNegative->GetColor ( -i/256.0, color ) ;
    lut->SetTableValue ( (vtkIdType) -i/256.0, color ) ;
  }
  
  mapper->SetColorModeToMapScalars () ;
  mapper->SetLookupTable (lut) ;
  mapper->SetScalarRange(-limit, limit) ;
  */
}


void ColorScheme::SignificanceSettings ( float t, float h0, float h1, float h2 ) 
{
  pValueThreshold = t ;
  pValueHue0 = h0 ;
  pValueHue1 = h1 ;
  pValueHue2 = h2 ;
}

void ColorScheme::HSV2RGB ( float hue, double &r, double &g, double &b ) 
{
  // for this particular application, s is always 1 and v is always 0.5
  
  int i;
  double f, p, q, t;
  double  s = 1 ;
  double  v = 0.7 ;

  if( s == 0 ) {
    // achromatic (grey)
    r = g = b = v;
    return;
  }

  hue /= 60;      // sector 0 to 5
  i = floor( hue );
  f = hue - i;      // factorial part of h
  p = v * ( 1 - s );
  q = v * ( 1 - s * f );
  t = v * ( 1 - s * ( 1 - f ) );

  switch( i ) {
    case 0:
      r = v;
      g = t;
      b = p;
      break;
    case 1:
      r = q;
      g = v;
      b = p;
      break;
    case 2:
      r = p;
      g = v;
      b = t;
      break;
    case 3:
      r = p;
      g = q;
      b = v;
      break;
    case 4:
      r = t;
      g = p;
      b = v;
      break;
    default:    // case 5:
      r = v;
      g = p;
      b = q;
      break;
  }

  printf("%f %f %f\n", r, g, b);
}
