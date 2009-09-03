
import sys
sys.path.append('/media/Buffalo_Ext3/Programming/Slicer/Slicer3-lib/teem-wrap/')
import teem
import ctypes
import numpy
import math as M
import time
import os.path
#import pp
from Slicer import slicer
import processing
# The Process is an abstraction that sets up another (Python) process, provides it code to run and a way for the parent application to control execution. 
# Queue objects are a thread/process safe, FIFO data structure. They can store any pickle-able Python object and are extremely useful for sharing data between processes
from processing import Process, Queue

class SlicerTwoTensorTractographyContainer(object):


###########################################################################################
#init function
###########################################################################################
        def __init__(self,inVolumeNode, outFibersNode, inROINode, fiducialNode, tensorType, anisotropy, anisotropyThreshold, stoppingCurvature, integrationStepLength, minimumLength, roiLabel, numberOfSeedpointsPerVoxel, fiducialRegionSize, fiducialStepSize, fiducial):

                self.inVolumeNode = inVolumeNode
                self.outFibersNode = outFibersNode
                self.inROINode = inROINode
                self.fiducialNode = fiducialNode
                self.tensorType = tensorType
                self.anisotropy = anisotropy
                self.anisotropyThreshold = anisotropyThreshold
                self.stoppingCurvature = stoppingCurvature
                self.integrationStepLength = integrationStepLength
#               self.fraction = fraction
                self.minimumLength = minimumLength
                self.roiLabel = roiLabel
                self.numberOfSeedpointsPerVoxel = numberOfSeedpointsPerVoxel
                self.fiducialRegionSize = fiducialRegionSize
                self.fiducialStepSize = fiducialStepSize
                self.fiducial = fiducial


###########################################################################################
#This function creates a Nrrd struct of TEEM around the DWI image
###########################################################################################
        def CreateDWINrrd(self):
                # image data array 
                dwiData = self.inVolumeNode.GetImageData().ToArray()
                # pointer to image data array
                dwi_p = ctypes.cast(dwiData.__array_interface__['data'][0], ctypes.POINTER(ctypes.c_float))

                n = teem.nrrdNew()

                # get image information
                inputVolumeType = self.inVolumeNode.GetImageData().ToArray().dtype.type
                inputVolumeDim = self.inVolumeNode.GetImageData().ToArray().ndim

                # axes size
                sz = (ctypes.c_size_t * inputVolumeDim)()
                # Get dimensions of axes x,y,z
                dims = self.inVolumeNode.GetImageData().GetDimensions() 
                # Get number of scalar components (for DWI it returns the total number of baseline and diffusion-weighted images)
                DWIComp = self.inVolumeNode.GetImageData().GetNumberOfScalarComponents()
                sz[:] = [DWIComp,dims[0],dims[1],dims[2]]


                # wrap N around the existing DWI data array
                teem.nrrdWrap_nva(n, dwi_p, self.typeCodeToTeemType(inputVolumeType)[2], inputVolumeDim, sz)

                # cast the pointer to the image sample data
                data_out = ctypes.cast(n.contents.data, ctypes.POINTER(self.nrrdToCType(n.contents.type))) 

        #       Test
        #       ni = teem.nrrdIterNew()
        #       teem.nrrdIterSetNrrd(ni, n)
        #       print teem.nrrdIterValue(ni)
        #       print teem.nrrdIterValue(ni)
        #       print teem.nrrdIterValue(ni)
        #       print teem.nrrdIterValue(ni)
        #       print teem.nrrdIterValue(ni)

        #       for i in range (0,500):
        #               print "data_out: ", data_out[i]

        ########################## set other image information ###############################

        #       set space field
                teem.nrrdSpaceSet(n, teem.nrrdSpace3DRightHanded) 

        #       Sets the topological dimension of the data 
        #       n.contents.spaceDim = self.inVolumeNode.GetImageData().GetDataDimension()
        
        #       get/set measurement frame matrix
                mat = slicer.vtkMatrix4x4()
                self.inVolumeNode.GetMeasurementFrameMatrix(mat)
        #       BE careful: columns and rows are interchanged between Teem and Slicer
                m0=[1,0,0]
                m1=[0,1,0]
                m2=[0,0,1]
                for i in range (0,3):
                        m0[i]=mat.GetElement(i,0)
                        m1[i]=mat.GetElement(i,1)
                        m2[i]=mat.GetElement(i,2)

                n.contents.measurementFrame[0][:3] =m0
                n.contents.measurementFrame[1][:3] =m1
                n.contents.measurementFrame[2][:3] =m2
        #       print n.contents.measurementFrame[0][:3]
        #       print n.contents.measurementFrame[1][:3]
        #       print n.contents.measurementFrame[2][:3]
        

                mat2 = slicer.vtkMatrix4x4()
                self.inVolumeNode.GetIJKToRASMatrix(mat2)
        #       print mat2.GetElement(0,0)
        #       print mat2.GetElement(1,0)
        #       print mat2.GetElement(0,1)
        #       print mat2.GetElement(1,2)
                for i in range (1,self.inVolumeNode.GetImageData().GetDimensions().__len__()+1):
                        for j in range (self.inVolumeNode.GetImageData().GetDimensions().__len__()):
                                n.contents.axis[i].spaceDirection[j] = mat2.GetElement(j,i-1)
        #       set space origin
                for k in range (self.inVolumeNode.GetImageData().GetDimensions().__len__()):
                        n.contents.spaceOrigin[:3] = self.inVolumeNode.GetOrigin()

        #       print "axis space direction", n.contents.axis[0].spaceDirection[0]
        #       print "axis space direction", n.contents.axis[0].spaceDirection[1]
        #       print "axis space direction", n.contents.axis[0].spaceDirection[2]      
        #       print "axis space direction", n.contents.axis[1].spaceDirection[0]
        #       print "axis space direction", n.contents.axis[1].spaceDirection[1]
        #       print "axis space direction", n.contents.axis[1].spaceDirection[2]
        #       print "axis space direction", n.contents.axis[2].spaceDirection[0]
        #       print "axis space direction", n.contents.axis[2].spaceDirection[1]
        #       print "axis space direction", n.contents.axis[2].spaceDirection[2]
        #       print "axis space direction", n.contents.axis[3].spaceDirection[0]
        #       print "axis space direction", n.contents.axis[3].spaceDirection[1]
        #       print "axis space direction", n.contents.axis[3].spaceDirection[2]      
        #       print n.contents.spaceOrigin[:3]
        


        #       set axis kind information
                n.contents.axis[0].kind = teem.nrrdKindList
                n.contents.axis[1].kind = teem.nrrdKindSpace
                n.contents.axis[2].kind = teem.nrrdKindSpace
                n.contents.axis[3].kind = teem.nrrdKindSpace
        
                saveNrrdToDisk = 0
                if saveNrrdToDisk ==1:
                        dwiName=self.inVolumeNode.GetName()
                        dwiName[:-5] + ".nhdr"
                        E = teem.nrrdSave(dwiName, n, None)
                        if (E):
                                err = teem.biffGetDone("nrrd")
                                print err 

                return n

###########################################################################################
#This function checks if the DWI volume contains only one baseline images
###########################################################################################     
        def ComputeNumberOfBaselineImages(self):
                numberOfGradients = self.inVolumeNode.GetNumberOfGradients()
                # Change to single B0 in case of multiple B0 values
                # check number of B0 images; create new text with only one B0 image
                numberOfB0Values = 0

                # Check if DWI volume contains more than one B0 image
                for grad in range(numberOfGradients):
                        # "%.4d" forces 4 digits in integer
                        if ((self.inVolumeNode.GetDiffusionGradients().ToArray()[grad][0] == 0) & (self.inVolumeNode.GetDiffusionGradients().ToArray()[grad][1] == 0) & (self.inVolumeNode.GetDiffusionGradients().ToArray()[grad][2] == 0)):
                                numberOfB0Values = numberOfB0Values + 1
                return numberOfB0Values


