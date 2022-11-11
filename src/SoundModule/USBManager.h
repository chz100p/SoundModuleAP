/****************************************************************************/
/*                                                                          */
/*  ファイル名  ： USBManager.h                                             */
/*                                                                          */
/*  説明        ： USBManager関連                                           */
/*                                                                          */
/****************************************************************************/

#ifndef USBMANAGER_H
#define USBMANAGER_H

/****************************************************************************/
/*  インクルード                                                            */
/****************************************************************************/
#include "Common.h"

#include <wtypes.h>
#include <setupapi.h>


/****************************************************************************/
/*  typedef                                                                 */
/****************************************************************************/
typedef struct _HIDD_ATTRIBUTES
{
    ULONG           uSize;
    WORD            wVendorID;
    WORD            wProductID;
    WORD            wVersionNumber;
} HIDD_ATTRIBUTES, *PHIDD_ATTRIBUTES;

typedef BOOL (__stdcall *PtrHidD_GetAttributes)(HANDLE, PHIDD_ATTRIBUTES);
typedef VOID (__stdcall *PtrHidD_GetHidGuid)(LPGUID);
typedef BOOL (__stdcall *PtrHidD_GetIndexedString)(HANDLE, ULONG, PVOID, ULONG);


/****************************************************************************/
/*  デファイン                                                              */
/****************************************************************************/
#define UMS_NONE                0
#define UMS_INIT                1
#define UMS_REDY                2
#define UMS_CLOSE               3
#define UMS_ERR_INIT            20
#define UMS_ERR_OPEN            21

#define DRV_VENDER_ID           0xBA03
#define DRV_PRODUCT_ID          0x0003

#define DRV_INDEX_NAME          2
#define DRV_INDEX_VER           3

#define USB_BUFFER_SIZE         64


/****************************************************************************/
/*                                                                          */
/*  クラス名：  USBManager                                                  */
/*                                                                          */
/****************************************************************************/
class USBManager
{
public:
    USBManager();
    ~USBManager();
    
    BOOL InitUSBManager();
    BOOL OpenUSB();
#if 1
    BOOL WriteDataEx( void*  pvBuffer, DWORD  dwBufSize );
#endif
    DWORD WriteData( void*  pvBuffer, DWORD dwBufSize );
    DWORD ReadData( void*  pvBuffer, DWORD dwBufSize );
    void CloseUSB();
    BYTE GetStatus();
    BOOL GetDriverName( WCHAR*  pwcIndexString, DWORD  dwStrSize );
    BOOL GetDriverVersion( WCHAR*  pwcIndexString, DWORD  dwStrSize );
    
    BOOL DeviceChangeNortification( UINT  wParam, void*  lParam );
    BOOL CheckDeviceName( WCHAR*  pDevName );
    
private:
    BYTE                                    m_bUSBStatus;
    
    HINSTANCE                               m_hHidDll;
    GUID                                    m_idHid;
    
    HANDLE                                  m_hReadHandle;
    HANDLE                                  m_hWriteHandle;
    
    HDEVNOTIFY                              m_hNotification;
    
    PSP_DEVICE_INTERFACE_DETAIL_DATA        m_pstDevInterfaceDetail;
};
#endif //USBMANAGER_H
