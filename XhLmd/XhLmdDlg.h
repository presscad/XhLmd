// XhLmdDlg.h : 头文件
//
#pragma once
#include "XhListCtrl.h"
#include "resource.h"

// CXhLmdDlg 对话框
class CXhLmdDlg : public CDialog
{
	void ToTray();
	void DeleteTray();
	void RefreshUserListView();
// 构造
public:
	CXhLmdDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_XHLMD_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
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
