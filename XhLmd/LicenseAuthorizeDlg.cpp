// LicenseAuthorizeDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "XhLmdI.h"
#include "LicenseAuthorizeDlg.h"
#include "XhLicAgent.h"
#include "Communication.h"
#include "VER.h"

// CLicenseAuthorizeDlg �Ի���
CStack<HINSTANCE> CModuleResourceOverride::m_xResModuleStack;
IMPLEMENT_DYNAMIC(CLicenseAuthorizeDlg, CDialogEx)

CLicenseAuthorizeDlg::CLicenseAuthorizeDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CLicenseAuthorizeDlg::IDD, pParent)
	, m_iDogType(0)
	, m_dwIpAddress(0)
	, m_sMasterLicenseHostName(_T(""))
	, m_sComputerName(_T(""))
	, m_sLicenseType(_T(""))
{
	m_cLicenseMode=LIC_MODE_CHOICE;
	m_bRunning=false;
	m_pErrLogFile=NULL;
	m_siPortNumber = 1376;
	m_nLicRetCode = 0;
	m_bInternalImported = false;
}

CLicenseAuthorizeDlg::~CLicenseAuthorizeDlg()
{
}

void CLicenseAuthorizeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RDO_DOG_TYPE, m_iDogType);
	DDX_IPAddress(pDX, IDC_IP_PRODUCT_SERVER_IP, m_dwIpAddress);
	DDX_Text(pDX, IDC_E_PRODUCT_SERVER_NAME, m_sMasterLicenseHostName);
	DDX_Text(pDX, IDC_E_CUSTOMER_NAME, m_sComputerName);
	DDV_MaxChars(pDX, m_sComputerName, 50);
	DDX_Text(pDX, IDC_E_AUTHORIZE_TYPE, m_sLicenseType);
	DDX_Control(pDX, IDC_S_ERROR_MSG, m_ctrlErrorMsg);
	DDX_Text(pDX, IDC_E_PORT_NUMBER, m_siPortNumber);
	DDV_MinMaxShort(pDX, m_siPortNumber, 0, 10000);
}


BEGIN_MESSAGE_MAP(CLicenseAuthorizeDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BN_SHIFT_TO_SERVER_LICENSE, OnBnShiftToServerLicense)
	ON_BN_CLICKED(IDC_BN_DOG_TEST, &CLicenseAuthorizeDlg::OnBnDogTest)
	ON_BN_CLICKED(IDC_BN_APPLY_FOR_LAN_LICENSE, &CLicenseAuthorizeDlg::OnBnApplyForLanLicense)
	ON_BN_CLICKED(IDOK, &CLicenseAuthorizeDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_RDO_DOG_TYPE,  &CLicenseAuthorizeDlg::OnRdoDogType)
	ON_BN_CLICKED(IDC_RDO_DOG_TYPE2, &CLicenseAuthorizeDlg::OnRdoDogType)
	ON_BN_CLICKED(IDC_RDO_DOG_TYPE3, &CLicenseAuthorizeDlg::OnRdoDogType)
	ON_BN_CLICKED(IDC_RDO_DOG_TYPE4, &CLicenseAuthorizeDlg::OnRdoDogType)
END_MESSAGE_MAP()


