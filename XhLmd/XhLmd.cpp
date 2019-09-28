// XhLmd.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include <afxwin.h>
#include <afxdllx.h>
#include "ArrayList.h"
#include "XhLmd.h"
#include "LicFuncDef.h"
#include "LicenseAuthorizeDlg.h"
#include "XhLmdI.h"
#include "ProductDef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/*static */AFX_EXTENSION_MODULE XhLmdDLL = { NULL, NULL };

#ifndef APP_EMBEDDED_MODULE_ENCRYPT
extern "C" int APIENTRY
	DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// 如果使用 lpReserved，请将此移除
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("XhLmd.DLL 正在初始化!\n");

		// 扩展 DLL 一次性初始化
		if (!AfxInitExtensionModule(XhLmdDLL, hInstance))
			return 0;

		// 将此 DLL 插入到资源链中
		// 注意: 如果此扩展 DLL 由
		//  MFC 规则 DLL (如 ActiveX 控件)隐式链接到，
		//  而不是由 MFC 应用程序链接到，则需要
		//  将此行从 DllMain 中移除并将其放置在一个
		//  从此扩展 DLL 导出的单独的函数中。使用此扩展 DLL 的
		//  规则 DLL 然后应显式
		//  调用该函数以初始化此扩展 DLL。否则，
		//  CDynLinkLibrary 对象不会附加到
		//  规则 DLL 的资源链，并将导致严重的
		//  问题。

		new CDynLinkLibrary(XhLmdDLL);

	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("XhLmd.DLL 正在终止!\n");

		// 在调用析构函数之前终止该库
		AfxTermExtensionModule(XhLmdDLL);
	}
	return 1;   // 确定
}
#endif
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
BYTE PRODUCT_TYPE=1;
char execute_path[MAX_PATH]="";
DWORD dwVersion[2]={0,0};
APL_INFO g_aplInfo;

int InternalImportLicFile(char* licfile,BYTE cProductType,DWORD version[2])
{
	FILE* fp=fopen(licfile,"rb");
	if(fp==NULL)
		return 1;	//无法打开证书文件
	fseek(fp,0,SEEK_END);
	UINT uBufLen=ftell(fp);	//获取认证文件长度
	CHAR_ARRAY buffer_s(uBufLen);
	fseek(fp,0,SEEK_SET);
	fread(buffer_s,uBufLen,1,fp);
	fclose(fp);
	return ImportLicBuffer(buffer_s,uBufLen,cProductType,version);
}
/*
int LocalProductId2ServerProductId(int local_produt_id)
{
	if(local_produt_id==PRODUCT_PNC)
		return 16;
	else if(local_produt_id==PRODUCT_IBOM)
		return 17;
	else if(local_produt_id==PRODUCT_TAP)
		return 13;
	else if(local_produt_id==PRODUCT_PAILIAO)
		return 6;
	else
		return local_produt_id;
}*/

