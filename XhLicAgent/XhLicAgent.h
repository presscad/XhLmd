#ifndef __XH_LIC_AGENT_H_
#define __XH_LIC_AGENT_H_

// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
//��ı�׼�������� DLL �е������ļ��������������϶���� XHLICAGENT_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
//�κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ 
// XHLICAGENT_API ������Ϊ�ǴӴ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#pragma once

#ifdef APP_EMBEDDED_MODULE_ENCRYPT
#define XHLICAGENT_API	//��ǶʱXHLICAGENT_APIʲô��������
#else
#ifdef XHLICAGENT_EXPORTS
#define XHLICAGENT_API __declspec(dllexport)
#else
#define XHLICAGENT_API __declspec(dllimport)
#endif
#endif
//PRODUCT_CTMAΪ��ƽ���ܰ汾��TMA
enum XHPRODUCT_TYPE {
	PRODUCT_TMA = 1, PRODUCT_LMA, PRODUCT_TSA, PRODUCT_LDS, PRODUCT_TDA, PRODUCT_CTMA,
	PRODUCT_PAILIAO, PRODUCT_PMS, PRODUCT_MRP, PRODUCT_TAP, PRODUCT_TMS, PRODUCT_PNC, PRODUCT_IBOM,
	PRODUCT_TMDH, PRODUCT_CNC
};
XHLICAGENT_API UINT SetHaspLoginScope(const char* scope_xml_str);	//ָ��Hasp���ķ��ʷ�Χ
XHLICAGENT_API void InitializeParentProcessSerialNumber(UINT uiParentProcessSerialNumber);	//Ϊ�����ӽ������������趨�����̵ĵ�ǰ��ʱ���к�
XHLICAGENT_API UINT GetCurrentProcessSerialNumber();	//��ǰ��������ʱ��ʱ���к�
XHLICAGENT_API int ImportAuthorizeFile(const char* autfile,const char* szAppPath,char ciProductType,DWORD m_dwRandApplyCode=0);
XHLICAGENT_API int ImportLicBuffer(char* licbuf,int licbuf_len,BYTE product,DWORD version[2]);
XHLICAGENT_API int ImportSoftLicFile(const char* licfile,BYTE product,DWORD version[2]);
XHLICAGENT_API DWORD GetRentalLeastTime(DWORD *deadline=NULL);
XHLICAGENT_API DWORD GetLicVersion();
XHLICAGENT_API BOOL VerifyValidFunction(const char* identity);
XHLICAGENT_API BOOL VerifyValidFunction(DWORD dwFuncFlag);	//������V1.0.0.2������֮ǰ��֤��
XHLICAGENT_API int ImportLocalFeatureAccessControlFile(const char* facfile);
XHLICAGENT_API UINT ValidateLocalizeFeature(const char* identity);	//��֤�Ƿ����ָ�����ػ����ܵ�ʹ��Ȩ��
XHLICAGENT_API UINT DogSerialNo();		//��ǰ���ܹ���
XHLICAGENT_API bool FindDog(BYTE cDogType);			//�Ƿ�����ҵ���ǰ���ܹ�(�����Ƿ����ͨ�Ź���)
XHLICAGENT_API bool ConnectDog(BYTE cDogType);		//�Ƿ�����������ӵ�¼����ǰ���ܹ�(���Լ��������ʵ�¼�Ƿ�����)
XHLICAGENT_API BYTE AgentDogType();		//��ǰ���ܹ�����
XHLICAGENT_API BYTE GetProductType();	//��ǰ�����Ʒ��Ȩ
XHLICAGENT_API void GetProductAuthorizeInfo(BYTE info[8]);	//��ȡ��Ȩ����
XHLICAGENT_API bool ExitCurrentDogSession();	//�˳���ǰ�������Ự
//��֤����
XHLICAGENT_API DWORD DogSelfCrypt(char* data_in,UINT in_n,char* data_out);	//ʹ�ü��ܹ���Ĭ�ϼ����㷨����
XHLICAGENT_API DWORD DogSelfDecrypt(char* data_in,UINT in_n,char* data_out);	//ʹ�ü��ܹ���Ĭ�Ͻ����㷨����
XHLICAGENT_API DWORD EncryptFile(char* data_in,UINT in_n,char* data_out,bool bEncryptByAES=false);
XHLICAGENT_API DWORD DecryptFile(char* data_in,UINT in_n,char* data_out,bool bDecryptByAES=false);
XHLICAGENT_API DWORD EncryptFileEx(char* data_in,UINT in_n,char* data_out,char ciEncryptMode);
XHLICAGENT_API DWORD DecryptFileEx(char* data_in,UINT in_n,char* data_out,char ciDecryptMode/*=0*/,DWORD exter_keyid);
/////////////////////////////////////////////////////////////////////////////////////////////////////////
struct XHLICAGENT_API XHVERSION{
	union{
		DWORD dwVersion;	//��ʽ��Ȩ�汾��, ��V4.1��λ�ֽ�:4(major version),��λ�ֽ�:1(minor version);
		struct{
			BYTE cBuild;	//�������
			BYTE cRevision;	//�޶��汾��
			BYTE cMinorVer;	//�ΰ汾��
			BYTE cMajorVer;	//���汾��
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
//�޹���Ȩ����
struct XHLICENSESTATE{
	class EXPIRATION{
		union{
			WORD wExpiryDate;	//��Ȩ��ֹ����
			WORD wExpiryTimes;	//ʣ����Ȩʹ�ô���
		};
	public:
		EXPIRATION(WORD wExpiration=0){wExpiryTimes=wExpiration;}
		EXPIRATION(WORD year,WORD month,WORD day);
		operator WORD(){return wExpiryTimes;}
		bool IsExpired(BYTE cExpireType);
		DWORD DecreaseTimes();
		//����������������Խ�ֹ������Ȩ����
		short Year();
		short Month();
		short Day();
	};
	BYTE ciStatus;			//0.����ǰ��ʼ����֤�飻1.�������Ч֤�飻2.����ʧЧ֤��
	BYTE ciExpireType;		//0.��ֹ������Ȩ��1.ʹ�ô�����Ȩ
	EXPIRATION xExpire;		//��Ȩ��ֹ���ڻ���Ȩʹ��ʣ�����
	EXPIRATION xLatestUseDate;	//���һ��ʹ�ø�֤�������
	WORD wLatestUseTime;	//ÿ�����ʱ���Է���Ϊ��λ���ۼƼ�ʱ
	DWORD dwWorkgroupSerial;//���޹���Ȩ�����Ĺ��������к�(һ��Ϊ�û�Ӳ����������)
	DWORD dwRentalLeastSeconds;	//������Ȩ��ʣ�������
	DWORD dwReserved;		//�����ֽڣ����ڴ洢���޹���Ȩ֤���Ȩ����Ϣ
};
XHLICAGENT_API BOOL GetActivationCode(BYTE* c2v_data,UINT *puC2vDataLen);
XHLICAGENT_API BOOL ActivateLicense(BYTE* c2v_data,UINT uC2vDataLen);
XHLICAGENT_API int  RenewLicense(const char* lic_file);	//����Լ֤��
XHLICAGENT_API XHLICENSESTATE LicenseState(const char* lic_file);	//��ȡ��ǰ֤��״̬��Ϣ
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
�������뷵��0�����򷵻ش�����:
1:�޷���֤���ļ�
2:֤���ļ��⵽�ƻ�
3:֤������ܹ���ƥ��
4:��Ȩ֤��ļ��ܰ汾����
5:֤������ܹ���Ʒ��Ȩ�汾��ƥ��
6:�����汾ʹ����Ȩ��Χ
*/
XHLICAGENT_API int GetC2VInfo(BYTE cDogType,char* c2vinfo_buf,UINT uMaxC2VBufLen=0xFFFF);//DogType:MICRO_DOG=0;NET_DOG=1;HASP_DOG=2;
XHLICAGENT_API int UpdateByV2CInfoData(const char* v2cinfo_file);		//����V2C�ļ����ƴ��ļ����¼�����
XHLICAGENT_API int UpdateByV2CInfoData(char* v2cinfo_buf,DWORD buf_len);//����V2C���ݿ���¼�����
//XHLICAGENT_API DWORD GetProductFuncFlag();
//**********************************************************************
//�û��Զ��庯��
struct CRYPT_PROCITEM{
	char cType;	//0:��ʾ���ݣ�����ֵ��ʾ����
	union VALUE{
		DWORD dwVal;
		DWORD (*func)(DWORD dwParam);
	}val;
	CRYPT_PROCITEM(){cType=0;val.dwVal=0;}
};
//���ݱ��ش����ʼ��֤������
XHLICAGENT_API bool InitOrders(int group,CRYPT_PROCITEM* item_arr,DWORD length);
//��ȡ֤�������е�ָ��������Ŀ˳��
XHLICAGENT_API bool GetCallOrders(int group,DWORD* order_arr,DWORD length);
//�ӱ��ؾ�������������ָ����
XHLICAGENT_API DWORD CallProcItem(int group,int item_order,DWORD dwParam);
//**********************************************************************
//�����������Ϣ��ȡ��
// �����Ǵ� Computer.dll ������
struct XHLICAGENT_API COMPUTER
{
	unsigned char computer_ip[4];
	char sComputerName[51];
public:	
	COMPUTER(bool init=true);
	virtual ~COMPUTER(){;}
	bool DefaultIP(unsigned char* computerIP);
	int  DefaultComputerName(char* strComputerName,int nMaxStrBufLen=51);	//��ȡ��������
	//��ȡIP��ַ, ����ֵΪ��������IP��ַʱ����ǰ��ȡIP��ַ���(1Ϊ��ʼ������
	static int  GetIP(unsigned char* computer_ip=NULL,const char* hostname=NULL,int index=0);
	static bool GetComputerName(char* strComputerName,int nMaxStrBufLen=51);	//��ȡ��������
#ifdef APP_EMBEDDED_MODULE_ENCRYPT
	static bool Generate128Identity(BYTE key[32],DWORD* timestamp=NULL,BYTE* details=NULL,UINT uiDetailsMaxBufLen=201,BYTE* hardcode16bytes=NULL);
#else
	static bool Generate128Identity(BYTE key[32],DWORD* timestamp=NULL,BYTE* details=NULL,UINT uiDetailsMaxBufLen=201);
	static bool Generate128Identity(BYTE key[32],DWORD* timestamp,BYTE* details,UINT uiDetailsMaxBufLen,BYTE* hardcode16bytes);
#endif
	static bool Verify128Identity(BYTE key[32]);	//Ĭ����Ч��Ϊ7��
};
//**********************************************************************
//������Ȩ�����༰����
//������λ��ʶ�趨��
class XHLICAGENT_API BYTEFLAG{
	BYTE m_cFlag;
public:
	BYTEFLAG(BYTE cFlag=0){m_cFlag=cFlag;}
	operator BYTE(){return m_cFlag;}
	operator char(){return (char)m_cFlag;}
	BYTE operator =(BYTE cFlag){return m_cFlag=cFlag;}
	BYTE RemoveBitState(BYTE cFlag);
	BYTE SetBitState(BYTE bitpos,bool bSet=true);	//bitposȡֵ0~7
	bool GetBitState(BYTE bitpos);					//bitposȡֵ0~7
};
//������λ��ʶ�趨��
class XHLICAGENT_API BITFLAG{
	DWORD m_dwFlagWord;
public:
	BITFLAG(DWORD flag=0){m_dwFlagWord=flag;}
	DWORD FlagWord(){return m_dwFlagWord;}
	BITFLAG operator =(DWORD flag);
	//���ȫ����ʽ
	void Clear(){m_dwFlagWord=0;}
	//�����趨����������ѡ��ı�ʶ��,����ѡ�������ʶλ�ڵ�λ������ѡ���ʶλ�Ը�λ����
	DWORD SetFlagDword(DWORD dwFlag,BOOL bSet=TRUE);
	//�Ƿ����ĳ�ֱ�ʶ�趨
	BOOL IsHasFlag(DWORD dwFlag);
};
//������֧�ֻ��⹦�ܵı�ʶ�趨��
class XHLICAGENT_API MASKFLAG{
	DWORD m_dwFlagWord;
	int m_nMaskBitCount;
public:
	MASKFLAG(DWORD dwFlagWord=0,int maskBitCount=8);
	int SetMaskBitCount(int maskBitCount);
	DWORD FlagWord(){return m_dwFlagWord;}
	int   MaskBitCount(){return m_nMaskBitCount;}
	//���ȫ����ʽ
	void Clear(){m_dwFlagWord=0;}
	//�����趨����������ѡ��ı�ʶ��,����ѡ�������ʶλ�ڵ�λ������ѡ���ʶλ�Ը�λ����
	DWORD SetFlagDword(DWORD dwFlag,BOOL bSet=TRUE);
	//�Ƿ����ĳ�ֱ�ʶ�趨
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
//-------------VVV--�����������--VVV-------------------
//���ַ����͵İ汾��ת��Ϊһ������������"1.01.3.21"-->1 01 03 21
XHLICAGENT_API long FromStringVersion(const char* version);
//����������ʽ�İ汾��ת��Ϊһ���ַ����Ͱ汾�ţ���1010321-->"1.01.3.21"
XHLICAGENT_API int ToStringVersion(UINT nVersion,char* version);
//�������ʾ�汾��1��,�������ʾͬ�汾�������ʾ�汾��1��
XHLICAGENT_API int compareVersion(const char* version1,const char* version2);
//�������ʽ��ΪDWORD������ĺŶ���
XHLICAGENT_API DWORD GetSingleWord(long iNo); //��1��ʼ����
//-------------VVV--�汾��������֤��������--VVV-------------------
XHLICAGENT_API DWORD LicAgentDllGetVersion();
XHLICAGENT_API bool ValidateLicAgentVersionCompatible(char* errmsg=NULL,int nMsgStrBuffLen=50);
#endif