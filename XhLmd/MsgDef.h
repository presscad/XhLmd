#ifndef __DOG_MSG_DEF_H_
#define __DOG_MSG_DEF_H_

#define PACK_SIZE				1024			//信息包大小

typedef struct MSGHEAD
{	//消息头
	BYTE command_id;//命令标识
	WORD msg_length;//消息长度
	UINT src_code;	//源节点编号(即会话Id)
	MSGHEAD(){command_id=0;msg_length=0;src_code=0;}
}XH_MSGHEADPTR;
typedef struct MSGBUF : public MSGHEAD
{
	BYTE *lpBuffer;			//消息体
	MSGBUF(){lpBuffer=NULL;}
}*XH_MSGBUFPTR;

typedef struct DOGSESSION
{
	static const BYTE MICRO_DOG	= 0;//微狗(单机狗)
	static const BYTE NET_DOG	= 1;//网络狗
	static const BYTE HASP_DOG	= 2;//HASP狗
	BYTE cDogType;				//加密狗类型0.单机狗;1.网络狗;2.HASP狗

	BYTE cProductType;
	WORD wNetDogModule;
	UINT m_idSession;			//与客户端的会话ID
	DWORD m_hNetDogSession;		//与网络狗间的会话访问句柄
	HANDLE m_hServerListenPipe;	//服务器监听用户请求的管道实例句柄
	HANDLE m_hServerSendPipe;	//服务器向用户发送消息的管道实例句柄
	DOGSESSION(){memset(this,0,sizeof(DOGSESSION));}
	//char m_sUserName[51];		//用户的名称
	//DWORD dwPermFlag;
}*DOGSESSION_PTR;

//用于本地加密版本TMA访问加密狗
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