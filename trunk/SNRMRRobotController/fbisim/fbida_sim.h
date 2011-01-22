/* ***************************************************************
   fbida.h 
   ---------------------------------------------------------------
   Version 1.03-04
   ---------------------------------------------------------------
   Date July 19, 2002
   ---------------------------------------------------------------
   Copyright 2000, 2002 Interface Corporation. All rights reserved.
   *************************************************************** */

#if !defined( _FBIDA_H_ )
#define _FBIDA_H_

#ifdef __cplusplus
extern "C" {
#endif

  /*#include <linux/ioctl.h>*/

typedef void (*PLPDACALLBACK)(int nReserved);
typedef void (  LPDACALLBACK)(int nReserved);

typedef void (*PDACONVPROC)(int nCh, unsigned long ulCount, void *pData);
typedef void (* DACONVPROC)(int nCh, unsigned long ulCount, void *pData);

/* Structure of Output Channel Config */
typedef struct _DASMPLCHREQ {
        unsigned long   ulChNo;
        unsigned long   ulRange;
}DASMPLCHREQ, *PDASMPLCHREQ;

/* Structure of Sampling Config */
typedef struct _DASMPLREQ {
        unsigned long   ulChCount;
        DASMPLCHREQ     SmplChReq[256];
        unsigned long   ulSamplingMode;
        float           fSmplFreq;
        unsigned long   ulSmplRepeat; 
        unsigned long   ulTrigMode;
        unsigned long   ulTrigPoint;
        unsigned long   ulTrigDelay;
        unsigned long   ulEClkEdge;
        unsigned long   ulTrigEdge;
        unsigned long   ulTrigDI;
}DASMPLREQ, *PDASMPLREQ;

/* Structure of Sampling Config(FIFO TYPE) */
typedef struct _DAFIFOREQ {
        unsigned long   ulChCount;
        DASMPLCHREQ     SmplChReq[256];
        float           fSmplFreq;
        unsigned long   ulSmplRepeat; 
        unsigned long   ulSmplNum;
        unsigned long   ulStartTrigCondition;
        unsigned long   ulStopTrigCondition;
        unsigned long   ulEClkEdge;
        unsigned long   ulTrigEdge;
}DAFIFOREQ, *PDAFIFOREQ;

/* Structure of Port Information */
typedef struct _DABOARDSPEC {
        unsigned long   ulBoardType;
        unsigned long   ulBoardID;
        unsigned long   ulSamplingMode;
        unsigned long   ulChCount;
        unsigned long   ulResolution;
        unsigned long   ulRange;
        unsigned long   ulIsolation;
        unsigned long   ulDi;
        unsigned long   ulDo;
}DABOARDSPEC ,*PDABOARDSPEC; 

typedef struct {
        unsigned long   ulRange;
        float           fVolt;
        unsigned long   ulFilter;
} DAMODECHREQ, *PDAMODECHREQ;

typedef struct _DAMODEREQ{
        DAMODECHREQ     ModeChReq[2]; 
        unsigned long   ulPulseMode; 
        unsigned long   ulSyntheOut; 
        unsigned long   ulInterval; 
        float           fIntervalCycle; 
        unsigned long   ulCounterClear; 
        unsigned long   ulDaLatch; 
        unsigned long   ulSamplingClock; 
        unsigned long   ulExControl; 
        unsigned long   ulExClock; 
} DAMODEREQ, *PDAMODEREQ;

typedef struct _DA_BOARD_SPEC{
        unsigned long   ulBoardType; 
        unsigned long   ulBoardID; 
        unsigned long   ulSamplingMode; 
        unsigned long   ulChCount; 
        unsigned long   ulResolution;
        unsigned long   ulRange;
        unsigned long   ulIsolation;
        unsigned long   ulDi; 
        unsigned long   ulDo; 

        unsigned long   ulOutsideControl; 
        unsigned long   ulInterrupt; 
        unsigned long   ulRefOut; 
        unsigned long   ulRangeSet; 
        unsigned long   ulAdjustment; 
        unsigned long   ulSettlingTime; 
        unsigned long   ulDefRng;
        unsigned long   ulOffsetMin;
        unsigned long   ulOffsetMax;
        unsigned long   ulMiddleMin;
        unsigned long   ulMiddleMax;
        unsigned long   ulGainMin;
        unsigned long   ulGainMax;

        unsigned char   precision_0;    // �������� ���  0-1V
        unsigned char   precision_1;    // �������� ���  0-2.5V
        unsigned char   precision_2;    // �������� ���  0-5V
        unsigned char   precision_3;    // �������� ���  0-10V
        unsigned char   precision_4;    // �������� ���  1-5V
        unsigned char   precision_5;    // �������� ͽ��
        unsigned char   precision_6;    // �������� ͽ��
        unsigned char   precision_7;    // �������� ͽ��

        unsigned char   precision_8;    // �������� ͽ��
        unsigned char   precision_9;    // �������� ͽ��
        unsigned char   precision_10;   // �������� ͽ��
        unsigned char   precision_11;   // �������� ͽ��

        unsigned char   precision_12;   // �������� ���  0-20mA
        unsigned char   precision_13;   // �������� ���  4-20mA
        unsigned char   precision_14;   // �������� ͽ��
        unsigned char   precision_15;   // �������� ͽ��

        unsigned char   precision_16;   // �������� ���  ��1V
        unsigned char   precision_17;   // �������� ���  ��2.5V
        unsigned char   precision_18;   // �������� ���  ��5V
        unsigned char   precision_19;   // �������� ���  ��10V
        unsigned char   precision_20;   // �������� ͽ��
        unsigned char   precision_21;   // �������� ͽ��
        unsigned char   precision_22;   // �������� ͽ��
        unsigned char   precision_23;   // �������� ͽ��

        unsigned char   precision_24;   // �������� ͽ��
        unsigned char   precision_25;   // �������� ͽ��
        unsigned char   precision_26;   // �������� ͽ��
        unsigned char   precision_27;   // �������� ͽ��        
        unsigned char   precision_28;   // �������� ͽ��
        unsigned char   precision_29;   // �������� ͽ��
        unsigned char   precision_30;   // �������� ͽ��
        unsigned char   precision_31;   // �������� ͽ��

} DA_BOARD_SPEC, *PDA_BOARD_SPEC;

/* Structure of DaDeviceFind Data */
typedef struct _DA_FIND_DEV {
        int     type;
        int     rsw;
        int     subsys;
}DA_FIND_DEV, *PDA_FIND_DEV;

//-----------------------------------------------------------------------------------------------
//
//              Ʊ���������̻�
//
//-----------------------------------------------------------------------------------------------
#define FLAG_SYNC                               1       // Ʊ���ǥ���ץ������
#define FLAG_ASYNC                              2       // ��Ʊ���ǥ���ץ������

//-----------------------------------------------------------------------------------------------
//
//              �ե�����������̻�
//
//-----------------------------------------------------------------------------------------------
#define FLAG_BIN                                1       // �Х��ʥ�����ե�����
#define FLAG_CSV                                2       // �ãӣַ����ե�����

//-----------------------------------------------------------------------------------------------
//
//              �ǡ����������̻�
//
//-----------------------------------------------------------------------------------------------
#define CONV_BIN                                1       // �Х��ʥ�ǡ���
#define CONV_PHYS                               2       // ʪ���̥ǡ���

//-----------------------------------------------------------------------------------------------
//
//              ����ץ�󥰾��ּ��̻�
//
//-----------------------------------------------------------------------------------------------
#define DA_STATUS_STOP_SAMPLING                 1       // ����ץ�󥰤���ߤ��Ƥ��ޤ�
#define DA_STATUS_WAIT_TRIGGER                  2       // ����ץ�󥰤ϥȥꥬ�Ԥ����֤Ǥ�
#define DA_STATUS_NOW_SAMPLING                  3       // ����ץ��ư����Ǥ�

//-----------------------------------------------------------------------------------------------
//
//              ���٥���װ����̻�
//
//-----------------------------------------------------------------------------------------------
#define DA_EVENT_STOP_TRIGGER                   1       // �ȥꥬ�ˤ�륵��ץ�����
#define DA_EVENT_STOP_FUNCTION                  2       // �ؿ��ˤ�륵��ץ�����
#define DA_EVENT_STOP_SAMPLING                  3       // ����ץ�󥰽�λ

//-----------------------------------------------------------------------------------------------
//
//              Ĵ���֣Ҽ��̻�
//
//-----------------------------------------------------------------------------------------------
#define DA_ADJUST_BIOFFSET                      1       // �Х��ݡ��饪�ե��å�Ĵ��
#define DA_ADJUST_UNIOFFSET                     2       // ��˥ݡ��饪�ե��å�Ĵ��
#define DA_ADJUST_BIGAIN                        3       // �Х��ݡ��饲����Ĵ��
#define DA_ADJUST_UNIGAIN                       4       // ��˥ݡ��饲����Ĵ��

//-----------------------------------------------------------------------------------------------
//
//              Ĵ�����̻�
//
//-----------------------------------------------------------------------------------------------
#define DA_ADJUST_UP                            1       // ���å�
#define DA_ADJUST_DOWN                          2       // ������
#define DA_ADJUST_STORE                         3       // ���ȥ�����
#define DA_ADJUST_STANDBY                       4       // ������Х�
#define DA_ADJUST_NOT_STORE                     5       // ���ȥ����ʤ�

//-----------------------------------------------------------------------------------------------
//
//              �ǡ������̻�
//
//-----------------------------------------------------------------------------------------------
#define DA_DATA_PHYSICAL                        1       // ʪ����(�Ű�[V]/��ή[mA])
#define DA_DATA_BIN8                            2       // 8bit�Х��ʥ�
#define DA_DATA_BIN12                           3       // 12bit�Х��ʥ�
#define DA_DATA_BIN16                           4       // 16bit�Х��ʥ�
#define DA_DATA_BIN24                           5       // 24bit�Х��ʥ�

//-----------------------------------------------------------------------------------------------
//
//              �ǡ����Ѵ����̻�
//
//-----------------------------------------------------------------------------------------------
#define DA_CONV_SMOOTH                          1       // �ǡ������Ф������ࡼ���󥰤ˤ���Ѵ���Ԥ��ޤ���
#define DA_CONV_AVERAGE1                        2       // �ǡ������Ф���ñ��ʿ�Ѥˤ���Ѵ���Ԥ��ޤ���
#define DA_CONV_AVERAGE2                        3       // �ǡ������Ф�����ưʿ�Ѥˤ���Ѵ���Ԥ��ޤ���

//-----------------------------------------------------------------------------------------------
//
//              ����ץ�󥰻��꼱�̻�
//
//-----------------------------------------------------------------------------------------------
#define DA_IO_SAMPLING                          1       // I/O����
#define DA_FIFO_SAMPLING                        2       // FIFO����
#define DA_MEM_SAMPLING                         4       // ��������

//-----------------------------------------------------------------------------------------------
//
//              �ȥꥬ�ݥ���Ȼ��꼱�̻�
//
//-----------------------------------------------------------------------------------------------
#define DA_TRIG_START                           1       // �������ȥȥꥬ(�ǥե����)
#define DA_TRIG_STOP                            2       // ���ȥåץȥꥬ
#define DA_TRIG_START_STOP                      3       // �������ȥ��ȥåץȥꥬ

//-----------------------------------------------------------------------------------------------
//
//              �ȥꥬ���꼱�̻�
//
//-----------------------------------------------------------------------------------------------
#define DA_FREERUN                              1       // �ȥꥬ�ʤ�(�ǥե����)
#define DA_EXTTRG                               2       // �����ȥꥬ
#define DA_EXTTRG_DI                            3       // ����+DI�ȥꥬ

//-----------------------------------------------------------------------------------------------
//
//              �������꼱�̻�
//
//-----------------------------------------------------------------------------------------------
#define DA_DOWN_EDGE                            1       // Ω�������ꥨ�å�(�ǥե����)
#define DA_UP_EDGE                              2       // Ω���夬�ꥨ�å�

//-----------------------------------------------------------------------------------------------
//
//              �ѥ륹�������꼱�̻�
//
//-----------------------------------------------------------------------------------------------
#define DA_LOW_PULSE                            1       // LOW�ѥ륹(�ǥե����)
#define DA_HIGH_PULSE                           2       // HIGH�ѥ륹

//-----------------------------------------------------------------------------------------------
//
//              ��®�⡼�ɻ��꼱�̻�
//
//-----------------------------------------------------------------------------------------------
#define DA_NORMAL_MODE                          1       // �̾�⡼�ɤǻ��Ѥ����̤�б��ܡ��ɡ�
#define DA_FAST_MODE                            2       // ��®�⡼�ɤǻ��Ѥ���

//-----------------------------------------------------------------------------------------------
//
//              ��󥸻��꼱�̻�
//
//-----------------------------------------------------------------------------------------------
#define DA_0_1V                         0x00000001      // �Ű�����˥ݡ���0��1V
#define DA_0_2P5V                       0x00000002      // �Ű�����˥ݡ���0��2.5V
#define DA_0_5V                         0x00000004      // �Ű�����˥ݡ���0��5V
#define DA_0_10V                        0x00000008      // �Ű�����˥ݡ���0��10V
#define DA_1_5V                         0x00000010      // �Ű�����˥ݡ���1��5V
#define DA_0_2V                         0x00000020      // �Ű�����˥ݡ���0��2V
#define DA_0_0P125V                     0x00000040      // �Ű�����˥ݡ���0��0.125V
#define DA_0_1P25V                      0x00000080      // �Ű�����˥ݡ���0��1.25V
#define DA_0_0P625V                     0x00000100      // �Ű�����˥ݡ���0��0.625V
#define DA_0_20mA                       0x00001000      // ��ή����˥ݡ���0��20mA
#define DA_4_20mA                       0x00002000      // ��ή����˥ݡ���4��20mA
#define DA_1V                           0x00010000      // �Ű����Х��ݡ��� ��1V
#define DA_2P5V                         0x00020000      // �Ű����Х��ݡ��� ��2.5V
#define DA_5V                           0x00040000      // �Ű����Х��ݡ��� ��5V
#define DA_10V                          0x00080000      // �Ű����Х��ݡ��� ��10V
#define DA_0P125V                       0x00400000      // �Ű����Х��ݡ��� ��0.125V
#define DA_1P25V                        0x00800000      // �Ű����Х��ݡ��� ��1.25V
#define DA_0P625V                       0x01000000      // �Ű����Х��ݡ��� ��0.625V

//-----------------------------------------------------------------------------------------------
//
//              �����꼱�̻�
//
//-----------------------------------------------------------------------------------------------
#define DA_ISOLATION                            1       // ���ܡ���
#define DA_NOT_ISOLATION                        2       // �����ܡ���

//-----------------------------------------------------------------------------------------------
//
//              ��󥸼��̻�
//
//-----------------------------------------------------------------------------------------------
#define DA_RANGE_UNIPOLAR                       1       // ��˥ݡ���
#define DA_RANGE_BIPOLAR                        2       // �Х��ݡ���

//-----------------------------------------------------------------------------------------------
//
//              �ե��륿���꼱�̻�
//
//-----------------------------------------------------------------------------------------------
#define DA_FILTER_OFF                           1       // �ե��륿����ʤ�
#define DA_FILTER_ON                            2       // �ե��륿���ꤢ��

//-----------------------------------------------------------------------------------------------
//
//              �⡼�ɼ��̻�
//
//-----------------------------------------------------------------------------------------------
#define DA_MODE_CUT                             1       // ���åȥ⡼��
#define DA_MODE_SYNTHE                          2       // ���󥻥⡼��

//-----------------------------------------------------------------------------------------------
//
//              �����֤����̻�
//
//-----------------------------------------------------------------------------------------------
#define DA_MODE_REPEAT                          1       // �����֤�����
#define DA_MODE_SINGLE                          2       // �������

//-----------------------------------------------------------------------------------------------
//
//              �����֤����Υ��󥿡��Х뼱�̻�
//
//-----------------------------------------------------------------------------------------------
#define DA_REPEAT_NONINTERVAL                   1       // �ǡ����򷫤��֤�����
#define DA_REPEAT_INTERVAL                      2       // ��������ˤƷ����֤�����

//-----------------------------------------------------------------------------------------------
//
//              �����󥿥��ꥢ���̻�
//
//-----------------------------------------------------------------------------------------------
#define DA_COUNTER_CLEAR                        1       // ����ץ�󥰥������Ȼ��˥����󥿤򥯥ꥢ
#define DA_COUNTER_NONCLEAR                     2       // ����ץ�󥰥������Ȼ��˥����󥿤򥯥ꥢ���ʤ�

//-----------------------------------------------------------------------------------------------
//
//              DA��å���ˡ���̻�
//
//-----------------------------------------------------------------------------------------------
#define DA_LATCH_CLEAR                          1       // DA��å��򥯥ꥢ
#define DA_LATCH_NONCLEAR                       2       // DA��å��򥯥ꥢ���ʤ�

//-----------------------------------------------------------------------------------------------
//
//              ��������ץ�󥰥���å����̻�
//
//-----------------------------------------------------------------------------------------------
#define DA_CLOCK_TIMER                          1       // �����ޡʣ��������˻���
#define DA_CLOCK_FIXED                          2       // 5MHz����

//-----------------------------------------------------------------------------------------------
//
//              �������������漱�̻�
//
//-----------------------------------------------------------------------------------------------
#define DA_EXTRG_IN                             1       // �����ȥꥬ����
#define DA_EXTRG_OUT                            2       // �����ȥꥬ����
#define DA_EXINT_IN                             3       // ��������������

//-----------------------------------------------------------------------------------------------
//
//              ��������å������ϼ��̻�
//
//-----------------------------------------------------------------------------------------------
#define DA_EXCLK_IN                             1       // ��������å�����
#define DA_EXCLK_OUT                            2       // ��������å�����

//-----------------------------------------------------------------------------------------------
//
//              Ʊ�������꼱�̻�
//
//-----------------------------------------------------------------------------------------------
#define DA_MASTER_MODE                          1       // �ޥ����⡼��
#define DA_SLAVE_MODE                           2       // ���졼�֥⡼��

//-----------------------------------------------------------------------------------------------
//
//              �����ݻ����꼱�̻�
//
//-----------------------------------------------------------------------------------------------
#define DA_OUTPUT_RESET                         0       // Close�������˽��Ϥ�ꥻ�å�
#define DA_OUTPUT_MAINTAIN                      1       // Close�������˽��Ϥ��ݻ�

//-----------------------------------------------------------------------------------------------
//
//              PCI-3525 CN3/CN4 ��ǽ���꼱�̻�
//
//-----------------------------------------------------------------------------------------------
#define DA_CN_FREE                              0
#define DA_CN_EXTRG_IN                          1
#define DA_CN_EXTRG_OUT                         2
#define DA_CN_EXCLK_IN                          3
#define DA_CN_EXCLK_OUT                         4
#define DA_CN_EXINT_IN                          5
#define DA_CN_ATRG_IN                           6
#define DA_CN_DI                                7
#define DA_CN_DO                                8

//-----------------------------------------------------------------------------------------------
//
//              PCI-3525 �ȥꥬ������꼱�̻�
//
//-----------------------------------------------------------------------------------------------
#define DA_START_DOWN_EDGE                      1       // ���Ͼ�ﳰ���ȥꥬΩ�������ꥨ�å�
#define DA_START_UP_EDGE                        2       // ���Ͼ�ﳰ���ȥꥬΩ���夬�ꥨ�å�
#define DA_STOP_DOWN_EDGE                       4       // ��λ��ﳰ���ȥꥬΩ�������ꥨ�å�
#define DA_STOP_UP_EDGE                         8       // ��λ��ﳰ���ȥꥬΩ���夬�ꥨ�å�

//-----------------------------------------------------------------------------------------------
//
//              PCI-3525 �ȥꥬ������꼱�̻�
//
//-----------------------------------------------------------------------------------------------
#define DA_TRG_FREERUN                          0       // �ȥꥬ�ʤ�(�ǥե����)
#define DA_TRG_EXTTRG                           1       // �����ȥꥬ
#define DA_TRG_ATRG                             2       // ���ʥ��ȥꥬ
#define DA_TRG_SIGTIMER                         3       // ���󥿡��Х륿����
#define DA_TRG_AD_START                         5       // AD��������
#define DA_TRG_AD_STOP                          6       // AD���ȥå�
#define DA_TRG_AD_PRETRG                        7       // AD�ץ�ȥꥬ
#define DA_TRG_AD_POSTTRG                       8       // AD�ݥ��ȥȥꥬ
#define DA_TRG_SMPLNUM                          9       // DA��������λ
#define DA_TRG_FIFO_EMPTY                       10      // FIFO EMPTY
#define DA_FIFORESET                            0x0100  // FIFO�ꥻ�å�
#define DA_RETRG                                0x0200  // ��ȥꥬ

//-----------------------------------------------------------------------------------------------
//
//              Ʊ���������꼱�̻�
//
//-----------------------------------------------------------------------------------------------
#define DA_NORMAL_OUTPUT                1       // Ʊ������̵��
#define DA_SYNC_OUTPUT                  2       // Ʊ������ͭ��


//-----------------------------------------------------------------------------------------------
//
//              ���顼���̻�
//
//-----------------------------------------------------------------------------------------------
#define DA_ERROR_SUCCESS                        0x00000000
#define DA_ERROR_NOT_DEVICE                     0xC0000001
#define DA_ERROR_NOT_OPEN                       0xC0000002
#define DA_ERROR_INVALID_DEVICE_NUMBER          0xC0000003
#define DA_ERROR_ALREADY_OPEN                   0xC0000004
#define DA_ERROR_NOT_SUPPORTED                  0xC0000009
#define DA_ERROR_NOW_SAMPLING                   0xC0001001
#define DA_ERROR_STOP_SAMPLING                  0xC0001002
#define DA_ERROR_START_SAMPLING                 0xc0001003
#define DA_ERROR_SAMPLING_TIMEOUT               0xC0001004
#define DA_ERROR_INVALID_PARAMETER              0xC0001021
#define DA_ERROR_ILLEGAL_PARAMETER              0xC0001022
#define DA_ERROR_NULL_POINTER                   0xC0001023
#define DA_ERROR_SET_DATA                       0xC0001024
#define DA_ERROR_USED_AD                        0xC0001025
#define DA_ERROR_FILE_OPEN                      0xC0001041
#define DA_ERROR_FILE_CLOSE                     0xC0001042
#define DA_ERROR_FILE_READ                      0xC0001043
#define DA_ERROR_FILE_WRITE                     0xC0001044
#define DA_ERROR_INVALID_DATA_FORMAT            0xC0001061
#define DA_ERROR_INVALID_AVERAGE_OR_SMOOTHING   0xC0001062
#define DA_ERROR_INVALID_SOURCE_DATA            0xC0001063
#define DA_ERROR_NOT_ALLOCATE_MEMORY            0xC0001081
#define DA_ERROR_NOT_LOAD_DLL                   0xC0001082
#define DA_ERROR_CALL_DLL                       0xC0001083

int DaOpen(int nDevice);
int DaClose(int nDevice);
int DaCloseEx(int nDevice, int nFinalState);
int DaGetDeviceInfo(int nDevice, PDABOARDSPEC pDaBoardSpec);
int DaSetBoardConfig(int nDevice, unsigned long ulSmplBufferSize, 
                     void* pReserved, PLPDACALLBACK pEventProc, int dwUser);
int DaGetBoardConfig(int nDevice, unsigned long *ulSmplBufferSize, 
                     unsigned long  *ulSmplEventFactor);
int DaSetSamplingConfig(int nDevice, PDASMPLREQ pDaSmplConfig);
int DaGetSamplingConfig(int nDevice, PDASMPLREQ pDaSmplConfig);
int DaSetFifoConfig(int nDevice, PDAFIFOREQ pDaFifoConfig);
int DaGetFifoConfig(int nDevice, PDAFIFOREQ pDaFifoConfig);
int DaSetMode(int nDevice, PDAMODEREQ pDaMode);
int DaGetMode(int nDevice, PDAMODEREQ pDaMode);
int DaSetSamplingData(int nDevice, void * pSmplData, unsigned long ulSmplDataNum);
int DaClearSamplingData(int nDevice);
int DaStartSampling(int nDevice, unsigned long ulSyncFlag);
int DaSyncSampling(int nDevice, unsigned long ulMode);
int DaStopSampling(int nDevice);
int DaGetStatus(int nDevice, unsigned long *ulDaSmplStatus, unsigned long *ulDaSmplCount, 
                unsigned long *ulDaAvailCount, unsigned long *ulDaAvailRepeat);
int DaGetOutputMode(int nDevice, unsigned long *ulMode);
int DaSetOutputMode(int nDevice, unsigned long ulMode);
int DaOutputDA(int nDevice, unsigned long ulCh, PDASMPLCHREQ pDaSmplChReq, void * lpData);
int DaInputDI(int nDevice, unsigned long *ulData);
int DaOutputDO(int nDevice, unsigned long ulData);
int DaDataConv(unsigned long, void *, unsigned long,  PDASMPLREQ,
               unsigned long, void*, unsigned long*, PDASMPLREQ,
               unsigned long, unsigned long, PDACONVPROC);
int DaSetInterval(int nDevice, unsigned long ulInterval);
int DaGetInterval(int nDevice, unsigned long *ulInterval);
int DaSetFunction(int nDevice, unsigned int unCnNo, unsigned long ulFunction);
int DaGetFunction(int nDevice, unsigned int unCnNo, unsigned long *ulFunction);

#ifdef __RTL__
int DaOpenEx(unsigned short wType, unsigned short wSubSystemID,
             unsigned char bRSW1, int * pnDevice);
int DaSetBoardConfigEx(int nDevice, unsigned long ulSmplBufferSize, 
                       unsigned int nReserved, void * WorkBuffer);
int DaGetBoardConfigEx(int nDevice, unsigned long * ulSmplBufferSize, 
                       unsigned int * nReserved, void ** WorkBuffer);
int DaSetOutputDAEx(int nDevice, unsigned long ulCh, PDASMPLCHREQ pSmplChReq, int nMode);
int DaOutputDAEx(int nDevice, void *pData);
#else
int DaStartFileSampling(int nDevice, char* pPathName, unsigned long ulFileFlag, 
                        unsigned long ulSmplNum);
int DaWriteFile(char* , void * , unsigned long, unsigned long, unsigned long);
#endif


#ifdef __cplusplus
}
#endif

#endif // _FBIDA_H_