INT_PTR CLicenseAuthorizeDlg::DoModal(bool bAutoRenew/*=false*/)
{
	m_bInternalImported = false;
	if(bAutoRenew&&m_cLicenseMode==LIC_MODE_CLIENT&&RenewLanLicense()==0)
	{
		m_bLicfileImported=true;
		return IDOK;
	}
#ifdef __WXPAY_RENTAL_ONLINE_
	else if (m_nLicRetCode == 1)
	{	//δ�ҵ�֤�飬�������ӷ����������Ƿ�δ����״̬������Ч����
		m_bInternalImported = true;
		if (licRentalPage.TestDownloadAndImportAuthFile())
		{
			m_bLicfileImported = true;
			m_bInternalImported = false;
			return IDOK;
		}
		else
			m_bInternalImported = false;
	}
#endif
	return CDialogEx::DoModal();
}
// CLicenseAuthorizeDlg ��Ϣ�������
void CLicenseAuthorizeDlg::ShowLanLicApplyPage(int nCmdShow)
{
	CWnd* pWnd=GetDlgItem(IDC_E_PRODUCT_SERVER_NAME);
	if(pWnd)
		pWnd->ShowWindow(nCmdShow);
	pWnd=GetDlgItem(IDC_IP_PRODUCT_SERVER_IP);
	if(pWnd)
		pWnd->ShowWindow(nCmdShow);
	pWnd=GetDlgItem(IDC_S_PRODUCT_SERVER_IP);
	if(pWnd)
		pWnd->ShowWindow(nCmdShow);
	pWnd=GetDlgItem(IDC_S_PRODUCT_SERVER_NAME);
	if(pWnd)
		pWnd->ShowWindow(nCmdShow);
	pWnd=GetDlgItem(IDC_S_PORT_NUMBER);
	if(pWnd)
		pWnd->ShowWindow(nCmdShow);
	pWnd=GetDlgItem(IDC_E_PORT_NUMBER);
	if(pWnd)
		pWnd->ShowWindow(nCmdShow);
	pWnd=GetDlgItem(IDC_BN_APPLY_FOR_LAN_LICENSE);
	if(pWnd)
		pWnd->ShowWindow(nCmdShow);
	pWnd=GetDlgItem(IDC_BN_SHIFT_TO_SERVER_LICENSE);
	if(pWnd)
		pWnd->ShowWindow(nCmdShow);
	InvalidateRect(&workpanelRc);
}
static DWORD MakeIpAddr(BYTE b1,BYTE b2,BYTE b3,BYTE b4)
{
	return (DWORD)(((DWORD)(b1)<<24)+((DWORD)(b2)<<16)+((DWORD)(b3)<<8)+((DWORD)(b4)));
}
static DWORD MakeIpAddr(BYTE* bytes)
{
	return MakeIpAddr(bytes[0],bytes[1],bytes[2],bytes[3]);
}
BOOL CLicenseAuthorizeDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	licServerPage.Create(IDD_LICENSE_SERVER_ACTIVATE_DLG,GetDlgItem(IDC_S_APPLY_LICENSE_PANEL));
	licRentalPage.Create(IDD_LICENSE_VIRTUAL_RENTAL_DLG,GetDlgItem(IDC_S_APPLY_LICENSE_PANEL));

	GetDlgItem(IDC_S_APPLY_LICENSE_PANEL)->GetWindowRect(&workpanelRc);//��ȡpicture  control�ؼ�������
	ScreenToClient(workpanelRc);


	m_iDogType=AgentDogType();
	UINT dwDogSerial=DogSerialNo();
	licServerPage.m_iDogType=licRentalPage.m_iDogType=m_iDogType;
	m_dwIpAddress=GetProfileIntFromReg("Settings","master_host_ip");
	m_sMasterLicenseHostName=GetProfileStringFromReg("Settings","master_host_name");
	UINT siPortNumber=GetProfileIntFromReg("Settings","master_host_port_number");
	if(siPortNumber>0)
		m_siPortNumber=siPortNumber;
	if(m_cLicenseMode==LIC_MODE_CLIENT)
	{
		ShowLanLicApplyPage(SW_SHOW);
		licServerPage.ShowWindow(false);
		GetDlgItem(IDC_BN_SHIFT_TO_SERVER_LICENSE)->ShowWindow(SW_HIDE);
	}
	else if(m_cLicenseMode==LIC_MODE_SERVER)
	{
		ShowLanLicApplyPage(SW_HIDE);
		licServerPage.ShowWindow(true);
		licRentalPage.ShowWindow(false);
	}
	else if(m_cLicenseMode==LIC_SOFT_LICENSE)
	{
		ShowLanLicApplyPage(SW_HIDE);
		licServerPage.ShowWindow(false);
		licRentalPage.ShowWindow(true);
	}
	else
	{
		licServerPage.ShowWindow(true);
		ShowLanLicApplyPage(SW_HIDE);
		if(LIC_MODE_SERVER==m_cLicenseMode)
			licServerPage.GetDlgItem(IDC_BN_ACTIVATE_END_LICENSE)->ShowWindow(SW_HIDE);
		else
			licServerPage.GetDlgItem(IDC_BN_ACTIVATE_END_LICENSE)->ShowWindow(SW_SHOW);
	}
	RECT rc;//=workpanelRc;
	GetDlgItem(IDC_S_APPLY_LICENSE_PANEL)->GetClientRect(&rc);
	rc.top+=2;
	rc.left+=1;
	rc.right-=1;
	rc.bottom-=1;
	licServerPage.MoveWindow(&rc);
	licRentalPage.MoveWindow(&rc);

	COMPUTER computer;
	m_sComputerName=computer.sComputerName;
	/*BYTE ipbytes[4];
	computer.GetIP(ipbytes,m_sComputerName);
	m_dwIpAddress=MakeIpAddr(ipbytes)*/
	BYTE authinfo[8];
	GetProductAuthorizeInfo(authinfo);
	DWORD leastRentalTime=0;
	//if (VerifyValidFunction(LICFUNC::FUNC_IDENTITY_INTERNAL_TEST))
	//	leastRentalTime = GetRentalLeastTime();
