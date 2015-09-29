/****************************************************************************/
/*                                                                          */
/*  ファイル名  ： USBManager.c                                             */
/*                                                                          */
/*  説明        ： USBアクセス関連                                          */
/*                                                                          */
/****************************************************************************/


/****************************************************************************/
/*  インクルード                                                            */
/****************************************************************************/
#include "USBManager.h"

#include <Dbt.h>
#include <initguid.h>
//#include <usbioctl.h>

#include "Main.h"
#include "Log.h"


# pragma comment(lib, "setupapi.lib")
# pragma comment(lib, "hid.lib")
# pragma comment(lib, "winmm.lib")


/****************************************************************************/
/*  グローバル変数宣言                                                      */
/****************************************************************************/
OVERLAPPED                      stReadOverlap;
OVERLAPPED                      stWriteOverlap;

BYTE                            bWOutBuf[USB_BUFFER_SIZE + 1]       = {0};
BYTE                            bROutBuf[USB_BUFFER_SIZE + 1]       = {0};

///****************************************************************************/
///*    Extern宣言                                                            */
///****************************************************************************/
//extern PipoApp                g_PipoApp;


/****************************************************************************/
/*                                                                          */
/*  関数名  ：  USBManager                                                  */
/*                                                                          */
/*  説明    ：  コンストラクタ                                              */
/*                                                                          */
/****************************************************************************/
USBManager::USBManager()
{
    m_bUSBStatus                = UMS_NONE;
    
    m_hHidDll                   = LoadLibraryA( "HID.DLL" );
    
//  m_idHid                     = GUID_DEVINTERFACE_USB_DEVICE;
    
    m_hReadHandle               = INVALID_HANDLE_VALUE;
    m_hWriteHandle              = INVALID_HANDLE_VALUE;
    
    m_hNotification             = NULL;
    
    m_pstDevInterfaceDetail     = NULL;
    
    return;
}


/****************************************************************************/
/*                                                                          */
/*  関数名  ：  ~USBManager                                                 */
/*                                                                          */
/*  説明    ：  デストラクタ                                                */
/*                                                                          */
/****************************************************************************/
USBManager::~USBManager()
{
    CloseUSB();
    
    return;
}


/****************************************************************************/
/*                                                                          */
/*  関数名  ：  InitUSBManager                                              */
/*                                                                          */
/*  説明    ：  USBManager初期処理                                          */
/*                                                                          */
/****************************************************************************/
BOOL USBManager::InitUSBManager()
{
    BOOL        bRet        = TRUE;
    
    if( m_bUSBStatus != UMS_NONE )
    {
        CloseUSB();
    }
    
    bRet = OpenUSB();
    
    return bRet;
}


