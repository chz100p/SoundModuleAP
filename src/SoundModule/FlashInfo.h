/****************************************************************************/
/*                                                                          */
/*  �t�@�C����  �F FlashInfo.h                                              */
/*                                                                          */
/*  ����        �F �t���b�V�����������                                     */
/*                                                                          */
/****************************************************************************/

#ifndef FLASHINFO_H
#define FLASHINFO_H

/****************************************************************************/
/*  �C���N���[�h                                                            */
/****************************************************************************/
#include "Common.h"


/****************************************************************************/
/*  �f�t�@�C��                                                              */
/****************************************************************************/
#define FLASH_PAGE_SIZE             256                 //(byte) Page Program.
#define FLASH_PAGE_IN_SECTOR        256                 //Max of page num in sector.
#define FLASH_SECTOR_MAX            64                  //Total sector num.
#define FLASH_TOTAL_PAGE            16384               //Total Page num.
#define FLASH_DATA_SIZE             (FLASH_TOTAL_PAGE * FLASH_PAGE_SIZE)

#define PLAY_BUFFER_SIZE            64                  //�Đ��p�o�b�t�@�̃T�C�Y.
#define PLAY_BUFFER_MAX             2                   //�Đ��p�o�b�t�@�̐�.

#define HEADER_ID_NOT_HEADER        0xBABABEEE
#define HEADER_ID                   0xBABABEEF          //����ID.
#define HEADER_ID_2                 0xBABBBEEF          //����ID.
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
/*  �\����                                                                  */
/****************************************************************************/
/* 64byte�ȏ�ɂȂ��Ă͂Ȃ�Ȃ� */

typedef struct
{
//--------
    DWORD               dwHeaderStartID;                //�w�b�_�[ID.
    BYTE                bHeaderSize;                    //���̍\���̂̃T�C�Y.
    BYTE                bReserved1[3];                  //�\��.
//-------- 8
    DWORD               dwDataSize;                     //�f�[�^�T�C�Y.
    DWORD               dwDataTotalPage;                //�f�[�^�̑��y�[�W��.
//-------- 16
    BYTE                bRestSize;                      //�Ō�̃y�[�W�̎c��o�C�g��.
    
    BYTE                bStartSector;                   //�ŏ��̃Z�N�^�[.
    BYTE                bStartPage;                     //bStartSector�̍ŏ��̃y�[�W.
    BYTE                bStartByte;                     //bStartPage�̍ŏ��̃o�C�g.
    
    DWORD               dwHeaderEndID;                  //�w�b�_�[ID.
//-------- 24
} FlashDataHeader;

typedef struct
{
//--------
    BYTE                bHeaderCount;
	BYTE				bEx;                            // 0xAB: v1, 0x02: v2
	WORD				wHeaderCount;                   // v2
	DWORD               dwReserved1;                    //�\��.
//-------- 8
	DWORD               dwReserved2;                    //�\��.
	DWORD               dwReserved3;                    //�\��.
//-------- 16
} FlashDataHeaderInfo;

typedef struct
{
//--------
    BYTE                bPlayStatus;                    //�X�e�[�^�X.
    
    BYTE                bPlayBufferNo;                  //�Đ����̃o�b�t�@No.
    BYTE                bReadBufferNo;                  //�ǂݍ��݂̃o�b�t�@No.
    BYTE                bReadFlag;                      //���̃t���O�����Ǝ��̃f�[�^��ǂݍ���.
//-------- 8
    BYTE                bCurrentSector;                 //���݂̃Z�N�^�[.
    BYTE                bCurrentPage;                   //wCurrentSector�̌��݂̃y�[�W.
    BYTE                bCurrentPos;                    //���݂̃f�[�^�ʒu.
    
    BYTE                bReadSector;                    //�������݂̃Z�N�^�[(���̃Z�N�^�[).
    BYTE                bReadPage;                      //wReadSector�̌��݂̃y�[�W.
    BYTE                bReadCount;                     //�o�b�t�@�̕����ǂݍ��݂̃J�E���g.
    
    BYTE                bReserved1;                     //�\��.
//-------- 16
    DWORD               dwPageCount;                    //�ǂݍ��񂾃y�[�W���v.
    
    BYTE                bReserved2[4];                  //�\��.
//-------- 24
} PlayInfo;

#endif //FLASHINFO_H
