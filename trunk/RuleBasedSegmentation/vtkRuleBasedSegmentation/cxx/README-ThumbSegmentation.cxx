/*** PSEUDOCODE FOR vtkImageBrainThumbSegmentation ***/
//
// Inputs:  vtkImageData inData (3D)
//          vtkIntArray SeedPointsList
// Outputs: vtkImageData outData (3D)
//
// PSEUDOCODE
//
// -Create **vtkImageData inDataSlice (2D)** for holding one slice of inData
// -Create **vtkImageData thumbMaskTemp (2D)** for holding segmentations during the processing of each slice
//
// for sliceIndex = 1:numberOfSlices,
//   -Initialize inDataSlice and thumbMaskTemp
//
//   -Create **vtkImageData costsSlice (2D)** computed from inDataSlice
//
//   for structureIndex = 1:numberOfStructures,
//     -Create lines connecting Centrum Ovale and the 2 Sulci Points and write to thumbMaskTemp
//
//     -Create lines connecting 2 Sulci Points and write to thumbMaskTemp
//       -Initialize Fast Marching
//         -Choose 1 start point and 1 end point from the 2 Sulci Points
//         -Create and Initialize **vtkImageData arrivalTimes (2D)**
//         -Create and Initialize **vtkImageData heap (2D)** (Visited=1; Trial=0; Far=-1)
//
//       while( trialPointsExist ),
//         -Let A be the trial point with the smallest arrival time using the priority queue
//         -Add A to the visited set and remove it from the trial set
//         -Add all neighbors of A not in visited to the trial set. Recompute T values of all trial neighbors to A.
//       end
//
//       -Run gradient descent to connect start point with end point.
//     end
//
//     -Run region filling on the current structure and assign a label (use vtkITKConfidenceConnectedImageFilter)
//     -Write to thumbMaskTemp
//
//     -Write thumbMaskTemp to **outData** which is the final thumb mask
//   end
// end
//
// return outData
