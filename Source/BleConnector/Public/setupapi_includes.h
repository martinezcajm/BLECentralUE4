#pragma once

#pragma comment(lib, "SetupAPI")
#pragma warning( disable : 4068 )

#define NTDDI_VERSION NTDDI_WIN8
#define _WIN32_WINNT _WIN32_WINNT_WIN8


//Unreal need the redefinition of the setup api macros

//Redefinition used to indicate windows platform
//https://docs.microsoft.com/en-us/windows/win32/api/setupapi/ns-setupapi-sp_altplatform_info_v2
// V1 is for windows lesser than XP and V2 from XP to Vista
#define USE_SP_ALTPLATFORM_INFO_V1 0
#define USE_SP_ALTPLATFORM_INFO_V3 1

//Define this identifier only if your component must run on Windows 98 or Millennium Edition, or on Windows NT. 
//https://docs.microsoft.com/en-us/windows/win32/api/setupapi/ns-setupapi-sp_drvinfo_data_v1_w
#define USE_SP_DRVINFO_DATA_V1 0

//TODO no info could be found about this two, for now set to 0 as V1 is usually for lower windows version
//observing the others
#define USE_SP_BACKUP_QUEUE_PARAMS_V1 0
#define USE_SP_INF_SIGNER_INFO_V1 0


#include "AllowWindowsPlatformTypes.h"
#include "Windows/COMPointer.h"

#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <setupapi.h>

////GUID_DEVINTERFACE_* values
#include <uuids.h>


#include "HideWindowsPlatformTypes.h"