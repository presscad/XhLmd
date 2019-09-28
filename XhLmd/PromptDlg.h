#if !defined(AFX_PROMPTDLG_H__8EC78781_1547_11D5_8CF3_0000B498B2A9__INCLUDED_)
#define AFX_PROMPTDLG_H__8EC78781_1547_11D5_8CF3_0000B498B2A9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PromptDlg.h : header file
//
#include "resource.h"       // main symbols
#include "XhCharString.h"
#include "atom_list.h"

/////////////////////////////////////////////////////////////////////////////
// CPromptDlg dialog
class CImageRecord
{
private:
	CXhChar200 m_sSketchFile;	//简易图文件
	CXhChar200 m_sImageFile;	//详解图文件
	CXhChar50 m_sLabel;
public:
	CImageRecord();
	CImageRecord(char* sketch_file,char* image_file,char* sLabel);
	BOOL GetSketchBitMap(CBitmap& xBitMap);
	BOOL GetImageBitMap(CBitmap& pBitMap);
	CXhChar50 GetLabelStr();
};
class CPromptDlg : public CDialog
{
	DECLARE_DYNAMIC(CPromptDlg)

	bool m_bEnableWindowMoveListen;
	bool m_bFirstPopup;
	bool m_bRelayout;
	static int nScrLocationX,nScrLocationY;
	void (*drawpane_func)(void* pContext);			//渲染回调函数
protected:
	CBrush m_bBrush;
	CWnd* m_pWnd;
	bool m_bFromInternalResource;	//true:从EXE文件内部资源加载图片；false:从外部文件加载图片
	int  m_iType;			//0.显示文字 1.显示图片 2.显示图片列表
	long m_idBmpResource;
	CImageList imageList;
	CBitmap m_bBitMap;
	BOOL m_bSelBitMap;
	CXhChar200 bitmapfile;
	int m_dwIconWidth,m_dwIconHeight;
public:
	ATOM_LIST<CImageRecord> m_xImageRecList;
	CPromptDlg(CWnd* pWnd);   // standard constructor
	BOOL Create();
	BOOL Destroy();
	void SetMsg(CString msg);
	void SetQRCodeStr(const char* szSourceSring,const char* sTitle,const char* sPropmt);
	void SetPromptPicture(long bitmapId);
	void SetPromptPicture(const char* bmpfile);
	void SetPromptPictures();
	void SetUdfDrawFunc(void (*drawfunc)(void* pContext));		// 设置预置重显函数
	void RelayoutWnd();
// Dialog Data
	//{{AFX_DATA(CPromptDlg)
	enum { IDD = IDD_PROMPT_DLG };
	CString	m_sPromptMsg;
	//}}AFX_DATA
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPromptDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPromptDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMove(int x, int y);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnItemchangedListCtrl(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnCancel();
};

class CSysPromptObject{
	CPromptDlg *m_pPromptWnd;
	void AssureValid(CWnd *pParentWnd=NULL);
public:
	CSysPromptObject();
	~CSysPromptObject();
	void Close();
	void Prompt(const char* promptstr);
	void SetPromptPicture(long bitmapId);
	void SetPromptPicture(const char* bmpfile);
	void SetQRCodeStr(const char* szSourceSring,const char* sTitle,
					  const char* sPropmt,CWnd *pParentWnd=NULL);
	void AddImageRecord(CImageRecord& imageRec);
	void SetPromptPictures();
public:
	static BOOL m_bEnablePrompt;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROMPTDLG_H__8EC78781_1547_11D5_8CF3_0000B498B2A9__INCLUDED_)