###########################################################################################
#This function is called if the DWI volume contains more than one B0 image. 
#The function averages all the B0 DWIs and concatenates the result with the 
#remaining DWIs. After, the function passes the gradient information to the DWI Nrrd.
###########################################################################################
        def UpdateDwiPassGradientsToNrrd(self, n, numberOfB0Values):

                # number of diffusion-weighted gradients
                numberOfGradients = self.inVolumeNode.GetNumberOfGradients()    
                
                # create new dwiNrrd which will contain only one baseline image 
                dwiNrrd = teem.nrrdNew()

                # copy image information of the old nrrd for the dwi to the newly created nrrd
                teem.nrrdCopy(dwiNrrd, n)

                inputVolumeType = self.inVolumeNode.GetImageData().ToArray().dtype.type
                inputVolumeDim = self.inVolumeNode.GetImageData().ToArray().ndim
#               DWIComp = self.inVolumeNode.GetImageData().GetNumberOfScalarComponents()
                dims = self.inVolumeNode.GetImageData().GetDimensions() 
                maxIdxB0 = numberOfB0Values- 1
                helpNrrd = teem.nrrdNew()
                helpNrrd2 = teem.nrrdNew()
                # axis sizes
                sz_min = (ctypes.c_size_t * inputVolumeDim)()
                sz_min[:] = [maxIdxB0,0,0,0]
                sz_max = (ctypes.c_size_t * inputVolumeDim)()
                sz_max[:] = [numberOfGradients-1,dims[0]-1,dims[1]-1,dims[2]-1]

                # crop out the B0 images
                # The function crop selects a sub-array from the input, bounded by the coordinates given to "-min" and "-max".
                E = teem.nrrdCrop(helpNrrd, n, sz_min, sz_max)
                if (E):
                        err = teem.biffGetDone("nrrd")
                        print err 

                sz_min2 = (ctypes.c_size_t * inputVolumeDim)()
                sz_max2 = (ctypes.c_size_t * inputVolumeDim)()
                sz_min2[:] = [0,0,0,0]
                sz_max2[:] = [maxIdxB0,dims[0]-1,dims[1]-1,dims[2]-1]

                # crop out the remaining DWI's
                E = teem.nrrdCrop(helpNrrd2, n, sz_min2, sz_max2)
                if (E):
                        err = teem.biffGetDone("nrrd")
                        print err 

                helpNrrd3 = teem.nrrdNew()
                # average all the B0 images; the third parameter indicates the axis to project along; 
                # the fourth parameter indicates the average method (in this case all the values in the scanline are measured with the "mean" measure) 
                # Projecting along an axis removes it from the axis ordering, going from (DWI,X,Y,Z) to (X,Y,Z)
                E = teem.nrrdProject(helpNrrd3, helpNrrd2, 0, 3, self.typeCodeToTeemType(inputVolumeType)[2])
                if (E):
                        err = teem.biffGetDone("nrrd")
                        print err 

                # concatenate the averaged B0 images with the remaining DWIs
                E = teem.nrrdSplice(dwiNrrd,helpNrrd, helpNrrd3, 0, 0)
                if (E):
                        err = teem.biffGetDone("nrrd")
                        print err 


                # set all the key/value pairs
                teem.nrrdKeyValueAdd(dwiNrrd, "modality", "DWMRI")
                # get b value
                bVal = self.inVolumeNode.GetBValues().GetMaxNorm()
                teem.nrrdKeyValueAdd(dwiNrrd, "DWMRI_b-value", str(bVal))
                # number of gradients in Teem: teem.nrrdKeyValueSize

                # set all the gradients; consider that we changed to just one B0 image
                numberOfB0Values = 0
                for grad in range(numberOfGradients):
                        # "%.4d" forces 4 digits in integer
                        if ((self.inVolumeNode.GetDiffusionGradients().ToArray()[grad][0] == 0) & (self.inVolumeNode.GetDiffusionGradients().ToArray()[grad][1] == 0) & (self.inVolumeNode.GetDiffusionGradients().ToArray()[grad][2] == 0)):
                                numberOfB0Values = numberOfB0Values + 1
                                if (numberOfB0Values ==1):
                                        teem.nrrdKeyValueAdd(dwiNrrd, "DWMRI_gradient_" + "%.4d" % (grad), "0 0 0")
                                else:
                                        continue
                        else:
                                teem.nrrdKeyValueAdd(dwiNrrd, "DWMRI_gradient_" + "%.4d" % (grad-numberOfB0Values + 1), str(self.inVolumeNode.GetDiffusionGradients().ToArray()[grad][0]) + " " + str(self.inVolumeNode.GetDiffusionGradients().ToArray()[grad][1]) + " " + str(self.inVolumeNode.GetDiffusionGradients().ToArray()[grad][2]) )

                # updated number of gradients
#               if numberOfB0Values == 0:
#                       n_KeyValueSize = self.inVolumeNode.GetNumberOfGradients()
#               else:
#                       n_KeyValueSize = self.inVolumeNode.GetNumberOfGradients() - numberOfB0Values + 1
#               print "Updated Number Of Gradients: ", n_KeyValueSize


                E = teem.nrrdNuke(helpNrrd)
                if (E):
                        err = teem.biffGetDone("nrrd")
                        print err
                E = teem.nrrdNuke(helpNrrd2)
                if (E):
                        err = teem.biffGetDone("nrrd")
                        print err
                E = teem.nrrdNuke(helpNrrd3)
                if (E):
                        err = teem.biffGetDone("nrrd")
                        print err

                # frees the nrrd; does nothing with the array data inside
                E = teem.nrrdNix(n)
                if (E):
                        err = teem.biffGetDone("nrrd")
                        print "nrrdNix(n): ", err

                return dwiNrrd

###########################################################################################
#This function is called if the DWI volume contains only one B0 image. 
#The function passes the gradient information to the DWI Nrrd.
###########################################################################################
        def PassGradientsToNrrd(self,dwiNrrd):
                numberOfGradients = self.inVolumeNode.GetNumberOfGradients()
#               dwiNrrd = teem.nrrdNew()
#               teem.nrrdCopy(dwiNrrd, n)

                # set all the key/value pairs
                teem.nrrdKeyValueAdd(dwiNrrd, "modality", "DWMRI")
                # get maximum b value
                bVal = self.inVolumeNode.GetBValues().GetMaxNorm()
                teem.nrrdKeyValueAdd(dwiNrrd, "DWMRI_b-value", str(bVal))
                # number of gradients in Teem: teem.nrrdKeyValueSize

                # set all the gradients; consider that we changed to just one B0 image
                numberOfB0Values = 0
                for grad in range(numberOfGradients):
                        teem.nrrdKeyValueAdd(dwiNrrd, "DWMRI_gradient_" + "%.4d" % (grad), str(self.inVolumeNode.GetDiffusionGradients().ToArray()[grad][0]) + " " + str(self.inVolumeNode.GetDiffusionGradients().ToArray()[grad][1]) + " " + str(self.inVolumeNode.GetDiffusionGradients().ToArray()[grad][2]) )

#               n_KeyValueSize = self.inVolumeNode.GetNumberOfGradients() 


                # frees the nrrd; does nothing with the array data inside
#               E = teem.nrrdNix(n)
#               if (E):
#                       err = teem.biffGetDone("nrrd")
#                       print "nrrdNix(n): ", err


                return dwiNrrd


###########################################################################################
#This function is called if fiducial seeding was chosen as seeding method. 
#The function creates a NumPy array containing the seed points.
###########################################################################################
        def CreateFiducialList(self,fiducialList, fiducialRegionSize, fiducialStepSize):


                numFiducialsPerComp = M.floor(fiducialRegionSize/fiducialStepSize)
                numberFiducials = fiducialList.GetNumberOfFiducials() + pow((numFiducialsPerComp+1),3)*fiducialList.GetNumberOfFiducials()
                if numFiducialsPerComp == 0:
                        numberFiducials = numberFiducials-fiducialList.GetNumberOfFiducials()
                fiducialPoints = numpy.zeros((numberFiducials,3), 'float' )
#               worldPointFiducial = numpy.zeros((numberFiducials,3), 'float')
                for i in range(fiducialList.GetNumberOfFiducials()):
                        fiducialPoints[i]=fiducialList.GetNthFiducialXYZ(i)
