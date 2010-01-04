
#include "vtkDiffusionRSHGlyph.h"

#include "vtkCell.h"
#include "vtkCellArray.h"
#include "vtkDataSet.h"
#include "vtkExecutive.h"
#include "vtkFloatArray.h"
#include "vtkMath.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkTransform.h"
#include "vtkStreamingDemandDrivenPipeline.h"

//Needed for RGB lookup
#include "vtkDiffusionTensorGlyph.h"

vtkCxxRevisionMacro(vtkDiffusionRSHGlyph, "$Revision: 1.60 $");
vtkStandardNewMacro(vtkDiffusionRSHGlyph);
vtkCxxSetObjectMacro(vtkDiffusionRSHGlyph,VolumePositionMatrix,vtkMatrix4x4);
vtkCxxSetObjectMacro(vtkDiffusionRSHGlyph,GlyphRotationMatrix,vtkMatrix4x4);

// Construct object with scaling on and scale factor 1.0. Eigenvalues are 
// extracted, glyphs are colored with input scalar data, and logarithmic
// scaling is turned off.
vtkDiffusionRSHGlyph::vtkDiffusionRSHGlyph()
{
  this->ScaleFactor = 1.0;
  this->ColorMode = COLOR_BY_VALUE;

  // These can be optionally set by the user
  this->VolumePositionMatrix = NULL;
  this->GlyphRotationMatrix = NULL;

  // Default to highest rendering resolution
  this->Resolution = 1;

  this->Dimensions[0] = 0;
  this->Dimensions[1] = 0;
  this->DimensionResolution[0] = 20;
  this->DimensionResolution[1] = 20;

  // Default large scalar factor for diffusion data. 
  // Display small magnitude eigenvalues in mm space.
  this->ScaleFactor = 1000;
  this->SetNumberOfInputPorts(2);
  this->RshSourcePts = vtkPoints::New();

  this->MinMaxNormalization = 0; //Defualt No Min Max Normalization
}

//----------------------------------------------------------------------------
vtkDiffusionRSHGlyph::~vtkDiffusionRSHGlyph()
{
}

//----------------------------------------------------------------------------
void vtkDiffusionRSHGlyph::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "Source: " << this->GetSource() << "\n";
  os << indent << "Scale Factor: " << this->ScaleFactor << "\n";
  os << indent << "Color Mode: " << this->ColorMode << endl;
}

