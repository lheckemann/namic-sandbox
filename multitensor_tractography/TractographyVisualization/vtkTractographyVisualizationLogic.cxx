#include <cfloat>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>

#include "itkListSample.h"
#include "itkVector.h"
#include "Statistics/itkCovarianceCalculator.h"
#include "Statistics/itkMeanCalculator.h"
#include "vtkCellArray.h"
#include "vtkFloatArray.h"
#include "vtkLine.h"
#include "vtkObjectFactory.h"
#include "vtkMRMLScene.h"
#include "vtkPointData.h"
#include "vtkSmartPointer.h"
#include "vtkTractographyVisualizationLogic.h"
#include "vtkTractographyVisualization.h"
#include "vtkTubeFilter.h"


//------------------------------------------------------------------------------
vtkTractographyVisualizationLogic* vtkTractographyVisualizationLogic::New()
{
  // First try to create the object from the vtkObjectFactory.
  vtkObject* ret =
      vtkObjectFactory::CreateInstance("vtkTractographyVisualizationLogic");
  if(ret)
  {
    return (vtkTractographyVisualizationLogic*)ret;
  }

  // If the factory was unable to create the object, then create it here.
  return new vtkTractographyVisualizationLogic;
}


//------------------------------------------------------------------------------
vtkTractographyVisualizationLogic::vtkTractographyVisualizationLogic()
{
  this->TractographyVisualizationNode = NULL;

  this->TmpData = NULL;
  this->TmpNode = NULL;
  this->TmpDisplayNode = NULL; 
  this->TmpTubeDisplayNode = NULL; 

  this->DownSampled = false;

  this->MinValues = NULL;
  this->MaxValues = NULL;

  this->MinFaValues = NULL;
  this->MaxFaValues = NULL;

  this->TubeFilter = NULL;

  this->FaAvailable = false;
}


//------------------------------------------------------------------------------
vtkTractographyVisualizationLogic::~vtkTractographyVisualizationLogic()
{
  vtkSetMRMLNodeMacro(this->TractographyVisualizationNode, NULL);

  if (this->TmpData)
  {
    this->TmpData->Delete();
    this->TmpNode->Delete();
    this->TmpDisplayNode->Delete();

    if (this->DownSampled)
    {
      this->PolyData->Delete();
    }

    this->MinValues->Delete();
    this->MaxValues->Delete();

    if (this->MinFaValues) {
      this->MinFaValues->Delete();
      this->MaxFaValues->Delete();
    }

    this->TubeFilter->Delete();
  }
}


//------------------------------------------------------------------------------
void vtkTractographyVisualizationLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkstd::cout << "printself TODO" << vtkstd::endl;
}