/****************************************************************************/
/*                                                                          */
/*  関数名  ：  OpenUSB                                                     */
/*                                                                          */
/*  説明    ：  USBオープン                                                 */
/*                                                                          */
/****************************************************************************/
BOOL USBManager::OpenUSB()
{
    BOOL                            bRet                    = FALSE;
    BOOL                            bResult                 = FALSE;
    // DWORD                           dwErr                   = 0;
    DWORD                           dwDevIndex              = 0;
    DWORD                           dwReqSize               = 0;
    PtrHidD_GetHidGuid              pHidD_GetHidGuid        = NULL;
    PtrHidD_GetAttributes           pHidD_GetAttributes     = NULL;
    HDEVINFO                        hDeviceInfoSet          = NULL;
    // WCHAR                           wcIndexStr[10]          = {0};
    // INT                             iIndexNo                = 0;
    SP_DEVICE_INTERFACE_DATA        stDevInterfaceData;
    HIDD_ATTRIBUTES                 stHidAttributes;
    
    if( m_bUSBStatus == UMS_REDY )
    {
        bRet = TRUE;
        goto END;
    }
    
    memset( &stDevInterfaceData, 0, sizeof(SP_DEVICE_INTERFACE_DATA) );
    memset( &stHidAttributes, 0, sizeof(HIDD_ATTRIBUTES) );
    
    m_bUSBStatus = UMS_ERR_OPEN;
    
    if( m_hHidDll != INVALID_HANDLE_VALUE )
    {
        pHidD_GetHidGuid = (PtrHidD_GetHidGuid)GetProcAddress( m_hHidDll, "HidD_GetHidGuid" );
        if( pHidD_GetHidGuid == NULL )
        {
            m_bUSBStatus = UMS_ERR_INIT;
            goto END;
        }
    }
    
    if( m_hHidDll != INVALID_HANDLE_VALUE )
    {
        pHidD_GetAttributes = (PtrHidD_GetAttributes)GetProcAddress( m_hHidDll, "HidD_GetAttributes" );
        if( pHidD_GetAttributes == NULL )
        {
            m_bUSBStatus = UMS_ERR_INIT;
            goto END;
        }
    }
    
    pHidD_GetHidGuid( &m_idHid );
    
    hDeviceInfoSet = SetupDiGetClassDevs( &m_idHid, NULL, NULL, DIGCF_PRESENT | DIGCF_INTERFACEDEVICE );
    if( hDeviceInfoSet == NULL )
    {
        m_bUSBStatus = UMS_ERR_INIT;
        goto END;
    }
    
    stDevInterfaceData.cbSize   = sizeof( SP_DEVICE_INTERFACE_DATA );
    stHidAttributes.uSize       = sizeof( HIDD_ATTRIBUTES );
    
    if( m_pstDevInterfaceDetail != NULL )
    {
        delete m_pstDevInterfaceDetail;
        m_pstDevInterfaceDetail = NULL;
    }
    
    for( dwDevIndex = 0; ; dwDevIndex++ )
    {
        bResult = SetupDiEnumDeviceInterfaces( hDeviceInfoSet, NULL, &m_idHid, dwDevIndex, &stDevInterfaceData );
        if( bResult == FALSE )
        {
            m_bUSBStatus = UMS_ERR_INIT;
            break;
        }
        
        bResult = SetupDiGetDeviceInterfaceDetail( hDeviceInfoSet, &stDevInterfaceData, NULL, 0, &dwReqSize, NULL );
        
        m_pstDevInterfaceDetail = (PSP_DEVICE_INTERFACE_DETAIL_DATA)new CHAR[dwReqSize];
        if( m_pstDevInterfaceDetail == NULL )
        {
            m_bUSBStatus = UMS_ERR_INIT;
            break;
        }
        
        m_pstDevInterfaceDetail->cbSize = sizeof( SP_DEVICE_INTERFACE_DETAIL_DATA );
        
        bResult = SetupDiGetDeviceInterfaceDetail( hDeviceInfoSet, &stDevInterfaceData, m_pstDevInterfaceDetail, dwReqSize, &dwReqSize, NULL );
        if( bResult == FALSE )
        {
            m_bUSBStatus = UMS_ERR_INIT;
            break;
        }
        
#if 0
        for( i = 0, iIndexNo = 0; i < 50;i ++ )
        {
            if( m_pstDevInterfaceDetail->DevicePath[i] == 'm' )
            {
                wcsncpy_s( wcIndexStr, 10, &m_pstDevInterfaceDetail->DevicePath[i] + 3, 2 );
                wcIndexStr[2] = NULL;
                
                iIndexNo = _wtoi( &wcIndexStr[0] );

                break;
            }
        }
#endif

#if D_LOG08
        OutputDebugStringA( "-----------------------------------------\n" );
        
        sprintf_s( g_cLog, LOG_SIZE,  "[ %d ]\n", (dwDevIndex + 1) );
        OutputDebugStringA( g_cLog );
        
        OutputDebugStringW( m_pstDevInterfaceDetail->DevicePath );
        OutputDebugStringA( "\n" );
#endif
        
        m_hReadHandle = CreateFile( m_pstDevInterfaceDetail->DevicePath,
                                    GENERIC_READ,
                                    FILE_SHARE_READ | FILE_SHARE_WRITE,
                                    (LPSECURITY_ATTRIBUTES)NULL,
                                    OPEN_EXISTING,
                                    FILE_FLAG_OVERLAPPED,
                                    NULL );
        
        if( m_hReadHandle != INVALID_HANDLE_VALUE )
        {
            bResult = pHidD_GetAttributes( m_hReadHandle, &stHidAttributes );
            if( bResult == TRUE )
            {
#if D_LOG08
                sprintf_s( g_cLog, LOG_SIZE,  "VendorID : 0x%X, ProductID : 0x%X\n", stHidAttributes.wVendorID, stHidAttributes.wProductID );
                OutputDebugStringA( g_cLog );
#endif
//              if( (stHidAttributes.wVendorID == DRV_VENDER_ID) && (stHidAttributes.wProductID == DRV_PRODUCT_ID) && (iIndexNo == 1) )
//              if( (stHidAttributes.wVendorID == DRV_VENDER_ID) && (stHidAttributes.wProductID == DRV_PRODUCT_ID) )
                if ((stHidAttributes.wVendorID == 0x22EA) && (stHidAttributes.wProductID == 0x0041))
                {
#if D_LOG08
                    OutputDebugStringA( "Find Target !!!\n" );
#endif
#if 1
                    m_hWriteHandle = CreateFile( m_pstDevInterfaceDetail->DevicePath,
                                                 GENERIC_WRITE,
                                                 FILE_SHARE_READ | FILE_SHARE_WRITE,
                                                 (LPSECURITY_ATTRIBUTES)NULL,
                                                 OPEN_EXISTING,
                                                 0,
                                                 NULL );
#else
                    m_hWriteHandle = CreateFile( m_pstDevInterfaceDetail->DevicePath,
                                                 GENERIC_WRITE,
                                                 FILE_SHARE_READ | FILE_SHARE_WRITE,
                                                 (LPSECURITY_ATTRIBUTES)NULL,
                                                 OPEN_EXISTING,
                                                 FILE_FLAG_OVERLAPPED,
                                                 NULL );
#endif
                    
                    if( m_hWriteHandle != INVALID_HANDLE_VALUE )
                    {
#if D_LOG08
                        OutputDebugStringA( "Connect Success !!!\n" );
#endif
                        m_bUSBStatus        = UMS_REDY;
                        bRet                = TRUE;
                    }
                    else
                    {
#if D_LOG08
                        OutputDebugStringA( "Connect Fial...\n" );
#endif
                        CloseHandle( m_hReadHandle );
                        m_hReadHandle = INVALID_HANDLE_VALUE;
                        
                        m_bUSBStatus = UMS_ERR_OPEN;
                    }
                    
#if D_LOG08
                    OutputDebugStringA( "-----------------------------------------\n" );
#endif
                    break;
                }
#if D_LOG08
                else
                {
                    OutputDebugStringA( "No match...\n" );
                }
#endif
            }
            else
            {
#if D_LOG08
                OutputDebugStringA( "Fail -> GetAttributes\n" );
#endif
                CloseHandle( m_hReadHandle );
                m_hReadHandle = INVALID_HANDLE_VALUE;
            }
        }
#if D_LOG08
        else
        {
            OutputDebugStringA( "Fail -> CreateFile\n" );
        }
#endif
        
        delete m_pstDevInterfaceDetail;
        m_pstDevInterfaceDetail = NULL;
        
#if D_LOG08
        OutputDebugStringA( "-----------------------------------------\n" );
#endif
    }
    
    if( bRet == TRUE )
    {
        DEV_BROADCAST_DEVICEINTERFACE_W                     stDBDI;
        
        memset( &stDBDI, 0, sizeof(DEV_BROADCAST_DEVICEINTERFACE) );
        
        stDBDI.dbcc_size            = sizeof( DEV_BROADCAST_DEVICEINTERFACE );
        stDBDI.dbcc_devicetype      = DBT_DEVTYP_DEVICEINTERFACE;
//      stDBDI.dbcc_classguid       = GUID_CLASS_USB_DEVICE;
//      stDBDI.dbcc_classguid       = GUID_DEVINTERFACE_USB_DEVICE;
        stDBDI.dbcc_classguid       = m_idHid;
        
        // m_hNotification              = RegisterDeviceNotification( g_PipoApp.GetMainHWND(), &stDBDI, DEVICE_NOTIFY_WINDOW_HANDLE );
    }
    
END:
    if( hDeviceInfoSet != NULL )
    {
        SetupDiDestroyDeviceInfoList( hDeviceInfoSet );
    }
    
    return bRet;
}


