#pragma once
// CLicenseVirtualRentalPage 对话框
#include "LinkLabel.h"
#include "PromptDlg.h"
#include "afxwin.h"
#include "ServerObject.h"
#include "atom_list.h"
#include "LicenseManager.h"
#include "PayQRCodeDlg.h"

class CLicenseRentalDlg : public CDialog
{
protected:
	CPayQRCodeDlg qrCodeDlg;
	//监听支付结果
	int m_nTimer;
	int m_nTimerElapse;		//单位为毫秒
	int m_nDeathLine;		//单位为毫秒
	bool m_bPaySuccess;		//支付成功
	static const int MAX_WAIT_PAY_TIME	= 180000;	//三分钟
	//
	CComboBox m_cmbRentalPeriod;
#ifdef __WXPAY_RENTAL_ONLINE_
	void StopListenPayResult();
	ATOM_LIST<CProductRental> rentalList;
public:
	ATOM_LIST<CProductRental>* GetRentalListPtr(){return &rentalList;}
	bool IsPaySuccess(){return m_bPaySuccess;}
	void StartListenPayResult();
	void ProcessPayResult(int nIdEvent);
	bool DownloadAndImportAuthFile(bool bPromptMsg = true);
	CProductRental* GetCurRentalPeriod();
	void InitProductRentalComboBox(CComboBox *pComboBox,ATOM_LIST<CProductRental> *pRentalList=NULL,int iCurSel=-1);
	bool TestDownloadAndImportAuthFile();
#endif
	static int TestLogin(bool bNeedInitAplInfo = false, bool bIncFuncUseFlag = false);
	bool ImportProductAuthorizeFile(const char* auth_file, bool bPromptMsg = true);
	int GetCurRentalPeriodIndex();
	DWORD m_dwLicenseApplyTimeStamp;
	CLicenseRentalDlg(UINT nIDTemplate,CWnd* pParent = NULL);   // standard constructor
};

class CLicenseVirtualRentalPage : public CLicenseRentalDlg
{
	DECLARE_DYNAMIC(CLicenseVirtualRentalPage)

	CLinkLabel m_ctrlNoCloseMsg;
public:
	CLicenseVirtualRentalPage(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CLicenseVirtualRentalPage();

// 对话框数据
	enum { IDD = IDD_LICENSE_VIRTUAL_RENTAL_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnShiftToEndLicense();
	afx_msg void OnBnShiftToSrvLicense();
	afx_msg void OnBnImportAuthorizeFile();
	afx_msg void OnBnClickedBnApplyLicenseOnline();
public:
	int m_iRentalType;
	int m_iDogType;
	
	afx_msg void OnBnCreateIdentityFile();
	void UpdateBtnStateByDogType(int iDogType);
};