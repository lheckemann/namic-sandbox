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

        unsigned char   precision_0;    // 相対精度 レンジ  0-1V
        unsigned char   precision_1;    // 相対精度 レンジ  0-2.5V
        unsigned char   precision_2;    // 相対精度 レンジ  0-5V
        unsigned char   precision_3;    // 相対精度 レンジ  0-10V
        unsigned char   precision_4;    // 相対精度 レンジ  1-5V
        unsigned char   precision_5;    // 相対精度 予約
        unsigned char   precision_6;    // 相対精度 予約
        unsigned char   precision_7;    // 相対精度 予約

        unsigned char   precision_8;    // 相対精度 予約
        unsigned char   precision_9;    // 相対精度 予約
        unsigned char   precision_10;   // 相対精度 予約
        unsigned char   precision_11;   // 相対精度 予約

        unsigned char   precision_12;   // 相対精度 レンジ  0-20mA
        unsigned char   precision_13;   // 相対精度 レンジ  4-20mA
        unsigned char   precision_14;   // 相対精度 予約
        unsigned char   precision_15;   // 相対精度 予約

        unsigned char   precision_16;   // 相対精度 レンジ  ±1V
        unsigned char   precision_17;   // 相対精度 レンジ  ±2.5V
        unsigned char   precision_18;   // 相対精度 レンジ  ±5V
        unsigned char   precision_19;   // 相対精度 レンジ  ±10V
        unsigned char   precision_20;   // 相対精度 予約
        unsigned char   precision_21;   // 相対精度 予約
        unsigned char   precision_22;   // 相対精度 予約
        unsigned char   precision_23;   // 相対精度 予約

        unsigned char   precision_24;   // 相対精度 予約
        unsigned char   precision_25;   // 相対精度 予約
        unsigned char   precision_26;   // 相対精度 予約
        unsigned char   precision_27;   // 相対精度 予約        
        unsigned char   precision_28;   // 相対精度 予約
        unsigned char   precision_29;   // 相対精度 予約
        unsigned char   precision_30;   // 相対精度 予約
        unsigned char   precision_31;   // 相対精度 予約

} DA_BOARD_SPEC, *PDA_BOARD_SPEC;

/* Structure of DaDeviceFind Data */
typedef struct _DA_FIND_DEV {
        int     type;
        int     rsw;
        int     subsys;
}DA_FIND_DEV, *PDA_FIND_DEV;

//-----------------------------------------------------------------------------------------------
//
//              同期処理識別子
//
//-----------------------------------------------------------------------------------------------
#define FLAG_SYNC                               1       // 同期でサンプリング入力
#define FLAG_ASYNC                              2       // 非同期でサンプリング入力

//-----------------------------------------------------------------------------------------------
//
//              ファイル形式識別子
//
//-----------------------------------------------------------------------------------------------
#define FLAG_BIN                                1       // バイナリ形式ファイル
#define FLAG_CSV                                2       // ＣＳＶ形式ファイル

//-----------------------------------------------------------------------------------------------
//
//              データ形式識別子
//
//-----------------------------------------------------------------------------------------------
#define CONV_BIN                                1       // バイナリデータ
#define CONV_PHYS                               2       // 物理量データ

//-----------------------------------------------------------------------------------------------
//
//              サンプリング状態識別子
//
//-----------------------------------------------------------------------------------------------
#define DA_STATUS_STOP_SAMPLING                 1       // サンプリングは停止しています
#define DA_STATUS_WAIT_TRIGGER                  2       // サンプリングはトリガ待ち状態です
#define DA_STATUS_NOW_SAMPLING                  3       // サンプリング動作中です

//-----------------------------------------------------------------------------------------------
//
//              イベント要因識別子
//
//-----------------------------------------------------------------------------------------------
#define DA_EVENT_STOP_TRIGGER                   1       // トリガによるサンプリング停止
#define DA_EVENT_STOP_FUNCTION                  2       // 関数によるサンプリング停止
#define DA_EVENT_STOP_SAMPLING                  3       // サンプリング終了