#if 1
/****************************************************************************/
/*                                                                          */
/*  関数名  ：  WriteData                                                   */
/*                                                                          */
/*  説明    ：  データ送信                                                  */
/*                                                                          */
/****************************************************************************/
BOOL USBManager::WriteDataEx( void*  pvBuffer, DWORD  dwBufSize )
{
    BOOL            bRet                                = TRUE;
    // DWORD            dwWriteTotalSize                    = dwBufSize;
    DWORD           dwWriteSize                             = 0;
    DWORD           dwWriteCnt                          = 0;
    
    while( 1 )
    {
        if( dwBufSize > USB_BUFFER_SIZE )
        {
            dwWriteSize = WriteData( (void*)((CHAR *)pvBuffer + dwWriteCnt), USB_BUFFER_SIZE );
            if( dwWriteSize != 0 )
            {
                dwWriteCnt += USB_BUFFER_SIZE;
                dwBufSize -= USB_BUFFER_SIZE;
            }
            else
            {
                break;
            }
        }
        else
        {
            dwWriteSize = WriteData( (void *)((CHAR *)pvBuffer + dwWriteCnt), dwBufSize );
            dwWriteCnt += dwBufSize;
            
            break;
        }
    }
    
#if D_LOG09
    sprintf_s( g_cLog, LOG_SIZE, "WriteFile [%d, %d]\n", dwWriteCnt, dwWriteTotalSize );
    OutputDebugStringA( g_cLog );
    
    if( dwWriteCnt != dwWriteTotalSize )
    {
        OutputDebugStringA( "Write Error !!!!!!!" );
    }
#endif
    
    return bRet;

}
#endif


