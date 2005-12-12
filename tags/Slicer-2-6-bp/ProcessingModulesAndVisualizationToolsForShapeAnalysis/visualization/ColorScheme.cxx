// ColorScheme.txx: implementation of the ColorScheme class.
//
//////////////////////////////////////////////////////////////////////

#include "ColorScheme.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ColorScheme::ColorScheme()
{

}

ColorScheme::~ColorScheme()
{

}

void ColorScheme::MakeSignificanceMap(vtkPolyDataMapper *mapper)
{
  vtkLookupTable *lut = vtkLookupTable::New () ;
  lut->SetHueRange (0, 0.25) ;
  lut->SetTableRange (0.0, 0.05) ;
  lut->SetRampToLinear () ;

  lut->Build() ;
  int n = lut->GetNumberOfColors() ;
  lut->SetTableValue (n-1, 0, 0, 1 ) ;

  //float color[3] ;
  //lut->GetColor ( 0.03, color ) ;
  //vtkGenericWarningMacro(<<color[0]) ;

  mapper->SetColorModeToMapScalars () ;
  
  mapper->SetLookupTable (lut) ;
  mapper->SetScalarRange(0, 0.05) ;
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

void ColorScheme::MakeDistanceMap(vtkPolyDataMapper *mapper, double min, double max)
{
  double limit ;
  limit = abs ( min ) ;
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
  float color[3] ;

  for ( i = 0 ; i < 256 ; i++ )
  {
    lutPositive->GetColor ( i/256.0, color ) ;
    lut->SetTableValue ( i/256.0, color ) ;
  }
  for ( i = 0 ; i < 256 ; i++ )
  {
    lutNegative->GetColor ( -i/256.0, color ) ;
    lut->SetTableValue ( -i/256.0, color ) ;
  }
  
  mapper->SetColorModeToMapScalars () ;
  mapper->SetLookupTable (lut) ;
  mapper->SetScalarRange(-limit, limit) ;
}
