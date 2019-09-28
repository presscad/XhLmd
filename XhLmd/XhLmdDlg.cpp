// XhLmdDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "XhLmd.h"
#include "XhLmdDlg.h"
#include "MainWorkLoop.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WM_SHOWTASK (WM_USER +1)

// CXhLmdDlg 对话框
CXhLmdDlg::CXhLmdDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CXhLmdDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CXhLmdDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_USER, m_xListUser);
}

BEGIN_MESSAGE_MAP(CXhLmdDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_SYSCOMMAND()
	ON_MESSAGE(WM_SHOWTASK,OnShowTask)
	ON_WM_CLOSE()
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BTN_REFRESH, &CXhLmdDlg::OnBnClickedBtnRefresh)
END_MESSAGE_MAP()


// CXhLmdDlg 消息处理程序

BOOL CXhLmdDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
	//
	m_xListUser.AddColumnHeader("Id");
	m_xListUser.AddColumnHeader("产品");
	long col_wide_arr[2]={50,65};
	m_xListUser.InitListCtrl(col_wide_arr);
	RefreshUserListView();

	PostMessage(WM_SYSCOMMAND, SC_MINIMIZE);
	return TRUE;  // 除非设置了控件的焦点，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CXhLmdDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
HCURSOR CXhLmdDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CXhLmdDlg::RefreshUserListView()
{
	m_xListUser.DeleteAllItems();
	CStringArray str_arr;
	str_arr.SetSize(2);
	for(DOGSESSION *pSession=CMainWorkLoop::hashSessions.GetFirst();pSession;pSession=CMainWorkLoop::hashSessions.GetNext())
	{
		str_arr[0].Format("%d",pSession->m_idSession);
		str_arr[1].Format("%d",pSession->cProductType);
		m_xListUser.InsertItemRecord(-1,str_arr);
	}
}

void CXhLmdDlg::OnBnClickedBtnRefresh()
{
	RefreshUserListView();
}

void CXhLmdDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if(nID==SC_MINIMIZE) 
	{
		ToTray();
		return;	//最小化到托盘后直接返回，则应用程序不会在任务栏显示图标
	}
	CDialog::OnSysCommand(nID, lParam);
}

void CXhLmdDlg::ToTray()
{
	NOTIFYICONDATA nid;
	nid.cbSize=(DWORD)sizeof(NOTIFYICONDATA);
	nid.hWnd=this->m_hWnd; nid.uID=IDR_MAINFRAME;
	nid.uFlags=NIF_ICON|NIF_MESSAGE|NIF_TIP ;
	nid.uCallbackMessage=WM_SHOWTASK;	//自定义的消息名称
	nid.hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	strcpy(nid.szTip,"加密狗服务程序");	//信息提示条
	Shell_NotifyIcon(NIM_ADD,&nid);		//在托盘区添加图标
	ShowWindow(SW_HIDE);				//隐藏主窗口
}
//删除托盘图标函数
void CXhLmdDlg::DeleteTray()
{
	NOTIFYICONDATA nid;
	nid.cbSize=(DWORD)sizeof(NOTIFYICONDATA);
	nid.hWnd=this->m_hWnd;
	nid.uID=IDR_MAINFRAME;
	nid.uFlags=NIF_ICON|NIF_MESSAGE|NIF_TIP ;
	nid.uCallbackMessage=WM_SHOWTASK;	//自定义的消息名称
	nid.hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	strcpy(nid.szTip,"加密狗服务程序");	//信息提示
	Shell_NotifyIcon(NIM_DELETE,&nid); //在托盘区删除图标
}

//wParam接收的是图标的ID，而lParam接收的是鼠标的行为
LRESULT CXhLmdDlg::OnShowTask(WPARAM wParam,LPARAM lParam)
{
	if(wParam!=IDR_MAINFRAME)
		return 1;
	switch(lParam)
	{
	case WM_RBUTTONUP://右键起来时弹出快捷菜单，这里只有一个“关闭”
		{ LPPOINT lpoint=new tagPOINT;
		::GetCursorPos(lpoint);//得到鼠标位置
		CMenu menu;
		menu.CreatePopupMenu();//声明一个弹出式菜单
		//增加菜单项“关闭”，点击则发送消息WM_DESTROY给主窗口将程序结束。
		menu.AppendMenu(MF_STRING,WM_DESTROY,"关闭"); //确定弹出式菜单的位置
		menu.TrackPopupMenu(TPM_LEFTALIGN,lpoint->x,lpoint->y,this); //资源回收
		HMENU hmenu=menu.Detach();
		menu.DestroyMenu();
		delete lpoint;
		} break;
	case WM_LBUTTONDBLCLK://双击左键的处理 
		if(IsWindowVisible())
			this->ShowWindow(SW_HIDE);//隐藏主窗口
		else 
			this->ShowWindow(SW_SHOW);//显示主窗口
		//DeleteTray();
		break;
	default: break;
	}
	return 0;
}

void CXhLmdDlg::OnClose()
{
	ToTray();
	//CDialog::OnClose();
}

void CXhLmdDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{	//第一次运行时不显示窗口
	if( GetStyle() & WS_VISIBLE )
		CDialog::OnShowWindow(bShow, nStatus);
	else 
	{
		long Style = ::GetWindowLong(*this, GWL_STYLE);
		::SetWindowLong(*this, GWL_STYLE, Style | WS_VISIBLE);
		CDialog::OnShowWindow(SW_HIDE, nStatus);
	}
	if(IsWindowVisible())
		RefreshUserListView();
}