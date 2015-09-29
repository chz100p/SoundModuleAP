/****************************************************************************/
/*                                                                          */
/*  ファイル名  ： Main.h                                                   */
/*                                                                          */
/*  説明        ： Windowsプログラムのメイン                                */
/*                                                                          */
/****************************************************************************/

#ifndef MAIN_H
#define MAIN_H

/****************************************************************************/
/*  インクルード                                                            */
/****************************************************************************/
#include "Common.h"


/****************************************************************************/
/*  デファイン                                                              */
/****************************************************************************/
#define MOUSE_END                   0
#define MOUSE_BIGIN                 1
#define MOUSE_MOVE                  2

//Timer ID.
#define TIMER_ID_NONE               0
#define TIMER_ID_TOTAL              1
#define TIMER_ID_ZOOM               2


/****************************************************************************/
/*                                                                          */
/*  クラス名：  メインクラス                                                */
/*                                                                          */
/****************************************************************************/
class PipoApp
{
public:
    PipoApp();
    ~PipoApp();
    
    void ResetGlobalMem();
    
    static HWND GetMainHWND();
    
    BOOL ValidateMusicInformation();
    BOOL ValidateAppInformation();
    BOOL ValidatePeriodInformation();
    BOOL ValidateEditStartEndInformation();
    BOOL ValidatePlayInformation();
    
    BOOL ValidateControlEnable();
    BOOL ALLEnableDisable( BOOL bFlag );
    
    BOOL WriteData();
    BOOL WriteDataTest();
    BOOL WriteDataTest2();
    
public:
    HINSTANCE           m_hInstance;
    HWND                m_hWnd;
};


/****************************************************************************/
/*  コールバック関数宣言                                                    */
/****************************************************************************/
BOOL CALLBACK DlgWndProc( HWND, UINT, WPARAM, LPARAM );
LRESULT CALLBACK ValidateTotalSubproc( HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam );
LRESULT CALLBACK ValidateZoomSubproc( HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam );

#endif //MAIN_H