#ifdef __WXPAY_RENTAL_ONLINE_
	//else
	{
		COrder validOrder;
		int idSession = VER.m_idSession;
		if (!(idSession & 0x80000000 || idSession & 0x40000000) && VER.QueryValidRentalOrder(idSession, PRODUCT_TYPE, &validOrder))
		{	//����ʣ������
			CTime now2sec = CTime::GetCurrentTime();
			int nYear = validOrder.applyTimeStamp >> 16;
			int nMonth = (validOrder.applyTimeStamp & 0x0000FFFF) >> 12;
			int nDay = (validOrder.applyTimeStamp & 0x00000FFF) >> 6;
			int nHour = validOrder.applyTimeStamp & 0x0000003F;
			CTime dwTimeStart(nYear, nMonth, nDay, nHour, 0, 0);
			DWORD dwTimeEnd = dwTimeStart.GetSecond() + validOrder.rentalDays * 86400;
			leastRentalTime = dwTimeEnd - now2sec.GetSecond();
		}
	}
#endif
	if(AgentDogType()==3&&leastRentalTime>0)	//������Ȩ
	{
		DWORD leastDays =leastRentalTime/86400;
		DWORD leastHours=(leastRentalTime%86400)/3600;
		m_sLicenseType+=CXhChar50("ʣ��%d��%dʱ",leastDays,leastHours);
	}
	else if(authinfo[7]&0x01)
		m_sLicenseType+="������Ȩ";
	else if(authinfo[7]&0x02)
		m_sLicenseType+="�ڼ���Ȩ";
	else
		m_sLicenseType="";
	if(m_bRunning)
	{
		GetDlgItem(IDC_RDO_DOG_TYPE)->EnableWindow(FALSE);
		GetDlgItem(IDC_RDO_DOG_TYPE2)->EnableWindow(FALSE);
		GetDlgItem(IDC_RDO_DOG_TYPE3)->EnableWindow(FALSE);
		GetDlgItem(IDC_RDO_DOG_TYPE4)->EnableWindow(FALSE);
		if(m_iDogType==3)
			licRentalPage.UpdateBtnStateByDogType(m_iDogType);
		else
			licServerPage.UpdateBtnStateByDogType(m_iDogType,m_cLicenseMode);
	}
	GetDlgItem(IDC_S_HARDLOCK_SERIAL)->SetWindowText(CXhChar16("%d",dwDogSerial));
	UpdateData(FALSE);
	m_ctrlErrorMsg.EnableURL(FALSE);
	m_ctrlErrorMsg.EnableHover(FALSE);
	m_ctrlErrorMsg.SetColours(RGB(255,0,0),RGB(255,0,0),RGB(255,0,0));
	m_ctrlErrorMsg.SetWindowText(m_sErrorMsg);
	m_ctrlErrorMsg.SetURL(m_sErrorMsg);	//��ʾ��Ϣ
	m_bLicfileImported=false;
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CLicenseAuthorizeDlg::DisableButton(UINT idButton,BOOL bEnable/*=FALSE*/)
{
	CWnd* pBtn=GetDlgItem(idButton);
	if(pBtn)
		pBtn->EnableWindow(bEnable);
}

void CLicenseAuthorizeDlg::OnBnDogTest()
{
	if(!FindDog((BYTE)m_iDogType))
		AfxMessageBox("δ�ҵ�������, ���ʵ�Ƿ����˷���ǽ����幷���������ͨ�Ź��ϻ�������ڰ�װ�˶�����繷����!��ȷ�Ϸ���ǽδ��,�Ҿ�����������������ͨ���趨gsnetdog.ini(ָ�����м������������ip��ַ)ֱ�ӷ���!");
	else if(m_iDogType==0&&!ConnectDog((BYTE)m_iDogType))
		AfxMessageBox("����������ʧ��!");
	else
	{
		UINT dwDogSerial=DogSerialNo();
		GetDlgItem(IDC_S_HARDLOCK_SERIAL)->SetWindowText(CXhChar16("%d",dwDogSerial));
		MessageBox(CXhChar50("�ҵ�������%d, ���ɹ����������Ӳ���!\n",dwDogSerial));
	}
}

