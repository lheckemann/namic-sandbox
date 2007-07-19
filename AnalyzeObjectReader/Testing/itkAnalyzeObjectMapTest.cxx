#include "objectmap.h"

int main (int argc, char * argv[])
{
  int error_count = 0 ;
    
  itk::AnalyzeObjectEntry::Pointer testAOE01=itk::AnalyzeObjectEntry::New();

  testAOE01->SetDisplayFlag(true);
  testAOE01->SetOpacity(0.123);

  itk::AnalyzeObjectEntry::Pointer testAOE03=itk::AnalyzeObjectEntry::New();
  testAOE03->Copy(testAOE01); //Stress the operator=
  if( testAOE03->GetOpacity() != static_cast<float>(0.123F) )
  {
    error_count++;
  }
  testAOE03->SetBlendFactor(5);
  if( testAOE03->GetBlendFactor() != 5)
  {
    error_count++;
  }
  if(testAOE03->GetDisplayFlag() != 1)
  {
    error_count++;
  }
  if(testAOE01->GetMaximumXValue() != 10)
  {
    error_count++;
  }
  testAOE01->SetStartBlue(5);
  if(testAOE01->GetStartBlue() != 5)
  {
    error_count++;
  }
  itk::AnalyzeObjectEntry::Index Center;
  Center[0] = 3;
  Center[1] = 5;
  Center[2] = 10;
  testAOE01->SetCenter(Center);

  if(testAOE01->GetCenter()[0]!=3)
  {
    error_count++;
  }
  if(testAOE01->GetCenter()[1] != 5)
  {
    error_count++;
  }
  if(testAOE01->GetCenter()[2] != 10)
  {
    error_count++;
  }
  testAOE01->SetStatusFlag(false);
  
  if(testAOE01->GetStatusFlag() != false)
  {
    error_count++;
  }
  testAOE03->Copy(testAOE01);
  if(testAOE03->GetStatusFlag() != false)
  {
    error_count++;
  }

  testAOE03->SetBlendFactor(1);
  if(testAOE03->GetBlendFactor() != 1)
  {
    error_count++;
  }
  Center[0]=50;
  Center[1]=51;
  Center[2]=52;
  testAOE03->SetCenter(Center);
  if(testAOE03->GetCenter()[0]!=50)
  {
    error_count++;
  }
  if(testAOE03->GetCenter()[1]!=51)
  {
    error_count++;
  }
  if(testAOE03->GetCenter()[2] != 52)
  {
    error_count++;
  }
  testAOE03->SetCopyFlag('n');
  if(testAOE03->GetCopyFlag()!='n')
  {
    error_count++;
  }
  testAOE03->SetDisplayFlag(2);
  if(testAOE03->GetDisplayFlag()!=2)
  {
    error_count++;
  }
  testAOE03->SetEndBlue(3);
  if(testAOE03->GetEndBlue()!=3)
  {
    error_count++;
  }
  itk::AnalyzeObjectEntry::intRGBPixel EndColor;
  EndColor.SetBlue(32);
  EndColor.SetRed(33);
  EndColor.SetGreen(34);
  testAOE03->SetEndColor(EndColor);
  if(testAOE03->GetEndColor()!=EndColor)
  {
    error_count++;
  }
  testAOE03->SetEndGreen(4);
  if(testAOE03->GetEndGreen()!=4)
  {
    error_count++;
  }
  testAOE03->SetEndRed(5);
  if(testAOE03->GetEndRed()!=5)
  {
    error_count++;
  }
  itk::AnalyzeObjectEntry::Index MaximumCoordinateValue;
  MaximumCoordinateValue[0] = 60;
  MaximumCoordinateValue[1] = 61;
  MaximumCoordinateValue[2] = 62;
  testAOE03->SetMaximumCoordinateValue(MaximumCoordinateValue);
  if(testAOE03->GetMaximumCoordinateValue()!=MaximumCoordinateValue)
  {
    error_count++;
  }
  testAOE03->SetMaximumXValue(5);
  if(testAOE03->GetMaximumXValue()!=5)
  {
    error_count++;
  }
  testAOE03->SetMaximumYValue(6);
  if(testAOE03->GetMaximumYValue()!=6)
  {
    error_count++;
  }
  testAOE03->SetMaximumZValue(7);
  if(testAOE03->GetMaximumZValue()!=7)
  {
    error_count++;
  }
  itk::AnalyzeObjectEntry::Index MinimumCoordinateValue;
  MinimumCoordinateValue[0] = 70;
  MinimumCoordinateValue[1] = 71;
  MinimumCoordinateValue[2] = 72;
  testAOE03->SetMinimumCoordinateValue(MinimumCoordinateValue);
  if(testAOE03->GetMinimumCoordinateValue()!=MinimumCoordinateValue)
  {
    error_count++;
  }
  testAOE03->SetMinimumXValue(8);
  if(testAOE03->GetMinimumXValue()!=8)
  {
    error_count++;
  }
  testAOE03->SetMinimumYValue(9);
  if(testAOE03->GetMinimumYValue()!=9)
  {
    error_count++;
  }
  testAOE03->SetMinimumZValue(10);
  if(testAOE03->GetMinimumZValue()!=10)
  {
    error_count++;
  }
  testAOE03->SetMirrorFlag('t');
  if(testAOE03->GetMirrorFlag()!='t')
  {
    error_count++;
  }
  testAOE03->SetName("Test I win");
  if(testAOE03->GetName()!="Test I win")
  {
    error_count++;
  }
  testAOE03->SetNeighborsUsedFlag('m');
  if(testAOE03->GetNeighborsUsedFlag()!='m')
  {
    error_count++;
  }
  testAOE03->SetOpacity(11.532);
  if(testAOE03->GetOpacity()!=(float)11.532)
  {
    error_count++;
  }
  testAOE03->SetOpacityThickness(12);
  if(testAOE03->GetOpacityThickness()!=12)
  {
    error_count++;
  }
  itk::AnalyzeObjectEntry::Index Rotation;
  Rotation[0] = 80;
  Rotation[1] =81;
  Rotation[2] = 82;
  testAOE03->SetRotation(Rotation);
  if(testAOE03->GetRotation()!=Rotation)
  {
    error_count++;
  }
  itk::AnalyzeObjectEntry::Index RotationIncrement;
  RotationIncrement[0]=90;
  RotationIncrement[1] = 91;
  RotationIncrement[2] = 92;
  testAOE03->SetRotationIncrement(RotationIncrement);
  if(testAOE03->GetRotationIncrement()!=RotationIncrement)
  {
    error_count++;
  }
  testAOE03->SetShades(13);
  if(testAOE03->GetShades()!=13)
  {
    error_count++;
  }
  testAOE03->SetStartBlue(14);
  if(testAOE03->GetStartBlue()!=14)
  {
    error_count++;
  }
  itk::AnalyzeObjectEntry::intRGBPixel StartColor;
  StartColor.SetRed(3);
  StartColor.SetGreen(5);
  StartColor.SetBlue(7);
  testAOE03->SetStartColor(StartColor);
  if(testAOE03->GetStartColor()!=StartColor)
  {
    error_count++;
  }
  testAOE03->SetStartGreen(15);
  if(testAOE03->GetStartGreen()!=15)
  {
    error_count++;
  }
  testAOE03->SetStartRed(16);
  if(testAOE03->GetStartRed()!=16)
  {
    error_count++;
  }
  testAOE03->SetStatusFlag('p');
  if(testAOE03->GetStatusFlag()!='p')
  {
    error_count++;
  }
  itk::AnalyzeObjectEntry::Index Translation;
  Translation[0]=100;
  Translation[1] = 101;
  Translation[2] = 102;
  testAOE03->SetTranslation(Translation);
  if(testAOE03->GetTranslation()!=Translation)
  {
    error_count++;
  }
  itk::AnalyzeObjectEntry::Index TranslationIncrement;
  TranslationIncrement[0]=110;
  TranslationIncrement[1] = 111;
  TranslationIncrement[2] = 112;
  testAOE03->SetTranslationIncrement(TranslationIncrement);
  if(testAOE03->GetTranslationIncrement()!=TranslationIncrement)
  {
    error_count++;
  }
  testAOE03->SetXCenter(17);
  if(testAOE03->GetXCenter()!=17)
  {
    error_count++;
  }
  testAOE03->SetYCenter(18);
  if(testAOE03->GetYCenter()!=18)
  {
    error_count++;
  }
  testAOE03->SetZCenter(19);
  if(testAOE03->GetZCenter()!=19)
  {
    error_count++;
  }

  itk::AnalyzeObjectMap::Pointer testAOE02 = itk::AnalyzeObjectMap::New();

  testAOE02->SetNumberOfObjects(55);
  if(testAOE02->GetNumberOfObjects() != 55)
  {
    error_count++;
  }
  testAOE02->SetNumberOfVolumes(155);
  if(testAOE02->GetNumberOfVolumes() != 155)
  {
    error_count++;
  }
  testAOE02->SetVersion(880102);
  if(testAOE02->GetVersion()!=880102)
  {
    error_count++;
  }
  testAOE02->SetXDim(123);
  if(testAOE02->GetXDim()!=123)
  {
    error_count++;
  }
  testAOE02->SetYDim(182);
  if(testAOE02->GetYDim() !=182)
  {
    error_count++;
  }
  testAOE02->SetZDim(254);
  if(testAOE02->GetZDim()!=254)
  {
    error_count++;
  }

  //const bool objectReadStatus = testAOE02->ReadObjectFile(argv[1]);
  const bool objectReadStatus = testAOE02->ReadObjectFile("D:/object_good/Data/test.obj");

  const bool objectWriteStatus = testAOE02->WriteObjectFile("D:/object_good/Data/testWrite6.obj");

  std::ifstream ReferenceFile;
  ReferenceFile.open("D:/object_good/Data/test.obj", std::ios::binary | std::ios::in);

  std::ifstream WrittenFile;
  WrittenFile.open("D:/object_good/Data/testWrite6.obj", std::ios::binary | std::ios::in);

  char ReferenceBytes;
  char WrittenBytes;
  int count = 0;
  std::ofstream myfile;
  myfile.open("testing1.txt");
  while(!ReferenceFile.eof()  && !WrittenFile.eof())
  {
    count++;
    ReferenceFile.read(reinterpret_cast<char *> (&ReferenceBytes), sizeof(char));
    WrittenFile.read(reinterpret_cast<char *>(&WrittenBytes), sizeof(char));
    if(ReferenceBytes != WrittenBytes)
    {
        error_count++;
        myfile<<count<< ":" << (int)ReferenceBytes << ":" << (int)WrittenBytes << std::endl;
    }
  }

  if(!ReferenceFile.eof())
  {
      error_count++;
      std::cout<<"ReferenceFile is not at end of file"<<std::endl;
  }
  if(!WrittenFile.eof())
  {
      error_count++;
      std::cout<<"WrittenFile is not at end of file"<<std::endl;
  }

  ReferenceFile.close();
  WrittenFile.close();
  if(!objectReadStatus)
  {
    std::cout<<"Reading the file failed"<<std::cout;
    error_count++;
  }

  if(!objectWriteStatus)
  {
      error_count++;
      std::cout<<"Writing the file failed"<<std::cout;
  }

  testAOE02->getObjectEntry(5)->SetEndBlue(500);

  const bool objectWriteStatusTwo = testAOE02->WriteObjectFile("D:/object_good/Data/testWrite8.obj");

  itk::AnalyzeObjectMap::Pointer testAOE04 = itk::AnalyzeObjectMap::New();

  const bool objectReadStatusTwo =  testAOE04->ReadObjectFile("D:/object_good/Data/testWrite8.obj");

  int EndBlue = testAOE02->getObjectEntry(5)->GetEndBlue();
  itk::ByteSwapper<int>::SwapFromSystemToBigEndian(&EndBlue);

  if(EndBlue != testAOE04->getObjectEntry(5)->GetEndBlue())
  {
    error_count++;
  }

  if(!objectReadStatusTwo)
  {
    error_count++;
  }

  if(!objectWriteStatusTwo)
  {
    error_count++;
  }

  if( error_count )
  {
  return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