#                       print "fiducialPoints", fiducialPoints
#                       print "len(fiducialPoints): ", len(fiducialPoints)

                pos = fiducialList.GetNumberOfFiducials()
                for i in range(0,fiducialList.GetNumberOfFiducials()):
                        for x in self.frange(-fiducialRegionSize/2.0,fiducialRegionSize/2.0,fiducialStepSize):
                                for y in self.frange(-fiducialRegionSize/2.0,fiducialRegionSize/2.0,fiducialStepSize):
                                        for z in self.frange(-fiducialRegionSize/2.0,fiducialRegionSize/2.0,fiducialStepSize):
                                                newXYZ=[0,0,0]
                                                newXYZ[0]=fiducialPoints[i][0]+x
                                                newXYZ[1]=fiducialPoints[i][1]+y
                                                newXYZ[2]=fiducialPoints[i][2]+z
                                                fiducialPoints[pos]=[newXYZ[0],newXYZ[1],newXYZ[2]]
                                                pos = pos+1

                return fiducialPoints


###########################################################################################
#This function is called if ROI seeding was chosen as seeding method. 
#The function creates a NumPy array containing the seed points.
###########################################################################################
        def CreateROI(self,inROINode, roiLabel, numberOfSeedpointsPerVoxel):

                # get IJK to RAS Matrix
                transPoint = slicer.vtkMatrix4x4()
                ijk2 = slicer.vtkMatrix4x4()
                self.inVolumeNode.GetIJKToRASMatrix(ijk2)
                transPoint.DeepCopy(ijk2)

                # get vtk seed point array
                roiType = inROINode.GetImageData().ToArray().dtype
                seeding_array = inROINode.GetImageData().ToArray()

                seedPoints = numpy.where( seeding_array == roiLabel )
#               seedPoints = numpy.where( seeding_array >= 0 )
#               order of seed points indices needs to be reversed
                seedPoints=numpy.array([seedPoints[2],seedPoints[1],seedPoints[0]]) 
                seedPoints=numpy.transpose(seedPoints)
#               print seedPoints
#               how many seedpoints ver voxel (seed spacing)    
#               replicate the seeding points by numberOfSeedpointPerVoxel times
                seedPoints = numpy.tile(seedPoints, (numberOfSeedpointsPerVoxel,1))
#               add random values to the replicated seeding points
                seedPoints = seedPoints + numpy.random.rand(seedPoints.shape[0],seedPoints.shape[1]) - 0.5

                for i in range(seedPoints.shape[0]):
                        ijkPoint=[seedPoints[i][0],seedPoints[i][1],seedPoints[i][2],1.0]
                        # transform seed points to world space
                        #worldPoint = transPoint.MultiplyPoint(seedPoints[i][0],seedPoints[i][1],seedPoints[i][2],1.0)
                        #worldPoint = self._parentClass.GetHelper().ConvertCoordinates2RAS(coordinates)
                        worldPoint = self.ConvertCoordinates2RAS(coordinates)
                        # check if ijk point is within tensor volume
#                       if (PointWithinTensorData(seedPoints[i],point2,self.inVolumeNode)):
#                               seedPts.InsertTuple3(numTup,worldPoint2[0],worldPoint2[1],worldPoint2[2])
#                               print "point: ", seedPoints[i], "worldPoint2: ", worldPoint2
#                               numTup = numTup+1
                        seedPoints[i][0] = worldPoint[0]
                        seedPoints[i][1] = worldPoint[1]
                        seedPoints[i][2] = worldPoint[2]

                return seedPoints


###########################################################################################
#This function set ups and runs tractography
###########################################################################################
        def CalculateRegion(self,seedPoints, cpuIdx):

                partitioning = 1

                numSeedPoints = seedPoints.shape[0]

                nCpu =  processing.cpuCount()
                minSeedPoints = cpuIdx*(numSeedPoints/nCpu) + ((cpuIdx<numSeedPoints % nCpu) and cpuIdx>0) + (numSeedPoints%nCpu)*(cpuIdx>=(numSeedPoints%nCpu))
                maxSeedPoints = (cpuIdx+1)*(numSeedPoints/nCpu) + (((cpuIdx+1)<=(numSeedPoints % nCpu)) and cpuIdx>0) + ((numSeedPoints%nCpu)-1)*(cpuIdx>=(numSeedPoints%nCpu))
#               numSeedPointsCpu = (((cpuIdx+1)*numSeedPoints/nCpu) - (cpuIdx*numSeedPoints/nCpu)) + (cpuIdx<numSeedPoints % nCpu)
                numSeedPointsCpu = maxSeedPoints-minSeedPoints+1
                print "numSeedPointsCpu: ", numSeedPointsCpu

                if partitioning == 0:

                        seedPointsCPU = numpy.zeros((numSeedPointsCpu,3), 'float' )
                        seedPointsCPU = seedPoints[minSeedPoints:maxSeedPoints+1][:],2
                else:

                        seedPointsCPU = numpy.zeros((numSeedPointsCpu,3), 'float' )
                        ########## Begin Cyclic Partitioning ##############
                        a=0
                        for j in range(numSeedPoints):
                                if j%nCpu == cpuIdx:
                                        seedPointsCPU[a]=[seedPoints[j][0],seedPoints[j][1],seedPoints[j][2]]
                                        a = a+1
                        ########## End Cyclic Partitioning ################
                print "seedPointsCPU: ", seedPointsCPU

                return seedPointsCPU



###########################################################################################
#This function set ups and runs tractography
###########################################################################################
        def StreamlineTractography(self,dwiNrrd, seedPoints, cpuIdx, queues, tensorType, anisotropy, anisotropyThreshold, stoppingCurvature, integrationStepLength, minimumLength):

                numSeedPoints = seedPoints.shape[0]
                print "numSeedPoints: ", numSeedPoints

                # compute minimal number and maximum number of seed points for process with number cpuIdx
                nCpu =  processing.cpuCount()
#               minSeedPoints = cpuIdx*(numSeedPoints/nCpu) + ((cpuIdx<=numSeedPoints % nCpu) and cpuIdx>0)
#               maxSeedPoints = (cpuIdx+1)*(numSeedPoints/nCpu) + ((cpuIdx+1)<=(numSeedPoints%nCpu))
#               numSeedPointsCpu = (((cpuIdx+1)*numSeedPoints/nCpu) - (cpuIdx*numSeedPoints/nCpu)) + (cpuIdx<numSeedPoints % nCpu)
#               numSeedPointsCpu = maxSeedPoints-minSeedPoints
                szROI = (ctypes.c_size_t * 2)()
                szROI[:] = [3,numSeedPoints]
                # create array which contains the selected seed points
                seedPts2 = slicer.vtkFloatArray()
                # set number of components of array
                seedPts2.SetNumberOfComponents(3)
                # set number of tuples of array
                seedPts2.SetNumberOfTuples(numSeedPoints)
#               print "maxSeedPoints: ", maxSeedPoints
#               print "minSeedPoints: ", minSeedPoints
                print "numSeedPoints: ", numSeedPoints
#               dataP = []
#               dataP = seedPoints[minSeedPoints:maxSeedPoints][:]
                seedPts2.FromArray(seedPoints[0:numSeedPoints][:],2)
#               print "data: ", dataP
                print "seedPts2.ToArray(): ", seedPts2.ToArray()


#               numpyarray = numpy.array( dataP, dtype = numpy.float, copy=True)
#               print "numpyarray: ", numpyarray

#               array = numpy.ascontiguousarray(dataP)

#               ptr = array.ctypes.data
#               dt = array.dtype

#               type_map = {numpy.dtype('byte'): teem.nrrdTypeChar,
#                           numpy.dtype('ubyte'): teem.nrrdTypeUChar,
#                           numpy.dtype('short'): teem.nrrdTypeShort,
#                           numpy.dtype('ushort'): teem.nrrdTypeUShort,
#                           numpy.dtype('int'): teem.nrrdTypeInt,
#                           numpy.dtype('uint'): teem.nrrdTypeUInt,
#                           numpy.dtype('int64'): teem.nrrdTypeLLong,
#                           numpy.dtype('uint64'): teem.nrrdTypeULLong,
#                           numpy.dtype('float32'): teem.nrrdTypeFloat,
#                           numpy.dtype('float64'): teem.nrrdTypeDouble}
#               if dt not in type_map:
#                       raise Exception("I do not understand dtype %s" % dt)

