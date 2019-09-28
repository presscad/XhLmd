// LicenseServerActivatePage.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "XhLmdI.h"
#include "afxdialogex.h"
#include "LicenseAuthorizeDlg.h"
#include "XhLicAgent.h"
#include "winsvc.h"

// CLicenseServerActivatePage �Ի���

IMPLEMENT_DYNAMIC(CLicenseServerActivatePage, CDialogEx)

CLicenseServerActivatePage::CLicenseServerActivatePage(CWnd* pParent /*=NULL*/)
	: CDialogEx(CLicenseServerActivatePage::IDD, pParent)
{
	m_iDogType=0;
	m_dwLicenseApplyTimeStamp=0;
}

CLicenseServerActivatePage::~CLicenseServerActivatePage()
{
}

void CLicenseServerActivatePage::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CLicenseServerActivatePage, CDialogEx)
	ON_BN_CLICKED(IDC_BN_ACTIVATE_END_LICENSE, OnBnActivateEndLicense)
	ON_BN_CLICKED(IDC_BN_ACTIVATE_SERVER_LICENSE, &CLicenseServerActivatePage::OnBnActivateServerLicense)
	ON_BN_CLICKED(IDC_BN_CREATE_IDENTITY_FILE, &CLicenseServerActivatePage::OnBnCreateIdentityFile)
	ON_BN_CLICKED(IDC_BN_IMPORT_AUTHORIZE_FILE, &CLicenseServerActivatePage::OnBnImportAuthorizeFile)
END_MESSAGE_MAP()


// CLicenseServerActivatePage ��Ϣ�������

#include "LicenseServerActivatePage.h"
void CLicenseServerActivatePage::UpdateBtnStateByDogType(int iDogType,char ciLicenseMode/*=LIC_MODE_CHOICE*/)
{
	m_iDogType=iDogType;
	bool enabled=(ciLicenseMode==CLicenseAuthorizeDlg::LIC_MODE_CHOICE)||(ciLicenseMode==CLicenseAuthorizeDlg::LIC_MODE_CLIENT);
	enabled=enabled&&(m_iDogType!=3);
	GetDlgItem(IDC_BN_ACTIVATE_END_LICENSE)->EnableWindow(enabled);
}
void CLicenseServerActivatePage::OnBnActivateEndLicense()
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

/*DWORD MakeTimeStamp(WORD year,WORD month,WORD day,WORD hour)
{
	DWORD stamp=year<<16;	//��ݵ���ռ2Bytes
	stamp+=month<<12;		//�·�ռ13~16λ
	stamp+=day<<6;			//����ռ7~12λ
	stamp+=hour&0x002F;		//Сʱռ1~6λ
	return stamp;
}*/
void CLicenseServerActivatePage::OnBnActivateServerLicense()
{
}

void CLicenseServerActivatePage::OnBnCreateIdentityFile()
{
	int c2v_len=GetC2VInfo((BYTE)m_iDogType,NULL);
	if(c2v_len==-1)
		AfxMessageBox("δ�ҵ�������, ���ʵ�Ƿ����˷���ǽ����幷���������ͨ�Ź��ϻ�������ڰ�װ�˶�����繷����!��ȷ�Ϸ���ǽδ��,�Ҿ�����������������ͨ���趨gsnetdog.ini(ָ�����м������������ip��ַ)ֱ�ӷ���!");
	else if(c2v_len==-2)
		AfxMessageBox("����������ʧ��!");
	if(c2v_len<=0)
		return;	//��ȡʧ��
	CBuffer buffer;
	CHAR_ARRAY c2v(c2v_len);
	c2v_len=GetC2VInfo((BYTE)m_iDogType,c2v);
	COMPUTER computer;
	BYTE key[32]={0},details[192]={0};
	COMPUTER::Generate128Identity(key,&m_dwLicenseApplyTimeStamp,details,192);
	DWORD dwDetailBytes=0;
	for(dwDetailBytes=0;dwDetailBytes<192;dwDetailBytes+=16)
	{
		INT64PTR i64(details+dwDetailBytes);
		if(i64[0]==0&&i64[1]==0)
			break;
	}
	UINT uDogSerial=DogSerialNo();	//��ȡ���������к�
	buffer.WriteDword(m_dwLicenseApplyTimeStamp);		//д��ָ������ʱ���
	buffer.Write(key,32);				//д��ָ�Ʊ�ʶ��Ϣ
	buffer.WriteDword(dwDetailBytes);	//д��ָ����ϸ��Ϣ�ڴ�鳤��
	buffer.Write(details,dwDetailBytes);//д��ָ����ϸ��Ϣ
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
	buffer.Write(c2v,c2v_len);
	//д���ļ�
	CXhChar16 product_name=GetProductName();
	char fn[MAX_PATH];
	sprintf(fn,"%s%d.apl",(char*)product_name,uDogSerial);
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
		XHVERSION licapplyfile_version("0.0.0.1");
		//1.�洢apl�ļ�ͷ��Ϣ
		UINT uDocType=2834535456;	//2834535456Ϊһ�������ʾ�ź���˾ ��Ȩ�����ļ�
		fwrite(&uDocType,4,1,fp);	//doc_type, 
		fwrite(&licapplyfile_version,4,1,fp);// 
		//2.�洢ʵ��������Ϣ
		DWORD dwContentsLength=buffer.GetLength();
		fwrite(&dwContentsLength,4,1,fp);
		fwrite(buffer.GetBufferPtr(),buffer.GetLength(),1,fp);
		fclose(fp);
	}
}