/****************************************************************************/
/*                                                                          */
/*  関数名  ：  WriteData                                                   */
/*                                                                          */
/*  説明    ：  データ送信(MAX64byte)                                       */
/*                                                                          */
/****************************************************************************/
DWORD USBManager::WriteData( void*  pvBuffer, DWORD  dwBufSize )
{
    BOOL            bRet                                = FALSE;
    DWORD           dwWriteSize                         = 0;
//  BYTE            bWOutBuf[USB_BUFFER_SIZE + 1]       = {0};
#if 0
    DWORD           dwResult                            = 0;
    HANDLE          hEvent                              = NULL;
#endif
    
    if( m_bUSBStatus != UMS_REDY )
    {
        goto END;
    }
    
    if( m_hWriteHandle == INVALID_HANDLE_VALUE )
    {
        goto END;
    }
    
    if( dwBufSize > USB_BUFFER_SIZE )
    {
        goto END;
    }
    
#if 0
    hEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
    if( hEvent == NULL )
    {
        goto END;
    }
    
    memset( &stWriteOverlap, 0, sizeof(OVERLAPPED) );
    stWriteOverlap.Offset       = 0;
    stWriteOverlap.OffsetHigh   = 0;
    stWriteOverlap.hEvent       = hEvent;
#endif
    
    bWOutBuf[0] = 0;
    
    memcpy( &bWOutBuf[1], pvBuffer, dwBufSize );
    
#if 0
    bRet = WriteFile( m_hWriteHandle, (void*)bWOutBuf, (USB_BUFFER_SIZE + 1), &dwWriteSize, &stWriteOverlap ); 
//  if( bRet == TRUE )
//  {
        dwResult = WaitForSingleObject( hEvent,     100 );
        switch( dwResult )
        {
            case WAIT_OBJECT_0:
                break;
            
            case WAIT_TIMEOUT: 
#if D_LOG09
                sprintf_s( g_cLog, LOG_SIZE, "WriteFile Time out(%d)\n", dwResult );
                OutputDebugStringA( g_cLog );
#endif
                CancelIo( m_hWriteHandle );
                break;
            
            default: 
#if D_LOG09
                sprintf_s( g_cLog, LOG_SIZE, "WriteFile Other error(%d)\n", dwResult );
                OutputDebugStringA( g_cLog );
#endif
                break;
            
        }
        
//  }
    
    CloseHandle( hEvent );
#else
    bRet = WriteFile( m_hWriteHandle, (void*)bWOutBuf, (USB_BUFFER_SIZE + 1), &dwWriteSize, NULL );
    
//  Sleep( 2 );
#endif
    
END:
    return dwWriteSize;
}


