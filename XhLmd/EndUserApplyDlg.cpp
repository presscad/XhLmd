// EndUserApplyDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "EndUserApplyDlg.h"
#include "afxdialogex.h"
#include "Resource.h"

// CEndUserApplyDlg 对话框

IMPLEMENT_DYNAMIC(CEndUserApplyDlg, CDialogEx)

CEndUserApplyDlg::CEndUserApplyDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CEndUserApplyDlg::IDD, pParent)
	, m_sEndUserName("")
	, m_sPhoneNumber("")
	, m_sClientName("")
{
	m_sWeiXin = "";
}

CEndUserApplyDlg::~CEndUserApplyDlg()
{
}

void CEndUserApplyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_E_ENDUSER, m_sEndUserName);
	DDX_Text(pDX, IDC_E_PHONENUMBER, m_sPhoneNumber);
	DDX_Text(pDX, IDC_E_WX_ACCOUNT, m_sWeiXin);
	DDX_Text(pDX, IDC_E_CLIENTNAME, m_sClientName);
}


BEGIN_MESSAGE_MAP(CEndUserApplyDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CEndUserApplyDlg::OnBnClickedOk)
END_MESSAGE_MAP()

BOOL CEndUserApplyDlg::ContentIsValid()
{
	UpdateData(TRUE);
	m_sPhoneNumber.Replace(" ",NULL);
	m_sEndUserName.Replace(" ",NULL);
	//判断输入的数据是否完整（终端用户和联系方式必填）
	if(m_sEndUserName == "" || m_sPhoneNumber == ""||m_sWeiXin=="")
	{
		AfxMessageBox("请输入必填项。（联系方式：固话请输入区号）");
		if(m_sEndUserName == "")
			GetDlgItem(IDC_E_ENDUSER)->SetFocus();
		else if(m_sPhoneNumber=="")
			GetDlgItem(IDC_E_PHONENUMBER)->SetFocus();
		else //if(m_sWeiXin=="")
			GetDlgItem(IDC_E_WX_ACCOUNT)->SetFocus();
		return FALSE;
	}
	m_sPhoneNumber.Replace("_",NULL);
	//联系方式是否正确
	if(m_sPhoneNumber.GetLength()!=11)
	{
		AfxMessageBox("联系方式长度不正确，请重新输入。（固话请输入区号）");
		return FALSE;
	}
	char cFirst=m_sPhoneNumber.GetAt(0);
	if(cFirst !='1' && cFirst !='0' )
	{
		AfxMessageBox("请输入正确的手机号或固话。（固话请输入区号）");
		return FALSE;
	}
	return TRUE;
}


void CEndUserApplyDlg::OnBnClickedOk()
{
	if(!ContentIsValid())
		return;
	CDialogEx::OnOK();
}