#               dims = list(reversed(array.shape))
#               roi_p = ctypes.cast(numpyarray.ctypes.data_as(ctypes.c_void_p), ctypes.POINTER(ctypes.c_float))

                # get pointer to seed points array
                roi_p = ctypes.cast(seedPts2.ToArray().__array_interface__['data'][0], ctypes.POINTER(ctypes.c_float))

                print "i before nrrdNew: ", cpuIdx

                # create nrrd for seed point area
                roiNrrd = teem.nrrdNew()
                # wrap roiNrrd around seed points array
                E = teem.nrrdWrap_nva(roiNrrd, roi_p, teem.nrrdTypeFloat, 2, szROI)
                if (E):
                        err = teem.biffGetDone("nrrd")
                        print "Error in nrrdWrap Seedpoint array: ", cpuIdx
                        print err
#                       teem.nrrdWrap_nva(roiNrrd[i], array.ctypes.data_as(ctypes.c_void_p), teem.nrrdTypeFloat, 2, szROI)
#                       teem.nrrdWrap_nva(roiNrrd[i], array.ctypes.data_as(ctypes.c_void_p),type_map[dt], len(array.shape), *dims)
        
                print "i before ctypes.cast(roiNrdd...): ", cpuIdx
        #       cast the pointer to the ROI seed point data
                data_out_roi = ctypes.cast(roiNrrd.contents.data, ctypes.POINTER(self.nrrdToCType(roiNrrd.contents.type))) 
#               print "data_out_roi: " 
#               for i in range(0,nCpu):
#                       print data_out_roi[i]
                E = teem.nrrdSave("roi"+ "%i" %cpuIdx+".nhdr", roiNrrd, None)
                if (E):
                        err = teem.biffGetDone("nrrd")
                        print err
#                       E = teem.nrrdSave('seedPoints-1.txt', roiNrrd, None)
#                       if (E):
#                               err = teem.biffGetDone("nrrd")
#                       print err

#               print "roiNrrd[i]: ", roiNrrd[i]
#               print "fiberPld[i]: ", fiberPld[i]

                ni = teem.nrrdIterNew()
                teem.nrrdIterSetNrrd(ni, roiNrrd)
                print cpuIdx, ", ", teem.nrrdIterValue(ni)
                print cpuIdx, ", ", teem.nrrdIterValue(ni)
                print cpuIdx, ", ", teem.nrrdIterValue(ni)
                print cpuIdx, ", ", teem.nrrdIterValue(ni)




                # create the fiber context around the DWI volume 
                tfx = teem.tenFiberContextDwiNew(dwiNrrd, 50, 1, 1,
                                        teem.tenEstimate1MethodLLS,
                                        teem.tenEstimate2MethodPeled)
                if (tfx==None):
                        err = teem.biffGetDone("ten")
                        print "tenFiberContextDwiNew: ", err

                # set the type of tractography; how tractography is done in DWI volumes
                # two-tensor estimation
                if (tensorType=="Two-Tensor"):
                        E = teem.tenFiberTypeSet(tfx, teem.tenDwiFiberType2Evec0)
                        if (E):
                                err = teem.biffGetDone("ten")
                                print err
                # one-tensor estimation
                elif (tensorType=="One-Tensor"):
                        E = teem.tenFiberTypeSet(tfx, teem.tenDwiFiberType1Evec0)
                        if (E):
                                err = teem.biffGetDone("ten")
                                print err

                # set up stopping criteria 
                # (Bass+Pier's fractional anisotropy)/sqrt(2)
                if anisotropy == "FA": 
                        anisoSet = teem.tenAniso_FA
                #  Westin's linear (first version)
                elif anisotropy == "Cl1":
                        anisoSet = teem.tenAniso_Cl1
                # Westin's planar (first version)
                elif anisotropy == "Cp1":
                        anisoSet = teem.tenAniso_Cp1
                # Westin's linear + planar (first version)
                elif anisotropy == "Ca1":
                        anisoSet = teem.tenAniso_Ca1
                # minimum of Cl and Cp (first version)
                elif anisotropy == "Clpmin1":
                        anisoSet = teem.tenAniso_Clpmin1
                # Westin's linear (second version)
                elif anisotropy == "Cl2":
                        anisoSet = teem.tenAniso_Cl2
                # Westin's planar (second version)
                elif anisotropy == "Cp2":
                        anisoSet = teem.tenAniso_Cp2
                # Westin's linear + planar (second version)
                elif anisotropy == "Ca2":
                        anisoSet = teem.tenAniso_Ca2
                # minimum of Cl and Cp (second version)
                elif anisotropy == "Clpmin2":
                        anisoSet = teem.tenAniso_Clpmin2
                # unknwon stopping mode
                else:
                        anisoSet = teem.tenAnisoUnknown

                E = teem.tenFiberStopAnisoSet(tfx, anisoSet, anisotropyThreshold)
                if (E):
                        err = teem.biffGetDone("ten")
                        print err

                # set radius of curvature
                E = teem.tenFiberStopDoubleSet(tfx, teem.tenFiberStopRadius, stoppingCurvature) 
                if (E):
                        err = teem.biffGetDone("ten")
                        print err

                # set maximum fiber length
                # teem.tenFiberStopDoubleSet(tfx, tenFiberStopLength, maximumLength)

                # set minimum fiber length
                E = teem.tenFiberStopDoubleSet(tfx, teem.tenFiberStopMinLength, minimumLength)
                if (E):
                        err = teem.biffGetDone("ten")
                        print err

#               teem.tenFiberStopDoubleSet(tfx, tenFiberStopConfidence, confidence)
#               took too many steps along fiber
#               teem.tenFiberStopUIntSet(tfx, teem.tenFiberStopNumSteps, steps)
#               fibers with total (both halves) small # steps are discarded (more general-purpose than tenFiberStopStub)
#               teem.tenFiberStopUIntSet(tfx, tenFiberStopMinNumSteps, minimumSteps)
 
                fraction =0.1
                E = teem.tenFiberStopDoubleSet(tfx, teem.tenFiberStopFraction, fraction)
                if (E):
                        err = teem.biffGetDone("ten")
                        print err

#               kernel for reconstructing tensor field
#               see documentation: http://teem.sourceforge.net/nrrd/kernels.html
                ksp = teem.nrrdKernelSpecNew()

#               mapping from a string to a NrrdKernel and a parameter vector
#               nrrdKernelBCCubic; The two-parameter family of first-order continuous, 4-sample support, piece-wise cubic splines
#               teem.nrrdKernelSpecParse(ksp, "cubic:2,1,0")
#               teem.nrrdKernelSpecParse(ksp, "cubic:0.333,0.333")
                teem.nrrdKernelSpecParse(ksp, "cubic:2,1,0")
                E = teem.tenFiberKernelSet(tfx, ksp.contents.kernel, ksp.contents.parm)
                if (E):
                        err = teem.biffGetDone("ten")
                        print err

#              set path integration & step size 
#              the different integration styles supported
#              tenFiberIntgUnknown,   /* 0: nobody knows */
#              tenFiberIntgEuler,     /* 1: dumb but fast */
#              tenFiberIntgMidpoint,  /* 2: 2nd order Runge-Kutta */
#              tenFiberIntgRK4,       /* 3: 4rth order Runge-Kutta */
#              tenFiberIntgLast
                E = teem.tenFiberIntgSet(tfx, teem.tenFiberIntgRK4) 
                if (E):
                        err = teem.biffGetDone("ten")
                        print err
