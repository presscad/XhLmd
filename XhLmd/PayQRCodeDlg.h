#pragma once

#include "LinkLabel.h"
#include "Resource.h"
// CPayQRCodeDlg �Ի���
struct RENTALORDER{
	CXhChar50 szPayURL;
	CXhChar16 szProductName;
	CXhChar16 szPeriod;		//���ʱ��
	UINT uiMoneyCents;		//��������λ����
	CXhChar16 szOrderDate;	//�µ�ʱ��
	CXhChar50 szOrderIdStr;	//������
	RENTALORDER(const char* payurlstr,const char* productname,const char* rental_period,UINT costcents=1,const char* orderidstr=NULL,const char* occuredatestr=NULL);
};
class CPayQRCodeDlg : public CDialog
{
	DECLARE_DYNAMIC(CPayQRCodeDlg)
	CBrush m_bBrush;
	CBitmap m_bBitMap;
	void RelayoutWnd();
public:
	CPayQRCodeDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CPayQRCodeDlg();
	void Close();
	BOOL Create();
	void SetQRCodeStr(RENTALORDER& order);
// �Ի�������
	enum { IDD = IDD_PAY_QRCODE_DLG };
	CString m_sTitle;
	CString	m_sPromptMsg;
	CColorLabel m_ctrlMsg;
protected:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPayQRCodeDlg)
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
		virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL
	// Generated message map functions
	//{{AFX_MSG(CPayQRCodeDlg)
		afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