void CLicenseAuthorizeDlg::OnBnShiftToServerLicense()
{
	ShowLanLicApplyPage(SW_HIDE);
	licRentalPage.ShowWindow(false);
	licServerPage.ShowWindow(true);
	InvalidateRect(&workpanelRc);
}

bool CLicenseAuthorizeDlg::_ImportLicFile(char* licfile, char* errorMsg)
{
	DWORD retCode=InternalImportLicFile(licfile,PRODUCT_TYPE,dwVersion);	
	CXhChar100 errormsgstr;
	bool bLicfileImported = false;
	if(retCode==0)
	{
		if(GetLicVersion()<1000005)
#ifdef AFX_TARG_ENU_ENGLISH
			errormsgstr.Copy("The certificate file has been out of date, the current software's version must use the new certificate file��");
#else 
			errormsgstr.Copy("��֤���ļ��ѹ�ʱ����ǰ����汾����ʹ����֤�飡");
#endif
		else if(!IsVerifyValidFunction())
#ifdef AFX_TARG_ENU_ENGLISH
			errormsgstr.Copy("Software Lacks of legitimate use authorized!");
#else 
			errormsgstr.Copy("���ȱ�ٺϷ�ʹ����Ȩ!");
#endif
		else
		{
			bLicfileImported=true;
			WriteProfileString("Settings","lic_file2",licfile);
		}
		if(!bLicfileImported)
			ExitCurrentDogSession();
	}
	else
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(retCode==-1)
			errormsgstr.Copy("0# Hard dog failed to initialize!");
		else if(retCode==1)
			errormsgstr.Copy("1# Unable to open the license file!");
		else if(retCode==2)
			errormsgstr.Copy("2# License file was damaged!");
		else if(retCode==3||retCode==4)
			errormsgstr.Copy("3# License file not found or does'nt match the hard lock!");
		else if(retCode==5)
			errormsgstr.Copy("5# License file doesn't match the authorized products in hard lock!");
		else if(retCode==6)
			errormsgstr.Copy("6# Beyond the scope of authorized version !");
		else if(retCode==7)
			errormsgstr.Copy("7# Beyond the scope of free authorize version !");
		else if(retCode==8)
			errormsgstr.Copy("8# The serial number of license file does'nt match the hard lock!");
#else 
		if(retCode==-1)
			errormsgstr.Copy("0#���ܹ���ʼ��ʧ��!");
		else if(retCode==1)
			errormsgstr.Copy("1#�޷���֤���ļ�");
		else if(retCode==2)
			errormsgstr.Copy("2#֤���ļ��⵽�ƻ�");
		else if(retCode==3||retCode==4)
			errormsgstr.Copy("3#ִ��Ŀ¼�²�����֤���ļ���֤���ļ�����ܹ���ƥ��");
		else if(retCode==5)
			errormsgstr.Copy("5#֤������ܹ���Ʒ��Ȩ�汾��ƥ��");
		else if(retCode==6)
			errormsgstr.Copy("6#�����汾ʹ����Ȩ��Χ");
		else if(retCode==7)
			errormsgstr.Copy("7#������Ѱ汾������Ȩ��Χ");
		else if(retCode==8)
			errormsgstr.Copy("8#֤������뵱ǰ���ܹ���һ��");
		else if(retCode==9)
			errormsgstr.Copy("9#��Ȩ���ڣ���������Ȩ");
		else if(retCode==10)
			errormsgstr.Copy("10#����ȱ����Ӧִ��Ȩ�ޣ����Թ���ԱȨ�����г���");
		else if (retCode == 11)
			errormsgstr.Copy("11#��Ȩ�쳣����ʹ�ù���ԱȨ����������֤��");
		else
			errormsgstr.Printf("δ֪���󣬴������%d#", retCode);
#endif
		ExitCurrentDogSession();
		bLicfileImported = false;
	}
	if (!bLicfileImported)
	{
		if (errorMsg != NULL)
			strncpy(errorMsg, errormsgstr, 100);
		//m_ctrlErrorMsg.SetWindowText(errormsgstr);
		//m_ctrlErrorMsg.SetURL((char*)errormsgstr);	//��ʾ��Ϣ
	}
	return bLicfileImported;
}
bool CLicenseAuthorizeDlg::ImportServerPrimaryLicFile(char* licfile)
{
	CXhChar100 sErrorMsg;
	if (_ImportLicFile(licfile, sErrorMsg) && !m_bInternalImported)
	{
		OnBnClickedOk();
		return true;
	}
	else if (m_ctrlErrorMsg.GetSafeHwnd() != NULL)
	{
		m_ctrlErrorMsg.SetWindowText(sErrorMsg);
		m_ctrlErrorMsg.SetURL((char*)sErrorMsg);	//��ʾ��Ϣ
	}
	return false;
}
void CLicenseAuthorizeDlg::OnBnApplyForLanLicense()
{
	CWaitCursor waitCursor;
	UpdateData();
	WriteProfileIntToReg("Settings","master_host_ip",m_dwIpAddress);
	WriteProfileStringToReg("Settings","master_host_name",m_sMasterLicenseHostName);
	WriteProfileIntToReg("Settings","master_host_port_number",m_siPortNumber);
	CXhChar200 srvname("%s",m_sMasterLicenseHostName),sHostIpAddress;
	BYTE* ipbytes=(BYTE*)&m_dwIpAddress;
	if(srvname.GetLength()==0&&m_dwIpAddress!=0)
		srvname.Printf("%d.%d.%d.%d",ipbytes[3],ipbytes[2],ipbytes[1],ipbytes[0]);
	sHostIpAddress.Printf("%d.%d.%d.%d",ipbytes[3],ipbytes[2],ipbytes[1],ipbytes[0]);
	CXhChar200 corePipeName("\\\\%s\\Pipe\\XhLicServ\\CorePipe",(char*)srvname);
	CXhChar200 readPipeName("\\\\%s\\Pipe\\XhLicServ\\%s_ListenPipe",(char*)srvname,m_sComputerName);
	CCommunicationObject commCore(sHostIpAddress,m_siPortNumber,corePipeName,false);
	CXhChar200 sError=commCore.GetErrorStr();
	if(sError.GetLength()>0)
	{
		sError.Append(CXhChar100(". ����ͨ�Źܵ�{%s}����ʧ��!",(char*)corePipeName));
		AfxMessageBox(sError);
		return;
	}
	CXhChar16 szSubKey=GetProductName();
	if(szSubKey.GetLength()<=0)
		AfxMessageBox(CXhChar50("Ŀǰ�ݲ�֧�ֵĲ�Ʒ��Ȩ#%d",PRODUCT_TYPE));
	CXhChar200 errfile("%s%s.err",execute_path,(char*)szSubKey);
	CTempFileBuffer error(errfile);
	error.EnableAutoDelete(false);
	m_pErrLogFile=&error;
	LICSERV_MSGBUF cmdmsg;
	cmdmsg.command_id=LICSERV_MSGBUF::APPLY_FOR_LICENSE;
	cmdmsg.src_code=0;
	CBuffer buffer;
	COMPUTER computer;
	BYTE identity[32],details[512]={0};
	computer.Generate128Identity(identity,NULL,details,512);
	//COMPUTER::Verify128Identity(identity);
	WORD wDetailsLen=0;
	for(WORD i=0;i<512;i+=16)
	{
		if(((__int64*)(details+i))[0]==0&&((__int64*)(details+i))[1]==0)
			break;
	}
	wDetailsLen=i;
	CXhChar16 ipstr("%d.%d.%d.%d",computer.computer_ip[0],computer.computer_ip[1],computer.computer_ip[2],computer.computer_ip[3]);
	//ReadFile(hCorePipe,msg_contents,msg.msg_length-32,&nBytesRead,NULL);
	//char status,cApplyProductType=0;
	DWORD nBytesRead,nBytesWritten;
	CXhChar50 hostname,hostipstr;
	buffer.WriteString(ipstr,17);
	buffer.WriteString(m_sComputerName,51);
	buffer.WriteWord(wDetailsLen);
	buffer.Write(details,wDetailsLen);
	buffer.WriteByte(PRODUCT_TYPE);
	buffer.WriteDword((DWORD)time(NULL));
	buffer.Write(identity,32);
	cmdmsg.msg_length=buffer.GetLength();
	error.WriteInteger(buffer.GetLength());
	error.Write(buffer.GetBufferPtr(),buffer.GetLength());
	if(!commCore.Write(&cmdmsg,9,&nBytesWritten))
	{
		error.WriteBooleanThin(false);
		return;
	}
	else
		error.WriteBooleanThin(true);
	if(!commCore.Write(buffer.GetBufferPtr(),buffer.GetLength(),&nBytesWritten))
	{
		error.WriteBooleanThin(false);
		DWORD dw=GetLastError();
		return;
	}
	else
		error.WriteBooleanThin(true);
	char state,status=-100;
	DWORD dwRecvBytesLen=0; 
	CCommunicationObject *pCommRead=&commCore;
	CCommunicationObject commRead;
	if(commCore.IsPipeConnect())
	{
		commRead.InitPipeConnect(readPipeName,false);
		pCommRead=&commRead;
	}
	if(!(state=pCommRead->Read(&status,1,&nBytesRead))||status<=0)
	{
		if(!pCommRead->IsValidConnection())
		{
			sError=pCommRead->GetErrorStr();
			sError.Append(". ʵ��ͨ�Źܵ�����ʧ��!");
			AfxMessageBox(sError);
		}
		sError=pCommRead->GetErrorStr();
		error.WriteByte(status);
		if(status==0)
			AfxMessageBox("��֤������ʧ�ܣ���鿴��Ȩ����������c:\\service.log��־���ų�����");
		else if(status==-1)
			AfxMessageBox("��ǰ��������Ȩ��������ʱ�Ӵ��ڹ���ƫ���У׼ʱ�Ӻ���������Ȩ");
		else if(status==-2)
			AfxMessageBox("��������ȱ�ٶ���Ӧ��Ʒ������Ȩ�����ļ�");
		else if(status==-3)
			AfxMessageBox("������������Ӧ��Ʒ������Ȩ��������Ȩ��������");
		return;
	}
	else
		error.WriteByte(status);
	if(!pCommRead->Read(&dwRecvBytesLen,4,&nBytesRead))
	{
		error.WriteInteger(0);
		return;
	}
	else
		error.WriteInteger(nBytesRead);
	BYTE_ARRAY licbuf_bytes(dwRecvBytesLen);
	if(!pCommRead->Read(licbuf_bytes,dwRecvBytesLen,&nBytesRead))
	{
		error.WriteBooleanThin(false);
		return;
	}
	else
		error.WriteBooleanThin(true);
	//���ݽ����������ɱ���֤���ע����ּ���֤���ļ�����
	//1.д��724��ע����ʼ���ŵ�����
	if(!WriteProfileBinaryToReg("",_T("Obfuscator"),(BYTE*)licbuf_bytes,724))
	{
		error.WriteBooleanThin(false);
		error.Write(licbuf_bytes,724);
		return;
	}
	else
		error.WriteBooleanThin(true);
	CXhChar200 licfile("%s%s.lic",execute_path,(char*)szSubKey);
	error.WriteString(licfile);
	FILE* fp=fopen(licfile,"wb");
	if(fp==NULL)
		error.WriteBooleanThin(false);
	else
	{
		error.WriteBooleanThin(true);
		fwrite(licbuf_bytes+724,dwRecvBytesLen-724,1,fp);
		fclose(fp);
	}
	WIN32_FIND_DATA FindFileData;
	if(FindFirstFileA(licfile,&FindFileData)==NULL)
		AfxMessageBox(CXhChar200("'%s'֤���ļ�δ�ҵ�",(char*)licfile));
	else
		MessageBox(CXhChar200("'%s'֤���ļ�������",(char*)licfile));
	CXhChar100 sErrorMsg;
	if(_ImportLicFile(licfile,sErrorMsg))
	{
		error.EnableAutoDelete(true);
		OnBnClickedOk();
	}
	else if (m_ctrlErrorMsg.GetSafeHwnd() != NULL)
	{
		m_ctrlErrorMsg.SetWindowText(sErrorMsg);
		m_ctrlErrorMsg.SetURL((char*)sErrorMsg);	//��ʾ��Ϣ
	}
}

