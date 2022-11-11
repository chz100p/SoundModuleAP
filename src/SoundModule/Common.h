/****************************************************************************/
/*                                                                          */
/*  ファイル名  ： Common.h                                                 */
/*                                                                          */
/*  説明        ： 共通定義関連                                             */
/*                                                                          */
/****************************************************************************/

#ifndef COMMON_H
#define COMMON_H

/****************************************************************************/
/*  インクルード                                                            */
/****************************************************************************/
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "resource.h"

//#include "Log.h"


/****************************************************************************/
/*  デファイン                                                              */
/****************************************************************************/
//#define SWAP_SHORT_LtoB( a ) ((a >> 8) | (a << 8))
//#define SWAP_LONG_LtoB( a ) ((a >> 24) | ((a << 8) & 0xFF0000) | ((a >> 8) & 0xFF00) | (a << 24))

#define BS_BYTE1(x) (  x        & 0xFF )
#define BS_BYTE2(x) ( (x >>  8) & 0xFF )
#define BS_BYTE3(x) ( (x >> 16) & 0xFF )
#define BS_BYTE4(x) ( (x >> 24) & 0xFF )

#define BS_INT16(x) ( (unsigned short)(BS_BYTE1(x)<<8 | BS_BYTE2(x)) )
#define BS_INT32(x) ( BS_BYTE1(x)<<24 | BS_BYTE2(x)<<16 | BS_BYTE3(x)<<8 | BS_BYTE4(x) )

//Play Status.
#define PS_NONE                             0
#define PS_ERROR                            1
#define PS_READY                            100
#define PS_PLAYING                          101
#define PS_PAUSE                            102
#define PS_FINISH                           103
#define PS_STOP                             104


/****************************************************************************/
/*                                                                          */
/*  クラス名：  便利クラス                                                  */
/*                                                                          */
/****************************************************************************/
class Utility
{
public:
    Utility();
    ~Utility();
    
    static WORD SwapINT16EndianLtoB( WORD wData );
    static DWORD SwapINT32EndianLtoB( DWORD dwData );
};


#endif //COMMON_H
