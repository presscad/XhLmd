#ifndef __DOG_MSG_DEF_H_
#define __DOG_MSG_DEF_H_

#define PACK_SIZE				1024			//��Ϣ����С

typedef struct MSGHEAD
{	//��Ϣͷ
	BYTE command_id;//�����ʶ
	WORD msg_length;//��Ϣ����
	UINT src_code;	//Դ�ڵ���(���ỰId)
	MSGHEAD(){command_id=0;msg_length=0;src_code=0;}
}XH_MSGHEADPTR;
typedef struct MSGBUF : public MSGHEAD
{
	BYTE *lpBuffer;			//��Ϣ��
	MSGBUF(){lpBuffer=NULL;}
}*XH_MSGBUFPTR;

typedef struct DOGSESSION
{
	static const BYTE MICRO_DOG	= 0;//΢��(������)
	static const BYTE NET_DOG	= 1;//���繷
	static const BYTE HASP_DOG	= 2;//HASP��
	BYTE cDogType;				//���ܹ�����0.������;1.���繷;2.HASP��

	BYTE cProductType;
	WORD wNetDogModule;
	UINT m_idSession;			//��ͻ��˵ĻỰID
	DWORD m_hNetDogSession;		//�����繷��ĻỰ���ʾ��
	HANDLE m_hServerListenPipe;	//�����������û�����Ĺܵ�ʵ�����
	HANDLE m_hServerSendPipe;	//���������û�������Ϣ�Ĺܵ�ʵ�����
	DOGSESSION(){memset(this,0,sizeof(DOGSESSION));}
	//char m_sUserName[51];		//�û�������
	//DWORD dwPermFlag;
}*DOGSESSION_PTR;

//���ڱ��ؼ��ܰ汾TMA���ʼ��ܹ�
enum XHDOGSERV_CMD{
	//Client-->Server
	LOGIN_DOG				=0x00000001,
	LOGOUT_DOG				=0x00000002,	//By CorePipe
	ASK_SERVER_VERSION		=0x00000003,
	VERIFY_VERSION			=0x00000004,	//By PipeServerListen
	ASK_ENCRYPT_KEY			=0x00000005,	//By PipeServerListen
	ASK_DECRYPT_KEY			=0x00000006,	//By PipeServerListen
	ASK_FALSE_KEY			=0x00000007,	//By PipeServerListen
	ASK_DOG_PERM_STATE		=0x00000008,
	ASK_EXPIRE_TIMES		=0x00000009,
	ASK_DOG_SERIAL_NO		=0x0000000A,
	DECREASE_EXPIRE_TIMES	=0x0000000B,
	MODIFY_USER_PASSWORD	=0x0000000C,
	ASK_DOG_ENCRYPT			=0x0000000D,	//By PipeServerListen
	ASK_DOG_DECRYPT			=0x0000000E,	//By PipeServerListen
	ASK_READ_DOG_DATA		=0x0000000F,
	ASK_WRITE_DOG_DATA		=0x00000010,
	//Server-->Client
	ASK_LIVE_STATUS			=0x10000001,
};

#endif