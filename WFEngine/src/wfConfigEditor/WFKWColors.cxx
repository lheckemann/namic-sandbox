#include "WFKWColors.h"
#include "vtkObjectFactory.h"

vtkStandardNewMacro( WFKWColors );
vtkCxxRevisionMacro(WFKWColors, "$Revision: 1.5 $");

WFKWColors::WFKWColors ( ) {
    
    //--- Set Slicer color palette
    this->DefineDefaultColorPalette ( );

    //--- Set Slicer colors using palette
    this->DefineWFColorNames ( );
}



//---------------------------------------------------------------------------
WFKWColors::~WFKWColors ( ) {
}

//---------------------------------------------------------------------------
void WFKWColors::DefineDefaultColorPalette ( ) {
    this->SetColor(this->White, 1.0, 1.0, 1.0);
    this->SetColor(this->Black,  0.0, 0.0, 0.0 );
    this->SetColor(this->DarkGrey, 0.25, 0.25, 0.25 );
    this->SetColor(this->MediumGrey,  0.65, 0.65, 0.65 );
    this->SetColor(this->LightGrey,  0.829, 0.829, 0.829 );
    this->SetColor(this->LightestGrey, 0.9, 0.9, 0.9 );
    this->SetColor(this->LightestGreyBlue, 0.9, 0.9, 1.0);
    this->SetColor(this->BurntOrange, 0.7, 0.4, 0.0 );

    this->SetColor(this->DarkStone, .811764, 0.8, 0.65413 );
//    this->SetColor(this->MediumCoolStone, 0.88627, 0.87843, 0.78039 );
//    this->SetColor(this->LightCoolStone, 0.98627, 0.97843, 0.88039 );
    this->SetColor(this->MediumCoolStone, 0.87627, 0.87039,  0.88843);
    this->SetColor(this->LightCoolStone, 0.94627, 0.94039, 0.97843);

    this->SetColor(this->LightStone,0.95941,0.945098, 0.823529 );
    this->SetColor ( this->LightestStone, 0.996078, 0.988235,0.913725);
    
    // warm, soft brown grey
    this->SetColor(this->Cornsilk2,  .93, .9098, .804 );
    this->SetColor(this->Cornsilk3,  .804, .784, .694 );
    this->SetColor(this->Cornsilk4,  .545, .5333, .47 );

    // warm, soft brown-pink grey
    this->SetColor(this->Seashell2,  .5411, .898, .87 );
    this->SetColor(this->Seashell3,  .803, 7725, .749 );
    this->SetColor(this->Seashell4,  .545, .525, .5098 );
    
    // Possibilities for SliceWindows
    this->SetColor(this->MediumGreen, .5137, .6509, .498);
    this->SetColor(this->DarkGreen, .149, .78, .149 );
    
    // Possibilities for SliceWindows
    this->SetColor(this->MediumRed,  .7568, .4, .3529) ;
    this->SetColor (this->BrightRed, .8, 0.0, 0.0 );
    this->SetColor(this->DarkRed,  .6, .25, 0.0 );
    
    // Possibilities for SliceWindows
    this->SetColor(this->MediumYellow,  .9333, .8392, .5019 );
    this->SetColor(this->DarkYellow,  .8196, .5804, .047);

    // bright yellow (warning message code)
    this->SetColor(this->BrightYellow, .902, .816, .242);

    // 3D viewer background
    this->SetColor(this->MediumBlue,  .4588, .5647, .6823 );
    this->SetColor(this->ViewerBlue, .70196, .70196, .90588 );  
    this->SetColor(this->LightBlue, .82, .82, 1.0);

    // RedSliceControllerGUI
    this->SetColor(this->SliceGUIRed,  .9568, .2929, .2029);
    // YellowSliceControllerGUI    
    this->SetColor(this->SliceGUIYellow,  .9333, .8392, .3019 );
    // GreenSliceControllerGUI
    this->SetColor(this->SliceGUIGreen,  .4337, .6909, .298);
}


//---------------------------------------------------------------------------
void WFKWColors::DefineWFColorNames ( ) {

    this->SetColor (this->SagittalColor, this->MediumYellow );
    this->SetColor (this->AxialColor, this->MediumRed );
    this->SetColor (this->CoronalColor, this->MediumGreen );

    //this->SetColor (this->ActiveTextColor, this->DarkGrey );
    this->SetColor (this->ActiveTextColor, this->Black );
    this->SetColor (this->DisabledTextColor, this->MediumGrey );
    this->SetColor (this->FocusTextColor, this->Black );
    this->SetColor (this->ActiveMenuBackgroundColor, this->LightGrey);
    this->SetColor (this->ActiveMenuForegroundColor, this->Black);

    this->SetColor (this->HighlightBackground, this->Black );
    this->SetColor (this->HighlightColor, this->MediumBlue );

    this->SetColor (this->NotificationMessageColor, this->MediumBlue );
    this->SetColor (this->WarningMessageColor, this->BrightYellow );
    this->SetColor (this->ErrorMessageColor, this->BrightRed );

    this->SetColor (this->GUIBgColor, this->White );
    //    this->SetColor(this->GUIBgColor, 0.8039, 0.7529, 0.69019);
    this->SetColor (this->ViewerBgColor, this->ViewerBlue );
    this->SetColor (this->SliceBgColor, this->Black );
    this->SetColor (this->RecessedColor, this->LightGrey );

}



//---------------------------------------------------------------------------
int WFKWColors::SetColor (double *color1, double *color2 ) {

    int ret = this->SetColor ( color1, color2[0], color2[1], color2[2] );
    return ret;
}



//---------------------------------------------------------------------------
int WFKWColors::SetColor(double * color,
                               double r, double g, double b) {
    
    if ( (r==color[0]) && (g==color[1]) && (b==color[2]) ||
         (r<0.0 || r>1.0) || (g<0.0 || g>1.0) || (b<0.0 || b>1.0)) {
        return 0;
    }
    color [0] = r;
    color [1] = g;
    color [2] = b;

    return 1;
}

//---------------------------------------------------------------------------
char *WFKWColors::GetColorAsHex(double *color)
{
    sprintf(this->HexColor, "%X%X%X",
            color[0]*16.0, color[1]*16.0, color[2]*16.0);
    return this->HexColor;
}