//-----------------------------------------------------------------------------------------------
//
//              調整ＶＲ識別子
//
//-----------------------------------------------------------------------------------------------
#define DA_ADJUST_BIOFFSET                      1       // バイポーラオフセット調整
#define DA_ADJUST_UNIOFFSET                     2       // ユニポーラオフセット調整
#define DA_ADJUST_BIGAIN                        3       // バイポーラゲイン調整
#define DA_ADJUST_UNIGAIN                       4       // ユニポーラゲイン調整

//-----------------------------------------------------------------------------------------------
//
//              調整操作識別子
//
//-----------------------------------------------------------------------------------------------
#define DA_ADJUST_UP                            1       // アップ
#define DA_ADJUST_DOWN                          2       // ダウン
#define DA_ADJUST_STORE                         3       // ストアする
#define DA_ADJUST_STANDBY                       4       // スタンバイ
#define DA_ADJUST_NOT_STORE                     5       // ストアしない

//-----------------------------------------------------------------------------------------------
//
//              データ識別子
//
//-----------------------------------------------------------------------------------------------
#define DA_DATA_PHYSICAL                        1       // 物理量(電圧[V]/電流[mA])
#define DA_DATA_BIN8                            2       // 8bitバイナリ
#define DA_DATA_BIN12                           3       // 12bitバイナリ
#define DA_DATA_BIN16                           4       // 16bitバイナリ
#define DA_DATA_BIN24                           5       // 24bitバイナリ

//-----------------------------------------------------------------------------------------------
//
//              データ変換識別子
//
//-----------------------------------------------------------------------------------------------
#define DA_CONV_SMOOTH                          1       // データに対し、スムージングによる変換を行います。
#define DA_CONV_AVERAGE1                        2       // データに対し、単純平均による変換を行います。
#define DA_CONV_AVERAGE2                        3       // データに対し、移動平均による変換を行います。

//-----------------------------------------------------------------------------------------------
//
//              サンプリング指定識別子
//
//-----------------------------------------------------------------------------------------------
#define DA_IO_SAMPLING                          1       // I/O方式
#define DA_FIFO_SAMPLING                        2       // FIFO方式
#define DA_MEM_SAMPLING                         4       // メモリ方式

//-----------------------------------------------------------------------------------------------
//
//              トリガポイント指定識別子
//
//-----------------------------------------------------------------------------------------------
#define DA_TRIG_START                           1       // スタートトリガ(デフォルト)
#define DA_TRIG_STOP                            2       // ストップトリガ
#define DA_TRIG_START_STOP                      3       // スタートストップトリガ

//-----------------------------------------------------------------------------------------------
//
//              トリガ指定識別子
//
//-----------------------------------------------------------------------------------------------
#define DA_FREERUN                              1       // トリガなし(デフォルト)
#define DA_EXTTRG                               2       // 外部トリガ
#define DA_EXTTRG_DI                            3       // 外部+DIトリガ

//-----------------------------------------------------------------------------------------------
//
//              極性指定識別子
//
//-----------------------------------------------------------------------------------------------
#define DA_DOWN_EDGE                            1       // 立ち下がりエッジ(デフォルト)
#define DA_UP_EDGE                              2       // 立ち上がりエッジ

//-----------------------------------------------------------------------------------------------
//
//              パルス極性指定識別子
//
//-----------------------------------------------------------------------------------------------
#define DA_LOW_PULSE                            1       // LOWパルス(デフォルト)
#define DA_HIGH_PULSE                           2       // HIGHパルス

//-----------------------------------------------------------------------------------------------
//
//              倍速モード指定識別子
//
//-----------------------------------------------------------------------------------------------
#define DA_NORMAL_MODE                          1       // 通常モードで使用する（未対応ボード）
#define DA_FAST_MODE                            2       // 倍速モードで使用する