/****************************************************************************/
/*                                                                          */
/*  関数名  ：  ReadData                                                    */
/*                                                                          */
/*  説明    ：  データ受信                                                  */
/*                                                                          */
/****************************************************************************/
DWORD USBManager::ReadData( void*  pvBuffer, DWORD  dwBufSize )
{
    BOOL            bRet                                = FALSE;
    DWORD           dwReadSize                          = 0;
//  BYTE            bROutBuf[USB_BUFFER_SIZE + 1]       = {0};
    DWORD           dwResult                            = 0;
    HANDLE          hEvent                              = NULL;
    
    if( m_bUSBStatus != UMS_REDY )
    {
        goto END;
    }
    
    if( m_hReadHandle == INVALID_HANDLE_VALUE )
    {
        goto END;
    }
    
    if( dwBufSize < USB_BUFFER_SIZE )
    {
        goto END;
    }
    
    hEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
    if( hEvent == NULL )
    {
        goto END;
    }
    
    memset( &stReadOverlap, 0, sizeof(OVERLAPPED) );
    stReadOverlap.Offset        = 0;
    stReadOverlap.OffsetHigh    = 0;
    stReadOverlap.hEvent        = hEvent;
    
    bROutBuf[0] = 0;
    
    bRet = ReadFile( m_hReadHandle, (void*)bROutBuf, (USB_BUFFER_SIZE + 1),     &dwReadSize, &stReadOverlap );
//  if( bRet == TRUE )
//  {

#if 0
        dwResult = WaitForSingleObject( hEvent,     10000 );
#else
        dwResult = WaitForSingleObject( hEvent,     100 );
#endif

#if D_LOG09
        if( dwReadSize != 0 )
        {
            sprintf_s( g_cLog, LOG_SIZE, "ReadFile [%d, %d]\n", dwBufSize, dwReadSize );
            OutputDebugStringA( g_cLog );
        }
#endif
        switch( dwResult )
        {
            case WAIT_OBJECT_0:
                memcpy( pvBuffer, &bROutBuf[1], USB_BUFFER_SIZE );
                
                dwReadSize = USB_BUFFER_SIZE;
                break;
                
            case WAIT_TIMEOUT: 
#if D_LOG09
                sprintf_s( g_cLog, LOG_SIZE, "ReadFile Time out(ReadSize:%d, %d)\n", dwReadSize, dwResult );
                OutputDebugStringA( g_cLog );
#endif
                CancelIo( m_hReadHandle );
                break;
                
            default: 
#if D_LOG09
                sprintf_s( g_cLog, LOG_SIZE, "ReadFile Other Err(%d)\n", dwResult );
                OutputDebugStringA( g_cLog );
#endif
                break;
        }
//  }
    
    CloseHandle( hEvent );
    
END:
    return dwReadSize;
}


