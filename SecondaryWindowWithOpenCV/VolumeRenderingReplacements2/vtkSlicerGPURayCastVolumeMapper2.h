/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkSlicerGPURayCastVolumeMapper2.h,v $
  
   Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   GPGPU Ray Cast Mapper 
  Module:    $HeadURL: $
  Date:      $Date: 2009-01-07  $
  Version:   $Revision:  $


=========================================================================*/
// .NAME vtkSlicerGPURayCastVolumeMapper2 - concrete implementation of 3D volume texture mapping

// .SECTION Description
// vtkSlicerGPURayCastVolumeMapper2 renders two volume using ray casting on 3D texture.
// See vtkVolumeTextureMapper3D for full description.

// .SECTION see also
// vtkVolumeTextureMapper3D vtkVolumeMapper

#ifndef __vtkSlicerGPURayCastVolumeMapper2_h
#define __vtkSlicerGPURayCastVolumeMapper2_h

#include "vtkSlicerGPUVolumeMapper2.h"
#include "vtkVolumeRenderingReplacements2.h"

#ifndef VTK_IMPLEMENT_MESA_CXX
# include "vtkOpenGL.h" // GLfloat type is used in some method signatures.
#endif
 
class vtkMatrix4x4;
class vtkRenderWindow;
class vtkVolumeProperty;

class VTK_VOLUMERENDERINGREPLACEMENTS_EXPORT vtkSlicerGPURayCastVolumeMapper2 : public vtkSlicerGPUVolumeMapper2
{
public:
  vtkTypeRevisionMacro(vtkSlicerGPURayCastVolumeMapper2,vtkSlicerGPUVolumeMapper2);
  void PrintSelf(ostream& os, vtkIndent indent);

  static vtkSlicerGPURayCastVolumeMapper2 *New();

  // Description:
  // Overall alpha for volume rendering result
  // Used for blending volume rendering with polygons
  // Default value: 1.0
  vtkSetMacro(GlobalAlpha, float);
  vtkGetMacro(GlobalAlpha, float);
  
  // Description:
  // Depth peeling threshold
  vtkSetMacro(DepthPeelingThreshold, float);
  vtkGetMacro(DepthPeelingThreshold, float);
  
  // Description:
  // Depth peeling threshold
  vtkSetMacro(FgBgRatio, float);
  vtkGetMacro(FgBgRatio, float);
  
  // Description:
  // set internal volume size
  void SetInternalVolumeSize(int size);
  
  // Description:
  // Enable/Disable clipping
  vtkSetMacro(Clipping, int);  
  vtkGetMacro(Clipping,int);
  vtkBooleanMacro(Clipping,int);
  
  // Description:
  // Set rendering technique
  void SetTechnique(int tech);
  
  // Description:
  // Set multi-volume color/opacity fusion method
  void SetColorOpacityFusion(int fusion);
  
  // Description:
  // Is hardware rendering supported? No if the input data is
  // more than one independent component, or if the hardware does
  // not support the required extensions
  int IsRenderSupported(vtkVolumeProperty *);
  
//BTX

  // Description:
  // WARNING: INTERNAL METHOD - NOT INTENDED FOR GENERAL USE
  // DO NOT USE THIS METHOD OUTSIDE OF THE RENDERING PROCESS
  // Render the volume
  virtual void Render(vtkRenderer *ren, vtkVolume *vol);

//ETX

  // Desciption:
  // Initialize when we go to render, or go to answer the
  // IsRenderSupported question. Don't call unless we have
  // a valid OpenGL context! 
  vtkGetMacro( Initialized, int );
  
  // Description:
  // Release any graphics resources that are being consumed by this texture.
  // The parameter window could be used to determine which graphic
  // resources to release.
  void ReleaseGraphicsResources(vtkWindow *);
  
protected:
  vtkSlicerGPURayCastVolumeMapper2();
  ~vtkSlicerGPURayCastVolumeMapper2();

//BTX  

  void GetLightInformation(vtkRenderer *ren,
                           vtkVolume *vol,
                           GLfloat lightDirection[2][4],
                           GLfloat lightDiffuseColor[2][4],
                           GLfloat lightSpecularColor[2][4],
                           GLfloat halfwayVector[2][4],
                           GLfloat *ambient );  
//ETX
    
  int              Initialized;
  int              RayCastInitialized;
  
  int              RayCastSupported;
  
  int              Technique;
  int              ColorOpacityFusion;
  float            FgBgRatio;
  int              Clipping;
  int              Shading;
  int              InternalVolumeSize;

  GLuint           Volume1Index;
  GLuint           Volume2Index;
  GLuint           Volume3Index;
  GLuint           ColorLookupIndex;
  GLuint           ColorLookup2Index;
  GLuint           RayCastVertexShader;
  GLuint           RayCastFragmentShader;
  GLuint           RayCastProgram;
  vtkRenderWindow *RenderWindow;
  
  double           VolumeBBoxVertices[8][3];
  double           VolumeBBoxVerticesColor[8][3];
  
  GLfloat          ParaMatrix[16];//4x4 matrix uniform for ray casting parameters
  
  float            RaySteps;
  float            GlobalAlpha;
  float            DepthPeelingThreshold;
  
  void Initialize();
  void InitializeRayCast();
  
  void RenderGLSL(vtkRenderer *pRen, vtkVolume *pVol);  
  void SetupTextures( vtkRenderer *ren, vtkVolume *vol );

  void DeleteTextureIndex( GLuint *index );
  void CreateTextureIndex( GLuint *index );
                         
  void DrawVolumeBBox();

  void SetupRayCastParameters( vtkRenderer *pRen, vtkVolume *pVol);
  
  void LoadVertexShader();
  
  // regular ray casting
  void LoadFragmentShader();
  void LoadNoShadingFragmentShader();
  
  // mip ray casting
  void LoadNoShadingFragmentShaderMIP();//lighting in MIP could be bad, so no shading here
  void LoadNoShadingFragmentShaderMINIP();//lighting in MIP could be bad, so no shading here
    
  void LoadRayCastProgram();

  void AdaptivePerformanceControl();  
  void PerformanceControl();
  
  // Description:
  // Check if we can support this texture size.
  int IsTextureSizeSupported( int size[3] );

  // Description:
  // Common code for setting up interpolation / clamping on 3D textures
  void Setup3DTextureParameters( vtkVolumeProperty *property );

private:
  vtkSlicerGPURayCastVolumeMapper2(const vtkSlicerGPURayCastVolumeMapper2&);  // Not implemented.
  void operator=(const vtkSlicerGPURayCastVolumeMapper2&);  // Not implemented.
  
  void PrintGLErrorString();
  void PrintFragmentShaderInfoLog();

  int ReloadShaderFlag;
};


#endif




