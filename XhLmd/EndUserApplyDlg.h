#pragma once
#include "afxwin.h"
#include "Resource.h"
// CEndUserApplyDlg 对话框

class CEndUserApplyDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CEndUserApplyDlg)
public:
	CString m_sEndUserName;	//终端用户名
	CString m_sPhoneNumber;	//电话号码
	CString m_sWeiXin;		//微信号
	CString m_sClientName;	//客户名称
public:
	CEndUserApplyDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CEndUserApplyDlg();
	BOOL ContentIsValid();
// 对话框数据
	enum { IDD = IDD_ENDUSER_APPLY_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};