#              teem.tenFiberParmSet(tfx, teem.tenFiberParmStepSize, 0.4)
#              integration method for fiber tracking
#              teem.tenFiberIntgSet(tfx, teem.tenFiberIntgEuler)
#              base step size
#              integrationStepLength = 0.5
                E = teem.tenFiberParmSet(tfx, teem.tenFiberParmStepSize, integrationStepLength)
                if (E):
                        err = teem.biffGetDone("ten")
                        print err
                # tractography happens in world-space (0 == false)
                # define seedpoint and output path in worldspace.  Otherwise, if 1: everything is in index space
                # non-zero iff output of fiber should be seeded in and output in index space, instead of default world
                E = teem.tenFiberParmSet(tfx, teem.tenFiberParmUseIndexSpace, 0)
                if (E):
                        err = teem.biffGetDone("ten")
                        print err

#               print tfx.contents.stop 
#               print tfx.contents.minRadius    
#               print tfx.contents.minFraction
#               print tfx.contents.anisoStopType
#               print tfx.contents.useIndexSpace
#               print tfx.contents.verbose
#               print tfx.contents.confThresh
#               print tfx.contents.ten2AnisoStop
#               print tfx.contents.maxNumSteps
#               print tfx.contents.minNumSteps
#               print tfx.contents.stepSize
#               print tfx.contents.minWholeLen

                E = teem.tenFiberVerboseSet(tfx, 2)     
                if (E):
                        err = teem.biffGetDone("ten")
                        print err

                # checks if all information in the tfx tenFiberContext are set; if yes tenFiberUpdate(tfx) returns 0
                E = teem.tenFiberUpdate(tfx)
                if (E):
                        err = teem.biffGetDone("ten")
                        print err

#               tfbs = teem.tenFiberSingleNew()
#               experimental struct for holding results from a multi tracing
#               tenFiberMulti is a container for multiple fibers; tenFiberMulti is basically just a container for an array of tenFiberSingle's
                tfml = teem.tenFiberMultiNew() 

                fiberPld = teem.limnPolyDataNew()

                # does tractography for a list of seedpoints 
                t2 = time.time()
#               print 'Start MultiTrace : ', t2, ' s'

                ni = teem.nrrdIterNew()
                teem.nrrdIterSetNrrd(ni, roiNrrd)
                print ", MultiTrace, cpuIdx: ", teem.nrrdIterValue(ni)
                print ", MultiTrace, cpuIdx: ", teem.nrrdIterValue(ni)
                print ", MultiTrace, cpuIdx: ", teem.nrrdIterValue(ni)
                print ", MultiTrace, cpuIdx: ", teem.nrrdIterValue(ni)

                # does tractography and puts tractography results into tfml
                E = teem.tenFiberMultiTrace(tfx, tfml ,roiNrrd)
                if (E):
                        err = teem.biffGetDone("ten")
                        print err
#               print 'End MultiTrace : ', time.time(), ' s'
                print 'MultiTrace computation time : ', time.time()-t2, ' s'

                # converts tenFiberMulti to polydata
                # puts all the fibers into a single polydata struct, defined in teem/src/limn/limn.h, called limnPolyData
                # The "xyzw" array is all the vertices of *all* fibers, with 4 values per point (x,y,z,w  where w can be ignored); "xyzwNum" is the total 
                # number of vertices;  "primNum" is the number of fibers; you can index "type[i]" and "icnt[i]" with i between 0 and primNum-1;  type[i] will be
                # limnPrimitiveLineStrip, "icnt[i]" will be the number of vertices in line i;  "indx" is the list of *all* vertex indices (indexing into "xyzw") - 
                # these indices identify which vertices belong to each of the lines.
                E = teem.tenFiberMultiPolyData(tfx, fiberPld, tfml)
                if (E):
                        err = teem.biffGetDone("ten")
                        print err

#               THIS SAVES limnPolyData to disk, 
#               save polydata output to Teem's LMPD format 
#               E = teem.limnPolyDataSave("fibers-dwi.lmpd", fiberPld)
#               if (E):
#                       err = teem.biffGetDone("limn")
#                       print err 
#               save individual fibers to txt files 
#               for vi in range(tfml.contents.fiberNum): 
#                       teem.nrrdSave('vert-%d.txt' % (vi), 
#                       tfml.contents.fiber[vi].nvert, None)



                # put the tractography results into numpy arrays and return it to process
                fiberPldCon = fiberPld.contents

                print "Number of fibers: ", fiberPldCon.primNum
#               numFibs = fiberPldCon.primNum
                print "Number of vertices for all fibers: "
                print [fiberPldCon.icnt[i] for i in range(fiberPldCon.primNum)] 

#               print "length of fiber array (tfml.contents.fiber): ", tfml.contents.fiberNum
#               print tfml.contents.fiber.contents
        
                numberOfPoints = sum([fiberPldCon.icnt[i] for i in range(fiberPldCon.primNum)])
                numberOfCells = numberOfPoints + fiberPldCon.primNum
                # create array for fiber points
                pts = numpy.zeros((numberOfPoints,3), 'float')
                # create array for fiber cells
                cells = numpy.zeros((numberOfCells,1), 'int')

#               numVert1 = (ctypes.c_uint * fiberPldCon.primNum)()
                numVert1 = [fiberPldCon.icnt[i] for i in range(fiberPldCon.primNum)] 
#               "primNum" is the number of fibers
                indexP=0
                indexC=0
                for numFib in range (fiberPldCon.primNum):      
                        cells[indexC,] = fiberPldCon.icnt[numFib]
                        indexC = indexC + 1
#                       icnt[i] will be the number of vertices in line i
                        for i in range(fiberPldCon.icnt[numFib]):
                                pts[indexP] = [float(fiberPldCon.xyzw[0+4*indexP]),float(fiberPldCon.xyzw[1+4*indexP]),float(fiberPldCon.xyzw[2+4*(indexP)])]
                                cells[indexC,:] = [indexP]
                                indexP=indexP + 1       
                                indexC = indexC + 1

                print "pts numpy: ", pts
                print "cells2 numpy: ", cells

#               E = teem.nrrdNix(roiNrrd)
#               if (E):
#                       err = teem.biffGetDone("nrrd")
#                       print err
        
                numVert = numpy.zeros((fiberPldCon.primNum,), 'int')
                for i in range(0,fiberPldCon.primNum):
                        print "numVert1[i]: ", numVert1[i]
                        numVert[i]=numVert1[i]
                print "numVert: ", numVert
                print "type numVert: ", type(numVert)

                E = teem.nrrdNix(roiNrrd)
                if (E):
                        err = teem.biffGetDone("nrrd")
                        print err

                E = teem.tenFiberContextNix(tfx)
                if (E):
                        err = teem.biffGetDone("ten")
                        print err
                E = teem.tenFiberMultiNix(tfml)
                if (E):
                        err = teem.biffGetDone("ten")
                        print err

                E = teem.nrrdKernelSpecNix(ksp)
                if (E):
                        err = teem.biffGetDone("nrrd")
                        print err

                E = teem.limnPolyDataNix(fiberPld)
                if (E):
                        err = teem.biffGetDone("limn")
                        print err
                # put items into the queue; block if necessary until a free slot is available
                queues.put([pts, cells, numVert])


###########################################################################################
#This function setups gage for getting the tensor used for fiber tracking
###########################################################################################

        def SetupGage(self,inputDWI,dwiNrrd,outputFiberBundleNode):

                kind = teem.tenDwiGageKindNew()
#               t=ctypes.POINTER(teem.airEnum)
#               what=ctypes.cast("peled", ctypes.POINTER(teem.airEnum))         
#               kind.enm =what
#               kind.name="dwi"
        
                ngradKVP = teem.nrrdNew()
                nbmatKVP = teem.nrrdNew()
                bKVP = ctypes.c_double(0)
                skip = ctypes.cast(None, ctypes.POINTER(ctypes.c_uint)) 
                skipNum = ctypes.c_uint(0)

                if (teem.tenDWMRIKeyValueParse(ngradKVP, nbmatKVP, bKVP, skip, skipNum, dwiNrrd[0])):
                        print teem.biffGetDone("ten")
                        print "trouble parsing DWI info"
                if (skipNum):
                        print "Sorry, can't do DWI skipping in tenDwiGage"
                if (teem.tenDwiGageKindSet(kind, 50, 1, bKVP, 1, ngradKVP, None, teem.tenEstimate1MethodLLS, teem.tenEstimate2MethodPeled, 42)):
                        print teem.biffGetDone("ten")
                        print "trouble parsing DWI info"

                ctx = teem.gageContextNew()
                pvl = teem.gagePerVolumeNew(ctx, dwiNrrd, kind)

                ksp = teem.nrrdKernelSpecNew()
