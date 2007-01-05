/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMrmlData.h,v $
  Date:      $Date: 2005/12/20 22:44:22 $
  Version:   $Revision: 1.11.2.1 $

=========================================================================auto=*/

#ifndef __vtkMrmlData_h
#define __vtkMrmlData_h


#include "vtkProcessObject.h"
#include "vtkIndirectLookupTable.h"
#include "vtkMrmlNode.h"
#include "vtkLookupTable.h"
#include "vtkImageData.h"
#include "vtkSlicer.h"


//----------------------------------------------------------------------------
class VTK_SLICER_BASE_EXPORT vtkMrmlData : public vtkProcessObject 
{
  public:
    static vtkMrmlData *New();

  vtkTypeMacro(vtkMrmlData,vtkProcessObject);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  // 
  // Dealing With Mrml Data
  //

  // Description:
  // Set/Get the MRML info
  vtkSetObjectMacro(MrmlNode, vtkMrmlNode);
  vtkGetObjectMacro(MrmlNode, vtkMrmlNode);
  
  // Description:
  // Copy another MmrlData's MrmlNode to this one
  // This does not need to be virtual 
  void CopyNode(vtkMrmlData *Data);

  //
  // Making sure the filters are UpToDate
  //

  // Description:
  // Provides opportunity to insure internal consistency before access.
  // Transfers all ivars from MrmlNode to internal VTK objects
  //
  // All subclasses MUST call vtkMRMLData::Update and vtkMRMLData::GetMTime
  virtual void Update();
  virtual unsigned long int GetMTime();
  vtkDataObject *GetOutput();

  //
  // Read/Write filters
  //

  // Description:
  // Read/Write the data
  // Return 1 on success, 0 on failure.
  virtual int Read() = 0;
  virtual int Write() = 0;

  // Description:
  // Has the object been changed in a way that one would want to write
  // it to disk? (to replace IsDirty...)
  vtkSetMacro(NeedToWrite, int);
  vtkGetMacro(NeedToWrite, int);
  vtkBooleanMacro(NeedToWrite, int);

  //
  // Display functions
  //

  // Description:
  // Get the indirect LUT (LookupTable).
  // If UseLabelLUT is on, then returns the LabelLUT, otherwise
  // the volume's own IndirectLookupTable.
  vtkIndirectLookupTable *GetIndirectLUT();

  // Description:
  // Set Label IndirectLookupTable
  vtkGetMacro(UseLabelIndirectLUT, int);
  vtkSetMacro(UseLabelIndirectLUT, int);
  vtkBooleanMacro(UseLabelIndirectLUT, int);
  vtkSetObjectMacro(LabelIndirectLUT, vtkIndirectLookupTable);
  vtkGetObjectMacro(LabelIndirectLUT, vtkIndirectLookupTable);

  // Description:
  // Set LookupTable
  void SetLookupTable(vtkLookupTable *lut) ;
  vtkLookupTable *GetLookupTable() ;

  // Description:
  // Enable or disable FMRI mapping 
  void EnableFMRIMapping(int yes) ;

  // Description:
  // For internal use during Read/Write
  vtkGetObjectMacro(ProcessObject, vtkProcessObject);

};

#endif
