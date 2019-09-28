// LicenseVirtualRentalPage.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "XhLmdI.h"
#include "LicenseAuthorizeDlg.h"
#include "LicenseVirtualRentalPage.h"
#include "afxdialogex.h"
#include "PromptDlg.h"
#include "WxPayAPI.h"
#include "VER.h"
#include "LicenseManager.h"
#include "XhCharString.h"
#include "EndUserApplyDlg.h"
#include "ApplyCreditResultDlg.h"
#include "LogFile.h"

// CLicenseVirtualRentalPage �Ի���
//////////////////////////////////////////////////////////////////////////
// CLicenseRentalDlg
CLicenseRentalDlg::CLicenseRentalDlg(UINT nIDTemplate,CWnd* pParent/* = NULL*/)
	: CDialog(nIDTemplate, pParent)
{
	m_nTimer=0;
	m_nTimer=0;
	m_nTimerElapse=1000;
	m_nDeathLine=0;
	m_bPaySuccess=false;
	m_dwLicenseApplyTimeStamp = 0;
}
#ifdef __WXPAY_RENTAL_ONLINE_
#ifndef __ALL_LICFUNC_
APL_INFO g_aplInfo;
#endif
void CALLBACK EXPORT ListenPayResult(HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime)
{
	CWnd* pWnd=CWnd::FromHandle(hWnd);
	if(pWnd==NULL)
		return;
	CLicenseRentalDlg *pPage=(CLicenseRentalDlg*)pWnd;
	pPage->ProcessPayResult(nIDEvent);
}

void CLicenseRentalDlg::StopListenPayResult()
{
	KillTimer(m_nTimer);
	m_nDeathLine=0;
}

void CLicenseRentalDlg::StartListenPayResult()
{
	if(m_nTimer>0)
		KillTimer(m_nTimer);
	m_nTimer=SetTimer(2,m_nTimerElapse,ListenPayResult);
	m_nDeathLine=0;
}

void CLicenseRentalDlg::ProcessPayResult(int nIdEvent)
{
	if(m_nTimer!=nIdEvent)
		return;
	if(m_nDeathLine>MAX_WAIT_PAY_TIME)
	{
		qrCodeDlg.Close();
		KillTimer(nIdEvent);	//�رն�ʱ��
		AfxMessageBox("֧����ʱ,������!");
		return;	//֧����ʱ
	}
	else if(VER.QueryOrderPayState(VER.m_idSession,g_aplInfo.m_dwRandApplyCode))
	{	//֧�����
		qrCodeDlg.Close();
		KillTimer(nIdEvent);	//�رն�ʱ��
		m_bPaySuccess=true;
		DownloadAndImportAuthFile();
		return;
	}
	else 
		m_bPaySuccess=false;
	m_nDeathLine+=m_nTimerElapse;
}
bool CLicenseRentalDlg::DownloadAndImportAuthFile(bool bPromptMsg /*= true*/)
{
	CBuffer buffer;
	if (VER.QueryAuthByteArr(VER.m_idSession, g_aplInfo.m_dwRandApplyCode, &buffer))
	{	//������Ȩ�ļ�
		if (buffer.GetLength() == 0)
		{
			if (bPromptMsg)
				AfxMessageBox("֧����ɣ�δ�ҵ���Ȩ�ļ��������������ϵ!");
			return false;
		}
		CXhChar200 authfile("%s%s.aut", execute_path, (char*)GetProductName());
		CTempFileBuffer auth(authfile);
		auth.EnableAutoDelete(false);
		auth.Write(buffer.GetBufferPtr(), buffer.GetLength());
		auth.FlushTmpFile();
		auth.CloseFile();
		return ImportProductAuthorizeFile(authfile);
	}
	else
	{
		if (bPromptMsg)
			AfxMessageBox("֧�����,������Ȩ�ļ�ʧ�ܣ�");
		return false;
	}
}
#endif

int CLicenseRentalDlg::GetCurRentalPeriodIndex()
{
	return m_cmbRentalPeriod.GetCurSel();
}