#               teem.nrrdKernelSpecParse(ksp, "cubic:0.333,0.333")
#               teem.nrrdKernelSpecParse(ksp, "cubic:1,0,0.5")
                teem.nrrdKernelSpecParse(ksp, "cubic:2,1,0")
                #0,0.5")
                teem.gageKernelSet(ctx, teem.gageKernel00, ksp.contents.kernel, ksp.contents.parm)
#               teem.nrrdKernelSpecParse(ksp, "cubicd:2,1,0")
                teem.gageKernelSet(ctx, teem.gageKernel11, ksp.contents.kernel, ksp.contents.parm)
                teem.gagePerVolumeAttach(ctx, pvl)

#               twoTensAndErr = teem.gageAnswerPointer(ctx, pvl, teem.tenDwiGage2TensorPeledAndError)
#               twoTensErr = teem.gageAnswerPointer(ctx, pvl, teem.tenDwiGage2TensorPeledError)
#               evecAns = teem.gageAnswerPointer(ctx, pvl, teem.tenGageEvec)
#               FAans = teem.gageAnswerPointer(ctx, pvl, teem.tenGageFA)
#               FADwians = teem.gageAnswerPointer(ctx, pvl, teem.tenDwiGageFA)
#               tenAns = teem.gageAnswerPointer(ctx, pvl, teem.tenGageTensor)
#               b0 = teem.gageAnswerPointer(ctx, pvl, teem.tenDwiGageB0)


                pdf = slicer.vtkTransformPolyDataFilter()
                ijk = slicer.vtkMatrix4x4()
                t = slicer.vtkTransform()
                inputDWI.GetRASToIJKMatrix(ijk)
                t.Identity()
                t.PreMultiply()
                t.SetMatrix(ijk)
                pdf.SetTransform(t)
                pdf.SetInput(outputFiberBundleNode.GetPolyData())
                pdf.Update()


                E = teem.nrrdKernelSpecNix(ksp)
                if (E):
                        err = teem.biffGetDone("nrrd")
                        print err

                teem.nrrdNuke(ngradKVP)
                teem.nrrdNuke(nbmatKVP)

                return pdf, ctx, pvl


###########################################################################################
#This function returns the tensor used for fiber tracking for each fiber vertex of the 
#resulting fiber tracts
###########################################################################################
        def GetTensorsUsedForTractography(self, pdf, ctx, pvl,outputFiberBundleNode, numVert, cpuIdx , tensorType, queues):

                outputPolyData = outputFiberBundleNode.GetPolyData()

                teem.gageQueryItemOn(ctx, pvl, teem.tenDwiGage2TensorPeled)
                teem.gageQueryItemOn(ctx, pvl, teem.tenDwiGageTensorLLS)
#               teem.gageQueryItemOn(ctx, pvl, teem.tenDwiGage2TensorPeledAndError)
#               teem.gageQueryItemOn(ctx, pvl, teem.tenDwiGage2TensorPeledError)
#               teem.gageQueryItemOn(ctx, pvl, teem.tenDwiGageFA)
#               teem.gageQueryItemOn(ctx, pvl, teem.tenGageEvec)
#               teem.gageQueryItemOn(ctx, pvl, teem.tenGageFA)
#               teem.gageQueryItemOn(ctx, pvl, teem.tenGageTensor)
#               teem.gageQueryItemOn(ctx, pvl, teem.tenDwiGageB0)
                # refresh all internal state in the gageContext prior to calling gageProbe()
                E = teem.gageUpdate(ctx)
                if (E):
                        err = teem.biffGetDone("gage")
                        print err

                twoTens = teem.gageAnswerPointer(ctx, pvl, teem.tenDwiGage2TensorPeled)
                oneTens = teem.gageAnswerPointer(ctx, pvl, teem.tenDwiGageTensorLLS)

                nCpu =  processing.cpuCount()
                # total number of fibers
                numFibers = outputPolyData.GetLines().GetNumberOfCells()
#               numberOfPointsProcess  = (cpuIdx+1)*numFibers/nCpu + ((cpuIdx+1)<=numFibers%nCpu)
                minFib = cpuIdx*numFibers/nCpu + ((cpuIdx<=numFibers % nCpu) and cpuIdx>0)
                maxFib = (cpuIdx+1)*numFibers/nCpu + ((cpuIdx+1)<=numFibers%nCpu)
                numberOfPointsCpu = sum([numVert[i] for i in range(minFib,maxFib)])
                print "minFib tensor: ", cpuIdx, minFib
                print "maxFib tensor: ", cpuIdx, maxFib
                print "numberOfPointsCpu: ", numberOfPointsCpu
                print "total number of points: ", outputPolyData.GetPoints().GetNumberOfPoints()
                numberOfCellsCpu = numberOfPointsCpu + maxFib - minFib
                # initialize array which will contain the tensors used for fiber tracking for each fiber vertex
                tensors = numpy.zeros((numberOfPointsCpu,9), 'float')

                numVertDone = sum([numVert[i] for i in range(0,minFib)])
                print "numVertDone: ", numVertDone
                print "minFib: ", minFib, "maxFib: ", maxFib
                indexPt = 0
                indexCell = numVertDone+minFib
                print "indexCell: ", indexCell, " "
#               indexCell = numberOfCellsCpu
                count = 0

                if (tensorType=="Two-Tensor"):
                        numberCells = pdf.GetOutput().GetLines().GetNumberOfCells()
                        fiberArray = pdf.GetOutput().GetPoints().GetData().ToArray().squeeze()
                        cellArray = pdf.GetOutput().GetLines().GetData().ToArray().squeeze()
                        for numFib in range(minFib,maxFib):
                                print "pdf.GetOutput().GetLines().GetData().ToArray().squeeze(): ", pdf.GetOutput().GetLines().GetData().ToArray().squeeze()
                                print "indexCellIf: ", indexCell
                                numPtsinCell = pdf.GetOutput().GetLines().GetData().ToArray().squeeze()[indexCell]
#                               print "numPtsinCell: ", numPtsinCell
                                # we need to increase indexCell by one, since indexCell contains the number of vertices for the respective fiber
                                # recall the form of the cell array: (n,id1,id2,...,idn, n,id1,id2,...,idn, ...) where n is the number of points 
                                # in the cell, and id is a zero-offset index into an associated point list (which is an array of vx-vy-vz 
                                #triplets accessible by (point or cell) id)
                                indexCell = indexCell +1        
                                # get fiber points array
                                polyDataArray = outputPolyData.GetPoints().GetData().ToArray().squeeze()
                                for i in range(numPtsinCell):
                                        # get fiber point index at cell index "indexCell"
                                        indexPt = cellArray[indexCell]
#                                       print "indexPt", indexPt
#                                       fiberPt = outputFiberBundleNode.GetPolyData().GetPoints().GetData().ToArray()[i]
#                                       print outputFiberBundleNode.GetPolyData().GetPoints().GetData().ToArray()[i]
                                        #indexPt = pdf.GetOutput().GetLines().GetData().ToArray
                                        # fiber point in ijk space
                                        fiberPt = fiberArray[indexPt]
                                        # Tangent  has to be computed in RAS (assuming the tensor coordinate also are in RAS)
                                        # fiber point in world space
                                        x1 = polyDataArray[indexPt]
                                        if (i!=numPtsinCell-1):#        print outputPolyData.GetLines().GetData().ToArray().squeeze()
                                                indexPt2 = cellArray[indexCell+1]
                                                x2 = polyDataArray[indexPt2]
                                        else:
                                                indexPt2 = cellArray[indexCell-1]
                                                x2 = polyDataArray[indexPt2]
                                        t = (x2-x1)
                                        length = numpy.linalg.norm(t)
#                                       print "length: ", length
                                        t_norm = t/length
