/*=========================================================================

Program:   Maverick
Module:    $RCSfile: config.h,v $

Copyright (c) Kitware Inc. 28 Corporate Drive,
Clifton Park, NY, 12065, USA.

All rights reserved. No part of this software may be reproduced, distributed,
or modified, in any form or by any means, without permission in writing from
Kitware Inc.

IN NO EVENT SHALL THE AUTHORS OR DISTRIBUTORS BE LIABLE TO ANY PARTY FOR
DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
OF THE USE OF THIS SOFTWARE, ITS DOCUMENTATION, OR ANY DERIVATIVES THEREOF,
EVEN IF THE AUTHORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

THE AUTHORS AND DISTRIBUTORS SPECIFICALLY DISCLAIM ANY WARRANTIES, INCLUDING,
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE, AND NON-INFRINGEMENT.  THIS SOFTWARE IS PROVIDED ON AN
"AS IS" BASIS, AND THE AUTHORS AND DISTRIBUTORS HAVE NO OBLIGATION TO PROVIDE
MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

=========================================================================*/
#ifndef __RegisterImagesModuleLogic_h
#define __RegisterImagesModuleLogic_h

#include "vtkSharedModuleLogic.h"
#include <vector>

class VTK_EXPORT RegisterImagesModuleLogic : public vtkSharedModuleLogic
{
public:

  static RegisterImagesModuleLogic *New();
  vtkTypeMacro(RegisterImagesModuleLogic, vtkSharedModuleLogic);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  vtkSetMRMLNodeArgumentAsStringMacro(FixedVolumeNodeId, fixedImage)
  vtkSetMRMLNodeArgumentAsStringMacro(MovingVolumeNodeId, movingImage)
  vtkSetMRMLNodeArgumentAsStringMacro(ResampledVolumeNodeId, resampledImage)
  
  vtkGetMRMLNodeArgumentAsStringMacro(FixedVolumeNodeId, fixedImage)
  vtkGetMRMLNodeArgumentAsStringMacro(MovingVolumeNodeId, movingImage)
  vtkGetMRMLNodeArgumentAsStringMacro(ResampledVolumeNodeId, resampledImage)

  vtkSetMRMLNodeArgumentAsStringMacro(Registration, registration);

  vtkSetMRMLNodeArgumentAsStringMacro(Initialization, initialization);

  vtkSetMRMLNodeArgumentAsStringMacro(LoadTransform, loadTransform);

  vtkSetMRMLNodeArgumentAsStringMacro(SaveTransform, saveTransform);
  
  void SetLandmarks ( const std::vector<std::vector<float> > &fixed, 
                      const std::vector<std::vector<float> > &moving);
  int AddLandmark ( const std::vector<float> &fixed, const std::vector<float> &moving);
  /** create a landmark if it doesn't exists */
  void SetLandmark ( int landmark, 
                     const std::vector<float> &fixed, 
                     const std::vector<float> &moving);
  void RemoveLandmark( int landmark );
  
protected:
  vtkSetMRMLNodeArgumentAsStringMacro(FixedLandmarks, fixedLandmarks);
  vtkSetMRMLNodeArgumentAsStringMacro(MovingLandmarks, movingLandmarks);
  
  RegisterImagesModuleLogic();
  virtual ~RegisterImagesModuleLogic();
  RegisterImagesModuleLogic(const RegisterImagesModuleLogic&);
  void operator=(const RegisterImagesModuleLogic&);

  // Derived to modified the output node
  void ApplyTask(void *clientdata);  
  
  std::vector< std::vector<float> > m_FixedLandmarks;
  std::vector< std::vector<float> > m_MovingLandmarks;
};

#endif