// EndUserApplyDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "EndUserApplyDlg.h"
#include "afxdialogex.h"
#include "Resource.h"

// CEndUserApplyDlg �Ի���

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
	//�ж�����������Ƿ��������ն��û�����ϵ��ʽ���
	if(m_sEndUserName == "" || m_sPhoneNumber == ""||m_sWeiXin=="")
	{
		AfxMessageBox("��������������ϵ��ʽ���̻����������ţ�");
		if(m_sEndUserName == "")
			GetDlgItem(IDC_E_ENDUSER)->SetFocus();
		else if(m_sPhoneNumber=="")
			GetDlgItem(IDC_E_PHONENUMBER)->SetFocus();
		else //if(m_sWeiXin=="")
			GetDlgItem(IDC_E_WX_ACCOUNT)->SetFocus();
		return FALSE;
	}
	m_sPhoneNumber.Replace("_",NULL);
	//��ϵ��ʽ�Ƿ���ȷ
	if(m_sPhoneNumber.GetLength()!=11)
	{
		AfxMessageBox("��ϵ��ʽ���Ȳ���ȷ�����������롣���̻����������ţ�");
		return FALSE;
	}
	char cFirst=m_sPhoneNumber.GetAt(0);
	if(cFirst !='1' && cFirst !='0' )
	{
		AfxMessageBox("��������ȷ���ֻ��Ż�̻������̻����������ţ�");
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
