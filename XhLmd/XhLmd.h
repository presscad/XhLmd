#pragma once

#if defined(APP_EMBEDDED_MODULE)||defined(APP_EMBEDDED_MODULE_ENCRYPT)
#define XHLMD_API
#else
#ifdef XHLMD_EXPORTS
#define XHLMD_API __declspec(dllexport)
#else
#define XHLMD_API __declspec(dllimport)
#endif
#endif
//返回值1:授权成功;-1:授权失败;0:用户自行退出授权
XHLMD_API DWORD ProcessLicenseAuthorize(BYTE cProduct,DWORD dwVer[2],const char* sExePath,bool bRunning,const char* errormsg=NULL);