//-----------------------------------------------------------------------------------------------
//
//              レンジ指定識別子
//
//-----------------------------------------------------------------------------------------------
#define DA_0_1V                         0x00000001      // 電圧　ユニポーラ0〜1V
#define DA_0_2P5V                       0x00000002      // 電圧　ユニポーラ0〜2.5V
#define DA_0_5V                         0x00000004      // 電圧　ユニポーラ0〜5V
#define DA_0_10V                        0x00000008      // 電圧　ユニポーラ0〜10V
#define DA_1_5V                         0x00000010      // 電圧　ユニポーラ1〜5V
#define DA_0_2V                         0x00000020      // 電圧　ユニポーラ0〜2V
#define DA_0_0P125V                     0x00000040      // 電圧　ユニポーラ0〜0.125V
#define DA_0_1P25V                      0x00000080      // 電圧　ユニポーラ0〜1.25V
#define DA_0_0P625V                     0x00000100      // 電圧　ユニポーラ0〜0.625V
#define DA_0_20mA                       0x00001000      // 電流　ユニポーラ0〜20mA
#define DA_4_20mA                       0x00002000      // 電流　ユニポーラ4〜20mA
#define DA_1V                           0x00010000      // 電圧　バイポーラ ±1V
#define DA_2P5V                         0x00020000      // 電圧　バイポーラ ±2.5V
#define DA_5V                           0x00040000      // 電圧　バイポーラ ±5V
#define DA_10V                          0x00080000      // 電圧　バイポーラ ±10V
#define DA_0P125V                       0x00400000      // 電圧　バイポーラ ±0.125V
#define DA_1P25V                        0x00800000      // 電圧　バイポーラ ±1.25V
#define DA_0P625V                       0x01000000      // 電圧　バイポーラ ±0.625V

//-----------------------------------------------------------------------------------------------
//
//              絶縁指定識別子
//
//-----------------------------------------------------------------------------------------------
#define DA_ISOLATION                            1       // 絶縁ボード
#define DA_NOT_ISOLATION                        2       // 非絶縁ボード

//-----------------------------------------------------------------------------------------------
//
//              レンジ識別子
//
//-----------------------------------------------------------------------------------------------
#define DA_RANGE_UNIPOLAR                       1       // ユニポーラ
#define DA_RANGE_BIPOLAR                        2       // バイポーラ

//-----------------------------------------------------------------------------------------------
//
//              フィルタ設定識別子
//
//-----------------------------------------------------------------------------------------------
#define DA_FILTER_OFF                           1       // フィルタ設定なし
#define DA_FILTER_ON                            2       // フィルタ設定あり

//-----------------------------------------------------------------------------------------------
//
//              モード識別子
//
//-----------------------------------------------------------------------------------------------
#define DA_MODE_CUT                             1       // カットモード
#define DA_MODE_SYNTHE                          2       // シンセモード

//-----------------------------------------------------------------------------------------------
//
//              繰り返し識別子
//
//-----------------------------------------------------------------------------------------------
#define DA_MODE_REPEAT                          1       // 繰り返し出力
#define DA_MODE_SINGLE                          2       // １回出力

//-----------------------------------------------------------------------------------------------
//
//              繰り返し時のインターバル識別子
//
//-----------------------------------------------------------------------------------------------
#define DA_REPEAT_NONINTERVAL                   1       // データを繰り返し出力
#define DA_REPEAT_INTERVAL                      2       // 一定周期にて繰り返し出力

//-----------------------------------------------------------------------------------------------
//
//              カウンタクリア識別子
//
//-----------------------------------------------------------------------------------------------
#define DA_COUNTER_CLEAR                        1       // サンプリングスタート時にカウンタをクリア
#define DA_COUNTER_NONCLEAR                     2       // サンプリングスタート時にカウンタをクリアしない

