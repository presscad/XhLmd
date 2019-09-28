#pragma once
#include "afxwin.h"
#include "Resource.h"
// CEndUserApplyDlg �Ի���

class CEndUserApplyDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CEndUserApplyDlg)
public:
	CString m_sEndUserName;	//�ն��û���
	CString m_sPhoneNumber;	//�绰����
	CString m_sWeiXin;		//΢�ź�
	CString m_sClientName;	//�ͻ�����
public:
	CEndUserApplyDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CEndUserApplyDlg();
	BOOL ContentIsValid();
// �Ի�������
	enum { IDD = IDD_ENDUSER_APPLY_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};
