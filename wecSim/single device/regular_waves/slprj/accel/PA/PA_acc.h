#ifndef RTW_HEADER_PA_acc_h_
#define RTW_HEADER_PA_acc_h_
#ifndef PA_acc_COMMON_INCLUDES_
#define PA_acc_COMMON_INCLUDES_
#include <stdlib.h>
#define S_FUNCTION_NAME simulink_only_sfcn
#define S_FUNCTION_LEVEL 2
#ifndef RTW_GENERATED_S_FUNCTION
#define RTW_GENERATED_S_FUNCTION
#endif
#include "sl_AsyncioQueue/AsyncioQueueCAPI.h"
#include "rtwtypes.h"
#include "simstruc.h"
#include "fixedpoint.h"
#endif
#include "PA_acc_types.h"
#include <stddef.h>
#include <float.h>
#include "mwmathutil.h"
#include "rt_defines.h"
#include "rt_nonfinite.h"
typedef struct { real_T B_6_0_0 [ 2 ] ; real_T B_6_2_16 [ 15 ] ; real_T
B_6_17_136 [ 6 ] ; real_T B_6_23_184 [ 6 ] ; real_T B_6_29_232 [ 6 ] ; real_T
B_6_35_280 ; real_T B_6_36_288 ; real_T B_6_37_296 [ 6 ] ; real_T B_6_43_344
[ 6 ] ; real_T B_6_49_392 [ 6 ] ; real_T B_6_55_440 [ 6 ] ; real_T B_6_61_488
[ 6 ] ; real_T B_6_67_536 [ 4 ] ; real_T B_6_71_568 [ 4 ] ; real_T B_6_75_600
[ 4 ] ; real_T B_6_79_632 [ 4 ] ; real_T B_6_83_664 [ 4 ] ; real_T B_6_87_696
[ 4 ] ; real_T B_6_91_728 [ 26 ] ; real_T B_6_117_936 [ 6 ] ; real_T
B_6_123_984 [ 6 ] ; real_T B_6_129_1032 [ 6 ] ; real_T B_6_135_1080 [ 6 ] ;
real_T B_6_141_1128 [ 6 ] ; real_T B_6_147_1176 [ 6 ] ; real_T B_6_153_1224 [
6 ] ; real_T B_6_159_1272 [ 6 ] ; real_T B_6_165_1320 [ 6 ] ; real_T
B_6_171_1368 [ 36 ] ; real_T B_6_207_1656 [ 3 ] ; real_T B_6_210_1680 [ 3 ] ;
real_T B_6_213_1704 [ 36 ] ; real_T B_6_249_1992 [ 6 ] ; real_T B_6_255_2040
[ 6 ] ; real_T B_6_261_2088 [ 36 ] ; real_T B_6_297_2376 ; real_T
B_6_298_2384 ; real_T B_6_299_2392 ; real_T B_6_300_2400 [ 6 ] ; real_T
B_6_306_2448 [ 6 ] ; real_T B_6_312_2496 ; real_T B_6_313_2504 ; real_T
B_6_314_2512 ; real_T B_6_315_2520 [ 6 ] ; real_T B_6_321_2568 ; real_T
B_6_322_2576 ; real_T B_6_323_2584 [ 6 ] ; real_T B_6_329_2632 ; real_T
B_6_330_2640 ; real_T B_6_331_2648 ; real_T B_6_332_2656 ; real_T
B_6_333_2664 ; real_T B_6_334_2672 [ 36 ] ; real_T B_6_370_2960 [ 36 ] ;
real_T B_6_406_3248 ; real_T B_6_407_3256 ; real_T B_6_408_3264 [ 6 ] ;
real_T B_6_414_3312 [ 6 ] ; real_T B_6_420_3360 [ 6 ] ; real_T B_6_426_3408 [
6 ] ; real_T B_6_432_3456 ; real_T B_6_433_3464 ; real_T B_5_434_3472 [ 6 ] ;
real_T B_5_440_3520 [ 6 ] ; real_T B_5_446_3568 [ 6 ] ; real_T B_5_452_3616 [
6 ] ; real_T B_5_458_3664 [ 6 ] ; real_T B_4_464_3712 [ 6 ] ; real_T
B_4_470_3760 [ 6 ] ; real_T B_4_476_3808 [ 6 ] ; real_T B_4_482_3856 [ 6 ] ;
real_T B_4_488_3904 [ 6 ] ; real_T B_3_494_3952 ; real_T B_2_495_3960 ;
real_T B_1_496_3968 [ 3 ] ; } B_PA_T ; typedef struct { real_T
INPUT_1_1_1_Discrete [ 2 ] ; real_T INPUT_1_1_2_Discrete [ 2 ] ; real_T
INPUT_1_1_3_Discrete [ 2 ] ; real_T INPUT_2_1_1_Discrete [ 2 ] ; real_T
INPUT_2_1_2_Discrete [ 2 ] ; real_T INPUT_2_1_3_Discrete [ 2 ] ; real_T
STATE_1_Discrete ; real_T OUTPUT_1_1_Discrete ; real_T OUTPUT_1_0_Discrete ;
struct { real_T modelTStart ; } TransportDelay_RWORK ; void *
STATE_1_Simulator ; void * STATE_1_SimData ; void * STATE_1_DiagMgr ; void *
STATE_1_ZcLogger ; void * STATE_1_TsInfo ; void * OUTPUT_1_1_Simulator ; void
* OUTPUT_1_1_SimData ; void * OUTPUT_1_1_DiagMgr ; void * OUTPUT_1_1_ZcLogger
; void * OUTPUT_1_1_TsInfo ; struct { void * TUbufferPtrs [ 12 ] ; }
TransportDelay_PWORK ; void * OUTPUT_1_0_Simulator ; void *
OUTPUT_1_0_SimData ; void * OUTPUT_1_0_DiagMgr ; void * OUTPUT_1_0_ZcLogger ;
void * OUTPUT_1_0_TsInfo ; struct { void * AQHandles ; }
_asyncqueue_inserted_for_ToWorkspace_PWORK ; struct { void * AQHandles ; }
_asyncqueue_inserted_for_ToWorkspace_PWORK_o ; void * SINK_1_RtwLogger ; void
* SINK_1_RtwLogBuffer ; void * SINK_1_RtwLogFcnManager ; struct { void *
AQHandles ; } _asyncqueue_inserted_for_ToWorkspace_PWORK_f ; struct { void *
AQHandles ; } _asyncqueue_inserted_for_ToWorkspace_PWORK_fx ; int32_T
YawKinematicTransforms_sysIdxToRun ; int32_T YawForceTransforms_sysIdxToRun ;
int32_T TmpAtomicSubsysAtSwitchInport1_sysIdxToRun ; int32_T
MATLABFunction1_sysIdxToRun ; int32_T quaternion2EulXYZ_sysIdxToRun ; int32_T
NonlinearWaveElevation_sysIdxToRun ; int_T STATE_1_Modes ; int_T
OUTPUT_1_1_Modes ; struct { int_T Tail [ 6 ] ; int_T Head [ 6 ] ; int_T Last
[ 6 ] ; int_T CircularBufSize [ 6 ] ; int_T MaxNewBufSize ; }
TransportDelay_IWORK ; int_T OUTPUT_1_0_Modes ; boolean_T STATE_1_FirstOutput
; boolean_T OUTPUT_1_1_FirstOutput ; boolean_T OUTPUT_1_0_FirstOutput ;
char_T pad_OUTPUT_1_0_FirstOutput [ 5 ] ; } DW_PA_T ; typedef struct { real_T
PATranslational_PTOTranslational_PCC_Prismatic_JointPzp [ 2 ] ; } X_PA_T ;
typedef struct { real_T
PATranslational_PTOTranslational_PCC_Prismatic_JointPzp [ 2 ] ; } XDot_PA_T ;
typedef struct { boolean_T
PATranslational_PTOTranslational_PCC_Prismatic_JointPzp [ 2 ] ; } XDis_PA_T ;
typedef struct { real_T
PATranslational_PTOTranslational_PCC_Prismatic_JointPzp [ 2 ] ; }
CStateAbsTol_PA_T ; typedef struct { real_T
PATranslational_PTOTranslational_PCC_Prismatic_JointPzp [ 2 ] ; }
CXPtMin_PA_T ; typedef struct { real_T
PATranslational_PTOTranslational_PCC_Prismatic_JointPzp [ 2 ] ; }
CXPtMax_PA_T ; struct P_PA_T_ { real_T P_0 ; real_T P_1 ; real_T P_2 ; real_T
P_3 ; real_T P_4 ; real_T P_5 ; real_T P_6 ; real_T P_7 ; real_T P_8 ; real_T
P_9 ; real_T P_10 ; real_T P_11 ; real_T P_12 ; real_T P_13 ; real_T P_14 [ 6
] ; real_T P_15 [ 6 ] ; real_T P_16 [ 6 ] ; real_T P_17 [ 36 ] ; real_T P_18
[ 3 ] ; real_T P_19 [ 3 ] ; real_T P_20 ; real_T P_21 [ 36 ] ; real_T P_22 ;
real_T P_23 ; real_T P_24 ; real_T P_25 [ 6 ] ; real_T P_26 [ 3 ] ; real_T
P_27 [ 3 ] ; real_T P_28 [ 3 ] ; real_T P_29 [ 6 ] ; real_T P_30 [ 36 ] ;
real_T P_31 ; real_T P_32 ; real_T P_33 ; real_T P_34 [ 6 ] ; real_T P_35 [ 6
] ; real_T P_36 [ 6 ] ; real_T P_37 ; real_T P_38 ; real_T P_39 ; real_T P_40
; real_T P_41 ; real_T P_42 ; real_T P_43 [ 6 ] ; real_T P_44 ; real_T P_45 ;
real_T P_46 ; real_T P_47 ; real_T P_48 ; real_T P_49 ; real_T P_50 [ 36 ] ;
real_T P_51 [ 36 ] ; real_T P_52 ; real_T P_53 ; real_T P_54 [ 6 ] ; real_T
P_55 [ 6 ] ; real_T P_56 [ 6 ] ; real_T P_57 [ 6 ] ; real_T P_58 ; real_T
P_59 ; } ; extern P_PA_T PA_rtDefaultP ;
#endif
