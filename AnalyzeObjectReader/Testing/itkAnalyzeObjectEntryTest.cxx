
#include "objectentry.h"

int main (int argc, char * argv[])
{
  int error_count = 0 ;
  itk::AnalyzeObjectEntry::Pointer testAOE01=itk::AnalyzeObjectEntry::New();
  testAOE01->SetDisplayFlag(true);
  testAOE01->SetOpacity(0.123);

  itk::AnalyzeObjectEntry::Index Center;

  itk::AnalyzeObjectEntry::Pointer testAOE03=itk::AnalyzeObjectEntry::New();
  testAOE03->Copy(testAOE01); //Stress the operator=
  if( testAOE03->GetOpacity() != static_cast<float>(0.123F) )
    {
    error_count++;
    }
  //testAOE03->SetBlendFactor(5);
  //if( testAOE03->GetBlendFactor() != 5)
  //{
  //    error_count++;
  //}
  //if(testAOE03->GetDisplayFlag() != 1)
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
  //testAOE03->Copy(testAOE01);
  //if(testAOE03->GetStatusFlag() != false)
  //{
  //    error_count++;
  //}

  //testAOE03->SetBlendFactor(1);
  //if(testAOE03->GetBlendFactor() != 1)
  //{
  //    error_count++;
  //}
  //Center[0]=50;
  //Center[1]=51;
  //Center[2]=52;
  //testAOE03->SetCenter(Center);
  //if(testAOE03->GetCenter()[0]!=50)
  //{
  //    error_count++;
  //}
  //if(testAOE03->GetCenter()[1]!=51)
  //{
  //    error_count++;
  //}
  //if(testAOE03->GetCenter()[2] != 52)
  //{
  //    error_count++;
  //}
  //testAOE03->SetCopyFlag('n');
  //if(testAOE03->GetCopyFlag()!='n')
  //{
  //    error_count++;
  //}
  //testAOE03->SetDisplayFlag(2);
  //testAOE03->SetEndBlue(3);
  //itk::AnalyzeObjectEntry::intRGBPixel EndColor;
  //EndColor.SetBlue(32);
  //EndColor.SetRed(33);
  //EndColor.SetGreen(34);
  //testAOE03->SetEndColor(EndColor);
  //testAOE03->SetEndGreen(4);
  //testAOE03->SetEndRed(4);
  //itk::AnalyzeObjectEntry::Index MaximumCoordinateValue;
  //MaximumCoordinateValue[0] = 60;
  //MaximumCoordinateValue[1] = 61;
  //MaximumCoordinateValue[2] = 62;
  //testAOE03->SetMaximumCoordinateValue(MaximumCoordinateValue);
  //testAOE03->SetMaximumXValue(5);
  //testAOE03->SetMaximumYValue(6);
  //testAOE03->SetMaximumZValue(7);
  //itk::AnalyzeObjectEntry::Index MinimumCoordinateValue;
  //MinimumCoordinateValue[0] = 70;
  //MinimumCoordinateValue[1] = 71;
  //MinimumCoordinateValue[2] = 72;
  //testAOE03->SetMinimumCoordinateValue(MinimumCoordinateValue);
  //testAOE03->SetMinimumXValue(8);
  //testAOE03->SetMinimumYValue(9);
  //testAOE03->SetMinimumZValue(10);
  //testAOE03->SetMirrorFlag('t');
  //testAOE03->SetName("Test I win");
  //testAOE03->SetNeighborsUsedFlag('m');
  //testAOE03->SetOpacity(11.532);
  //testAOE03->SetOpacityThickness(12);
  //itk::AnalyzeObjectEntry::Index Rotation;
  //Rotation[0] = 80;
  //Rotation[1] =81;
  //Rotation[2] = 82;
  //testAOE03->SetRotation(Rotation);
  //itk::AnalyzeObjectEntry::Index RotationIncrement;
  //RotationIncrement[0]=90;
  //RotationIncrement[1] = 91;
  //RotationIncrement[2] = 92;
  //testAOE03->SetRotationIncrement(RotationIncrement);
  //testAOE03->SetShades(13);
  //testAOE03->SetStartBlue(14);
  //itk::AnalyzeObjectEntry::intRGBPixel StartColor;
  //StartColor.SetRed(3);
  //StartColor.SetGreen(5);
  //StartColor.SetBlue(7);
  //testAOE03->SetStartColor(StartColor);
  //testAOE03->SetStartGreen(15);
  //testAOE03->SetStartRed(16);
  //testAOE03->SetStatusFlag('p');
  //itk::AnalyzeObjectEntry::Index Translation;
  //Translation[0]=100;
  //Translation[1] = 101;
  //Translation[2] = 102;
  //testAOE03->SetTranslation(Translation);
  //itk::AnalyzeObjectEntry::Index TranslationIncrement;
  //TranslationIncrement[0]=110;
  //TranslationIncrement[1] = 111;
  //TranslationIncrement[2] = 112;
  //testAOE03->SetTranslationIncrement(TranslationIncrement);
  //testAOE03->SetXCenter(17);
  //testAOE03->SetYCenter(18);
  //testAOE03->SetZCenter(19);

  if( error_count )
    {
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}

