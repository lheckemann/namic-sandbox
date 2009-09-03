from Slicer import slicer
from time import strftime

from SlicerTwoTensorTractographyContainer import SlicerTwoTensorTractographyContainer

###
### Helper class for Two Tensor Tractography..
###
class SlicerTwoTensorTractographyGUIHelper(object):


    def __init__(self,mainGUIClass):

        self._mainGUIClass = mainGUIClass
        self._isInteractiveModeActive = 0
        self._interactiveModeHandlerClass = None

        self._renderWindowInteractor = None
        self._redSliceInteractor = None
        self._yellowSliceInteractor = None
        self._greenSliceInteractor = None

    def SetIsInteractiveMode(self,onoff,handlerClass):
        self._isInteractiveModeActive = onoff
        self._interactiveModeHandlerClass = handlerClass

    def GetIsInteractiveMode(self):
        return self._isInteractiveModeActive

    ###
    ### debug prints to stdout (better than print because of flush)
    ###
    def debug(self,msg):

        debugMode = 1

        if debugMode:
            print "[slicerTwoTensorTractography " + strftime("%H:%M:%S") + "] " + str(msg)
            import sys
            sys.stdout.flush()

    ###
    ### registers the interactors passed through from the parent class
    ###
    def RegisterInteractors(self, renderWindowInteractor, redSliceInteractor, yellowSliceInteractor, greenSliceInteractor):

        self._renderWindowInteractor = renderWindowInteractor
        self._redSliceInteractor = redSliceInteractor
        self._yellowSliceInteractor = yellowSliceInteractor
        self._greenSliceInteractor = greenSliceInteractor

    ###
    ### click handlers for the 3D render window
    ###
    def HandleClickInRenderWindow(self):

        if self._isInteractiveModeActive:

            self.debug("Clicked in 3D")

            coordinates = self._renderWindowInteractor.GetLastEventPosition()
            self.debug(coordinates[0])
            self.debug(coordinates[1])


    ###
    ### click handlers for the slices
    ###
    def HandleClickInRedSliceWindow(self):
        self.HandleClickInSliceWindow("Red",self._redSliceInteractor)

    def HandleClickInYellowSliceWindow(self):
        self.HandleClickInSliceWindow("Yellow",self._yellowSliceInteractor)

    def HandleClickInGreenSliceWindow(self):
        self.HandleClickInSliceWindow("Green",self._greenSliceInteractor)

    def HandleClickInSliceWindow(self,which,interactor):

        if self._isInteractiveModeActive:
            
            self.debug("Clicked on " +which + "Slice")

            coordinates = interactor.GetLastEventPosition()

            self.debug(coordinates[0])
            self.debug(coordinates[1])

            self._interactiveModeHandlerClass.HandleClickInSliceWindowWithCoordinates(which, coordinates)


    ###
    ### convert coordinates to RAS
    ###
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

    ###
    ### convert coordinates to IJK
    ###
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
            
    ###
    ### convert RAS to IJK
    ###
    def ConvertRAS2IJK(self,coordinates):

        rasPt = coordinates
        rasPt.append(1)

        #convert to IJK
        node = self._mainGUIClass.GetScriptedModuleNode()
        if node:
            inVolume = node.GetParameter('InputVolumeRef')
            if not inVolume:
                slicer.Application.ErrorMessage("No input volume found\n")
                return
            else:
                matrixIJK = slicer.vtkMatrix4x4()
                inVolume.GetRASToIJKMatrix(matrixIJK)
                #self.debug(matrixIJK)
                ijkPt = matrixIJK.MultiplyPoint(*rasPt)
                return ijkPt

    def SetAndMergeInitVolume(self,resultContainer):

        scene = self._mainGUIClass.GetLogic().GetMRMLScene()

        volumeNode = resultContainer.GetNode()
        threshold = resultContainer.GetThreshold()

        if self._mainGUIClass._outInitVolume == None:

            # no node so far

            self._mainGUIClass._outInitVolume = slicer.vtkMRMLScalarVolumeNode()
            self._mainGUIClass._outInitVolume.SetName("Two-Tensor Tractography Output Volume")
            self._mainGUIClass._outInitVolume.SetScene(scene)
            self._mainGUIClass._outInitVolume.SetAndObserveImageData(slicer.vtkImageData())
            scene.AddNode(self._mainGUIClass._outInitVolume)

            self._mainGUIClass._outInitVolumeLast = slicer.vtkMRMLScalarVolumeNode()
            self._mainGUIClass._outInitVolumeLast.SetName("Two-Tensor Tractography Output Volume (Last Step)")
            self._mainGUIClass._outInitVolumeLast.SetScene(scene)
            scene.AddNode(self._mainGUIClass._outInitVolumeLast)

        matrix = slicer.vtkMatrix4x4()

        # copy current outInitVolume to outInitVolumeLast
        self._mainGUIClass._outInitVolumeLast.SetAndObserveImageData(self._mainGUIClass._outInitVolume.GetImageData())
        self._mainGUIClass._outInitVolume.GetIJKToRASMatrix(matrix)
        self._mainGUIClass._outInitVolumeLast.SetIJKToRASMatrix(matrix)
        self._mainGUIClass._outInitVolumeLast.SetModifiedSinceRead(1)

        volumeNodeData = volumeNode.GetImageData()

        # merge the oldVolume with volumeNode if oldVolume has already content
        if self._mainGUIClass._outInitVolume.GetImageData().GetPointData().GetScalars():
            # oldVolume has already content
            minFilter = slicer.vtkImageMathematics()
            minFilter.SetOperationToMin()
            minFilter.SetInput1(self._mainGUIClass._outInitVolume.GetImageData()) #the old one
            minFilter.SetInput2(volumeNode.GetImageData()) #the new one
            minFilter.Update()
            volumeNodeData.DeepCopy(minFilter.GetOutput())

        # copy new volume to outInitVolume
        volumeNode.GetIJKToRASMatrix(matrix)
        self._mainGUIClass._outInitVolume.SetAndObserveImageData(volumeNodeData)
        self._mainGUIClass._outInitVolume.SetIJKToRASMatrix(matrix)
        self._mainGUIClass._outInitVolume.SetModifiedSinceRead(1)

        outputContainer = SlicerTwoTensorTractographyContainer(self._mainGUIClass._outInitVolume,threshold)

        return outputContainer

    def Undo(self):

        # copy outInitVolumeLast to outInitVolume

        matrix = slicer.vtkMatrix4x4()
        self._mainGUIClass._outInitVolumeLast.GetIJKToRASMatrix(matrix)

        self._mainGUIClass._outInitVolume.SetAndObserveImageData(self._mainGUIClass._outInitVolumeLast.GetImageData())
        self._mainGUIClass._outInitVolume.SetIJKToRASMatrix(matrix)
        self._mainGUIClass._outInitVolume.SetModifiedSinceRead(1)


        outputContainer = SlicerTwoTensorTractographyContainer(self._mainGUIClass._outInitVolume,0.0)

        return outputContainer

    def GenerateModel(self,resultContainer):

        matrix = slicer.vtkMatrix4x4()

        image = resultContainer.GetNode().GetImageData()

        resultContainer.GetNode().GetIJKToRASMatrix(matrix)
        threshold = 0.0

        polyData = slicer.vtkPolyData()

        if image.GetPointData().GetScalars():
            # marching Cubes, only if image has content
            polyData.DeepCopy(self._mainGUIClass.GetMyLogic().MarchingCubes(image,matrix,threshold))

        scene = self._mainGUIClass.GetLogic().GetMRMLScene()
        if self._mainGUIClass._outModel == None:

            # no node so far
            self._mainGUIClass._outModel = slicer.vtkMRMLModelNode()
            self._mainGUIClass._outModel.SetName("Two-Tensor Tractography Output Model")
            self._mainGUIClass._outModel.SetAndObservePolyData(slicer.vtkPolyData())
            self._mainGUIClass._outModel.SetScene(scene)
            scene.AddNode(self._mainGUIClass._outModel)

            self._mainGUIClass._outModelDisplay = slicer.vtkMRMLModelDisplayNode()
            self._mainGUIClass._outModelDisplay.SetColor(0.8, 0.0, 0.0)
            self._mainGUIClass._outModelDisplay.SetPolyData(slicer.vtkPolyData())
            self._mainGUIClass._outModelDisplay.SetVisibility(1)
            self._mainGUIClass._outModelDisplay.SetSliceIntersectionVisibility(1)

        self._mainGUIClass._outModel.SetAndObservePolyData(polyData)
        self._mainGUIClass._outModel.SetModifiedSinceRead(1)
        self._mainGUIClass._outModelDisplay.SetPolyData(self._mainGUIClass._outModel.GetPolyData())
        scene.AddNode(self._mainGUIClass._outModelDisplay)
        
        self._mainGUIClass._outModel.SetAndObserveDisplayNodeID(self._mainGUIClass._outModelDisplay.GetID())