int CLicenseAuthorizeDlg::RenewLanLicense()	//�����������Ȩ
{
	CWaitCursor waitCursor;
	DWORD dwIpAddress=GetProfileIntFromReg("Settings","master_host_ip");
	CString masterLicenseHostName=GetProfileStringFromReg("Settings","master_host_name");
	u_short siPortNumber=GetProfileIntFromReg("Settings","master_host_port_number");
	CXhChar200 srvname("%s",masterLicenseHostName),sHostIpAddress;
	BYTE* ipbytes=(BYTE*)&dwIpAddress;
	if(srvname.GetLength()==0&&dwIpAddress!=0)
		srvname.Printf("%d.%d.%d.%d",ipbytes[3],ipbytes[2],ipbytes[1],ipbytes[0]);
	sHostIpAddress.Printf("%d.%d.%d.%d",ipbytes[3],ipbytes[2],ipbytes[1],ipbytes[0]);
	COMPUTER computer;
	CXhChar200 corePipeName("\\\\%s\\Pipe\\XhLicServ\\CorePipe",(char*)srvname);
	CXhChar200 readPipeName("\\\\%s\\Pipe\\XhLicServ\\%s_ListenPipe",(char*)srvname,computer.sComputerName);
	CCommunicationObject commCore(sHostIpAddress,siPortNumber,corePipeName,false);
	CXhChar200 sError=commCore.GetErrorStr();
	if(sError.GetLength()>0)
	{
		//sError.Append(CXhChar100(". ����ͨ�Źܵ�{%s}����ʧ��!",(char*)corePipeName));
		//AfxMessageBox(sError);
		return -2;
	}
	CXhChar16 szSubKey=GetProductName();
	if(szSubKey.GetLength()<=0)
		AfxMessageBox(CXhChar50("Ŀǰ�ݲ�֧�ֵĲ�Ʒ��Ȩ#%d",PRODUCT_TYPE));
	CXhChar200 errfile("%s%s.err",execute_path,(char*)szSubKey);
	CTempFileBuffer error(errfile);
	error.EnableAutoDelete(false);
	//m_pErrLogFile=&error;
	LICSERV_MSGBUF cmdmsg;
	cmdmsg.command_id=LICSERV_MSGBUF::APPLY_FOR_LICENSE;
	cmdmsg.src_code=0;
	CBuffer buffer;
	BYTE identity[32],details[512]={0};
	computer.Generate128Identity(identity,NULL,details,512);
	//COMPUTER::Verify128Identity(identity);
	WORD wDetailsLen=0;
	for(WORD i=0;i<512;i+=16)
	{
		if(((__int64*)(details+i))[0]==0&&((__int64*)(details+i))[1]==0)
			break;
	}
	wDetailsLen=i;
	CXhChar16 ipstr("%d.%d.%d.%d",computer.computer_ip[0],computer.computer_ip[1],computer.computer_ip[2],computer.computer_ip[3]);
	//ReadFile(hCorePipe,msg_contents,msg.msg_length-32,&nBytesRead,NULL);
	//char status,cApplyProductType=0;
	DWORD nBytesRead,nBytesWritten;
	CXhChar50 hostname,hostipstr;
	buffer.WriteString(ipstr,17);
	buffer.WriteString(computer.sComputerName,51);
	buffer.WriteWord(wDetailsLen);
	buffer.Write(details,wDetailsLen);
	buffer.WriteByte(PRODUCT_TYPE);
	buffer.WriteDword((DWORD)time(NULL));
	buffer.Write(identity,32);
	cmdmsg.msg_length=buffer.GetLength();
	error.WriteInteger(buffer.GetLength());
	error.Write(buffer.GetBufferPtr(),buffer.GetLength());
	if(!commCore.Write(&cmdmsg,9,&nBytesWritten))
	{
		error.WriteBooleanThin(false);
		return -2;
	}
	else
		error.WriteBooleanThin(true);
	if(!commCore.Write(buffer.GetBufferPtr(),buffer.GetLength(),&nBytesWritten))
	{
		error.WriteBooleanThin(false);
		return -2;
	}
	else
		error.WriteBooleanThin(true);
	char state,status=-100;
	DWORD dwRecvBytesLen=0;
	CCommunicationObject *pCommRead=&commCore;
	CCommunicationObject commRead("",0,readPipeName,false);
	if(commCore.IsPipeConnect())
		pCommRead=&commRead;
	if(!(state=pCommRead->Read(&status,1,&nBytesRead))||status<=0)
	{
		if(!pCommRead->IsValidConnection())
		{
			sError=pCommRead->GetErrorStr();
			sError.Append(". ʵ��ͨ�Źܵ�����ʧ��!");
			AfxMessageBox(sError);
		}
		error.WriteByte(status);
		if(status==0)
			AfxMessageBox("��֤������ʧ�ܣ���鿴��Ȩ����������c:\\service.log��־���ų�����");
		else if(status==-1)
			AfxMessageBox("��ǰ��������Ȩ��������ʱ�Ӵ��ڹ���ƫ���У׼ʱ�Ӻ���������Ȩ");
		else if(status==-2)
			AfxMessageBox("��������ȱ�ٶ���Ӧ��Ʒ������Ȩ�����ļ�");
		else if(status==-3)
			AfxMessageBox("������������Ӧ��Ʒ������Ȩ��������Ȩ��������");
		return -2;
	}
	else
		error.WriteByte(status);
	if(!pCommRead->Read(&dwRecvBytesLen,4,&nBytesRead))
	{
		error.WriteInteger(0);
		return -2;
	}
	else
		error.WriteInteger(nBytesRead);
	BYTE_ARRAY licbuf_bytes(dwRecvBytesLen);
	if(!pCommRead->Read(licbuf_bytes,dwRecvBytesLen,&nBytesRead))
	{
		error.WriteBooleanThin(false);
		return -2;
	}
	else
		error.WriteBooleanThin(true);
	//���ݽ����������ɱ���֤���ע����ּ���֤���ļ�����
	//1.д��724��ע����ʼ���ŵ�����
	if(!WriteProfileBinaryToReg("",_T("Obfuscator"),(BYTE*)licbuf_bytes,724))
	{
		error.WriteBooleanThin(false);
		error.Write(licbuf_bytes,724);
		return -2;
	}
	else
		error.WriteBooleanThin(true);
	CXhChar200 licfile("%s%s.lic",execute_path,(char*)szSubKey);
	error.WriteString(licfile);
	FILE* fp=fopen(licfile,"wb");
	if(fp==NULL)
		error.WriteBooleanThin(false);
	else
	{
		error.WriteBooleanThin(true);
		fwrite(licbuf_bytes+724,dwRecvBytesLen-724,1,fp);
		fclose(fp);
	}
	WIN32_FIND_DATA FindFileData;
	if(FindFirstFileA(licfile,&FindFileData)==NULL)
		AfxMessageBox(CXhChar200("'%s'֤���ļ�δ�ҵ�",(char*)licfile));
	int retCode=InternalImportLicFile(licfile,PRODUCT_TYPE,dwVersion);
	if(retCode==0&&(GetLicVersion()<1000004||!IsVerifyValidFunction()))
		retCode=9;	//ͨ��������������Ȩʧ��
	if(retCode==0)
		error.EnableAutoDelete(true);
	else
		ExitCurrentDogSession();
	return retCode;
}