//------------------------------------------------------------------------------
void vtkTractographyVisualizationLogic::Apply()
{
  //vtkstd::cout << "apply" << vtkstd::endl;

  vtkMRMLTractographyVisualizationNode *n =
      this->TractographyVisualizationNode;

  double threshold = n->GetThreshold();

  vtkMRMLModelNode *inputModelNode = vtkMRMLModelNode::SafeDownCast(
      this->GetMRMLScene()->GetNodeByID(n->GetInputModelRef()));

  vtkSmartPointer<vtkFloatArray> values;
  if (n->GetValueType() == 0) {
    values = vtkFloatArray::SafeDownCast(
        inputModelNode->GetPolyData()->GetPointData()->GetArray("norm"));
  } else {
    values = vtkFloatArray::SafeDownCast(
        inputModelNode->GetPolyData()->GetPointData()->GetArray("FA"));
  }

  if (!values)
  {
    vtkErrorMacro("The model doesn't have 'values'");
    return;
  }

  this->TmpData->Reset();
  this->TmpData->SetPoints(this->PolyData->GetPoints());
  this->TmpData->Allocate();

  // Also use the same values.
  this->TmpData->GetPointData()->AddArray(values);

  // Iterate through all lines.
  vtkIdType numLines = this->PolyData->GetNumberOfLines();
  vtkIdList *ptIds = vtkIdList::New();
  //vtkstd::cout << "start create cells " << numLines << vtkstd::endl;
  bool perLine = n->GetRemovalMode();
  for (vtkIdType i = 0; i < numLines; ++i)
  {
    this->PolyData->GetCellPoints(i, ptIds);
    if (ptIds->GetNumberOfIds() < 2)
    {
      continue;
    }

    if (perLine)
    {
      if (n->GetValueType() == 0) {
        if (this->MinValues->GetValue(i) < threshold)
        {
          this->PolyData->GetCellPoints(i, ptIds);
          this->TmpData->InsertNextCell(VTK_LINE, ptIds);
        }
      } else {
        if (this->MinFaValues->GetValue(i) < threshold)
        {
          this->PolyData->GetCellPoints(i, ptIds);
          this->TmpData->InsertNextCell(VTK_LINE, ptIds);
        }
      }
    }
    else
    {
      vtkIdList *ptIdsKeep = vtkIdList::New();

      for (vtkIdType j = 0; j < ptIds->GetNumberOfIds(); ++j)
      {
        if (values->GetValue(ptIds->GetId(j)) < threshold)
        {
          ptIdsKeep->InsertNextId(ptIds->GetId(j));
        }
        else
        {
          break;
        }
      }

      if (ptIdsKeep->GetNumberOfIds() >= 2)
      {
        this->TmpData->InsertNextCell(VTK_LINE, ptIdsKeep);
      }
      ptIdsKeep->Delete();
    }
  }
  ptIds->Delete();

  inputModelNode->GetModelDisplayNode()->SetVisibility(0);
  this->TmpDisplayNode->SetVisibility(1);

  // Careful, this makes it really slow, we should not recalculate the tubes
  // every time, should be in done in preprocessing.
  /*if (n->GetShowTubes())
  {
    this->TubeFilter->Update();
  }*/
}