//----------------------------------------------------------------------------
int vtkDiffusionRSHGlyph::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{

  if ( this->MinMaxNormalization )
    std::cerr << " Request Data: Using MinMaxNormalization!" << std::endl;

  // get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  // get the input and ouptut
  vtkDataSet *input = vtkDataSet::SafeDownCast(
    inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkPolyData *output = vtkPolyData::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

  vtkPointData *pd;
  vtkDataArray *inScalars; // Scalars for Scaling
  vtkIdType numPts, numSourcePts, numSourceCells, inPtId, i;
  int index;
  vtkPoints *sourcePts = NULL;
  vtkPoints *defSourcePts = NULL;
  vtkPoints *newPts;
  vtkDataArray *newScalars=NULL;
  vtkDataArray *newNormals=NULL;
  vtkDataArray *defScalars=NULL;
  double x[3], x2[3], scalarTmp;
  vtkTransform *trans = vtkTransform::New();
  vtkCell *cell;
  vtkIdList *cellPts;
  int npts;
  vtkIdList *pts;
  vtkIdType ptIncr, cellId;
  vtkPointData *outputPD = output->GetPointData();
  vtkPolyData *defaultSource = NULL;
  vtkIdTypeArray *pointIds=0;
  vtkPolyData *source = 0;

  vtkDebugMacro(<<"Generating glyphs");

  pts = vtkIdList::New();
  pts->Allocate(VTK_CELL_SIZE);

  pd = input->GetPointData();
  inScalars = pd->GetScalars();
  
  ///TODO This should be made a check and the computation moved to setInput!
  this->NumberOfSphericalHarmonics = inScalars->GetNumberOfComponents();
  this->Order = 2 * (int) (
        ((1 + vcl_sqrt(8 * NumberOfSphericalHarmonics - 7)) / 2) / 2);

  double *rshComps = new double[NumberOfSphericalHarmonics];


  numPts = input->GetNumberOfPoints();
  if (numPts < 1)
    {
    vtkDebugMacro(<<"No points to glyph!");
    pts->Delete();
    trans->Delete();
    return 1;
    }

  // Allocate storage for output PolyData
  //
  outputPD->CopyAllOff();
  
  if (!this->GetSource(0, inputVector[1]))
    {
    defaultSource = vtkPolyData::New();
    defaultSource->Allocate();
    vtkPoints *defaultPoints = vtkPoints::New();
    defaultPoints->Allocate(6);
    defaultPoints->InsertNextPoint(0, 0, 0);
    defaultPoints->InsertNextPoint(1, 0, 0);
    vtkIdType defaultPointIds[2];
    defaultPointIds[0] = 0;
    defaultPointIds[1] = 1;
    defaultSource->SetPoints(defaultPoints);
    defaultSource->InsertNextCell(VTK_LINE, 2, defaultPointIds);
    defaultSource->SetUpdateExtent(0, 1, 0);
    this->SetSource(defaultSource);
    defaultSource->Delete();
    defaultSource = NULL;
    defaultPoints->Delete();
    defaultPoints = NULL;
    }

  source = this->GetSource(0, inputVector[1]);
  sourcePts = source->GetPoints();
  numSourcePts = sourcePts->GetNumberOfPoints();
  numSourceCells = source->GetNumberOfCells();

  //Make sure the basis is the right size...
  if (rshBasisMatrix.rows() != (unsigned int)numSourcePts ||
    rshBasisMatrix.cols() != this->NumberOfSphericalHarmonics )
  {
    vtkDebugMacro(<<"reinitializing RshBasisMatrix");
    this->InitalizeRshBasisMatrix(sourcePts);
  }
  
  // Prepare to copy output.
  pd = input->GetPointData();
  outputPD->CopyAllocate(pd,numPts*numSourcePts);

  newPts = vtkPoints::New();
  newPts->Allocate(numPts*numSourcePts);
  if ( this->GeneratePointIds )
    {
    pointIds = vtkIdTypeArray::New();
    pointIds->SetName(this->PointIdsName);
    pointIds->Allocate(numPts*numSourcePts);
    outputPD->AddArray(pointIds);
    pointIds->Delete();
    }

  //These should be set
  newNormals = vtkFloatArray::New();
  newNormals->SetNumberOfComponents(3);
  newNormals->Allocate(3*numPts*numSourcePts);
  newNormals->SetName("Normals");

  newScalars = vtkFloatArray::New();
  newScalars->SetNumberOfComponents(1);
  newScalars->Allocate(numPts*numSourcePts);
  newScalars->SetName("Scalars");
  
  defScalars = vtkFloatArray::New();  
  defScalars->SetNumberOfComponents(1);
  defScalars->Allocate(numSourcePts);
  defScalars->SetName("DeformedScalars");
  
  // Setting up for calls to PolyData::InsertNextCell()
  if (this->IndexMode != VTK_INDEXING_OFF )
    {
    output->Allocate(3*numPts*numSourceCells,numPts*numSourceCells);
    }
  else
    {
    output->Allocate(this->GetSource(0, inputVector[1]),
                     3*numPts*numSourceCells, numPts*numSourceCells);
    }

  // Traverse all Input points, transforming Source points and copying
  // point attributes.
  //

  // Figure out if we are transforming output point locations
  vtkTransform *userVolumeTransform = NULL;
  if (this->VolumePositionMatrix)
    {
    // user transform to move points
    userVolumeTransform = vtkTransform::New();
    
    userVolumeTransform->SetMatrix(this->VolumePositionMatrix);
    userVolumeTransform->PreMultiply();
    }

  trans->PreMultiply();

  //Make space for deformedSourcePoints
  defSourcePts = vtkPoints::New();
  
  // Compute steps along dimensions
  int skipRows = 0;
  int skipCols = this->Resolution;
  int rowLength = numPts;
  int colLength = 1;
  int row = 0;
  int col = 0;
  if (this->Dimensions[0] > 1 && this->Dimensions[1] > 1)
    {
    skipRows = DimensionResolution[1];
    skipCols = DimensionResolution[0];
    rowLength = Dimensions[0];
    colLength = Dimensions[1];
    }

  if (numPts != ( Dimensions[0] * Dimensions[1] ) )
  {
    std::cerr << "numpts Doesn't agree with Dimensions Array" << std::endl;
    return 1;
  }

  ptIncr=0;
  for (inPtId=0; inPtId < numPts; inPtId += skipCols)
    {

    if (col >= rowLength)
      {
      row += skipRows;
      inPtId = row * rowLength;
      col = 0;
      if (inPtId >= numPts)
        {
        break;
        }
      }
    col += skipCols;

    //~ scalex = scaley = scalez = 1.0;
    if ( ! (inPtId % 10000) )
      {
      this->UpdateProgress(static_cast<double>(inPtId)/numPts);
      if (this->GetAbortExecute())
        {
        break;
        }
      }

    // Clamp data scale if enabled
    if ( this->Clamping )
      {
      ///TODO Figure out Scale Clamping
      
      }
    
    // Compute index into table of glyphs
    if ( this->IndexMode == VTK_INDEXING_OFF )
      {
      index = 0;
      }
    else 
      {
      ///TODO Figure out this->IndexMode
      
      }

    // Make sure we're not indexing into empty glyph
    if ( this->GetSource(index, inputVector[1]) == NULL )
      {
      continue;
      }

    if (!this->IsPointVisible(input, inPtId))
      {
      continue;
      }
    
    inScalars->GetTuple( inPtId, rshComps );
    if ( rshComps[0] == 0 )
      {
      continue;
      }
    
    // Now begin copying/transforming glyph
    trans->Identity();
    
    // Copy all topology (transformation independent)
    for (cellId=0; cellId < numSourceCells; cellId++)
      {
      cell = this->GetSource(index, inputVector[1])->GetCell(cellId);
      cellPts = cell->GetPointIds();
      npts = cellPts->GetNumberOfIds();
      for (pts->Reset(), i=0; i < npts; i++) 
        {
        pts->InsertId(i,cellPts->GetId(i) + ptIncr);
        }
      output->InsertNextCell(cell->GetCellType(),pts);
      }
    
    // translate Source to Input point
    input->GetPoint(inPtId, x);
    
    // If we have a user-specified matrix modifying the output point locations
    if ( userVolumeTransform != NULL )
      {
      userVolumeTransform->TransformPoint(x,x2);
      trans->Translate(x2[0], x2[1], x2[2]);
      }  
    else
      {
      trans->Translate(x[0], x[1], x[2]);
      }

    
    // If we have a user-specified matrix rotating each tensor
    if (this->GlyphRotationMatrix)
      {
      trans->Concatenate(this->GlyphRotationMatrix);
      }
    
    
    // scale data if appropriate
    if ( this->Scaling )
      {
      trans->Scale(this->ScaleFactor,this->ScaleFactor,this->ScaleFactor);
      }
    
    
    //get the Deformed points.
    defSourcePts->DeepCopy(this->RshSourcePts);

      
    this->GetDeformedSourcePoints(defSourcePts, defScalars, rshComps);
    
    // multiply points and normals by resulting matrix
    trans->TransformPoints(defSourcePts,newPts);

    for (i = 0; i < numSourcePts; i++)
      {
      defScalars->GetTuple(i, &scalarTmp);
      newScalars->InsertTuple(i+ptIncr, &scalarTmp);
      }
    
    // Copy point data from source (if possible)
    if ( pd ) 
      {
        outputPD->CopyData(pd,inPtId,ptIncr);
      }

    // If point ids are to be generated, do it here
    if ( this->GeneratePointIds )
      {
      for (i=0; i < numSourcePts; i++)
        {
        pointIds->InsertNextValue(inPtId);
        }
      }

    ptIncr += numSourcePts;
    } 
  
  // Update ourselves and release memory
  //
  output->SetPoints(newPts);
  newPts->Delete();

  if (newScalars)
    {
    int idx = outputPD->SetScalars(newScalars);
    outputPD->SetActiveAttribute(idx, vtkDataSetAttributes::SCALARS);
    newScalars->Delete();
    }

  if (defScalars) defScalars->Delete();
  if (defSourcePts) defSourcePts->Delete();
  
  //~ outputPD->SetNormals(newNormals);
  if (newNormals) newNormals->Delete();

  output->Squeeze();
  trans->Delete();
  pts->Delete();

  return 1;
}


void vtkDiffusionRSHGlyph::InitalizeRshBasisMatrix(vtkPoints *sourcePts)
{
  
  this->RshSourcePts->Reset();
  
  //Rotate the SourcePts!
  if (this->GlyphRotationMatrix)
  {
    vtkTransform *rotate = vtkTransform::New();
    rotate->SetMatrix(this->GlyphRotationMatrix);
    rotate->TransformPoints(sourcePts, this->RshSourcePts );
    rotate->Delete();
  }
  
  vtkIdType numSourcePts = this->RshSourcePts->GetNumberOfPoints( );
  
  rshBasisMatrix.set_size( numSourcePts, this->NumberOfSphericalHarmonics );
  double pt[3];

  for (vtkIdType m = 0; m < numSourcePts; m++)
  {
    this->RshSourcePts->GetPoint(m,pt);

    //Normalize Pt to unit sphere.
    double norm = vcl_sqrt(pt[0] * pt[0] + pt[1] * pt[1] + pt[2] * pt[2]); 
    pt[0] /= norm; pt[1] /= norm; pt[2] /= norm; 

    /***
     * We use the following converntion
     * 
     * theta \in (0-PI) is measures from the postize z axis.
     * phi \in (0,2 PI) from the positive z axis.
     *
     * Grad directions relation to theta phi
     * x = sin(theta) * cos(phi)
     * y = sin(theta) * sin(phi)
     * z = cos(theta)
     */

    double theta, phi;
    {
      theta     = vcl_acos(pt[2]);
      phi       = vcl_atan2(pt[1],pt[0]); // atan2(y,x) = atan(y/x);
      phi       = phi < 0 ? phi + 2*vtkMath::Pi() : phi;
    }

    for (unsigned int c = 0; c < this->NumberOfSphericalHarmonics; c++)
    {
      rshBasisMatrix[m][c]  = EvaluateBasis(c,theta,phi);
    }

  }

}

double vtkDiffusionRSHGlyph::EvaluateBasis(
  int c,
  double theta,
  double phi
  )
{
  switch ( Order )
  {
    case 0:
    {
      return itk::RealSymSphericalHarmonicBasis<0>::Y(c+1,static_cast<double>(theta),static_cast<double>(phi));
      break;
    }
    case 2:
    {
      return itk::RealSymSphericalHarmonicBasis<2>::Y(c+1,static_cast<double>(theta),static_cast<double>(phi));
      break;
    }
    case 4:
    {
      return itk::RealSymSphericalHarmonicBasis<4>::Y(c+1,static_cast<double>(theta),static_cast<double>(phi));
      break;
    }
    case 6:
    {
      return itk::RealSymSphericalHarmonicBasis<6>::Y(c+1,static_cast<double>(theta),static_cast<double>(phi));
      break;
    }
    case 8:
    {
      return itk::RealSymSphericalHarmonicBasis<8>::Y(c+1,static_cast<double>(theta),static_cast<double>(phi));
      break;
    }
    case 10:
    {
      return itk::RealSymSphericalHarmonicBasis<10>::Y(c+1,static_cast<double>(theta),static_cast<double>(phi));
      break;
    }
    case 12:
    {
      return itk::RealSymSphericalHarmonicBasis<12>::Y(c+1,static_cast<double>(theta),static_cast<double>(phi));
      break;
    }
    case 14:
    {
      return itk::RealSymSphericalHarmonicBasis<14>::Y(c+1,static_cast<double>(theta),static_cast<double>(phi));
      break;
    }
    case 16:
    {
      return itk::RealSymSphericalHarmonicBasis<16>::Y(c+1,static_cast<double>(theta),static_cast<double>(phi));
      break;
    }
    case 18:
    {
      return itk::RealSymSphericalHarmonicBasis<18>::Y(c+1,static_cast<double>(theta),static_cast<double>(phi));
      break;
    }
    default:
    {
      std::cout << "Unsupported RSH ORDER : " << Order << std::endl;
      return -1.0;
      //EXCEPTION!!!
    }
  }

}


void vtkDiffusionRSHGlyph::GetDeformedSourcePoints( vtkPoints *newSourcePts, vtkDataArray *defScalars, double * rshVector)
{

  vnl_vector< double > coeffs(rshVector,NumberOfSphericalHarmonics);
  
  
  vnl_vector< double > values = rshBasisMatrix * coeffs;

  double value;
  vtkIdType numSourcePts = newSourcePts->GetNumberOfPoints( );
  double pt[3];

  double maxValue = 1.0;
  double minValue = 0.0;
  
  if ( this->ColorMode == COLOR_BY_VALUE || this->MinMaxNormalization )
  {
    maxValue = values.max_value();
    minValue = values.min_value();
  }
  
  for (vtkIdType m = 0; m < numSourcePts; m++)
  {
    newSourcePts->GetPoint(m,pt);

    if ( this->MinMaxNormalization ) 
    {
      value = (values[m] - minValue) / (maxValue - minValue);
    }
    else
    { //Not sure what is best Here! Currently No Normalization
      value = values[m];
    }
    
    pt[0] *= value;
    pt[1] *= value;
    pt[2] *= value;

    newSourcePts->SetPoint(m,pt);

    //Set the scalars for coloring
    if ( this->ColorMode == COLOR_BY_ORIENTATION)
    {
      double norm = vcl_sqrt(pt[0] * pt[0] + pt[1] * pt[1] + pt[2] * pt[2]); 
      pt[0] /= norm; pt[1] /= norm; pt[2] /= norm; 
      vtkDiffusionTensorGlyph::RGBToIndex(fabs(pt[0]),fabs(pt[1]),fabs(pt[2]),value);
    }
    else if ( this->ColorMode == COLOR_BY_VALUE)
    {
      value = values[m] / maxValue;
    }
    
    defScalars->SetTuple(m,&value);

  }

  
}

