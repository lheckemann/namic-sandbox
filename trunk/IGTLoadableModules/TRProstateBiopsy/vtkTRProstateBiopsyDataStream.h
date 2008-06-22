#ifndef __vtkTRProstateBiopsyDataStream_h
#define __vtkTRProstateBiopsyDataStream_h

#include "vtkIGTWin32Header.h" 
#include "vtkIGTDataStream.h"

#include "vtkMatrix4x4.h"

#include "vtkIGTOpenTrackerStream.h"
#include "vtkIGTMessageAttributeSet.h"


class VTK_TRPROSTATEBIOPSY_EXPORT vtkTRProstateBiopsyDataStream : public vtkIGTOpenTrackerStream
{
public:

  static vtkTRProstateBiopsyDataStream *New();
  vtkTypeRevisionMacro(vtkTRProstateBiopsyDataStream,vtkIGTOpenTrackerStream);
  void PrintSelf(ostream& os, vtkIndent indent);

  vtkGetObjectMacro(NeedleMatrix,vtkMatrix4x4);

  //Description:
  //Constructor
  vtkTRProstateBiopsyDataStream();

  //Description:
  //Destructor
  virtual ~vtkTRProstateBiopsyDataStream();
  
  void Init(const char *configFile);
  
  void AddCallbacks();
  static void OnRecieveMessageFromRobot(vtkIGTMessageAttributeSet* data, void* arg);
  static void OnRecieveMessageFromScanner(vtkIGTMessageAttributeSet* data, void* arg);

  //BTX
  std::string GetRobotStatus();
  std::string GetScanStatus();
  
  void SetRobotPosition(std::vector<float> pos, std::vector<float> ori);
  void SetRobotCommand(std::string key, std::string value);

  void SetScanPosition(std::vector<float> pos, std::vector<float> ori);
  void SetScanCommand(std::string key, std::string value);
  //ETX

  vtkTransform* GetNeedleTransform();

  void Normalize(float *a);  
  void Cross(float *a, float *b, float *c);

private:

  //Context *context;
  
  //void CloseConnection();
  vtkIGTMessageAttributeSet* AttrSetRobot;    // <- Should it be a circular buffer?
  vtkIGTMessageAttributeSet* AttrSetScanner;  // <- Should it be a circular buffer?

  vtkMatrix4x4* NeedleMatrix;
  vtkTransform* NeedleTransform;

};

#endif // __vtkTRProstateBiopsyDataStream_h




