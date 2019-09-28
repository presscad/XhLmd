// WorkLoop.cpp: implementation of the CMainWorkLoop class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "direct.h"
#include "MD5.h"
#include "GSMH.h"
#include "MainWorkLoop.h"
#include "DogAgentAPI.h"
#include "ArrayList.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
DWORD CMainWorkLoop::m_hNetDogSession=0;
SECURITY_ATTRIBUTES CMainWorkLoop::sa;
CHashList<DOGSESSION> CMainWorkLoop::hashSessions;
//char CMainWorkLoop::m_sFileVersion[21]="2.52.1.1";
CMainWorkLoop _MainLoop;
char *PIPE_PATH="\\\\.\\Pipe\\XhLmd_Stub";
char *PIPE_CORE_NAME="\\\\.\\Pipe\\XhLmd_Stub\\CorePipe";
BOOL CMainWorkLoop::m_bQuitNow=FALSE;
HANDLE CMainWorkLoop::m_hCorePipe=NULL;
//主任务循环
void GetSysPath(char* App_Path,char *src_path=NULL)
{
	char drive[4];
	char dir[MAX_PATH];
	char fname[MAX_PATH];
	char ext[MAX_PATH];

	if(src_path)
		_splitpath(src_path,drive,dir,fname,ext);
	else
		_splitpath(__argv[0],drive,dir,fname,ext);
	strcpy(App_Path,drive);
	strcat(App_Path,dir);
	_chdir(App_Path);
}
void AssignDogSessionKey(DOGSESSION* pSession,DWORD key){pSession->m_idSession=(UINT)key;}
CMainWorkLoop::CMainWorkLoop()
{
	PSECURITY_DESCRIPTOR    pSD;
									   // create a security NULL security
									   // descriptor, one that allows anyone
									   // to write to the pipe... WARNING
									   // entering NULL as the last attribute
									   // of the CreateNamedPipe() will
									   // indicate that you wish all
									   // clients connecting to it to have
									   // all of the same security attributes
									   // as the user that started the
									   // pipe server.

	pSD = (PSECURITY_DESCRIPTOR) LocalAlloc(LPTR,
			   SECURITY_DESCRIPTOR_MIN_LENGTH);
	if (!InitializeSecurityDescriptor(pSD, SECURITY_DESCRIPTOR_REVISION))
	{
		LocalFree((HLOCAL)pSD);
		return;
	}
							   // add a NULL disc. ACL to the
							   // security descriptor.
	if (!SetSecurityDescriptorDacl(pSD, TRUE, (PACL) NULL, FALSE))
	 {
		LocalFree((HLOCAL)pSD);
		return;
	 }

	CMainWorkLoop::sa.nLength = sizeof(sa);
	CMainWorkLoop::sa.lpSecurityDescriptor = pSD;
	CMainWorkLoop::sa.bInheritHandle = TRUE;

	m_hNetDogSession=0;
	hashSessions.LoadDefaultObjectInfo=AssignDogSessionKey;
}

CMainWorkLoop::~CMainWorkLoop()
{
	EndLoop();
	if(CMainWorkLoop::sa.lpSecurityDescriptor)
		LocalFree((HLOCAL)CMainWorkLoop::sa.lpSecurityDescriptor);
}