CXhChar16 GetProductName()
{
	CXhChar16 product_name;
	if (PRODUCT_TYPE == PRODUCT_TMA)
		product_name.Copy("TMA");
	else if (PRODUCT_TYPE == PRODUCT_LMA)
		product_name.Copy("LMA");
	else if (PRODUCT_TYPE == PRODUCT_TSA)
		product_name.Copy("TSA");
	else if (PRODUCT_TYPE == PRODUCT_LDS)
		product_name.Copy("LDS");
	else if (PRODUCT_TYPE == PRODUCT_TDA)
		product_name.Copy("TDA");
	else if (PRODUCT_TYPE == PRODUCT_PAILIAO)
		product_name.Copy("PaiLiao");
	else if (PRODUCT_TYPE == PRODUCT_PMS)
		product_name.Copy("PMS");
	else if (PRODUCT_TYPE == PRODUCT_TAP)
		product_name.Copy("TAP");
	else if (PRODUCT_TYPE == PRODUCT_PNC)
		product_name.Copy("PNC");
	else if (PRODUCT_TYPE == PRODUCT_IBOM)
		product_name.Copy("IBOM");
	else if (PRODUCT_TYPE == XHPRODUCT_TYPEEX::PRODUCT_TMDH)
		product_name.Copy("GIM");
	else if (PRODUCT_TYPE == XHPRODUCT_TYPEEX::PRODUCT_CNC)
		product_name.Copy("CNC");
	return product_name;
}
BOOL IsVerifyValidFunction()
{
	if(PRODUCT_TYPE==PRODUCT_TMA)
		return VerifyValidFunction(TMA_LICFUNC::FUNC_IDENTITY_BASIC);
	else if(PRODUCT_TYPE==PRODUCT_LMA)
		return VerifyValidFunction(LMA_LICFUNC::FUNC_IDENTITY_BASIC);
	else if(PRODUCT_TYPE==PRODUCT_TSA)
		return VerifyValidFunction(TSA_LICFUNC::FUNC_IDENTITY_BASIC);
	else if(PRODUCT_TYPE==PRODUCT_LDS)
		return VerifyValidFunction(LDS_LICFUNC::FUNC_IDENTITY_BASIC);
	else if(PRODUCT_TYPE==PRODUCT_TDA)
		return VerifyValidFunction(TDA_LICFUNC::FUNC_IDENTITY_BASIC);
	else if(PRODUCT_TYPE==PRODUCT_PAILIAO)
		return VerifyValidFunction(PAILIAO_LICFUNC::FUNC_IDENTITY_BASIC);
	else if(PRODUCT_TYPE==PRODUCT_PMS)
		return VerifyValidFunction(PMS_LICFUNC::FUNC_IDENTITY_BASIC);
	else if(PRODUCT_TYPE==PRODUCT_TAP)
		return VerifyValidFunction(TAP_LICFUNC::FUNC_IDENTITY_BASIC);
	else if(PRODUCT_TYPE==PRODUCT_PNC)
		return VerifyValidFunction(PNC_LICFUNC::FUNC_IDENTITY_BASIC);
	else if(PRODUCT_TYPE==PRODUCT_IBOM)
		return VerifyValidFunction(IBOM_LICFUNC::FUNC_IDENTITY_BASIC);
	else if (PRODUCT_TYPE == XHPRODUCT_TYPEEX::PRODUCT_TMDH)
		return VerifyValidFunction(TMDH_LICFUNC::FUNC_IDENTITY_BASIC);
	else if (PRODUCT_TYPE == XHPRODUCT_TYPEEX::PRODUCT_CNC)
		return VerifyValidFunction(CNC_LICFUNC::FUNC_IDENTITY_BASIC);
	else
		return FALSE;
}
void WriteProfileIntToReg(LPCTSTR lpszSection,LPCTSTR lpszEntry,DWORD dwValue)
{
	char sSubKey[MAX_PATH]="";
	sprintf(sSubKey,"Software\\Xerofox\\%s\\%s",(char*)GetProductName(),lpszSection);
	DWORD dwLength=sizeof(DWORD);
	HKEY hKey;
	if(RegOpenKeyEx(HKEY_CURRENT_USER,sSubKey,0,KEY_WRITE,&hKey)==ERROR_SUCCESS&&hKey)
	{
		RegSetValueEx(hKey,lpszEntry,NULL,REG_DWORD,(BYTE*)&dwValue,dwLength);
		RegCloseKey(hKey);
	}
}
void WriteProfileStringToReg(LPCTSTR lpszSection,LPCTSTR lpszEntry,const char* sValue)
{
	char sSubKey[MAX_PATH]="";
	sprintf(sSubKey,"Software\\Xerofox\\%s\\%s",(char*)GetProductName(),lpszSection);
	DWORD dwLength=strlen(sValue);
	HKEY hKey;
	if(RegOpenKeyEx(HKEY_CURRENT_USER,sSubKey,0,KEY_WRITE,&hKey)==ERROR_SUCCESS&&hKey)
	{
		RegSetValueEx(hKey,lpszEntry,NULL,REG_SZ,(BYTE*)&sValue[0],dwLength);
		RegCloseKey(hKey);
	}
}
BOOL WriteProfileBinaryToReg(LPCTSTR lpszSection,LPCTSTR lpszEntry,BYTE* biValue,DWORD dwLen)
{
	char sSubKey[MAX_PATH]="";
	sprintf(sSubKey,"Software\\Xerofox\\%s\\%s",(char*)GetProductName(),lpszSection);
	HKEY hKey;
	if(RegOpenKeyEx(HKEY_CURRENT_USER,sSubKey,0,KEY_WRITE,&hKey)==ERROR_SUCCESS&&hKey)
	{
		RegSetValueEx(hKey,lpszEntry,NULL,REG_BINARY,(BYTE*)biValue,dwLen);
		RegCloseKey(hKey);
		return TRUE;
	}
	return FALSE;
}
int GetProfileIntFromReg(LPCTSTR lpszSection,LPCTSTR lpszEntry)
{
	char sSubKey[MAX_PATH]="";
	sprintf(sSubKey,"Software\\Xerofox\\%s\\%s",(char*)GetProductName(),lpszSection);
	DWORD dwDataType,dwValue=0,dwLength=sizeof(DWORD);
	HKEY hKey;
	if(RegOpenKeyEx(HKEY_CURRENT_USER,sSubKey,0,KEY_READ,&hKey)==ERROR_SUCCESS&&hKey)
	{
		if(RegQueryValueEx(hKey,lpszEntry,NULL,&dwDataType,(BYTE*)&dwValue,&dwLength)== ERROR_SUCCESS)
			RegCloseKey(hKey);
	}
	return (UINT)dwValue;
}
CString GetProfileStringFromReg(LPCTSTR lpszSection,LPCTSTR lpszEntry)
{
	char sSubKey[MAX_PATH]="",sValue[MAX_PATH];;
	sprintf(sSubKey,"Software\\Xerofox\\%s\\%s",(char*)GetProductName(),lpszSection);
	DWORD dwDataType,dwLength=MAX_PATH;
	HKEY hKey;
	if(RegOpenKeyEx(HKEY_CURRENT_USER,sSubKey,0,KEY_READ,&hKey)==ERROR_SUCCESS&&hKey)
	{
		if(RegQueryValueEx(hKey,lpszEntry,NULL,&dwDataType,(BYTE*)&sValue[0],&dwLength)== ERROR_SUCCESS)
			RegCloseKey(hKey);
	}
	return CString(sValue);
}
//
XHLMD_API DWORD ProcessLicenseAuthorize(BYTE cProduct,DWORD dwVer[2],const char* sExePath,bool bRunning,const char* errormsg/*=NULL*/)
{
	PRODUCT_TYPE=cProduct;
	strcpy(execute_path,sExePath);
	dwVersion[0]=dwVer[0];
	dwVersion[1]=dwVer[1];
	char lic_file[MAX_PATH];
	sprintf(lic_file,"%s%s.lic",execute_path,(char*)GetProductName());
	//
#if !(defined(APP_EMBEDDED_MODULE)||defined(APP_EMBEDDED_MODULE_ENCRYPT))
	CModuleResourceOverride resOverride;
#endif
	CLicenseAuthorizeDlg authdlg;
	authdlg.m_bRunning=bRunning;
	if(errormsg!=NULL)
		authdlg.m_sErrorMsg=errormsg;
	authdlg.InitLicenseModeByLicFile(lic_file);
	if(authdlg.DoModal() == IDOK)
	{
		if(authdlg.IsLicfileImported())
			return 1;	//授权成功	
		else
			return -1;	//授权失败	
	}
	else
		return 0;	//用户退出	
}
