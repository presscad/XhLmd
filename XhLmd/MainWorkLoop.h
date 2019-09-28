// WorkLoop.h: interface for the CWorkLoop class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINWORKLOOP_H__316E35B1_D46B_4391_9677_E27400FA1D65__INCLUDED_)
#define AFX_MAINWORKLOOP_H__316E35B1_D46B_4391_9677_E27400FA1D65__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MsgDef.h"
#include "Hashtable.h"

class CMainWorkLoop  
{
public:
	static DWORD m_hNetDogSession;
	static SECURITY_ATTRIBUTES sa;
	static BOOL m_bQuitNow;
	static HANDLE m_hCorePipe;
	static CHashList<DOGSESSION> hashSessions;	//¼üÖµÎªDOGSESSION::m_idSession
public:
	static BOOL Login(HANDLE hPipe,MSGBUF &msg);
	static void Logout(DOGSESSION *pSession);
	static void AskDogSerialNo(DOGSESSION *pSession);
	static void AskServerVersion(HANDLE hPipe);
	static void VerifyVersion(DOGSESSION* pSession,MSGBUF &msg);
	static void AskEncryptKey(DOGSESSION* pSession,MSGBUF &msg);
	static void AskDecryptKey(DOGSESSION* pSession,MSGBUF &msg);
	static void AskFalseKey(DOGSESSION* pSession,MSGBUF &msg);
	static void AskDogEncrypt(DOGSESSION* pSession,MSGBUF &msg);
	static void AskDogDecrypt(DOGSESSION* pSession,MSGBUF &msg);
	static void AskReadDogData(DOGSESSION* pSession,MSGBUF &msg);
	static void AskWriteDogData(DOGSESSION* pSession,MSGBUF &msg);
	static void AskDogPermState(DOGSESSION* pSession);
	static void AskExpireTimes(DOGSESSION* pSession);
	static void DecreaseExpireTimes(DOGSESSION* pSession);
	void StartLoop();
	void EndLoop();
	CMainWorkLoop();
	virtual ~CMainWorkLoop();
};
extern CMainWorkLoop _MainLoop;

#endif // !defined(AFX_MAINWORKLOOP_H__316E35B1_D46B_4391_9677_E27400FA1D65__INCLUDED_)