DWORD WINAPI SubPipeInstanceProc(LPVOID lpParameter)
{
	DOGSESSION *pSession=(DOGSESSION*)lpParameter;
	ConnectNamedPipe(pSession->m_hServerListenPipe,NULL);
	ConnectNamedPipe(pSession->m_hServerSendPipe,NULL);
	MSGBUF msg;
	DWORD nBytesRead;
	int i=0;
	while(ReadFile(pSession->m_hServerListenPipe,&msg,sizeof(MSGHEAD),&nBytesRead,NULL))
	{
		DYN_ARRAY<BYTE> msg_buf(msg.msg_length);
		if(msg.msg_length>0)
		{
			msg.lpBuffer=(BYTE*)msg_buf;
			ReadFile(pSession->m_hServerListenPipe,msg.lpBuffer,msg.msg_length,&nBytesRead,NULL);
		}
		if(msg.command_id==LOGOUT_DOG)
		{
			CMainWorkLoop::Logout(pSession);
			break;	//退出会话
		}
		else if(msg.command_id==ASK_DOG_SERIAL_NO)
			CMainWorkLoop::AskDogSerialNo(pSession);
		else if(msg.command_id==VERIFY_VERSION)
			CMainWorkLoop::VerifyVersion(pSession,msg);
		else if(msg.command_id==ASK_ENCRYPT_KEY)
			CMainWorkLoop::AskEncryptKey(pSession,msg);
		else if(msg.command_id==ASK_DECRYPT_KEY)
			CMainWorkLoop::AskDecryptKey(pSession,msg);
		else if(msg.command_id==ASK_FALSE_KEY)
			CMainWorkLoop::AskFalseKey(pSession,msg);
		else if(msg.command_id==ASK_DOG_ENCRYPT)
			CMainWorkLoop::AskDogEncrypt(pSession,msg);
		else if(msg.command_id==ASK_DOG_DECRYPT)
			CMainWorkLoop::AskDogDecrypt(pSession,msg);
		else if(msg.command_id==ASK_READ_DOG_DATA)
			CMainWorkLoop::AskReadDogData(pSession,msg);
		else if(msg.command_id==ASK_WRITE_DOG_DATA)
			CMainWorkLoop::AskWriteDogData(pSession,msg);
		else if(msg.command_id==ASK_DOG_PERM_STATE)
			CMainWorkLoop::AskDogPermState(pSession);
		else if(msg.command_id==ASK_EXPIRE_TIMES)
			CMainWorkLoop::AskExpireTimes(pSession);
		else if(msg.command_id==DECREASE_EXPIRE_TIMES)
			CMainWorkLoop::DecreaseExpireTimes(pSession);
	}
	if(pSession->m_hServerListenPipe)
	{
		DisconnectNamedPipe(pSession->m_hServerListenPipe);
		CloseHandle(pSession->m_hServerListenPipe);
		pSession->m_hServerListenPipe=NULL;
	}
	if(pSession->m_hServerSendPipe)
	{
		DisconnectNamedPipe(pSession->m_hServerSendPipe);
		CloseHandle(pSession->m_hServerSendPipe);
		pSession->m_hServerSendPipe=NULL;
	}
	CMainWorkLoop::hashSessions.DeleteNode(pSession->m_idSession);
	return TRUE;
}
DWORD WINAPI MainPipeInstanceProc(LPVOID lpParameter)
{
	CMainWorkLoop::m_hCorePipe= CreateNamedPipe(PIPE_CORE_NAME,PIPE_ACCESS_DUPLEX,PIPE_TYPE_BYTE|PIPE_READMODE_BYTE,
												32,0,0,2000,&CMainWorkLoop::sa);
	BYTE buffer[2048]={'\0'};
	MSGBUF msg;
	msg.msg_length=0;
	msg.lpBuffer=buffer;
	DWORD nBytesRead;
	long pipeline_no=0;
	while(1)
	{
		ConnectNamedPipe(CMainWorkLoop::m_hCorePipe,NULL);
		ReadFile(CMainWorkLoop::m_hCorePipe,(void*)&msg,9,&nBytesRead,NULL);
		if(msg.msg_length>0)
		{
			ReadFile(CMainWorkLoop::m_hCorePipe,buffer,msg.msg_length,&nBytesRead,NULL);
			buffer[nBytesRead]='\0';
		}
		if(msg.command_id==LOGIN_DOG)	//新用户请求注册
			CMainWorkLoop::Login(CMainWorkLoop::m_hCorePipe,msg);
		else if(msg.command_id==ASK_SERVER_VERSION)
			CMainWorkLoop::AskServerVersion(CMainWorkLoop::m_hCorePipe);
		
		DisconnectNamedPipe(CMainWorkLoop::m_hCorePipe);
		if(CMainWorkLoop::m_bQuitNow)
			break;
	}
	CloseHandle(CMainWorkLoop::m_hCorePipe);
	return 0;
}

