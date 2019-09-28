// PayQRCodeDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "XhLmdI.h"
#include "PayQRCodeDlg.h"
#include "afxdialogex.h"
#include "QRcode.h"

// CPayQRCodeDlg 对话框

IMPLEMENT_DYNAMIC(CPayQRCodeDlg, CDialog)

RENTALORDER::RENTALORDER(const char* payurlstr,const char* productname,const char* rental_period,UINT costcents/*=1*/,
			const char* orderidstr/*=NULL*/,const char* occuredatestr/*=NULL*/)
{
	szPayURL=payurlstr;
	szProductName=productname;
	szPeriod=rental_period;		//租借时间
	uiMoneyCents=costcents;		//租金计量单位，分
	if(orderidstr)
		szOrderIdStr=orderidstr;	//订单号
	if(occuredatestr)
		szOrderDate=occuredatestr;	//下单时间
	else
	{
		CTime t=CTime::GetCurrentTime();
		szOrderDate=t.Format("%Y-%m-%d %H:%M:%S");
	}
}
CPayQRCodeDlg::CPayQRCodeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPayQRCodeDlg::IDD, pParent)
{
	m_sPromptMsg = _T("");
	m_sTitle = _T("");
}

CPayQRCodeDlg::~CPayQRCodeDlg()
{
}

void CPayQRCodeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPayQRCodeDlg)
	DDX_Text(pDX, IDC_MSG, m_sPromptMsg);
	DDX_Control(pDX, IDC_MSG, m_ctrlMsg);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPayQRCodeDlg, CDialog)
	//{{AFX_MSG_MAP(CPayQRCodeDlg)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// CPayQRCodeDlg 消息处理程序
BOOL CPayQRCodeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_ctrlMsg.SetTextColor(RGB(255,0,0));
	m_ctrlMsg.SetBackgroundColour(RGB(255,255,255));//240,240,240));
	SetWindowText(m_sTitle);
	RelayoutWnd();
	return TRUE;
}

static const int TOP_MARGIN		= 15;
static const int PROPM_CTRL_H	= 50;//40;
static const int EDGE_MARGIN	= 5;
void CPayQRCodeDlg::OnPaint()
{
	CRect rcClient;
	GetClientRect(rcClient);
	CPaintDC dc(this);
	dc.SetBkColor(RGB(255,255,255));
	dc.SelectStockObject(WHITE_BRUSH);
	dc.SetDCPenColor(RGB(255,255,255));
	dc.Rectangle(rcClient);
	CDC MemDc;
	BITMAP bmInfo;
	m_bBitMap.GetBitmap(&bmInfo);	
	MemDc.CreateCompatibleDC(&dc);
	CBitmap* pOldMap=MemDc.SelectObject(&m_bBitMap);
	dc.BitBlt(EDGE_MARGIN,PROPM_CTRL_H,bmInfo.bmWidth,bmInfo.bmHeight,&MemDc,0,0,SRCCOPY);
	MemDc.SelectObject(pOldMap);
}

void CPayQRCodeDlg::SetQRCodeStr(RENTALORDER& order)
{
	if(m_bBitMap.GetSafeHandle())
		m_bBitMap.DeleteObject();
	CXhChar200 szPayUrlSourceString;
	//const char* szSourceSring,const char* sTitle
	//	,const char* sPrompt
	if(!QRCode(order.szPayURL,m_bBitMap))
		return;
	if (GetSafeHwnd() == 0)
		Create();
	ShowWindow(SW_SHOW);
	m_sTitle="微信扫码授权";
	SetWindowText(m_sTitle);
	m_sPromptMsg.Format("%s租借授权订单",(char*)order.szProductName);
	GetDlgItem(IDC_RENTAL_PERIOD)->SetWindowText(order.szPeriod);
	UpdateData(FALSE);
	SetFocus();
	RelayoutWnd();
	Invalidate();
}

void CPayQRCodeDlg::RelayoutWnd()
{
	CRect rect,client_rect;
	CWnd* pHeaderCtrl=(CWnd*)GetDlgItem(IDC_S_PIC_WXPAY_SYMBOL);//IDC_MSG);
	pHeaderCtrl->GetWindowRect(&rect);
	int w=rect.right-rect.left,h=rect.bottom-rect.top;
	pHeaderCtrl->MoveWindow(CRect(EDGE_MARGIN,EDGE_MARGIN,EDGE_MARGIN+w,EDGE_MARGIN+h));
	GetWindowRect(rect);
	GetClientRect(client_rect);
	long fWidth=rect.Width()-client_rect.Width();
	long fHight=rect.Height()-client_rect.Height();
	int nClientW=0,nClientH=0;
	BITMAP bmInfo;
	m_bBitMap.GetBitmap(&bmInfo);
	nClientW=bmInfo.bmWidth+EDGE_MARGIN*2;
	nClientH=bmInfo.bmHeight+PROPM_CTRL_H+EDGE_MARGIN*2;
	//
	nClientW+=430;
	int cx=GetSystemMetrics(SM_CXSCREEN);
	int cy=GetSystemMetrics(SM_CYSCREEN);
	rect.right=(int)(0.5*(cx+nClientW));
	rect.left =rect.right-nClientW-fWidth;
	rect.top=(int)(0.5*(cy-nClientH));
	rect.bottom =rect.top+nClientH+fHight+2*EDGE_MARGIN;
	MoveWindow(rect,TRUE);
}

void CPayQRCodeDlg::Close()
{
	CDialog::DestroyWindow();
}

BOOL CPayQRCodeDlg::Create()
{
	return CDialog::Create(CPayQRCodeDlg::IDD);
}