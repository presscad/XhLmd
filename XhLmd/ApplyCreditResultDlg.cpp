// CApplyCreditResultDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "ApplyCreditResultDlg.h"
#include "Resource.h"

// CApplyCreditResultDlg 对话框

IMPLEMENT_DYNAMIC(CApplyCreditResultDlg, CDialog)

CApplyCreditResultDlg::CApplyCreditResultDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_APPLY_CREDIT_RESULT_DLG, pParent)
{

}

CApplyCreditResultDlg::~CApplyCreditResultDlg()
{
}

void CApplyCreditResultDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CApplyCreditResultDlg, CDialog)
END_MESSAGE_MAP()


// CApplyCreditResultDlg 消息处理程序
BOOL CApplyCreditResultDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
#ifdef __TMA_
	GetDlgItem(IDC_S_PROMPT)->SetWindowText("关注公众号，回复“TMA”,通知客服！");
#elif __LDS_
	GetDlgItem(IDC_S_PROMPT)->SetWindowText("关注公众号，回复“LDS”,通知客服！");
#elif __LMA_
	GetDlgItem(IDC_S_PROMPT)->SetWindowText("关注公众号，回复“LMA”,通知客服！");
#endif

	return TRUE;
}

