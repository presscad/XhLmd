#ifndef __XH_LIC_AGENT_H_
#define __XH_LIC_AGENT_H_

// 下列 ifdef 块是创建使从 DLL 导出更简单的
//宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 XHLICAGENT_EXPORTS
// 符号编译的。在使用此 DLL 的
//任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将 
// XHLICAGENT_API 函数视为是从此 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#pragma once

#ifdef APP_EMBEDDED_MODULE_ENCRYPT
#define XHLICAGENT_API	//内嵌时XHLICAGENT_API什么都不代表
#else
#ifdef XHLICAGENT_EXPORTS
#define XHLICAGENT_API __declspec(dllexport)
#else
#define XHLICAGENT_API __declspec(dllimport)
#endif
#endif
//PRODUCT_CTMA为四平加密版本的TMA
enum XHPRODUCT_TYPE {
	PRODUCT_TMA = 1, PRODUCT_LMA, PRODUCT_TSA, PRODUCT_LDS, PRODUCT_TDA, PRODUCT_CTMA,
	PRODUCT_PAILIAO, PRODUCT_PMS, PRODUCT_MRP, PRODUCT_TAP, PRODUCT_TMS, PRODUCT_PNC, PRODUCT_IBOM,
	PRODUCT_TMDH, PRODUCT_CNC
};
XHLICAGENT_API UINT SetHaspLoginScope(const char* scope_xml_str);	//指定Hasp锁的访问范围
XHLICAGENT_API void InitializeParentProcessSerialNumber(UINT uiParentProcessSerialNumber);	//为加速子进程启动可以设定父进程的当前临时序列号
XHLICAGENT_API UINT GetCurrentProcessSerialNumber();	//当前进程启动时临时序列号
XHLICAGENT_API int ImportAuthorizeFile(const char* autfile,const char* szAppPath,char ciProductType,DWORD m_dwRandApplyCode=0);
XHLICAGENT_API int ImportLicBuffer(char* licbuf,int licbuf_len,BYTE product,DWORD version[2]);
XHLICAGENT_API int ImportSoftLicFile(const char* licfile,BYTE product,DWORD version[2]);
XHLICAGENT_API DWORD GetRentalLeastTime(DWORD *deadline=NULL);
XHLICAGENT_API DWORD GetLicVersion();
XHLICAGENT_API BOOL VerifyValidFunction(const char* identity);
XHLICAGENT_API BOOL VerifyValidFunction(DWORD dwFuncFlag);	//仅用于V1.0.0.2（含）之前的证书
XHLICAGENT_API int ImportLocalFeatureAccessControlFile(const char* facfile);
XHLICAGENT_API UINT ValidateLocalizeFeature(const char* identity);	//验证是否具有指定本地化功能的使用权限
XHLICAGENT_API UINT DogSerialNo();		//当前加密狗号
XHLICAGENT_API bool FindDog(BYTE cDogType);			//是否可以找到当前加密狗(测试是否存在通信故障)
XHLICAGENT_API bool ConnectDog(BYTE cDogType);		//是否可以正常连接登录到当前加密狗(测试加密锁访问登录是否正常)
XHLICAGENT_API BYTE AgentDogType();		//当前加密狗类型
XHLICAGENT_API BYTE GetProductType();	//当前导入产品授权
XHLICAGENT_API void GetProductAuthorizeInfo(BYTE info[8]);	//获取授权类型
XHLICAGENT_API bool ExitCurrentDogSession();	//退出当前加密锁会话
//认证函数
XHLICAGENT_API DWORD DogSelfCrypt(char* data_in,UINT in_n,char* data_out);	//使用加密狗的默认加密算法加密
XHLICAGENT_API DWORD DogSelfDecrypt(char* data_in,UINT in_n,char* data_out);	//使用加密狗的默认解密算法解密
XHLICAGENT_API DWORD EncryptFile(char* data_in,UINT in_n,char* data_out,bool bEncryptByAES=false);
XHLICAGENT_API DWORD DecryptFile(char* data_in,UINT in_n,char* data_out,bool bDecryptByAES=false);
XHLICAGENT_API DWORD EncryptFileEx(char* data_in,UINT in_n,char* data_out,char ciEncryptMode);
XHLICAGENT_API DWORD DecryptFileEx(char* data_in,UINT in_n,char* data_out,char ciDecryptMode/*=0*/,DWORD exter_keyid);
/////////////////////////////////////////////////////////////////////////////////////////////////////////
struct XHLICAGENT_API XHVERSION{
	union{
		DWORD dwVersion;	//正式授权版本号, 如V4.1低位字节:4(major version),高位字节:1(minor version);
		struct{
			BYTE cBuild;	//编译序号
			BYTE cRevision;	//修订版本号
			BYTE cMinorVer;	//次版本号
			BYTE cMajorVer;	//主版本号
		};
	};
	XHVERSION(DWORD dwVer){dwVersion=dwVer;}
	XHVERSION(const char* version=NULL);
	operator DWORD(){return dwVersion;}
	bool ToString(char* version,char level=4);
	int  Compare(const XHVERSION& version,char level=4);
	int  Compare(const char* version,char level=4);
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//无狗授权函数
struct XHLICENSESTATE{
	class EXPIRATION{
		union{
			WORD wExpiryDate;	//授权截止日期
			WORD wExpiryTimes;	//剩余授权使用次数
		};
	public:
		EXPIRATION(WORD wExpiration=0){wExpiryTimes=wExpiration;}
		EXPIRATION(WORD year,WORD month,WORD day);
		operator WORD(){return wExpiryTimes;}
		bool IsExpired(BYTE cExpireType);
		DWORD DecreaseTimes();
		//以下三个函数仅针对截止日期授权类型
		short Year();
		short Month();
		short Day();
	};
	BYTE ciStatus;			//0.激活前初始种子证书；1.激活后有效证书；2.过期失效证书
	BYTE ciExpireType;		//0.截止日期授权；1.使用次数授权
	EXPIRATION xExpire;		//授权截止日期或授权使用剩余次数
	EXPIRATION xLatestUseDate;	//最近一次使用该证书的日期
	WORD wLatestUseTime;	//每天从零时起，以分钟为单位的累计计时
	DWORD dwWorkgroupSerial;//该无狗授权从属的工作组序列号(一般为用户硬件加密锁号)
	DWORD dwRentalLeastSeconds;	//无锁授权的剩余租借期
	DWORD dwReserved;		//保留字节，用于存储该无狗授权证书的权限信息
};
XHLICAGENT_API BOOL GetActivationCode(BYTE* c2v_data,UINT *puC2vDataLen);
XHLICAGENT_API BOOL ActivateLicense(BYTE* c2v_data,UINT uC2vDataLen);
XHLICAGENT_API int  RenewLicense(const char* lic_file);	//再续约证书
XHLICAGENT_API XHLICENSESTATE LicenseState(const char* lic_file);	//获取当前证书状态信息
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
正常导入返回0，否则返回错误码:
1:无法打证书文件
2:证书文件遭到破坏
3:证书与加密狗不匹配
4:授权证书的加密版本不对
5:证书与加密狗产品授权版本不匹配
6:超出版本使用授权范围
*/
XHLICAGENT_API int GetC2VInfo(BYTE cDogType,char* c2vinfo_buf,UINT uMaxC2VBufLen=0xFFFF);//DogType:MICRO_DOG=0;NET_DOG=1;HASP_DOG=2;
XHLICAGENT_API int UpdateByV2CInfoData(const char* v2cinfo_file);		//根据V2C文件名称打开文件更新加密锁
XHLICAGENT_API int UpdateByV2CInfoData(char* v2cinfo_buf,DWORD buf_len);//根据V2C数据块更新加密锁
//XHLICAGENT_API DWORD GetProductFuncFlag();
//**********************************************************************
//用户自定义函数
struct CRYPT_PROCITEM{
	char cType;	//0:表示数据，其余值表示函数
	union VALUE{
		DWORD dwVal;
		DWORD (*func)(DWORD dwParam);
	}val;
	CRYPT_PROCITEM(){cType=0;val.dwVal=0;}
};
//根据本地代码初始化证书容器
XHLICAGENT_API bool InitOrders(int group,CRYPT_PROCITEM* item_arr,DWORD length);
//获取证书容器中的指定组内项目顺序
XHLICAGENT_API bool GetCallOrders(int group,DWORD* order_arr,DWORD length);
//从本地经容器调用组内指定项
XHLICAGENT_API DWORD CallProcItem(int group,int item_order,DWORD dwParam);
//**********************************************************************
//计算机主机信息提取类
// 此类是从 Computer.dll 导出的
struct XHLICAGENT_API COMPUTER
{
	unsigned char computer_ip[4];
	char sComputerName[51];
public:	
	COMPUTER(bool init=true);
	virtual ~COMPUTER(){;}
	bool DefaultIP(unsigned char* computerIP);
	int  DefaultComputerName(char* strComputerName,int nMaxStrBufLen=51);	//获取电脑名称
	//获取IP地址, 返回值为主机多重IP地址时，当前获取IP地址序号(1为起始基数）
	static int  GetIP(unsigned char* computer_ip=NULL,const char* hostname=NULL,int index=0);
	static bool GetComputerName(char* strComputerName,int nMaxStrBufLen=51);	//获取电脑名称
#ifdef APP_EMBEDDED_MODULE_ENCRYPT
	static bool Generate128Identity(BYTE key[32],DWORD* timestamp=NULL,BYTE* details=NULL,UINT uiDetailsMaxBufLen=201,BYTE* hardcode16bytes=NULL);
#else
	static bool Generate128Identity(BYTE key[32],DWORD* timestamp=NULL,BYTE* details=NULL,UINT uiDetailsMaxBufLen=201);
	static bool Generate128Identity(BYTE key[32],DWORD* timestamp,BYTE* details,UINT uiDetailsMaxBufLen,BYTE* hardcode16bytes);
#endif
	static bool Verify128Identity(BYTE key[32]);	//默认有效期为7天
};
//**********************************************************************
//加密授权工具类及函数
//单纯的位标识设定类
class XHLICAGENT_API BYTEFLAG{
	BYTE m_cFlag;
public:
	BYTEFLAG(BYTE cFlag=0){m_cFlag=cFlag;}
	operator BYTE(){return m_cFlag;}
	operator char(){return (char)m_cFlag;}
	BYTE operator =(BYTE cFlag){return m_cFlag=cFlag;}
	BYTE RemoveBitState(BYTE cFlag);
	BYTE SetBitState(BYTE bitpos,bool bSet=true);	//bitpos取值0~7
	bool GetBitState(BYTE bitpos);					//bitpos取值0~7
};
//单纯的位标识设定类
class XHLICAGENT_API BITFLAG{
	DWORD m_dwFlagWord;
public:
	BITFLAG(DWORD flag=0){m_dwFlagWord=flag;}
	DWORD FlagWord(){return m_dwFlagWord;}
	BITFLAG operator =(DWORD flag);
	//清空全部格式
	void Clear(){m_dwFlagWord=0;}
	//用于设定含有排它性选项的标识字,排它选项掩码标识位在低位，排它选项标识位自高位排列
	DWORD SetFlagDword(DWORD dwFlag,BOOL bSet=TRUE);
	//是否具有某种标识设定
	BOOL IsHasFlag(DWORD dwFlag);
};
//带掩码支持互斥功能的标识设定类
class XHLICAGENT_API MASKFLAG{
	DWORD m_dwFlagWord;
	int m_nMaskBitCount;
public:
	MASKFLAG(DWORD dwFlagWord=0,int maskBitCount=8);
	int SetMaskBitCount(int maskBitCount);
	DWORD FlagWord(){return m_dwFlagWord;}
	int   MaskBitCount(){return m_nMaskBitCount;}
	//清空全部格式
	void Clear(){m_dwFlagWord=0;}
	//用于设定含有排它性选项的标识字,排它选项掩码标识位在低位，排它选项标识位自高位排列
	DWORD SetFlagDword(DWORD dwFlag,BOOL bSet=TRUE);
	//是否具有某种标识设定
	BOOL IsHasFlag(DWORD dwFlag);
};
class XHLICAGENT_API VALUE_SCOPE{
	bool bInitialized;
	double _min_val,_max_val;
public:
	VALUE_SCOPE();
	void Clear();
	bool IsInitialized(){return bInitialized;}
	int VerifyValue(double val);
	double MaxValue();
	double MinValue();
};
//-------------VVV--输出函数声明--VVV-------------------
//将字符串型的版本号转换为一个长整数，如"1.01.3.21"-->1 01 03 21
XHLICAGENT_API long FromStringVersion(const char* version);
//将长整数型式的版本号转换为一个字符串型版本号，如1010321-->"1.01.3.21"
XHLICAGENT_API int ToStringVersion(UINT nVersion,char* version);
//大于零表示版本号1高,等于零表示同版本，否则表示版本号1低
XHLICAGENT_API int compareVersion(const char* version1,const char* version2);
//由序号形式变为DWORD类型配材号对象
XHLICAGENT_API DWORD GetSingleWord(long iNo); //从1开始计数
//-------------VVV--版本兼容性验证函数声明--VVV-------------------
XHLICAGENT_API DWORD LicAgentDllGetVersion();
XHLICAGENT_API bool ValidateLicAgentVersionCompatible(char* errmsg=NULL,int nMsgStrBuffLen=50);
#endif