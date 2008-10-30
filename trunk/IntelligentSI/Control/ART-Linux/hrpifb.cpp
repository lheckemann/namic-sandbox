#ifdef __linux__

#include "hrpifb.h"

using namespace std;

HRPIFB* HRPIFB::m_Instance = NULL;

HRPIFB::HRPIFB() {

  board_num = DeviceLookup();  

  if (DeviceInit(board_num) < 0) {
    cout << "HRPIFB: ERROR: Board unit " << board_num << " not ready." << endl;
  }
  else {
    cout << "HRPIFB: Board unit " << board_num << " is ready." << endl;
  }
}

HRPIFB::~HRPIFB() {
  I7fClose(board_num);
}

HRPIFB* HRPIFB::getInstance() {
  if (m_Instance == NULL )
    m_Instance = new HRPIFB();
  return m_Instance;
}

int
HRPIFB::DeviceLookup() {
  int bnum = 0; 
  //TODO: find the board using libpci
  return (bnum);
}

int
HRPIFB::DeviceInit(int unitNum) {
  int fd;
  int ret;
  int ch = 0; //tmp

  if ((fd = I7fOpen(unitNum)) < 0) {
    cout << "HRPIFB: ERROR: Can not open /dev/hrpib70. fd=" << fd << endl;
    return -1;
  }
  //reset
  ret = I7fReset(unitNum); //set outputs of all D/A, PIO channels to 0

  //get device information
  Ib7ParaInfo.Ib7no = unitNum; 
  ret = I7fGetInfo(&Ib7ParaInfo);
  // A/D
  Ib7ParaAd.Ib7no = unitNum;
  Ib7ParaAd.ChData.ch = ch;
  // D/A
  Ib7ParaDa.Ib7no = unitNum;
  Ib7ParaDa.ChData.ch = ch;
  // Enc
  Ib7ParaEnc.Ib7no = unitNum;
  Ib7ParaEnc.ChData.mode = ENC_MODE_X4; // TODO: confirm which mode to use

  for (int i=0; i<4; i++) {
    Ib7ParaEnc.ChData.ch = i;
    if ((ret = I7fEncMode(&Ib7ParaEnc)) < 0) {
      cout << "HRPIFB: ERROR: can not set ENC MODE to " << Ib7ParaEnc.ChData.mode << "for channel ch=" << i << endl;
      ret = -1;
    }
  }

  //TODO: confirm if we need to clear the encoder here /or where/?
  for (int i=0; i<4; i++) {
    Ib7ParaEnc.ChData.ch = i;
    if((ret=I7fEncClear(&Ib7ParaEnc)) < 0){ // clear the counters
      cout << "HRPIFB: ERROR: ENC clear for ch=" << i << ". ret=" << ret << endl;
      ret = -1;
    }
  }

  //TODO: confirm when to start the encoder? - OK here
  for (int i=0; i<4; i++) {
    Ib7ParaEnc.ChData.ch = i;
    if ((ret = I7fEncStart(&Ib7ParaEnc)) < 0) {
      cout << "HRPIFB: ERROR: can not start ENC for channel ch=" << i << ". ret=" << ret << endl;
      ret = -1;
    }
  }

  

  return (0);
}

/* Encoder read 
 * TODO: fixme - now I am assuming that *readData is readData[3]
 *               this is set in the DRIVER::jNum 
 *               and NOT sent as additional function argument
 *       check - handling the sign: 
 *               the counter is (signed) 24bit value received as unsigned int (32bit)
 *               we should put (signed) long int in *readData 
 */
void
HRPIFB::Read(int unitNum, long *readData){
  unsigned int tmpDat[4]; //DWORD == unsigned int (from I7sDriver/I7sPciBase.h)
  int ret;
  //TODO: interrupt lock?
  
  // Ib7ParaEnc.Ib7no = unitNum; //TODO: confirm: the DRIVER calling us doesn't know the unit number
  for (int ch=0; ch<4; ch++) {
    Ib7ParaEnc.ChData.ch = ch;
    if ((ret = I7fEncRead(&Ib7ParaEnc)) < 0) {
      cerr << "HRPIFB: Encoder Read, ERROR " << ret << endl;
    }

    tmpDat[ch]=Ib7ParaEnc.ChData.dwData[0];
  }
  //now: fill long int readData[3] with unsigned int dwDat[4] 
  //      for now just use the ch0-2 and leave ch3 unused
  //for (int ch=0; ch<3; ch++) {
  for (int ch=1; ch<2; ch++) {
    *(readData+ch) = ((long int)(tmpDat[ch] << 8) >> 8); 
  }
}

