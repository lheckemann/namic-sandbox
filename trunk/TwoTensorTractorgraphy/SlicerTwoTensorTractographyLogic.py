import sys
sys.path.append('/media/Buffalo_Ext3/Programming/Slicer/Slicer3-lib/teem-wrap/')
import teem
import ctypes
import numpy
import math as M
import time
import os.path
import pp
import processing
# The Process is an abstraction that sets up another (Python) process, provides it code to run and a way for the parent application to control execution. 
# Queue objects are a thread/process safe, FIFO data structure. They can store any pickle-able Python object and are extremely useful for sharing data between processes
from processing import Process, Queue

from Slicer import slicer

from SlicerTwoTensorTractographyContainer import SlicerTwoTensorTractographyContainer

### pseudo logic class
class SlicerTwoTensorTractographyLogic(object):

        def __init__(self,mainGUIClass):

                self._helper = mainGUIClass.GetHelper()


        def ExecuteTractography(self,inVolumeNode, outFibersNode, inROINode, fiducialNode, tensorType, stoppingMode, anisotropyThreshold, stoppingCurvature, integrationStepLength, minimumLength, roiLabel, numberOfSeedpointsPerVoxel, fiducialRegionSize, fiducialStepSize, fiducial):


                self._helper.debug("Starting execution of fiducial seeding..")

                if not inVolumeNode or not outFibersNode or not fiducialNode: #or not targetSeedsNode:
                        self._helper.debug(inVolumeNode)
                        self._helper.debug(outFibersNode)
#                       self._helper.debug(lowerThreshold)         
#                        self._helper.debug(higherThreshold)  
                        self._helper.debug(fiducialNode)  
#                       self._helper.debug(targetSeedsNode)         
                        slicer.Application.ErrorMessage("Not enough information!!! Aborting fiducial seeding..\n")
                        return
                else:
                        tract = SlicerTwoTensorTractographyContainer(inVolumeNode, outFibersNode,  inROINode, fiducialNode, tensorType, stoppingMode, anisotropyThreshold, stoppingCurvature, integrationStepLength, minimumLength, roiLabel, numberOfSeedpointsPerVoxel, fiducialRegionSize, fiducialStepSize, fiducial)

                        t1=time.time() 

                        #seedPoint=""

                #       Set up the slicer environment   
                        scene = slicer.MRMLScene
        
                #       Get the input volume node from the MRML tree    
                #        inputDWI = scene.GetNodeByID(self._inVolumeNode)
                #       inputDWI = self._inVolumeNode
                        if tract.fiducial == True:
                #               fiducialList = scene.CreateNodeByClass("vtkMRMLFiducialListNode")
                                fiducialList = tract.fiducialNode
                                inROINode = ""
                        else:
                #               Get input ROI from the MRML tree
                #               inROINode = scene.GetNodeByID(self._inROINode)
                                inROINode = tract.inROINode
                #       Set output node
                #       outputFiberBundleNode = scene.GetNodeByID(self._outFibersNode)
                        outputFiberBundleNode = tract.outFibersNode

                        if not tract.inVolumeNode:
                                print "no input volume"
                        
                        if not inROINode:
                                print "no ROI"

                        if not tract.inVolumeNode or not (inROINode or fiducialList):
                                print "No input ROI or fiducial list specified!"
                                return



                ########################################## Create DWI ###########################################

                        n = tract.CreateDWINrrd()

                        # check if the dwi volume consists of just one or multiple BO images
                        numberOfB0Values = tract.ComputeNumberOfBaselineImages()

                        if numberOfB0Values > 1:
                                # change to single B0 in case of multiple B0 values and create nrrd for input dwi
                                dwiNrrd = tract.UpdateDwiPassGradientsToNrrd(n, numberOfB0Values)

                        else: 
                                # create nrrd for input dwi
                                dwiNrrd = tract.PassGradientsToNrrd(n)


                ########################################## Seeding ###############################################

                        # if fiducial seeding is used...
                        if tract.fiducial:
                                # ... create numpy array containing the fiducial points
                                seedPoints = tract.CreateFiducialList(fiducialList, tract.fiducialRegionSize, tract.fiducialStepSize)
                        # if ROI seeding is used...
                        else:
                                # ... create numpy array containing the ROI seeding points
                                seedPoints = tract.CreateROI(inROINode, tract.roiLabel, tract.numberOfSeedpointsPerVoxel)

                        if seedPoints.shape[0] == 0:
                                print "No seed data!"
                                return