/****************************************************************************/
/*                                                                          */
/*  関数名  ：  CloseUSB                                                    */
/*                                                                          */
/*  説明    ：  USBオープン                                                 */
/*                                                                          */
/****************************************************************************/
void USBManager::CloseUSB()
{
    if( m_hReadHandle != INVALID_HANDLE_VALUE )
    {
        CloseHandle( m_hReadHandle );
        m_hReadHandle = INVALID_HANDLE_VALUE;
    }
    
    if( m_hWriteHandle != INVALID_HANDLE_VALUE )
    {
        CloseHandle( m_hWriteHandle );
        m_hWriteHandle = INVALID_HANDLE_VALUE;
    }
    
    if( m_hNotification != NULL )
    {
        UnregisterDeviceNotification( m_hNotification );
        m_hNotification = NULL;
    }
    
    if( m_pstDevInterfaceDetail != NULL )
    {
        delete m_pstDevInterfaceDetail;
        m_pstDevInterfaceDetail = NULL;
    }
    
    m_bUSBStatus = UMS_CLOSE;
}


/****************************************************************************/
/*                                                                          */
/*  関数名  ：  GetStatus                                                   */
/*                                                                          */
/*  説明    ：  USBStatus取得                                               */
/*                                                                          */
/****************************************************************************/
BYTE USBManager::GetStatus()
{
    return m_bUSBStatus;
}


/****************************************************************************/
/*                                                                          */
/*  関数名  ：  GetDriverName                                               */
/*                                                                          */
/*  説明    ：  DriverName取得                                              */
/*                                                                          */
/****************************************************************************/
BOOL USBManager::GetDriverName( WCHAR*  pwcIndexString, DWORD  dwStrSize )
{
    BOOL                                    bRet                        = TRUE;
    PtrHidD_GetIndexedString                pHidD_GetIndexedString      = NULL;
    
    if( m_hReadHandle == NULL )
    {
        bRet = FALSE;
        goto END;
    }
    
    if( m_hHidDll != INVALID_HANDLE_VALUE )
    {
        pHidD_GetIndexedString  = (PtrHidD_GetIndexedString)GetProcAddress( m_hHidDll, "HidD_GetIndexedString" );
        if( pHidD_GetIndexedString == NULL )
        {
            bRet = FALSE;
            goto END;
        }
    }
    
    bRet = pHidD_GetIndexedString( m_hReadHandle, DRV_INDEX_NAME, pwcIndexString, dwStrSize );
    
END:
    return bRet;
}


/****************************************************************************/
/*                                                                          */
/*  関数名  ：  GetDriverVersion                                            */
/*                                                                          */
/*  説明    ：  DriverVersion取得                                           */
/*                                                                          */
/****************************************************************************/
BOOL USBManager::GetDriverVersion( WCHAR*  pwcIndexString, DWORD  dwStrSize )
{
    BOOL                                    bRet                        = TRUE;
    PtrHidD_GetIndexedString                pHidD_GetIndexedString      = NULL;
    
    if( m_hReadHandle == NULL )
    {
        bRet = FALSE;
        goto END;
    }
    
    if( m_hHidDll != INVALID_HANDLE_VALUE )
    {
        pHidD_GetIndexedString  = (PtrHidD_GetIndexedString)GetProcAddress( m_hHidDll, "HidD_GetIndexedString" );
        if( pHidD_GetIndexedString == NULL )
        {
            bRet = FALSE;
            goto END;
        }
    }
    
    bRet = pHidD_GetIndexedString( m_hReadHandle, DRV_INDEX_VER, pwcIndexString, dwStrSize );
    
END:
    return bRet;
}


