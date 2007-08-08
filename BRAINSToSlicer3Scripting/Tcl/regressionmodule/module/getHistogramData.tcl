# \author    Hans J. Johnson"
# \date        $Date: 2005-03-08 16:40:34 -0600 (Tue, 08 Mar 2005) $
# \brief    This module tests the b2 get histogram data command
# \fn        proc getHistogramData {pathToRegressionDir dateString}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString            - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
# Test the b2 get histogram data command
#
# To Do
#------------------------------------------------------------------------
# Nothing
#


proc getHistogramData {pathToRegressionDir dateString} {

    set ModuleName "getHistogramData"
    set ModuleAuthor "Hans J. Johnson"
    set ModuleDescription "Test the b2 get histogram data command"
    global MODULE_SUCCESS
    global MODULE_FAILURE
    set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString]


########################################
########################################
# Run Tests
########################################
########################################


    set ExpectData { {0.000000 1.000000 2139} {1.000000 2.000000 17124} {2.000000 3.000000 0} {3.000000 4.000000 0} \
                     {4.000000 5.000000 0} {5.000000 6.000000 0} {6.000000 7.000000 0} {7.000000 8.000000 0} \
                     {8.000000 9.000000 0} {9.000000 10.000000 0} {10.000000 11.000000 0} {11.000000 12.000000 0} \
                     {12.000000 13.000000 0} {13.000000 14.000000 0} {14.000000 15.000000 0} {15.000000 16.000000 0} \
                     {16.000000 17.000000 0} {17.000000 18.000000 0} {18.000000 19.000000 0} {19.000000 20.000000 0} \
                     {20.000000 21.000000 0} {21.000000 22.000000 0} {22.000000 23.000000 0} {23.000000 24.000000 0} \
                     {24.000000 25.000000 0} {25.000000 26.000000 0} {26.000000 27.000000 0} {27.000000 28.000000 0} \
                     {28.000000 29.000000 0} {29.000000 30.000000 0} {30.000000 31.000000 0} {31.000000 32.000000 0} \
                     {32.000000 33.000000 0} {33.000000 34.000000 0} {34.000000 35.000000 0} {35.000000 36.000000 0} \
                     {36.000000 37.000000 0} {37.000000 38.000000 2385} {38.000000 39.000000 2169} \
                     {39.000000 40.000000 1923} {40.000000 41.000000 1899} {41.000000 42.000000 1899} \
                     {42.000000 43.000000 2054} {43.000000 44.000000 2189} {44.000000 45.000000 2330} \
                     {45.000000 46.000000 2402} {46.000000 47.000000 2492} {47.000000 48.000000 2672} \
                     {48.000000 49.000000 2525} {49.000000 50.000000 2650} {50.000000 51.000000 3109} \
                     {51.000000 52.000000 2772} {52.000000 53.000000 2931} {53.000000 54.000000 2652} \
                     {54.000000 55.000000 3166} {55.000000 56.000000 3134} {56.000000 57.000000 3187} \
                     {57.000000 58.000000 3219} {58.000000 59.000000 3207} {59.000000 60.000000 4067} \
                     {60.000000 61.000000 4046} {61.000000 62.000000 4590} {62.000000 63.000000 4481} \
                     {63.000000 64.000000 5488} {64.000000 65.000000 5284} {65.000000 66.000000 5294} \
                     {66.000000 67.000000 5961} {67.000000 68.000000 5485} {68.000000 69.000000 4984} \
                     {69.000000 70.000000 5579} {70.000000 71.000000 5371} {71.000000 72.000000 5434} \
                     {72.000000 73.000000 5418} {73.000000 74.000000 5480} {74.000000 75.000000 5281} \
                     {75.000000 76.000000 5621} {76.000000 77.000000 5574} {77.000000 78.000000 5116} \
                     {78.000000 79.000000 5755} {79.000000 80.000000 5352} {80.000000 81.000000 5971} \
                     {81.000000 82.000000 5402} {82.000000 83.000000 6055} {83.000000 84.000000 5596} \
                     {84.000000 85.000000 5784} {85.000000 86.000000 5832} {86.000000 87.000000 5865} \
                     {87.000000 88.000000 5847} {88.000000 89.000000 6143} {89.000000 90.000000 5602}
                     {90.000000 91.000000 9434} {91.000000 92.000000 10291} {92.000000 93.000000 10835} \
                     {93.000000 94.000000 9919} {94.000000 95.000000 10575} {95.000000 96.000000 11136} \
                     {96.000000 97.000000 11301} {97.000000 98.000000 10831} {98.000000 99.000000 11227} \
                     {99.000000 100.000000 11283} {100.000000 101.000000 10263} {101.000000 102.000000 10782} \
                     {102.000000 103.000000 11580} {103.000000 104.000000 10349} {104.000000 105.000000 9729} \
                     {105.000000 106.000000 11341} {106.000000 107.000000 9467} {107.000000 108.000000 11022} \
                     {108.000000 109.000000 10237} {109.000000 110.000000 10974} {110.000000 111.000000 9969} \
                     {111.000000 112.000000 10420} {112.000000 113.000000 12595} {113.000000 114.000000 9811} \
                     {114.000000 115.000000 11741} {115.000000 116.000000 10522} {116.000000 117.000000 11005} \
                     {117.000000 118.000000 11236} {118.000000 119.000000 11589} {119.000000 120.000000 11660} \
                     {120.000000 121.000000 10214} {121.000000 122.000000 12333} {122.000000 123.000000 12484} \
                     {123.000000 124.000000 10444} {124.000000 125.000000 12617} {125.000000 126.000000 10339} \
                     {126.000000 127.000000 11979} {127.000000 128.000000 9344} {128.000000 129.000000 11421} \
                     {129.000000 130.000000 10428} {130.000000 131.000000 18211} {131.000000 132.000000 7638} \
                     {132.000000 133.000000 9029} {133.000000 134.000000 8889} {134.000000 135.000000 9365} \
                     {135.000000 136.000000 9409} {136.000000 137.000000 10350} {137.000000 138.000000 9631} \
                     {138.000000 139.000000 9638} {139.000000 140.000000 8675} {140.000000 141.000000 9522} \
                     {141.000000 142.000000 9413} {142.000000 143.000000 9389} {143.000000 144.000000 8528} \
                     {144.000000 145.000000 9321} {145.000000 146.000000 9260} {146.000000 147.000000 9025} \
                     {147.000000 148.000000 8123} {148.000000 149.000000 8890} {149.000000 150.000000 8771} \
                     {150.000000 151.000000 7767} {151.000000 152.000000 8649} {152.000000 153.000000 7678} \
                     {153.000000 154.000000 7559} {154.000000 155.000000 8178} {155.000000 156.000000 7345} \
                     {156.000000 157.000000 7365} {157.000000 158.000000 7866} {158.000000 159.000000 7143} \
                     {159.000000 160.000000 7842} {160.000000 161.000000 6964} {161.000000 162.000000 6937} \
                     {162.000000 163.000000 7741} {163.000000 164.000000 6694} {164.000000 165.000000 7378} \
                     {165.000000 166.000000 7341} {166.000000 167.000000 7225} {167.000000 168.000000 6460} \
                     {168.000000 169.000000 7062} {169.000000 170.000000 6988} {170.000000 171.000000 9027} \
                     {171.000000 172.000000 10985} {172.000000 173.000000 11691} {173.000000 174.000000 11492} \
                     {174.000000 175.000000 10933} {175.000000 176.000000 11194} {176.000000 177.000000 10781} \
                     {177.000000 178.000000 10162} {178.000000 179.000000 9968} {179.000000 180.000000 9139} \
                     {180.000000 181.000000 8974} {181.000000 182.000000 9293} {182.000000 183.000000 9241} \
                     {183.000000 184.000000 8005} {184.000000 185.000000 7419} {185.000000 186.000000 7517} \
                     {186.000000 187.000000 7761} {187.000000 188.000000 6997} {188.000000 189.000000 7250} \
                     {189.000000 190.000000 6558} {190.000000 191.000000 6762} {191.000000 192.000000 5944} \
                     {192.000000 193.000000 6937} {193.000000 194.000000 6117}\
                    {194.000000 195.000000 6056} {195.000000 196.000000 6291} {196.000000 197.000000 6103} \
                    {197.000000 198.000000 5375} {198.000000 199.000000 6333} {199.000000 200.000000 6233} \
                    {200.000000 201.000000 5402} {201.000000 202.000000 6279} {202.000000 203.000000 5343} \
                    {203.000000 204.000000 6106} {204.000000 205.000000 5980} {205.000000 206.000000 5965} \
                    {206.000000 207.000000 5152} {207.000000 208.000000 6448} {208.000000 209.000000 5553} \
                    {209.000000 210.000000 6313} {210.000000 211.000000 5156} {211.000000 212.000000 6363} \
                    {212.000000 213.000000 5464} {213.000000 214.000000 5843} {214.000000 215.000000 5529} \
                    {215.000000 216.000000 5781} {216.000000 217.000000 5857} {217.000000 218.000000 5898} \
                    {218.000000 219.000000 5793} {219.000000 220.000000 5720} {220.000000 221.000000 5833} \
                    {221.000000 222.000000 5688} {222.000000 223.000000 5737} {223.000000 224.000000 6037} \
                    {224.000000 225.000000 6191} {225.000000 226.000000 4995} {226.000000 227.000000 0} \
                    {227.000000 228.000000 0} {228.000000 229.000000 0} {229.000000 230.000000 0} \
                    {230.000000 231.000000 0} {231.000000 232.000000 0} {232.000000 233.000000 0} \
                    {233.000000 234.000000 0} {234.000000 235.000000 0} {235.000000 236.000000 0} \
                    {236.000000 237.000000 0} {237.000000 238.000000 0} {238.000000 239.000000 0} \
                    {239.000000 240.000000 0} {240.000000 241.000000 0} {241.000000 242.000000 0} \
                    {242.000000 243.000000 0} {243.000000 244.000000 0} {244.000000 245.000000 0} \
                    {245.000000 246.000000 0} {246.000000 247.000000 0} {247.000000 248.000000 0} \
                    {248.000000 249.000000 0} {249.000000 250.000000 0} {250.000000 251.000000 0} \
                    {251.000000 252.000000 0} {252.000000 253.000000 0} {253.000000 254.000000 0} \
                    {254.000000 255.000000 0} {255.000000 256.000000 0}}





    set SubTestDes "Get histogram data - Load Histogram test"
    set TestHistID [b2 load histogram $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/user/cran_histogram]
    if { [ ReportTestStatus $LogFile  [ expr {$TestHistID != -1 } ] $ModuleName $SubTestDes ] == 0} {
        return $MODULE_FAILURE
    }


    # First Test for invalid arguements
    set SubTestDes "required arguement test (1)"
    set errorTest [b2 get histogram data]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "arguement number test"
    set errorTest [b2 get histogram data $TestHistID -1 junk= ]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "optional arguement test"
    set errorTest [b2 get histogram data $TestHistID -1 junk= test]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes


    set SubTestDes "Histogram Data test"
    set histData [b2 get histogram data $TestHistID]
    ReportTestStatus $LogFile  [ expr {[llength $histData] == 256 } ] $ModuleName $SubTestDes

    for {set i 0} {$i < [llength $ExpectData]} {incr i} {
        for {set j 0} {$j < 3} {incr j} {
            ReportTestStatus $LogFile  [ expr {[lindex [lindex $histData $i] $j] == [lindex [lindex $ExpectData $i] $j] } ] $ModuleName "Histogram $i value $j test"
        }
    }


    ReportTestStatus $LogFile  [ expr { [ b2 destroy histogram $TestHistID ] != -1 } ] $ModuleName "Destroying histogram $TestHistID"

    return [ StopModule  $LogFile $ModuleName ]

    return $MODULE_SUCCESS
}