//------------------------------------------------------------------------------
bool vtkTractographyVisualizationLogic::CreateTempNode()
{
  vtkMRMLTractographyVisualizationNode *n =
      this->TractographyVisualizationNode;

  // Check if input is valid.
  //vtkstd::cout << "Check input." << vtkstd::endl;
  vtkMRMLModelNode *inputModelNode = vtkMRMLModelNode::SafeDownCast(
      this->GetMRMLScene()->GetNodeByID(n->GetInputModelRef()));
  if (!inputModelNode)
  {
    vtkErrorMacro("No input model found.");
    return false;
  }

  if (!inputModelNode->GetPolyData())
  {
    vtkErrorMacro("Input model has no poly data.");
    return false;
  }

  if (!inputModelNode->GetPolyData()->GetPointData())
  {
    vtkErrorMacro("Input model has no point data.");
    return false;
  }

  if (!inputModelNode->GetPolyData()->GetPointData()->GetArray("norm"))
  {
    vtkErrorMacro("The model doesn't have 'norm' values");
    return false;
  }

  this->CleanUp();

  this->PolyData = inputModelNode->GetPolyData();
  if (this->PolyData->GetPointData()->GetArray("FA")) {
    this->FaAvailable = true;
  }

  // Create temporary node.
  //vtkstd::cout << "Create temporary node." << vtkstd::endl;

  this->TmpData = vtkPolyData::New();
  this->TmpNode = vtkMRMLModelNode::New();
  this->TmpDisplayNode = vtkMRMLModelDisplayNode::New();

  TmpNode->SetScene(this->GetMRMLScene());
  TmpDisplayNode->SetScene(this->GetMRMLScene());

  this->GetMRMLScene()->AddNode(TmpNode);
  this->GetMRMLScene()->AddNode(TmpDisplayNode);

  this->TubeFilter = vtkTubeFilter::New();
  this->TubeFilter->SetInput(this->TmpData);
  this->TubeFilter->SetRadius(0.5);
  this->TubeFilter->SetNumberOfSides(6);
  this->TubeFilter->Update();

  this->TmpNode->SetAndObservePolyData(this->TmpData);
  this->TmpNode->SetAndObserveDisplayNodeID(this->TmpDisplayNode->GetID());

  this->TmpDisplayNode->SetPolyData(TmpNode->GetPolyData());

  this->TmpNode->SetName("TemporaryNode");
  this->TmpDisplayNode->SetName("TemporaryDisplayNode");

  // Find max and min values.
  this->MinValues = vtkFloatArray::New(); 
  this->MaxValues = vtkFloatArray::New(); 
  this->CalculateMinMax("norm", this->MinValues, this->MaxValues, this->NormMin,
                        this->NormMax);
  this->NormDefaultThres = this->CalculateStatistics("norm", this->PolyData);
  if (this->FaAvailable) {
    this->MinFaValues = vtkFloatArray::New(); 
    this->MaxFaValues = vtkFloatArray::New(); 
    this->CalculateMinMax("FA", this->MinFaValues, this->MaxFaValues,
                          this->FaMin, this->FaMax);
    this->FaDefaultThres = this->CalculateStatistics("FA", this->PolyData);
  }

  SetValueType();

  // Subsample if necessary. At most we want 10k lines, also remove those lines
  // existing of only one point.
  vtkIdType numLines = this->PolyData->GetNumberOfLines();

  const int maxNumLines = 11500;

  if (numLines > maxNumLines)
  {
    int filter =
        static_cast<int>(ceil(static_cast<float>(numLines) / maxNumLines));
    vtkstd::cout << "Downsample" << vtkstd::endl << "Filter: " << filter
                 << vtkstd::endl << "NumLines: "
                 << this->PolyData->GetNumberOfLines() << vtkstd::endl;

    vtkPolyData *DownSampledData = vtkPolyData::New();
    DownSampledData->SetPoints(this->PolyData->GetPoints());
    DownSampledData->Allocate();

    vtkIdList *ptIds = vtkIdList::New();
    for (vtkIdType i = 0; i < this->PolyData->GetNumberOfLines(); ++i)
    {
      if (!(i % filter))
      {
        this->PolyData->GetCellPoints(i, ptIds);
        DownSampledData->InsertNextCell(VTK_LINE, ptIds);
      }
    }
    ptIds->Delete();

    this->PolyData = DownSampledData;
    this->DownSampled = true;
    vtkstd::cout << "NumLines after: " << this->PolyData->GetNumberOfLines()
                 << vtkstd::endl;
  }

  return true;
}


//------------------------------------------------------------------------------
void vtkTractographyVisualizationLogic::CleanUp()
{
  //vtkstd::cout << "cleanup" << vtkstd::endl;
  if (this->TmpData)
  {
    //vtkstd::cout << "Delete old temporary node." << vtkstd::endl;

    this->GetMRMLScene()->RemoveNode(TmpNode);
    this->GetMRMLScene()->RemoveNode(TmpDisplayNode);

    this->TmpData->Delete();
    this->TmpNode->Delete();
    this->TmpDisplayNode->Delete();

    if (this->DownSampled)
    {
      this->PolyData->Delete();
    }

    this->MinValues->Delete();
    this->MaxValues->Delete();

    if (this->MinFaValues) {
      this->MinFaValues->Delete();
      this->MaxFaValues->Delete();
      this->MinFaValues = NULL;
      this->MaxFaValues = NULL;
    }

    this->TubeFilter->Delete();
  }
  this->DownSampled = false;
  this->FaAvailable = false;

  this->TmpData = NULL;
  this->TmpNode = NULL;
  this->TmpDisplayNode = NULL;
}