//-----------------------------------------------------------------------------------------------
//
//              DAラッチ方法識別子
//
//-----------------------------------------------------------------------------------------------
#define DA_LATCH_CLEAR                          1       // DAラッチをクリア
#define DA_LATCH_NONCLEAR                       2       // DAラッチをクリアしない

//-----------------------------------------------------------------------------------------------
//
//              内部サンプリングクロック識別子
//
//-----------------------------------------------------------------------------------------------
#define DA_CLOCK_TIMER                          1       // タイマ（８２５４）使用
#define DA_CLOCK_FIXED                          2       // 5MHz固定

//-----------------------------------------------------------------------------------------------
//
//              外部入出力制御識別子
//
//-----------------------------------------------------------------------------------------------
#define DA_EXTRG_IN                             1       // 外部トリガ入力
#define DA_EXTRG_OUT                            2       // 外部トリガ出力
#define DA_EXINT_IN                             3       // 外部割り込み入力

//-----------------------------------------------------------------------------------------------
//
//              外部クロック入出力識別子
//
//-----------------------------------------------------------------------------------------------
#define DA_EXCLK_IN                             1       // 外部クロック入力
#define DA_EXCLK_OUT                            2       // 外部クロック出力

//-----------------------------------------------------------------------------------------------
//
//              同期化設定識別子
//
//-----------------------------------------------------------------------------------------------
#define DA_MASTER_MODE                          1       // マスタモード
#define DA_SLAVE_MODE                           2       // スレーブモード

//-----------------------------------------------------------------------------------------------
//
//              出力保持設定識別子
//
//-----------------------------------------------------------------------------------------------
#define DA_OUTPUT_RESET                         0       // Close処理時に出力をリセット
#define DA_OUTPUT_MAINTAIN                      1       // Close処理時に出力を保持

//-----------------------------------------------------------------------------------------------
//
//              PCI-3525 CN3/CN4 機能設定識別子
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
//              PCI-3525 トリガ条件設定識別子
//
//-----------------------------------------------------------------------------------------------
#define DA_START_DOWN_EDGE                      1       // 開始条件外部トリガ立ち下がりエッジ
#define DA_START_UP_EDGE                        2       // 開始条件外部トリガ立ち上がりエッジ
#define DA_STOP_DOWN_EDGE                       4       // 終了条件外部トリガ立ち下がりエッジ
#define DA_STOP_UP_EDGE                         8       // 終了条件外部トリガ立ち上がりエッジ

//-----------------------------------------------------------------------------------------------
//
//              PCI-3525 トリガ条件設定識別子
//
//-----------------------------------------------------------------------------------------------
#define DA_TRG_FREERUN                          0       // トリガなし(デフォルト)
#define DA_TRG_EXTTRG                           1       // 外部トリガ
#define DA_TRG_ATRG                             2       // アナログトリガ
#define DA_TRG_SIGTIMER                         3       // インターバルタイマ
#define DA_TRG_AD_START                         5       // ADスタート
#define DA_TRG_AD_STOP                          6       // ADストップ
#define DA_TRG_AD_PRETRG                        7       // ADプリトリガ
#define DA_TRG_AD_POSTTRG                       8       // ADポストトリガ
#define DA_TRG_SMPLNUM                          9       // DA指定件数終了
#define DA_TRG_FIFO_EMPTY                       10      // FIFO EMPTY
#define DA_FIFORESET                            0x0100  // FIFOリセット
#define DA_RETRG                                0x0200  // リトリガ

//-----------------------------------------------------------------------------------------------
//
//              同時出力設定識別子
//
//-----------------------------------------------------------------------------------------------
#define DA_NORMAL_OUTPUT                1       // 同時出力無向
#define DA_SYNC_OUTPUT                  2       // 同時出力有向


//-----------------------------------------------------------------------------------------------
//
//              エラー識別子
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
