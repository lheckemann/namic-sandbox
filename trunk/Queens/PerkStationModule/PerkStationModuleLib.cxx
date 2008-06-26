#include "PerkStationModuleLib.h"

#include "vtkPerkStationModuleLogic.h"
#include "vtkPerkStationModuleGUI.h"

static char PerkStationModuleDescription[] =
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
"<loadable>\n"
"  <name>Curvature Anisotropic Module</name>\n"
"  <shortname>PerkStationModule</shortname>\n"
"  <guiname>PerkStationModule</guiname>\n"
"  <tclinitname>Perkstationmodule_Init</tclinitname>\n"
"  <message>Initializing Curvature Anisotropic Module...</message>\n"
"</loadable>";

char*
GetLoadableModuleDescription()
{
  return PerkStationModuleDescription;
}

void*
GetLoadableModuleGUI()
{
  return vtkPerkStationModuleGUI::New ( );
}


void*
GetLoadableModuleLogic()
{
  return vtkPerkStationModuleLogic::New ( );
}