void CMainWorkLoop::StartLoop()
{
	DWORD idThread;
	HANDLE hThread=CreateThread(NULL,0,&MainPipeInstanceProc,NULL,0,&idThread);
	//int prority=GetThreadPriority(hThread);
	//SetThreadPriority(hThread,THREAD_PRIORITY_NORMAL);
	CloseHandle(hThread);
}

void CMainWorkLoop::EndLoop()
{
	for(DOGSESSION *pSession=hashSessions.GetFirst();pSession;pSession=hashSessions.GetNext())
	{
		Logout(pSession);
		CloseHandle(pSession->m_hServerListenPipe);
		CloseHandle(pSession->m_hServerSendPipe);
	}
	hashSessions.Empty();
	m_bQuitNow=TRUE;
}
BOOL CMainWorkLoop::Login(HANDLE hCorePipe,MSGBUF &msg)
{
	DWORD nBytesWritten;
	if(msg.msg_length!=4)
		return FALSE;	//消息体长度不对
	BYTE cDogType=*msg.lpBuffer;
	BYTE cProductType=*(msg.lpBuffer+1);
	WORD wNetDogModule=*((WORD*)(msg.lpBuffer+2));
	DOGSESSION* pSession=NULL;
	if(GetCurrentNo()<=0)
		return FALSE;
	DWORD hDogHandle=LoginDog(cDogType,cProductType,wNetDogModule);
	pSession=hashSessions.Add(0);
	pSession->m_hNetDogSession=hDogHandle;
	pSession->cProductType=cProductType;
	pSession->wNetDogModule=wNetDogModule;
	if(pSession)
		pSession->cDogType=cDogType;
	else
		return FALSE;

	char PIPE_LISTEN[MAX_PATH],PIPE_SEND[MAX_PATH];
	sprintf(PIPE_LISTEN,"%s\\ID_%d_ListenPipe",PIPE_PATH,pSession->m_idSession);
	sprintf(PIPE_SEND,"%s\\ID_%d_SendPipe",PIPE_PATH,pSession->m_idSession);

	pSession->m_hServerListenPipe =CreateNamedPipe(PIPE_LISTEN,PIPE_ACCESS_DUPLEX,PIPE_TYPE_BYTE|PIPE_READMODE_BYTE,32,0,0,2000,&CMainWorkLoop::sa);
	pSession->m_hServerSendPipe	  =CreateNamedPipe(PIPE_SEND,PIPE_ACCESS_DUPLEX,PIPE_TYPE_BYTE|PIPE_READMODE_BYTE,32,0,0,2000,&CMainWorkLoop::sa);
	WriteFile(hCorePipe,&pSession->m_idSession,sizeof(long),&nBytesWritten,NULL);

	DWORD ThreadId;
	HANDLE ThreadHandle=CreateThread(NULL,0,SubPipeInstanceProc,pSession,0,&ThreadId);
	CloseHandle(ThreadHandle);
	return TRUE;
}

void CMainWorkLoop::Logout(DOGSESSION* pSession)
{
	bool bRetCode=LogoutDog(pSession);
	DWORD dwBytesWritten=0;
	WriteFile(m_hCorePipe,&bRetCode,sizeof(long),&dwBytesWritten,NULL);
}

