##############################################################################
# \file  GLISTR.py
# \brief Scripted Python module of GLISTR for Slicer4.
#
# Visit https://www.rad.upenn.edu/sbia/software/index.html#glistr for
# information on how to obtain a copy of the GLISTR sources.
#
# Copyright (c) 2011 University of Pennsylvania. All rights reserved.
# See COPYING file or https://www.rad.upenn.edu/sbia/software/license.html.
#
# Contact: sbia-software at uphs.upenn.edu
##############################################################################

# ============================================================================
# imports
# ============================================================================

from __main__ import slicer, qt, vtk
import os, sys, subprocess

# ============================================================================
# helpers
# ============================================================================

# ****************************************************************************
## Execute command.
#
# \todo
def execCmd(cmdArray):
    cmd = [str (i) for i in cmdArray]
    print ' '.join (cmd)
    p = subprocess.Popen (cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    (out, err) = p.communicate ()
    sys.stdout.write (out)
    sys.stderr.write (err)
    return p.returncode

# ============================================================================
# class: GLISTR - the module description
# ============================================================================

## Provides the module description for Slicer
class GLISTR:
    
    # ************************************************************************
    ## Constructor.
    def __init__ (self, parent):
        parent.title               = "GLISTR"
        parent.category            = "Segmentation"
        parent.contributor         = "Ali Gooya, Andreas Schuh, Christos Davatzikos, Kilian Pohl"
        parent.helpText            = \
"""
The Glioma Image Segmentation and Registration (GLISTR) includes the following
steps, some of which are optional.<br />

<h4>Affine Registration of Patient Scans</h4>
<p>
  At first, the patient scans acquired with different MRI protocols have to be
  registered affine to the used atlas. Further, the brain mask of the atlas is
  applied to mask out the skull and background from the patient scans.
</p>

<h4>Manual Annotation of Tissues and Seed Point</h4>
<p>
  Open the Annotation module and set fiducial points inside the following tissues:
  TUMOR, NCR, VEINS, CSF, GM, WM, EDEMA, and BG. The intensity values at these
  fiducial points in the affine registered volumes are used as initial mean values
  for the annotated tissue types.
</p>
<p>
  <b>Note:</b> Some patients do not have both types of tumors. In those cases,
  annotate only TUMOR and not NCR. Similarly, if no edema is observed, it does not
  need to be annotated either in those cases.
</p>

<h4>Joint Segmentation &amp; Registration</h4>
<p>
  Once the different tissue types are annotated, the input text file of
  initial mean values is generated. Using these mean values and the other
  parameters, the joint segmentation &amp; registration is performed.
  One of the results of this main step are the posterior maps, one for each
  tissue type.
</p>

<h4>Transformation of Posteriors into Patient Space</h4>
<p>
  Optionally, the obtained posterior maps can be transformed back into the
  patient space given that the pre-processing step has been executed before.
  In particular, the inverse of the affine transformation applied in the
  pre-processing step is applied to the posterior maps in order to transform
  them into the original patient space.
</p>

<h4>Generation of Segmentation from Posteriors</h4>
<p>
  This final step generates a segmentation, i.e., a label image from the
  posterior maps obtained by the joint segmentation &amp; registration.
<p>
"""
        parent.acknowledgementText = """The acknowledgements"""
        self.parent = parent

# ============================================================================
# class: GLISTRLogic - interface to GLISTR itself
# ============================================================================

## Implements the module's logic, i.e., wraps the calls to GLISTR.
class GLISTRLogic:
    
    # ------------------------------------------------------------------------
    # construction
    # ------------------------------------------------------------------------
    
    ## Constructor.
    def __init__ (self):
        self.__mrmlScene        = None # the MRML scene
        self.__volumeNodes      = None # selected input volume nodes
        self.__workingDirectory = '/tmp/glistr' # working directory
        
        # constants
        self.__featuresListFile    = 'glistr_features.lst'      # generated features list file
        self.__fixedImagesListFile = 'glistr_fixed_images.lst'  # generated fixed images list file
        self.__initialMeansFile    = 'glistr_initial_means.txt' # generated initial means file
        self.__tumorGrowthLength   = 120
        
        # tissue types/labels
        self.__requiredTissueTypes = ['TUMOR', 'VEINS', 'CSF', 'GM', 'WM', 'BG']
        self.__optionalTissueTypes = {'NCR': 'TUMOR', 'EDEMA': 'WM'}
        
        # \todo There should be a better way to connect to the GLISTR software
        #       itself. All three main executables which are required by this
        #       module are implemented in Python. Adjustments of these script
        #       should enable the direct import of these.
        self.__glistrPrefix = os.environ.get ('GLISTR_PREFIX')
        if self.__glistrPrefix:
            self.__glistrBin = os.path.join (self.__glistrPrefix, 'bin')
            # \todo The atlas directory should be selectable by the user with a
            #       preset as given here. The reference T1 scan of the atlas must
            #       then be identified as well as the list of moving images.
            #       This could be easily achieved by introducing simple naming
            #       conventions for the files making up the atlas. The moving
            #       image list could in fact be generated by this module.
            #       Alternatively, have the user select all the different atlas
            #       volumes. These could be default set to the default atlas
            #       volumes which are packaged with GLISTR.
            self.__atlasDirectory       = os.path.join (self.__glistrPrefix,   'data/Atlas')
            self.__refAtlasFile         = os.path.join (self.__atlasDirectory, 'jakob_stripped_with_cere_lps_256256128.hdr')
            self.__brainMaskFile        = os.path.join (self.__atlasDirectory, 'brain_mask.hdr')
            self.__movingImagesListFile = os.path.join (self.__atlasDirectory, 'moving_image_list64_LVN.txt')
        else:
            sys.stderr.write ('GLISTR_PREFIX environment variable not set!\n')
    
    # ------------------------------------------------------------------------
    # setter/getter
    # ------------------------------------------------------------------------
    
    # ************************************************************************
    ## Set MRML scene.
    def setMRMLScene (self, mrmlScene):
        self.__mrmlScene = mrmlScene
    
    # ************************************************************************
    ## Set MRML nodes of the input volumes.
    def setVolumes (self, volumeNodes):
        self.__volumeNodes = volumeNodes
    
    # ************************************************************************
    ## Set working directory.
    def setWorkingDirectory (self, directory):
        self.__workingDirectory = directory
    
    # ************************************************************************
    ## Set tumor growth length parameter value.
    def setTumorGrowthLength (self, length):
        self.__tumorGrowthLength = length
    
    # ************************************************************************
    ## Get working directory.
    def getWorkingDirectory (self):
        return self.__workingDirectory
    
    # ************************************************************************
    ## Get the MRML nodes of the input volumes.
    def getVolumes (self):
        return self.__volumeNodes
    
    # ************************************************************************
    ## Get MRML scene.
    def getMRMLScene (self):
        return self.__mrmlScene
    
    # ************************************************************************
    ## Get the tumor growth length parameter value.
    def getTumorGrowthLength (self):
        return self.__tumorGrowthLength
    
    # ------------------------------------------------------------------------
    # workflow
    # ------------------------------------------------------------------------
    
    # ************************************************************************
    ## Pre-process volumes, i.e., transform them into atlas space.
    def preProcessVolumes (self):
        volumeNodes      = self.__volumeNodes
        workingDirectory = self.__workingDirectory
        inputListFile    = workingDirectory + '/' + self.__featuresListFile
        outputListFile   = workingDirectory + '/' + self.__fixedImagesListFile
        refAtlasFile     = self.__refAtlasFile
        brainMaskFile    = self.__brainMaskFile
        
        # check arguments
        if len (volumeNodes) == 0:
            sys.stderr.write ("No volumes to pre-process!\n")
            return False
        
        # save input volumes and create corresponding list file
        if not self.writeVolumes (inputListFile, volumeNodes):
            return False
        
        # read file name of first volume to use it as reference (e.g., T1)
        fp = open (inputListFile, 'r')
        if not fp:
            sys.stderr.write ("Failed to open file '" + inputListFile + "' for reading\n")
            return False
        refVolumeFile = fp.readline ().strip ()
        fp.close ()
        
        # compose pre-processing command
        cmd = []
        if self.__glistrBin and self.__glistrBin != '':
            cmd = cmd + [os.path.join (self.__glistrBin, 'PreProcessFeatures')]
        else:
            cmd = cmd + ['PreProcessFeatures']
        cmd = cmd + ['-i', inputListFile,
                     '-o', workingDirectory,
                     '-T', refVolumeFile,
                     '-A', refAtlasFile,
                     '-p', outputListFile]
        
        if brainMaskFile and brainMaskFile != '':
            cmd = cmd + ['-m', brainMaskFile]
        
        # perform pre-processing
        if execCmd (cmd) != 0:
            sys.stderr.write ("Failed to pre-process volumes\n")
            return False
        
        # unload volumes and load pre-processed ones instead
        for volumeNode in volumeNodes:
            slicer.mrmlScene.RemoveNode (volumeNode)
        volumeNodes = self.loadVolumes (outputListFile)
        if not volumeNodes or len (volumeNodes) == 0:
            return False
        self.__volumeNodes = volumeNodes
        
        # select first volume
        self.selectVolume (volumeNodes [0])
        
        # done
        return True
    
    # ************************************************************************
    ## Perform joint segmentation & registration.
    def segmentAndRegisterVolumes (self):
        volumeNodes          = self.__volumeNodes
        workingDirectory     = self.__workingDirectory
        fixedImagesListFile  = workingDirectory + '/' + self.__fixedImagesListFile
        movingImagesListFile = self.__movingImagesListFile
        initialMeansFile     = workingDirectory + '/' + self.__initialMeansFile
        tumorGrowthLength    = self.__tumorGrowthLength
        
        # write the initial means file
        if not self.writeInitialMeans (initialMeansFile):
            return False
        
        # get seed point
        seed = self.getTumorSeed ()
        if not seed:
            return False
        
        # compose segmentation & registration command
        cmd = []
        if self.__glistrBin and self.__glistrBin != '':
            cmd = cmd + [os.path.join (self.__glistrBin, 'glistr')]
        else:
            cmd = cmd + ['glistr']
        cmd = cmd + ['-f', fixedImagesListFile,
                     '-m', movingImagesListFile,
                     '-I', initialMeansFile,
                     '-x', seed [0],
                     '-y', seed [1],
                     '-z', seed [2],
                     '-T', tumorGrowthLength]
        
        # perform joint segmentation & registration
        if execCmd (cmd) != 0:
            return False
        
        # done
        return True
    
    # ------------------------------------------------------------------------
    # helper
    # ------------------------------------------------------------------------
    
    # ************************************************************************
    ##  Write the initial means file.
    def writeInitialMeans (self, initialMeansFile):
        mrmlScene           = self.__mrmlScene
        volumeNodes         = self.__volumeNodes
        requiredTissueTypes = self.__requiredTissueTypes
        optionalTissueTypes = self.__optionalTissueTypes
        
        # get number of volumes, i.e., number of mean values per tissue type
        if not volumeNodes or len (volumeNodes) == 0:
            sys.stderr.write ("No volumes to process!\n")
            return False
        
        # get coordinates of annotated fiducial points
        fiducialCoords = {}
        fiducialNodes = mrmlScene.GetNodesByClass ('vtkMRMLAnnotationFiducialNode')
        if fiducialNodes.GetNumberOfItems () == 0:
            sys.stderr.write ("No fiducial points given. Forgot to annotate the pre-processed volumes?\n")
            return None
        
        fiducialNodes.InitTraversal ()
        fiducialNode = fiducialNodes.GetNextItemAsObject ()
        while fiducialNode:
            coord = [0, 0, 0]
            fiducialNode.GetFiducialCoordinates (coord)
            fiducialText = fiducialNode.GetText (0)
            if fiducialText in requiredTissueTypes or optionalTissueTypes.has_key (fiducialText):
                fiducialCoords [fiducialText] = coord
                fiducialNode = fiducialNodes.GetNextItemAsObject ()
        
        # verify that all required tissue types were annotated and set values
        # of optional tissue types to the ones of the mapped tissues
        for label in requiredTissueTypes:
            if not fiducialCoords.has_key (label):
                sys.stderr.write ("No fiducial point for " + label + " tissue type found. " \
                                  "Forgot to set fiducial point or to set its text to " + label + "?\n")
                return None
        for label in optionalTissueTypes.keys ():
            if not fiducialCoords.has_key (label):
                fiducialCoords [label] = fiducialCoords [optionalTissueTypes [label]]
        
        # get intentensities at set fiducials from all volumes
        initialMeans = {}
        for label in fiducialCoords.keys ():
            initialMeans [label] = []
            for volumeNode in volumeNodes:
                initialMeans [label].append (self.getIntensity (volumeNode, fiducialCoords [label]))
        
        # write mean values to file
        fp = open (initialMeansFile, 'w')
        if not fp:
            sys.stderr.write ("Failed to open file " + initialMeansFile + " for writing\n")
            return None
        for label in initialMeans.keys ():
            fp.write (label + "\n")
            initialMeansStr = " ".join ([str (mean) for mean in initialMeans [label]])
            fp.write (initialMeansStr + "\n")
        fp.close ()
        
        return initialMeansFile
    
    # ************************************************************************
    ## Get tumor seed.
    def getTumorSeed (self):
        seed = None
        # get annotated fiducial points
        fiducialNodes = mrmlScene.GetNodesByClass ('vtkMRMLAnnotationFiducialNode')
        if fiducialNodes.GetNumberOfItems () == 0:
            sys.stderr.write ("No fiducial points given. Forgot to annotate the pre-processed volumes?\n")
            return None
        # first consider fiducials which clearly related to "seed"
        fiducialNodes.InitTraversal ()
        fiducialNode = fiducialNodes.GetNextItemAsObject ()
        while fiducialNode:
            coord = [0, 0, 0]
            fiducialNode.GetFiducialCoordinates (coord)
            fiducialText = fiducialNode.GetText (0)
            if fiducialText in ['SEED', 'TUMOR SEED']:
                seed = coord
                break
            fiducialNode = fiducialNodes.GetNextItemAsObject ()
        # then use fiducial labeled as 'TUMOR'
        if not seed:
            fiducialNodes.InitTraversal ()
            fiducialNode = fiducialNodes.GetNextItemAsObject ()
            while fiducialNode:
                coord = [0, 0, 0]
                fiducialNode.GetFiducialCoordinates (coord)
                fiducialText = fiducialNode.GetText (0)
                if fiducialText in ['TUMOR']:
                    seed = coord
                fiducialNode = fiducialNodes.GetNextItemAsObject ()
        # any fiducial point found?
        if not seed:
            return None
        # convert coordinates of seed point
        # \todo
        return seed
    
    # ************************************************************************
    ## Write volume to disk.
    def writeVolume (self, volumeNode):
        outputDirectory = self.__workingDirectory
        fileNameExt     = '.nii'
        
        storageNode = volumeNode.GetStorageNode ()
        fileName = volumeNode.GetName ().replace ('.', '_')
        fileName = 'glistr_' + fileName + fileNameExt
        fileName = outputDirectory + '/' + fileName
        storageNode.SetFileName (fileName)
        if not storageNode.WriteData (volumeNode):
            sys.stderr.write ("Failed to write volume '" \
                + volumeNode.GetName () + "' to file '" + fileName + "'\n")
            return ''
        
        return fileName
    
    # ************************************************************************
    ## Write volumes to disk and create list file.
    def writeVolumes (self, outputListFile, volumeNodes):
        outputDirectory = self.__workingDirectory
        fileNameExt     = '.nii'
        outputList      = ''
        
        # iterate over volumes, write them to disk and add line to output list
        for volumeNode in volumeNodes:
            fileName = self.writeVolume (volumeNode)
            if fileName == '':
                return False
            outputList = outputList + fileName + '\n'
        
        # write features list file
        fp = open (outputListFile, 'w')
        if not fp:
            sys.stderr.write ("Failed to open file '" + outputListFile + "' for writing\n")
            return False
        fp.write (outputList)
        fp.close ()
        
        return True
       
    # ************************************************************************
    ## Load volume.
    def loadVolume (self, fileName):
        volumesLogic = slicer.modules.volumes.logic ()
        nodeName     = os.path.basename (fileName).split ('.',1) [0]
        return volumesLogic.AddArchetypeScalarVolume (fileName, nodeName, 0)
    
    # ************************************************************************
    ## Unload volume.
    def unloadVolume (self, volumeNode):
        slicer.mrmlScene.RemoveNode (volumeNode)
    
    # ************************************************************************
    ## Load volumes specified by a list file.
    def loadVolumes (self, listFile):
        volumeNodes = []
        fp = open (listFile, 'r')
        if not fp:
            sys.stderr.write ("Failed to open file '" + listFile + "' for reading\n")
            return False
        fileName = fp.readline ().strip ()
        while fileName != '':
            volumeNode = self.loadVolume (fileName)
            if volumeNode:
                volumeNodes.append (volumeNode)
            else:
                for volumeNode in volumeNodes:
                    unloadVolume (volumeNode)
                return None
            fileName = fp.readline ().strip ()
        return volumeNodes
    
    # ************************************************************************
    ## Select a given volume to make it visible in the viewers.
    def selectVolume (self, volumeNode):
        mrmlLogic = slicer.app.mrmlApplicationLogic ()
        selNode = mrmlLogic.GetSelectionNode ()
        selNode.SetReferenceActiveVolumeID (volumeNode.GetID ())
        mrmlLogic.PropagateVolumeSelection ()
    
    # ************************************************************************
    ## Get intensity value of scalar volume at given fiducial coordinates.
    def getIntensity (self, volumeNode, fiducialCoords):
        ras = fiducialCoords
        if len (ras) < 4:
            ras.append (1)
        ras2ijk = vtk.vtkMatrix4x4 ()
        volumeNode.GetRASToIJKMatrix (ras2ijk)
        (i,j,k,w) = ras2ijk.MultiplyDoublePoint (ras)
        i = int (round (i))
        j = int (round (j))
        k = int (round (k))
        return volumeNode.GetImageData ().GetScalarComponentAsDouble (i, j, k, 0)

# ============================================================================
# class: GLISTRWidget - the widget
# ============================================================================

## This is the graphical user interface of the GLISTR Slicer module.
#
class GLISTRWidget:
    
    # ************************************************************************
    ## Constructor.
    #
    # If no parent is given, this widget connects itself with slicer, i.e.,
    # declares itself as being derived from the slicer.qMRMLWidget class.
    def __init__ (self, parent = None):
        
        # connect with Slicer
        if not parent:
            self.parent = slicer.qMRMLWidget ()
            self.parent.setLayout (qt.QVBoxLayout ())
            self.parent.setMRMLScene (slicer.mrmlScene)      
        else:
            self.parent = parent
        self.layout = self.parent.layout ()
        
        # instantiate widget logic
        self.__logic = GLISTRLogic ()
        self.__logic.setMRMLScene (slicer.mrmlScene)
        
        # create the widget and display it
        if not parent:
            self.setup ()
            self.parent.show()
        
        # initialize members
        self.__workingDirectoryDlg = None
    
    # ************************************************************************
    ## Create widget.
    #
    # The setup method creates the widget and registers the handlers for
    # the events triggered by the user interacting with Slicer or the widget.
    def setup (self):
        
        logic     = self.__logic
        mrmlScene = logic.getMRMLScene ()
        
        # --------------------------------------------------------------------
        # general
        panel       = qt.QWidget ()
        panelLayout = qt.QGridLayout (panel)
        row         = 0 # row counter for grid layout
        
        # working directory
        workingDirectoryBtn          = qt.QPushButton ()
        workingDirectoryBtn.text     = "..."
        workingDirectoryBtn.setMaximumWidth (20)
        
        workingDirectoryBtn.connect ('clicked ()', self.showWorkingDirectoryDialog)
        
        workingDirectoryEdit         = qt.QLineEdit ()
        workingDirectoryEdit.text    = logic.getWorkingDirectory ()
        workingDirectoryEdit.toolTip = "Set the working directory"
        workingDirectoryEdit.enabled = True
        workingDirectoryEdit.sizeHint.setHeight (workingDirectoryBtn.height)
        
        workingDirectoryEdit.connect ('editingFinished ()', self.onWorkingDirectoryChanged)
        
        panelLayout.addWidget (qt.QLabel ("Working Directory:"), row, 0, 1, 1)
        panelLayout.addWidget (workingDirectoryEdit,             row, 1, 1, 1)
        panelLayout.addWidget (workingDirectoryBtn,              row, 2, 1, 1)
        row = row + 1
        
        self.__workingDirectoryEdit = workingDirectoryEdit
        
        # selection of volumes
        volumeSelector        = []
        volumeSelectorLabel   = []
        volumeSelectorToolTip = []
        
        volumeSelectorLabel  .append ("T1 Volume:")
        volumeSelectorToolTip.append ("Select affine registered T1 volume")
        volumeSelectorLabel  .append ("T1-CE Volume:")
        volumeSelectorToolTip.append ("Select affine registered T1-CE volume")
        volumeSelectorLabel  .append ("T2 Volume:")
        volumeSelectorToolTip.append ("Select affine registered T2 volume")
        volumeSelectorLabel  .append ("FLAIR Volume:")
        volumeSelectorToolTip.append ("Select affine registered FLAIR volume")
        
        for i in range (len (volumeSelectorLabel)):
            volumeSelector.append (slicer.qMRMLNodeComboBox ())
            volumeSelector [i].toolTip       = volumeSelectorToolTip [i]
            volumeSelector [i].nodeTypes     = ['vtkMRMLScalarVolumeNode']
            volumeSelector [i].noneEnabled   = True
            volumeSelector [i].addEnabled    = False
            volumeSelector [i].removeEnabled = False
            
            volumeSelector [i].setMRMLScene (mrmlScene)
            
            volumeSelector [i].connect ('currentNodeChanged (vtkMRMLNode*)', self.onCurrentVolumeChanged)
            
            panelLayout.addWidget (qt.QLabel (volumeSelectorLabel [i]), row, 0, 1, 1)
            panelLayout.addWidget (volumeSelector [i],                  row, 1, 1, 2)
            row = row + 1
        
        self.__volumeSelector = []
        for widget in volumeSelector:
            self.__volumeSelector.append (widget)

        # parameters
        tumorGrowthLengthEdit         = qt.QLineEdit ()
        tumorGrowthLengthEdit.text    = str (logic.getTumorGrowthLength ())
        tumorGrowthLengthEdit.toolTip = "The tumor growth length parameter"
        tumorGrowthLengthEdit.enabled = True
        
        tumorGrowthLengthEdit.connect ('editingFinished ()', self.onTumorGrowthLengthChanged)
        
        panelLayout.addWidget (qt.QLabel ("Tumor Growth Length:"), row, 0, 1, 1)
        panelLayout.addWidget (tumorGrowthLengthEdit,              row, 1, 1, 2)
        row = row + 1
        
        self.__tumorGrowthLengthEdit = tumorGrowthLengthEdit
        
        # execute buttons
        preBtn       = qt.QPushButton ()
        preBtn.text  = "Pre-process Volumes"
        runBtn       = qt.QPushButton ()
        runBtn.text  = "Jointly Segment && Register Volumes"
        postBtn      = qt.QPushButton ()
        postBtn.text = "Post-process Posteriors"
        lblBtn       = qt.QPushButton ()
        lblBtn.text  = "Make Segmentation"
        
        preBtn .connect ('clicked ()', self.onPreProcess)
        runBtn .connect ('clicked ()', self.onSegmentAndRegister)
        postBtn.connect ('clicked ()', self.onPostProcess)
        lblBtn .connect ('clicked ()', self.onMakeSegmentation)
        
        panelLayout.addWidget (preBtn, row, 0, 1, 3)
        row = row + 1
        panelLayout.addWidget (runBtn, row, 0, 1, 3)
        row = row + 1
        panelLayout.addWidget (postBtn, row, 0, 1, 3)
        row = row + 1
        panelLayout.addWidget (lblBtn, row, 0, 1, 3)
        row = row + 1
        
        # add panel
        self.layout.addWidget (panel)
    
    # ************************************************************************
    ## Show output directory selection dialog for pre-processing.
    def showWorkingDirectoryDialog (self):
        dlg = self.__workingDirectoryDlg
        if not dlg:
            dlg          = qt.QFileDialog ()
            dlg.fileMode = dlg.DirectoryOnly
            dlg.connect ('fileSelected (QString)', self.onSelectedWorkingDirectory)
            self.__workingDirectoryDlg = dlg
        directory = self.__workingDirectoryEdit.text
        if directory == '':
            directory = os.environ.get ('HOME')
            if directory:
                dlg.setDirectory (directory + "/")
        dlg.show ()
    
    # ************************************************************************
    ## Set output directory for pre-processing results.
    def onSelectedWorkingDirectory (self, directory):
        self.__workingDirectoryEdit.text = directory
        self.__logic.setWorkingDirectory (directory)
    
    # ************************************************************************
    ## Handle change of working directory.
    def onWorkingDirectoryChanged (self):
        self.__logic.setWorkingDirectory (self.__workingDirectoryEdit.text)
    
    # ************************************************************************
    ## Handle change of tumor growth length.
    def onTumorGrowthLengthChanged (self):
        self.__logic.setTumorGrowthLength (float (self.__tumorGrowthLengthEdit.text))
    
    # ************************************************************************
    ## Handle change of selected volume.
    def onCurrentVolumeChanged (self, volumeNode):
        self.updateVolumes ()
    
    # ************************************************************************
    ## Update list of selected volumes.
    def updateVolumes (self):
        volumeNodes = []
        for widget in self.__volumeSelector:
            volumeNode = widget.currentNode ()
            if volumeNode:
                volumeNodes.append (volumeNode)
        self.__logic.setVolumes (volumeNodes)
    
    # ************************************************************************
    ## Pre-process the features, i.e., affine transform patient scans.
    def onPreProcess (self):
        self.__logic.preProcessVolumes ()
        volumeNodes = self.__logic.getVolumes ()
        for i in range (len (self.__volumeSelector)):
            if i > len (volumeNodes):
                break
            self.__volumeSelector [i].setCurrentNode (volumeNodes [i])
    
    # ************************************************************************
    ## Perform joint segmentation & registration.
    def onSegmentAndRegister (self):
        self.__logic.segmentAndRegisterVolumes ()
    
    # ************************************************************************
    ## Post-process posteriors.
    def onPostProcess (self):
        return False
    
    # ************************************************************************
    ## Make segmentation from posteriors.
    def onMakeSegmentation (self):
        return False
