
#include "objectentry.h"

int main (int argc, char * argv[])
{
  int error_count = 0 ;
  itk::AnalyzeObjectEntry::Pointer testAOE01=itk::AnalyzeObjectEntry::New();
  testAOE01->SetDisplayFlag(true);
  testAOE01->SetOpacity(0.123);

  itk::AnalyzeObjectEntry::Index Center;

  itk::AnalyzeObjectEntry::Pointer testAOE02=itk::AnalyzeObjectEntry::New();
  testAOE02->Copy(testAOE01); //Stress the operator=
  if( testAOE02->GetOpacity() != static_cast<float>(0.123F) )
    {
    error_count++;
    }
  //testAOE02->SetBlendFactor(5);
  //if( testAOE02->GetBlendFactor() != 5)
  //{
  //    error_count++;
  //}
  //if(testAOE02->GetDisplayFlag() != 1)
  //{
  //    error_count++;
  //}
  //if(testAOE01->GetMaximumXValue() != 10)
  //{
  //    error_count++;
  //}
  //testAOE01->SetStartBlue(5);
  //if(testAOE01->GetStartBlue() != 5)
  //{
  //    error_count++;
  //}
  //Center[0] = 3;
  //Center[1] = 5;
  //Center[2] = 10;
  //testAOE01->SetCenter(Center);

  //if(testAOE01->GetCenter()[0]!=3)
  //{
  //    error_count++;
  //}
  //if(testAOE01->GetCenter()[1] != 5)
  //{
  //    error_count++;
  //}
  //if(testAOE01->GetCenter()[2] != 10)
  //{
  //    error_count++;
  //}
  //testAOE01->SetStatusFlag(false);
  //
  //if(testAOE01->GetStatusFlag() != false)
  //{
  //    error_count++;
  //}
  //testAOE02->Copy(testAOE01);
  //if(testAOE02->GetStatusFlag() != false)
  //{
  //    error_count++;
  //}

  //testAOE02->SetBlendFactor(1);
  //if(testAOE02->GetBlendFactor() != 1)
  //{
  //    error_count++;
  //}
  //Center[0]=50;
  //Center[1]=51;
  //Center[2]=52;
  //testAOE02->SetCenter(Center);
  //if(testAOE02->GetCenter()[0]!=50)
  //{
  //    error_count++;
  //}
  //if(testAOE02->GetCenter()[1]!=51)
  //{
  //    error_count++;
  //}
  //if(testAOE02->GetCenter()[2] != 52)
  //{
  //    error_count++;
  //}
  //testAOE02->SetCopyFlag('n');
  //if(testAOE02->GetCopyFlag()!='n')
  //{
  //    error_count++;
  //}
  //testAOE02->SetDisplayFlag(2);
  //testAOE02->SetEndBlue(3);
  //itk::AnalyzeObjectEntry::intRGBPixel EndColor;
  //EndColor.SetBlue(32);
  //EndColor.SetRed(33);
  //EndColor.SetGreen(34);
  //testAOE02->SetEndColor(EndColor);
  //testAOE02->SetEndGreen(4);
  //testAOE02->SetEndRed(4);
  //itk::AnalyzeObjectEntry::Index MaximumCoordinateValue;
  //MaximumCoordinateValue[0] = 60;
  //MaximumCoordinateValue[1] = 61;
  //MaximumCoordinateValue[2] = 62;
  //testAOE02->SetMaximumCoordinateValue(MaximumCoordinateValue);
  //testAOE02->SetMaximumXValue(5);
  //testAOE02->SetMaximumYValue(6);
  //testAOE02->SetMaximumZValue(7);
  //itk::AnalyzeObjectEntry::Index MinimumCoordinateValue;
  //MinimumCoordinateValue[0] = 70;
  //MinimumCoordinateValue[1] = 71;
  //MinimumCoordinateValue[2] = 72;
  //testAOE02->SetMinimumCoordinateValue(MinimumCoordinateValue);
  //testAOE02->SetMinimumXValue(8);
  //testAOE02->SetMinimumYValue(9);
  //testAOE02->SetMinimumZValue(10);
  //testAOE02->SetMirrorFlag('t');
  //testAOE02->SetName("Test I win");
  //testAOE02->SetNeighborsUsedFlag('m');
  //testAOE02->SetOpacity(11.532);
  //testAOE02->SetOpacityThickness(12);
  //itk::AnalyzeObjectEntry::Index Rotation;
  //Rotation[0] = 80;
  //Rotation[1] =81;
  //Rotation[2] = 82;
  //testAOE02->SetRotation(Rotation);
  //itk::AnalyzeObjectEntry::Index RotationIncrement;
  //RotationIncrement[0]=90;
  //RotationIncrement[1] = 91;
  //RotationIncrement[2] = 92;
  //testAOE02->SetRotationIncrement(RotationIncrement);
  //testAOE02->SetShades(13);
  //testAOE02->SetStartBlue(14);
  //itk::AnalyzeObjectEntry::intRGBPixel StartColor;
  //StartColor.SetRed(3);
  //StartColor.SetGreen(5);
  //StartColor.SetBlue(7);
  //testAOE02->SetStartColor(StartColor);
  //testAOE02->SetStartGreen(15);
  //testAOE02->SetStartRed(16);
  //testAOE02->SetStatusFlag('p');
  //itk::AnalyzeObjectEntry::Index Translation;
  //Translation[0]=100;
  //Translation[1] = 101;
  //Translation[2] = 102;
  //testAOE02->SetTranslation(Translation);
  //itk::AnalyzeObjectEntry::Index TranslationIncrement;
  //TranslationIncrement[0]=110;
  //TranslationIncrement[1] = 111;
  //TranslationIncrement[2] = 112;
  //testAOE02->SetTranslationIncrement(TranslationIncrement);
  //testAOE02->SetXCenter(17);
  //testAOE02->SetYCenter(18);
  //testAOE02->SetZCenter(19);

  if( error_count )
    {
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}

