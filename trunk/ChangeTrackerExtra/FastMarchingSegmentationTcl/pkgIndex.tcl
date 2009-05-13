if {[catch {package require Itcl}]} { return }
package ifneeded FastMarchingSegmentationTcl 3.0 [list 
  source [file join $dir FastMarchingSegmentationTcl.tcl]
  source [file join $dir FastMarchingSegmentationTclNode.tcl]
  source [file join $dir FastMarchingSegmentationTclLogic.tcl]
  source [file join $dir FastMarchingSegmentationTclGUI.tcl]
]