#ifdef __WXPAY_RENTAL_ONLINE_
CProductRental* CLicenseRentalDlg::GetCurRentalPeriod()
{
	CProductRental *pRental=NULL;
	int iCurSel=m_cmbRentalPeriod.GetCurSel();
	if(iCurSel>=0)
		pRental=(CProductRental*)m_cmbRentalPeriod.GetItemData(iCurSel);
	return pRental;
}
#endif

IMPLEMENT_DYNAMIC(CLicenseVirtualRentalPage, CLicenseRentalDlg)

CLicenseVirtualRentalPage::CLicenseVirtualRentalPage(CWnd* pParent /*=NULL*/)
	: CLicenseRentalDlg(CLicenseVirtualRentalPage::IDD, pParent)
	, m_iRentalType(0)
{
	m_iDogType=0;
	m_bPaySuccess=false;
	m_ctrlNoCloseMsg.EnableURL(FALSE);
	m_ctrlNoCloseMsg.EnableHover(FALSE);
	m_ctrlNoCloseMsg.SetColours(RGB(255,0,0),RGB(255,0,0),RGB(255,0,0));
	m_nTimerElapse=1000;
	m_nDeathLine=0;
}

CLicenseVirtualRentalPage::~CLicenseVirtualRentalPage()
{
}

void CLicenseVirtualRentalPage::DoDataExchange(CDataExchange* pDX)
{
	CLicenseRentalDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLicenseVirtualRentalPage)
	DDX_CBIndex(pDX, IDC_CMB_RENTAL_PERIOD, m_iRentalType);
	DDX_Control(pDX, IDC_S_MSG_NO_CLOSE, m_ctrlNoCloseMsg);
	DDX_Control(pDX, IDC_CMB_RENTAL_PERIOD, m_cmbRentalPeriod);
	//}}AFX_DATA_MAP
}
DWORD _LocalRandDword()
{
	DWORD seed = 0;
	seed = rand();
	seed <<= 16;
	DWORD lowpart = rand();
	seed |= (0x0000ffff & lowpart);
	return seed;
}
BOOL CLicenseVirtualRentalPage::OnInitDialog()
{
	srand((UINT)time(NULL));
	DWORD s0 = rand();
	m_iDogType=AgentDogType();	//OninitDialog��һ���ڸ�����֮����ã�������������ǰ���ã������ʼ�������� wjh-2017.10.8
	g_aplInfo.Init();
#ifdef _DEBUG
	GetDlgItem(IDC_S_RAND_APPLY_CODE)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_S_RAND_APPLY_CODE)->SetWindowText(CXhChar50("�������:%d",g_aplInfo.m_dwRandApplyCode));
#else
	GetDlgItem(IDC_S_RAND_APPLY_CODE)->ShowWindow(SW_HIDE);
#endif
	GetDlgItem(IDC_BN_SHIFT_TO_SERVER_LICENSE)->EnableWindow(m_iDogType!=3);
	GetDlgItem(IDC_BN_SHIFT_TO_END_LICENSE)->EnableWindow(m_iDogType!=3);
	//GetDlgItem(IDC_BN_APPLY_LICENSE_ONLINE)->ShowWindow(SW_HIDE);
	//m_ctrlNoCloseMsg.SetWindowText("����������Ȩ�ļ�ǰ����رնԻ���");
	BOOL retcode=CLicenseRentalDlg::OnInitDialog();
#ifdef __WXPAY_RENTAL_ONLINE_
	InitProductRentalComboBox(&m_cmbRentalPeriod);
	if (VER.m_idSession & 0x80000000)	//���λΪ1��ʾ����ǰ�û�δͨ����� wht 18-09-08
		GetDlgItem(IDC_BN_APPLY_LICENSE_ONLINE)->SetWindowText("��ͨ΢��ɨ������");
	else
		GetDlgItem(IDC_BN_APPLY_LICENSE_ONLINE)->SetWindowText("΢��ɨ��������Ȩ");
	CWnd* pParent = GetParent();
	CLicenseAuthorizeDlg* pParentDialg = pParent != NULL ? (CLicenseAuthorizeDlg*)pParent->GetParent() : NULL;
	if (pParentDialg && pParentDialg->m_bRunning)
		GetDlgItem(IDC_BN_APPLY_LICENSE_ONLINE)->EnableWindow(false);
	else
		GetDlgItem(IDC_BN_APPLY_LICENSE_ONLINE)->EnableWindow(true);