//����XhLicServ
BOOL RestartXhLicServ()
{
	TCHAR m_szServiceName[256]="XhLicServ";
	SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (hSCM == NULL)
		return FALSE;
	SC_HANDLE hService = ::OpenService(hSCM, m_szServiceName, SERVICE_START | SERVICE_STOP);
	if (hService == NULL)
	{
		::CloseServiceHandle(hSCM);
		return FALSE;
	}
	SERVICE_STATUS status;
	BOOL stopped=FALSE,started=FALSE;
	stopped=::ControlService(hService, SERVICE_CONTROL_STOP, &status);
	started=::StartServiceA(hService,0,NULL);
	::CloseServiceHandle(hService);
	::CloseServiceHandle(hSCM);
	if (started)
		return TRUE;
	else
		return FALSE;
}

void CLicenseServerActivatePage::OnBnImportAuthorizeFile()
{
	UINT uDogSerial=DogSerialNo();	//��ȡ���������к�
	//д���ļ�
	CXhChar16 productname=GetProductName();
	char fn[MAX_PATH];
	strcpy(fn,productname);
	strcat(fn,CXhChar16("%d",uDogSerial));
	CFileDialog dlg(TRUE,"aut",fn,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"�û�����Ȩ�����ļ�(*.aut)|*.aut|�����ļ�(*.*)|*.*||");
	if(dlg.DoModal()!=IDOK)
		return;
	FILE* fp=fopen(dlg.GetPathName(),"rb");
	if(fp==NULL)
	{
		AfxMessageBox("�û�����������Ȩ�ļ���ʧ��!");
		return;
	}
	CWaitCursor waitcursorobj;
	XHVERSION authfile_version("0.0.0.1");
	//1.��ȡauthentication�ļ�ͷ��Ϣ
	UINT uDocType=2834534455;	//2834534455Ϊһ�������ʾ�ź���˾ ��Ȩ�ļ�
	fread(&uDocType,4,1,fp);	//doc_type, 
	if(uDocType!=2834534455)
	{
		AfxMessageBox("�û�����������Ȩ�ļ�������֤ʧ��!");
		return;
	}
	fread(&authfile_version,4,1,fp);// 
	//2.��ȡʵ��������Ϣ
	DWORD dwContentsLength=0,dwParam=0;
	fread(&dwContentsLength,4,1,fp);
	char header_bytes[75]={0};
	CHAR_ARRAY contents(dwContentsLength);
	CBuffer buffer(contents,dwContentsLength);
	fread(contents,dwContentsLength,1,fp);
	fclose(fp);
	BYTE identity[32]={0};
	buffer.Read(identity,32);
	if(!COMPUTER::Verify128Identity(identity))
	{
		AfxMessageBox("��ǰ������Ȩ�ļ��뵱ǰ�û�����ָ����Ϣ��֤ʧ��!");
		return;
	}
	// 1)��ȡ������V2C�����ļ�
	WORD v2cbuf_len=0,wContentsLen=0;
	buffer.ReadWord(&v2cbuf_len);
	if(v2cbuf_len>0)
	{
		CHAR_ARRAY v2cbytes(v2cbuf_len);
		buffer.Read(v2cbytes,11);
		XHVERSION v2c_version("0.0.0.1");
		memcpy(&v2c_version,v2cbytes,4);
		memcpy(&uDogSerial,v2cbytes+4,4);
		m_iDogType=*(v2cbytes+8);
		memcpy(&v2cbuf_len,v2cbytes+9,2);
		buffer.Read(v2cbytes+11,v2cbuf_len);
		int status=UpdateByV2CInfoData(v2cbytes,11+v2cbuf_len);
		if(status!=1&&status!=-6)
		{	//status==-6��ʾ������Ȩ�����¼���������ǰV2C�����������ܹ���ƥ�䣩
			AfxMessageBox("��������Ȩ��Ϣ����ʧ��!");
			return;
		}
	}
	// 2)��ȡ�����ɲ�ע������Ȩ�����ļ�
	buffer.ReadWord(&wContentsLen);
	if(wContentsLen>0)
	{
		CXhChar200 svcfile("%s%s.svc",execute_path,(char*)productname);
		if((fp=fopen(svcfile,"wb"))!=NULL)
		{
			fwrite(buffer.GetCursorBuffer(),wContentsLen,1,fp);
			buffer.Offset(wContentsLen);
			fclose(fp);
			HKEY hKey=NULL,hSoftwareKey=NULL;
			RegOpenKeyEx(HKEY_LOCAL_MACHINE,"Software\\Xerofox\\XhLicServ\\SrvLicenses",0,KEY_WRITE,&hKey);
			if(hKey==NULL)
			{
				DWORD dw;
				RegOpenKeyEx(HKEY_LOCAL_MACHINE,"Software",0,KEY_WRITE,&hSoftwareKey);
				RegCreateKeyEx(hSoftwareKey, "Xerofox\\XhLicServ\\SrvLicenses", 0, REG_NONE, REG_OPTION_NON_VOLATILE, KEY_WRITE|KEY_READ, NULL, &hKey, &dw);
			}
			if(hKey==NULL||RegSetValueEx(hKey,productname,NULL,REG_SZ,(BYTE*)(char*)svcfile,svcfile.GetLength())!= ERROR_SUCCESS)
				AfxMessageBox("����Ȩ�����ļ�ע��ʧ��");//FALSE;
			DWORD dwPortNumber=1376;
			if(hKey==NULL||RegSetValueEx(hKey,"PortNumber",NULL,REG_DWORD,(BYTE*)&dwPortNumber,4)!= ERROR_SUCCESS)
				AfxMessageBox("����Ȩ����˿ں�ע��ʧ��");//FALSE;
			if(hKey)
				RegCloseKey(hKey);
			//�����ź���Ȩ����
			RestartXhLicServ();
		}
		else
			AfxMessageBox(CXhChar50("����Ȩ�����ļ�{%s}����ʧ��",(char*)svcfile));
	}
	// 3)��ȡ��������֤���ļ�
	buffer.ReadWord(&wContentsLen);
	CXhChar200 licfile("%s%s.lic",execute_path,(char*)productname);
	if((fp=fopen(licfile,"wb"))!=NULL)
	{
		fwrite(buffer.GetCursorBuffer(),wContentsLen,1,fp);
		fclose(fp);
		CWnd* pParent=GetParent();
		CLicenseAuthorizeDlg* m_pParentDialg=pParent!=NULL?(CLicenseAuthorizeDlg*)pParent->GetParent():NULL;
		if(m_pParentDialg&&!m_pParentDialg->m_bRunning)
			m_pParentDialg->ImportServerPrimaryLicFile(licfile);
		if(m_pParentDialg)
			m_pParentDialg->DisableButton(IDC_BN_APPLY_FOR_LAN_LICENSE,FALSE);
	}
	else
		AfxMessageBox(CXhChar50("��֤���ļ�{%s}����ʧ��",(char*)licfile));
}
