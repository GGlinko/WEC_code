#include "PA_acc.h"
#include "PA_acc_types.h"
#include "mwmathutil.h"
#include <emmintrin.h>
#include "rtwtypes.h"
#include <string.h>
#include "PA_acc_private.h"
#include "multiword_types.h"
#include <stdio.h>
#include "slexec_vm_simstruct_bridge.h"
#include "slexec_vm_zc_functions.h"
#include "slexec_vm_lookup_functions.h"
#include "slsv_diagnostic_codegen_c_api.h"
#include "simtarget/slSimTgtMdlrefSfcnBridge.h"
#include "simstruc.h"
#include "fixedpoint.h"
#define CodeFormat S-Function
#define AccDefine1 Accelerator_S-Function
#include "simtarget/slAccSfcnBridge.h"
#ifndef __RTW_UTFREE__  
extern void * utMalloc ( size_t ) ; extern void utFree ( void * ) ;
#endif
boolean_T PA_acc_rt_TDelayUpdateTailOrGrowBuf ( int_T * bufSzPtr , int_T *
tailPtr , int_T * headPtr , int_T * lastPtr , real_T tMinusDelay , real_T * *
uBufPtr , boolean_T isfixedbuf , boolean_T istransportdelay , int_T *
maxNewBufSzPtr ) { int_T testIdx ; int_T tail = * tailPtr ; int_T bufSz = *
bufSzPtr ; real_T * tBuf = * uBufPtr + bufSz ; real_T * xBuf = ( NULL ) ;
int_T numBuffer = 2 ; if ( istransportdelay ) { numBuffer = 3 ; xBuf = *
uBufPtr + 2 * bufSz ; } testIdx = ( tail < ( bufSz - 1 ) ) ? ( tail + 1 ) : 0
; if ( ( tMinusDelay <= tBuf [ testIdx ] ) && ! isfixedbuf ) { int_T j ;
real_T * tempT ; real_T * tempU ; real_T * tempX = ( NULL ) ; real_T * uBuf =
* uBufPtr ; int_T newBufSz = bufSz + 1024 ; if ( newBufSz > * maxNewBufSzPtr
) { * maxNewBufSzPtr = newBufSz ; } tempU = ( real_T * ) utMalloc ( numBuffer
* newBufSz * sizeof ( real_T ) ) ; if ( tempU == ( NULL ) ) { return ( false
) ; } tempT = tempU + newBufSz ; if ( istransportdelay ) tempX = tempT +
newBufSz ; for ( j = tail ; j < bufSz ; j ++ ) { tempT [ j - tail ] = tBuf [
j ] ; tempU [ j - tail ] = uBuf [ j ] ; if ( istransportdelay ) tempX [ j -
tail ] = xBuf [ j ] ; } for ( j = 0 ; j < tail ; j ++ ) { tempT [ j + bufSz -
tail ] = tBuf [ j ] ; tempU [ j + bufSz - tail ] = uBuf [ j ] ; if (
istransportdelay ) tempX [ j + bufSz - tail ] = xBuf [ j ] ; } if ( * lastPtr
> tail ) { * lastPtr -= tail ; } else { * lastPtr += ( bufSz - tail ) ; } *
tailPtr = 0 ; * headPtr = bufSz ; utFree ( uBuf ) ; * bufSzPtr = newBufSz ; *
uBufPtr = tempU ; } else { * tailPtr = testIdx ; } return ( true ) ; } real_T
PA_acc_rt_TDelayInterpolate ( real_T tMinusDelay , real_T tStart , real_T *
uBuf , int_T bufSz , int_T * lastIdx , int_T oldestIdx , int_T newIdx ,
real_T initOutput , boolean_T discrete , boolean_T
minorStepAndTAtLastMajorOutput ) { int_T i ; real_T yout , t1 , t2 , u1 , u2
; real_T * tBuf = uBuf + bufSz ; if ( ( newIdx == 0 ) && ( oldestIdx == 0 )
&& ( tMinusDelay > tStart ) ) return initOutput ; if ( tMinusDelay <= tStart
) return initOutput ; if ( ( tMinusDelay <= tBuf [ oldestIdx ] ) ) { if (
discrete ) { return ( uBuf [ oldestIdx ] ) ; } else { int_T tempIdx =
oldestIdx + 1 ; if ( oldestIdx == bufSz - 1 ) tempIdx = 0 ; t1 = tBuf [
oldestIdx ] ; t2 = tBuf [ tempIdx ] ; u1 = uBuf [ oldestIdx ] ; u2 = uBuf [
tempIdx ] ; if ( t2 == t1 ) { if ( tMinusDelay >= t2 ) { yout = u2 ; } else {
yout = u1 ; } } else { real_T f1 = ( t2 - tMinusDelay ) / ( t2 - t1 ) ;
real_T f2 = 1.0 - f1 ; yout = f1 * u1 + f2 * u2 ; } return yout ; } } if (
minorStepAndTAtLastMajorOutput ) { if ( newIdx != 0 ) { if ( * lastIdx ==
newIdx ) { ( * lastIdx ) -- ; } newIdx -- ; } else { if ( * lastIdx == newIdx
) { * lastIdx = bufSz - 1 ; } newIdx = bufSz - 1 ; } } i = * lastIdx ; if (
tBuf [ i ] < tMinusDelay ) { while ( tBuf [ i ] < tMinusDelay ) { if ( i ==
newIdx ) break ; i = ( i < ( bufSz - 1 ) ) ? ( i + 1 ) : 0 ; } } else { while
( tBuf [ i ] >= tMinusDelay ) { i = ( i > 0 ) ? i - 1 : ( bufSz - 1 ) ; } i =
( i < ( bufSz - 1 ) ) ? ( i + 1 ) : 0 ; } * lastIdx = i ; if ( discrete ) {
double tempEps = ( DBL_EPSILON ) * 128.0 ; double localEps = tempEps *
muDoubleScalarAbs ( tBuf [ i ] ) ; if ( tempEps > localEps ) { localEps =
tempEps ; } localEps = localEps / 2.0 ; if ( tMinusDelay >= ( tBuf [ i ] -
localEps ) ) { yout = uBuf [ i ] ; } else { if ( i == 0 ) { yout = uBuf [
bufSz - 1 ] ; } else { yout = uBuf [ i - 1 ] ; } } } else { if ( i == 0 ) {
t1 = tBuf [ bufSz - 1 ] ; u1 = uBuf [ bufSz - 1 ] ; } else { t1 = tBuf [ i -
1 ] ; u1 = uBuf [ i - 1 ] ; } t2 = tBuf [ i ] ; u2 = uBuf [ i ] ; if ( t2 ==
t1 ) { if ( tMinusDelay >= t2 ) { yout = u2 ; } else { yout = u1 ; } } else {
real_T f1 = ( t2 - tMinusDelay ) / ( t2 - t1 ) ; real_T f2 = 1.0 - f1 ; yout
= f1 * u1 + f2 * u2 ; } } return ( yout ) ; } void rt_ssGetBlockPath (
SimStruct * S , int_T sysIdx , int_T blkIdx , char_T * * path ) {
_ssGetBlockPath ( S , sysIdx , blkIdx , path ) ; } void rt_ssSet_slErrMsg (
void * S , void * diag ) { SimStruct * castedS = ( SimStruct * ) S ; if ( !
_ssIsErrorStatusAslErrMsg ( castedS ) ) { _ssSet_slErrMsg ( castedS , diag )
; } else { _ssDiscardDiagnostic ( castedS , diag ) ; } } void
rt_ssReportDiagnosticAsWarning ( void * S , void * diag ) {
_ssReportDiagnosticAsWarning ( ( SimStruct * ) S , diag ) ; } void
rt_ssReportDiagnosticAsInfo ( void * S , void * diag ) {
_ssReportDiagnosticAsInfo ( ( SimStruct * ) S , diag ) ; } static void
mdlOutputs ( SimStruct * S , int_T tid ) { __m128d tmp_1 ; __m128d tmp_2 ;
B_PA_T * _rtB ; DW_PA_T * _rtDW ; P_PA_T * _rtP ; real_T rtb_B_6_596_4768 [
60 ] ; real_T rtb_B_6_656_5248 [ 48 ] ; real_T rtb_B_6_572_4576 [ 24 ] ;
real_T B_6_710_5680 [ 6 ] ; real_T rtb_B_6_710_5680 [ 6 ] ; real_T tmp [ 6 ]
; real_T tmp_0 [ 2 ] ; real_T B_6_29_232_0 ; real_T rtb_B_6_499_3992 ; real_T
rtb_B_6_500_4000 ; real_T rtb_B_6_501_4008 ; real_T rtb_B_6_502_4016 ;
int32_T i ; int32_T isHit ; _rtDW = ( ( DW_PA_T * ) ssGetRootDWork ( S ) ) ;
_rtP = ( ( P_PA_T * ) ssGetModelRtp ( S ) ) ; _rtB = ( ( B_PA_T * )
_ssGetModelBlockIO ( S ) ) ; ssCallAccelRunBlock ( S , 6 , 0 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 6 , 1 , SS_CALL_MDL_OUTPUTS
) ; ssCallAccelRunBlock ( S , 1 , 0 , SS_CALL_MDL_OUTPUTS ) ; { static const
boolean_T isDiscrete [ ] = { false , false , false , false , false , false }
; real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK .
TUbufferPtrs [ 0 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay ; {
int_T i1 ; real_T * y0 = & _rtB -> B_6_17_136 [ 0 ] ; int_T * iw_Tail = &
_rtDW -> TransportDelay_IWORK . Tail [ 0 ] ; int_T * iw_Head = & _rtDW ->
TransportDelay_IWORK . Head [ 0 ] ; int_T * iw_Last = & _rtDW ->
TransportDelay_IWORK . Last [ 0 ] ; int_T * iw_CircularBufSize = & _rtDW ->
TransportDelay_IWORK . CircularBufSize [ 0 ] ; for ( i1 = 0 ; i1 < 3 ; i1 ++
) { tMinusDelay = ( ( _rtP -> P_4 > 0.0 ) ? _rtP -> P_4 : 0.0 ) ; tMinusDelay
= simTime - tMinusDelay ; y0 [ i1 ] = PA_acc_rt_TDelayInterpolate (
tMinusDelay , 0.0 , * uBuffer , iw_CircularBufSize [ i1 ] , & iw_Last [ i1 ]
, iw_Tail [ i1 ] , iw_Head [ i1 ] , _rtP -> P_5 , isDiscrete [ i1 ] , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ( * uBuffer + iw_CircularBufSize [
i1 ] ) [ iw_Head [ i1 ] ] == ssGetT ( S ) ) ) ) ; uBuffer ++ ; } y0 = & _rtB
-> B_6_17_136 [ 3 ] ; iw_Tail = & _rtDW -> TransportDelay_IWORK . Tail [ 3 ]
; iw_Head = & _rtDW -> TransportDelay_IWORK . Head [ 3 ] ; iw_Last = & _rtDW
-> TransportDelay_IWORK . Last [ 3 ] ; iw_CircularBufSize = & _rtDW ->
TransportDelay_IWORK . CircularBufSize [ 3 ] ; for ( i1 = 0 ; i1 < 3 ; i1 ++
) { tMinusDelay = ( ( _rtP -> P_4 > 0.0 ) ? _rtP -> P_4 : 0.0 ) ; tMinusDelay
= simTime - tMinusDelay ; y0 [ i1 ] = PA_acc_rt_TDelayInterpolate (
tMinusDelay , 0.0 , * uBuffer , iw_CircularBufSize [ i1 ] , & iw_Last [ i1 ]
, iw_Tail [ i1 ] , iw_Head [ i1 ] , _rtP -> P_5 , isDiscrete [ i1 + 3 ] , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ( * uBuffer + iw_CircularBufSize [
i1 ] ) [ iw_Head [ i1 ] ] == ssGetT ( S ) ) ) ) ; uBuffer ++ ; } } } _rtB ->
B_5_434_3472 [ 0 ] = _rtB -> B_6_2_16 [ 9 ] ; _rtB -> B_5_434_3472 [ 3 ] =
_rtB -> B_1_496_3968 [ 0 ] ; _rtB -> B_5_440_3520 [ 0 ] = _rtB -> B_6_2_16 [
12 ] ; _rtB -> B_5_440_3520 [ 3 ] = _rtB -> B_6_2_16 [ 6 ] ; _rtB ->
B_5_434_3472 [ 1 ] = _rtB -> B_6_2_16 [ 10 ] ; _rtB -> B_5_434_3472 [ 4 ] =
_rtB -> B_1_496_3968 [ 1 ] ; _rtB -> B_5_440_3520 [ 1 ] = _rtB -> B_6_2_16 [
13 ] ; _rtB -> B_5_440_3520 [ 4 ] = _rtB -> B_6_2_16 [ 7 ] ; _rtB ->
B_5_434_3472 [ 2 ] = _rtB -> B_6_2_16 [ 11 ] ; _rtB -> B_5_434_3472 [ 5 ] =
_rtB -> B_1_496_3968 [ 2 ] ; _rtB -> B_5_440_3520 [ 2 ] = _rtB -> B_6_2_16 [
14 ] ; _rtB -> B_5_440_3520 [ 5 ] = _rtB -> B_6_2_16 [ 8 ] ;
ssCallAccelRunBlock ( S , 5 , 2 , SS_CALL_MDL_OUTPUTS ) ; for ( isHit = 0 ;
isHit < 6 ; isHit ++ ) { rtb_B_6_499_3992 = 0.0 ; B_6_29_232_0 = 0.0 ; for (
i = 0 ; i < 6 ; i ++ ) { rtb_B_6_499_3992 += _rtB -> B_6_334_2672 [ 6 * i +
isHit ] * _rtB -> B_5_452_3616 [ i ] ; B_6_29_232_0 += _rtB -> B_6_370_2960 [
6 * i + isHit ] * _rtB -> B_5_458_3664 [ i ] ; } _rtB -> B_6_29_232 [ isHit ]
= B_6_29_232_0 ; _rtB -> B_6_23_184 [ isHit ] = rtb_B_6_499_3992 ; }
rtb_B_6_499_3992 = ssGetT ( S ) ; if ( rtb_B_6_499_3992 < _rtB ->
B_6_329_2632 ) { _rtB -> B_3_494_3952 = rtb_B_6_499_3992 * _rtB ->
B_6_333_2664 + _rtB -> B_6_332_2656 ; rtb_B_6_500_4000 = ( (
muDoubleScalarSin ( _rtP -> P_2 * _rtB -> B_3_494_3952 + _rtP -> P_3 ) * _rtP
-> P_0 + _rtP -> P_1 ) + _rtB -> B_6_331_2648 ) * _rtB -> B_6_330_2640 ; }
else { rtb_B_6_500_4000 = _rtB -> B_6_297_2376 ; } rtb_B_6_501_4008 =
rtb_B_6_499_3992 * _rtB -> B_6_322_2576 ; ssCallAccelRunBlock ( S , 2 , 0 ,
SS_CALL_MDL_OUTPUTS ) ; _rtB -> B_6_35_280 = ( rtb_B_6_501_4008 + _rtB ->
B_6_298_2384 ) + _rtB -> B_2_495_3960 ; rtb_B_6_502_4016 = muDoubleScalarSin
( _rtP -> P_8 * _rtB -> B_6_35_280 + _rtP -> P_9 ) * _rtP -> P_6 + _rtP ->
P_7 ; _rtB -> B_6_36_288 = rtb_B_6_501_4008 + _rtB -> B_2_495_3960 ;
rtb_B_6_501_4008 = muDoubleScalarSin ( _rtP -> P_12 * _rtB -> B_6_36_288 +
_rtP -> P_13 ) * _rtP -> P_10 + _rtP -> P_11 ; tmp [ 0 ] = _rtB ->
B_5_446_3568 [ 0 ] - _rtB -> B_6_207_1656 [ 0 ] ; tmp [ 3 ] = _rtB ->
B_5_446_3568 [ 3 ] - _rtB -> B_6_210_1680 [ 0 ] ; tmp [ 1 ] = _rtB ->
B_5_446_3568 [ 1 ] - _rtB -> B_6_207_1656 [ 1 ] ; tmp [ 4 ] = _rtB ->
B_5_446_3568 [ 4 ] - _rtB -> B_6_210_1680 [ 1 ] ; tmp [ 2 ] = _rtB ->
B_5_446_3568 [ 2 ] - _rtB -> B_6_207_1656 [ 2 ] ; tmp [ 5 ] = _rtB ->
B_5_446_3568 [ 5 ] - _rtB -> B_6_210_1680 [ 2 ] ; for ( i = 0 ; i < 6 ; i ++
) { _rtB -> B_6_37_296 [ i ] = ( ( ( _rtB -> B_6_321_2568 * _rtB ->
B_6_306_2448 [ i ] * rtb_B_6_502_4016 + _rtB -> B_6_315_2520 [ i ] ) - _rtB
-> B_6_321_2568 * rtb_B_6_501_4008 * _rtB -> B_6_300_2400 [ i ] ) + _rtB ->
B_6_323_2584 [ i ] ) * rtb_B_6_500_4000 ; B_6_29_232_0 = 0.0 ; for ( isHit =
0 ; isHit < 6 ; isHit ++ ) { B_6_29_232_0 += _rtB -> B_6_213_1704 [ 6 * isHit
+ i ] * tmp [ isHit ] ; } _rtB -> B_6_43_344 [ i ] = _rtB -> B_6_249_1992 [ i
] + B_6_29_232_0 ; } _rtB -> B_4_464_3712 [ 0 ] = _rtB -> B_6_2_16 [ 9 ] ;
_rtB -> B_4_464_3712 [ 3 ] = _rtB -> B_1_496_3968 [ 0 ] ; _rtB ->
B_4_464_3712 [ 1 ] = _rtB -> B_6_2_16 [ 10 ] ; _rtB -> B_4_464_3712 [ 4 ] =
_rtB -> B_1_496_3968 [ 1 ] ; _rtB -> B_4_464_3712 [ 2 ] = _rtB -> B_6_2_16 [
11 ] ; _rtB -> B_4_464_3712 [ 5 ] = _rtB -> B_1_496_3968 [ 2 ] ;
ssCallAccelRunBlock ( S , 4 , 1 , SS_CALL_MDL_OUTPUTS ) ; rtb_B_6_710_5680 [
0 ] = _rtB -> B_6_2_16 [ 12 ] ; rtb_B_6_710_5680 [ 3 ] = _rtB -> B_6_2_16 [ 6
] ; rtb_B_6_710_5680 [ 1 ] = _rtB -> B_6_2_16 [ 13 ] ; rtb_B_6_710_5680 [ 4 ]
= _rtB -> B_6_2_16 [ 7 ] ; rtb_B_6_710_5680 [ 2 ] = _rtB -> B_6_2_16 [ 14 ] ;
rtb_B_6_710_5680 [ 5 ] = _rtB -> B_6_2_16 [ 8 ] ; for ( isHit = 0 ; isHit <=
4 ; isHit += 2 ) { tmp_0 [ 0 ] = muDoubleScalarAbs ( rtb_B_6_710_5680 [ isHit
] ) ; tmp_0 [ 1 ] = muDoubleScalarAbs ( rtb_B_6_710_5680 [ isHit + 1 ] ) ;
tmp_1 = _mm_loadu_pd ( & rtb_B_6_710_5680 [ isHit ] ) ; tmp_2 = _mm_loadu_pd
( & tmp_0 [ 0 ] ) ; _mm_storeu_pd ( & B_6_710_5680 [ isHit ] , _mm_mul_pd (
tmp_1 , tmp_2 ) ) ; } for ( i = 0 ; i < 6 ; i ++ ) { B_6_29_232_0 = 0.0 ;
rtb_B_6_500_4000 = 0.0 ; for ( isHit = 0 ; isHit < 6 ; isHit ++ ) {
B_6_29_232_0 += _rtB -> B_6_261_2088 [ 6 * isHit + i ] * B_6_710_5680 [ isHit
] ; rtb_B_6_500_4000 += _rtB -> B_6_171_1368 [ 6 * isHit + i ] *
rtb_B_6_710_5680 [ isHit ] ; } _rtB -> B_6_55_440 [ i ] = rtb_B_6_500_4000 ;
B_6_29_232_0 -= _rtB -> B_6_255_2040 [ i ] ; _rtB -> B_6_49_392 [ i ] =
B_6_29_232_0 ; _rtB -> B_6_61_488 [ i ] = ( ( ( ( _rtB -> B_4_470_3760 [ i ]
- _rtB -> B_4_476_3808 [ i ] ) - _rtB -> B_4_482_3856 [ i ] ) - _rtB ->
B_4_488_3904 [ i ] ) - B_6_29_232_0 ) - rtb_B_6_500_4000 ; } _rtB ->
B_6_67_536 [ 0 ] = _rtB -> B_6_61_488 [ 0 ] ; _rtB -> B_6_67_536 [ 1 ] = 0.0
; _rtB -> B_6_67_536 [ 2 ] = 0.0 ; _rtB -> B_6_67_536 [ 3 ] = 0.0 ; _rtB ->
B_6_71_568 [ 0 ] = _rtB -> B_6_61_488 [ 1 ] ; _rtB -> B_6_71_568 [ 1 ] = 0.0
; _rtB -> B_6_71_568 [ 2 ] = 0.0 ; _rtB -> B_6_71_568 [ 3 ] = 0.0 ; _rtB ->
B_6_75_600 [ 0 ] = _rtB -> B_6_61_488 [ 2 ] ; _rtB -> B_6_75_600 [ 1 ] = 0.0
; _rtB -> B_6_75_600 [ 2 ] = 0.0 ; _rtB -> B_6_75_600 [ 3 ] = 0.0 ; _rtB ->
B_6_79_632 [ 0 ] = _rtB -> B_6_61_488 [ 3 ] ; _rtB -> B_6_79_632 [ 1 ] = 0.0
; _rtB -> B_6_79_632 [ 2 ] = 0.0 ; _rtB -> B_6_79_632 [ 3 ] = 0.0 ; _rtB ->
B_6_83_664 [ 0 ] = _rtB -> B_6_61_488 [ 4 ] ; _rtB -> B_6_83_664 [ 1 ] = 0.0
; _rtB -> B_6_83_664 [ 2 ] = 0.0 ; _rtB -> B_6_83_664 [ 3 ] = 0.0 ; _rtB ->
B_6_87_696 [ 0 ] = _rtB -> B_6_61_488 [ 5 ] ; _rtB -> B_6_87_696 [ 1 ] = 0.0
; _rtB -> B_6_87_696 [ 2 ] = 0.0 ; _rtB -> B_6_87_696 [ 3 ] = 0.0 ;
ssCallAccelRunBlock ( S , 6 , 39 , SS_CALL_MDL_OUTPUTS ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { for ( i = 0 ; i < 6 ; i ++
) { rtb_B_6_572_4576 [ i ] = _rtB -> B_6_153_1224 [ i ] ; rtb_B_6_572_4576 [
i + 6 ] = _rtB -> B_6_159_1272 [ i ] ; rtb_B_6_572_4576 [ i + 12 ] = _rtB ->
B_6_165_1320 [ i ] ; rtb_B_6_572_4576 [ i + 18 ] = _rtB -> B_6_91_728 [ i ] ;
} { if ( _rtDW -> _asyncqueue_inserted_for_ToWorkspace_PWORK . AQHandles &&
ssGetLogOutput ( S ) ) { sdiWriteSignal ( _rtDW ->
_asyncqueue_inserted_for_ToWorkspace_PWORK . AQHandles , ssGetTaskTime ( S ,
1 ) , ( char * ) & rtb_B_6_572_4576 [ 0 ] + 0 ) ; } } } { if ( _rtDW ->
_asyncqueue_inserted_for_ToWorkspace_PWORK_o . AQHandles && ssGetLogOutput (
S ) ) { sdiWriteSignal ( _rtDW ->
_asyncqueue_inserted_for_ToWorkspace_PWORK_o . AQHandles , ssGetTaskTime ( S
, 0 ) , ( char * ) & rtb_B_6_499_3992 + 0 ) ; } } ssCallAccelRunBlock ( S , 6
, 52 , SS_CALL_MDL_OUTPUTS ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { rtb_B_6_596_4768 [ 0 ] = _rtB -> B_6_2_16 [ 9 ] ;
rtb_B_6_596_4768 [ 3 ] = _rtB -> B_1_496_3968 [ 0 ] ; rtb_B_6_596_4768 [ 6 ]
= _rtB -> B_6_2_16 [ 12 ] ; rtb_B_6_596_4768 [ 9 ] = _rtB -> B_6_2_16 [ 6 ] ;
rtb_B_6_596_4768 [ 12 ] = _rtB -> B_6_91_728 [ 23 ] ; rtb_B_6_596_4768 [ 15 ]
= _rtB -> B_6_91_728 [ 20 ] ; rtb_B_6_596_4768 [ 1 ] = _rtB -> B_6_2_16 [ 10
] ; rtb_B_6_596_4768 [ 4 ] = _rtB -> B_1_496_3968 [ 1 ] ; rtb_B_6_596_4768 [
7 ] = _rtB -> B_6_2_16 [ 13 ] ; rtb_B_6_596_4768 [ 10 ] = _rtB -> B_6_2_16 [
7 ] ; rtb_B_6_596_4768 [ 13 ] = _rtB -> B_6_91_728 [ 24 ] ; rtb_B_6_596_4768
[ 16 ] = _rtB -> B_6_91_728 [ 21 ] ; rtb_B_6_596_4768 [ 2 ] = _rtB ->
B_6_2_16 [ 11 ] ; rtb_B_6_596_4768 [ 5 ] = _rtB -> B_1_496_3968 [ 2 ] ;
rtb_B_6_596_4768 [ 8 ] = _rtB -> B_6_2_16 [ 14 ] ; rtb_B_6_596_4768 [ 11 ] =
_rtB -> B_6_2_16 [ 8 ] ; rtb_B_6_596_4768 [ 14 ] = _rtB -> B_6_91_728 [ 25 ]
; rtb_B_6_596_4768 [ 17 ] = _rtB -> B_6_91_728 [ 22 ] ; for ( i = 0 ; i < 6 ;
i ++ ) { rtb_B_6_596_4768 [ i + 18 ] = _rtB -> B_6_61_488 [ i ] ;
rtb_B_6_596_4768 [ i + 24 ] = _rtB -> B_4_470_3760 [ i ] ; rtb_B_6_596_4768 [
i + 30 ] = _rtB -> B_4_476_3808 [ i ] ; rtb_B_6_596_4768 [ i + 36 ] = _rtB ->
B_4_482_3856 [ i ] ; rtb_B_6_596_4768 [ i + 42 ] = _rtB -> B_4_488_3904 [ i ]
; rtb_B_6_596_4768 [ i + 48 ] = _rtB -> B_6_49_392 [ i ] ; rtb_B_6_596_4768 [
i + 54 ] = _rtB -> B_6_55_440 [ i ] ; } { if ( _rtDW ->
_asyncqueue_inserted_for_ToWorkspace_PWORK_f . AQHandles && ssGetLogOutput (
S ) ) { sdiWriteSignal ( _rtDW ->
_asyncqueue_inserted_for_ToWorkspace_PWORK_f . AQHandles , ssGetTaskTime ( S
, 1 ) , ( char * ) & rtb_B_6_596_4768 [ 0 ] + 0 ) ; } } } for ( i = 0 ; i < 6
; i ++ ) { _rtB -> B_6_117_936 [ i ] = _rtB -> B_6_408_3264 [ i ] ; _rtB ->
B_6_123_984 [ i ] = _rtB -> B_6_414_3312 [ i ] ; _rtB -> B_6_129_1032 [ i ] =
_rtB -> B_6_420_3360 [ i ] ; _rtB -> B_6_135_1080 [ i ] = _rtB ->
B_6_426_3408 [ i ] ; } _rtB -> B_6_117_936 [ 2 ] = _rtB -> B_6_2_16 [ 0 ] ;
_rtB -> B_6_123_984 [ 2 ] = _rtB -> B_6_2_16 [ 1 ] ; _rtB -> B_6_129_1032 [ 2
] = _rtB -> B_6_91_728 [ 6 ] ; _rtB -> B_6_135_1080 [ 2 ] = _rtB ->
B_6_91_728 [ 7 ] ; for ( i = 0 ; i <= 4 ; i += 2 ) { tmp_1 = _mm_loadu_pd ( &
_rtB -> B_6_117_936 [ i ] ) ; tmp_2 = _mm_loadu_pd ( & _rtB -> B_6_123_984 [
i ] ) ; tmp_1 = _mm_sub_pd ( _mm_sub_pd ( _mm_set1_pd ( 0.0 ) , _mm_mul_pd (
tmp_1 , _mm_set1_pd ( _rtB -> B_6_433_3464 ) ) ) , _mm_mul_pd ( _mm_set1_pd (
_rtB -> B_6_432_3456 ) , tmp_2 ) ) ; _mm_storeu_pd ( & _rtB -> B_6_141_1128 [
i ] , tmp_1 ) ; _mm_storeu_pd ( & _rtB -> B_6_147_1176 [ i ] , _mm_mul_pd (
tmp_1 , tmp_2 ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 )
{ for ( i = 0 ; i < 6 ; i ++ ) { rtb_B_6_656_5248 [ i ] = _rtB -> B_6_117_936
[ i ] ; rtb_B_6_656_5248 [ i + 6 ] = _rtB -> B_6_123_984 [ i ] ;
rtb_B_6_656_5248 [ i + 12 ] = _rtB -> B_6_129_1032 [ i ] ; rtb_B_6_656_5248 [
i + 18 ] = _rtB -> B_6_91_728 [ i + 14 ] ; rtb_B_6_656_5248 [ i + 24 ] = _rtB
-> B_6_135_1080 [ i ] ; rtb_B_6_656_5248 [ i + 30 ] = _rtB -> B_6_91_728 [ i
+ 8 ] ; rtb_B_6_656_5248 [ i + 36 ] = _rtB -> B_6_141_1128 [ i ] ;
rtb_B_6_656_5248 [ i + 42 ] = _rtB -> B_6_147_1176 [ i ] ; } { if ( _rtDW ->
_asyncqueue_inserted_for_ToWorkspace_PWORK_fx . AQHandles && ssGetLogOutput (
S ) ) { sdiWriteSignal ( _rtDW ->
_asyncqueue_inserted_for_ToWorkspace_PWORK_fx . AQHandles , ssGetTaskTime ( S
, 1 ) , ( char * ) & rtb_B_6_656_5248 [ 0 ] + 0 ) ; } } } UNUSED_PARAMETER (
tid ) ; } static void mdlOutputsTID2 ( SimStruct * S , int_T tid ) { __m128d
tmp ; __m128d tmp_0 ; B_PA_T * _rtB ; P_PA_T * _rtP ; real_T rtb_B_6_547_4376
[ 6 ] ; real_T rtb_B_6_553_4424 [ 6 ] ; real_T B_6_719_5752_idx_0 ; real_T
B_6_719_5752_idx_1 ; real_T B_6_719_5752_idx_2 ; real_T rtb_B_6_545_4360 ;
int32_T i ; _rtP = ( ( P_PA_T * ) ssGetModelRtp ( S ) ) ; _rtB = ( ( B_PA_T *
) _ssGetModelBlockIO ( S ) ) ; for ( i = 0 ; i < 6 ; i ++ ) { _rtB ->
B_6_153_1224 [ i ] = _rtP -> P_14 [ i ] ; _rtB -> B_6_159_1272 [ i ] = _rtP
-> P_15 [ i ] ; _rtB -> B_6_165_1320 [ i ] = _rtP -> P_16 [ i ] ; } _rtB ->
B_6_207_1656 [ 0 ] = _rtP -> P_18 [ 0 ] ; _rtB -> B_6_210_1680 [ 0 ] = _rtP
-> P_19 [ 0 ] ; _rtB -> B_6_207_1656 [ 1 ] = _rtP -> P_18 [ 1 ] ; _rtB ->
B_6_210_1680 [ 1 ] = _rtP -> P_19 [ 1 ] ; _rtB -> B_6_207_1656 [ 2 ] = _rtP
-> P_18 [ 2 ] ; _rtB -> B_6_210_1680 [ 2 ] = _rtP -> P_19 [ 2 ] ; memcpy ( &
_rtB -> B_6_171_1368 [ 0 ] , & _rtP -> P_17 [ 0 ] , 36U * sizeof ( real_T ) )
; memcpy ( & _rtB -> B_6_213_1704 [ 0 ] , & _rtP -> P_21 [ 0 ] , 36U * sizeof
( real_T ) ) ; rtb_B_6_545_4360 = _rtP -> P_23 * _rtP -> P_22 * _rtP -> P_24
; B_6_719_5752_idx_0 = _rtP -> P_27 [ 0 ] - _rtP -> P_28 [ 0 ] ;
B_6_719_5752_idx_1 = _rtP -> P_27 [ 1 ] - _rtP -> P_28 [ 1 ] ;
B_6_719_5752_idx_2 = _rtP -> P_27 [ 2 ] - _rtP -> P_28 [ 2 ] ; for ( i = 0 ;
i < 6 ; i ++ ) { rtb_B_6_547_4376 [ i ] = _rtP -> P_25 [ i ] ;
rtb_B_6_553_4424 [ i ] = _rtP -> P_25 [ i ] ; } rtb_B_6_547_4376 [ 2 ] = _rtP
-> P_22 * _rtP -> P_20 - rtb_B_6_545_4360 ; rtb_B_6_553_4424 [ 3 ] = _rtP ->
P_26 [ 1 ] * B_6_719_5752_idx_2 - rtb_B_6_545_4360 * B_6_719_5752_idx_1 ;
rtb_B_6_553_4424 [ 4 ] = B_6_719_5752_idx_0 * rtb_B_6_545_4360 - _rtP -> P_26
[ 0 ] * B_6_719_5752_idx_2 ; rtb_B_6_553_4424 [ 5 ] = _rtP -> P_26 [ 0 ] *
B_6_719_5752_idx_1 - B_6_719_5752_idx_0 * _rtP -> P_26 [ 1 ] ; for ( i = 0 ;
i <= 4 ; i += 2 ) { tmp = _mm_loadu_pd ( & rtb_B_6_547_4376 [ i ] ) ; tmp_0 =
_mm_loadu_pd ( & rtb_B_6_553_4424 [ i ] ) ; _mm_storeu_pd ( & _rtB ->
B_6_249_1992 [ i ] , _mm_add_pd ( tmp , tmp_0 ) ) ; tmp = _mm_loadu_pd ( &
_rtP -> P_29 [ i ] ) ; _mm_storeu_pd ( & _rtB -> B_6_255_2040 [ i ] , tmp ) ;
} memcpy ( & _rtB -> B_6_261_2088 [ 0 ] , & _rtP -> P_30 [ 0 ] , 36U * sizeof
( real_T ) ) ; _rtB -> B_6_297_2376 = _rtP -> P_31 ; _rtB -> B_6_298_2384 =
_rtP -> P_32 ; _rtB -> B_6_299_2392 = _rtP -> P_33 ; _rtB -> B_6_312_2496 =
_rtP -> P_37 ; _rtB -> B_6_313_2504 = _rtP -> P_38 ; _rtB -> B_6_314_2512 =
_rtP -> P_39 ; _rtB -> B_6_321_2568 = _rtP -> P_41 ; _rtB -> B_6_322_2576 =
_rtP -> P_42 ; for ( i = 0 ; i <= 4 ; i += 2 ) { tmp = _mm_loadu_pd ( & _rtP
-> P_34 [ i ] ) ; _mm_storeu_pd ( & _rtB -> B_6_300_2400 [ i ] , tmp ) ; tmp
= _mm_loadu_pd ( & _rtP -> P_35 [ i ] ) ; _mm_storeu_pd ( & _rtB ->
B_6_306_2448 [ i ] , tmp ) ; tmp = _mm_loadu_pd ( & _rtP -> P_36 [ i ] ) ;
_mm_storeu_pd ( & _rtB -> B_6_315_2520 [ i ] , _mm_mul_pd ( _mm_set1_pd (
_rtP -> P_40 ) , tmp ) ) ; tmp = _mm_loadu_pd ( & _rtP -> P_43 [ i ] ) ;
_mm_storeu_pd ( & _rtB -> B_6_323_2584 [ i ] , tmp ) ; } _rtB -> B_6_329_2632
= _rtP -> P_44 ; _rtB -> B_6_330_2640 = _rtP -> P_45 ; _rtB -> B_6_331_2648 =
_rtP -> P_46 ; _rtB -> B_6_332_2656 = _rtP -> P_47 ; _rtB -> B_6_333_2664 =
_rtP -> P_48 / _rtP -> P_49 ; memcpy ( & _rtB -> B_6_334_2672 [ 0 ] , & _rtP
-> P_50 [ 0 ] , 36U * sizeof ( real_T ) ) ; memcpy ( & _rtB -> B_6_370_2960 [
0 ] , & _rtP -> P_51 [ 0 ] , 36U * sizeof ( real_T ) ) ; _rtB -> B_6_406_3248
= _rtP -> P_52 ; _rtB -> B_6_407_3256 = _rtP -> P_53 ; for ( i = 0 ; i < 6 ;
i ++ ) { _rtB -> B_6_408_3264 [ i ] = _rtP -> P_54 [ i ] ; _rtB ->
B_6_414_3312 [ i ] = _rtP -> P_55 [ i ] ; _rtB -> B_6_420_3360 [ i ] = _rtP
-> P_56 [ i ] ; _rtB -> B_6_426_3408 [ i ] = _rtP -> P_57 [ i ] ; } _rtB ->
B_6_432_3456 = _rtP -> P_58 ; _rtB -> B_6_433_3464 = _rtP -> P_59 ;
UNUSED_PARAMETER ( tid ) ; }
#define MDL_UPDATE
static void mdlUpdate ( SimStruct * S , int_T tid ) { B_PA_T * _rtB ; DW_PA_T
* _rtDW ; P_PA_T * _rtP ; _rtDW = ( ( DW_PA_T * ) ssGetRootDWork ( S ) ) ;
_rtP = ( ( P_PA_T * ) ssGetModelRtp ( S ) ) ; _rtB = ( ( B_PA_T * )
_ssGetModelBlockIO ( S ) ) ; ssCallAccelRunBlock ( S , 6 , 0 ,
SS_CALL_MDL_UPDATE ) ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK . TUbufferPtrs [ 0 ] ; real_T simTime = ssGetT ( S ) ;
_rtDW -> TransportDelay_IWORK . Head [ 0 ] = ( ( _rtDW ->
TransportDelay_IWORK . Head [ 0 ] < ( _rtDW -> TransportDelay_IWORK .
CircularBufSize [ 0 ] - 1 ) ) ? ( _rtDW -> TransportDelay_IWORK . Head [ 0 ]
+ 1 ) : 0 ) ; if ( _rtDW -> TransportDelay_IWORK . Head [ 0 ] == _rtDW ->
TransportDelay_IWORK . Tail [ 0 ] ) { if ( !
PA_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW -> TransportDelay_IWORK .
CircularBufSize [ 0 ] , & _rtDW -> TransportDelay_IWORK . Tail [ 0 ] , &
_rtDW -> TransportDelay_IWORK . Head [ 0 ] , & _rtDW -> TransportDelay_IWORK
. Last [ 0 ] , simTime - _rtP -> P_4 , uBuffer , ( boolean_T ) 0 , false , &
_rtDW -> TransportDelay_IWORK . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * uBuffer + _rtDW ->
TransportDelay_IWORK . CircularBufSize [ 0 ] ) [ _rtDW ->
TransportDelay_IWORK . Head [ 0 ] ] = simTime ; ( * uBuffer ++ ) [ _rtDW ->
TransportDelay_IWORK . Head [ 0 ] ] = _rtB -> B_6_91_728 [ 23 ] ; _rtDW ->
TransportDelay_IWORK . Head [ 1 ] = ( ( _rtDW -> TransportDelay_IWORK . Head
[ 1 ] < ( _rtDW -> TransportDelay_IWORK . CircularBufSize [ 1 ] - 1 ) ) ? (
_rtDW -> TransportDelay_IWORK . Head [ 1 ] + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK . Head [ 1 ] == _rtDW -> TransportDelay_IWORK . Tail [ 1
] ) { if ( ! PA_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK . CircularBufSize [ 1 ] , & _rtDW ->
TransportDelay_IWORK . Tail [ 1 ] , & _rtDW -> TransportDelay_IWORK . Head [
1 ] , & _rtDW -> TransportDelay_IWORK . Last [ 1 ] , simTime - _rtP -> P_4 ,
uBuffer , ( boolean_T ) 0 , false , & _rtDW -> TransportDelay_IWORK .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * uBuffer + _rtDW -> TransportDelay_IWORK . CircularBufSize
[ 1 ] ) [ _rtDW -> TransportDelay_IWORK . Head [ 1 ] ] = simTime ; ( *
uBuffer ++ ) [ _rtDW -> TransportDelay_IWORK . Head [ 1 ] ] = _rtB ->
B_6_91_728 [ 24 ] ; _rtDW -> TransportDelay_IWORK . Head [ 2 ] = ( ( _rtDW ->
TransportDelay_IWORK . Head [ 2 ] < ( _rtDW -> TransportDelay_IWORK .
CircularBufSize [ 2 ] - 1 ) ) ? ( _rtDW -> TransportDelay_IWORK . Head [ 2 ]
+ 1 ) : 0 ) ; if ( _rtDW -> TransportDelay_IWORK . Head [ 2 ] == _rtDW ->
TransportDelay_IWORK . Tail [ 2 ] ) { if ( !
PA_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW -> TransportDelay_IWORK .
CircularBufSize [ 2 ] , & _rtDW -> TransportDelay_IWORK . Tail [ 2 ] , &
_rtDW -> TransportDelay_IWORK . Head [ 2 ] , & _rtDW -> TransportDelay_IWORK
. Last [ 2 ] , simTime - _rtP -> P_4 , uBuffer , ( boolean_T ) 0 , false , &
_rtDW -> TransportDelay_IWORK . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * uBuffer + _rtDW ->
TransportDelay_IWORK . CircularBufSize [ 2 ] ) [ _rtDW ->
TransportDelay_IWORK . Head [ 2 ] ] = simTime ; ( * uBuffer ++ ) [ _rtDW ->
TransportDelay_IWORK . Head [ 2 ] ] = _rtB -> B_6_91_728 [ 25 ] ; _rtDW ->
TransportDelay_IWORK . Head [ 3 ] = ( ( _rtDW -> TransportDelay_IWORK . Head
[ 3 ] < ( _rtDW -> TransportDelay_IWORK . CircularBufSize [ 3 ] - 1 ) ) ? (
_rtDW -> TransportDelay_IWORK . Head [ 3 ] + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK . Head [ 3 ] == _rtDW -> TransportDelay_IWORK . Tail [ 3
] ) { if ( ! PA_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK . CircularBufSize [ 3 ] , & _rtDW ->
TransportDelay_IWORK . Tail [ 3 ] , & _rtDW -> TransportDelay_IWORK . Head [
3 ] , & _rtDW -> TransportDelay_IWORK . Last [ 3 ] , simTime - _rtP -> P_4 ,
uBuffer , ( boolean_T ) 0 , false , & _rtDW -> TransportDelay_IWORK .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * uBuffer + _rtDW -> TransportDelay_IWORK . CircularBufSize
[ 3 ] ) [ _rtDW -> TransportDelay_IWORK . Head [ 3 ] ] = simTime ; ( *
uBuffer ++ ) [ _rtDW -> TransportDelay_IWORK . Head [ 3 ] ] = _rtB ->
B_6_91_728 [ 20 ] ; _rtDW -> TransportDelay_IWORK . Head [ 4 ] = ( ( _rtDW ->
TransportDelay_IWORK . Head [ 4 ] < ( _rtDW -> TransportDelay_IWORK .
CircularBufSize [ 4 ] - 1 ) ) ? ( _rtDW -> TransportDelay_IWORK . Head [ 4 ]
+ 1 ) : 0 ) ; if ( _rtDW -> TransportDelay_IWORK . Head [ 4 ] == _rtDW ->
TransportDelay_IWORK . Tail [ 4 ] ) { if ( !
PA_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW -> TransportDelay_IWORK .
CircularBufSize [ 4 ] , & _rtDW -> TransportDelay_IWORK . Tail [ 4 ] , &
_rtDW -> TransportDelay_IWORK . Head [ 4 ] , & _rtDW -> TransportDelay_IWORK
. Last [ 4 ] , simTime - _rtP -> P_4 , uBuffer , ( boolean_T ) 0 , false , &
_rtDW -> TransportDelay_IWORK . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * uBuffer + _rtDW ->
TransportDelay_IWORK . CircularBufSize [ 4 ] ) [ _rtDW ->
TransportDelay_IWORK . Head [ 4 ] ] = simTime ; ( * uBuffer ++ ) [ _rtDW ->
TransportDelay_IWORK . Head [ 4 ] ] = _rtB -> B_6_91_728 [ 21 ] ; _rtDW ->
TransportDelay_IWORK . Head [ 5 ] = ( ( _rtDW -> TransportDelay_IWORK . Head
[ 5 ] < ( _rtDW -> TransportDelay_IWORK . CircularBufSize [ 5 ] - 1 ) ) ? (
_rtDW -> TransportDelay_IWORK . Head [ 5 ] + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK . Head [ 5 ] == _rtDW -> TransportDelay_IWORK . Tail [ 5
] ) { if ( ! PA_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK . CircularBufSize [ 5 ] , & _rtDW ->
TransportDelay_IWORK . Tail [ 5 ] , & _rtDW -> TransportDelay_IWORK . Head [
5 ] , & _rtDW -> TransportDelay_IWORK . Last [ 5 ] , simTime - _rtP -> P_4 ,
uBuffer , ( boolean_T ) 0 , false , & _rtDW -> TransportDelay_IWORK .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * uBuffer + _rtDW -> TransportDelay_IWORK . CircularBufSize
[ 5 ] ) [ _rtDW -> TransportDelay_IWORK . Head [ 5 ] ] = simTime ; ( *
uBuffer ) [ _rtDW -> TransportDelay_IWORK . Head [ 5 ] ] = _rtB -> B_6_91_728
[ 22 ] ; } UNUSED_PARAMETER ( tid ) ; }
#define MDL_UPDATE
static void mdlUpdateTID2 ( SimStruct * S , int_T tid ) { UNUSED_PARAMETER (
tid ) ; }
#define MDL_DERIVATIVES
static void mdlDerivatives ( SimStruct * S ) { ssCallAccelRunBlock ( S , 6 ,
0 , SS_CALL_MDL_DERIVATIVES ) ; }
#define MDL_PROJECTION
static void mdlProjection ( SimStruct * S ) { ssCallAccelRunBlock ( S , 6 , 0
, SS_CALL_MDL_PROJECTION ) ; } static void mdlInitializeSizes ( SimStruct * S
) { ssSetChecksumVal ( S , 0 , 1971810285U ) ; ssSetChecksumVal ( S , 1 ,
1159982725U ) ; ssSetChecksumVal ( S , 2 , 2396851234U ) ; ssSetChecksumVal (
S , 3 , 1016121785U ) ; { mxArray * slVerStructMat = ( NULL ) ; mxArray *
slStrMat = mxCreateString ( "simulink" ) ; char slVerChar [ 10 ] ; int status
= mexCallMATLAB ( 1 , & slVerStructMat , 1 , & slStrMat , "ver" ) ; if (
status == 0 ) { mxArray * slVerMat = mxGetField ( slVerStructMat , 0 ,
"Version" ) ; if ( slVerMat == ( NULL ) ) { status = 1 ; } else { status =
mxGetString ( slVerMat , slVerChar , 10 ) ; } } mxDestroyArray ( slStrMat ) ;
mxDestroyArray ( slVerStructMat ) ; if ( ( status == 1 ) || ( strcmp (
slVerChar , "23.2" ) != 0 ) ) { return ; } } ssSetOptions ( S ,
SS_OPTION_EXCEPTION_FREE_CODE ) ; if ( ssGetSizeofDWork ( S ) != ( SLSize )
sizeof ( DW_PA_T ) ) { static char msg [ 256 ] ; snprintf ( msg , 256 ,
"Unexpected error: Internal DWork sizes do "
"not match for accelerator mex file (%ld vs %lu)." , ( signed long )
ssGetSizeofDWork ( S ) , ( unsigned long ) sizeof ( DW_PA_T ) ) ;
ssSetErrorStatus ( S , msg ) ; } if ( ssGetSizeofGlobalBlockIO ( S ) != (
SLSize ) sizeof ( B_PA_T ) ) { static char msg [ 256 ] ; snprintf ( msg , 256
, "Unexpected error: Internal BlockIO sizes do "
"not match for accelerator mex file (%ld vs %lu)." , ( signed long )
ssGetSizeofGlobalBlockIO ( S ) , ( unsigned long ) sizeof ( B_PA_T ) ) ;
ssSetErrorStatus ( S , msg ) ; } { int ssSizeofParams ; ssGetSizeofParams ( S
, & ssSizeofParams ) ; if ( ssSizeofParams != sizeof ( P_PA_T ) ) { static
char msg [ 256 ] ; snprintf ( msg , 256 ,
"Unexpected error: Internal Parameters sizes do "
"not match for accelerator mex file (%d vs %lu)." , ssSizeofParams , (
unsigned long ) sizeof ( P_PA_T ) ) ; ssSetErrorStatus ( S , msg ) ; } }
_ssSetModelRtp ( S , ( real_T * ) & PA_rtDefaultP ) ; rt_InitInfAndNaN (
sizeof ( real_T ) ) ; } static void mdlInitializeSampleTimes ( SimStruct * S
) { { SimStruct * childS ; SysOutputFcn * callSysFcns ; childS =
ssGetSFunction ( S , 0 ) ; callSysFcns = ssGetCallSystemOutputFcnList (
childS ) ; callSysFcns [ 3 + 0 ] = ( SysOutputFcn ) ( NULL ) ; childS =
ssGetSFunction ( S , 1 ) ; callSysFcns = ssGetCallSystemOutputFcnList (
childS ) ; callSysFcns [ 3 + 0 ] = ( SysOutputFcn ) ( NULL ) ; childS =
ssGetSFunction ( S , 2 ) ; callSysFcns = ssGetCallSystemOutputFcnList (
childS ) ; callSysFcns [ 3 + 0 ] = ( SysOutputFcn ) ( NULL ) ; childS =
ssGetSFunction ( S , 3 ) ; callSysFcns = ssGetCallSystemOutputFcnList (
childS ) ; callSysFcns [ 3 + 0 ] = ( SysOutputFcn ) ( NULL ) ; }
slAccRegPrmChangeFcn ( S , mdlOutputsTID2 ) ; } static void mdlTerminate (
SimStruct * S ) { }
#include "simulink.c"