#                                       print "t_norm: ", t_norm

                                        # probe tensors in ijk space
                                        probeRet = teem.gageProbe(ctx, fiberPt[0],fiberPt[1],fiberPt[2])
                                        firstTens = numpy.array([ [twoTens[1],twoTens[2],twoTens[3]],[twoTens[2],twoTens[4],twoTens[5]],[twoTens[3],twoTens[5],twoTens[6] ]])

                                        (eigenvals1, eigenvecs1) = numpy.linalg.eig(firstTens)
#                                       print "eigenvals1: ", eigenvals1
#                                       largestEigenvalPos1 = computeLargestEigenval(eigenvals1)
#                                       eigvec1=eigenvecs1[:,largestEigenvalPos1]
                                        eigvec1=eigenvecs1[:,max(xrange(len(eigenvals1)), key=eigenvals1.__getitem__)]
                                        eigenvec1Norm = eigvec1/numpy.linalg.norm(eigvec1)
#                                       print "largestEigenvalPos1: ", largestEigenvalPos1
                                        secondTens = numpy.array([ [twoTens[8],twoTens[9],twoTens[10]],[twoTens[9],twoTens[11],twoTens[12]],[twoTens[10],twoTens[12],twoTens[13] ]])
                                        (eigenvals2, eigenvecs2) = numpy.linalg.eig(secondTens)
#                                       print "eigenvals2: ",eigenvals2
#                                       largestEigenvalPos2 = computeLargestEigenval(eigenvals2)
#                                       eigvec2=eigenvecs2[:,largestEigenvalPos2]
                                        eigvec2=eigenvecs2[:,max(xrange(len(eigenvals2)), key=eigenvals2.__getitem__)] 
                                        eigenvec2Norm = eigvec2/numpy.linalg.norm(eigvec2)
#                                       print "largestEigenvalPos2: ", largestEigenvalPos2
                                        # inner product to figure out which eigenvector is parallel to the tangent
                                        e1 = numpy.inner(t_norm, eigenvec1Norm)
#                                       print "e1: ", e1
                                        e2 = numpy.inner(t_norm, eigenvec2Norm)
#                                       print "e2: ", e2
                                        val1 = abs(abs(e1)-1)
#                                       print "val1: ", val1
                                        val2 = abs(abs(e2)-1)
#                                       print "val2: ", val2
#                                       print "twoTens: ", twoTens[0:14]
                                        if (min(val1,val2) == val1):            
                                                tensors[count,:] = numpy.array([float(twoTens[1]),float(twoTens[2]),float(twoTens[3]),float(twoTens[2]),float(twoTens[4]),float(twoTens[5]),float(twoTens[3]),float(twoTens[5]),float(twoTens[6])], 'float')
                                        else:           
                                                tensors[count,:] = numpy.array([float(twoTens[8]),float(twoTens[9]),float(twoTens[10]),float(twoTens[9]),float(twoTens[11]),float(twoTens[12]),float(twoTens[10]),float(twoTens[12]),float(twoTens[13])], 'float')

#                                       if createDTIs == True:
#                                               dti1[fiberPt[0],fiberPt[1],fiberPt[2],:] = numpy.array([twoTens[0],twoTens[1],twoTens[2],twoTens[3],twoTens[4],twoTens[5],twoTens[6]])
#                                               dti2[fiberPt[0],fiberPt[1],fiberPt[2],:] = numpy.array([twoTens[7],twoTens[8],twoTens[9],twoTens[10],twoTens[11],twoTens[12],twoTens[13]])
                        #               print "used tensor: ", tensors.GetTuple9(i)
                        #               print "twoTensErr: ", twoTensErr[0:14]
                        #               print "tenDwiAns: ", tenDwiAns[0:7]
                        #               print "FADwians: ", FADwians[0]
                        #               print "b0: ", b0[0]
                        #               print "FAans: ", FAans[0]
                        #               print "evecAns: ",evecAns[0:10]
                        #               print "tenAns: ", tenAns[0:7]
                                        indexCell = indexCell+1
                                        count = count + 1

                elif (tensorType=="One-Tensor"):
                        fiberArray = pdf.GetOutput().GetPoints().GetData().ToArray()
                        cellArray = pdf.GetOutput().GetLines().GetData().ToArray().squeeze()
                        for numFib in range (minFib,maxFib):
                                numPtsinCell = cellArray[indexCell]
#                               print "numPtsinCell: ", numPtsinCell
                                indexCell = indexCell +1        
                                # icnt[i] will be the number of vertices in line i
                                for i in range(numPtsinCell):
#                                       indexPt = pdf.GetOutput().GetLines().GetData().ToArray().squeeze()[indexCell]
                                        # get fiber point index at cell index "indexCell"
                                        indexPt = cellArray[indexCell]
                                        # fiber point in ijk space
                                        fiberPt = fiberArray[indexPt]
#                                       print "fiberPt squeeze: ", pdf.GetOutput().GetPoints().GetData().ToArray().squeeze()[indexPt]
#                                       print "fiberPt without squeeze: ", pdf.GetOutput().GetPoints().GetData().ToArray()[indexPt]
#                                       fiberPt = pdf.GetOutput().GetPoints().GetData().ToArray()[indexPt][0]
                                        # probe tensors in ijk space
                                        probeRet = teem.gageProbe(ctx, fiberPt[0],fiberPt[1],fiberPt[2])        
                                        tensors[count,:] = numpy.array([float(twoTens[1]),float(twoTens[2]),float(twoTens[3]),float(twoTens[2]),float(twoTens[4]),float(twoTens[5]),float(twoTens[3]),float(twoTens[5]),float(twoTens[6])], 'float')
                                        indexCell = indexCell+1
                                        count = count + 1

                # put item into the queue; block if necessary until a free slot is available
                print "tensors shape: ", tensors.shape[0]
                queues.put([tensors])


###########################################################################################
#This function creates a DTI Nrrd
###########################################################################################
        def CreateDTINrrd(self,dwiNrrd, dti, dtiName):
                dtiNrrd = teem.nrrdNew()

                # axes size
                sz_dti = (ctypes.c_size_t * 4)()
                # Get number of scalar components (for tensor it returns 9)
                dim = [0,0,0]
                for i in range(3):
                        dim[i] = dwiNrrd.contents.axis[i+1].size
                sz_dti[:] = [7,dim[0],dim[1],dim[2]]

                dti_p = ctypes.cast(dti.__array_interface__['data'][0], ctypes.POINTER(ctypes.c_float))
                E=teem.nrrdWrap_nva(dtiNrrd, dti_p, teem.nrrdTypeFloat, dwiNrrd.contents.dim, sz_dti)
                if (E):
                        err = teem.biffGetDone("nrrd")
                        print err

                data_out_dti = ctypes.cast(dtiNrrd.contents.data, ctypes.POINTER(self.nrrdToCType(dtiNrrd.contents.type)))
#               for i in range (0,outputPolyData.GetPoints().GetNumberOfPoints()):
#               print "data_out_dti1: ", outputPolyData.GetPoints().GetData().ToArray().squeeze()[i]
#               print "Tensors: ", outputFiberBundleNode.GetPolyData().GetPointData().GetTensors().ToArray().squeeze()[i]


                # set space and space dimension
                teem.nrrdSpaceSet(dtiNrrd, teem.nrrdSpace3DRightHanded) 

                dtiNrrd.contents.measurementFrame[0][:3] = dwiNrrd.contents.measurementFrame[0][:3]
                dtiNrrd.contents.measurementFrame[1][:3] = dwiNrrd.contents.measurementFrame[1][:3]
                dtiNrrd.contents.measurementFrame[2][:3] = dwiNrrd.contents.measurementFrame[2][:3]


                for i in range (1,dwiNrrd.contents.dim+1):
                        for j in range (dwiNrrd.contents.dim):
                                dtiNrrd.contents.axis[i].spaceDirection[j] = dwiNrrd.contents.axis[i].spaceDirection[j]


                # set space origin
                for k in range (dwiNrrd.contents.dim):
                        dtiNrrd.contents.spaceOrigin[:3] = dwiNrrd.contents.spaceOrigin[:3]

                # set axis kind information
                dtiNrrd.contents.axis[0].kind = dwiNrrd.contents.axis[0].kind
                dtiNrrd.contents.axis[1].kind = dwiNrrd.contents.axis[1].kind
                dtiNrrd.contents.axis[2].kind = dwiNrrd.contents.axis[2].kind
                dtiNrrd.contents.axis[3].kind = dwiNrrd.contents.axis[3].kind



