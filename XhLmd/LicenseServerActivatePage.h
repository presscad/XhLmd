#pragma once

#include "resource.h"
#include "afxdialogex.h"
// CLicenseServerActivatePage �Ի���
class CLicenseServerActivatePage : public CDialogEx
{
	DECLARE_DYNAMIC(CLicenseServerActivatePage)
public:
	CLicenseServerActivatePage(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CLicenseServerActivatePage();
	void UpdateBtnStateByDogType(int iDogType,char ciLicenseMode=0);

// �Ի�������
	enum { IDD = IDD_LICENSE_SERVER_ACTIVATE_DLG };
	int m_iDogType;
	DWORD m_dwLicenseApplyTimeStamp;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnActivateEndLicense();
	afx_msg void OnBnActivateServerLicense();
	afx_msg void OnBnCreateIdentityFile();
	afx_msg void OnBnImportAuthorizeFile();
};