/****************************************************************************/
/*                                                                          */
/*  関数名  ：  DeviceChangeNortification                                   */
/*                                                                          */
/*  説明    ：  USBイベント通知                                             */
/*                                                                          */
/****************************************************************************/
BOOL USBManager::DeviceChangeNortification( UINT  wParam, void*  lParam )
{
    BOOL                                    bRet                        = FALSE;
    BOOL                                    bCheck                      = FALSE;
    WCHAR                                   wcMsg[MAX_PATH]                 = {0};
    DEV_BROADCAST_DEVICEINTERFACE_W             *pstDevBroadCast            = NULL;
    
#if D_LOG08
    OutputDebugStringA( "-----------------------------------------\n" );
    
    sprintf_s( g_cLog, LOG_SIZE,  "DeviceChange[W : %d, L : %d]\n", wParam, lParam );
    OutputDebugStringA( g_cLog );
#endif
    
    if( m_bUSBStatus != UMS_REDY )
    {
        goto END;
    }
    
    if( lParam == NULL )
    {
        goto END;
    }
    
    pstDevBroadCast = (DEV_BROADCAST_DEVICEINTERFACE*)lParam;
    
    if( pstDevBroadCast->dbcc_devicetype != DBT_DEVTYP_DEVICEINTERFACE )
    {
        goto END;
    }
    
    bCheck = CheckDeviceName( (WCHAR*)pstDevBroadCast->dbcc_name );
    if( bCheck == FALSE )
    {
        goto END;
    }
    
#if D_LOG08
    switch( wParam )
    {
        case DBT_QUERYCHANGECONFIG:
            swprintf_s( wcMsg, MAX_PATH, L"DBT_QUERYCHANGECONFIG(%d) : 設定変更要求発行\n", wParam );
            OutputDebugStringW( wcMsg );
            break;
            
        case DBT_CONFIGCHANGECANCELED:
            swprintf_s( wcMsg, MAX_PATH, L"DBT_CONFIGCHANGECANCELED(%d) : 設定変更要求キャンセル\n", wParam );
            OutputDebugStringW( wcMsg );
            break;
            
        case DBT_CONFIGCHANGED:
            swprintf_s( wcMsg, MAX_PATH, L"DBT_CONFIGCHANGED(%d) : 設定変更\n", wParam );
            OutputDebugStringW( wcMsg );
            break;
            
        case DBT_DEVICEARRIVAL:
            swprintf_s( wcMsg, MAX_PATH, L"DBT_DEVICEARRIVAL(%d) : デバイス使用可能\n", wParam );
            OutputDebugStringW( wcMsg );
            break;
            
        case DBT_DEVICEQUERYREMOVE:
            swprintf_s( wcMsg, MAX_PATH, L"DBT_DEVICEQUERYREMOVE(%d) : デバイス停止要求発行\n", wParam );
            OutputDebugStringW( wcMsg );
            break;
            
        case DBT_DEVICEQUERYREMOVEFAILED:
            swprintf_s( wcMsg, MAX_PATH, L"DBT_DEVICEQUERYREMOVEFAILED(%d) : デバイス停止要求失敗\n", wParam );
            OutputDebugStringW( wcMsg );
            break;
            
        case DBT_DEVICEREMOVEPENDING:
            swprintf_s( wcMsg, MAX_PATH, L"DBT_DEVICEREMOVEPENDING(%d) : デバイス停止中\n", wParam );
            OutputDebugStringW( wcMsg );
            break;
            
        case DBT_DEVICEREMOVECOMPLETE:
            swprintf_s( wcMsg, MAX_PATH, L"DBT_DEVICEREMOVECOMPLETE(%d) : デバイス停止\n", wParam );
            OutputDebugStringW( wcMsg );
            break;
            
        case DBT_DEVNODES_CHANGED:
            swprintf_s( wcMsg, MAX_PATH, L"DBT_DEVNODES_CHANGED(%d) : デバイス状態変化\n", wParam );
            OutputDebugStringW( wcMsg );
            break;
            
        case DBT_DEVICETYPESPECIFIC:
            swprintf_s( wcMsg, MAX_PATH, L"DBT_DEVICETYPESPECIFIC(%d) : 独自イベント発行\n", wParam );
            OutputDebugStringW( wcMsg );
            break;
            
        case DBT_CUSTOMEVENT:
            swprintf_s( wcMsg, MAX_PATH, L"DBT_CUSTOMEVENT(%d) : カスタムイベント発行\n", wParam );
            OutputDebugStringW( wcMsg );
            break;
            
        case DBT_USERDEFINED:
            swprintf_s( wcMsg, MAX_PATH, L"DBT_USERDEFINED(%d) : \n", wParam );
            OutputDebugStringW( wcMsg );
            break;
            
        default:
            swprintf_s( wcMsg, MAX_PATH, L"UNKNOWN(%d) : 不明イベント\n", wParam );
            OutputDebugStringW( wcMsg );
            break;
    }
#endif
    
    switch( wParam )
    {
        case DBT_QUERYCHANGECONFIG:
            break;
            
        case DBT_CONFIGCHANGECANCELED:
            break;
            
        case DBT_CONFIGCHANGED:
            break;
            
        case DBT_DEVICEARRIVAL:
            break;
            
        case DBT_DEVICEQUERYREMOVEFAILED:
            break;
            
        case DBT_DEVICEREMOVEPENDING:
            break;
            
        case DBT_DEVICEQUERYREMOVE:
        case DBT_DEVICEREMOVECOMPLETE:
            CloseUSB();
            
            bRet = TRUE;
            break;
            
        case DBT_DEVNODES_CHANGED:
            break;
            
        case DBT_DEVICETYPESPECIFIC:
            break;
            
        case DBT_CUSTOMEVENT:
            break;
            
        case DBT_USERDEFINED:
            break;
            
        default:
            break;
    }
    
END:
#if D_LOG08
    OutputDebugStringA( "-----------------------------------------\n" );
#endif
    return bRet;
}