/* Encoder write
 *  fill unsigned int dwData[ch] with long int wData[ch] 
 *   if outside signed 24bit range [-0x800000 : 0x7fffff] - put a WARNING and clip;
 *   zero the excess bits before sending.
 */ 
void
HRPIFB::Write(int unitNum, long *wData){
  int ret;
  //TODO: interrupt lock?

  // Ib7ParaEnc.Ib7no = unitNum; //TODO: confirm: the DRIVER calling us doesn't know the unit number
  //for (int ch=0; ch<3; ch++) {  //TODO: fixme - ch should go from 0 to min(DRIVER::jNum,maxch)
  for (int ch=1; ch<2; ch++) {  //TODO: temporary
    Ib7ParaEnc.ChData.ch = ch;
    if (!((wData[ch] >=  -0x800000l) && (wData[ch] <=  0x7fffffl))) {
      cerr << "HRPIFB: Encoder Write, WARNING: the value sent wData[" << ch <<"] " << wData[ch] << " is outside the allowed range +/-8388607 (signed 24bit); continuing with a clipped value." << endl;
      wData[ch] = (wData[ch] > 0) ? 0x7fffffl : 0x800000l;
    }
    Ib7ParaEnc.ChData.dwData[0] = (wData[ch] & 0xffffffl);
    if ((ret = I7fEncWrite(&Ib7ParaEnc)) < 0) {
      cerr << "HRPIFB: Encoder Write, ERROR " << ret << endl;
    }
  }
}

// A/D read data
double
HRPIFB::ReadData(int unitNum, int ch){
  WORD wDat;
  double volt=0.0;
  int ret;

  //Ib7ParaAd.Ib7no = unitNum; //TODO: confirm: the DRIVER calling us doesn't know the unit number
  Ib7ParaAd.ChData.ch = ch;
  if ((ret = I7fAdRead(&Ib7ParaAd)) < 0) {
    cerr << "HRPIFB: A/D Read ERROR " << ret << endl;
  }
  wDat=Ib7ParaAd.ChData.wData[0];
  volt=20.0*(double)wDat/4096.0-10.0; //data -> Volt (+/- 10V input range)

  return (volt);
}

// D/A write data
void
HRPIFB::Output(int unitNum,  double v1, double v2, double v3, double v4){
  WORD wDat; // WORD == unsigned short (from I7sDriver/I7sPciBase.h)
  double volt[4];
  int ret;

  volt[0] = v1;  //TODO: confirm
  volt[1] = v2;
  volt[2] = v3;
  volt[3] = v4;
  //Ib7ParaDa.Ib7no = unitNum;  //TODO: confirm: the DRIVER calling us doesn't know the unit number

  for (int ch=0; ch<4; ch++) {
    Ib7ParaDa.ChData.ch = ch;
    if(volt[ch] > 10.0) {cerr << "HRPIFB: D/A Output, WARNING: the value sent volt[" << ch <<"] " << volt[ch] << " is above the allowed 10.0V; continuing with a clipped value." << endl; volt[ch] = 10.0;}
    else if(volt[ch] < -10.0) {cerr << "HRPIFB: D/A Output, WARNING: the value sent volt[" << ch <<"] " << volt[ch] << " is below the allowed -10.0V; continuing with a clipped value." << endl; volt[ch] = -10.0;}

    if ((volt[ch] >= -10.0) && (volt[ch] <= 10.0)) {
      wDat=(WORD)(volt[ch]*4096.0/20.0+2047.5); //Volt -> data (+/- 10V output range);
      Ib7ParaDa.ChData.wData[0] = wDat;
    }
    else {
      Ib7ParaDa.ChData.wData[0] = (WORD)0.0; //TODO: confirm if this is a sensible value
    }
    // if (ch == 0) cout << "DA[0]=" << Ib7ParaDa.ChData.wData[0] << " volt[0]="<< volt[0] << endl; //temporary output
    if ((ret = I7fDaWrite(&Ib7ParaDa)) < 0) {
      cerr << "HRPIFB: D/A Write ERROR" << ret << endl;
    }
  }

}

#endif // __linux__