//发送TMA版本号
BYTE version_verify[4]={0};
void CMainWorkLoop::VerifyVersion(DOGSESSION* pSession,MSGBUF &msg)
{
	char tma_version[4]={0};
	DWORD nBytesWritten;
	memcpy(tma_version,msg.lpBuffer,4);
	long ret = 0;
	//比较加密狗设置上限版本号与TMA版本号
	if(version_verify[0]==0&&version_verify[1]==0&&version_verify[2]==0&&version_verify[3]==0)
		ReadDogData(pSession,102,&version_verify,4);	
	for(int i=0;i<4;i++)
	{
		if(version_verify[i]>tma_version[i])
		{
			ret = 1;
			break;
		}
		else if(version_verify[i]<tma_version[i])
		{
			ret = -1;
			break;
		}
	}
	WriteFile(pSession->m_hServerSendPipe,&ret,sizeof(long),&nBytesWritten,NULL);
}
DWORD dwEncryptKey=0;
void CMainWorkLoop::AskEncryptKey(DOGSESSION* pSession,MSGBUF &msg)
{
	/*DWORD nBytesWritten,dogpipeline_no=0,result=0;
	DogData = &dogpipeline_no;
	DogBytes = 4;
	GetCurrentNo();
	DogConvert();
	if(NetDog(0,CMD_GETCURRNO,&m_hNetDogSession,&dogpipeline_no,NULL,NULL,4,DogPassword,0)!=NETDOG_SUCCESS)
		dogpipeline_no=0;
	NetDog(0,CMD_CONVERT,&m_hNetDogSession,&dogpipeline_no,&result,NULL,4,DogPassword,0);
	if(dwEncryptKey==0)
	{
		MD5_CTX auth;
		BYTE digest[16];
		auth.MD5Update((BYTE*)&DogResult,4);
		auth.MD5Final(digest);
		memcpy(&dwEncryptKey,digest,4);
	}
	WriteFile(hPipe,&dwEncryptKey,sizeof(DWORD),&nBytesWritten,NULL);*/
}
DWORD dwFalseKey=0;
void CMainWorkLoop::AskFalseKey(DOGSESSION* pSession,MSGBUF &msg)
{
	DWORD nBytesWritten;
	ReadDogData(pSession,120,&dwFalseKey,4);
	WriteFile(pSession->m_hServerSendPipe,&dwFalseKey,sizeof(DWORD),&nBytesWritten,NULL);
}
void CMainWorkLoop::AskDecryptKey(DOGSESSION* pSession,MSGBUF &msg)
{
	DWORD nBytesWritten,pipeline_no,dwPermission=0,dwDecryptKey=0;
	memcpy(&pipeline_no,msg.lpBuffer,sizeof(DWORD));
	/*long user_id;
	memcpy(&user_id,msg.lpBuffer,sizeof(long));
	for(XHUSER_STRU *pUser=userList.GetFirst();pUser;pUser=userList.GetNext())
	{
		if(pUser->m_nUserId==user_id)
		{
			dwPermission = pUser->dwPermFlag;
			break;
		}
	}
	DWORD dwDecryptKey=0;
	if(pUser)
	{
		if(pUser->dwPermFlag&PERM_DECRYPT_FILE)
		{*/
			//针对不同企业的加密狗生成密码
			//NetDog(0,CMD_CONVERT,&m_hNetDogSession,&pipeline_no,&dwDecryptKey,NULL,4,DogPassword,0);
			/*DogData = &pipeline_no;
			DogBytes = 4;
			DogConvert();
			dwDecryptKey=DogResult;*/
		/*}
	}*/
	WriteFile(pSession->m_hServerSendPipe,&dwDecryptKey,sizeof(DWORD),&nBytesWritten,NULL);
}
void CMainWorkLoop::AskDogPermState(DOGSESSION* pSession)
{
	/*DWORD dwPermState=0;
	DWORD nBytesWritten;
	BYTE* byteDogState=(BYTE*)&dwPermState;
	NetDog(0,CMD_READ,&m_hNetDogSession,&byteDogState[0],NULL,4,1,DogPassword,0);
	NetDog(0,CMD_WRITE,&m_hNetDogSession,&byteDogState[1],NULL,24,1,DogPassword,0);
	WriteFile(hPipe,&byteDogState[1],1,&nBytesWritten,NULL);
	WriteFile(hPipe,&byteDogState[0],1,&nBytesWritten,NULL);
	BYTE flag;
	DogData = &flag;
	DogAddr=24;
	DogBytes=1;
	ReadDog();
	WriteFile(hPipe,&flag,1,&nBytesWritten,NULL);
	DogData=&flag;
	DogAddr=4;
	DogBytes=1;
	ReadDog();
	WriteFile(hPipe,&flag,1,&nBytesWritten,NULL);*/
}
void CMainWorkLoop::AskExpireTimes(DOGSESSION* pSession)
{
	long expire_times=0;
	ReadDogData(pSession,20,&expire_times,4);
	DWORD nBytesWritten;
	WriteFile(pSession->m_hServerSendPipe,&expire_times,sizeof(long),&nBytesWritten,NULL);
}
void CMainWorkLoop::DecreaseExpireTimes(DOGSESSION* pSession)
{
	DWORD expire_times=0;
	ReadDogData(pSession,20,&expire_times,4);
	if(expire_times>0)
		expire_times--;
	WriteDogData(pSession,20,&expire_times,4);
}
void CMainWorkLoop::AskServerVersion(HANDLE hPipe)
{
	DWORD nBytesWritten;
	char version[4]={1,0,0,0};
	WriteFile(hPipe,&version,4,&nBytesWritten,NULL);
}

