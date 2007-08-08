# \author  Greg Harris
# \date    
# \brief  date stamping for use in filenames.
# \fn    proc DateStamp {} 
# \result  String of the form yyyymmmdd_hhmmss


proc DateStamp {} {
    global env
    set date_list [exec date]
    set time_list [split [lindex $date_list 3] :]
    set date_stamp "[lindex $date_list 5][lindex $date_list 1][lindex $date_list 2]_[lindex $time_list 0][lindex $time_list 1][lindex $time_list 2]"
    return $date_stamp
}

