import numpy

#def abc():
#    sys.stderr.write('abc() is called.\n')

if len(argv) < 2:
    sys.exit()

BundleNodeID = sys.argv[1]
scene        = slicer.MRMLScene
bundleNode   = scene.GetNodeByID(BundleNodeID)

sys.stderr.write('4D Bundle: %s\n' % ''.join( node.GetName() ))
                 
nFrames = bundleNode.GetNumberOfFrames()

for i in range(nFrames):
    volumeNode = bundleNode.GetFrameNode(i)
    sys.stderr.write('4D Bundle: %s\n' % ''.join( volumeNode.GetName() ))
    