#                       print "seedPoints: ", seedPoints

                ########################################## Tractography ############################################

                        # get number of CPUs
                        nCpu =  processing.cpuCount()

                        queues = []
                        jobs = []

                        for i in range(0,nCpu):
                                print "i before append: ", i
                                queues.append(Queue())
                                # Queues are especially useful when passed as a parameter to a Process' target function to enable the Process to consume (or return) data.
                                seedPointsCPU = tract.CalculateRegion(seedPoints, i)

                                # instantiate a Process object with the function it should run; processes are spawned by creating a Process object
                                jobs.append(Process(target = tract.StreamlineTractography,args=(dwiNrrd, seedPointsCPU, i, queues[i], tract.tensorType, tract.anisotropy, tract.anisotropyThreshold, tract.stoppingCurvature, tract.integrationStepLength, tract.minimumLength)))               
                                print "i after append job: ", i

                        for i in range(0,nCpu):
                                print "start: ", i
                                # start the process's activity
                                jobs[i].start()
                                print "started: ", i

                        tblocks = []
                        for i in range(0,nCpu):
                                # remove and return an item from the queue;  block if necessary until an item is available
                                # tblocks[i][0] will contain the points array of process i, tblocks[i][1] the cells array and tblocks[i][2] the numVerts array 
                                tblocks.append(queues[i].get())


                        for i in range(0,nCpu):
                                # block the calling thread until the process whose join() method is called terminates
                                jobs[i].join()
                                print "join"
        
                
                        indexC = 0      
                        endL = 0
                        # in case there is more than one process, update the cells array for processes 1 to nCpu, since the cells and points arrays of these processes will be appended to the cells and points array of process 0
                        for i in range(1,nCpu):
                                print "tblocks[i][1] before change: ", tblocks[i][1]
                                print "len(tblocks[i][2]): ",len(tblocks[i][2])
                                # number of cells - number of fibers = last position in cells array; at location endL + 1 the next cell can be inserted
                                endL = endL + tblocks[i-1][1].shape[0] - len(tblocks[i-1][2])
                                print "endL: ",endL
                                indexC=0
                                for k in range(0,len(tblocks[i][2])):
                                        indexC = indexC+1
                                        print "tblocks[i][1][indexC][0]+1: ", tblocks[i][1][indexC-1][0]+1
                                        for j in range(indexC,indexC + tblocks[i][1][indexC-1][0]):
                                                indexC = indexC + 1
                                                print "tblocks[i][1][j][0] old: ", tblocks[i][1][j][0]
                                                # add the last position of the cell array of the preceding cell array to each element of the cell array of process i
                                                tblocks[i][1][j][0] = endL + tblocks[i][1][j][0]
                                                print "tblocks[i][1][j][0] new: ", tblocks[i][1][j][0]
        

                        pts = tblocks[0][0]
                        cells = tblocks[0][1]
                        numVerts = tblocks[0][2]
                        # in case there is more than one process, append the cells, points and numVerts array of processes 1 to nCpu to the cells, points and numVerts array of process 0
                        for i in range(1,nCpu):
                                pts = numpy.append(pts,tblocks[i][0], axis=0)
                                cells = numpy.append(cells,tblocks[i][1], axis=0)
                                numVerts = numpy.append(numVerts,tblocks[i][2],axis=0)

                        print "pts append: ", pts
                        print "cells append: ", cells
                        print "numVerts append: ", numVerts

                        # the length of the numVerts array returns the number of fibers
                        numFibs = len(numVerts)
                        print "numFibs: ", numFibs

        

                ############################# Save results of tractography in vtkPolyData structure ##############################


                        # setup the output node; Observe the bundle node which observes the polydata, in order to update the polydata when it changes
                        outputFiberBundleNode.SetAndObservePolyData(slicer.vtkPolyData())
                        outputPolyData = outputFiberBundleNode.GetPolyData()

                        # we have to create a vtkFloatArray for the point data and a vtkIdTypeArray for the cell data, since vtkPolyData requires the points and cells arrays to be in vtk format
                        ptsVTK = slicer.vtkFloatArray()
                        cellsVTK = slicer.vtkIdTypeArray()
        
                        # slicer python array to vtkFloatArray
                        # get points data from numpy array "pts"
                        ptsVTK.FromArray(pts,2)
                        # get cells data from numpy array "cells"
                        cellsVTK.FromArray(cells,2)
                        # create vtkPoints() and vtkCellArray()
                        ptsArr = slicer.vtkPoints()
                        cellsArr = slicer.vtkCellArray()

                        # set points and cells data of both vtkPoints() and vtkCellArray()
                        ptsArr.SetData(ptsVTK)
                        cellsArr.SetCells(numFibs,cellsVTK)

                        print "pts.ToArray() new: ", ptsArr.GetData().ToArray()
                        print "cells.ToArray() new: ", cellsArr.GetData().ToArray()

                        # set points in vtkPolyData structure
                        outputPolyData.SetPoints(ptsArr)
                #       print outputPolyData.GetPoints()
                        # set lines in vtkPolyData structure of the outputFiberBundleNode
                        outputPolyData.SetLines(cellsArr)
                #       print outputPolyData.GetLines() of the outputFiberBundleNode
                        print outputPolyData.GetPoints().GetData().ToArray()
                #       print outputPolyData.GetLines().GetData().ToArray().squeeze()
                        # update polydata of the outputFiberBundleNode
                        outputPolyData.Update()

