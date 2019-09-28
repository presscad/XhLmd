#pragma once


// CApplyCreditResultDlg 对话框

class CApplyCreditResultDlg : public CDialog
{
	DECLARE_DYNAMIC(CApplyCreditResultDlg)

public:
	CApplyCreditResultDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CApplyCreditResultDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_APPLY_CREDIT_RESULT_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

	virtual BOOL OnInitDialog();
};
