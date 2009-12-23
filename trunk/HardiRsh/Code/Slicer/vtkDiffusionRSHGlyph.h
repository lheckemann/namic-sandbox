
#ifndef __vtkDiffusionRSHGlyph_h
#define __vtkDiffusionRSHGlyph_h

#include "itkRealSymSphericalHarmonicBasis.h"

#include "vtkGlyph3D.h"
#include "vnl/vnl_matrix.h"
#include "vtkPointData.h"
#include "vtkDataArray.h"
#include "vtkSmartPointer.h"
#include <vnl/vnl_matrix_fixed.txx>

VNL_MATRIX_FIXED_INSTANTIATE(double,6,6);
VNL_MATRIX_FIXED_INSTANTIATE(double,15,15);
VNL_MATRIX_FIXED_INSTANTIATE(double,28,28);
VNL_MATRIX_FIXED_INSTANTIATE(double,45,45);
VNL_MATRIX_FIXED_INSTANTIATE(double,66,66);

class vtkImageData;
class vtkMatrix4x4;

class VTK_GRAPHICS_EXPORT vtkDiffusionRSHGlyph : public vtkGlyph3D
{
public:
  vtkTypeRevisionMacro(vtkDiffusionRSHGlyph,vtkGlyph3D);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description
  // Construct object with scaling on and scale factor 1.0. Eigenvalues are 
  // extracted, glyphs are colored with input scalar data, and logarithmic
  // scaling is turned off.
  static vtkDiffusionRSHGlyph *New();

//BTX
  enum
  {
      COLOR_BY_VALUE,
      COLOR_BY_ORIENTATION
  };
//ETX

  // Description:
  // Set the color mode to be used for the glyphs.  This can be set to
  // use the input scalars (default) or to use the eigenvalues at the
  // point.  If ThreeGlyphs is set and the eigenvalues are chosen for
  // coloring then each glyph is colored by the corresponding
  // eigenvalue and if not set the color corresponding to the largest
  // eigenvalue is chosen.  The recognized values are:
  // COLOR_BY_SCALARS = 0 (default)
  // COLOR_BY_EIGENVALUES = 1
  vtkSetClampMacro(ColorMode, int, COLOR_BY_VALUE, COLOR_BY_ORIENTATION);
  vtkGetMacro(ColorMode, int);
  void ColorGlyphsByValue()
    {this->SetColorMode(COLOR_BY_VALUE);};
  void ColorGlyphsByOrientation()
    {this->SetColorMode(COLOR_BY_ORIENTATION);};  

  // Description
  // Transform output glyph locations (not orientations!) 
  // by this matrix.
  //
  // Example usage is as follows:
  // 1) Reformat a slice through a tensor volume.
  // 2) Set VolumePositionMatrix to the reformat matrix.
  //    This is analogous to setting the actor's UserMatrix
  //    to this matrix, which only works for scalar data.
  // 3) The output glyphs are positioned correctly without
  //    incorrectly rotating the tensors, as would be the 
  //    case if positioning the scene's actor with this matrix.
  // 
  virtual void SetVolumePositionMatrix(vtkMatrix4x4*);
  vtkGetObjectMacro(VolumePositionMatrix, vtkMatrix4x4);


  // Description
  // Transform output glyph orientations
  // by this matrix.
  //
  // Example usage is as follows:
  // 1) If tensors are to be displayed in a coordinate system
  //    that is not IJK (array-based), and the whole volume is
  //    being rotated, each tensor needs also to be rotated.
  //    First find the matrix that positions your volume.
  //    This is how the entire volume is positioned, not 
  //    the matrix that positions an arbitrary reformatted slice.
  // 2) Remove scaling and translation from this matrix; we
  //    just need to rotate each tensor.
  // 3) Set TensorRotationMatrix to this rotation matrix.
  //
  virtual void SetGlyphRotationMatrix(vtkMatrix4x4*);
  vtkGetObjectMacro(GlyphRotationMatrix, vtkMatrix4x4);

  // Description:
  // Resolution of the output glyphs. This parameter is a integer value
  // that sets the number of tensors (points) that are skipped before a glyph is rendered.
  // 1 is the finest level meaning that every input point a glyph is rendered.
  vtkSetClampMacro(Resolution,int,1,VTK_LARGE_INTEGER);
  vtkGetMacro(Resolution,int);

  // Description:
  // If the points in the image are organized into a 2D array (image),
  // The dimensions of that array. 
  // Two numbers: the number of columns and rows of pixels in each image.
  // If any of the numbers are < 1 a 1D point structure is assumed.
  vtkGetVector2Macro(Dimensions, int);
  vtkSetVector2Macro(Dimensions, int);

  // Description:
  // Resolution of the output glyphs in each dimension. 
  // It is used only if Dimensions is set > 1 .
  // This parameter is a integer value
  // that sets the number of tensors (points) that are skipped before a glyph is rendered.
  // 1 is the finest level meaning that every input point a glyph is rendered.
  vtkGetVector2Macro(DimensionResolution, int);
  vtkSetVector2Macro(DimensionResolution, int);

  vtkGetMacro(MinMaxNormalization,int);  
  vtkSetMacro(MinMaxNormalization,int);  
  vtkBooleanMacro(MinMaxNormalization,int);  

protected:
  vtkDiffusionRSHGlyph();
  ~vtkDiffusionRSHGlyph();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

  int ColorMode; // The coloring mode to use for the glyphs.
  
  int Resolution; // allows skipping some tensors for lower resolution glyphing
  int Dimensions[2];
  int DimensionResolution[2];

  vtkMatrix4x4 *VolumePositionMatrix;
  vtkMatrix4x4 *GlyphRotationMatrix;

  unsigned int              NumberOfSphericalHarmonics;
  unsigned int              Order;
//BTX
  vnl_matrix<double>        rshBasisMatrix;
//ETX

  virtual void InitalizeRshBasisMatrix( vtkPoints * );
  virtual double EvaluateBasis(int, double, double);
  
  virtual void GetDeformedSourcePoints( vtkPoints *, vtkDataArray *, double *);

  vtkPoints   *RshSourcePts; //Points used to generate the rshBasis
private:
  vtkDiffusionRSHGlyph(const vtkDiffusionRSHGlyph&);  // Not implemented.
  void operator=(const vtkDiffusionRSHGlyph&);  // Not implemented.
  
  int                      MinMaxNormalization;
};

#endif