#                       outputFiberBundleNode.Modified()

                ################################## Get Tensors used for tracking #################################################
                        pdf, ctx, pvl = tract.SetupGage(tract.inVolumeNode,dwiNrrd,outputFiberBundleNode)
                        # create DTI volumes of both tensor data?
                        createDTIs = False

                        queues=[]
                        jobs=[]
        
                        # distribute the number of fibers to the number of processors and attach the jobs to the queue
                        for i in range(0,nCpu):
                                queues.append(Queue())
                                jobs.append(Process(target = tract.GetTensorsUsedForTractography,args=(pdf, ctx, pvl, outputFiberBundleNode, numVerts, i , tract.tensorType, queues[i])))

                        for i in range(0,nCpu):
                                # start the process's activity
                                jobs[i].start()
        
                        tblocks2 = []
                        for i in range(0,nCpu):
                                # remove and return an item from the queue;  block if necessary until an item is available
                                # tblocks[i][0] will contain the tensor array of process i 
                                tblocks2.append(queues[i].get())

                        for i in range(0,nCpu):
                                # block the calling thread until the process whose join() method is called terminates
                                jobs[i].join()

                        numberOfPoints = outputPolyData.GetPoints().GetNumberOfPoints()
                        # create vtk array for tensors, as vtkPolyData requires a vtk array for assigning tensors
                        tensorsVtk = slicer.vtkDoubleArray()
                        # set number of components and tuples of the vtkDoubleArray containing the tensors
                        tensorsVtk.SetNumberOfComponents(9)
                        tensorsVtk.SetNumberOfTuples(numberOfPoints)


                        # join the tensor information from the different processes
                        tens = tblocks2[0][0]
                        #in case there is more than one process, append the tensors arrays of processes 1 to nCpu to the tensors array of process 0
                        for i in range(1,nCpu):
                                tens  = numpy.append(tens,tblocks2[i][0], axis=0)

                #       insert tensors into vtk tensor array
                #       for i in range(0,numberOfPoints):
                #               tensorsVtk.InsertTuple9(i,tens[i][0],tens[i][1],tens[i][2],tens[i][3],tens[i][4],tens[i][5],tens[i][6],tens[i][7],tens[i][8])

                        tensorsVtk.FromArray(tens,2)

                        # assign tensors to vtkPolyData structure
                        outputPolyData.GetPointData().SetTensors(tensorsVtk)
                        # set scalars of vtkPolyData structure to None (since they will be computed automatically by vtk)
                        outputPolyData.GetPointData().SetScalars(None)
                #       print outputFiberBundleNode.GetPolyData().GetPointData().GetTensors().ToArray()

                        outputFiberBundleLineDisplayNode = slicer.vtkMRMLFiberBundleLineDisplayNode()
                        outputFiberBundleTubeDisplayNode = slicer.vtkMRMLFiberBundleTubeDisplayNode()
                        outputFiberBundleGlyphDisplayNode = slicer.vtkMRMLFiberBundleGlyphDisplayNode()

                        # create display properties node for line, tube and glyph
                        lineDTDPN = slicer.vtkMRMLDiffusionTensorDisplayPropertiesNode()
                        tubeDTDPN = slicer.vtkMRMLDiffusionTensorDisplayPropertiesNode()
                        glyphDTDPN = slicer.vtkMRMLDiffusionTensorDisplayPropertiesNode()

#                       outputFiberBundleLineDisplayNode.SetScene(scene)
#                       outputFiberBundleTubeDisplayNode.SetScene(scene)
#                       outputFiberBundleGlyphDisplayNode.SetScene(scene)

                        # set visibility of fiber bundle tube display node on
                        outputFiberBundleLineDisplayNode.SetVisibility(0)
                        outputFiberBundleTubeDisplayNode.SetVisibility(1)
                        outputFiberBundleGlyphDisplayNode.SetVisibility(0)

                        # set auto scalar range off
                        outputFiberBundleLineDisplayNode.SetAutoScalarRange(0)
                        outputFiberBundleTubeDisplayNode.SetAutoScalarRange(0)
                        outputFiberBundleGlyphDisplayNode.SetAutoScalarRange(0)

                        # set scalar range
                        outputFiberBundleLineDisplayNode.SetScalarRange(0,1)
                        outputFiberBundleTubeDisplayNode.SetScalarRange(0,1)
                        outputFiberBundleGlyphDisplayNode.SetScalarRange(0,1)