#endif
	return retcode;
}
void CLicenseVirtualRentalPage::UpdateBtnStateByDogType(int iDogType)
{
	m_iDogType=iDogType;
	GetDlgItem(IDC_BN_SHIFT_TO_SERVER_LICENSE)->EnableWindow(m_iDogType!=3);
	GetDlgItem(IDC_BN_SHIFT_TO_END_LICENSE)->EnableWindow(m_iDogType!=3);
}


BEGIN_MESSAGE_MAP(CLicenseVirtualRentalPage, CLicenseRentalDlg)
	ON_BN_CLICKED(IDC_BN_SHIFT_TO_END_LICENSE, OnBnShiftToEndLicense)
	ON_BN_CLICKED(IDC_BN_SHIFT_TO_SERVER_LICENSE, OnBnShiftToSrvLicense)
	ON_BN_CLICKED(IDC_BN_CREATE_IDENTITY_FILE, OnBnCreateIdentityFile)
	ON_BN_CLICKED(IDC_BN_IMPORT_AUTHORIZE_FILE, OnBnImportAuthorizeFile)
	ON_BN_CLICKED(IDC_BN_APPLY_LICENSE_ONLINE, OnBnClickedBnApplyLicenseOnline)
END_MESSAGE_MAP()


// CLicenseVirtualRentalPage ��Ϣ�������
void CLicenseVirtualRentalPage::OnBnShiftToEndLicense()
{
	CLicenseAuthorizeDlg* m_pParentDialg=NULL;
	CWnd* pParent=GetParent();
	if(pParent)
		m_pParentDialg=(CLicenseAuthorizeDlg*)pParent->GetParent();
	if(m_pParentDialg)
	{
		ShowWindow(SW_HIDE);
		m_pParentDialg->ShowLanLicApplyPage(SW_SHOW);
	}
}
void CLicenseVirtualRentalPage::OnBnShiftToSrvLicense()
{
	CLicenseAuthorizeDlg* m_pParentDialg=NULL;
	CWnd* pParent=GetParent();
	if(pParent)
		m_pParentDialg=(CLicenseAuthorizeDlg*)pParent->GetParent();
	if(m_pParentDialg)
	{
		ShowWindow(SW_HIDE);
		m_pParentDialg->ShowServerLicApplyPage(SW_SHOW);
	}
}
void CLicenseVirtualRentalPage::OnBnCreateIdentityFile()
{
	if(g_aplInfo.m_bNewApplyRandCode)
	{
		g_aplInfo.m_dwRandApplyCode=g_aplInfo._LocalRandDword();
#ifdef _DEBUG
		GetDlgItem(IDC_S_RAND_APPLY_CODE)->SetWindowText(CXhChar50("��Ȩ�������:%d",g_aplInfo.m_dwRandApplyCode));
#endif
		g_aplInfo.m_bNewApplyRandCode=false;
	}
	UpdateData();
	COMPUTER computer;
	BYTE key[32] = { 0 }, details[192] = { 0 }, hardidcode[16] = { 0 };
	COMPUTER::Generate128Identity(key, &m_dwLicenseApplyTimeStamp, details, 192, hardidcode);
	DWORD dwDetailBytes = 0;
	for (dwDetailBytes = 0; dwDetailBytes < 192; dwDetailBytes += 16)
	{
		INT64PTR i64(details + dwDetailBytes);
		if (i64[0] == 0 && i64[1] == 0)
			break;
	}
	//TODO:��ȡ�ִ���������Ϣ�д����ƣ��ݰ�����������Ϣ����
	CBuffer buffer;
	buffer.WriteDword(m_dwLicenseApplyTimeStamp);		//д��ָ������ʱ���
	buffer.Write(key, 32);				//д��ָ�Ʊ�ʶ��Ϣ
	buffer.WriteDword(dwDetailBytes);	//д��ָ����ϸ��Ϣ�ڴ�鳤��
	buffer.Write(details, dwDetailBytes);//д��ָ����ϸ��Ϣ
	buffer.WriteString(computer.sComputerName,51);	//д����������
	BYTE ipv4[4];
	BYTE ciCount=(BYTE)COMPUTER::GetIP(NULL,computer.sComputerName,0);
	buffer.WriteByte(ciCount);	//д��IP����
	for(BYTE i=0;i<ciCount;i++)
	{
		ipv4[0]=ipv4[1]=ipv4[2]=ipv4[3]=0;
		COMPUTER::GetIP(ipv4,computer.sComputerName,i);
		buffer.Write(ipv4,4);	//д��ÿ��IP�ĵ�ַ
	}
	UINT uDogSerial=DogSerialNo();//��ȡ���������к�
	UINT uDocType=829357911;	//829357911Ϊһ�������ʾ�ź���˾ C2V�ļ�
	buffer.WriteDword(uDocType);
	XHVERSION c2v_version("0.0.0.1");
	buffer.Write(&c2v_version,4);
	buffer.WriteDword(uDogSerial);	// 
	buffer.WriteByte(3);//CDogAgent::SOFT_DOG);
	buffer.WriteWord((short)0);//WORD wContentLength=0;
	buffer.WriteByte(PRODUCT_TYPE);	//����Ʒ����
	WORD wRentalDays=7;
	if(m_iRentalType==0)	//����Ʒ
		buffer.WriteWord(wRentalDays);
	CXhChar50 hardcodestr=g_aplInfo.GetHardIdCodeStr();
	//д���ļ�
	CXhChar16 product_name=GetProductName();
	char fn[MAX_PATH];
	sprintf(fn,"%s#%s.apl",(char*)product_name,(char*)hardcodestr);
	CFileDialog dlg(FALSE,"apl",fn,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"�û�����Ȩ�����ļ�(*.apl)|*.apl|�����ļ�(*.*)|*.*||");
	if(dlg.DoModal()==IDOK)
	{
		//v2cfn=dlg.GetPathName().GetString();
		FILE* fp=fopen(dlg.GetPathName(),"wb");
		if(fp==NULL)
		{
			AfxMessageBox("APL�ļ���ʧ��!");
			return;
		}
		XHVERSION licapplyfile_version("0.0.0.2");	//��������������Ȩ
		//1.�洢apl�ļ�ͷ��Ϣ
		UINT uDocType=2834535456;	//2834535456 Ϊһ�������ʾ�ź���˾ ��Ȩ�����ļ�
		fwrite(&uDocType,4,1,fp);	//doc_type, 
		fwrite(&licapplyfile_version,4,1,fp);// 
		g_aplInfo.m_bNewApplyRandCode=true;
		fwrite(&g_aplInfo.m_dwRandApplyCode,4,1,fp);	//д����Ȩ���������֤ʶ����
		//2.�洢ʵ��������Ϣ
		DWORD dwContentsLength=buffer.GetLength();
		fwrite(&dwContentsLength,4,1,fp);
		fwrite(buffer.GetBufferPtr(),buffer.GetLength(),1,fp);
		fclose(fp);
#ifdef _DEBUG
		GetDlgItem(IDC_S_RAND_APPLY_CODE)->SetWindowText(CXhChar50("��Ȩ�������:%d",g_aplInfo.m_dwRandApplyCode));
#endif
	}
}
void CLicenseVirtualRentalPage::OnBnImportAuthorizeFile()
{
	UINT uDogSerial=0;//DogSerialNo();	//��ȡ���������к�
	//д���ļ�
	CXhChar16 productname=GetProductName();
	char fn[MAX_PATH];
	strcpy(fn,productname);
	strcat(fn,CXhChar16("%d",uDogSerial));
	CFileDialog dlg(TRUE,"aut",fn,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"�û�����Ȩ�����ļ�(*.aut)|*.aut|�����ļ�(*.*)|*.*||");
	if(dlg.DoModal()!=IDOK)
		return;
	ImportProductAuthorizeFile(dlg.GetPathName());
}
bool CLicenseRentalDlg::ImportProductAuthorizeFile(const char* auth_file, bool bPromptMsg /*= true*/)
{
	CXhChar16 productname=GetProductName();
	CXhChar200 licfile("%s%s.lic",execute_path,(char*)productname);
	CWaitCursor waitcursor;
	bool bRetCode = false;
	int retcode=ImportAuthorizeFile(auth_file,execute_path,PRODUCT_TYPE,g_aplInfo.m_dwRandApplyCode);
	if(retcode!=0)
	{
		waitcursor.Restore();
		CXhChar200 message("������Ȩ֤���ļ�{%s}����ʧ��",(char*)licfile);
		if(retcode==-3)
			message.Append("��ԭ����Ȩ����������Ȩ��Ų�ƥ��");
		else if(retcode==-4)
			message.Append("��ԭ���û�����ָ����Ϣ��֤ʧ��");
		else if(retcode==-7)
			message.Append("��ԭ��ȱ����Ӧִ��Ȩ�ޣ����Թ���ԱȨ�����г���");
		else
			message.Append(CXhChar16("��ԭ��#%d",retcode));
		if (bPromptMsg)
			AfxMessageBox(message);
	}
	else
	{	//�������ɵ���֤���ļ�
		CXhChar200 licfile("%s%s.lic", execute_path, (char*)productname);
		if (GetSafeHwnd() != NULL)
		{
			CWnd* pParent = GetParent();
			CLicenseAuthorizeDlg* pParentDialg = pParent != NULL ? (CLicenseAuthorizeDlg*)pParent->GetParent() : NULL;
			if (pParentDialg && !pParentDialg->m_bRunning)
				bRetCode = pParentDialg->ImportServerPrimaryLicFile(licfile);
		}
		else
			bRetCode = CLicenseAuthorizeDlg::_ImportLicFile(licfile, NULL);
	}
	return bRetCode;
}
#ifdef __WXPAY_RENTAL_ONLINE_
void CLicenseRentalDlg::InitProductRentalComboBox(CComboBox *pComboBox,ATOM_LIST<CProductRental> *pRentalList/*=NULL*/,int iCurSel/*=-1*/)
{
	CBuffer buffer;
	rentalList.Empty();
	if(pRentalList!=NULL)
	{
		for(CProductRental *pRental=pRentalList->GetFirst();pRental;pRental=pRentalList->GetNext())
		{
			CProductRental *pNewRental=rentalList.append();
			pNewRental->CopyProperty(*pRental);
		}
	}
	if(VER.QueryProductRentals(PRODUCT_TYPE,&buffer))
	{
		buffer.SeekToBegin();
		int nCount=buffer.ReadInteger();
		for(int i=0;i<nCount;i++)
		{
			CProductRental *pRental=rentalList.append();
			pRental->FromBuffer(buffer);
		}
	}
	//
	if(pComboBox==NULL||pComboBox->GetSafeHwnd()==NULL)
		return;
	while(pComboBox->GetCount()>0)
		pComboBox->DeleteString(0);
	for(CProductRental *pRental=rentalList.GetFirst();pRental;pRental=rentalList.GetNext())
	{
		int iItem=pComboBox->AddString(pRental->rentalDescription);
		pComboBox->SetItemData(iItem,(DWORD)pRental);
	}
	if(iCurSel>=0)
		pComboBox->SetCurSel(iCurSel);
	else
		pComboBox->SetCurSel(0);
}
//���Ե�½���ҵ�����ʱ�����ã���ѡ���ڳ����������˳�ʱ���ã�����Ҫ�����ռ��û�ʹ��ϰ��
int CLicenseRentalDlg::TestLogin(bool bNeedInitAplInfo /*= false*/, bool bIncFuncUseFlag /*= false*/)
{	//���Ե�½�ϴ�����
	CEndUser endUser;
	if (bNeedInitAplInfo)
		g_aplInfo.Init();
	g_aplInfo.InitEndUserBasiceInfo(endUser);
	endUser.m_uProductId = PRODUCT_TYPE;
	endUser.m_uDogNo = DogSerialNo();
	endUser.m_uVersion = dwVersion[0];
	endUser.m_biApplyAutState = g_aplInfo.m_biApplyAutState;
	endUser.m_iInfoFlag = CEndUser::INFO_COMPUTER;
	if (bIncFuncUseFlag)
	{	//�ӱ����ļ��м����ճ�ʹ��ϰ��
		BYTE functionUseFlag[64] = { 0 };
		//LoadFunctionUseFlagFromFile(functionUseFlag);
	}
	int idSession = VER.LoginUser(&endUser);
	return idSession;
}

