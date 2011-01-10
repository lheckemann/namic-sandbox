proc log { txt } {
  puts "$txt"
  set logFile [open $::logFilename a]
  puts $logFile "$txt"
  close $logFile
}

proc GetDirectoryNameForCase { groupId caseId } {
  set dirname "$::workDir/${groupId}_[format %03i $caseId]"
  return $dirname 
}


###########################################################

proc CreateFileList { listfilename relativepath groupId startInd endInd } {
  set fp [open $listfilename w]
  for {set i $startInd} {$i<=$endInd} {incr i} {
    set dirName [GetDirectoryNameForCase $groupId $i]
    set fullFilename "$dirName/$relativepath"
    if { [file exist $fullFilename] } {
      puts $fp "$dirName/$relativepath"
    } else {
      log "Warning: $fullFilename was not found"
    }
  }
  close $fp
} 

##################

set workDir "c:/Users/andras/Documents/DefRegEval/test"
set binDir "c:/Users/andras/devel/DefRegEval-bin/Release"

set ::logFilename "$workDir/statResults.txt"
 
log "-------------------------"
set groupId ForcePerturb3
CreateFileList filelist.txt DeformFieldDiffMagImage.mha $groupId 1 50
exec "$binDir/ComputeMultiImageStatistics.exe" --fileList=filelist.txt --meanImage=$workDir/${groupId}_treMean.mha --maxImage=$workDir/${groupId}_treMax.mha
CreateFileList filelist.txt DeformFieldFemImage.mha $groupId 1 50
exec "$binDir/ComputeMultiImageStatistics.exe" --vectorInput --fileList=filelist.txt --meanImage=$workDir/${groupId}_femDefMean.mha --maxImage=$workDir/${groupId}_femDefMax.mha 

log "-------------------------"
set groupId ForcePerturb4
CreateFileList filelist.txt DeformFieldDiffMagImage.mha $groupId 1 50
exec "$binDir/ComputeMultiImageStatistics.exe" --fileList=filelist.txt --meanImage=$workDir/${groupId}_treMean.mha --maxImage=$workDir/${groupId}_treMax.mha
CreateFileList filelist.txt DeformFieldFemImage.mha $groupId 1 50
exec "$binDir/ComputeMultiImageStatistics.exe" --vectorInput --fileList=filelist.txt --meanImage=$workDir/${groupId}_femDefMean.mha --maxImage=$workDir/${groupId}_femDefMax.mha 

log "-------------------------"
set groupId ForcePerturb5
CreateFileList filelist.txt DeformFieldDiffMagImage.mha $groupId 1 50
exec "$binDir/ComputeMultiImageStatistics.exe" --fileList=filelist.txt --meanImage=$workDir/${groupId}_treMean.mha --maxImage=$workDir/${groupId}_treMax.mha
CreateFileList filelist.txt DeformFieldFemImage.mha $groupId 1 50
exec "$binDir/ComputeMultiImageStatistics.exe" --vectorInput --fileList=filelist.txt --meanImage=$workDir/${groupId}_femDefMean.mha --maxImage=$workDir/${groupId}_femDefMax.mha 

log "-------------------------"
set groupId SegmPerturb1
CreateFileList filelist.txt DeformFieldDiffMagImage.mha $groupId 1 30
exec "$binDir/ComputeMultiImageStatistics.exe" --fileList=filelist.txt --meanImage=$workDir/${groupId}_treMean.mha --maxImage=$workDir/${groupId}_treMax.mha

log "-------------------------"
set groupId SegmPerturb2
CreateFileList filelist.txt DeformFieldDiffMagImage.mha $groupId 1 19
exec "$binDir/ComputeMultiImageStatistics.exe" --fileList=filelist.txt --meanImage=$workDir/${groupId}_treMean.mha --maxImage=$workDir/${groupId}_treMax.mha