#                       mapper = slicer.vtkPolyDataMapper()
#                       mapper.SetInput(outputPolyData)
#                       mapper.SetScalarRange(0, 1)

                        scene.AddNode(outputFiberBundleLineDisplayNode)
                        scene.AddNode(outputFiberBundleTubeDisplayNode)
                        scene.AddNode(outputFiberBundleGlyphDisplayNode)

#                       print "scalar range: ", outputFiberBundleTubeDisplayNode.GetScalarRange()

                        # Add display properties node to MRML scene and set diffusion tensor display MRML object for fiber line, tube and glyph
                        scene.AddNode(lineDTDPN)
                        outputFiberBundleLineDisplayNode.SetAndObserveDiffusionTensorDisplayPropertiesNodeID(lineDTDPN.GetID())
                        scene.AddNode(tubeDTDPN)
                        outputFiberBundleTubeDisplayNode.SetAndObserveDiffusionTensorDisplayPropertiesNodeID(tubeDTDPN.GetID())
                        scene.AddNode(glyphDTDPN)
                        outputFiberBundleGlyphDisplayNode.SetAndObserveDiffusionTensorDisplayPropertiesNodeID(glyphDTDPN.GetID())

                        # set color for fiber bundle display nodes
                        outputFiberBundleLineDisplayNode.SetAndObserveColorNodeID("vtkMRMLColorTableNodeRainbow")
                        outputFiberBundleTubeDisplayNode.SetAndObserveColorNodeID("vtkMRMLColorTableNodeRainbow")
                        outputFiberBundleGlyphDisplayNode.SetAndObserveColorNodeID("vtkMRMLColorTableNodeRainbow")

                        outputFiberBundleNode.SetAndObserveDisplayNodeID(outputFiberBundleLineDisplayNode.GetID())
                        outputFiberBundleNode.AddAndObserveDisplayNodeID(outputFiberBundleTubeDisplayNode.GetID())
                        outputFiberBundleNode.AddAndObserveDisplayNodeID(outputFiberBundleGlyphDisplayNode.GetID())

                        outputFiberBundleLineDisplayNode.SetPolyData(outputFiberBundleNode.GetPolyData())
                        outputFiberBundleTubeDisplayNode.SetPolyData(outputFiberBundleNode.GetPolyData())
                        outputFiberBundleGlyphDisplayNode.SetPolyData(outputFiberBundleNode.GetPolyData())

#                       print "scalar range: ", outputFiberBundleTubeDisplayNode.GetScalarRange()


                        if numberOfB0Values == 1:
                                # frees the nrrd; does nothing with the array data inside
                                E = teem.nrrdNix(dwiNrrd)
                                if (E):
                                        err = teem.biffGetDone("nrrd")
                                        print err
                        else:
                                # blows away the nrrd and everything inside
                                E = teem.nrrdNuke(dwiNrrd)
                                if (E):
                                        err = teem.biffGetDone("nrrd")
                                        print err
                #       E = teem.limnPolyDataNix(fiberPld)
                #       if (E):
                #               err = teem.biffGetDone("limn")
                #               print err


                #       Computation time
                        print "Total Tractography computation time : ", time.time()-t1, ' s'


        
                ##################################### 7) Save parameter settings ####################################

                #       parameterSettings= "tensorType: ", tensorType, "\n", "anisotropy: ", anisotropy, "\n" , "anisotropyThreshold: ", anisotropyThreshold, "\n", "stoppingCurvature: ", stoppingCurvature, "\n", "integrationStepLength: ", integrationStepLength, "\n", "steps: ", steps, "\n", "confidence: ", confidence, "\n", "fraction: ", fraction, "\n", "minimumLength: ", minimumLength, "\n", "maximumLength: ", maximumLength, "\n", "roiLabel: ", roiLabel, "\n",  "numberOfSeedpointsPerVoxel: ", numberOfSeedpointsPerVoxel, "\n", "fiducial: ", fiducial, "\n"

                #       E = teem.nrrdSave('ParameterSettings.txt', parameterSettings, None)
                #       if (E):
                #               err = teem.biffGetDone("nrrd")
                #               print err


                        return 