void CLicenseAuthorizeDlg::OnBnClickedOk()
{
	UpdateData();
	WriteProfileIntToReg("Settings","master_host_ip",m_dwIpAddress);
	WriteProfileStringToReg("Settings","master_host_name",m_sMasterLicenseHostName);
	WriteProfileIntToReg("Settings","master_host_port_number",m_siPortNumber);
	CDialogEx::OnOK();
}


void CLicenseAuthorizeDlg::OnRdoDogType()
{
	UpdateData();
	licServerPage.m_iDogType=licRentalPage.m_iDogType=m_iDogType;
	GetDlgItem(IDC_BN_SHIFT_TO_SERVER_LICENSE)->EnableWindow(m_iDogType!=3);
	GetDlgItem(IDC_BN_APPLY_FOR_LAN_LICENSE)->EnableWindow(m_iDogType!=3);
	if(licServerPage.m_iDogType==3)	//����������
	{
		ShowLanLicApplyPage(SW_HIDE);
		licServerPage.ShowWindow(false);
		licRentalPage.ShowWindow(true);
		InvalidateRect(&workpanelRc);
	}
	else
	{
		ShowLanLicApplyPage(SW_HIDE);
		licServerPage.ShowWindow(true);
		licRentalPage.ShowWindow(false);
		InvalidateRect(&workpanelRc);
	}
	licServerPage.UpdateBtnStateByDogType(m_iDogType);
	licRentalPage.UpdateBtnStateByDogType(m_iDogType);
}

void CLicenseAuthorizeDlg::InitLicenseModeByLicFile(const char* lic_file)
{
	m_cLicenseMode=LIC_MODE_CHOICE;
	CString sLicFile;
	if(lic_file==NULL)
		sLicFile=GetProfileStringFromReg("Settings","lic_file2");
	else
		sLicFile=lic_file;
	FILE* fp=fopen(sLicFile,"rb");
	if(fp==NULL)
		return;
	DWORD dwLicVersion=0;
	fread(&dwLicVersion,4,1,fp);
	if(dwLicVersion>=1000004)
	{
		BYTE ciDogType=0;
		fread(&ciDogType,1,1,fp);
		if(ciDogType==3&&dwLicVersion>=1000005)
			m_cLicenseMode=LIC_SOFT_LICENSE;
		else
		{
			fseek(fp,2,SEEK_CUR);	//2Byte wModule
			BYTE ciLicenseType=0;
			fread(&ciLicenseType,1,1,fp);
			if(ciLicenseType==1)
				m_cLicenseMode=LIC_MODE_SERVER;
			else if(ciLicenseType==2)	//����涯̬��֤���ļ�
				m_cLicenseMode=LIC_MODE_CLIENT;
		}
	}
	fclose(fp);
}