/****************************************************************************/
/*                                                                          */
/*  関数名  ：  CheckDeviceName                                             */
/*                                                                          */
/*  説明    ：  デバイス名チェック                                          */
/*                                                                          */
/****************************************************************************/
BOOL USBManager::CheckDeviceName( WCHAR*  pDevName )
{
    BOOL            bRet        = FALSE;
    INT             nResult     = 0;
    
#if D_LOG08
        OutputDebugStringA( "-----------------------------------------\n" );
        OutputDebugStringA( "< CheckDeviceName >\n" );
#endif
    
    if( pDevName == NULL )
    {
#if D_LOG08
        OutputDebugStringA( "CheckDevice  : NULL\n" );
#endif
        goto END;
    }
    
#if D_LOG08
    OutputDebugStringA( "FSForcetouch : " );
    OutputDebugStringW( m_pstDevInterfaceDetail->DevicePath );
    OutputDebugStringA( "\n" );
    
    OutputDebugStringA( "CheckDevice  : " );
    OutputDebugStringW( pDevName );
    OutputDebugStringA( "\n" );
#endif
    
    nResult = _wcsicmp( m_pstDevInterfaceDetail->DevicePath, pDevName );
    
#if D_LOG08
    sprintf_s( g_cLog, LOG_SIZE,  "Cmp Result : %d\n", nResult );
    OutputDebugStringA( g_cLog );
#endif
    
    if( nResult == 0 )
    {
        bRet = TRUE;
    }
    
END:
#if D_LOG08
    OutputDebugStringA( "-----------------------------------------\n" );
#endif
    return bRet;
}