//------------------------------------------------------------------------------
float vtkTractographyVisualizationLogic::CalculateStatistics(const char *name,
                                                             vtkPolyData *data)
{
  //vtkstd::cout << name << vtkstd::endl;
  const unsigned int measurementVectorLength = 1;
  typedef itk::Vector<float, measurementVectorLength> MeasurementVectorType;
  typedef itk::Statistics::ListSample<MeasurementVectorType> SampleType;
  SampleType::Pointer sample = SampleType::New();

  vtkSmartPointer<vtkFloatArray> values = 
      vtkFloatArray::SafeDownCast(data->GetPointData()->GetArray(name));
  if (!values)
  {
    vtkErrorMacro("The model doesn't have the required values");
    return 0;
  }

  sample->SetMeasurementVectorSize(measurementVectorLength);

  for (vtkIdType i = 0; i < values->GetNumberOfTuples(); ++i)
  {
    MeasurementVectorType mv;
    mv[0] = values->GetValue(i);
    sample->PushBack(mv);
  }
               
  typedef itk::Statistics::MeanCalculator<SampleType> MeanAlgorithmType;
  MeanAlgorithmType::Pointer meanAlgorithm = MeanAlgorithmType::New();
  meanAlgorithm->SetInputSample(sample);
  meanAlgorithm->Update();

  //vtkstd::cout << "Sample mean = " << *(meanAlgorithm->GetOutput())
  //             << vtkstd::endl;

  typedef itk::Statistics::CovarianceCalculator<SampleType>
      CovarianceAlgorithmType;
  CovarianceAlgorithmType::Pointer covarianceAlgorithm =
      CovarianceAlgorithmType::New();

  covarianceAlgorithm->SetInputSample(sample);
  covarianceAlgorithm->SetMean(meanAlgorithm->GetOutput());
  covarianceAlgorithm->Update();
  //vtkstd::cout << "Sample covariance = " << *(covarianceAlgorithm->GetOutput())
  //             << vtkstd::endl;

  double thres = (*meanAlgorithm->GetOutput())[0] +
      std::sqrt((*covarianceAlgorithm->GetOutput())[0][0]) * 2;

  return thres;
}


//------------------------------------------------------------------------------
void vtkTractographyVisualizationLogic::ToggleTubeDisplay()
{
  //vtkstd::cout << "toggle tubes" << vtkstd::endl;

  vtkMRMLTractographyVisualizationNode *n =
      this->TractographyVisualizationNode;

  this->TubeFilter->Update();
  if (n->GetShowTubes())
  {
    TmpNode->SetAndObservePolyData(this->TubeFilter->GetOutput());
  }
  else
  {
    TmpNode->SetAndObservePolyData(this->TmpData);
  }

  this->TmpDisplayNode->SetPolyData(TmpNode->GetPolyData());

  this->TmpDisplayNode->Modified();
  this->TmpNode->Modified();
}


//------------------------------------------------------------------------------
void vtkTractographyVisualizationLogic::CalculateMinMax(
    const char *name,
    vtkFloatArray *min_values,
    vtkFloatArray *max_values,
    double& min,
    double& max) {
  min = FLT_MAX;
  max = 0;
  float line_min;
  float line_max;

  vtkMRMLTractographyVisualizationNode *n =
      this->TractographyVisualizationNode;
  vtkMRMLModelNode *inputModelNode = vtkMRMLModelNode::SafeDownCast(
      this->GetMRMLScene()->GetNodeByID(n->GetInputModelRef()));

  vtkPolyData *pd = this->PolyData;
  if (this->DownSampled)
  {
    pd = inputModelNode->GetPolyData();
  }

  vtkSmartPointer<vtkFloatArray> values = 
      vtkFloatArray::SafeDownCast(pd->GetPointData()->GetArray(name));
  if (!values)
  {
    vtkErrorMacro("The model doesn't have the required values");
  }

  vtkIdList *ptIds = vtkIdList::New();
  for (vtkIdType i = 0; i < pd->GetNumberOfLines(); ++i)
  {
    //if (!(i % 1000)) {
    //  vtkstd::cout << i << vtkstd::endl;
    //}
    pd->GetCellPoints(i, ptIds);
    line_min = FLT_MAX;
    line_max = 0;
    for (vtkIdType j = 0; j < ptIds->GetNumberOfIds(); ++j)
    {
      float value = values->GetValue(ptIds->GetId(j));

      if (value < min)
      {
        min = value;
      }
      else if (value > max)
      {
        max = value;
      }
      
      if (value < line_min)
      {
        line_min = value;
      }
      if (value > line_max)
      {
        line_max = value;
      }
    }

    min_values->InsertNextValue(line_min);
    max_values->InsertNextValue(line_max);
  }
  ptIds->Delete();

  //vtkstd::cout << "Max: " << max << vtkstd::endl << "Min: " << min
  //             << vtkstd::endl;
  //vtkstd::cout << "Value components: " << values->GetNumberOfComponents()
  //             << " Value tuples: " << values->GetNumberOfTuples()
  //             << vtkstd::endl;
  //vtkstd::cout << "Points: "
  //             << pd->GetPoints()->GetNumberOfPoints()
  //             << vtkstd::endl;
}


