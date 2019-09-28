// XhLmdDlg.h : ͷ�ļ�
//
#pragma once
#include "XhListCtrl.h"
#include "resource.h"

// CXhLmdDlg �Ի���
class CXhLmdDlg : public CDialog
{
	void ToTray();
	void DeleteTray();
	void RefreshUserListView();
// ����
public:
	CXhLmdDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_XHLMD_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnShowTask(WPARAM wParam,LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	CXhListCtrl m_xListUser;
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnClose();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedBtnRefresh();
};