#              teem.tenMeasurementFrameReduce(dtiNrrd)

                E = teem.nrrdSave(dtiName, dtiNrrd, None)
                if (E):
                        err = teem.biffGetDone("nrrd")
                        print err 

                E = teem.nrrdNix(dtiNrrd)
                if (E):
                        err = teem.biffGetDone("nrrd")
                        print "nrrdNix(n): ", err

                return


###########################################################################################
#For a given nrrd type, this function returns a tuple with the corresponding ctypes type, 
#array typecode and numpy type
###########################################################################################
        def typeCodeToPythonType(self, typecode ):
            """For a given nrrd type, return a tuple with the corresponding ctypes type, array typecode and numpy type"""
            typeTable = {
                teem.nrrdTypeChar : ( ctypes.c_byte, 'b', numpy.int8 ),
                teem.nrrdTypeUChar     : ( ctypes.c_ubyte, 'B', numpy.uint8 ),
                teem.nrrdTypeShort     : ( ctypes.c_short, 'h', numpy.int16 ),
                teem.nrrdTypeUShort     : ( ctypes.c_ushort, 'H', numpy.uint8 ),
                teem.nrrdTypeInt     : ( ctypes.c_int, 'l' , numpy.int),
                teem.nrrdTypeUInt     : ( ctypes.c_uint, 'L' , numpy.uint),
                teem.nrrdTypeFloat     : ( ctypes.c_float, 'f' , numpy.float ),
                teem.nrrdTypeDouble     : ( ctypes.c_double, 'd' , numpy.double ) }

            # no python array types are available for the following nrrd types:

            #teem.nrrdTypeLLong     : ( ctypes.c_char, 'c' ),
            #teem.nrrdTypeULLong     : ( ctypes.c_char, 'c' ) }

            return typeTable[typecode]


###########################################################################################
#For a given numpy type, this function returns a tuple with the corresponding ctypes type, 
#array typecode and nrrd type
###########################################################################################
        def typeCodeToTeemType(self, typecode ):
            """For a given numpy type, return a tuple with the corresponding ctypes type, array typecode and nrrd type"""
            typeTable = {
                numpy.int8 : ( ctypes.c_byte, 'b', teem.nrrdTypeChar),
                numpy.uint8     : ( ctypes.c_ubyte, 'B', teem.nrrdTypeUChar ),
                numpy.int16     : ( ctypes.c_short, 'h', teem.nrrdTypeShort ),
                numpy.uint16     : ( ctypes.c_ushort, 'H', teem.nrrdTypeUShort ),
                numpy.int     : ( ctypes.c_int, 'l' , teem.nrrdTypeInt ),
                numpy.uint     : ( ctypes.c_uint, 'L' , teem.nrrdTypeUInt ),
                numpy.float     : ( ctypes.c_float, 'f' , teem.nrrdTypeFloat ),
                numpy.float32     : ( ctypes.c_float, 'f' , teem.nrrdTypeFloat ),
                numpy.double     : ( ctypes.c_double, 'd' , teem.nrrdTypeDouble ) }

            # no python array types are available for the following nrrd types:

            #teem.nrrdTypeLLong     : ( ctypes.c_char, 'c' ),
            #teem.nrrdTypeULLong     : ( ctypes.c_char, 'c' ) }

            return typeTable[typecode]


###########################################################################################
#For a given nrrd type, this function returns the corresponding ctypes type
###########################################################################################
        def nrrdToCType(self, ntype ):
            """For a given nrrd type, return the corresponding ctypes type"""
            typeTable = {
                teem.nrrdTypeChar : ctypes.c_byte,
                teem.nrrdTypeUChar : ctypes.c_ubyte,
                teem.nrrdTypeShort : ctypes.c_short,
                teem.nrrdTypeUShort : ctypes.c_ushort,
                teem.nrrdTypeInt : ctypes.c_int,
                teem.nrrdTypeUInt : ctypes.c_uint,
                teem.nrrdTypeLLong : ctypes.c_longlong,
                teem.nrrdTypeULLong : ctypes.c_ulonglong,
                teem.nrrdTypeFloat : ctypes.c_float,
                teem.nrrdTypeDouble : ctypes.c_double
            }
            return typeTable[ntype] 


###########################################################################################
#This function setups an output node.
###########################################################################################
        def setupTheOutputNode(self, outputFiberBundleNode ):
                if ( outputFiberBundleNode.GetPolyData()==[] ):
                        outputFiberBundleNode.SetAndObservePolyData(slicer.vtkPolyData())

                outputPolyData = outputFiberBundleNode.GetPolyData()
                outputPolyData.Update()

                return clusters


###########################################################################################
#This function checks if a point is within a data set
###########################################################################################
        def PointWithinDwiData(self,point, pointw):
                bounds = self.inVolumeNode.GetImageData().GetBounds()

          
                inbounds=1
                if (point[0] < bounds[0]):
                        inbounds = 0
                if (point[0] > bounds[1]): 
                        inbounds = 0
                if (point[1] < bounds[2]): 
                        inbounds = 0
                if (point[1] > bounds[3]): 
                        inbounds = 0
                if (point[2] < bounds[4]):
                        inbounds = 0
                if (point[2] > bounds[5]):
                        inbounds = 0

                if (inbounds ==0):
                        print "point ", pointw, " outside of tensor dataset \n"

                return(inbounds)


        #def frange(limit1, limit2 = None, increment = 1.):
                """
                Range function that accepts floats (and integers).

                Usage:
                frange(-2, 2, 0.1)
                frange(10)
                frange(10, increment = 0.5)

                The returned value is an iterator.  Use list(frange) for a list.
                """

        #  if limit2 is None:
        #    limit2, limit1 = limit1, 0.
        #  else:
        #    limit1 = float(limit1)

        #  count = int(M.ceil((limit2 - limit1)/increment))
        #  return (limit1 + n*increment for n in range(0,count))


###########################################################################################
#This function has the same functionality as Python's built-in function range(), but 
#allows to use ranges with float arguments.
###########################################################################################
        def frange(self, limit1, limit2, step):
                while limit1 <= limit2:
                        yield limit1
                        limit1 += step


###########################################################################################
#This function convertes coordinates from IJK to RAS
###########################################################################################
        def ConvertCoordinates2RAS(self,coordinates):
                # convert to RAS
                ijkPt = [coordinates[0], coordinates[1], coordinates[2], 1]
                # get IJK to RAS Matrix
                node = self._mainGUIClass.GetScriptedModuleNode()
                if node:
                    inVolume = node.GetParameter('InputVolumeRef')
                    if not inVolume:
                        slicer.Application.ErrorMessage("No input volume found\n")
                        return
                    else:
                        transPoint = slicer.vtkMatrix4x4()
                        matrixRAS = slicer.vtkMatrix4x4()
                        inVolume.GetIJKToRASMatrix(matrixRAS)
                        transPoint.DeepCopy(matrixRAS)
                        transPoint.MultiplyPoint(*ijkPt)
                return 

###########################################################################################
#This function convertes coordinates from RAS to IJK
###########################################################################################
        def ConvertCoordinates2IJK(self,coordinates):
                #need RAS
                #rasPt = self.ConvertCoordinates2RAS(coordinates)
                rasPt = [coordinates[0], coordinates[1], coordinates[2], 1]
                #convert to IJK
                node = self._mainGUIClass.GetScriptedModuleNode()
                if node:
                    inVolume = node.GetParameter('InputVolumeRef')
                    if not inVolume:
                        slicer.Application.ErrorMessage("No input volume found\n")
                        return
                    else:
                        transPoint = slicer.vtkMatrix4x4()
                        matrixIJK = slicer.vtkMatrix4x4()
                        inVolume.GetIJKToRASMatrix(matrixIJK)
                        transPoint.DeepCopy(matrixIJK)
                        transPoint.MultiplyPoint(*rasPt)
                        return ijkPt

