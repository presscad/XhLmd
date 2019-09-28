#pragma once
#include "LicenseServerActivatePage.h"
#include "LicenseVirtualRentalPage.h"
#include "LinkLabel.h"
#include "TempFile.h"
#include "atom_list.h"

/////////////////////////////////////////////////
extern AFX_EXTENSION_MODULE XhLmdDLL;
class CModuleResourceOverride {
	BOOL pushed;
	static  CStack<HINSTANCE> m_xResModuleStack;
public:
	CModuleResourceOverride ()
	{
		if(AfxGetModuleState()->m_hCurrentResourceHandle==NULL)
			pushed=FALSE;
		else
			pushed=m_xResModuleStack.push(AfxGetResourceHandle());
		AfxSetResourceHandle(XhLmdDLL.hResource);
	}
	~CModuleResourceOverride ()
	{
		if(pushed)
		{
			HINSTANCE hInst;
			if(m_xResModuleStack.pop(hInst))
				AfxSetResourceHandle(hInst);
		}
	}
};

// CLicenseAuthorizeDlg �Ի���

class CLicenseAuthorizeDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CLicenseAuthorizeDlg)
	CRect workpanelRc;
	bool m_bInternalImported;	//�ڲ��������ص�֤�飬����Ҫˢ��UI wht 19-06-09
	bool m_bLicfileImported;
	CTempFileBuffer* m_pErrLogFile;
	CLicenseServerActivatePage licServerPage;
	CLicenseVirtualRentalPage  licRentalPage;
public:
	CLicenseAuthorizeDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CLicenseAuthorizeDlg();
	static bool _ImportLicFile(char* licfilename, char* errorMsg);

// �Ի�������
	enum { IDD = IDD_LICENSE_AHTORIZE_DLG };

protected:
	CLinkLabel m_ctrlErrorMsg;
	bool _ImportLicFile(char* licfilename);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnDogTest();
	afx_msg void OnBnShiftToServerLicense();
	afx_msg void OnBnApplyForLanLicense();
	afx_msg void OnBnClickedOk();
	afx_msg void OnRdoDogType();
public:
	static  int RenewLanLicense();	//�����������Ȩ
	virtual INT_PTR DoModal(bool bAutoRenew=false);
	virtual BOOL OnInitDialog();
	void DisableButton(UINT idButton,BOOL bEnable=FALSE);
	void ShowLanLicApplyPage(int nCmdShow);
	void ShowVirtualLicApplyPage(int nCmdShow){licRentalPage.ShowWindow(nCmdShow);}
	void ShowServerLicApplyPage(int nCmdShow){licServerPage.ShowWindow(nCmdShow);}
	bool ImportServerPrimaryLicFile(char* licfilename);
	bool IsLicfileImported(){return m_bLicfileImported;}
	void InitLicenseModeByLicFile(const char* lic_file);
	const static BYTE LIC_MODE_CHOICE	= 0;	//��ѡ
	const static BYTE LIC_MODE_CLIENT	= 1;	//�ͻ���ģʽ
	const static BYTE LIC_MODE_SERVER	= 2;	//������ģʽ
	const static BYTE LIC_SOFT_LICENSE	= 3;	//���������ģʽ
	BYTE m_cLicenseMode;
	bool m_bRunning;	//���������У���ֹ�л�������
	int m_iDogType;
	DWORD m_dwIpAddress;
	short m_siPortNumber;
	CString m_sMasterLicenseHostName;
	CString m_sComputerName;
	CString m_sLicenseType;
	CString m_sErrorMsg;
	int m_nLicRetCode;
};
struct LICSERV_MSGBUF
{	//��Ϣͷ
	long msg_length;//��Ϣ����
	long command_id;//�����ʶ
	BYTE src_code;	//Դ�ڵ���
	BYTE *lpBuffer;			//��Ϣ��
public:
	static const long APPLY_FOR_LICENSE = 1;	//�����ն�ʹ����Ȩ
	static const long RETURN_LICENSE	= 2;	//�����ն�ʹ����Ȩ
	static const long LICENSES_MODIFIED	= 3;	//�Ϸ��ն��û���Ϣ�����仯
};