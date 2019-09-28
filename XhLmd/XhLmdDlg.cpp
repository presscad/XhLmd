// XhLmdDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "XhLmd.h"
#include "XhLmdDlg.h"
#include "MainWorkLoop.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WM_SHOWTASK (WM_USER +1)

// CXhLmdDlg �Ի���
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


// CXhLmdDlg ��Ϣ�������

BOOL CXhLmdDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��
	//
	m_xListUser.AddColumnHeader("Id");
	m_xListUser.AddColumnHeader("��Ʒ");
	long col_wide_arr[2]={50,65};
	m_xListUser.InitListCtrl(col_wide_arr);
	RefreshUserListView();

	PostMessage(WM_SYSCOMMAND, SC_MINIMIZE);
	return TRUE;  // ���������˿ؼ��Ľ��㣬���򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CXhLmdDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ��������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù����ʾ��
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
		return;	//��С�������̺�ֱ�ӷ��أ���Ӧ�ó��򲻻�����������ʾͼ��
	}
	CDialog::OnSysCommand(nID, lParam);
}

void CXhLmdDlg::ToTray()
{
	NOTIFYICONDATA nid;
	nid.cbSize=(DWORD)sizeof(NOTIFYICONDATA);
	nid.hWnd=this->m_hWnd; nid.uID=IDR_MAINFRAME;
	nid.uFlags=NIF_ICON|NIF_MESSAGE|NIF_TIP ;
	nid.uCallbackMessage=WM_SHOWTASK;	//�Զ������Ϣ����
	nid.hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	strcpy(nid.szTip,"���ܹ��������");	//��Ϣ��ʾ��
	Shell_NotifyIcon(NIM_ADD,&nid);		//�����������ͼ��
	ShowWindow(SW_HIDE);				//����������
}
//ɾ������ͼ�꺯��
void CXhLmdDlg::DeleteTray()
{
	NOTIFYICONDATA nid;
	nid.cbSize=(DWORD)sizeof(NOTIFYICONDATA);
	nid.hWnd=this->m_hWnd;
	nid.uID=IDR_MAINFRAME;
	nid.uFlags=NIF_ICON|NIF_MESSAGE|NIF_TIP ;
	nid.uCallbackMessage=WM_SHOWTASK;	//�Զ������Ϣ����
	nid.hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	strcpy(nid.szTip,"���ܹ��������");	//��Ϣ��ʾ
	Shell_NotifyIcon(NIM_DELETE,&nid); //��������ɾ��ͼ��
}

//wParam���յ���ͼ���ID����lParam���յ���������Ϊ
LRESULT CXhLmdDlg::OnShowTask(WPARAM wParam,LPARAM lParam)
{
	if(wParam!=IDR_MAINFRAME)
		return 1;
	switch(lParam)
	{
	case WM_RBUTTONUP://�Ҽ�����ʱ������ݲ˵�������ֻ��һ�����رա�
		{ LPPOINT lpoint=new tagPOINT;
		::GetCursorPos(lpoint);//�õ����λ��
		CMenu menu;
		menu.CreatePopupMenu();//����һ������ʽ�˵�
		//���Ӳ˵���رա������������ϢWM_DESTROY�������ڽ����������
		menu.AppendMenu(MF_STRING,WM_DESTROY,"�ر�"); //ȷ������ʽ�˵���λ��
		menu.TrackPopupMenu(TPM_LEFTALIGN,lpoint->x,lpoint->y,this); //��Դ����
		HMENU hmenu=menu.Detach();
		menu.DestroyMenu();
		delete lpoint;
		} break;
	case WM_LBUTTONDBLCLK://˫������Ĵ��� 
		if(IsWindowVisible())
			this->ShowWindow(SW_HIDE);//����������
		else 
			this->ShowWindow(SW_SHOW);//��ʾ������
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
{	//��һ������ʱ����ʾ����
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