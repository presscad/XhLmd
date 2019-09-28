#include "XhCharString.h"
#include "XhLicAgent.h"
#include "LicenseManager.h"
#include "iphlpapi.h" 

#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"iphlpapi.lib")
//
#ifdef __ALL_LICFUNC_
extern BYTE PRODUCT_TYPE;
extern char execute_path[MAX_PATH];
extern DWORD dwVersion[2];
#endif

template<class TYPE> class DATA_PTR{
	int  m_iCurrElem;
	TYPE* m_data;
public:
	DATA_PTR(BYTE* data,int iElem=0)
	{
		m_iCurrElem=iElem;
		m_data=(TYPE*)data;
	}
	DATA_PTR(char* data,int iElem=0)
	{
		m_iCurrElem=iElem;
		m_data=(TYPE*)data;
	}
	operator TYPE&(){return m_data[m_iCurrElem];}
	TYPE& operator =(const TYPE& v){return m_data[m_iCurrElem]=v;}
	TYPE& operator[](int i)//���������Ż�ȡ�߶�ʵ��
	{
		return m_data[i];
	}
};
typedef DATA_PTR< DWORD > DWORDPTR;
typedef DATA_PTR<__int64> INT64PTR;

//int LocalProductId2ServerProductId(int local_produt_id);
int InternalImportLicFile(char* licfile,BYTE cProductType,DWORD version[2]);
CXhChar16 GetProductName();
struct APL_INFO{
	DWORD m_dwRandApplyCode;	//���������
	bool m_bNewApplyRandCode;
	const static BYTE STATE_NEED_APPLY				= 0;	//��ʼ״̬����Ҫ����������Ȩ
	const static BYTE STATE_APPROVED_APPLY			= 1;	//��ͨ����˵�
	const static BYTE STATE_CUR_PRODUCT_NEED_APPLY	= 2;	//�����ţ�����ǰ��Ʒδ��Ȩ
	BYTE m_biApplyAutState;		//��¼��ǰ�û��Ƿ��������������Ȩ 18-09-09
	//
	DWORD dwDetailBytes;
	DWORD dwLicApplyTimeStamp;
	BYTE key[32],details[192],hardidcode[16];
	BYTE IP[4],IP2[4],MAC[6];
	CXhChar200 sComputerName;
	APL_INFO(bool init=false){
		m_dwRandApplyCode=0;
		m_bNewApplyRandCode=false;
		m_biApplyAutState=0;
		memset(key,0,32);
		memset(details,0,192);
		memset(hardidcode,0,16);
		memset(IP,0,4);
		memset(IP2,0,4);
		memset(MAC,0,6);
		dwDetailBytes=dwLicApplyTimeStamp=0;
		if(init)
			Init();
	}
	void Init(){
		//��ʼ�������������Ϣ
		COMPUTER::Generate128Identity(key,&dwLicApplyTimeStamp,details,192,hardidcode);
		for(dwDetailBytes=0;dwDetailBytes<192;dwDetailBytes+=16)
		{
			INT64PTR i64(details+dwDetailBytes);
			if(i64[0]==0&&i64[1]==0)
				break;
		}
		//��ʼ��IP
		COMPUTER::GetComputerName(sComputerName,50);
		InitMacAddress();
		BYTE ciCount=(BYTE)COMPUTER::GetIP(NULL,sComputerName,0);
		BYTE ipv4[4];
		for(BYTE i=0;i<ciCount;i++)
		{
			ipv4[0]=ipv4[1]=ipv4[2]=ipv4[3]=0;
			COMPUTER::GetIP(ipv4,sComputerName,i);
			if(i==0)
				memcpy(IP,ipv4,4);
			else if(i==1)
				memcpy(IP2,ipv4,4);
			else
				break;
		}
		//��ʼ����Ȩ��
		srand((DWORD)time(NULL));
		DWORD s0=rand();
		m_bNewApplyRandCode=false;
		m_dwRandApplyCode=_LocalRandDword();
	}
	CXhChar50 GetHardIdCodeStr(){
		CXhChar50 hardcodestr;
		for(int i=0;i<16;i++)
		{
			if(hardidcode[i]>0x0f)
				hardcodestr.Append(CXhChar16("%2X",hardidcode[i]));
			else
				hardcodestr.Append(CXhChar16("0%X",hardidcode[i]));
		}
		return hardcodestr;
	}
	CXhChar16 GetNonceStr(){
		return CXhChar16("%d",m_dwRandApplyCode);
	}
	DWORD _LocalRandDword()
	{
		DWORD seed=0;
		seed=rand();
		seed<<=16;
		DWORD lowpart=rand();
		seed|=(0x0000ffff&lowpart);
		return seed;
	}
#ifdef __WXPAY_RENTAL_ONLINE_
	bool InitEndUserBasiceInfo(CEndUser &endUser){
		memcpy(endUser.computer_ip,IP,4);
		memcpy(endUser.computer_ip2,IP2,4);
		endUser.m_sComputerName=sComputerName;
		memcpy(endUser.computer_mac,MAC,6);
		memcpy(endUser.hardIdCode,hardidcode,16);
		memcpy(endUser.fingerprint,key,32);
		memcpy(endUser.fingerDetail,details,192);
		endUser.m_dwApplyTimeStamp=dwLicApplyTimeStamp;
		return true;
	}
	bool InitOrder(COrder &order,int endUserId,CProductRental &productRental){
		if(endUserId<=0)
			return false;
		order.endUserId=endUserId;
		order.fingerHardId=GetHardIdCodeStr();
		order.rentalDays=productRental.rentalDays;
		order.money=productRental.money;
		order.applyRandCode=m_dwRandApplyCode;
		order.applyTimeStamp=dwLicApplyTimeStamp;
		order.productId = PRODUCT_TYPE;
		return true;
	}
#endif
	BOOL InitMacAddress()
	{
		PIP_ADAPTER_INFO pAdapterInfo;
		DWORD AdapterInfoSize;
		DWORD Err;
		AdapterInfoSize=0;
		Err=GetAdaptersInfo(NULL,&AdapterInfoSize);
		if((Err!=0)&&(Err!=ERROR_BUFFER_OVERFLOW)){
			TRACE("���������Ϣʧ�ܣ�");
			return FALSE;
		}
		//   ����������Ϣ�ڴ�
		pAdapterInfo=(PIP_ADAPTER_INFO)GlobalAlloc(GPTR,AdapterInfoSize);
		if(pAdapterInfo==NULL){
			TRACE("����������Ϣ�ڴ�ʧ��");
			return FALSE;
		}
		if(GetAdaptersInfo(pAdapterInfo,&AdapterInfoSize)!=0){
			TRACE(_T("���������Ϣʧ�ܣ�\n"));
			GlobalFree(pAdapterInfo);
			return   FALSE;
		}
		for(int i=0;i<6;i++)
			MAC[i]=pAdapterInfo->Address[i];
		GlobalFree(pAdapterInfo);
		return TRUE;
	}
};
extern APL_INFO g_aplInfo;
BOOL IsVerifyValidFunction();
void WriteProfileIntToReg(LPCTSTR lpszSection,LPCTSTR lpszEntry,DWORD dwValue);
void WriteProfileStringToReg(LPCTSTR lpszSection,LPCTSTR lpszEntry,const char* sValue);
BOOL WriteProfileBinaryToReg(LPCTSTR lpszSection,LPCTSTR lpszEntry,BYTE* biValue,DWORD dwLen);
int GetProfileIntFromReg(LPCTSTR lpszSection,LPCTSTR lpszEntry);
CString GetProfileStringFromReg(LPCTSTR lpszSection,LPCTSTR lpszEntry);