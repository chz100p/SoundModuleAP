/****************************************************************************/
/*                                                                          */
/*  ファイル名  ： FlashInfo.h                                              */
/*                                                                          */
/*  説明        ： フラッシュメモリ情報                                     */
/*                                                                          */
/****************************************************************************/

#ifndef FLASHINFO_H
#define FLASHINFO_H

/****************************************************************************/
/*  インクルード                                                            */
/****************************************************************************/
#include "Common.h"


/****************************************************************************/
/*  デファイン                                                              */
/****************************************************************************/
#define FLASH_PAGE_SIZE             256                 //(byte) Page Program.
#define FLASH_PAGE_IN_SECTOR        256                 //Max of page num in sector.
#define FLASH_SECTOR_MAX            64                  //Total sector num.
#define FLASH_TOTAL_PAGE            16384               //Total Page num.
#define FLASH_DATA_SIZE             (FLASH_TOTAL_PAGE * FLASH_PAGE_SIZE)

#define PLAY_BUFFER_SIZE            64                  //再生用バッファのサイズ.
#define PLAY_BUFFER_MAX             2                   //再生用バッファの数.

#define HEADER_ID_NOT_HEADER        0xBABABEEE
#define HEADER_ID                   0xBABABEEF          //識別ID.
#define HEADER_ID_2                 0xBABBBEEF          //識別ID.
#define HEADER_PAGE_NUM             1
#define HEADER_PAGE_SIZE            (FLASH_PAGE_SIZE * HEADER_PAGE_NUM)
#define HEADER_COUNT_MAX            ((HEADER_PAGE_SIZE - sizeof(FlashDataHeaderInfo)) / sizeof(FlashDataHeader))
#define HEADER_START_ADDRESS                0
#define HEADER_INFO_ADDRESS         (HEADER_COUNT_MAX * sizeof(FlashDataHeader))

#define SOUNDMODULE_RATE            46875
#define SOUNDMODULE_HEADER_START_PAGE   0
#define SOUNDMODULE_SAMPLE_START_PAGE   (SOUNDMODULE_HEADER_START_PAGE + HEADER_PAGE_NUM)
#define SOUNDMODULE_SAMPLE_PAGE_NUM (FLASH_TOTAL_PAGE - SOUNDMODULE_SAMPLE_START_PAGE)
#define SOUNDMODULE_SAMPLE_COUNT_MAX    (FLASH_PAGE_SIZE * SOUNDMODULE_SAMPLE_PAGE_NUM)
#define SOUNDMODULE_SAMPLE_START_ADDRESS        (FLASH_PAGE_SIZE * SOUNDMODULE_SAMPLE_START_PAGE)

#define HEADER_EX_V2				2
#define HEADER_COUNT_MAX_EX_V1		HEADER_COUNT_MAX

/****************************************************************************/
/*  typedef                                                                 */
/****************************************************************************/
typedef DWORD                       FLASH_ADDRESS;


/****************************************************************************/
/*  構造体                                                                  */
/****************************************************************************/
/* 64byte以上になってはならない */

typedef struct
{
//--------
    DWORD               dwHeaderStartID;                //ヘッダーID.
    BYTE                bHeaderSize;                    //この構造体のサイズ.
    BYTE                bReserved1[3];                  //予約.
//-------- 8
    DWORD               dwDataSize;                     //データサイズ.
    DWORD               dwDataTotalPage;                //データの総ページ数.
//-------- 16
    BYTE                bRestSize;                      //最後のページの残りバイト数.
    
    BYTE                bStartSector;                   //最初のセクター.
    BYTE                bStartPage;                     //bStartSectorの最初のページ.
    BYTE                bStartByte;                     //bStartPageの最初のバイト.
    
    DWORD               dwHeaderEndID;                  //ヘッダーID.
//-------- 24
} FlashDataHeader;

typedef struct
{
//--------
    BYTE                bHeaderCount;
	BYTE				bEx;                            // 0xAB: v1, 0x02: v2
	WORD				wHeaderCount;                   // v2
	DWORD               dwReserved1;                    //予約.
//-------- 8
	DWORD               dwReserved2;                    //予約.
	DWORD               dwReserved3;                    //予約.
//-------- 16
} FlashDataHeaderInfo;

typedef struct
{
//--------
    BYTE                bPlayStatus;                    //ステータス.
    
    BYTE                bPlayBufferNo;                  //再生中のバッファNo.
    BYTE                bReadBufferNo;                  //読み込みのバッファNo.
    BYTE                bReadFlag;                      //このフラグが立つと次のデータを読み込む.
//-------- 8
    BYTE                bCurrentSector;                 //現在のセクター.
    BYTE                bCurrentPage;                   //wCurrentSectorの現在のページ.
    BYTE                bCurrentPos;                    //現在のデータ位置.
    
    BYTE                bReadSector;                    //準備澄みのセクター(次のセクター).
    BYTE                bReadPage;                      //wReadSectorの現在のページ.
    BYTE                bReadCount;                     //バッファの分割読み込みのカウント.
    
    BYTE                bReserved1;                     //予約.
//-------- 16
    DWORD               dwPageCount;                    //読み込んだページ合計.
    
    BYTE                bReserved2[4];                  //予約.
//-------- 24
} PlayInfo;

#endif //FLASHINFO_H