void CMainWorkLoop::AskDogSerialNo(DOGSESSION *pSession)
{
	UINT dog_serial=DogSerialId(pSession);
	DWORD nBytesWritten;
	WriteFile(pSession->m_hServerSendPipe,&dog_serial,sizeof(UINT),&nBytesWritten,NULL);
}
void CMainWorkLoop::AskDogEncrypt(DOGSESSION* pSession,MSGBUF &msg)
{
	if(msg.msg_length<=0)
		return;
	DYN_ARRAY<BYTE> encrypt_buf(msg.msg_length);
	DogCrypt(pSession,msg.lpBuffer,msg.msg_length,(BYTE*)encrypt_buf);
	DWORD nBytesWritten;
	WriteFile(pSession->m_hServerSendPipe,(BYTE*)encrypt_buf,msg.msg_length,&nBytesWritten,NULL);
}
void CMainWorkLoop::AskDogDecrypt(DOGSESSION* pSession,MSGBUF &msg)
{
	if(msg.msg_length<=0)
		return;
	DYN_ARRAY<BYTE> decrypt_buf(msg.msg_length);
	DogDecrypt(pSession,msg.lpBuffer,msg.msg_length,(BYTE*)decrypt_buf);
	DWORD nBytesWritten;
	WriteFile(pSession->m_hServerSendPipe,(BYTE*)decrypt_buf,msg.msg_length,&nBytesWritten,NULL);
}

void CMainWorkLoop::AskReadDogData(DOGSESSION* pSession,MSGBUF &msg)
{
	if(msg.msg_length!=6)
		return;
	short swAddr=*((short*)msg.lpBuffer);
	int data_len=*((int*)(msg.lpBuffer+2));
	if(data_len>0)
	{
		DYN_ARRAY<BYTE> dog_data(data_len);
		memset((BYTE*)dog_data,0,data_len);
		ReadDogData(pSession,swAddr,(BYTE*)dog_data,data_len);
		DWORD nBytesWritten;
		WriteFile(pSession->m_hServerSendPipe,(BYTE*)dog_data,data_len,&nBytesWritten,NULL);
	}
}
void CMainWorkLoop::AskWriteDogData(DOGSESSION* pSession,MSGBUF &msg)
{
	if(msg.msg_length!=6)
		return;
	short swAddr=*((short*)msg.lpBuffer);
	int data_len=*((int*)(msg.lpBuffer+2));
	if(data_len>0)
	{
		DYN_ARRAY<BYTE> dog_data(data_len);
		int len=WriteDogData(pSession,swAddr,(BYTE*)dog_data,data_len);
		DWORD nBytesWritten;
		WriteFile(pSession->m_hServerSendPipe,&len,4,&nBytesWritten,NULL);
	}
}