//���Ե�½�ж��Ƿ�Ϊ�����û�����������޷�Χ���Զ����ؿ���֤��
bool CLicenseRentalDlg::TestDownloadAndImportAuthFile()
{
	int idSession = TestLogin(true, false);
	//��ѯ��ǰ�û�����ǰ��Ʒ�Ƿ�����Ч����
	COrder validOrder;
	if (!(idSession & 0x80000000 || idSession & 0x40000000) && VER.QueryValidRentalOrder(idSession, PRODUCT_TYPE, &validOrder))
	{
		g_aplInfo.m_dwRandApplyCode = validOrder.applyRandCode;
		return DownloadAndImportAuthFile(false);
	}
	else
		return false;
}
#endif
static CEndUserApplyDlg endUserApplyDlg;
void CLicenseVirtualRentalPage::OnBnClickedBnApplyLicenseOnline()
{
#ifdef __WXPAY_RENTAL_ONLINE_
	CLogFile logOnLineLic;
	CXhChar16 productname = GetProductName();
	CXhChar200 logfile("%s%sOnLineLic.log", execute_path, (char*)productname);
	logOnLineLic.InitLogFile(logfile,false);
	logOnLineLic.EnableTimestamp();
	//
	CEndUser endUser;
	g_aplInfo.InitEndUserBasiceInfo(endUser);
	endUser.m_uProductId = PRODUCT_TYPE;
	endUser.m_uDogNo = DogSerialNo();
	endUser.m_uVersion = dwVersion[0];
	endUser.m_biApplyAutState = g_aplInfo.m_biApplyAutState;
	endUser.m_iInfoFlag = CEndUser::INFO_COMPUTER;
	int idSession = VER.LoginUser(&endUser);
	//��ѯ��ǰ�û�����ǰ��Ʒ�Ƿ�����Ч����
	COrder validOrder;
	if (!(idSession & 0x80000000 || idSession & 0x40000000) && VER.QueryValidRentalOrder(idSession, endUser.m_uProductId, &validOrder))
	{
		g_aplInfo.m_dwRandApplyCode = validOrder.applyRandCode;
		DownloadAndImportAuthFile();
		return;
	}
	if(g_aplInfo.m_dwRandApplyCode==0)//g_aplInfo.m_bNewApplyRandCode)
	{
		g_aplInfo.m_dwRandApplyCode=g_aplInfo._LocalRandDword();
#ifdef _DEBUG
		GetDlgItem(IDC_S_RAND_APPLY_CODE)->SetWindowText(CXhChar50("��Ȩ�������:%d",g_aplInfo.m_dwRandApplyCode));
#endif
		g_aplInfo.m_bNewApplyRandCode=false;
	}
	else
	{
#ifdef _DEBUG
		GetDlgItem(IDC_S_RAND_APPLY_CODE)->SetWindowText(CXhChar50("��Ȩ�������:%d",g_aplInfo.m_dwRandApplyCode));
#endif
		if(IsPaySuccess())
		{
			DownloadAndImportAuthFile();
			return;
		}
	}
	if(idSession&0x80000000)
	{	//���λΪ1��ʾ����ǰ�û�δͨ����� wht 18-09-08
		idSession=idSession&0x7FFFFFFF;
		endUser.m_biApplyAutState=APL_INFO::STATE_NEED_APPLY;
	}
	else if(idSession&0x40000000)
	{	//�θ�λΪ1��ʾ����ǰ�û���ͨ�����,����ǰ��Ʒδ��Ȩ wht 18-10-30
		idSession=idSession&0xBFFFFFFF;
		endUser.m_biApplyAutState=APL_INFO::STATE_CUR_PRODUCT_NEED_APPLY;
	}
	else
		endUser.m_biApplyAutState=APL_INFO::STATE_APPROVED_APPLY;
	g_aplInfo.m_biApplyAutState=endUser.m_biApplyAutState;
	VER.m_idSession=idSession;
	if(VER.m_idSession<=0)
	{	//��¼ʧ��
		AfxMessageBox("���ӷ�����ʧ�ܣ����Ժ����ԣ�");
		return;
	}
	else if(endUser.m_biApplyAutState==APL_INFO::STATE_CUR_PRODUCT_NEED_APPLY)
	{
		AfxMessageBox("��ǰ��Ʒδ��Ȩ������ϵ�ͷ���ȡ��Ʒ��Ȩ(010-6824 1139)��");
		return;
	}
	else if(endUser.m_biApplyAutState==APL_INFO::STATE_NEED_APPLY)
	{
		endUser.m_iOnLineRentalState=CEndUser::TYPE_AUT_APPLY_RENTAL;	//ע������Ȩ��
		endUser.m_uProductId = PRODUCT_TYPE;
		CEndUser serverEndUser;
		if (VER.DownloadEndUser(VER.m_idSession, &serverEndUser))
		{
			endUserApplyDlg.m_sPhoneNumber = serverEndUser.m_sPhoneNumber;
			endUserApplyDlg.m_sClientName = serverEndUser.m_sClientName;
			endUserApplyDlg.m_sEndUserName = serverEndUser.m_sEndUserName;
			endUserApplyDlg.m_sWeiXin = serverEndUser.m_sWeiXin;
		}
		if(endUserApplyDlg.DoModal()==IDOK)
		{
			endUser.m_sPhoneNumber=endUserApplyDlg.m_sPhoneNumber;	//�绰��
			endUser.m_sClientName=endUserApplyDlg.m_sClientName;	//��λ��
			endUser.m_sEndUserName=endUserApplyDlg.m_sEndUserName;	//�û���
			endUser.m_sWeiXin = endUserApplyDlg.m_sWeiXin;
			endUser.m_iInfoFlag = CEndUser::INFO_COMPUTER | CEndUser::INFO_ENDUSER;
			if (VER.LoginUser(&endUser) > 0)
			{
				endUser.m_biApplyAutState = APL_INFO::STATE_CUR_PRODUCT_NEED_APPLY;
				CApplyCreditResultDlg dlg;
				dlg.DoModal();
				//AfxMessageBox("�����뿪ͨ�������޹��ܣ�����ϵ�ͷ����ͨ��(010-6824 1139)��");
			}
			else
			{
				logOnLineLic.Log("Log Id:%d", VER.m_idSession);
				AfxMessageBox("����ʧ�ܣ����Ժ����ԣ�");
			}
		}
		return;
	}
	CProductRental *pRental=GetCurRentalPeriod();
	if(pRental==NULL)
	{
		AfxMessageBox("��ѡ����Ч��Ȩ��,���������룡");
		return;
	}
	//CProductRental rental;
	//if(m_iRentalType==0)
	//	rental.rentalDays=7;
	//else
	//	rental.rentalDays=30;
	CString sOutTraderNo=CWxPayApi::GenerateOutTradeNo(g_aplInfo.GetHardIdCodeStr(),g_aplInfo.GetNonceStr());
	COrder order;
	g_aplInfo.InitOrder(order,VER.m_idSession,*pRental);
	order.orderSerial.Copy(sOutTraderNo);
	int id=VER.SaveOrUpdateObject("Order",&order);
	if (id <= 0)
	{
		AfxMessageBox("ɨ��֧���µ�ʧ�ܣ�����ϵ�����̣�");
		return;
	}
	CString sPeriod="";
	GetDlgItem(IDC_CMB_RENTAL_PERIOD)->GetWindowText(sPeriod);
	int nMoneyCents=1;
	CString sPayURL=CNativePay::GetPayUrl(nMoneyCents,g_aplInfo.GetHardIdCodeStr(),g_aplInfo.GetNonceStr(),sOutTraderNo,GetProductName(),NULL,NULL);
	if(sPayURL.GetLength()>0)
	{
		logOnLineLic.Log("PayURL:%s��Product��%s,OrderSerial:%s", sPayURL, (char*)GetProductName(), (char*)order.orderSerial);
		StartListenPayResult();
		RENTALORDER rentalorder(sPayURL,GetProductName(),sPeriod,1,order.orderSerial);
		qrCodeDlg.SetQRCodeStr(rentalorder);
		//pPrompt->SetQRCodeStr(sPayURL,CXhChar100("΢��֧����%s %.2f Ԫ",(char*)sProductName,nMoney*0.01),this);
	}
	else
	{
		AfxMessageBox("΢��ɨ��֧����ά���ȡʧ�ܣ����Ժ����ԣ�");
		return;
	}
#endif
}