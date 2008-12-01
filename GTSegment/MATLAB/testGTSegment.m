clear all;
close all;
dbstop if error;
dbstop if warning;

dwiNhdrFilePath = '/tmp/GTSegmentTemp/EEJB_vtkMRMLDiffusionWeightedVolumeNodeB.nrrd';
initialTensorsNhdrFilePath = '/tmp/GTSegmentTemp/EEJB_vtkMRMLDiffusionTensorVolumeNodeE.nrrd';
initialMaskNhdrFilePath = '/tmp/GTSegmentTemp/EEJB_vtkMRMLScalarVolumeNodeD.nrrd';
roiNhdrFilePath = '/tmp/GTSegmentTemp/EEJB_vtkMRMLScalarVolumeNodeC.nrrd';
roiSeedVal = '2026';
roiTargetVal = '54';
vtkOutputFile = '/tmp/GTSegmentTemp/EEJB_at.vtk';
fbOutputFile = '/tmp/GTSegmentTemp/EEJB_fb.nrrd';

GeodesicTractographySegmentation( ...
    dwiNhdrFilePath, initialTensorsNhdrFilePath, initialMaskNhdrFilePath, ...
    roiNhdrFilePath, roiSeedVal, roiTargetVal, ...
    vtkOutputFile, fbOutputFile );