//------------------------------------------------------------------------------
void vtkTractographyVisualizationLogic::SetValueType()
{
  vtkMRMLTractographyVisualizationNode *n =
      this->TractographyVisualizationNode;

  if (n->GetValueType() == 0) {
    n->SetThresholdMax(this->NormMax + 1.0);
    n->SetThresholdMin(std::max(this->NormMin - 1.0, 0.0));
    n->SetThreshold(std::min(this->NormDefaultThres, n->GetThresholdMax()));
    vtkstd::cout << "set value: " << "norm" << vtkstd::endl;
  } else {
    n->SetThresholdMax(this->FaMax + 1.0);
    n->SetThresholdMin(std::max(this->FaMin - 1.0, 0.0));
    n->SetThreshold(std::min(this->FaDefaultThres, n->GetThresholdMax()));
    vtkstd::cout << "set value: " << "FA" << vtkstd::endl;
  }
}


//------------------------------------------------------------------------------
void vtkTractographyVisualizationLogic::Reduce(bool rec_fa, bool rec_ra,
                                               bool rec_trace, bool rec_avg_fa)
{
  vtkMRMLTractographyVisualizationNode *n =
      this->TractographyVisualizationNode;

  double threshold = n->GetThreshold();

  vtkMRMLModelNode *inputModelNode = vtkMRMLModelNode::SafeDownCast(
      this->GetMRMLScene()->GetNodeByID(n->GetInputModelRef()));

  vtkMRMLModelNode *outputModelNode = vtkMRMLModelNode::SafeDownCast(
      this->GetMRMLScene()->GetNodeByID(n->GetOutputModelRef()));

  vtkSmartPointer<vtkFloatArray> values;
  if (n->GetValueType() == 0) {
    values = vtkFloatArray::SafeDownCast(
        inputModelNode->GetPolyData()->GetPointData()->GetArray("norm"));
  } else {
    values = vtkFloatArray::SafeDownCast(
        inputModelNode->GetPolyData()->GetPointData()->GetArray("FA"));
  }
  if (!values)
  {
    vtkErrorMacro("The model doesn't have 'values'");
    return;
  }

  vtkPolyData *out_data = vtkPolyData::New();
  outputModelNode->SetAndObservePolyData(out_data);
  vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
  vtkSmartPointer<vtkCellArray> lines = vtkSmartPointer<vtkCellArray>::New();

  // Copy the whole polydata with all value arrays but only with the indices we
  // want.
  vtkPolyData *in_data = inputModelNode->GetPolyData();

  // Allocate max amount of cells, will be squeezed afterwards.
  vtkIdType numLines = in_data->GetNumberOfLines();

  // Iterate through all lines.
  //TODO recalculate min max if its downsampled.
  vtkFloatArray *min;
  vtkFloatArray *max;
  if (this->DownSampled) {
    min = vtkFloatArray::New(); 
    max = vtkFloatArray::New(); 

    double tmp1, tmp2;
    if (n->GetValueType() == 0) {
      this->CalculateMinMax("norm", min, max, tmp1, tmp2);
    } else {
      this->CalculateMinMax("FA", min, max, tmp1, tmp2);
    }
  } else {
    if (n->GetValueType() == 0) {
      min = this->MinValues;
      max = this->MaxValues;
    } else {
      min = this->MinFaValues;
      max = this->MaxFaValues;
    }
  }
  vtkIdList *ptIds = vtkIdList::New();
  bool perLine = n->GetRemovalMode();
  std::map<vtkIdType, vtkIdType> pointIdMap;
  vtkIdType pointCounter = 0;
  int lineCounter = 0;
  std::pair<std::map<vtkIdType, vtkIdType>::iterator, bool> res;

  // Create the data arrays
  int num_arrays = in_data->GetPointData()->GetNumberOfArrays();
  std::vector<vtkSmartPointer<vtkFloatArray> > data_arrays(num_arrays);
  for (int a = 0; a < num_arrays; ++a) {
    vtkstd::cout << "array: " << in_data->GetPointData()->GetArrayName(a) << vtkstd::endl;
    //vtkstd::cout << "  components: " << in_data->GetPointData()->GetNumberOfComponents() << vtkstd::endl;
    //vtkstd::cout << "  tuples: " << in_data->GetPointData()->GetNumberOfTuples() << vtkstd::endl;
    data_arrays[a] = vtkFloatArray::New();
    data_arrays[a]->SetNumberOfComponents(
        in_data->GetPointData()->GetArray(a)->GetNumberOfComponents());
    vtkstd::cout << "  components: " << data_arrays[a]->GetNumberOfComponents() << vtkstd::endl;
    data_arrays[a]->SetName(in_data->GetPointData()->GetArrayName(a));
  }

  for (vtkIdType i = 0; i < numLines; ++i)
  {
    in_data->GetCellPoints(i, ptIds);
    if (ptIds->GetNumberOfIds() < 2)
    {
      continue;
    }

    if (perLine)
    {
      if (n->GetValueType() == 0) {
        if (min->GetValue(i) < threshold)
        {
          in_data->GetCellPoints(i, ptIds);

          vtkSmartPointer<vtkLine> line = vtkSmartPointer<vtkLine>::New();
          line->GetPointIds()->SetNumberOfIds(ptIds->GetNumberOfIds());
          for (int a = 0; a < num_arrays; ++a) {
            data_arrays[a]->SetNumberOfTuples(ptIds->GetNumberOfIds());
          }
          for (vtkIdType j = 0; j < ptIds->GetNumberOfIds(); ++j)
          {
            int newId;
            double point[3];
            in_data->GetPoint(ptIds->GetId(j), point);

            res = pointIdMap.insert(std::pair<vtkIdType,
                                    vtkIdType>(ptIds->GetId(j), pointCounter));
            if (!res.second)
            {
              newId = res.first->second;
            }
            else
            {
              points->InsertNextPoint(point[0], point[1], point[2]);
              newId = pointCounter;

              for (int a = 0; a < num_arrays; ++a) {
                data_arrays[a]->SetTuple(newId, ptIds->GetId(j),
                                         in_data->GetPointData()->GetArray(a));
              }

              ++pointCounter;
            }

            // Finally insert point into line.
            line->GetPointIds()->SetId(j, newId);
          }

          lines->InsertNextCell(line);
          ++lineCounter;
        }
      }
    }
    else
    {
      int line_point_counter = 0;

      // Make sure that the line will be at least two points long.
      if (values->GetValue(ptIds->GetId(0)) >= threshold || 
          values->GetValue(ptIds->GetId(1)) >= threshold) 
      {
         continue;
      }

      vtkSmartPointer<vtkLine> line = vtkSmartPointer<vtkLine>::New();
      for (vtkIdType j = 0; j < ptIds->GetNumberOfIds(); ++j)
      {
        if (values->GetValue(ptIds->GetId(j)) < threshold)
        {
          int newId;
          double point[3];
          in_data->GetPoint(ptIds->GetId(j), point);

          res = pointIdMap.insert(std::pair<vtkIdType,
                                  vtkIdType>(ptIds->GetId(j), pointCounter));
          if (!res.second)
          {
            newId = res.first->second;
          }
          else
          {
            points->InsertNextPoint(point[0], point[1], point[2]);
            newId = pointCounter;

            for (int a = 0; a < num_arrays; ++a) {
              data_arrays[a]->InsertTuple(newId, ptIds->GetId(j),
                                          in_data->GetPointData()->GetArray(a));
            }

            ++pointCounter;
          }

          // Finally insert point into line.
          line->GetPointIds()->InsertId(j, newId);
          ++line_point_counter;
        }
        else
        {
          break;
        }
      }

      lines->InsertNextCell(line);
      ++lineCounter;
    }
  }
  //vtkstd::cout << pointCounter << vtkstd::endl;
  ptIds->Delete();

  out_data->SetPoints(points);
  out_data->SetLines(lines);

  for (int a = 0; a < num_arrays; ++a) {
    out_data->GetPointData()->AddArray(data_arrays[a]);
    data_arrays[a]->Delete();
  }

  // Clean up.
  if (this->DownSampled) {
    min->Delete();
    max->Delete();
  }
  inputModelNode->GetModelDisplayNode()->SetVisibility(0);
 
  outputModelNode->SetAndObservePolyData(out_data);
  outputModelNode->SetModifiedSinceRead(1);
 
  vtkMRMLModelDisplayNode *displayNode = outputModelNode->GetModelDisplayNode();
  if (displayNode)
  {
    this->GetMRMLScene()->RemoveNode(displayNode);
  }
 
  displayNode = vtkMRMLModelDisplayNode::New();
  displayNode->SetScene(this->GetMRMLScene());
 
  this->GetMRMLScene()->AddNode(displayNode);
 
  outputModelNode->SetAndObserveDisplayNodeID(displayNode->GetID());
 
  displayNode->SetPolyData(outputModelNode->GetPolyData());
  displayNode->SetVisibility(1);
 
  displayNode->Delete();

  this->TmpDisplayNode->SetVisibility(0);

  //// Calculate Additional statistics.
  //Average FA -- 0.5 *(l2fa(tensor1) + l2fa(tensor2))

  int num_values =
        out_data->GetPointData()->GetArray("norm")->GetNumberOfTuples();
  if (rec_fa) {
    if (!out_data->GetPointData()->GetArray("FA")) {
      vtkFloatArray *data = vtkFloatArray::SafeDownCast(
          out_data->GetPointData()->GetArray("state"));
      if (data) {
        // Calculate FA
        int comp = data->GetNumberOfComponents();
        double *d = new double[comp];
        vtkSmartPointer<vtkFloatArray> arr = vtkFloatArray::New();
        arr->SetName("FA");
        arr->SetNumberOfComponents(1);
        arr->SetNumberOfValues(num_values);
        for (int i = 0; i < num_values; ++i) {
          data->GetTuple(i, d);
          if (comp == 12 || comp == 6) {
            arr->SetValue(i, 255.0 * this->Fa(d[3], d[4], d[5]));
          } else {
            arr->SetValue(i, 255.0 * this->Fa(d[2], d[3], d[4]));
          }
        }
        delete[] d;
        out_data->GetPointData()->AddArray(arr);
        arr->Delete();
      } else {
        vtkErrorMacro("Needs state to compute FA");
      }
    }
  }
  if (rec_ra) {
    vtkFloatArray *data = vtkFloatArray::SafeDownCast(
        out_data->GetPointData()->GetArray("state"));
    if (data) {
      // Calculate RA
      int comp = data->GetNumberOfComponents();
      double *d = new double[comp];
      vtkSmartPointer<vtkFloatArray> arr = vtkFloatArray::New();
      arr->SetName("RA");
      arr->SetNumberOfComponents(1);
      arr->SetNumberOfValues(num_values);
      for (int i = 0; i < num_values; ++i) {
        data->GetTuple(i, d);
        if (comp == 12 || comp == 6) {
          arr->SetValue(i, this->Ra(d[3], d[4], d[5]));
        } else {
          arr->SetValue(i, this->Ra(d[2], d[3], d[4]));
        }
      }
      delete[] d;
      out_data->GetPointData()->AddArray(arr);
      arr->Delete();
    } else {
      vtkErrorMacro("Needs state to compute RA");
    }
  }
  if (rec_trace) {
    vtkFloatArray *data = vtkFloatArray::SafeDownCast(
        out_data->GetPointData()->GetArray("state"));
    if (data) {
      // Calculate trace
      int comp = data->GetNumberOfComponents();
      double *d = new double[comp];
      vtkSmartPointer<vtkFloatArray> arr = vtkFloatArray::New();
      arr->SetName("trace");
      arr->SetNumberOfComponents(1);
      arr->SetNumberOfValues(num_values);
      for (int i = 0; i < num_values; ++i) {
        data->GetTuple(i, d);
        if (comp == 12 || comp == 6) {
          arr->SetValue(i, (d[3] + d[4] + d[5]) / 3.0);
        } else {
          arr->SetValue(i, (d[2] + d[3] + d[4]) / 3.0);
        }
      }
      delete[] d;
      out_data->GetPointData()->AddArray(arr);
      arr->Delete();
    } else {
      vtkErrorMacro("Needs state to compute trace");
    }
  }
  if (rec_avg_fa) {
    vtkFloatArray *data = vtkFloatArray::SafeDownCast(
        out_data->GetPointData()->GetArray("state"));
    if (data) {
      // Calculate trace
      int comp = data->GetNumberOfComponents();
      if (comp != 12 && comp != 10) {
        vtkErrorMacro("Average FA can only be computed if we have more than "
                      "one tensor");
      } else {
        double *d = new double[comp];
        vtkSmartPointer<vtkFloatArray> arr = vtkFloatArray::New();
        arr->SetName("trace");
        arr->SetNumberOfComponents(1);
        arr->SetNumberOfValues(num_values);
        for (int i = 0; i < num_values; ++i) {
          data->GetTuple(i, d);
          if (comp == 12) {
            arr->SetValue(i, 0.5 * (this->Fa(d[3], d[4], d[5]) +
                                    this->Fa(d[9], d[10], d[11])));
          } else {
            arr->SetValue(i, 0.5 * (this->Fa(d[2], d[3], d[4]) +
                                    this->Fa(d[7], d[8], d[9])));
          }
        }
        delete[] d;
        out_data->GetPointData()->AddArray(arr);
        arr->Delete();
      }
    } else {
      vtkErrorMacro("Needs state to compute average FA");
    }
  }

  out_data->Delete();
}


//------------------------------------------------------------------------------
double vtkTractographyVisualizationLogic::Fa(double d1, double d2, double d3)
{
  if (d2 == d3) {
    return fabs(d1 - d2) / sqrt(d1 * d1 + 2 * d2 * d2)                      ;
  } else {
    double s = (d1 + d2 + d3) / 3.0;
    return sqrt(.5 * ((d1 - s) * (d1 - s) +
                      (d2 - s) * (d2 - s) +
                      (d3 - s) * (d3 - s)) /
                (d1 * d1 + d2 * d2 + d3 * d3));
  }
}


//------------------------------------------------------------------------------
double vtkTractographyVisualizationLogic::Ra(double d1, double d2, double d3)
{
  return sqrt(.5 * ((d1 - d2) * (d1 - d2) +
                    (d1 - d3) * (d1 - d3) +
                    (d2 - d3) * (d2 - d3)) / (d1 + d2 + d3));
}
