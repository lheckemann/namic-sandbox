#include "WFKWTheme.h"

#include <vtkObjectFactory.h>
#include "vtkKWOptionDataBase.h"
#include "vtkKWApplication.h"

vtkStandardNewMacro( WFKWTheme );
vtkCxxRevisionMacro(WFKWTheme, "$Revision: 1.5 $");

WFKWTheme::WFKWTheme ( )
{
 this->WFColors = new WFKWColors;
}



//---------------------------------------------------------------------------
WFKWTheme::~WFKWTheme ( )
{
    if ( this->WFColors != NULL ) {
        this->WFColors->Delete ( );
        this->WFColors = NULL;
    }
}

void WFKWTheme::Install ( )
{
 vtkKWApplication *app = vtkKWApplication::SafeDownCast(this->GetApplication() );
    
    if ( app == NULL )
        {
            return;
        }
    this->vtkKWTheme::Install ( );

    vtkKWOptionDataBase *odb = app->GetOptionDataBase ( );
    
    // ---
    // Set default font:
    // ---
    odb->AddFontOptions ( "{Helvetica 8 normal}" );
    

    // ---
    // Background and foreground for all widgets in general (some will be overridden):
    // ---
    odb->AddBackgroundColorOptions( this->WFColors->GUIBgColor );
    odb->AddEntryAsDouble3 ( "vtkKWWidget", "SetDisabledForegroundColor",
                    this->WFColors->DisabledTextColor );
    odb->AddEntryAsDouble3 ( "vtkKWWidget", "SetActiveForegroundColor",
                             this->WFColors->FocusTextColor );
    odb->AddEntryAsDouble3 ( "vtkKWWidget", "SetActiveBackgroundColor",
                             this->WFColors->GUIBgColor );
    odb->AddEntryAsDouble3 ( "vtkKWWidget", "SetBackgroundColor",
                    this->WFColors->GUIBgColor );
    odb->AddEntryAsInt ("vtkKWWidget", "SetHighlightThickness", 0 );
    odb->AddEntryAsDouble3 ( "vtkKWWidget", "SetHighlightBackground",
                             this->WFColors->HighlightBackground);
    odb->AddEntryAsDouble3 ( "vtkKWWidget", "SetHighlightColor",
                             this->WFColors->HighlightColor);
    odb->AddEntryAsInt ("vtkKWWidget", "SetBorderWidth", 2 );
    odb->AddEntryAsInt ("vtkKWWidget", "SetActiveBorderWidth", 2 );
    odb->AddEntryAsDouble3 ( "vtkKWWidget", "SetTroughColor", this->WFColors->LightGrey );
    

    // ---
    // Individual widgets:
    // ---
    
    
    // Slicer Scales
    odb->AddEntryAsDouble3 ( "vtkKWScale", "SetTroughColor", this->WFColors->LightGrey );

    

    // Slicer Scrollbars 
    odb->AddEntryAsDouble3 ( "vtkKWScrollbar", "SetBackgroundColor",
                    this->WFColors->LightGrey );
    odb->AddEntryAsDouble3 ( "vtkKWScrollbar", "SetActiveBackgroundColor",
                    this->WFColors->LightGrey );
    odb->AddEntryAsDouble3 ( "vtkKWScrollbar", "SetTroughColor", this->WFColors->LightGrey );

    

    // Slicer Frames
    odb->AddEntryAsDouble3 ( "vtkKWFrame", "SetBackgroundColor",
                    this->WFColors->GUIBgColor );
    odb->AddEntryAsInt ( "vtkKWFrame", "SetBorderWidth", 0 );
    odb->AddEntryAsDouble3 ( "vtkKWFrame", "SetHighlightColor",
                             this->WFColors->GUIBgColor );
    odb->AddEntryAsDouble3 ( "vtkKWFrame", "SetHighlightColor",
                             this->WFColors->GUIBgColor );
    odb->AddEntryAsDouble3 ( "vtkKWFrame", "SetHighlightBackground",
                             this->WFColors->GUIBgColor );    
    odb->AddEntryAsInt ( "vtkKWFrame", "SetHighlightThickness", 0 );
    odb->AddEntry ( "vtkKWFrame", "SetRelief", "flat" );
    
    odb->AddEntryAsInt ( "vtkKWSeparator", "SetBorderWidth", 2 );
    

    
    // Slicer Frames inside Scrolled windows
    odb->AddEntryAsDouble3 ( "vtkKWFrameWithScrollbar:Frame", "SetBackgroundColor",
                    this->WFColors->GUIBgColor );
    odb->AddEntryAsInt ( "vtkKWFrameWithScrollbar:Frame", "SetBorderWidth", 0 );
    odb->AddEntryAsInt ( "vtkKWFrameWithScrollbar:Frame", "SetHighlightThickness", 0 );
    odb->AddEntry ( "vtkKWFrameWithScrollbar:Frame", "SetRelief", "sunken" );

    

    // Slicer Frames inside ApplicationGUI
    odb->AddEntryAsDouble3 ( "vtkSlicerApplicationGUI:DropShadowFrame", "SetBackgroundColor",
                    this->WFColors->LightestGreyBlue );

    
    // Module stacked, collapsing frames 
    odb->AddEntryAsDouble3 ( "vtkSlicerModuleCollapsibleFrame:LabelFrame", "SetBackgroundColor",
                             this->WFColors->MediumCoolStone );    
    odb->AddEntryAsInt ( "vtkSlicerModuleCollapsibleFrame:LabelFrame", "SetHighlightThickness", 1 );
    odb->AddEntry ( "vtkSlicerModuleCollapsibleFrame:LabelFrame", "SetReliefToGroove", NULL );
    odb->AddEntryAsDouble3 ( "vtkSlicerModuleCollapsibleFrame:Label", "SetBackgroundColor",
                             this->WFColors->MediumCoolStone );
    odb->AddEntryAsDouble3 ( "vtkSlicerModuleCollapsibleFrame:Icon", "SetBackgroundColor",
                             this->WFColors->MediumCoolStone );
    

    // Slicer Notebooks
    odb->AddEntryAsDouble3 ( "vtkKWNotebook", "SetBackgroundColor", this->WFColors->GUIBgColor );
    odb->AddEntryAsDouble3 ( "vtkKWNotebook", "SetPageTabColor", this->WFColors->MediumCoolStone );
    odb->AddEntryAsDouble3 ( "vtkKWNotebook", "SetSelectedPageTabColor", this->WFColors->LightCoolStone );
    odb->AddEntryAsInt ( "vtkKWNotebook", "SetSelectedPageTabPadding", 0 );


    
    // Slicer Pushbuttons
    odb->AddEntry ( "vtkKWPushButton", "SetReliefToGroove", NULL );
    odb->AddEntry ( "vtkKWPushButton", "SetDefault", "active" );
    odb->AddEntryAsInt ( "vtkKWPushButton", "SetHighlightThickness", 0 );
    odb->AddEntryAsDouble3 ( "vtkKWPushButton", "SetActiveForeground",
                             this->WFColors->FocusTextColor );

    
    // Slicer Checkbuttons
    odb->AddEntryAsDouble3 ("vtkKWCheckButton", "SetSelectColor",
                   this->WFColors->LightGrey );
    odb->AddEntryAsDouble3 ("vtkKWCheckButton", "SetActiveBackgroundColor",
                            this->WFColors->ActiveMenuBackgroundColor );
    odb->AddEntry( "vtkKWCheckButton", "IndicatorVisibilityOn", NULL);
    odb->AddEntryAsDouble3( "vtkKWCheckButton", "SetSelectColor", this->WFColors->HighlightColor);

    

    // Slicer Radiobuttons
    odb->AddEntryAsDouble3 ("vtkKWRadioButton", "SetSelectColor",
                   this->WFColors->LightGrey );
    odb->AddEntryAsDouble3 ("vtkKWRadioButton", "SetActiveBackgroundColor",
                            this->WFColors->ActiveMenuBackgroundColor );


    
    // Slicer WidgetWithLabel (scrap the groove around all)
    odb->AddEntry ( "vtkKWWidgetWithLabel", "SetRelief", "flat" );    


    
    // Slicer vtkKWSeparators
    odb->AddEntryAsDouble3 ( "vtkKWSeparator", "SetBackgroundColor",
                             this->WFColors->GUIBgColor );


    // Slicer MultiColumnLists
    // font
    odb->AddEntry ( "vtkKWMultiColumnList", "SetFont", "{Helvetica 8 normal}" );
    // column header 
    odb->AddEntryAsDouble3 ( "vtkKWMultiColumnList", "SetColumnLabelBackgroundColor",
                             this->WFColors->MediumBlue );
    odb->AddEntryAsDouble3 ( "vtkKWMultiColumnList", "SetColumnLabelForegroundColor",
                             this->WFColors->White );
    // selection
    odb->AddEntryAsDouble3 ( "vtkKWMultiColumnList", "SetSelectionBackgroundColor",
         this->WFColors->ActiveMenuBackgroundColor);
    odb->AddEntryAsDouble3 ( "vtkKWMultiColumnList", "SetSelectionForegroundColor",
         this->WFColors->ActiveTextColor);
    // MulticolumnList
    odb->AddEntryAsDouble3 ("vtkKWMultiColumnList", "SetStripeBackgroundColor",
                   this->WFColors->LightestGreyBlue );

    // Window's status fame
    odb->AddEntryAsDouble3 ( "vtkKWWindow:TrayFrame", "SetBackgroundColor",
                    this->WFColors->MediumCoolStone );    


    
    // Slicer MultiColumnListsWithScrollbars
    // scroll bars
    odb->AddEntryAsInt("vtkKWScrollbar", "SetWidth", 10);
    odb->AddEntryAsDouble3("vtkKWScrollbar", "SetTroughColor", this->WFColors->RecessedColor);
    

    
    // Slicer Menu and MenuButtons
    odb->AddEntryAsDouble3 ("vtkKWMenu", "SetActiveBackgroundColor",
                            this->WFColors->ActiveMenuBackgroundColor );
    odb->AddEntryAsDouble3 ("vtkKWMenu", "SetActiveForegroundColor",
                            this->WFColors->ActiveMenuForegroundColor );
    odb->AddEntry ( "vtkKWMenuButton", "SetReliefToGroove", NULL );

    // Wizard
    odb->AddEntryAsDouble3 ( "vtkKWWizardWidget", "SetTitleAreaBackgroundColor",
                    this->WFColors->LightestGreyBlue);    

    // anything special here?
    // Slicer Menubar
    // Slicer Entry
    // Slicer Listbox
    // Slicer Messages
    // ....?
}
