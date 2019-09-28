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

// CLicenseAuthorizeDlg 对话框

class CLicenseAuthorizeDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CLicenseAuthorizeDlg)
	CRect workpanelRc;
	bool m_bInternalImported;	//内部导入下载的证书，不需要刷新UI wht 19-06-09
	bool m_bLicfileImported;
	CTempFileBuffer* m_pErrLogFile;
	CLicenseServerActivatePage licServerPage;
	CLicenseVirtualRentalPage  licRentalPage;
public:
	CLicenseAuthorizeDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CLicenseAuthorizeDlg();
	static bool _ImportLicFile(char* licfilename, char* errorMsg);

// 对话框数据
	enum { IDD = IDD_LICENSE_AHTORIZE_DLG };

protected:
	CLinkLabel m_ctrlErrorMsg;
	bool _ImportLicFile(char* licfilename);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnDogTest();
	afx_msg void OnBnShiftToServerLicense();
	afx_msg void OnBnApplyForLanLicense();
	afx_msg void OnBnClickedOk();
	afx_msg void OnRdoDogType();
public:
	static  int RenewLanLicense();	//续借局域网授权
	virtual INT_PTR DoModal(bool bAutoRenew=false);
	virtual BOOL OnInitDialog();
	void DisableButton(UINT idButton,BOOL bEnable=FALSE);
	void ShowLanLicApplyPage(int nCmdShow);
	void ShowVirtualLicApplyPage(int nCmdShow){licRentalPage.ShowWindow(nCmdShow);}
	void ShowServerLicApplyPage(int nCmdShow){licServerPage.ShowWindow(nCmdShow);}
	bool ImportServerPrimaryLicFile(char* licfilename);
	bool IsLicfileImported(){return m_bLicfileImported;}
	void InitLicenseModeByLicFile(const char* lic_file);
	const static BYTE LIC_MODE_CHOICE	= 0;	//可选
	const static BYTE LIC_MODE_CLIENT	= 1;	//客户端模式
	const static BYTE LIC_MODE_SERVER	= 2;	//服务器模式
	const static BYTE LIC_SOFT_LICENSE	= 3;	//软件虚拟锁模式
	BYTE m_cLicenseMode;
	bool m_bRunning;	//正在运行中，禁止切换狗类型
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
{	//消息头
	long msg_length;//消息长度
	long command_id;//命令标识
	BYTE src_code;	//源节点编号
	BYTE *lpBuffer;			//消息体
public:
	static const long APPLY_FOR_LICENSE = 1;	//申请终端使用授权
	static const long RETURN_LICENSE	= 2;	//返还终端使用授权
	static const long LICENSES_MODIFIED	= 3;	//合法终端用户信息发生变化
};