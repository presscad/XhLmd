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
//����ֵ1:��Ȩ�ɹ�;-1:��Ȩʧ��;0:�û������˳���Ȩ
XHLMD_API DWORD ProcessLicenseAuthorize(BYTE cProduct,DWORD dwVer[2],const char* sExePath,bool bRunning,const char* errormsg=NULL);
