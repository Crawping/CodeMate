
// CodeMateDlg.cpp : ʵ���ļ�
// ������

#include "stdafx.h"
#include "CodeMate.h"
#include "CodeMateDlg.h"
#include "CodeEdit.h"
#include "AboutDlg.h"
#include "AddItemDlg.h"
#include "ChangeTypeDlg.h"
#include "FilePrepDlg.h"
#include "MoveToDlg.h"
#include "GoToDlg.h"
#include "ConfigDlg.h"
#include "IsValidFileName.h"

#pragma comment(lib, "Version.lib")
#pragma comment(lib, "UxTheme.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CCodeEdit* g_pCodeEdit = NULL;
// void LineDrawProc(HDC hDC, LPTSTR lpszText, int iLen, RECT rect, int iSelectionStart, int iSelectionEnd);
CString GetMyDirectory();

static UINT indicators[] =
{
	AFX_IDS_IDLEMESSAGE,
	IDS_INDICATOR_TIME,
	ID_INDICATOR_LANG,
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

#define SETTING_TMP_LEN_1	256
#define SETTING_TMP_LEN_2	1024

// CCodeMateDlg �Ի���

CCodeMateDlg::CCodeMateDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CCodeMateDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CCodeMateDlg::~CCodeMateDlg()
{
	m_sMyConfig.m_arrKeyWord.RemoveAll();
	m_sMyConfig.m_arrKeyWord.FreeExtra();
	m_sMyConfig.m_arrConstant.RemoveAll();
	m_sMyConfig.m_arrConstant.FreeExtra();
	m_arrLanString.RemoveAll();
	m_arrLanString.FreeExtra();
}

void CCodeMateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_wndTreeView);
	DDX_Control(pDX, IDC_BUTTON_SEARCH, m_btnSearch);
	DDX_Control(pDX, IDC_EDIT1, m_edtKeyWord);
	DDX_Control(pDX, IDC_STATIC_SEARCH, m_stcContent);
	DDX_Control(pDX, IDC_CHECK1, m_chkCase);
	DDX_Control(pDX, IDC_BUTTON_HIDE, m_btnHide);
	DDX_Control(pDX, IDC_COMBO1, m_cboType);
}

BEGIN_MESSAGE_MAP(CCodeMateDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_MOVE()
	ON_WM_SIZING()
	ON_WM_SIZE()
	ON_WM_MENUSELECT()
	ON_WM_SETCURSOR()
	ON_WM_INITMENUPOPUP()
	ON_WM_SYSCOMMAND()
	ON_WM_THEMECHANGED()
	// }}AFX_MSG_MAP
	ON_REGISTERED_MESSAGE(WM_FINDREPLACE, OnFindReplace)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXT, 0, 0xFFFF, OnToolTipText)
	ON_NOTIFY(NM_RCLICK, IDC_TREE1, OnNMRclickTree1)
	ON_NOTIFY(NM_SETFOCUS, IDC_TREE1, OnNMSetfocusTree1)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, OnTvnSelchangedTree1)
	ON_NOTIFY(TVN_ENDLABELEDIT, IDC_TREE1, OnTvnEndlabeleditTree1)
	ON_NOTIFY(TVN_ITEMEXPANDED, IDC_TREE1, OnTvnItemexpandedTree1)
	ON_NOTIFY(TVN_BEGINLABELEDIT, IDC_TREE1, OnTvnBeginlabeleditTree1)
	ON_NOTIFY(TVN_SELCHANGING, IDC_TREE1, OnTvnSelchangingTree1)
	ON_BN_CLICKED(IDC_BUTTON_HIDE, OnBnClickedButtonHide)
	ON_BN_CLICKED(IDC_BUTTON_SEARCH, OnBnClickedButtonSearch)
	ON_COMMAND(ID_APP_ABOUT, OnMainAbout)
	ON_COMMAND(ID_APP_EXIT, OnFileExit)
	ON_COMMAND(ID_FILE_ADDITEM, OnFileAdditem)
	ON_COMMAND(ID_FILE_ADDCLASS, OnFileAddclass)
	ON_COMMAND(ID_RENAME_ITEM, OnRenameItem)
	ON_COMMAND(ID_DELETE_ITEM, OnDeleteItem)
	ON_COMMAND(ID_CLEAR_CLASS, OnClearClass)
	ON_COMMAND(ID_FILE_MOVETO, OnFileMoveto)
	ON_COMMAND(ID_FILE_SEARCH, OnFileSearch)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_COMMAND(ID_EDIT_SELALL, OnEditSelall)
	ON_COMMAND(ID_EDIT_CLEAR_ALL, OnEditClearAll)
	ON_COMMAND(ID_EDIT_GOTO, OnEditGoto)
	ON_COMMAND(ID_EDIT_SHOWLIST, OnEditShowlist)
	ON_COMMAND(ID_EDIT_FIND, OnEditFind)
	ON_COMMAND(ID_EDIT_REPLACE, OnEditReplace)
	ON_COMMAND(ID_EDIT_FIND_NEXT, OnEditFindNext)
	ON_COMMAND(ID_FILE_RELOAD, OnFileReload)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	ON_COMMAND(ID_FILE_CLOSE, OnFileClose)
	ON_COMMAND(ID_EDIT_CHANGE_TYPE, OnEditChangeType)
	ON_COMMAND(ID_A_LINE, OnALine)
	ON_COMMAND(ID_DEL_A_LINE, OnDelALine)
	ON_COMMAND(ID_FILE_PREP, OnFilePrep)
	ON_COMMAND(ID_VIEW_TOOLBAR, OnViewToolbar)
	ON_COMMAND(ID_VIEW_STATUS_BAR, OnViewStatusBar)
	ON_COMMAND(ID_SET_OPTION, OnSetOption)
	ON_COMMAND(ID_LAN_CHNS, OnLanChns)
	ON_COMMAND(ID_LAN_CHNT, OnLanChnt)
	ON_COMMAND(ID_LAN_ENG, OnLanEng)
	ON_COMMAND(ID_HELP, OnHelp)
	ON_UPDATE_COMMAND_UI(ID_FILE_MOVETO, OnUpdateFileMoveTo)
	ON_UPDATE_COMMAND_UI(ID_DELETE_ITEM, OnUpdateFileDeleteItem)
	ON_UPDATE_COMMAND_UI(ID_RENAME_ITEM, OnUpdateFileRenameItem)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS, OnUpdateFileSaveAs)
	ON_UPDATE_COMMAND_UI(ID_FILE_CLOSE, OnUpdateFileClose)
	ON_UPDATE_COMMAND_UI(ID_FILE_PREP, OnUpdateFilePrep)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CLEAR, OnUpdateEditClear)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SELALL, OnUpdateEditSelall)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CLEAR_ALL, OnUpdateEditClearAll)
	ON_UPDATE_COMMAND_UI(ID_EDIT_FIND, OnUpdateEditFind)
	ON_UPDATE_COMMAND_UI(ID_EDIT_FIND_NEXT, OnUpdateEditFindNext)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REPLACE, OnUpdateEditReplace)
	ON_UPDATE_COMMAND_UI(ID_EDIT_GOTO, OnUpdateEditGoto)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SHOWLIST, OnUpdateEditShowlist)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CHANGE_TYPE, OnUpdateEditChangeType)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TOOLBAR, OnUpdateViewToolbar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_STATUS_BAR, OnUpdateViewStatusBar)
	ON_UPDATE_COMMAND_UI(ID_LAN_CHNS, OnUpdateLanChns)
	ON_UPDATE_COMMAND_UI(ID_LAN_CHNT, OnUpdateLanChnt)
	ON_UPDATE_COMMAND_UI(ID_LAN_ENG, OnUpdateLanEng)
	ON_COMMAND_RANGE(ID_SKIN, ID_SKIN + 50, OnSkinList)
END_MESSAGE_MAP()



// CCodeMateDlg ��Ϣ�������

CString GetMyDirectory()
{
	CString strDir; 
	unsigned long ulSize = GetCurrentDirectory(0, NULL);
	LPTSTR lpszPath = new TCHAR[ulSize];
	if(GetCurrentDirectory(ulSize, lpszPath)!=0)
	{
		strDir = lpszPath;
	}
	if (strDir.Right(1) != _T("\\"))
	{
		strDir += _T("\\");
	}
	delete [] lpszPath;
	return strDir;
}

void CSetting::SaveSettings(LPCTSTR lpszFileName)
{
	CString strSaveConfig;

	// ���洰������
	strSaveConfig.Format(_T("%d"), m_uiLanguage);
	WritePrivateProfileString(NAME_MAIN_FRAME, _T("Language"),
		strSaveConfig, lpszFileName);

	strSaveConfig.Format(_T("%d"), m_bMaximize);
	WritePrivateProfileString(NAME_MAIN_FRAME, _T("IsMaximzed"),
		strSaveConfig, lpszFileName);

	WritePrivateProfileString(NAME_MAIN_FRAME, _T("Skin"),
		m_strSkin, lpszFileName);

	WritePrivateProfileStruct(NAME_MAIN_FRAME, _T("WindowPosition"),
		m_recWinPos, sizeof(m_recWinPos), lpszFileName);

	strSaveConfig.Format(_T("%d"), m_lSplPos);
	WritePrivateProfileString(NAME_MAIN_FRAME, _T("SplitterPosition"),
		strSaveConfig, lpszFileName);

	strSaveConfig.Format(_T("%d"), m_bShowToolBar);
	WritePrivateProfileString(NAME_MAIN_FRAME, _T("ShowToolBar"), 
		strSaveConfig, lpszFileName);

	strSaveConfig.Format(_T("%d"), m_bShowStatusBar);
	WritePrivateProfileString(NAME_MAIN_FRAME, _T("ShowStatusBar"),
		strSaveConfig, lpszFileName);

	// ���湤���ļ���
	WritePrivateProfileString(NAME_USER_CONFIG, _T("WorkingDirectory"),
		m_strWorkPath, lpszFileName);

	// ����༭������
	strSaveConfig.Format(_T("%d"), m_iTabLen);
	WritePrivateProfileString(NAME_USER_CONFIG, _T("TabLength"),
		strSaveConfig, lpszFileName);

	strSaveConfig.Format(_T("%d"), m_bShowLineNum);
	WritePrivateProfileString(NAME_USER_CONFIG, _T("ShowLineNum"),
		strSaveConfig, lpszFileName);

	strSaveConfig.Format(_T("%d"), m_uiDefType);
	WritePrivateProfileString(NAME_USER_CONFIG, _T("DefaultType"),
		strSaveConfig, lpszFileName);

	strSaveConfig.Format(_T("%d"), m_uiDefEncode);
	WritePrivateProfileString(NAME_USER_CONFIG, _T("DefaultEncoding"),
		strSaveConfig, lpszFileName);

	WritePrivateProfileStruct(NAME_USER_CONFIG, _T("CustomColor"),
		&m_coColor, sizeof(m_coColor), lpszFileName);

	WritePrivateProfileString(NAME_USER_CONFIG, _T("FontName"),
		m_strFontName, lpszFileName);

	strSaveConfig.Format(_T("%d"), m_iSize);
	WritePrivateProfileString(NAME_USER_CONFIG, _T("FontSize"),
		strSaveConfig, lpszFileName);

	strSaveConfig.Format(_T("%d"), m_iHeight);
	WritePrivateProfileString(NAME_USER_CONFIG, _T("FontHeight"),
		strSaveConfig, lpszFileName);

	strSaveConfig.Format(_T("%d"), m_bBold);
	WritePrivateProfileString(NAME_USER_CONFIG, _T("FontBold"), 
		strSaveConfig, lpszFileName);

	strSaveConfig.Format(_T("%d"), m_bItalic);
	WritePrivateProfileString(NAME_USER_CONFIG, _T("FontItalic"),
		strSaveConfig, lpszFileName);

	strSaveConfig.Format(_T("%d"), m_iCharSet);
	WritePrivateProfileString(NAME_USER_CONFIG, _T("FontCharSet"),
		strSaveConfig, lpszFileName);

	WritePrivateProfileString(NAME_USER_CONFIG, _T("FindString"),
		strFindString, lpszFileName);

	strSaveConfig.Format(_T("%d"), m_bMatchCase);
	WritePrivateProfileString(NAME_USER_CONFIG, _T("MatchCase"),
		strSaveConfig, lpszFileName);

	strSaveConfig.Format(_T("%d"), m_bMatchWholeWord);
	WritePrivateProfileString(NAME_USER_CONFIG, _T("MatchWholeWord"),
		strSaveConfig, lpszFileName);

	strSaveConfig.Format(_T("%d"), m_bDirectionDown);
	WritePrivateProfileString(NAME_USER_CONFIG, _T("DirectionDown"),
		strSaveConfig, lpszFileName);

	// �����Զ���ؼ��ʺͳ�������
	WritePrivateProfileString(NAME_USER_CONFIG, _T("CppKeywords"),
		m_arrKeyWord[0], lpszFileName);
	WritePrivateProfileString(NAME_USER_CONFIG, _T("CKeywords"),
		m_arrKeyWord[1], lpszFileName);
	WritePrivateProfileString(NAME_USER_CONFIG, _T("CSharpKeywords"),
		m_arrKeyWord[2], lpszFileName);
	WritePrivateProfileString(NAME_USER_CONFIG, _T("HTMLKeywords"),
		m_arrKeyWord[3], lpszFileName);
	WritePrivateProfileString(NAME_USER_CONFIG, _T("JSKeywords"),
		m_arrKeyWord[4], lpszFileName);
	WritePrivateProfileString(NAME_USER_CONFIG, _T("VBSKeywords"),
		m_arrKeyWord[5], lpszFileName);
	WritePrivateProfileString(NAME_USER_CONFIG, _T("SQLKeywords"),
		m_arrKeyWord[6], lpszFileName);

	WritePrivateProfileString(NAME_USER_CONFIG, _T("CppConstants"),
		m_arrConstant[0], lpszFileName);
	WritePrivateProfileString(NAME_USER_CONFIG, _T("CConstants"),
		m_arrConstant[1], lpszFileName);
	WritePrivateProfileString(NAME_USER_CONFIG, _T("CSharpConstants"),
		m_arrConstant[2], lpszFileName);
	WritePrivateProfileString(NAME_USER_CONFIG, _T("HTMLConstants"),
		m_arrConstant[3], lpszFileName);
	WritePrivateProfileString(NAME_USER_CONFIG, _T("JSConstants"),
		m_arrConstant[4], lpszFileName);
	WritePrivateProfileString(NAME_USER_CONFIG, _T("VBSConstants"),
		m_arrConstant[5], lpszFileName);
	WritePrivateProfileString(NAME_USER_CONFIG, _T("SQLConstants"),
		m_arrConstant[6], lpszFileName);
}

void CSetting::LoadSettings(LPCTSTR lpszFileName)
{
	// ���ش�������
	m_uiLanguage = GetPrivateProfileInt(NAME_MAIN_FRAME, _T("Language"),
		0, lpszFileName);

	m_bMaximize = GetPrivateProfileInt(NAME_MAIN_FRAME, _T("IsMaximzed"),
		0, lpszFileName);

	BOOL bResult;
	bResult = GetPrivateProfileStruct(NAME_MAIN_FRAME, _T("WindowPosition"),
		m_recWinPos, sizeof(m_recWinPos), lpszFileName);
	if (bResult == FALSE)
	{
		m_recWinPos.top = 121;
		m_recWinPos.bottom = 617;
		m_recWinPos.left = 165;
		m_recWinPos.right = 860;
	}

	m_lSplPos = GetPrivateProfileInt(NAME_MAIN_FRAME, _T("SplitterPosition"),
		0, lpszFileName);

	m_bShowToolBar = GetPrivateProfileInt(NAME_MAIN_FRAME, _T("ShowToolBar"),
		1, lpszFileName);

	m_bShowStatusBar = GetPrivateProfileInt(NAME_MAIN_FRAME, _T("ShowStatusBar"),
		1, lpszFileName);

	// �����û�����
	m_uiDefType = GetPrivateProfileInt(NAME_USER_CONFIG, _T("DefaultType"),
		0, lpszFileName);

	m_uiDefEncode = GetPrivateProfileInt(NAME_USER_CONFIG, _T("DefaultEncoding"),
		FET_UNICODE, lpszFileName);

	bResult = GetPrivateProfileStruct(NAME_USER_CONFIG, _T("CustomColor"),
		&m_coColor, sizeof(m_coColor), lpszFileName);
	if (bResult == FALSE)
	{
		m_coColor.crBackgroundColor = RGB(255,255,255);
		m_coColor.crForegroundColor = RGB(0,0,0);
		m_coColor.crKeywordColor = RGB(0,0,255);
		m_coColor.crConstantColor = RGB(128,128,128);
		m_coColor.crCommentColor = RGB(0,128,0);
		m_coColor.crTextColor = RGB(128,0,0);
		m_coColor.crLineNumBGColor = RGB(128,128,128);
		m_coColor.crLineNumFGColor = RGB(255,255,255);
	}

	m_iTabLen = GetPrivateProfileInt(NAME_USER_CONFIG, _T("TabLength"),
		4, lpszFileName);

	m_bShowLineNum = GetPrivateProfileInt(NAME_USER_CONFIG, _T("ShowLineNum"),
		1, lpszFileName);

	m_iSize = GetPrivateProfileInt(NAME_USER_CONFIG, _T("FontSize"),
		11, lpszFileName);

	m_iHeight = GetPrivateProfileInt(NAME_USER_CONFIG, _T("FontHeight"),
		-15, lpszFileName);

	m_bBold = GetPrivateProfileInt(NAME_USER_CONFIG, _T("FontBold"),
		0, lpszFileName);

	m_bItalic = GetPrivateProfileInt(NAME_USER_CONFIG, _T("FontItalic"),
		0, lpszFileName);

	m_iCharSet = GetPrivateProfileInt(NAME_USER_CONFIG, _T("FontCharSet"),
		ANSI_CHARSET, lpszFileName);

	m_bMatchCase = GetPrivateProfileInt(NAME_USER_CONFIG, _T("MatchCase"),
		1, lpszFileName);

	m_bMatchWholeWord = GetPrivateProfileInt(NAME_USER_CONFIG, _T("MatchWholeWord"),
		0, lpszFileName);

	m_bDirectionDown = GetPrivateProfileInt(NAME_USER_CONFIG, _T("DirectionDown"),
		1, lpszFileName);

	LPTSTR lpszTemp = new _TCHAR[SETTING_TMP_LEN_1];
	GetPrivateProfileString(NAME_MAIN_FRAME, _T("Skin"),
		_T("AquaOS.ssk"), lpszTemp, SETTING_TMP_LEN_1, lpszFileName);
	m_strSkin = lpszTemp;

	GetPrivateProfileString(NAME_USER_CONFIG, _T("FontName"),
		_T("Courier New"), lpszTemp, SETTING_TMP_LEN_1, lpszFileName);
	m_strFontName = lpszTemp;

	GetPrivateProfileString(NAME_USER_CONFIG, _T("FindString"),
		_T(""), lpszTemp, SETTING_TMP_LEN_1, lpszFileName);
	strFindString = lpszTemp;

	GetPrivateProfileString(NAME_USER_CONFIG, _T("WorkingDirectory"),
		_T(".\\"), lpszTemp, SETTING_TMP_LEN_1, lpszFileName);
	m_strWorkPath = lpszTemp;

	delete []lpszTemp;

	// �����Զ���ؼ��ʺͳ�������
	LPTSTR lpszWords = new _TCHAR[SETTING_TMP_LEN_2];
	GetPrivateProfileString(NAME_USER_CONFIG, _T("CppKeywords"),
		_T(""), lpszWords, SETTING_TMP_LEN_2, lpszFileName);
	m_arrKeyWord.Add(CString(lpszWords));

	GetPrivateProfileString(NAME_USER_CONFIG, _T("CKeywords"),
		_T(""), lpszWords, SETTING_TMP_LEN_2, lpszFileName);
	m_arrKeyWord.Add(CString(lpszWords));

	GetPrivateProfileString(NAME_USER_CONFIG, _T("CSharpKeywords"),
		_T(""), lpszWords, SETTING_TMP_LEN_2, lpszFileName);
	m_arrKeyWord.Add(CString(lpszWords));

	GetPrivateProfileString(NAME_USER_CONFIG, _T("HTMLKeywords"),
		_T(""), lpszWords, SETTING_TMP_LEN_2, lpszFileName);
	m_arrKeyWord.Add(CString(lpszWords));

	GetPrivateProfileString(NAME_USER_CONFIG, _T("JSKeywords"),
		_T(""), lpszWords, SETTING_TMP_LEN_2, lpszFileName);
	m_arrKeyWord.Add(CString(lpszWords));

	GetPrivateProfileString(NAME_USER_CONFIG, _T("VBSKeywords"),
		_T(""), lpszWords, SETTING_TMP_LEN_2, lpszFileName);
	m_arrKeyWord.Add(CString(lpszWords));

	GetPrivateProfileString(NAME_USER_CONFIG, _T("SQLKeywords"),
		_T(""), lpszWords, SETTING_TMP_LEN_2, lpszFileName);
	m_arrKeyWord.Add(CString(lpszWords));

	GetPrivateProfileString(NAME_USER_CONFIG, _T("CppConstants"),
		_T(""), lpszWords, SETTING_TMP_LEN_2, lpszFileName);
	m_arrConstant.Add(CString(lpszWords));

	GetPrivateProfileString(NAME_USER_CONFIG, _T("CConstants"),
		_T(""), lpszWords, SETTING_TMP_LEN_2, lpszFileName);
	m_arrConstant.Add(CString(lpszWords));

	GetPrivateProfileString(NAME_USER_CONFIG, _T("CSharpConstants"),
		_T(""), lpszWords, SETTING_TMP_LEN_2, lpszFileName);
	m_arrConstant.Add(CString(lpszWords));

	GetPrivateProfileString(NAME_USER_CONFIG, _T("HTMLConstants"),
		_T(""), lpszWords, SETTING_TMP_LEN_2, lpszFileName);
	m_arrConstant.Add(CString(lpszWords));

	GetPrivateProfileString(NAME_USER_CONFIG, _T("JSConstants"),
		_T(""), lpszWords, SETTING_TMP_LEN_2, lpszFileName);
	m_arrConstant.Add(CString(lpszWords));

	GetPrivateProfileString(NAME_USER_CONFIG, _T("VBSConstants"),
		_T(""), lpszWords, SETTING_TMP_LEN_2, lpszFileName);
	m_arrConstant.Add(CString(lpszWords));

	GetPrivateProfileString(NAME_USER_CONFIG, _T("SQLConstants"),
		_T(""), lpszWords, SETTING_TMP_LEN_2, lpszFileName);
	m_arrConstant.Add(CString(lpszWords));

	delete []lpszWords;
}

BOOL CCodeMateDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	// ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// ��ʼ����־����
	m_bFocused = false;
	m_bItemSelected = false;
	m_bTextSelected = false;
	m_bIsFolder = true;
	m_bMoving = false;
	m_bShowSearch = false;
	m_point_x = 0;
	m_enFileType = SCT_ERROR;
	m_usOperaType = 0;
	// ��ʼ��ָ��
	m_pFindDlg = NULL;
	m_hRoot = NULL;
	m_strAppPath = GetMyDirectory();
	// ���������ļ�
	m_sMyConfig.LoadSettings(CONFIG_FILE_NAME);
	if (!m_bSkinLoad)
		m_sMyConfig.m_strSkin = _T("<ϵͳ����>");
	BOOL bWinMax = m_sMyConfig.m_bMaximize;	// �����������״̬
	if (!SetCurrentDirectory(m_sMyConfig.m_strWorkPath))
	{
		AfxMessageBox(_T("���󣺴洢�ļ�����Ч��\n�ָ�Ĭ�ϴ洢�ļ��С�"), MB_ICONERROR);
		m_sMyConfig.m_strWorkPath = _T(".\\");
	}
	RECT rcClient;
	GetClientRect(&rcClient);
	rcClient.left = rcClient.left + 150 + m_sMyConfig.m_lSplPos;	// �༭��λ��
	rcClient.bottom = rcClient.bottom - 18;	// ���ͻ����߶�
	// ����Ϊͼ���б��ʼ��
	m_wndImageList.Create(16, 16, ILC_COLOR24|ILC_MASK, 4, 4);		// ����ͼ���б�
	CBitmap bm;
	for (UINT nId = IDB_BITMAP1; nId <= IDB_BITMAP18; nId++)
		AddBitmap(bm, nId);
	bm.Detach();

	m_wndTreeView.SetImageList(&m_wndImageList, TVSIL_NORMAL); // Ϊ���οؼ����ͼ���б�
	m_wndTreeView.SetBkColor(m_sMyConfig.m_coColor.crBackgroundColor);
	m_wndTreeView.SetTextColor(m_sMyConfig.m_coColor.crForegroundColor);

	// �з�����ʼ��
	m_wndSplitter.LoadBitmaps(IDB_SPLITTER);  // ��ť��ʾ������λͼ��Դ
	m_wndSplitter.SubclassDlgItem(IDC_BUTTON_SPLITTER, this);
	m_wndSplitter.ShowWindow(SW_HIDE);

	// ����༭���ؼ�
	if (!m_wndCodeEdit.Create(WS_EX_CLIENTEDGE, WS_CHILD | WS_CLIPSIBLINGS,
		CRect(0,0,10,10), this, IDC_EDITOR))
	{
		AfxMessageBox(_T("��������δ�ܴ����༭����"), MB_ICONERROR );
		OnCancel();
	}
	// ����༭����������
	m_wndCodeEdit.SetSyntaxColoring(SCT_NONE);
	m_wndCodeEdit.SetDefaultFileEncoding(FILE_ENCODING_TYPE(m_sMyConfig.m_uiDefEncode));
	m_wndCodeEdit.SetTABs(m_sMyConfig.m_iTabLen);
	m_wndCodeEdit.SetShowLineNumberStatus(m_sMyConfig.m_bShowLineNum);
	m_wndCodeEdit.SetColorStruct(m_sMyConfig.m_coColor);
	m_wndCodeEdit.SetEditorFont((LPTSTR)(LPCTSTR)(m_sMyConfig.m_strFontName),
		m_sMyConfig.m_iHeight, m_sMyConfig.m_bBold,
		m_sMyConfig.m_bItalic, m_sMyConfig.m_iCharSet);
	// m_wndCodeEdit.SetLineDrawProc(LineDrawProc);	// �Զ����ʽ
	m_wndCodeEdit.ShowWindow(SW_HIDE);
	g_pCodeEdit = &m_wndCodeEdit;

	// ���빤����
	if (!m_wndToolBar.Create( this ) ||	!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		AfxMessageBox(_T("��������δ�ܴ�����������"), MB_ICONERROR );
	}
	m_wndToolBar.LoadTrueColorToolBar(16, IDR_MAINFRAME, IDR_MAINFRAME, IDB_TOOLBAR_DISABLE);
	// ��ʾ��������ʾ
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY );
	// ���ù�����
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);
	// ����״̬��
	if (!m_wndStatusBar.Create(this) || !m_wndStatusBar.SetIndicators(indicators,sizeof(indicators)/sizeof(UINT)))
	{
		AfxMessageBox(_T("��������δ�ܴ���״̬����"), MB_ICONERROR);
	}
	m_wndStatusBar.SetPaneInfo(1, IDS_INDICATOR_TIME, SBPS_STRETCH, 200);
	m_wndStatusBar.SetPaneInfo(2, ID_INDICATOR_LANG, SBPS_NORMAL, 100);
	// ����״̬��
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, IDS_INDICATOR_TIME);
	// ���ý�������
	CString strLoad;
	m_chkCase.SetCheck(m_sMyConfig.m_bMatchCase);
	strLoad.LoadString(IDS_SEARCH_CONTENT);
	m_stcContent.SetWindowText(strLoad);
	strLoad.LoadString(IDS_SEARCH_BUTTON);
	m_btnSearch.SetWindowText(strLoad);
	strLoad.LoadString(IDS_SEARCH_CHECK);
	m_chkCase.SetWindowText(strLoad);
	strLoad.LoadString(IDS_SEARCH_HIDE);
	m_btnHide.SetWindowText(strLoad);
	if (m_sMyConfig.m_uiLanguage != 0)
	{
		CString strPath;
		if (m_sMyConfig.m_uiLanguage == 1)
		{
			strPath = m_strAppPath + _T("TChinese.lng");
			if (!SetMainLanguage(m_sMyConfig.m_uiLanguage, strPath))
				m_sMyConfig.m_uiLanguage = 0;
		}
		else if (m_sMyConfig.m_uiLanguage == 2)
		{
			strPath = m_strAppPath + _T("English.lng");
			if (!SetMainLanguage(m_sMyConfig.m_uiLanguage, strPath))
				m_sMyConfig.m_uiLanguage = 0;
		}
	}
	else	//Ĭ������
	{
		if (m_sMyConfig.m_uiLanguage == 0)
			strLoad.LoadString(IDS_INDICATOR_FILE);
		else
			strLoad = m_arrLanString[54];
		m_wndStatusBar.SetPaneText(2, strLoad);
		strLoad.LoadString(IDS_COMBO_ALL);
		m_cboType.InsertString(0, strLoad);
		strLoad.LoadString(IDS_COMBO_CLASS);
		m_cboType.InsertString(1, strLoad);
		m_cboType.InsertString(2, _T("C++"));
		m_cboType.InsertString(3, _T("ANSI C"));
		m_cboType.InsertString(4, _T("C#.NET"));
		m_cboType.InsertString(5, _T("HTML"));
		m_cboType.InsertString(6, _T("Java Script"));
		m_cboType.InsertString(7, _T("VB Script"));
		m_cboType.InsertString(8, _T("SQL"));
		strLoad.LoadString(IDS_NORMAL_TEXT);
		m_cboType.InsertString(9, strLoad);
		m_cboType.SetCurSel(0);
	}
	// ��ȡϵͳ�汾
	OSVERSIONINFOEX OS;
	OS.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	if (!GetVersionEx((OSVERSIONINFO*)&OS))
		AfxMessageBox(_T("���󣺻�ȡ����ϵͳ�汾ʧ�ܡ�"), MB_ICONERROR);

	m_bShowMenuImage = false;
	LoadSkinMenu();
	// ����˵�ͼ�꣨Vista���ϰ汾,����ϵͳ���⣩
	if (OS.dwMajorVersion >= 6 && IsThemeActive())
	{
		// ��ʾ�˵�ͼ��
		m_bShowMenuImage = true;
		SetMenuImage(true, false);
	}

	// ��ʾĬ�ϱ�����
	m_strAppTitle.LoadString(AFX_IDS_APP_TITLE);
	SetWindowText(m_strAppTitle);
	// Ӧ�����ã���ʾ�����ع�������״̬��
	int iShowStatus = (m_sMyConfig.m_bShowToolBar ? SW_SHOW : SW_HIDE);
	m_wndToolBar.ShowWindow(iShowStatus);
	iShowStatus = (m_sMyConfig.m_bShowStatusBar ? SW_SHOW : SW_HIDE);
	m_wndStatusBar.ShowWindow(iShowStatus);
	// �������մ����С
	MoveWindow(m_sMyConfig.m_recWinPos.left, m_sMyConfig.m_recWinPos.top,
		m_sMyConfig.m_recWinPos.Width(), m_sMyConfig.m_recWinPos.Height());
	if (bWinMax == TRUE)
	{
		ShowWindow(SW_SHOWMAXIMIZED);
		m_sMyConfig.m_bMaximize = TRUE;
	}
	// ��������
	if (m_sMyConfig.m_uiLanguage == 0)
		strLoad.LoadString(IDS_CLASS_NORMAL);
	else
		strLoad = m_arrLanString[64];
	m_hRoot = m_wndTreeView.InsertItem(strLoad, 3, 3, TVI_ROOT);// ��Ӹ����
	CString strFolderPath = _T(".\\Code");
	// �ж�·���Ƿ����   
	if  ( !PathIsDirectory(strFolderPath))
	{
		if ( CreateDirectory(strFolderPath, NULL))
		{
			if (m_sMyConfig.m_uiLanguage == 0)
				strLoad.LoadString(IDS_ITEM_SHOW);
			else
				strLoad = m_arrLanString[65];
			strFolderPath = strFolderPath + _T("\\") + strLoad;
			CreateDirectory(strFolderPath, NULL);
		}
		else
			AfxMessageBox(_T("��������δ�ܴ������ࡣ"), MB_ICONERROR);
	}
	GetMainClass(m_hRoot, _T(".\\Code\\*.*"));
	GetClassDir(m_hRoot);
	m_wndTreeView.Expand(m_hRoot, TVE_EXPAND);	// ֻչ�����ڵ�
	m_wndTreeView.SelectItem(m_hRoot);
	// �����ݼ�
	hAcc = LoadAccelerators(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDR_MAINFRAME));

	return TRUE;  // ���������˿ؼ��Ľ��㣬���򷵻� TRUE
}

void CCodeMateDlg::AddBitmap(CBitmap &bm, UINT uiBmpId)
{
	bm.LoadBitmap(uiBmpId);									// ����Bitmap��Դ
	m_wndImageList.Add(&bm, NORMAL_BG_COLOR);				// ��ӵ�ͼ���б�
	bm.DeleteObject();
}

BOOL CCodeMateDlg::ContinueModal()
{
	m_wndToolBar.SendMessage( WM_IDLEUPDATECMDUI, WPARAM(TRUE), 0);	// Ϊ����������UPDATECMDUI��Ϣ
	return CDialogEx::ContinueModal();
}

BOOL CCodeMateDlg::SetMainLanguage(UINT uiLanguage, LPCTSTR lpszFileName)
{
	// �ж���������
	int iLang = 0;
	CMenu mnuMain;
	if (uiLanguage != 0)
	{
		switch(uiLanguage)
		{
		case 1:
			iLang = GetPrivateProfileInt(LANG_APP_TITLE, _T("Language"),
				0, lpszFileName);
			if (iLang != 1)
			{
				AfxMessageBox(_T("�oЧ���Z�ԙn����Ո�_���n�����ځK�������_�ġ�"));
				return FALSE;
			}
			break;
		case 2:
			iLang = GetPrivateProfileInt(LANG_APP_TITLE, _T("Language"),
				0, lpszFileName);
			if (iLang != 2)
			{
				AfxMessageBox(_T("The language file is invalid! Please ensure that it exists and is a right one."));
				return FALSE;
			}
			break;
		default:
			AfxMessageBox(_T("�������Բ�������\n���ָ�Ϊ�������ģ�Ĭ�ϣ���"));
			m_sMyConfig.m_uiLanguage = 0;
			return FALSE;
		}

		// ����Ƥ��
		if (m_bSkinLoad)
		{
			if (!skinppRemoveSkin())
				return FALSE;
		}

		ShowInterfaceLanguage(lpszFileName);

		if (m_bSkinLoad)
		{
			// �ָ�Ƥ��
			CString PathName = _T("Skins\\") + m_sMyConfig.m_strSkin;
			char *pText = NULL;
#ifdef _UNICODE
			int n = PathName.GetLength();
			int len = WideCharToMultiByte(CP_ACP,0,PathName,n,NULL,0,NULL,NULL);
			pText = new char[len+1];
			WideCharToMultiByte(CP_ACP,0,PathName,n,pText,len,NULL,NULL);
			pText[len] = '\0';
#else
			int len = PathName.GetLength();
			pText = new char[len+1];
			strcpy_s(pText, len+1, PathName.GetBuffer(0));
			pText[len] = '\0';
#endif
			if (skinppLoadSkin(pText) == FALSE) return FALSE;
			Invalidate();
			delete []pText;
		}
	}
	else	// �����Ĭ������
	{
		// ����Ƥ��
		if (m_bSkinLoad)
		{
			if (!skinppRemoveSkin())
				return FALSE;
		}

		// ����Ĭ�ϲ˵�
		mnuMain.LoadMenu(IDR_MAINFRAME);
		SetMenu(&mnuMain);
		LoadSkinMenu();
		if (m_bShowMenuImage)
			SetMenuImage(false, false);
		mnuMain.Detach();
		CString strWinText;
		strWinText.LoadString(IDS_SEARCH_CONTENT);
		m_stcContent.SetWindowText(strWinText);
		strWinText.LoadString(IDS_SEARCH_BUTTON);
		m_btnSearch.SetWindowText(strWinText);
		strWinText.LoadString(IDS_SEARCH_CHECK);
		m_chkCase.SetWindowText(strWinText);
		strWinText.LoadString(IDS_SEARCH_HIDE);
		m_btnHide.SetWindowText(strWinText);
		int iSel = m_cboType.GetCurSel();
		m_cboType.ResetContent();
		strWinText.LoadString(IDS_COMBO_ALL);
		m_cboType.InsertString(0, strWinText);
		strWinText.LoadString(IDS_COMBO_CLASS);
		m_cboType.InsertString(1, strWinText);
		m_cboType.InsertString(2, _T("C++"));
		m_cboType.InsertString(3, _T("ANSI C"));
		m_cboType.InsertString(4, _T("C#.NET"));
		m_cboType.InsertString(5, _T("HTML"));
		m_cboType.InsertString(6, _T("Java Script"));
		m_cboType.InsertString(7, _T("VB Script"));
		m_cboType.InsertString(8, _T("SQL"));
		strWinText.LoadString(IDS_NORMAL_TEXT);
		m_cboType.InsertString(9, strWinText);
		m_cboType.SetCurSel(iSel);

		if (m_bSkinLoad)
		{
			// �ָ�Ƥ��
			CString strPathName = _T("Skins\\") + m_sMyConfig.m_strSkin;
			char *pText = NULL;
#ifdef _UNICODE
			int n = strPathName.GetLength();
			int len = WideCharToMultiByte(CP_ACP,0,strPathName,n,NULL,0,NULL,NULL);
			pText = new char[len+1];
			WideCharToMultiByte(CP_ACP,0,strPathName,n,pText,len,NULL,NULL);
			pText[len] = '\0';
#else
			int len = PathName.GetLength();
			pText = new char[len+1];
			strcpy_s(pText, len+1, PathName.GetBuffer(0));
			pText[len] = '\0';
#endif
			if (skinppLoadSkin(pText) == FALSE)
				return FALSE;
			Invalidate();
			delete []pText;
		}
	}

	// ����״̬����Ϣ
	CString strLoad, strShow;
	if (m_enFileType != SCT_ERROR)
	{
		CString strPath = ItemToPath(m_hSelected);// ��ȡ��ǰ�����ȫ·��
		strPath = strPath.Mid(7);
		CString strConv;
		if (uiLanguage == 0)
			strConv.LoadString(IDS_CLASS_NORMAL);
		else
			strConv = m_arrLanString[64];
		strPath = strConv + _T("\\") + strPath;
		m_strTitleShow = strPath;
		CString strLang;
		switch (m_enFileType)	// �ж��ļ�����
		{
		case SCT_C_PLUS_PLUS:	// C++
			{
				strLang = _T("C++");
			}
			break;
		case SCT_C_ANSI:	// C
			{
				strLang = _T("C");
			}
			break;
		case SCT_C_SHARP:	// C#
			{
				strLang = _T("C#");
			}
			break;
		case SCT_HTML:	// HTM
			{
				strLang = _T("HTML");
			}
			break;
		case SCT_JSCRIPT:	// JScript
			{
				strLang = _T("JS");
			}
			break;
		case SCT_VBSCRIPT:	// VBScript
			{
				strLang = _T("VBS");
			}
			break;
		case SCT_SQL:	// SQL
			{
				strLang = _T("SQL");
			}
			break;
		case SCT_NONE:	// TXT
			{
				strLang = _T("TXT");
			}
			break;
		}
		if (uiLanguage == 0)
			strLoad.LoadString(ID_INDICATOR_LANG);
		else
			strLoad = m_arrLanString[58];
		strShow.Format(strLoad, strLang);
		m_wndStatusBar.SetPaneText(2, strShow);
	}
	else
	{
		if (uiLanguage == 0)
			strLoad.LoadString(IDS_INDICATOR_FILE);
		else
			strLoad = m_arrLanString[54];
		m_wndStatusBar.SetPaneText(2, strLoad);
	}
	// ���ø��ڵ��ı�
	if (uiLanguage == 0)
		strLoad.LoadString(IDS_CLASS_NORMAL);
	else
		strLoad = m_arrLanString[64];
	if (m_hRoot != NULL)
		m_wndTreeView.SetItemText(m_hRoot, strLoad);

	return TRUE;
}

void CCodeMateDlg::ShowInterfaceLanguage(LPCTSTR lpszFileName)
{
	//����ַ���
	m_arrLanString.RemoveAll();

	LPTSTR lpszLang = new _TCHAR[LANG_STRING_LEN];

	// ��������
	GetPrivateProfileString(LANG_SEARCH_AREA, _T("STATIC_CONTENT"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	m_stcContent.SetWindowText(lpszLang);

	GetPrivateProfileString(LANG_SEARCH_AREA, _T("BUTTON_SEARCH"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	m_btnSearch.SetWindowText(lpszLang);

	GetPrivateProfileString(LANG_SEARCH_AREA, _T("CHECK_MATCH_CASE"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	m_chkCase.SetWindowText(lpszLang);

	GetPrivateProfileString(LANG_SEARCH_AREA, _T("BUTTON_HIDE"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	m_btnHide.SetWindowText(lpszLang);

	int iSel = m_cboType.GetCurSel();
	if (iSel == CB_ERR)
		iSel = 0;
	m_cboType.ResetContent();

	GetPrivateProfileString(LANG_STRING, _T("ALL_TYPES"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	m_cboType.InsertString(0, lpszLang);

	GetPrivateProfileString(LANG_STRING, _T("CLASSES"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	m_cboType.InsertString(1, lpszLang);

	m_cboType.InsertString(2, _T("C++"));
	m_cboType.InsertString(3, _T("ANSI C"));
	m_cboType.InsertString(4, _T("C#.NET"));
	m_cboType.InsertString(5, _T("HTML"));
	m_cboType.InsertString(6, _T("Java Script"));
	m_cboType.InsertString(7, _T("VB Script"));
	m_cboType.InsertString(8, _T("SQL"));

	GetPrivateProfileString(LANG_STRING, _T("NORMAL_TEXT"), 
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	m_cboType.InsertString(9, lpszLang);

	m_cboType.SetCurSel(iSel);

	// �ļ��˵���ʾ�ı�
	GetPrivateProfileString(LANG_FILE_MENU, _T("TEXT_FILE_ADD_ITEM"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	m_arrLanString.Add(CString(lpszLang));

	GetPrivateProfileString(LANG_FILE_MENU, _T("TEXT_FILE_CREATE_CLASS"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	m_arrLanString.Add(CString(lpszLang));

	GetPrivateProfileString(LANG_FILE_MENU, _T("TEXT_FILE_SAVE"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	m_arrLanString.Add(CString(lpszLang));

	GetPrivateProfileString(LANG_FILE_MENU, _T("TEXT_FILE_EXPORT"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	m_arrLanString.Add(CString(lpszLang));

	GetPrivateProfileString(LANG_FILE_MENU, _T("TEXT_FILE_CLOSE"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	m_arrLanString.Add(CString(lpszLang));

	GetPrivateProfileString(LANG_FILE_MENU, _T("TEXT_FILE_MOVE"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	m_arrLanString.Add(CString(lpszLang));

	GetPrivateProfileString(LANG_FILE_MENU, _T("TEXT_FILE_DELETE_ITEM"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	m_arrLanString.Add(CString(lpszLang));

	GetPrivateProfileString(LANG_FILE_MENU, _T("TEXT_FILE_RENAME_ITEM"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	m_arrLanString.Add(CString(lpszLang));

	GetPrivateProfileString(LANG_FILE_MENU, _T("TEXT_FILE_ATTRIBUTE"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	m_arrLanString.Add(CString(lpszLang));

	GetPrivateProfileString(LANG_FILE_MENU, _T("TEXT_FILE_REFRESH"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	m_arrLanString.Add(CString(lpszLang));

	GetPrivateProfileString(LANG_FILE_MENU, _T("TEXT_FILE_CLEAR_ALL"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	m_arrLanString.Add(CString(lpszLang));

	GetPrivateProfileString(LANG_FILE_MENU, _T("TEXT_FILE_EXIT"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	m_arrLanString.Add(CString(lpszLang));

	GetPrivateProfileString(LANG_FILE_MENU, _T("MENU_FILE_DELETE_ITEM"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	m_arrLanString.Add(CString(lpszLang));

	GetPrivateProfileString(LANG_FILE_MENU, _T("MENU_FILE_DELETE_CLASS"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	m_arrLanString.Add(CString(lpszLang));

	GetPrivateProfileString(LANG_FILE_MENU, _T("MENU_FILE_RENAME_ITEM"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	m_arrLanString.Add(CString(lpszLang));

	GetPrivateProfileString(LANG_FILE_MENU, _T("MENU_FILE_RENAME_CLASS"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	m_arrLanString.Add(CString(lpszLang));

	// �ļ��˵�
	GetPrivateProfileString(LANG_FILE_MENU, _T("MENU_FILE"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	this->GetMenu()->ModifyMenu(0, MF_BYPOSITION, 0, lpszLang);

	GetPrivateProfileString(LANG_FILE_MENU, _T("MENU_FILE_ADD_ITEM"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	this->GetMenu()->GetSubMenu(0)->ModifyMenu(ID_FILE_ADDITEM,
		MF_BYCOMMAND, ID_FILE_ADDITEM, lpszLang);
	m_arrLanString.Add(CString(lpszLang));

	GetPrivateProfileString(LANG_FILE_MENU, _T("MENU_FILE_CREATE_CLASS"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	this->GetMenu()->GetSubMenu(0)->ModifyMenu(ID_FILE_ADDCLASS,
		MF_BYCOMMAND, ID_FILE_ADDCLASS, lpszLang);
	m_arrLanString.Add(CString(lpszLang));

	GetPrivateProfileString(LANG_FILE_MENU, _T("MENU_FILE_SAVE"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	this->GetMenu()->GetSubMenu(0)->ModifyMenu(ID_FILE_SAVE,
		MF_BYCOMMAND, ID_FILE_SAVE, lpszLang);

	GetPrivateProfileString(LANG_FILE_MENU, _T("MENU_FILE_EXPORT"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	this->GetMenu()->GetSubMenu(0)->ModifyMenu(ID_FILE_SAVE_AS,
		MF_BYCOMMAND, ID_FILE_SAVE_AS, lpszLang);

	GetPrivateProfileString(LANG_FILE_MENU, _T("MENU_FILE_CLOSE"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	this->GetMenu()->GetSubMenu(0)->ModifyMenu(ID_FILE_CLOSE,
		MF_BYCOMMAND, ID_FILE_CLOSE, lpszLang);

	GetPrivateProfileString(LANG_FILE_MENU, _T("MENU_FILE_MOVE"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	this->GetMenu()->GetSubMenu(0)->ModifyMenu(ID_FILE_MOVETO,
		MF_BYCOMMAND, ID_FILE_MOVETO, lpszLang);
	m_arrLanString.Add(CString(lpszLang));

	GetPrivateProfileString(LANG_FILE_MENU, _T("MENU_FILE_ATTRIBUTE"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	this->GetMenu()->GetSubMenu(0)->ModifyMenu(ID_FILE_PREP,
		MF_BYCOMMAND, ID_FILE_PREP, lpszLang);
	m_arrLanString.Add(CString(lpszLang));

	GetPrivateProfileString(LANG_FILE_MENU, _T("MENU_FILE_REFRESH"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	this->GetMenu()->GetSubMenu(0)->ModifyMenu(ID_FILE_RELOAD,
		MF_BYCOMMAND, ID_FILE_RELOAD, lpszLang);

	GetPrivateProfileString(LANG_FILE_MENU, _T("MENU_FILE_CLEAR_ALL"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	this->GetMenu()->GetSubMenu(0)->ModifyMenu(ID_CLEAR_CLASS,
		MF_BYCOMMAND, ID_CLEAR_CLASS, lpszLang);

	GetPrivateProfileString(LANG_FILE_MENU, _T("MENU_FILE_EXIT"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	this->GetMenu()->GetSubMenu(0)->ModifyMenu(ID_APP_EXIT,
		MF_BYCOMMAND, ID_APP_EXIT, lpszLang);

	GetPrivateProfileString(LANG_FILE_MENU, _T("MENU_FILE_SEARCH"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	m_arrLanString.Add(CString(lpszLang));

	// �༭�˵�
	GetPrivateProfileString(LANG_EDIT_MENU, _T("MENU_EDIT"), _T(""),
		lpszLang, LANG_STRING_LEN, lpszFileName);
	this->GetMenu()->ModifyMenu(1, MF_BYPOSITION, 0, lpszLang);

	GetPrivateProfileString(LANG_EDIT_MENU, _T("MENU_EDIT_UNDO"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	this->GetMenu()->GetSubMenu(1)->ModifyMenu(ID_EDIT_UNDO,
		MF_BYCOMMAND, ID_EDIT_UNDO, lpszLang);
	m_arrLanString.Add(CString(lpszLang));

	GetPrivateProfileString(LANG_EDIT_MENU, _T("MENU_EDIT_CUT"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	this->GetMenu()->GetSubMenu(1)->ModifyMenu(ID_EDIT_CUT,
		MF_BYCOMMAND, ID_EDIT_CUT, lpszLang);
	m_arrLanString.Add(CString(lpszLang));

	GetPrivateProfileString(LANG_EDIT_MENU, _T("MENU_EDIT_COPY"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	this->GetMenu()->GetSubMenu(1)->ModifyMenu(ID_EDIT_COPY, 
		MF_BYCOMMAND, ID_EDIT_COPY, lpszLang);
	m_arrLanString.Add(CString(lpszLang));

	GetPrivateProfileString(LANG_EDIT_MENU, _T("MENU_EDIT_PASTE"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	this->GetMenu()->GetSubMenu(1)->ModifyMenu(ID_EDIT_PASTE,
		MF_BYCOMMAND, ID_EDIT_PASTE, lpszLang);
	m_arrLanString.Add(CString(lpszLang));

	GetPrivateProfileString(LANG_EDIT_MENU, _T("MENU_EDIT_DELETE"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	this->GetMenu()->GetSubMenu(1)->ModifyMenu(ID_EDIT_CLEAR,
		MF_BYCOMMAND, ID_EDIT_CLEAR, lpszLang);
	m_arrLanString.Add(CString(lpszLang));

	GetPrivateProfileString(LANG_EDIT_MENU, _T("MENU_EDIT_SELECT_ALL"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	this->GetMenu()->GetSubMenu(1)->ModifyMenu(ID_EDIT_SELALL,
		MF_BYCOMMAND, ID_EDIT_SELALL, lpszLang);
	m_arrLanString.Add(CString(lpszLang));

	GetPrivateProfileString(LANG_EDIT_MENU, _T("MENU_EDIT_CLEAR_ALL"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	this->GetMenu()->GetSubMenu(1)->ModifyMenu(ID_EDIT_CLEAR_ALL,
		MF_BYCOMMAND, ID_EDIT_CLEAR_ALL, lpszLang);

	GetPrivateProfileString(LANG_EDIT_MENU, _T("MENU_EDIT_GOTO"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	this->GetMenu()->GetSubMenu(1)->ModifyMenu(ID_EDIT_GOTO,
		MF_BYCOMMAND, ID_EDIT_GOTO, lpszLang);

	GetPrivateProfileString(LANG_EDIT_MENU, _T("MENU_EDIT_SHOW_LIST"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	this->GetMenu()->GetSubMenu(1)->ModifyMenu(ID_EDIT_SHOWLIST,
		MF_BYCOMMAND, ID_EDIT_SHOWLIST, lpszLang);
	m_arrLanString.Add(CString(lpszLang));

	GetPrivateProfileString(LANG_EDIT_MENU, _T("MENU_EDIT_CHANGE_TYPE"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	this->GetMenu()->GetSubMenu(1)->ModifyMenu(ID_EDIT_CHANGE_TYPE,
		MF_BYCOMMAND, ID_EDIT_CHANGE_TYPE, lpszLang);

	GetPrivateProfileString(LANG_EDIT_MENU, _T("MENU_EDIT_FIND"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	this->GetMenu()->GetSubMenu(1)->ModifyMenu(ID_EDIT_FIND,
		MF_BYCOMMAND, ID_EDIT_FIND, lpszLang);

	GetPrivateProfileString(LANG_EDIT_MENU, _T("MENU_EDIT_FIND_NEXT"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	this->GetMenu()->GetSubMenu(1)->ModifyMenu(ID_EDIT_FIND_NEXT,
		MF_BYCOMMAND, ID_EDIT_FIND_NEXT, lpszLang);

	GetPrivateProfileString(LANG_EDIT_MENU, _T("MENU_EDIT_REPLACE"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	this->GetMenu()->GetSubMenu(1)->ModifyMenu(ID_EDIT_REPLACE,
		MF_BYCOMMAND, ID_EDIT_REPLACE, lpszLang);

	// �༭�˵���ʾ�ı�
	GetPrivateProfileString(LANG_EDIT_MENU, _T("TEXT_EDIT_UNDO"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	m_arrLanString.Add(CString(lpszLang));

	GetPrivateProfileString(LANG_EDIT_MENU, _T("TEXT_EDIT_CUT"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	m_arrLanString.Add(CString(lpszLang));

	GetPrivateProfileString(LANG_EDIT_MENU, _T("TEXT_EDIT_COPY"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	m_arrLanString.Add(CString(lpszLang));

	GetPrivateProfileString(LANG_EDIT_MENU, _T("TEXT_EDIT_PASTE"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	m_arrLanString.Add(CString(lpszLang));

	GetPrivateProfileString(LANG_EDIT_MENU, _T("TEXT_EDIT_DELETE"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	m_arrLanString.Add(CString(lpszLang));

	GetPrivateProfileString(LANG_EDIT_MENU, _T("TEXT_EDIT_SELECT_ALL"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	m_arrLanString.Add(CString(lpszLang));

	GetPrivateProfileString(LANG_EDIT_MENU, _T("TEXT_EDIT_CLEAR_ALL"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	m_arrLanString.Add(CString(lpszLang));

	GetPrivateProfileString(LANG_EDIT_MENU, _T("TEXT_EDIT_SHOW_LIST"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	m_arrLanString.Add(CString(lpszLang));

	GetPrivateProfileString(LANG_EDIT_MENU, _T("TEXT_EDIT_CHANGE_TYPE"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	m_arrLanString.Add(CString(lpszLang));

	GetPrivateProfileString(LANG_EDIT_MENU, _T("TEXT_EDIT_FIND"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	m_arrLanString.Add(CString(lpszLang));

	// ���ò˵���ʾ�ı�
	GetPrivateProfileString(LANG_SETTING_MENU, _T("TEXT_SETTING_OPTION"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	m_arrLanString.Add(CString(lpszLang));

	GetPrivateProfileString(LANG_SETTING_MENU, _T("TEXT_VIEW_TOOLBAR"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	m_arrLanString.Add(CString(lpszLang));

	GetPrivateProfileString(LANG_SETTING_MENU, _T("TEXT_VIEW_STATUSBAR"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	m_arrLanString.Add(CString(lpszLang));

	// ���ò˵�
	GetPrivateProfileString(LANG_SETTING_MENU, _T("MENU_SETTING"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	this->GetMenu()->ModifyMenu(2, MF_BYPOSITION, 0, lpszLang);

	GetPrivateProfileString(LANG_SETTING_MENU, _T("MENU_SETTING_OPTION"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	this->GetMenu()->GetSubMenu(2)->ModifyMenu(ID_SET_OPTION,
		MF_BYCOMMAND, ID_SET_OPTION, lpszLang);

	GetPrivateProfileString(LANG_SETTING_MENU, _T("MENU_SETTING_VIEW"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	this->GetMenu()->GetSubMenu(2)->ModifyMenu(2,
		MF_BYPOSITION, 0, lpszLang);
	GetPrivateProfileString(LANG_SETTING_MENU, _T("MENU_VIEW_TOOLBAR"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	this->GetMenu()->GetSubMenu(2)->GetSubMenu(2)->ModifyMenu(
		ID_VIEW_TOOLBAR, MF_BYCOMMAND, ID_VIEW_TOOLBAR, lpszLang);

	GetPrivateProfileString(LANG_SETTING_MENU, _T("MENU_VIEW_STATUSBAR"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	this->GetMenu()->GetSubMenu(2)->GetSubMenu(2)->ModifyMenu(
		ID_VIEW_STATUS_BAR, MF_BYCOMMAND, ID_VIEW_STATUS_BAR, lpszLang);

	GetPrivateProfileString(LANG_SETTING_MENU, _T("MENU_SETTING_LANGUAGE"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	this->GetMenu()->GetSubMenu(2)->ModifyMenu(4,
		MF_BYPOSITION, 0, lpszLang);

	// Ƥ���˵�
	GetPrivateProfileString(LANG_SKIN_MENU, _T("MENU_SKIN"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	this->GetMenu()->ModifyMenu(3, MF_BYPOSITION, 0, lpszLang);

	GetPrivateProfileString(LANG_SKIN_MENU, _T("MENU_SKIN_SYSTEM"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	this->GetMenu()->GetSubMenu(3)->ModifyMenu(ID_SKIN,
		MF_BYCOMMAND, ID_SKIN, lpszLang);

	if (m_sMyConfig.m_strSkin.Compare(_T("<ϵͳ����>")) == 0)
	{
		this->GetMenu()->GetSubMenu(3)->CheckMenuRadioItem(
			ID_SKIN,
			ID_SKIN + 30,
			ID_SKIN,
			MF_CHECKED | MF_BYCOMMAND
			);
	}


	// �����˵�
	GetPrivateProfileString(LANG_HELP_MENU, _T("MENU_HELP"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	this->GetMenu()->ModifyMenu(4, MF_BYPOSITION, 0, lpszLang);

	GetPrivateProfileString(LANG_HELP_MENU, _T("MENU_HELP_VIEW_HELP"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	this->GetMenu()->GetSubMenu(4)->ModifyMenu(ID_HELP,
		MF_BYCOMMAND, ID_HELP, lpszLang);

	GetPrivateProfileString(LANG_HELP_MENU, _T("MENU_HELP_ABOUT"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	this->GetMenu()->GetSubMenu(4)->ModifyMenu(ID_APP_ABOUT,
		MF_BYCOMMAND, ID_APP_ABOUT, lpszLang);

	// �����˵���ʾ�ı�
	GetPrivateProfileString(LANG_HELP_MENU, _T("TEXT_HELP_VIEW_HELP"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	m_arrLanString.Add(CString(lpszLang));

	GetPrivateProfileString(LANG_HELP_MENU, _T("TEXT_HELP_ABOUT"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	m_arrLanString.Add(CString(lpszLang));

	// �ػ�˵���
	DrawMenuBar();

	// ��������ʾ�ı�
	GetPrivateProfileString(LANG_TOOLBAR, _T("BUTTON_ADD_ITEM"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	m_arrLanString.Add(CString(lpszLang));

	GetPrivateProfileString(LANG_TOOLBAR, _T("BUTTON_NEW_CLASS"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	m_arrLanString.Add(CString(lpszLang));

	GetPrivateProfileString(LANG_TOOLBAR, _T("BUTTON_SAVE"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	m_arrLanString.Add(CString(lpszLang));

	GetPrivateProfileString(LANG_TOOLBAR, _T("BUTTON_UNDO"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	m_arrLanString.Add(CString(lpszLang));

	GetPrivateProfileString(LANG_TOOLBAR, _T("BUTTON_CUT"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	m_arrLanString.Add(CString(lpszLang));

	GetPrivateProfileString(LANG_TOOLBAR, _T("BUTTON_COPY"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	m_arrLanString.Add(CString(lpszLang));

	GetPrivateProfileString(LANG_TOOLBAR, _T("BUTTON_PASTE"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	m_arrLanString.Add(CString(lpszLang));

	GetPrivateProfileString(LANG_TOOLBAR, _T("BUTTON_CHANGE_TYPE"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	m_arrLanString.Add(CString(lpszLang));

	GetPrivateProfileString(LANG_TOOLBAR, _T("BUTTON_FIND"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	m_arrLanString.Add(CString(lpszLang));

	GetPrivateProfileString(LANG_TOOLBAR, _T("BUTTON_HELP"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	m_arrLanString.Add(CString(lpszLang));

	// �����ַ���
	GetPrivateProfileString(LANG_STRING, _T("STATUS_IDLE_MESSAGE"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	m_arrLanString.Add(CString(lpszLang));

	GetPrivateProfileString(LANG_STRING, _T("STATUS_NO_OPENED_FILE"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	m_arrLanString.Add(CString(lpszLang));

	GetPrivateProfileString(LANG_STRING, _T("STATUS_CLICK_TO_EDIT"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	m_arrLanString.Add(CString(lpszLang));

	GetPrivateProfileString(LANG_STRING, _T("STATUS_SELECT_AN_ITEM"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	m_arrLanString.Add(CString(lpszLang));

	GetPrivateProfileString(LANG_STRING, _T("STATUS_LINE_INFORMATION"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	m_arrLanString.Add(CString(lpszLang));

	GetPrivateProfileString(LANG_STRING, _T("STATUS_CODE_TYPE"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	m_arrLanString.Add(CString(lpszLang));

	GetPrivateProfileString(LANG_STRING, _T("STATUS_NUM"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	m_arrLanString.Add(CString(lpszLang));

	GetPrivateProfileString(LANG_STRING, _T("STATUS_CAPS"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	m_arrLanString.Add(CString(lpszLang));

	GetPrivateProfileString(LANG_STRING, _T("STATUS_SCRL"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	m_arrLanString.Add(CString(lpszLang));

	GetPrivateProfileString(LANG_STRING, _T("STATUS_SPLITTER_EDGE"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	m_arrLanString.Add(CString(lpszLang));

	GetPrivateProfileString(LANG_STRING, _T("STATUS_SPLITTER_MOVING"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	m_arrLanString.Add(CString(lpszLang));

	GetPrivateProfileString(LANG_STRING, _T("TREE_ALL_CLASSES"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	m_arrLanString.Add(CString(lpszLang));

	GetPrivateProfileString(LANG_STRING, _T("TREE_NORMAL_CLASS"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	m_arrLanString.Add(CString(lpszLang));

	GetPrivateProfileString(LANG_STRING, _T("NEW_CLASS_NAME"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	m_arrLanString.Add(CString(lpszLang));

	GetPrivateProfileString(LANG_MESSAGEBOX, _T("TITLE_INFORMATION"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	m_arrLanString.Add(CString(lpszLang));

	GetPrivateProfileString(LANG_MESSAGEBOX, _T("TITLE_WARNING"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	m_arrLanString.Add(CString(lpszLang));

	GetPrivateProfileString(LANG_MESSAGEBOX, _T("TITLE_ERROR"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	m_arrLanString.Add(CString(lpszLang));

	GetPrivateProfileString(LANG_MESSAGEBOX, _T("TITLE_QUESTION"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	m_arrLanString.Add(CString(lpszLang));

	// �����˵�
	GetPrivateProfileString(LANG_EDIT_MENU, _T("TEXT_EDIT_GOTO"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	m_arrLanString.Add(CString(lpszLang));

	GetPrivateProfileString(LANG_EDIT_MENU, _T("MENU_EDIT_ADD_A_LINE"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	m_arrLanString.Add(CString(lpszLang));

	GetPrivateProfileString(LANG_EDIT_MENU, _T("TEXT_EDIT_ADD_A_LINE"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	m_arrLanString.Add(CString(lpszLang));

	GetPrivateProfileString(LANG_EDIT_MENU, _T("MENU_EDIT_DEL_A_LINE"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	m_arrLanString.Add(CString(lpszLang));

	GetPrivateProfileString(LANG_EDIT_MENU, _T("TEXT_EDIT_DEL_A_LINE"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	m_arrLanString.Add(CString(lpszLang));

	GetPrivateProfileString(LANG_EDIT_MENU, _T("TEXT_EDIT_FIND_NEXT"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	m_arrLanString.Add(CString(lpszLang));

	GetPrivateProfileString(LANG_EDIT_MENU, _T("TEXT_EDIT_REPLACE"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	m_arrLanString.Add(CString(lpszLang));

	GetPrivateProfileString(LANG_FILE_MENU, _T("TEXT_FILE_SEARCH"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	m_arrLanString.Add(CString(lpszLang));

	GetPrivateProfileString(LANG_SKIN_MENU, _T("TEXT_SKIN_SYSTEM"),
		_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
	m_arrLanString.Add(CString(lpszLang));

	// ɾ���ַ���
	delete []lpszLang;
}

void CCodeMateDlg::GetLanguageString(UINT nItemID, LPTSTR strText, bool bTooltip)
{
	// ����ID���ز�ͬ���ַ���
	switch(nItemID)
	{
	case ID_FILE_ADDITEM:
		if (bTooltip == true)
			_tcscpy_s(strText, LANG_STRING_LEN, m_arrLanString[43]);
		else
			_tcscpy_s(strText, LANG_STRING_LEN, m_arrLanString[0]);
		break;
	case ID_FILE_ADDCLASS:
		if (bTooltip == true)
			_tcscpy_s(strText, LANG_STRING_LEN, m_arrLanString[44]);
		else
			_tcscpy_s(strText, LANG_STRING_LEN, m_arrLanString[1]);
		break;
	case ID_FILE_SAVE:
		if (bTooltip == true)
			_tcscpy_s(strText, LANG_STRING_LEN, m_arrLanString[45]);
		else
			_tcscpy_s(strText, LANG_STRING_LEN, m_arrLanString[2]);
		break;
	case ID_FILE_SAVE_AS:
		_tcscpy_s(strText, LANG_STRING_LEN, m_arrLanString[3]);
		break;
	case ID_FILE_CLOSE:
		_tcscpy_s(strText, LANG_STRING_LEN, m_arrLanString[4]);
		break;
	case ID_FILE_MOVETO:
		_tcscpy_s(strText, LANG_STRING_LEN, m_arrLanString[5]);
		break;
	case ID_DELETE_ITEM:
		_tcscpy_s(strText, LANG_STRING_LEN, m_arrLanString[6]);
		break;
	case ID_RENAME_ITEM:
		_tcscpy_s(strText, LANG_STRING_LEN, m_arrLanString[7]);
		break;
	case ID_FILE_PREP:
		_tcscpy_s(strText, LANG_STRING_LEN, m_arrLanString[8]);
		break;
	case ID_FILE_RELOAD:
		_tcscpy_s(strText, LANG_STRING_LEN, m_arrLanString[9]);
		break;
	case ID_FILE_SEARCH:
		_tcscpy_s(strText, LANG_STRING_LEN, m_arrLanString[78]);
		break;
	case ID_CLEAR_CLASS:
		_tcscpy_s(strText, LANG_STRING_LEN, m_arrLanString[10]);
		break;
	case ID_APP_EXIT:
		_tcscpy_s(strText, LANG_STRING_LEN, m_arrLanString[11]);
		break;
	case ID_EDIT_UNDO:
		if (bTooltip == true)
			_tcscpy_s(strText, LANG_STRING_LEN, m_arrLanString[46]);
		else
			_tcscpy_s(strText, LANG_STRING_LEN, m_arrLanString[28]);
		break;
	case ID_EDIT_CUT:
		if (bTooltip == true)
			_tcscpy_s(strText, LANG_STRING_LEN, m_arrLanString[47]);
		else
			_tcscpy_s(strText, LANG_STRING_LEN, m_arrLanString[29]);
		break;
	case ID_EDIT_COPY:
		if (bTooltip == true)
			_tcscpy_s(strText, LANG_STRING_LEN, m_arrLanString[48]);
		else
			_tcscpy_s(strText, LANG_STRING_LEN, m_arrLanString[30]);
		break;
	case ID_EDIT_PASTE:
		if (bTooltip == true)
			_tcscpy_s(strText, LANG_STRING_LEN, m_arrLanString[49]);
		else
			_tcscpy_s(strText, LANG_STRING_LEN, m_arrLanString[31]);
		break;
	case ID_EDIT_CLEAR:
		_tcscpy_s(strText, LANG_STRING_LEN, m_arrLanString[32]);
		break;
	case ID_EDIT_SELALL:
		_tcscpy_s(strText, LANG_STRING_LEN, m_arrLanString[33]);
		break;
	case ID_EDIT_CLEAR_ALL:
		_tcscpy_s(strText, LANG_STRING_LEN, m_arrLanString[34]);
		break;
	case ID_EDIT_GOTO:
		_tcscpy_s(strText, LANG_STRING_LEN, m_arrLanString[71]);
		break;
	case ID_A_LINE:
		_tcscpy_s(strText, LANG_STRING_LEN, m_arrLanString[73]);
		break;
	case ID_DEL_A_LINE:
		_tcscpy_s(strText, LANG_STRING_LEN, m_arrLanString[75]);
		break;
	case ID_EDIT_SHOWLIST:
		_tcscpy_s(strText, LANG_STRING_LEN, m_arrLanString[35]);
		break;
	case ID_EDIT_CHANGE_TYPE:
		if (bTooltip == true)
			_tcscpy_s(strText, LANG_STRING_LEN, m_arrLanString[50]);
		else
			_tcscpy_s(strText, LANG_STRING_LEN, m_arrLanString[36]);
		break;
	case ID_EDIT_FIND:
		if (bTooltip == true)
			_tcscpy_s(strText, LANG_STRING_LEN, m_arrLanString[51]);
		else
			_tcscpy_s(strText, LANG_STRING_LEN, m_arrLanString[37]);
		break;
	case ID_EDIT_FIND_NEXT:
		_tcscpy_s(strText, LANG_STRING_LEN, m_arrLanString[76]);
		break;
	case ID_EDIT_REPLACE:
		_tcscpy_s(strText, LANG_STRING_LEN, m_arrLanString[77]);
		break;
	case ID_SET_OPTION:
		_tcscpy_s(strText, LANG_STRING_LEN, m_arrLanString[38]);
		break;
	case ID_VIEW_TOOLBAR:
		_tcscpy_s(strText, LANG_STRING_LEN, m_arrLanString[39]);
		break;
	case ID_VIEW_STATUS_BAR:
		_tcscpy_s(strText, LANG_STRING_LEN, m_arrLanString[40]);
		break;
	case ID_HELP:
		_tcscpy_s(strText, LANG_STRING_LEN, m_arrLanString[41]);
		break;
	case ID_APP_ABOUT:
		if (bTooltip == true)
			_tcscpy_s(strText, LANG_STRING_LEN, m_arrLanString[52]);
		else
			_tcscpy_s(strText, LANG_STRING_LEN, m_arrLanString[42]);
		break;
	case ID_SKIN:
		_tcscpy_s(strText, LANG_STRING_LEN, m_arrLanString[79]);
	default:
		strText = _T("");
	}
}

LRESULT CCodeMateDlg::OnThemeChanged()
{
	// ��ȡϵͳ�汾
	OSVERSIONINFOEX OS;
	OS.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	if (!GetVersionEx((OSVERSIONINFO*)&OS))
		AfxMessageBox(_T("���󣺻�ȡ����ϵͳ�汾ʧ�ܡ�"), MB_ICONERROR);
	// ����˵�ͼ�꣨Vista���ϰ汾,����ϵͳ���⣩
	if (OS.dwMajorVersion >= 6 && IsThemeActive())
	{
		// ��ʾ�˵�ͼ��
		m_bShowMenuImage = true;
		SetMenuImage(false, false);
	}
	else
	{
		SetMenuImage(false, true);
		m_bShowMenuImage = false;
	}
	return 0;
}

void CCodeMateDlg::OnInitMenuPopup(CMenu *pPopupMenu, UINT nIndex, BOOL bSysMenu) 
{ 
	ASSERT(pPopupMenu != NULL);
	// �����˵���Ŀ���״̬
	CCmdUI  cuState;
	cuState.m_pMenu = pPopupMenu;
	ASSERT(cuState.m_pOther == NULL);
	ASSERT(cuState.m_pParentMenu == NULL);
	// ȷ���˵��Ƕ���˵��������� m_pOther
	// m_pParentMenu == NULL���ʾ���Ƕ���˵�
	HMENU hParentMenu;
	if (AfxGetThreadState()-> m_hTrackingMenu ==  pPopupMenu-> m_hMenu)
		cuState.m_pParentMenu = pPopupMenu;	// Parent == child ���ڸ��ٵ����˵�
	else if ((hParentMenu = ::GetMenu(m_hWnd)) != NULL)
	{
		CWnd* pParent = this;
		// �Ӵ���û�в˵������ض���
		if (pParent != NULL &&
			(hParentMenu = ::GetMenu(pParent-> m_hWnd)) != NULL)
		{
			int nIndexMax = ::GetMenuItemCount(hParentMenu);
			for (int nIndex = 0; nIndex < nIndexMax; nIndex++)
			{
				if (::GetSubMenu(hParentMenu, nIndex) == pPopupMenu-> m_hMenu)
				{
					// ���ֵ����˵���m_pParentMenu �����в˵�
					cuState.m_pParentMenu = CMenu::FromHandle(hParentMenu);
					break;
				}
			}
		}
	}
	cuState.m_nIndexMax = pPopupMenu-> GetMenuItemCount();
	for (cuState.m_nIndex = 0; cuState.m_nIndex < cuState.m_nIndexMax;
		cuState.m_nIndex++)
	{
		cuState.m_nID = pPopupMenu-> GetMenuItemID(cuState.m_nIndex);
		if (cuState.m_nID == 0)
			continue;   // �˵��ָ���Ϊ��Ч�������֮
		ASSERT(cuState.m_pOther == NULL);
		ASSERT(cuState.m_pMenu != NULL);
		if (cuState.m_nID == (UINT)-1)
		{
			// �����Ǹ������˵����õ��׸��˵���
			cuState.m_pSubMenu = pPopupMenu-> GetSubMenu(cuState.m_nIndex);
			if (cuState.m_pSubMenu == NULL   ||
				(cuState.m_nID = cuState.m_pSubMenu-> GetMenuItemID(0)) == 0 ||
				cuState.m_nID == (UINT)-1)
			{
				continue;        // �޷���ȡ�׸��˵���
			}
			cuState.DoUpdate(this, TRUE);   // �˵���ᱻ�Զ�����
		}
		else
		{
			// ����������ͨ�˵���
			// �����ܴ����� m_bAutoMenuEnable�����Զ����û����ò˵���
			// ���������ϵͳ�˵�
			cuState.m_pSubMenu = NULL;
			cuState.DoUpdate(this, FALSE);
		}
		// �����˵������ɾ
		UINT nCount = pPopupMenu-> GetMenuItemCount();
		if (nCount < cuState.m_nIndexMax)
		{

			cuState.m_nIndex -= (cuState.m_nIndexMax - nCount);
			while (cuState.m_nIndex < nCount  &&
				pPopupMenu-> GetMenuItemID(cuState.m_nIndex) == cuState.m_nID)
			{
				cuState.m_nIndex++;
			}
		}
		cuState.m_nIndexMax = nCount;
	}
	// ���Ĳ˵�����ɫ
	MENUINFO menuInfo;
	menuInfo.cbSize = sizeof(menuInfo);
	menuInfo.fMask = MIM_STYLE | MIM_BACKGROUND | MIM_APPLYTOSUBMENUS;
	GetMenuInfo(pPopupMenu->m_hMenu, &menuInfo);
	menuInfo.dwStyle |= MNS_CHECKORBMP | MNS_MODELESS;
	menuInfo.hbrBack = CreateSolidBrush(RGB(255,255,255)); 
	SetMenuInfo(pPopupMenu->m_hMenu, &menuInfo);
}

void CCodeMateDlg::SetMenuImage(bool bFirst, bool bDelImg)
{
	// ����ͼ���б���Ϣ
	CImageList *pMenuImage = m_wndToolBar.GetToolBarCtrl().GetImageList();
	// ��ť����
	UINT uiButtonCount = 0;
	// ��ȡ�˵�
	CMenu *pMainMenu, *pMenu;
	// ���˵�
	pMainMenu = this->GetMenu();
	int iMainCount = pMainMenu->GetMenuItemCount();
	// �������в˵�
	for (int i = 0; i < iMainCount; i ++)
	{
		pMenu = pMainMenu->GetSubMenu(i);
		int ItemCount = pMenu->GetMenuItemCount();
		// �������в˵���
		for (int j = 0; j < ItemCount; j ++)
		{
			if (bDelImg == true)
			{
				pMenu ->SetMenuItemBitmaps(
					j,
					MF_BYPOSITION,
					NULL,
					NULL
					);
			}
			else
			{
				// �õ��˵�ID
				UINT uiMenuIndex = pMenu->GetMenuItemID(j);
				if (uiMenuIndex == 0)
					continue;
				int iButtonID = -1, iImageID = -1;
				// �õ���ťID
				iButtonID = m_wndToolBar.CommandToIndex(uiMenuIndex);
				if (iButtonID == -1)
					continue;
				// �õ�ͼ��ID
				UINT uiCommandID, uiStyle;
				m_wndToolBar.GetButtonInfo(
					iButtonID,
					uiCommandID,
					uiStyle,
					iImageID
					);
				if (iImageID == -1)
					continue;
				// �õ�ͼ��
				if (bFirst == true)
				{
					GetListImage(
						*pMenuImage,
						m_bmpMenu[uiButtonCount],
						iImageID
						);
				}
				// ����ͼ��
				pMenu ->SetMenuItemBitmaps(
					j,
					MF_BYPOSITION,
					&m_bmpMenu[uiButtonCount],
					&m_bmpMenu[uiButtonCount]
				);
				uiButtonCount ++;
			}
		}
	}
}

BOOL CCodeMateDlg::LoadSkinMenu()
{
	/* ���¼���Ƥ����̬�˵� */
	m_pMainMenu = this->GetMenu();
	CMenu* subMenu = NULL;
	subMenu = m_pMainMenu->GetSubMenu(3);
	subMenu->AppendMenu(MF_SEPARATOR);
	// ����Skins�ļ���������Ƥ���ļ�
	CFileFind cfFile;
	BOOL bContinue = cfFile.FindFile(_T(".\\Skins\\*.ssk"));
	int i = 1;
	int iCurItem = 0;	// ���ڱ��浱ǰƤ����
	while(bContinue)
	{
		bContinue = cfFile.FindNextFile();
		CString strFileName = cfFile.GetFileName();
		if (!strFileName.IsEmpty() && !cfFile.IsDots())
		{
			if(cfFile.IsDirectory()) continue; // �����Ŀ¼
			if (strFileName == m_sMyConfig.m_strSkin)
				iCurItem = i;
			CString strMessage;
			// ȥ���ļ���չ��������˵�
			strMessage.Format(_T("%s\t"),
				strFileName.Left(strFileName.GetLength()-4));
			subMenu->AppendMenu(MF_STRING, ID_SKIN + i, strMessage);
			i++;
		}
	}
	// ���õ�ǰƤ���˵���ѡ��״̬
	if (m_bSkinLoad)
	{
		subMenu->CheckMenuRadioItem(
			ID_SKIN,
			ID_SKIN + 30,
			ID_SKIN + iCurItem,
			MF_CHECKED | MF_BYCOMMAND
			);
		return TRUE;
	}
	else
	{
		subMenu->CheckMenuRadioItem(
			ID_SKIN,
			ID_SKIN + 30,
			ID_SKIN,
			MF_CHECKED | MF_BYCOMMAND
			);
		return FALSE;
	}
}

void CCodeMateDlg::OnSkinList(UINT nIdMenuCmd) 
{
	// ѡ��Ƥ���Ĳ˵�
	CString strSkinName;
	CMenu* pSubMenu = m_pMainMenu->GetSubMenu(3);
	if (nIdMenuCmd == ID_SKIN)
	{
		m_sMyConfig.m_strSkin = _T("<ϵͳ����>");
		if (m_bSkinLoad)
		{
			AfxMessageBox(_T("�ر�Ƥ������������´ε�¼���л�Ƥ�����������������Ч��"));
			if (skinppRemoveSkin() == TRUE)
			{
				pSubMenu->CheckMenuRadioItem(
					ID_SKIN,
					ID_SKIN + 30,
					nIdMenuCmd,
					MF_CHECKED | MF_BYCOMMAND
					);
				m_bSkinLoad = false;
			}
		}
		return;
	}
	pSubMenu->GetMenuString(nIdMenuCmd, strSkinName, MF_BYCOMMAND);
	strSkinName.TrimRight();
	strSkinName += _T(".ssk");
	m_sMyConfig.m_strSkin = strSkinName;
	CString PathName = _T("Skins\\") + strSkinName;

	char *pText = NULL;
#ifdef _UNICODE
	int n = PathName.GetLength();
	int len = WideCharToMultiByte(CP_ACP,0,PathName,n,NULL,0,NULL,NULL);
	pText = new char[len+1];
	WideCharToMultiByte(CP_ACP,0,PathName,n,pText,len,NULL,NULL);
	pText[len] = '\0';
#else
	int len = PathName.GetLength();
	pText = new char[len+1];
	strcpy_s(pText, len+1, PathName.GetBuffer(0));
	pText[len] = '\0';
#endif

	if (skinppLoadSkin(pText) == TRUE)
	{
		pSubMenu->CheckMenuRadioItem(
			ID_SKIN,
			ID_SKIN+30,
			nIdMenuCmd,
			MF_CHECKED | MF_BYCOMMAND
			);
		m_bSkinLoad = true;
	}
	Invalidate();

	delete []pText;
}

void CCodeMateDlg::GetListImage(CImageList &ImageList,CBitmap &Bitmap, int nImageIdx)
{
	CClientDC dcClient(NULL);
	// ��ȡͼ����Ϣ
	IMAGEINFO ImageInfo;
	ImageList.GetImageInfo(nImageIdx, &ImageInfo);
	// ͼ��ߴ�
	int nWidth  = abs(ImageInfo.rcImage.right - ImageInfo.rcImage.left);
	int m_iHeight = abs(ImageInfo.rcImage.bottom - ImageInfo.rcImage.top);
	// ����λͼ
	Bitmap.CreateCompatibleBitmap(&dcClient, nWidth, m_iHeight);
	// ׼������
	CDC dcMemory;
	dcMemory.CreateCompatibleDC((CDC*)NULL) ;
	// ����λͼ����
	CBitmap * pBmpOld = dcMemory.SelectObject(&Bitmap);
	dcMemory.FillSolidRect(0, 0, nWidth, m_iHeight, GetSysColor(COLOR_MENU));
	// ����ͼ��
	ImageList.Draw(&dcMemory, nImageIdx, CPoint(0,0), ILD_NORMAL);
	dcMemory.SelectObject(pBmpOld);
	// ������
	dcMemory.DeleteDC();
	::DeleteObject( ImageInfo.hbmImage );
	::DeleteObject( ImageInfo.hbmMask );
}

void CCodeMateDlg::OnUpdateFileSave(CCmdUI *pCmdUI)
{
	BOOL changed=m_wndCodeEdit.GetChangedStatus();	// ���༭���ļ��Ƿ��и���
	pCmdUI-> Enable(changed);
}

void CCodeMateDlg::OnUpdateFileSaveAs(CCmdUI *pCmdUI)
{
	if (m_enFileType != SCT_ERROR)		// ����Ƿ��д�һ���ļ�
		pCmdUI-> Enable(TRUE);
	else
		pCmdUI-> Enable(FALSE);
}

void CCodeMateDlg::OnUpdateFileClose(CCmdUI *pCmdUI)
{
	if (m_enFileType != SCT_ERROR)		// ����Ƿ��д�һ���ļ�
		pCmdUI-> Enable(TRUE);
	else
		pCmdUI-> Enable(FALSE);
}

void CCodeMateDlg::OnUpdateFileMoveTo(CCmdUI* pCmdUI)
{
	pCmdUI-> Enable(m_bItemSelected);	// ����Ҫѡ��һ���������Ŀ
}

void CCodeMateDlg::OnUpdateFileDeleteItem(CCmdUI* pCmdUI)
{
	pCmdUI-> Enable(m_bItemSelected);
	CString strLoad;
	if (m_bIsFolder)	// ����Ƿ���
	{
		if (m_sMyConfig.m_uiLanguage == 0)
			strLoad.LoadString(IDS_MNU_DELCLASS);	// �����ַ�����Դ
		else
			strLoad = m_arrLanString[13];
		pCmdUI->SetText(strLoad);
	}
	else	// �������Ŀ
	{
		if (m_sMyConfig.m_uiLanguage == 0)
			strLoad.LoadString(IDS_MNU_DELITEM);
		else
			strLoad = m_arrLanString[12];
		pCmdUI->SetText(strLoad);
	}
}

void CCodeMateDlg::OnUpdateFileRenameItem(CCmdUI* pCmdUI)
{
	pCmdUI-> Enable(m_bItemSelected);
	CString strLoad;
	if (m_bIsFolder)
	{
		if (m_sMyConfig.m_uiLanguage == 0)
			strLoad.LoadString(IDS_MNU_RENAMECLASS);
		else
			strLoad = m_arrLanString[15];
		pCmdUI->SetText(strLoad);
	}
	else
	{
		if (m_sMyConfig.m_uiLanguage == 0)
			strLoad.LoadString(IDS_MNU_RENAMEITEM);
		else
			strLoad = m_arrLanString[14];
		pCmdUI->SetText(strLoad);
	}
}

void CCodeMateDlg::OnUpdateFilePrep(CCmdUI *pCmdUI)
{
	pCmdUI-> Enable(m_bItemSelected);
}

void CCodeMateDlg::OnUpdateEditUndo(CCmdUI *pCmdUI)
{
	BOOL changed = m_wndCodeEdit.GetUndoStatus();	// ���༭���Ƿ���Գ���
	pCmdUI-> Enable(changed);
}

void CCodeMateDlg::OnUpdateEditCut(CCmdUI* pCmdUI)
{
	if (m_bFocused)
		pCmdUI-> Enable(m_bTextSelected);	// ����Ҫ�ڱ༭��ѡ������
	else
		pCmdUI-> Enable(false);
}

void CCodeMateDlg::OnUpdateEditCopy(CCmdUI *pCmdUI)
{
	if (m_bFocused)
		pCmdUI-> Enable(m_bTextSelected);
	else
		pCmdUI-> Enable(false);
}

void CCodeMateDlg::OnUpdateEditPaste(CCmdUI *pCmdUI)
{
	pCmdUI-> Enable(m_bFocused);
}

void CCodeMateDlg::OnUpdateEditClear(CCmdUI *pCmdUI)
{
	if (m_bFocused)
		pCmdUI-> Enable(m_bTextSelected);
	else
		pCmdUI-> Enable(false);
}

void CCodeMateDlg::OnUpdateEditSelall(CCmdUI *pCmdUI)
{
	pCmdUI-> Enable(m_bFocused);
}

void CCodeMateDlg::OnUpdateEditClearAll(CCmdUI *pCmdUI)
{
	pCmdUI-> Enable(m_bFocused);
}

void CCodeMateDlg::OnUpdateEditFind(CCmdUI *pCmdUI)
{
	if (m_enFileType != SCT_ERROR)
		pCmdUI-> Enable(TRUE);
	else
		pCmdUI-> Enable(FALSE);
}

void CCodeMateDlg::OnUpdateEditFindNext(CCmdUI *pCmdUI)
{
	if (m_enFileType != SCT_ERROR)
		pCmdUI-> Enable(TRUE);
	else
		pCmdUI-> Enable(FALSE);
}

void CCodeMateDlg::OnUpdateEditReplace(CCmdUI *pCmdUI)
{
	if (m_enFileType != SCT_ERROR)
		pCmdUI-> Enable(TRUE);
	else
		pCmdUI-> Enable(FALSE);
}

void CCodeMateDlg::OnUpdateEditGoto(CCmdUI *pCmdUI)
{
	if (m_enFileType != SCT_ERROR)
		pCmdUI-> Enable(TRUE);
	else
		pCmdUI-> Enable(FALSE);
}

void CCodeMateDlg::OnUpdateEditShowlist(CCmdUI *pCmdUI)
{
	pCmdUI-> Enable(m_bFocused);
}

void CCodeMateDlg::OnUpdateEditChangeType(CCmdUI *pCmdUI)
{
	if (m_enFileType != SCT_ERROR)
		pCmdUI-> Enable(TRUE);
	else
		pCmdUI-> Enable(FALSE);
}

void CCodeMateDlg::OnUpdateViewToolbar(CCmdUI *pCmdUI)
{
	// ���ݹ������Ƿ���ʾ���ò˵���״̬
	pCmdUI->SetCheck(m_sMyConfig.m_bShowToolBar);
}

void CCodeMateDlg::OnUpdateViewStatusBar(CCmdUI *pCmdUI)
{
	// ����״̬���Ƿ���ʾ���ò˵���״̬
	pCmdUI->SetCheck(m_sMyConfig.m_bShowStatusBar);
}

void CCodeMateDlg::OnUpdateLanChns(CCmdUI *pCmdUI)
{
	// ���ݽ�������ѡ��
	if (m_sMyConfig.m_uiLanguage == 0)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
}

void CCodeMateDlg::OnUpdateLanChnt(CCmdUI *pCmdUI)
{
	// ���ݽ�������ѡ��
	if (m_sMyConfig.m_uiLanguage == 1)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
}

void CCodeMateDlg::OnUpdateLanEng(CCmdUI *pCmdUI)
{
	// ���ݽ�������ѡ��
	if (m_sMyConfig.m_uiLanguage == 2)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
}

void CCodeMateDlg::OnSplitterUp(UINT nID, CPoint point)
{
	// ������οؼ��ͱ༭���ؼ��Ĵ�С��λ��
	CRect rect1,rect2;
	m_wndTreeView.GetWindowRect(&rect1);
	ScreenToClient(&rect1);
	m_wndCodeEdit.GetWindowRect(&rect2);
	ScreenToClient(&rect2);
	// �����ƶ��ľ���
	int nX = point.x - m_point_x;
	// �����з������ƶ�����
	long width1 = LIMITED_LEFT - rect1.Width();
	long width2 = rect2.Width() - LIMITED_RIGHT;
	if ( nX < width1 )
		nX = width1;
	if ( nX > width2 )
		nX = width2;
	// �õ��з����Ĵ�С��λ��
	CRect rt;  
	m_wndSplitter.GetWindowRect(&rt);
	ScreenToClient(&rt);
	rt.left += nX;
	rt.right += nX;
	// ���з���λ�ñ��浽������
	m_sMyConfig.m_lSplPos += nX;
	// ��������ؼ����յ�λ�úʹ�С
	m_wndSplitter.MoveWindow(rt, false);
	rect1.right += nX;
	m_wndTreeView.MoveWindow(rect1, false);
	rect2.left += nX;
	m_wndCodeEdit.MoveWindow(rect2, false);
	// ��������
	if (m_bShowSearch)
	{
		m_edtKeyWord.GetWindowRect(&rt);
		ScreenToClient(&rt);
		rt.right += nX;
		m_edtKeyWord.MoveWindow(rt, false);
		m_btnSearch.GetWindowRect(&rt);
		ScreenToClient(&rt);
		rt.left += nX;
		rt.right += nX;
		m_btnSearch.MoveWindow(rt, false);
		m_cboType.GetWindowRect(&rt);
		ScreenToClient(&rt);
		rt.right += nX;
		m_cboType.MoveWindow(rt, false);
		m_btnHide.GetWindowRect(&rt);
		ScreenToClient(&rt);
		rt.left += nX;
		rt.right += nX;
		m_btnHide.MoveWindow(rt, false);
	}
	// �з���ֹͣ�ƶ�
	m_bMoving=false;
	// ���´���
	Invalidate(); 
}

void CCodeMateDlg::OnSplitterDown(UINT nID, CPoint point)
{
	// �з�����ʼ�ƶ�
	m_bMoving=true;
	// ���浱ǰλ��
	m_point_x = point.x;
	// ������οؼ�λ�úʹ�С
	CRect rect1;
	m_wndTreeView.GetWindowRect(&rect1);
	// ��ô��ڴ�С
	CRect rect;
	GetClientRect(&rect);
	// ����Ҫ���Ƶ�ֱ�ߵ������յ�
	m_ptLineStart.x = point.x + rect1.Width();
	m_ptLineStart.y = 30;
	m_ptLineEnd.x = point.x + rect1.Width();
	m_ptLineEnd.y = rect.Height() - 18;
}

void CCodeMateDlg::OnSplitterMove(UINT nID, CPoint point)
{
	if (m_bMoving)	// ����з��������ƶ���
	{
		// ������οؼ����༭���ؼ���λ�úʹ�С�����һ��ɴ�������
		CString str;
		CRect rect1,rect2;
		m_wndTreeView.GetWindowRect(&rect1);
		ScreenToClient(&rect1);
		m_wndCodeEdit.GetWindowRect(&rect2);
		ScreenToClient(&rect2);
		// ��ô��ڵ�λ�úʹ�С
		CRect rect;
		GetClientRect(&rect);
		// �����з����ƶ�ƫ����
		int nX = point.x - m_point_x;  
		long width1 = LIMITED_LEFT - rect1.Width();
		long width2 = rect2.Width() - LIMITED_RIGHT;
		// �ж��Ƿ������������ڣ�����״̬����ʾ��ʾ
		CString strLoad;
		if (m_sMyConfig.m_uiLanguage == 0)
			strLoad.LoadString(IDS_SPRITTER_EAGE);
		else
			strLoad = m_arrLanString[62];
		if ( nX <= width1 )
			str.Format(strLoad,width1 + m_point_x);
		else if ( nX >= width2 )
			str.Format(strLoad,width2 + m_point_x);
		else
		{
			// ��ֱ��
			CClientDC dc(this);
			dc.SetROP2(R2_NOT);   // ��ɫ��ʾֱ��
			dc.SelectStockObject(NULL_BRUSH); // ��ʹ�û�ˢ
			dc.MoveTo(m_ptLineStart);
			dc.LineTo(m_ptLineEnd);
			m_ptLineStart.x = point.x + rect1.Width();
			m_ptLineStart.y = m_sMyConfig.m_bShowToolBar ? 30 : 0;
			m_ptLineEnd.x = point.x + rect1.Width();
			m_ptLineEnd.y = rect.Height() - 
				(m_sMyConfig.m_bShowStatusBar ? 18 : 0);
			// ����ǰ�滭��ֱ��
			dc.MoveTo(m_ptLineStart);
			dc.LineTo(m_ptLineEnd);
			// ��״̬����ʾ�з����ƶ�״̬
			if (m_sMyConfig.m_uiLanguage == 0)
				strLoad.LoadString(IDS_SPRITTER_MOVING);
			else
				strLoad = m_arrLanString[63];
			str.Format(strLoad,point.x);
		}
		m_wndStatusBar.SetPaneText(0,str);
	}
}

void CCodeMateDlg::ShowKeyBoardState()
{
	// �жϼ���NUMLOCK��CAPSLOCK��SCROLLLOCK��״̬������ʾ��״̬��
	BYTE   key[LANG_STRING_LEN];   
	GetKeyboardState((LPBYTE)&key);
	CString strLoad;
	if(key[VK_CAPITAL]&1)
	{
		if (m_sMyConfig.m_uiLanguage == 0)
			strLoad.LoadString(ID_INDICATOR_CAPS);
		else
			strLoad = m_arrLanString[60];
		m_wndStatusBar.SetPaneText(3, strLoad, TRUE);
	}
	else
		m_wndStatusBar.SetPaneText(3, _T("����"), TRUE);
	if(key[VK_NUMLOCK]&1)
	{
		if (m_sMyConfig.m_uiLanguage == 0)
			strLoad.LoadString(ID_INDICATOR_NUM);
		else
			strLoad = m_arrLanString[59];
		m_wndStatusBar.SetPaneText(4, strLoad, TRUE);
	}
	else
		m_wndStatusBar.SetPaneText(4, _T("����"), TRUE);
	if(key[VK_SCROLL]&1)
	{
		if (m_sMyConfig.m_uiLanguage == 0)
			strLoad.LoadString(ID_INDICATOR_SCRL);
		else
			strLoad = m_arrLanString[61];
		m_wndStatusBar.SetPaneText(5, strLoad, TRUE);
	}
	else
		m_wndStatusBar.SetPaneText(5, _T("����"), TRUE);
}

// �����Ի��������С����ť������Ҫ����Ĵ���
// �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
// �⽫�ɿ���Զ���ɡ�

void CCodeMateDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND,
			reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

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
		CDialogEx::OnPaint();
	}
	ShowKeyBoardState();
}

// ���û��϶���С������ʱϵͳ���ô˺���ȡ�ù����ʾ��
HCURSOR CCodeMateDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CCodeMateDlg::OnMove(int x, int y)
{
	CDialogEx::OnMove(x, y);
	// ��ô��ڵ�λ�ü���С
	CString mytest;
	CRect rcWin;
	GetWindowRect(rcWin);
	if (rcWin.left < 0 && rcWin.top < 0)	// �������󻯻���С��
		return;
	else
		GetWindowRect(&(m_sMyConfig.m_recWinPos));	// ���洰��λ�úʹ�С
}

void CCodeMateDlg::OnSize(UINT uiType, int iX, int iY)
{
	if (uiType != SIZE_MINIMIZED)     // ������С��״̬��ִ�У���ֹ����
	{
		// �����������ÿؼ��߶�
		int iSBHeight = (m_sMyConfig.m_bShowStatusBar ? 18 : 0);
		int iTBHeight = (m_sMyConfig.m_bShowToolBar ? 30 : 0);
		int iSearchHeight = (m_bShowSearch ? 50 : 0);

		if (uiType != SIZE_MAXIMIZED)     // ����������
		{
			m_sMyConfig.m_bMaximize = false;	// ���治����󻯵�״̬
			GetWindowRect(&(m_sMyConfig.m_recWinPos));	// ���洰��λ�úʹ�С
		}
		else     // ��������
		{
			m_sMyConfig.m_bMaximize = true;	// ��������󻯵�״̬
		}
		// ���µ������ؼ��ľ���λ��
		if (m_wndStatusBar.GetSafeHwnd())	// ״̬��
		{
			m_wndStatusBar.MoveWindow(0, iY - 18, iX, 18);
			m_wndStatusBar.SetPaneInfo(0, IDS_INDICATOR_MESSAGE,
				SBPS_NORMAL, iX - 450);      
		}

		if (m_wndToolBar.GetSafeHwnd())		// ������
			m_wndToolBar.MoveWindow(0, 0, iX, 30);

		if (m_wndCodeEdit.GetSafeHwnd())
		{
			if (m_enFileType != SCT_ERROR)
			{
				m_wndCodeEdit.MoveWindow(200 + m_sMyConfig.m_lSplPos,
					iTBHeight, iX - 200 - m_sMyConfig.m_lSplPos,
					iY - iTBHeight - iSBHeight);
			}
			m_iWidth = iX - 200 - m_sMyConfig.m_lSplPos;
			m_iHeight = iY;
		}

		if (m_wndTreeView.GetSafeHwnd())		// ���οؼ�
		{
			m_wndTreeView.MoveWindow(0, iTBHeight, 197 + m_sMyConfig.m_lSplPos,
				iY - iTBHeight - iSBHeight - iSearchHeight);
		}

		if (m_bShowSearch)	// ��ʾ������
		{
			if (m_stcContent.GetSafeHwnd())
			{
				m_stcContent.MoveWindow(3,
					iY - iSBHeight - iSearchHeight + 3,
					60, 20);
			}

			if (m_edtKeyWord.GetSafeHwnd())
			{
				m_edtKeyWord.MoveWindow(63, iY - iSBHeight - iSearchHeight + 3,
					m_sMyConfig.m_lSplPos + 80 , 20);
			}

			if (m_btnSearch.GetSafeHwnd())
			{
				m_btnSearch.MoveWindow(197 + m_sMyConfig.m_lSplPos - 53,
					iY - iSBHeight - iSearchHeight + 3, 50, 20);
			}

			if (m_chkCase.GetSafeHwnd())
			{
				m_chkCase.MoveWindow(3, iY - iSBHeight - iSearchHeight + 26
					, 80, 20);
				m_chkCase.Invalidate();
			}

			if (m_cboType.GetSafeHwnd())
			{
				m_cboType.MoveWindow(83, iY - iSBHeight - iSearchHeight + 26,
					m_sMyConfig.m_lSplPos + 60 , 20);
				m_cboType.Invalidate();
			}

			if (m_btnHide.GetSafeHwnd())
			{
				m_btnHide.MoveWindow(197 + m_sMyConfig.m_lSplPos - 53,
					iY - iSBHeight - iSearchHeight + 26, 50, 20);
				m_btnHide.Invalidate();
			}
		}

		if (m_wndSplitter.GetSafeHwnd())		// �з���
		{
			m_wndSplitter.MoveWindow(197 + m_sMyConfig.m_lSplPos,
				iTBHeight, 3, iY - iTBHeight - iSBHeight);
		}
	}
}

void CCodeMateDlg::OnSizing(UINT fwSide, LPRECT pRect)
{
	CDialogEx::OnSizing(fwSide, pRect);
	// ��ȡ�����Ⱥ͸߶�
	CRect rect(pRect);
	int width = rect.Width();
	int height = rect.Height();
	// �жϸ��ָı䴰�ڴ�С�Ĳ����������ƴ��ڵĿ����Сֵ
	switch(fwSide)
	{
	case WMSZ_BOTTOM:
		{
			if (height < MINDLG_HEIGHT)
				pRect->bottom = pRect->top + MINDLG_HEIGHT;
			break;
		}
	case WMSZ_LEFT:
		{
			if (width < MINDLG_WIDTH)
				pRect->left = pRect->right - MINDLG_WIDTH;
			if (pRect->right < pRect->left + 166 +
				m_sMyConfig.m_lSplPos + LIMITED_RIGHT)
			{
				pRect->left = pRect->right - 166 -
					m_sMyConfig.m_lSplPos - LIMITED_RIGHT;
			}
			break;
		}
	case WMSZ_RIGHT:
		{ 
			if (width < MINDLG_WIDTH)
				pRect->right = pRect->left + MINDLG_WIDTH;
			if (pRect->right < pRect->left + 166 +
				m_sMyConfig.m_lSplPos + LIMITED_RIGHT)
			{
				pRect->right = pRect->left + 166 +
					m_sMyConfig.m_lSplPos + LIMITED_RIGHT;
			}
			break;
		}
	case WMSZ_TOP:
		{
			if (height < MINDLG_HEIGHT)
				pRect->top = pRect->bottom - MINDLG_HEIGHT;
			break;
		}
	case WMSZ_TOPLEFT:
		{
			if (width < MINDLG_WIDTH)
				pRect->left = pRect->right - MINDLG_WIDTH;
			if (pRect->right < pRect->left + 166 +
				m_sMyConfig.m_lSplPos + LIMITED_RIGHT)
			{
				pRect->left = pRect->right - 166 -
					m_sMyConfig.m_lSplPos - LIMITED_RIGHT;
			}
			if (height < MINDLG_HEIGHT)
				pRect->top = pRect->bottom - MINDLG_HEIGHT;
			break;
		}
	case WMSZ_TOPRIGHT:
		{
			if (width < MINDLG_WIDTH)
				pRect->right = pRect->left + MINDLG_WIDTH;
			if (pRect->right < pRect->left + 166 +
				m_sMyConfig.m_lSplPos + LIMITED_RIGHT)
			{
				pRect->right = pRect->left + 166 +
					m_sMyConfig.m_lSplPos + LIMITED_RIGHT;
			}
			if (height < MINDLG_HEIGHT)
				pRect->top = pRect->bottom - MINDLG_HEIGHT;
			break;
		}
	case WMSZ_BOTTOMLEFT:
		{
			if (width < MINDLG_WIDTH)
				pRect->left = pRect->right - MINDLG_WIDTH;
			if (pRect->right < pRect->left + 166 +
				m_sMyConfig.m_lSplPos + LIMITED_RIGHT)
			{
				pRect->left = pRect->right - 166 -
					m_sMyConfig.m_lSplPos - LIMITED_RIGHT;
			}
			if (height < MINDLG_HEIGHT)
				pRect->bottom = pRect->top + MINDLG_HEIGHT;
			break;
		}
	case WMSZ_BOTTOMRIGHT:
		{
			if (width < MINDLG_WIDTH)
				pRect->right = pRect->left + MINDLG_WIDTH;
			if (pRect->right < pRect->left + 166 +
				m_sMyConfig.m_lSplPos + LIMITED_RIGHT)
			{
				pRect->right = pRect->left + 166 +
					m_sMyConfig.m_lSplPos + LIMITED_RIGHT;
			}
			if (height < MINDLG_HEIGHT)
				pRect->bottom = pRect->top + MINDLG_HEIGHT;
			break;
		}
	}
}

BOOL CCodeMateDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if(pWnd == this)
		SetPaneText();	// ������ָ��û�о����ӿؼ�������ΪĬ������
	else
	{
		if(DYNAMIC_DOWNCAST(CComboBox, pWnd->GetOwner()))
			SetPaneText(pWnd->GetOwner()->GetDlgCtrlID());    // ֻ������CCombo(Ex)�ؼ�	
		else
			SetPaneText(pWnd->GetDlgCtrlID());	              // �����ؼ�
	}
	return CDialogEx::OnSetCursor(pWnd, nHitTest, message);
}

void CCodeMateDlg::SetPaneText(UINT nID)
{
	CString sText;
	if(!sText.LoadString(nID) || nID == (UINT)m_wndStatusBar.GetDlgCtrlID())	// ״̬������
	{
		if (m_sMyConfig.m_uiLanguage == 0)
			sText.LoadString(AFX_IDS_IDLEMESSAGE);	// һ������£���ʾĬ������
		else
			sText = m_arrLanString[53];
	}
	if (nID != (UINT)m_wndToolBar.GetDlgCtrlID())
		m_wndStatusBar.SetPaneText(0, sText);		// ���������⣬��ʾ�ؼ���Ӧ����ʾ��Ϣ
}

BOOL CCodeMateDlg::PreTranslateMessage(MSG* pMsg)
{
	CString strLoad;
	ShowKeyBoardState();
	// �����̺�������Ϣ
	BOOL bResult = FALSE;
	if (m_bFocused)	// ��������ڱ༭��
	{
		switch (pMsg->message)
		{
		case WM_MOUSEWHEEL:
			{
				// ���������Ϣ
				bResult = TRUE;
				m_wndCodeEdit.OnMouseWheel(
					LOWORD(pMsg->wParam),
					HIWORD(pMsg->wParam),
					CPoint(LOWORD(pMsg->lParam),
					HIWORD(pMsg->lParam))
					);
			}
			break;

		case WM_KEYDOWN:
			{
				// ���������Ϣ
				switch (pMsg->wParam)
				{
				case VK_UP:
				case VK_DOWN:
				case VK_LEFT:
				case VK_RIGHT:
				case VK_HOME:
				case VK_END:
				case VK_DELETE:
					{
						bResult = TRUE;
						m_wndCodeEdit.OnKeyDown(
							(UINT)pMsg->wParam,
							LOWORD(pMsg->lParam),
							HIWORD(pMsg->lParam)
							);
					}
					break;
				case VK_ESCAPE:
					m_bEscPressed=true;
					break;
				}
			}
		case WM_SYSKEYDOWN:
			{
				// �����ݼ�
				int iResult = TranslateAccelerator(m_hWnd,hAcc,pMsg);
				if(iResult)	return TRUE;
			}
			break;

		case WM_KEYUP:
			{
				// ���������Ϣ
				switch (pMsg->wParam)
				{
				case VK_UP:
				case VK_DOWN:
				case VK_LEFT:
				case VK_RIGHT:
				case VK_HOME:
				case VK_END:
				case VK_DELETE:
					{
						bResult = TRUE;
						m_wndCodeEdit.OnKeyUp(
							(UINT)pMsg->wParam,
							LOWORD(pMsg->lParam),
							HIWORD(pMsg->lParam)
							);
					}
					break;
				}
				// ���»�ñ༭������ѡȡ��Ϣ
				int sl,sc,el,ec;
				m_wndCodeEdit.GetSelection(sl,sc,el,ec);
				if ( el - sl != 0 )
				{
					m_bTextSelected=true;
				}
				else
				{
					if ( ec-sc != 0 )
						m_bTextSelected=true;
					else
						m_bTextSelected=false;
				}
			}
			break;

		case WM_CHAR:
			{
				// ���������Ϣ
				bResult = TRUE;
				m_wndCodeEdit.OnChar(
					(UINT)pMsg->wParam,
					LOWORD(pMsg->lParam),
					HIWORD(pMsg->lParam)
					);
			}
			break;

		case WM_LBUTTONUP:
			{
				// �жϱ༭������ѡ��״̬���������ü��и���ճ���������״̬
				int sl,sc,el,ec;
				m_wndCodeEdit.GetSelection(sl,sc,el,ec);
				if ( el - sl != 0 )
				{
					m_bTextSelected=true;
				}
				else
				{
					if ( ec-sc  != 0 )
						m_bTextSelected=true;
					else
						m_bTextSelected=false;
				}
				// û��break���������������Ϣ
			}

		default:
			{
				// ����ΪĬ�����ͼ�����Ϣ
				bResult = CWnd::PreTranslateMessage(pMsg);
			}
		}
		// ��״̬����ʾ�кš��кš�������
		int linenum=m_wndCodeEdit.GetCurrentLine();
		int columnnum=m_wndCodeEdit.GetCurrentColumn();
		int charnum=m_wndCodeEdit.GetCurrentChar();
		if (m_sMyConfig.m_uiLanguage == 0)
			strLoad.LoadString(IDS_LINE_INFO);
		else
			strLoad = m_arrLanString[57];
		CString txt;
		txt.Format(strLoad, linenum+1, columnnum, charnum);
		m_wndStatusBar.SetPaneText(1,txt);
	}
	else	// ������㲻�ڱ༭��
	{
		// ״̬��������ʾ�кš��к���Ϣ
		if (m_enFileType != SCT_ERROR)	// �Ѵ��ĵ�
		{
			if (m_sMyConfig.m_uiLanguage == 0)
				strLoad.LoadString(IDS_INDICATOR_FOCUS);
			else
				strLoad = m_arrLanString[55];
		}
		else
		{
			if (m_sMyConfig.m_uiLanguage == 0)
				strLoad.LoadString(IDS_INDICATOR_TIME);
			else
				strLoad = m_arrLanString[56];
		}
		m_wndStatusBar.SetPaneText(1, strLoad, TRUE);
		// ���WM_KEYDOWN��Ϣ��WM_SYSKEYDOWN��Ϣ�������ݼ�
		switch(pMsg->message)
		{
		case WM_KEYDOWN:
			switch (pMsg->wParam)
			{
			case VK_ESCAPE:
				if (GetFocus()->GetDlgCtrlID() == IDC_EDIT1)
					OnBnClickedButtonHide();
				else
					m_bEscPressed = true;
				break;
			case VK_RETURN:
				if (GetFocus()->GetDlgCtrlID() == IDC_EDIT1)
					OnBnClickedButtonSearch();
				break;
			}
		case WM_SYSKEYDOWN:
			int iResult = TranslateAccelerator(m_hWnd,hAcc,pMsg);
			// �����ݼ��ɹ�������TRUE
			if(iResult)	return TRUE;
		}
		bResult = CWnd::PreTranslateMessage(pMsg);
	}
	// �����жϱ༭�������Ƿ��иĶ����������ñ�������״̬
	if (m_enFileType != SCT_ERROR)
	{
		CString strShow, strLang;
		switch (m_enFileType)
		{
		case SCT_C_PLUS_PLUS:	// C++
			strLang = _T("C++");
			break;
		case SCT_C_ANSI:	// C
			strLang = _T("ANSI C");
			break;
		case SCT_C_SHARP:	// C#
			strLang = _T("C#.NET");
			break;
		case SCT_HTML:	// HTM
			strLang = _T("HTML");
			break;
		case SCT_JSCRIPT:	// JScript
			strLang = _T("Java Script");
			break;
		case SCT_VBSCRIPT:	// VBScript
			strLang = _T("VB Script");
			break;
		case SCT_SQL:	// SQL
			strLang = _T("SQL");
			break;
		default:	// ��ͨ�ı�
			strLang.LoadString(IDS_NORMAL_TEXT);
		}
		BOOL bChanged = m_wndCodeEdit.GetChangedStatus();
		if (bChanged)
		{
			strShow.Format(_T("%s - %s * [%s]"),
				m_strAppTitle, m_strTitleShow, strLang);
		}
		else
		{
			strShow.Format(_T("%s - %s [%s]"),
				m_strAppTitle, m_strTitleShow, strLang);
		}
		SetWindowText(strShow);
	}
	return bResult;
}

// void LineDrawProc(HDC hDC, LPTSTR lpszText, int iLen, RECT rect, int iSelectionStart, int iSelectionEnd)
// {
// 	// �༭���Զ�����ʾ״̬����
// 	g_pCppEditWnd->DefaultLineDrawProc(lpszText, iLen, rect, iSelectionStart, iSelectionEnd);
// }

BOOL CCodeMateDlg::OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult)
{
	ASSERT(pNMHDR->code == TTN_NEEDTEXTA || pNMHDR->code == TTN_NEEDTEXTW);
	// �������ܴ�����Ϣ
	if (GetRoutingFrame() != NULL)
		return FALSE;
	// ���� ANSI �� UNICODE �汾����Ϣ
	TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
	TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
	TCHAR szFullText[LANG_STRING_LEN];
	szFullText[0] = '\0';
	CString cstTipText;
	CString cstStatusText;
	// ��ȡID
	UINT_PTR nID = pNMHDR->idFrom;
	if (pNMHDR->code == TTN_NEEDTEXTA && (pTTTA->uFlags & TTF_IDISHWND) ||
		pNMHDR->code == TTN_NEEDTEXTW && (pTTTW->uFlags & TTF_IDISHWND))
	{
		// idFrom �ǹ��ߵ� HWND
		nID = ((UINT)(WORD)::GetDlgCtrlID((HWND)nID));
	}
	if (nID != 0) // �ָ�����ID��0
	{
		if (m_sMyConfig.m_uiLanguage == 0)
		{
			AfxLoadString((UINT)nID, szFullText);
			// ����ID���ǰ�ť����
			AfxExtractSubString(cstTipText, szFullText, 1, '\n');
			AfxExtractSubString(cstStatusText, szFullText, 0, '\n');
		}
		else
		{
			GetLanguageString((UINT)nID, szFullText, true);
			cstTipText = szFullText;
			GetLanguageString((UINT)nID, szFullText, false);
			cstStatusText = szFullText;
		}
	}
	// ����״̬����Ϣ
	m_wndStatusBar.SetPaneText(0, cstStatusText);
	// �� UNICODE �ַ�������ʾ�� ToolTip ����
	// ���½�������ת��
	int n = cstTipText.GetLength();
	// ��ȡ���ֽ��ַ��Ĵ�С����С�ǰ��ֽڼ����
	int len = WideCharToMultiByte(CP_ACP,0,cstTipText,
		cstTipText.GetLength(),NULL,0,NULL,NULL);
	// Ϊ���ֽ��ַ���������ռ䣬�����СΪ���ֽڼ���Ŀ��ֽ��ֽڴ�С
	char * pTipText = new char[len+1];   // ���ֽ�Ϊ��λ
	// ���ֽڱ���ת���ɶ��ֽڱ���
	WideCharToMultiByte(CP_ACP,0,cstTipText,cstTipText.GetLength(),
		pTipText,len,NULL,NULL);
	pTipText[len] = '\0';   // ���ֽ��ַ���'\0'����
	// ANSI �� UNICODE �汾�Բ�ͬ��ʽ����
	if (pNMHDR->code == TTN_NEEDTEXTA)
		lstrcpyn((LPTSTR)pTTTA->szText, cstTipText,
		(sizeof(pTTTA->szText)/sizeof(pTTTA->szText[0])));
	else
		_mbstowcsz(pTTTW->szText, pTipText,
		(sizeof(pTTTW->szText)/sizeof(pTTTW->szText[0])));
	*pResult = 0;
	// �� ToolTip ����λ����������֮��
	::SetWindowPos(pNMHDR->hwndFrom, HWND_TOP, 0, 0, 0, 0,
		SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE);
	return TRUE;    // ��Ϣ�Ѿ�����
}

void CCodeMateDlg::OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu) 
{
	CDialogEx::OnMenuSelect(nItemID, nFlags, hSysMenu);
	LPTSTR szFullText = new _TCHAR[LANG_STRING_LEN];
	szFullText[0] = '\0';
	CString cstStatusText;
	// ����Ϣ��ʾ�������ڵ�״̬��
	if (nItemID != 0) // �ָ��������˵�����
	{
		if (m_sMyConfig.m_uiLanguage == 0)
		{
			AfxLoadString(nItemID, szFullText);
			// ����ID���ǰ�ť����
			AfxExtractSubString(cstStatusText, szFullText, 0, '\n');
		}
		else
		{
			if (nItemID == ID_LAN_CHNS ||
				nItemID == ID_LAN_CHNT ||
				nItemID == ID_LAN_ENG)
			{
				AfxLoadString(nItemID, szFullText);
				// ����ID���ǰ�ť����
				AfxExtractSubString(cstStatusText, szFullText, 0, '\n');
			}
			else
			{
				GetLanguageString(nItemID, szFullText, false);
				cstStatusText = szFullText;
			}
		}

		m_wndStatusBar.SetPaneText(0,cstStatusText);
	}
	// �����б��
	m_wndCodeEdit.DestroyList();
}

void CCodeMateDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if(nID == 61536)	// ������µ��ǹرհ�ť
	{ 
		if (m_wndCodeEdit.GetChangedStatus() == TRUE)	// �ļ��Ѿ��޸�
		{
			CString strContent,strTitle;
			if (m_sMyConfig.m_uiLanguage == 0)
			{
				strContent.LoadString(IDS_MSGBOX_EXITWARN);
				strTitle.LoadString(IDS_MSGBOX_QUES);
			}
			else
			{
				SetCurrentDirectory(m_strAppPath);
				LPTSTR strFileName = NULL;
				if (m_sMyConfig.m_uiLanguage == 1)
					strFileName = LANG_TCHINESE_FILE;
				else if (m_sMyConfig.m_uiLanguage == 2)
					strFileName = LANG_ENGLISH_FILE;
				LPTSTR lpszLang = new _TCHAR[LANG_STRING_LEN];
				GetPrivateProfileString(LANG_MESSAGEBOX,
					_T("CONTENT_FILE_SAVE"), _T(""), lpszLang,
					LANG_STRING_LEN, strFileName);
				strContent = lpszLang;
				strContent.Replace(_T("\\n"), _T("\n"));
				strTitle = m_arrLanString[70];
				delete []lpszLang;
				SetCurrentDirectory(m_sMyConfig.m_strWorkPath);
			}
			int iResult = MessageBox(strContent, strTitle,
				MB_ICONQUESTION | MB_YESNOCANCEL);
			if (iResult == IDCANCEL)
				return;	// ȡ���˳�
			if (iResult == IDYES)	// �����ļ�
			{
				if (!m_wndCodeEdit.Save((LPTSTR)((LPCTSTR)m_strFileName)))
				{
					if (m_sMyConfig.m_uiLanguage == 0)
					{
						strContent.LoadString(IDS_MSGBOX_SAVEERROR);
						strTitle.LoadString(IDS_MSGBOX_ERROR);
					}
					else
					{
						SetCurrentDirectory(m_strAppPath);
						LPTSTR strFileName = NULL;
						if (m_sMyConfig.m_uiLanguage == 1)
							strFileName = LANG_TCHINESE_FILE;
						else if (m_sMyConfig.m_uiLanguage == 2)
							strFileName = LANG_ENGLISH_FILE;
						LPTSTR lpszLang = new _TCHAR[LANG_STRING_LEN];
						GetPrivateProfileString(LANG_MESSAGEBOX,
							_T("CONTENT_SAVE_ERROR"), _T(""),
							lpszLang, LANG_STRING_LEN, strFileName);
						strContent = lpszLang;
						strContent.Replace(_T("\\n"), _T("\n"));
						strTitle = m_arrLanString[69];
						delete []lpszLang;
						SetCurrentDirectory(m_sMyConfig.m_strWorkPath);
					}
					MessageBox(strContent, strTitle, MB_ICONERROR);
					return;
				}
			}
		}
		// ��������
		SetCurrentDirectory(m_strAppPath);
		m_sMyConfig.SaveSettings(CONFIG_FILE_NAME);
	}

	CDialogEx::OnSysCommand(nID, lParam);
}

void CCodeMateDlg::OnMainAbout()
{
	// �����б��
	m_wndCodeEdit.DestroyList();
	// ��ʾ�����ڡ��Ի���
	CAboutDlg dlgAbout;
	dlgAbout.m_uiLanguage = m_sMyConfig.m_uiLanguage;
	dlgAbout.m_strAppPath = m_strAppPath;
	dlgAbout.m_strWorkPath = m_sMyConfig.m_strWorkPath;
	dlgAbout.DoModal();
}

void CCodeMateDlg::OnFileExit()
{
	if (m_wndCodeEdit.GetChangedStatus() == TRUE)	// �ļ��Ѿ��޸�
	{
		CString strContent,strTitle;
		if (m_sMyConfig.m_uiLanguage == 0)
		{
			strContent.LoadString(IDS_MSGBOX_EXITWARN);
			strTitle.LoadString(IDS_MSGBOX_QUES);
		}
		else
		{
			SetCurrentDirectory(m_strAppPath);
			LPTSTR strFileName = NULL;
			if (m_sMyConfig.m_uiLanguage == 1)
				strFileName = LANG_TCHINESE_FILE;
			else if (m_sMyConfig.m_uiLanguage == 2)
				strFileName = LANG_ENGLISH_FILE;
			LPTSTR lpszLang = new _TCHAR[LANG_STRING_LEN];
			GetPrivateProfileString(LANG_MESSAGEBOX,
				_T("CONTENT_FILE_SAVE"), _T(""),
				lpszLang, LANG_STRING_LEN, strFileName);
			strContent = lpszLang;
			strContent.Replace(_T("\\n"), _T("\n"));
			strTitle = m_arrLanString[70];
			delete []lpszLang;
			SetCurrentDirectory(m_sMyConfig.m_strWorkPath);
		}
		int iResult = MessageBox(strContent, strTitle,
			MB_ICONQUESTION | MB_YESNOCANCEL);
		if (iResult == IDCANCEL)
			return;	// ȡ���˳�
		if (iResult == IDYES)	// �����ļ�
		{
			if (!m_wndCodeEdit.Save((LPTSTR)((LPCTSTR)m_strFileName)))
			{
				if (m_sMyConfig.m_uiLanguage == 0)
				{
					strContent.LoadString(IDS_MSGBOX_SAVEERROR);
					strTitle.LoadString(IDS_MSGBOX_ERROR);
				}
				else
				{
					SetCurrentDirectory(m_strAppPath);
					LPTSTR strFileName = NULL;
					if (m_sMyConfig.m_uiLanguage == 1)
						strFileName = LANG_TCHINESE_FILE;
					else if (m_sMyConfig.m_uiLanguage == 2)
						strFileName = LANG_ENGLISH_FILE;
					LPTSTR lpszLang = new _TCHAR[LANG_STRING_LEN];
					GetPrivateProfileString(LANG_MESSAGEBOX,
						_T("CONTENT_SAVE_ERROR"), _T(""),
						lpszLang, LANG_STRING_LEN, strFileName);
					strContent = lpszLang;
					strContent.Replace(_T("\\n"), _T("\n"));
					strTitle = m_arrLanString[69];
					delete []lpszLang;
					SetCurrentDirectory(m_sMyConfig.m_strWorkPath);
				}
				MessageBox(strContent, strTitle, MB_ICONERROR);
				return;
			}
		}
	}
	// ��������
	SetCurrentDirectory(m_strAppPath);
	m_sMyConfig.SaveSettings(CONFIG_FILE_NAME);

	OnOK();
}


void CCodeMateDlg::OnFileSave()
{
	// �����б��
	m_wndCodeEdit.DestroyList();
	// �����ļ�
	if (!m_wndCodeEdit.Save((LPTSTR)((LPCTSTR)m_strFileName)))
	{
		// �������ʧ�ܣ�����������Ϣ
		CString strContent,strTitle;
		if (m_sMyConfig.m_uiLanguage == 0)
		{
			strContent.LoadString(IDS_MSGBOX_SAVEERROR);
			strTitle.LoadString(IDS_MSGBOX_ERROR);
		}
		else
		{
			SetCurrentDirectory(m_strAppPath);
			LPTSTR strFileName = NULL;
			if (m_sMyConfig.m_uiLanguage == 1)
				strFileName = LANG_TCHINESE_FILE;
			else if (m_sMyConfig.m_uiLanguage == 2)
				strFileName = LANG_ENGLISH_FILE;
			LPTSTR lpszLang = new _TCHAR[LANG_STRING_LEN];
			GetPrivateProfileString(LANG_MESSAGEBOX,
				_T("CONTENT_SAVE_ERROR"), _T(""),
				lpszLang, LANG_STRING_LEN, strFileName);
			strContent = lpszLang;
			strContent.Replace(_T("\\n"), _T("\n"));
			strTitle = m_arrLanString[69];
			delete []lpszLang;
			SetCurrentDirectory(m_sMyConfig.m_strWorkPath);
		}
		MessageBox(strContent, strTitle, MB_ICONERROR);
	}
}

void CCodeMateDlg::OnFileSaveAs()
{
	// �ж�Ҫ�����ļ�����չ������ȷ�ϵ����Ի����е��ļ������б�����
	CString strDefExt,strFileExt;
	switch(m_enFileType)
	{
	case SCT_C_PLUS_PLUS:
		{
			strDefExt = _T("CPP");
			strFileExt = _T("C++ �����ļ� (*.cpp)|*.cpp|C++ ͷ�ļ� (*.h)|*.h|�����ļ� (*.*)|*.*||");
		}
		break;
	case SCT_C_ANSI:
		{
			strDefExt = _T("C");
			strFileExt = _T("C �����ļ� (*.c)|*.c|C ͷ�ļ� (*.h)|*.h|�����ļ� (*.*)|*.*||");
		}
		break;
	case SCT_C_SHARP:
		{
			strDefExt = _T("CS");
			strFileExt = _T("C# �����ļ� (*.cs)|*.cs|�����ļ� (*.*)|*.*||");
		}
		break;
	case SCT_HTML:
		{
			strDefExt = _T("HTM");
			strFileExt = _T("HTM �ļ� (*.htm)|*.htm|HTML �ļ� (*.html)|*.html|�����ļ� (*.*)|*.*||");
		}
		break;
	case SCT_JSCRIPT:
		{
			strDefExt = _T("JS");
			strFileExt = _T("Java Script �����ļ� (*.js)|*.js|�����ļ� (*.*)|*.*||");
		}
		break;
	case SCT_VBSCRIPT:
		{
			strDefExt = _T("VBS");
			strFileExt = _T("VB Script �����ļ� (*.vbs)|*.vbs|�����ļ� (*.*)|*.*||");
		}
		break;
	case SCT_SQL:
		{
			strDefExt = _T("SQL");
			strFileExt = _T("SQL �����ļ� (*.sql)|*.sql|�����ļ� (*.*)|*.*||");
		}
		break;
	case SCT_NONE:
		{
			strDefExt = _T("TXT");
			strFileExt = _T("�ı��ļ� (*.txt)|*.txt|�����ļ� (*.*)|*.*||");
		}
		break;
	}
	CString strFileTitle = m_wndTreeView.GetItemText(m_hSelected);
	// ��ʾ�����ļ��Ի���
	CFileDialog dlgSaveAs( FALSE, strDefExt, strFileTitle,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST,
		strFileExt,NULL);
	if(dlgSaveAs.DoModal()==IDOK)
	{
		CString strContent,strTitle;
		CString ExportFile = dlgSaveAs.GetPathName();
		// ���ļ����浽ָ��λ��
		if (!m_wndCodeEdit.Save((LPTSTR)((LPCTSTR)ExportFile)))
		{
			if (m_sMyConfig.m_uiLanguage == 0)
			{
				strContent.LoadString(IDS_MSGBOX_SAVEERROR);
				strTitle.LoadString(IDS_MSGBOX_ERROR);
			}
			else
			{
				SetCurrentDirectory(m_strAppPath);
				LPTSTR strFileName = NULL;
				if (m_sMyConfig.m_uiLanguage == 1)
					strFileName = LANG_TCHINESE_FILE;
				else if (m_sMyConfig.m_uiLanguage == 2)
					strFileName = LANG_ENGLISH_FILE;
				LPTSTR lpszLang = new _TCHAR[LANG_STRING_LEN];
				GetPrivateProfileString(LANG_MESSAGEBOX,
					_T("CONTENT_SAVE_ERROR"), _T(""),
					lpszLang, LANG_STRING_LEN, strFileName);
				strContent = lpszLang;
				strContent.Replace(_T("\\n"), _T("\n"));
				strTitle = m_arrLanString[69];
				delete []lpszLang;
				SetCurrentDirectory(m_sMyConfig.m_strWorkPath);
			}
			MessageBox(strContent, strTitle, MB_ICONERROR);
		}
		else	// �����ɹ�
		{
			if (m_sMyConfig.m_uiLanguage == 0)
			{
				strContent.LoadString(IDS_MSGBOX_EXPSUC);
				strTitle.LoadString(IDS_MSGBOX_INFO);
			}
			else
			{
				SetCurrentDirectory(m_strAppPath);
				LPTSTR strFileName = NULL;
				if (m_sMyConfig.m_uiLanguage == 1)
					strFileName = LANG_TCHINESE_FILE;
				else if (m_sMyConfig.m_uiLanguage == 2)
					strFileName = LANG_ENGLISH_FILE;
				LPTSTR lpszLang = new _TCHAR[LANG_STRING_LEN];
				GetPrivateProfileString(LANG_MESSAGEBOX,
					_T("CONTENT_EXPORT_SUCCESS"), _T(""),
					lpszLang, LANG_STRING_LEN, strFileName);
				strContent = lpszLang;
				strContent.Replace(_T("\\n"), _T("\n"));
				strTitle = m_arrLanString[67];
				delete []lpszLang;
				SetCurrentDirectory(m_sMyConfig.m_strWorkPath);
			}
			MessageBox(strContent, strTitle, MB_ICONINFORMATION);
		}
	}
	else	// ȡ����ȻҪ�ػ�˵���������
	{
		DrawMenuBar();
	}
}

void CCodeMateDlg::OnFileClose()
{
	if (m_wndCodeEdit.GetChangedStatus() == TRUE)	// ����ļ��Ƿ���Ҫ����
	{
		CString strContent,strTitle;
		if (m_sMyConfig.m_uiLanguage == 0)
		{
			strContent.LoadString(IDS_MSGBOX_EXITWARN);
			strTitle.LoadString(IDS_MSGBOX_QUES);
		}
		else
		{
			SetCurrentDirectory(m_strAppPath);
			LPTSTR strFileName = NULL;
			if (m_sMyConfig.m_uiLanguage == 1)
				strFileName = LANG_TCHINESE_FILE;
			else if (m_sMyConfig.m_uiLanguage == 2)
				strFileName = LANG_ENGLISH_FILE;
			LPTSTR lpszLang = new _TCHAR[LANG_STRING_LEN];
			GetPrivateProfileString(LANG_MESSAGEBOX,
				_T("CONTENT_FILE_SAVE"), _T(""),
				lpszLang, LANG_STRING_LEN, strFileName);
			strContent = lpszLang;
			strContent.Replace(_T("\\n"), _T("\n"));
			strTitle = m_arrLanString[70];
			delete []lpszLang;
			SetCurrentDirectory(m_sMyConfig.m_strWorkPath);
		}
		int iResult = MessageBox(strContent, strTitle,
			MB_ICONQUESTION | MB_YESNOCANCEL);
		if (iResult == IDCANCEL)
			return;	// ȡ������
		if (iResult == IDYES)	// �����ļ�
		{
			if (!m_wndCodeEdit.Save((LPTSTR)((LPCTSTR)m_strFileName)))
			{
				if (m_sMyConfig.m_uiLanguage == 0)
				{
					strContent.LoadString(IDS_MSGBOX_SAVEERROR);
					strTitle.LoadString(IDS_MSGBOX_ERROR);
				}
				else
				{
					SetCurrentDirectory(m_strAppPath);
					LPTSTR strFileName = NULL;
					if (m_sMyConfig.m_uiLanguage == 1)
						strFileName = LANG_TCHINESE_FILE;
					else if (m_sMyConfig.m_uiLanguage == 2)
						strFileName = LANG_ENGLISH_FILE;
					LPTSTR lpszLang = new _TCHAR[LANG_STRING_LEN];
					GetPrivateProfileString(LANG_MESSAGEBOX,
						_T("CONTENT_SAVE_ERROR"), _T(""),
						lpszLang, LANG_STRING_LEN, strFileName);
					strContent = lpszLang;
					strContent.Replace(_T("\\n"), _T("\n"));
					strTitle = m_arrLanString[69];
					delete []lpszLang;
					SetCurrentDirectory(m_sMyConfig.m_strWorkPath);
				}
				MessageBox(strContent, strTitle, MB_ICONERROR);
				return;
			}
		}
	}
	// ��ձ༭��
	m_wndCodeEdit.ClearText();
	// ���ø��ı�־
	m_wndCodeEdit.SetChangedStatus(FALSE);
	// ���ر༭�����з���
	m_wndCodeEdit.ShowWindow(SW_HIDE);
	m_wndSplitter.ShowWindow(SW_HIDE);
	// û�д��ļ���־
	m_enFileType = SCT_ERROR;
	// ��ǰѡ��ڵ�Ϊ��
	m_hSelected = NULL;
	// ѡ����ڵ�
	m_wndTreeView.SelectItem(m_hRoot);
	// ״̬������
	CString strLoad;
	if (m_sMyConfig.m_uiLanguage == 0)
		strLoad.LoadString(IDS_INDICATOR_FILE);
	else
		strLoad = m_arrLanString[54];
	m_wndStatusBar.SetPaneText(2, strLoad);
	// ��������ԭ
	SetWindowText(m_strAppTitle);
}

void CCodeMateDlg::OnFileAdditem()
{
	// �����б��
	m_wndCodeEdit.DestroyList();
	CAddItemDlg dlgAddItem;
	// ��ȡ��ǰѡ����Ŀ
	HTREEITEM hCurrent = m_wndTreeView.GetSelectedItem();
	if (!hCurrent)
		hCurrent = m_hRoot;

	// ��ȡ��Ŀͼ�꣬�ж���Ŀ����
	int iImage, iImageSel;
	m_wndTreeView.GetItemImage(hCurrent, iImage, iImageSel);
	if (iImage != 3 && iImage != 2) // �����ļ���
		hCurrent = m_wndTreeView.GetParentItem(hCurrent);
	// ���屣���½���Ŀָ��ı���
	HTREEITEM hNew=NULL;
	// Ϊ�Ի������ʾ��ʵ��·��ת��Ϊ����·��
	CString strConv, strPath, strClass;
	if (m_sMyConfig.m_uiLanguage == 0)
		strConv.LoadString(IDS_CLASS_NORMAL);
	else
		strConv = m_arrLanString[64];
	if (hCurrent != m_hRoot)
	{
		strClass = ItemToPath(hCurrent);
		strPath = strClass;
		strPath = strPath.Mid(7);
		strPath = strConv + _T("\\") + strPath;
	}
	else
	{
		strClass = _T(".\\Code");
		strPath=strConv + _T("\\");
	}
	// �Ի��������ʼ��
	dlgAddItem.m_strAppPath = m_strAppPath;
	dlgAddItem.m_strWorkPath = m_sMyConfig.m_strWorkPath;
	dlgAddItem.m_strClass = strPath;
	dlgAddItem.m_strFullPath = strClass;
	dlgAddItem.m_uiLangType = m_sMyConfig.m_uiDefType;
	dlgAddItem.m_uiLanguage = m_sMyConfig.m_uiLanguage;
	if (dlgAddItem.DoModal()==IDOK)
	{
		// ����ļ��Ƿ���Ҫ����
		if (m_wndCodeEdit.GetChangedStatus() == TRUE)
		{
			CString strContent,strTitle;
			if (m_sMyConfig.m_uiLanguage == 0)
			{
				strContent.LoadString(IDS_MSGBOX_EXITWARN);
				strTitle.LoadString(IDS_MSGBOX_QUES);
			}
			else
			{
				SetCurrentDirectory(m_strAppPath);
				LPTSTR strFileName = NULL;
				if (m_sMyConfig.m_uiLanguage == 1)
					strFileName = LANG_TCHINESE_FILE;
				else if (m_sMyConfig.m_uiLanguage == 2)
					strFileName = LANG_ENGLISH_FILE;
				LPTSTR lpszLang = new _TCHAR[LANG_STRING_LEN];
				GetPrivateProfileString(LANG_MESSAGEBOX,
					_T("CONTENT_FILE_SAVE"), _T(""),
					lpszLang, LANG_STRING_LEN, strFileName);
				strContent = lpszLang;
				strContent.Replace(_T("\\n"), _T("\n"));
				strTitle = m_arrLanString[70];
				delete []lpszLang;
				SetCurrentDirectory(m_sMyConfig.m_strWorkPath);
			}
			int iResult = MessageBox(strContent, strTitle,
				MB_ICONQUESTION | MB_YESNOCANCEL);
			if (iResult == IDCANCEL)
				return;	// ȡ������
			if (iResult == IDYES)	// �����ļ�
			{
				if (!m_wndCodeEdit.Save((LPTSTR)((LPCTSTR)m_strFileName)))
				{
					if (m_sMyConfig.m_uiLanguage == 0)
					{
						strContent.LoadString(IDS_MSGBOX_SAVEERROR);
						strTitle.LoadString(IDS_MSGBOX_ERROR);
					}
					else
					{
						SetCurrentDirectory(m_strAppPath);
						LPTSTR strFileName = NULL;
						if (m_sMyConfig.m_uiLanguage == 1)
							strFileName = LANG_TCHINESE_FILE;
						else if (m_sMyConfig.m_uiLanguage == 2)
							strFileName = LANG_ENGLISH_FILE;
						LPTSTR lpszLang = new _TCHAR[LANG_STRING_LEN];
						GetPrivateProfileString(LANG_MESSAGEBOX,
							_T("CONTENT_SAVE_ERROR"), _T(""), lpszLang,
							LANG_STRING_LEN, strFileName);
						strContent = lpszLang;
						strContent.Replace(_T("\\n"), _T("\n"));
						strTitle = m_arrLanString[69];
						delete []lpszLang;
						SetCurrentDirectory(m_sMyConfig.m_strWorkPath);
					}
					MessageBox(strContent, strTitle, MB_ICONERROR);
					return;
				}
			}
		}
		if (dlgAddItem.m_bSaveAsDefault == true)
		{
			// ����Ĭ������
			m_sMyConfig.m_uiDefType = dlgAddItem.m_uiLangType;
		}
		// ���·���Ѿ��ı���
		if (dlgAddItem.m_bReConfirm == true)
		{
			// �ҵ���·����Ӧ����Ŀ
			hCurrent = PathToItem(dlgAddItem.m_strFullPath);
			strClass = dlgAddItem.m_strFullPath;
		}
		// ���ݵõ�����Ϣת��Ϊȫ·������������Ŀ
		if (strClass.Right(1) != _T("\\")) strClass += _T("\\");
		CString strFilePath;
		CString strFileExt,strDefExt;
		UINT uiSourceType = dlgAddItem.m_uiSourceType;
		UINT uiLangType = dlgAddItem.m_uiLangType;
		strFilePath = strClass + _T("\\") + dlgAddItem.m_strTitle;
		// ������Ŀ����ȷ����չ�����ļ������б���Ŀͼ�����Ϣ
		switch(uiLangType)
		{
		case 0:
			{
				strFilePath += _T(".c-p");
				strDefExt = _T("CPP");
				strFileExt = _T("C++ �����ļ� (*.cpp)|*.cpp|C++ ͷ�ļ� (*.h)|*.h|�����ļ� (*.*)|*.*||");
				hNew=m_wndTreeView.InsertItem(dlgAddItem.m_strTitle,1,0,hCurrent);
			}
			break;
		case 1:
			{
				strFilePath += _T(".c-a");
				strDefExt = _T("C");
				strFileExt = _T("C �����ļ� (*.c)|*.c|C ͷ�ļ� (*.h)|*.h|�����ļ� (*.*)|*.*||");
				hNew=m_wndTreeView.InsertItem(dlgAddItem.m_strTitle,5,4,hCurrent);
			}
			break;
		case 2:
			{
				strFilePath += _T(".c-s");
				strDefExt = _T("CS");
				strFileExt = _T("C# �����ļ� (*.cs)|*.cs|�����ļ� (*.*)|*.*||");
				hNew=m_wndTreeView.InsertItem(dlgAddItem.m_strTitle,17,16,hCurrent);
			}
			break;
		case 3:
			{
				strFilePath += _T(".h-t");
				strDefExt = _T("HTM");
				strFileExt = _T("HTM �ļ� (*.htm)|*.htm|HTML �ļ� (*.html)|*.html|�����ļ� (*.*)|*.*||");
				hNew=m_wndTreeView.InsertItem(dlgAddItem.m_strTitle,7,6,hCurrent);
			}
			break;
		case 4:
			{
				strFilePath += _T(".j-s");
				strDefExt = _T("JS");
				strFileExt = _T("Java Script �����ļ� (*.js)|*.js|�����ļ� (*.*)|*.*||");
				hNew=m_wndTreeView.InsertItem(dlgAddItem.m_strTitle,9,8,hCurrent);
			}
			break;
		case 5:
			{
				strFilePath += _T(".v-b");
				strDefExt = _T("VBS");
				strFileExt = _T("JVB Script �����ļ� (*.vbs)|*.vbs|�����ļ� (*.*)|*.*||");
				hNew=m_wndTreeView.InsertItem(dlgAddItem.m_strTitle,15,14,hCurrent);
			}
			break;
		case 6:
			{
				strFilePath += _T(".s-q");
				strDefExt = _T("SQL");
				strFileExt = _T("SQL �����ļ� (*.sql)|*.sql|�����ļ� (*.*)|*.*||");
				hNew=m_wndTreeView.InsertItem(dlgAddItem.m_strTitle,11,10,hCurrent);
			}
			break;
		case 7:
			{
				strFilePath += _T(".txt");
				strDefExt = _T("TXT");
				strFileExt = _T("�ı��ĵ� (*.txt)|*.txt|�����ļ� (*.*)|*.*||");
				hNew=m_wndTreeView.InsertItem(dlgAddItem.m_strTitle,13,12,hCurrent);
			}
			break;

		}
		// ���ݴ�����ʽ
		switch (uiSourceType)
		{
		case 0:
			{
				if (m_enFileType != SCT_ERROR) m_wndCodeEdit.ClearText();	// ��ձ༭��
			}
			break;
		case 1:
			{
				// ��ձ༭�����Ӽ�����ճ����Ŀ
				if (m_enFileType != SCT_ERROR) m_wndCodeEdit.ClearText();
				m_wndCodeEdit.OnChar(0x16, 0, 0);
			}
			break;
		case 2:
			{
				// ��ʾ�����ļ��Ի���
				CFileDialog dlgImport( TRUE, strDefExt, dlgAddItem.m_strTitle,
					OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST,
					strFileExt,NULL);
				if(dlgImport.DoModal()==IDOK)
				{
					CString ExportFile = dlgImport.GetPathName();
					if (!m_wndCodeEdit.Load((LPTSTR)((LPCTSTR)ExportFile)))
					{
						CString strContent,strTitle;
						if (m_sMyConfig.m_uiLanguage == 0)
						{
							strContent.LoadString(IDS_MSGBOX_LOADERROR);
							strTitle.LoadString(IDS_MSGBOX_ERROR);
						}
						else
						{
							SetCurrentDirectory(m_strAppPath);
							LPTSTR strFileName = NULL;
							if (m_sMyConfig.m_uiLanguage == 1)
								strFileName = LANG_TCHINESE_FILE;
							else if (m_sMyConfig.m_uiLanguage == 2)
								strFileName = LANG_ENGLISH_FILE;
							LPTSTR lpszLang = new _TCHAR[LANG_STRING_LEN];
							GetPrivateProfileString(LANG_MESSAGEBOX,
								_T("CONTENT_READ_ERROR"), _T(""),
								lpszLang, LANG_STRING_LEN, strFileName);
							strContent = lpszLang;
							strContent.Replace(_T("\\n"), _T("\n"));
							strTitle = m_arrLanString[69];
							delete []lpszLang;
							SetCurrentDirectory(m_sMyConfig.m_strWorkPath);
						}
						MessageBox(strContent, strTitle, MB_ICONERROR);
						m_wndTreeView.DeleteItem(hNew);
						return;
					}
				}
				else	// ������¡�ȡ����
				{
					m_wndTreeView.DeleteItem(hNew);
					return;
				}
			}
			break;
		}
		m_strFileName = strFilePath;	// ���浱ǰ�ļ�����ȫ·����
		// ���浽���ļ�
		if (!m_wndCodeEdit.Save((LPTSTR)((LPCTSTR)m_strFileName)))
		{
			CString strContent,strTitle;
			if (m_sMyConfig.m_uiLanguage == 0)
			{
				strContent.LoadString(IDS_MSGBOX_SAVEERROR);
				strTitle.LoadString(IDS_MSGBOX_ERROR);
			}
			else
			{
				SetCurrentDirectory(m_strAppPath);
				LPTSTR strFileName = NULL;
				if (m_sMyConfig.m_uiLanguage == 1)
					strFileName = LANG_TCHINESE_FILE;
				else if (m_sMyConfig.m_uiLanguage == 2)
					strFileName = LANG_ENGLISH_FILE;
				LPTSTR lpszLang = new _TCHAR[LANG_STRING_LEN];
				GetPrivateProfileString(LANG_MESSAGEBOX,
					_T("CONTENT_SAVE_ERROR"), _T(""),
					lpszLang, LANG_STRING_LEN, strFileName);
				strContent = lpszLang;
				strContent.Replace(_T("\\n"), _T("\n"));
				strTitle = m_arrLanString[69];
				delete []lpszLang;
				SetCurrentDirectory(m_sMyConfig.m_strWorkPath);
			}
			MessageBox(strContent, strTitle, MB_ICONERROR);
			m_wndTreeView.DeleteItem(hNew);
			return;
		}
		// ѡ���½��ļ���������꽹���л����༭��
		m_wndTreeView.SelectItem(hNew);
		m_wndCodeEdit.SetFocus();
	}
}

void CCodeMateDlg::OnFileAddclass()
{
	// ��õ�ǰѡ����Ŀ
	HTREEITEM hCurrent = m_wndTreeView.GetSelectedItem();
	if (!hCurrent)
		hCurrent = m_hRoot;

	// ��ȡ��Ŀͼ��
	int iImage, iImageSel;
	m_wndTreeView.GetItemImage(hCurrent, iImage, iImageSel);
	CString strClassName;
	if (m_sMyConfig.m_uiLanguage == 0)
		strClassName.LoadString(IDS_NEW_CLASS);
	else
		strClassName = m_arrLanString[66];
	if (iImage != 3 && iImage != 2) // �����ļ���
		hCurrent = m_wndTreeView.GetParentItem(hCurrent);
	// �����·��࣬��������ָ��
	HTREEITEM hNew = m_wndTreeView.InsertItem(strClassName, 3, 3, hCurrent);
	m_wndTreeView.SelectItem(hNew);
	// ���ò�������Ϊ�½����ࣨ1��
	m_usOperaType = 1;
	m_wndTreeView.EditLabel(hNew);// ��ǰ������༭״̬
}

void CCodeMateDlg::OnRenameItem()
{
	m_usOperaType = 0;	// ���ò�������Ϊ��������0������Ҳ��Ĭ�ϵķ�ʽ
	m_wndTreeView.EditLabel(m_wndTreeView.GetSelectedItem());// ��ǰ������༭״̬
}

void CCodeMateDlg::OnDeleteItem()
{
	// �����б��
	m_wndCodeEdit.DestroyList();
	CString strTitle,strContent;
	if (m_bIsFolder)	// ������ļ���
	{
		if (m_sMyConfig.m_uiLanguage == 0)
		{
			strTitle.LoadString(IDS_MSGBOX_QUES);
			strContent.LoadString(IDS_MSGBOX_DELCLASS);
		}
		else
		{
			SetCurrentDirectory(m_strAppPath);
			LPTSTR strFileName = NULL;
			if (m_sMyConfig.m_uiLanguage == 1)
				strFileName = LANG_TCHINESE_FILE;
			else if (m_sMyConfig.m_uiLanguage == 2)
				strFileName = LANG_ENGLISH_FILE;
			LPTSTR lpszLang = new _TCHAR[LANG_STRING_LEN];
			GetPrivateProfileString(LANG_MESSAGEBOX,
				_T("CONTENT_DELETE_CLASS"), _T(""),
				lpszLang, LANG_STRING_LEN, strFileName);
			strContent = lpszLang;
			strContent.Replace(_T("\\n"), _T("\n"));
			strTitle = m_arrLanString[70];
			delete []lpszLang;
			SetCurrentDirectory(m_sMyConfig.m_strWorkPath);
		}
		if (MessageBox(strContent, strTitle,
			MB_ICONQUESTION + MB_YESNO + MB_DEFBUTTON2)
			== IDYES)
		{
			// ��õ�ǰѡ����Ŀ
			HTREEITEM hCurrent = m_wndTreeView.GetSelectedItem();
			if (!hCurrent)
				hCurrent = m_hRoot;

			CString strPathname = ItemToPath(hCurrent);
			strPathname += _T("\\");
			if ( m_strFileName.Find(strPathname) != -1)		// �����ǰ�򿪵��ļ�λ��Ҫɾ���ķ���֮��
			{
				if (m_sMyConfig.m_uiLanguage == 0)
				{
					strTitle.LoadString(IDS_MSGBOX_WARN);
					strContent.LoadString(IDS_MSGBOX_INCLASS);
				}
				else
				{
					SetCurrentDirectory(m_strAppPath);
					LPTSTR strFileName = NULL;
					if (m_sMyConfig.m_uiLanguage == 1)
						strFileName = LANG_TCHINESE_FILE;
					else if (m_sMyConfig.m_uiLanguage == 2)
						strFileName = LANG_ENGLISH_FILE;
					LPTSTR lpszLang = new _TCHAR[LANG_STRING_LEN];
					GetPrivateProfileString(LANG_MESSAGEBOX,
						_T("CONTENT_IN_THE_CLASS"), _T(""),
						lpszLang, LANG_STRING_LEN, strFileName);
					strContent = lpszLang;
					strContent.Replace(_T("\\n"), _T("\n"));
					strTitle = m_arrLanString[68];
					delete []lpszLang;
					SetCurrentDirectory(m_sMyConfig.m_strWorkPath);
				}
				if (MessageBox(strContent, strTitle,
					MB_ICONWARNING + MB_YESNO + MB_DEFBUTTON2)
					== IDNO)
				{
					// ȡ���Է����ɾ������
					return;
				}
				else	// ������Ҫɾ������
				{
					m_wndCodeEdit.ClearText();	// ����ı�
					m_wndCodeEdit.SetChangedStatus(FALSE);	// ���øĶ���־
					m_enFileType = SCT_ERROR;	// �ļ�δ��
					m_wndCodeEdit.ShowWindow(SW_HIDE);	// ���ر༭��
					m_wndSplitter.ShowWindow(SW_HIDE);	// �����з���
					SetWindowText(m_strAppTitle);
					m_hSelected = NULL;	// ��ǰѡ��ڵ�Ϊ��
					m_wndTreeView.SelectItem(m_hRoot);	// ѡ����ڵ�
					// ״̬������
					CString strLoad;
					if (m_sMyConfig.m_uiLanguage == 0)
						strLoad.LoadString(IDS_INDICATOR_FILE);
					else
						strLoad = m_arrLanString[54];
					m_wndStatusBar.SetPaneText(2, strLoad);
				}
			}
			if (DeleteDirectory(strPathname))	// ɾ���ɹ�
			{
				m_wndTreeView.DeleteItem(hCurrent);// ɾ����Ľ��
			}
			else	// ɾ��ʧ��
			{
				if (m_sMyConfig.m_uiLanguage == 0)
				{
					strTitle.LoadString(IDS_MSGBOX_ERROR);
					strContent.LoadString(IDS_MSGBOX_DELERROR);
				}
				else
				{
					SetCurrentDirectory(m_strAppPath);
					LPTSTR strFileName = NULL;
					if (m_sMyConfig.m_uiLanguage == 1)
						strFileName = LANG_TCHINESE_FILE;
					else if (m_sMyConfig.m_uiLanguage == 2)
						strFileName = LANG_ENGLISH_FILE;
					LPTSTR lpszLang = new _TCHAR[LANG_STRING_LEN];
					GetPrivateProfileString(LANG_MESSAGEBOX,
						_T("CONTENT_DELETE_ERROR"), _T(""),
						lpszLang, LANG_STRING_LEN, strFileName);
					strContent = lpszLang;
					strContent.Replace(_T("\\n"), _T("\n"));
					strTitle = m_arrLanString[69];
					delete []lpszLang;
					SetCurrentDirectory(m_sMyConfig.m_strWorkPath);
				}
				MessageBox(strContent, strTitle, MB_ICONERROR);
			}
		}
	}
	else	// �������ͨ�ļ�
	{
		if (m_sMyConfig.m_uiLanguage == 0)
		{
			strTitle.LoadString(IDS_MSGBOX_QUES);
			strContent.LoadString(IDS_MSGBOX_DELITEM);
		}
		else
		{
			SetCurrentDirectory(m_strAppPath);
			LPTSTR strFileName = NULL;
			if (m_sMyConfig.m_uiLanguage == 1)
				strFileName = LANG_TCHINESE_FILE;
			else if (m_sMyConfig.m_uiLanguage == 2)
				strFileName = LANG_ENGLISH_FILE;
			LPTSTR lpszLang = new _TCHAR[LANG_STRING_LEN];
			GetPrivateProfileString(LANG_MESSAGEBOX,
				_T("CONTENT_DELETE_ITEM"), _T(""),
				lpszLang, LANG_STRING_LEN, strFileName);
			strContent = lpszLang;
			strContent.Replace(_T("\\n"), _T("\n"));
			strTitle = m_arrLanString[70];
			delete []lpszLang;
			SetCurrentDirectory(m_sMyConfig.m_strWorkPath);
		}
		if (MessageBox(strContent, strTitle,
			MB_ICONQUESTION + MB_YESNO + MB_DEFBUTTON2)
			== IDYES)
		{
			// ��õ�ǰѡ����Ŀ
			HTREEITEM hCurrent= m_wndTreeView.GetSelectedItem();
			if (!hCurrent)
				hCurrent = m_hRoot;

			// ��õ�ǰ��Ŀȫ·��
			CString strPath = ItemToPath(hCurrent);
			// ��õ�ǰ��Ŀͼ�꣬���ݴ��ж���Ŀ����
			int iImage, iImageSel;
			m_wndTreeView.GetItemImage(hCurrent, iImage, iImageSel);
			switch (iImage)
			{
			case 1:	// C++
				strPath += _T(".c-p");
				break;
			case 5:	// C
				strPath += _T(".c-a");
				break;
			case 7:	// HTM
				strPath += _T(".h-t");
				break;
			case 9:	// JScript
				strPath += _T(".j-s");
				break;
			case 11:	// SQL
				strPath += _T(".s-q");
				break;
			case 13:	// TXT
				strPath += _T(".txt");
				break;
			case 15:	// VBScript
				strPath += _T(".v-b");
				break;
			case 17:	// C#
				strPath += _T(".c-s");
				break;
			}
			if (DeleteFile(strPath))	// ɾ���ɹ�
			{
				m_wndCodeEdit.ClearText();
				m_wndCodeEdit.SetChangedStatus(FALSE);
				m_enFileType = SCT_ERROR;
				m_wndCodeEdit.ShowWindow(SW_HIDE);
				m_wndSplitter.ShowWindow(SW_HIDE);
				SetWindowText(m_strAppTitle);
				m_hSelected = NULL;	// ��ǰѡ��ڵ�Ϊ��
				m_wndTreeView.SelectItem(m_hRoot);	// ѡ����ڵ�
				// ״̬������
				CString strLoad;
				if (m_sMyConfig.m_uiLanguage == 0)
					strLoad.LoadString(IDS_INDICATOR_FILE);
				else
					strLoad = m_arrLanString[54];
				m_wndStatusBar.SetPaneText(2, strLoad);
				m_wndTreeView.DeleteItem(hCurrent);// ɾ����Ľ��
			}
			else	// ɾ��ʧ��
			{
				if (m_sMyConfig.m_uiLanguage == 0)
				{
					strTitle.LoadString(IDS_MSGBOX_ERROR);
					strContent.LoadString(IDS_MSGBOX_DELERROR);
				}
				else
				{
					SetCurrentDirectory(m_strAppPath);
					LPTSTR strFileName = NULL;
					if (m_sMyConfig.m_uiLanguage == 1)
						strFileName = LANG_TCHINESE_FILE;
					else if (m_sMyConfig.m_uiLanguage == 2)
						strFileName = LANG_ENGLISH_FILE;
					LPTSTR lpszLang = new _TCHAR[LANG_STRING_LEN];
					GetPrivateProfileString(LANG_MESSAGEBOX,
						_T("CONTENT_DELETE_ERROR"), _T(""),
						lpszLang, LANG_STRING_LEN, strFileName);
					strContent = lpszLang;
					strContent.Replace(_T("\\n"), _T("\n"));
					strTitle = m_arrLanString[69];
					delete []lpszLang;
					SetCurrentDirectory(m_sMyConfig.m_strWorkPath);
				}
				MessageBox(strContent, strTitle, MB_ICONERROR);
			}
		}
	}
}

bool CCodeMateDlg::DeleteDirectory(CString strDirName)
{
	// �ַ�������ת��
	_TCHAR *strChar;
	strChar = strDirName.GetBuffer(strDirName.GetLength()+1);
	strChar[_tcslen(strChar)+1] = 0;
	// ����ṹ������ռ�
	SHFILEOPSTRUCT sfOpera;
	ZeroMemory((void*)&sfOpera, sizeof(SHFILEOPSTRUCT));
	// ���ò�����ɾ��
	sfOpera.fFlags = FOF_SILENT | FOF_NOCONFIRMATION;
	sfOpera.hNameMappings = NULL;
	sfOpera.hwnd = NULL;
	sfOpera.lpszProgressTitle = NULL;
	sfOpera.pFrom = strChar;
	sfOpera.pTo = NULL;
	sfOpera.wFunc = FO_DELETE;
	// ����API���������ؽ��
	return SHFileOperation(&sfOpera) == 0;
}

bool CCodeMateDlg::ReNameFolder(CString strFromPath, CString strToPath)
{
	// �ַ�������ת��
	_TCHAR *strChar1,*strChar2;
	strChar1 = strFromPath.GetBuffer(strFromPath.GetLength()+1);
	strChar1[_tcslen(strChar1)+1] = 0;
	strChar2 = strToPath.GetBuffer(strToPath.GetLength()+1);
	strChar2[_tcslen(strChar2)+1] = 0;
	// ����ṹ������ռ�
	SHFILEOPSTRUCT sfOpera;
	ZeroMemory((void*)&sfOpera, sizeof(SHFILEOPSTRUCT));
	// ���ò�����������
	sfOpera.fFlags = FOF_SILENT | FOF_NOCONFIRMATION ;
	sfOpera.hNameMappings = NULL;
	sfOpera.hwnd = NULL;
	sfOpera.lpszProgressTitle = NULL;
	sfOpera.pFrom = strFromPath;
	sfOpera.pTo = strToPath;
	sfOpera.wFunc = FO_RENAME;
	// ����API���������ؽ��
	return SHFileOperation(&sfOpera) == 0;
}

bool CCodeMateDlg::MoveFolder(CString strFromPath, CString strToPath)
{
	// �ַ�������ת��
	_TCHAR *strChar1,*strChar2;
	strChar1=strFromPath.GetBuffer(strFromPath.GetLength()+1);
	strChar1[_tcslen(strChar1)+1] = 0;
	strChar2=strToPath.GetBuffer(strToPath.GetLength()+1);
	strChar2[_tcslen(strChar2)+1] = 0;
	// ����ṹ������ռ�
	SHFILEOPSTRUCT sfOpera;
	ZeroMemory((void*)&sfOpera, sizeof(SHFILEOPSTRUCT));
	// ���ò������ƶ�
	sfOpera.fFlags = FOF_SILENT | FOF_NOCONFIRMATION ;
	sfOpera.hNameMappings = NULL;
	sfOpera.hwnd = NULL;
	sfOpera.lpszProgressTitle = NULL;
	sfOpera.pFrom = strFromPath;
	sfOpera.pTo = strToPath;
	sfOpera.wFunc = FO_MOVE;
	// ����API���������ؽ��
	return SHFileOperation(&sfOpera) == 0;
}

void CCodeMateDlg::OnClearClass()
{
	CString strTitle,strContent;
	if (m_sMyConfig.m_uiLanguage == 0)
	{
		strTitle.LoadString(IDS_MSGBOX_WARN);
		strContent.LoadString(IDS_MSGBOX_CLEARCLASS);
	}
	else
	{
		SetCurrentDirectory(m_strAppPath);
		LPTSTR strFileName = NULL;
		if (m_sMyConfig.m_uiLanguage == 1)
			strFileName = LANG_TCHINESE_FILE;
		else if (m_sMyConfig.m_uiLanguage == 2)
			strFileName = LANG_ENGLISH_FILE;
		LPTSTR lpszLang = new _TCHAR[LANG_STRING_LEN];
		GetPrivateProfileString(LANG_MESSAGEBOX,
			_T("CONTENT_CLEAR_ALL_CLASS"), _T(""),
			lpszLang, LANG_STRING_LEN, strFileName);
		strContent = lpszLang;
		strContent.Replace(_T("\\n"), _T("\n"));
		strTitle = m_arrLanString[68];
		delete []lpszLang;
		SetCurrentDirectory(m_sMyConfig.m_strWorkPath);
	}
	if (MessageBox(strContent, strTitle,
		MB_ICONWARNING + MB_OKCANCEL + MB_DEFBUTTON2)
		== IDOK)
	{
		if (DeleteDirectory(_T(".\\Code")))	// ��շ���ɹ�
		{
			m_wndTreeView.DeleteAllItems();
			if ( CreateDirectory(_T(".\\Code"), NULL))	// ���½����ļ���
			{
				CString strLoad;
				if (m_sMyConfig.m_uiLanguage == 0)
					strLoad.LoadString(IDS_CLASS_NORMAL);
				else
					strLoad = m_arrLanString[64];
				m_hRoot = m_wndTreeView.InsertItem(strLoad,3,3,TVI_ROOT);// ��Ӹ����
				m_wndTreeView.SelectItem(m_hRoot);
			}
			else
				AfxMessageBox(_T("��������δ�ܴ������ࡣ"), MB_ICONERROR);
		}
		else	// ʧ��
		{
			if (m_sMyConfig.m_uiLanguage == 0)
			{
				strTitle.LoadString(IDS_MSGBOX_ERROR);
				strContent.LoadString(IDS_MSGBOX_DELERROR);
			}
			else
			{
				SetCurrentDirectory(m_strAppPath);
				LPTSTR strFileName = NULL;
				if (m_sMyConfig.m_uiLanguage == 1)
					strFileName = LANG_TCHINESE_FILE;
				else if (m_sMyConfig.m_uiLanguage == 2)
					strFileName = LANG_ENGLISH_FILE;
				LPTSTR lpszLang = new _TCHAR[LANG_STRING_LEN];
				GetPrivateProfileString(LANG_MESSAGEBOX,
					_T("CONTENT_DELETE_ERROR"), _T(""),
					lpszLang, LANG_STRING_LEN, strFileName);
				strContent = lpszLang;
				strContent.Replace(_T("\\n"), _T("\n"));
				strTitle = m_arrLanString[69];
				delete []lpszLang;
				SetCurrentDirectory(m_sMyConfig.m_strWorkPath);
			}
			MessageBox(strContent, strTitle, MB_ICONERROR);
		}
	}
}

void CCodeMateDlg::OnFileMoveto()
{
	CMoveToDlg dlgMoveTo;
	dlgMoveTo.m_uiLanguage = m_sMyConfig.m_uiLanguage;
	dlgMoveTo.m_strAppPath = m_strAppPath;
	dlgMoveTo.m_strWorkPath = m_sMyConfig.m_strWorkPath;
	if (dlgMoveTo.DoModal()==IDOK)
	{
		// ��õ�ǰѡ����Ŀ
		HTREEITEM hCurrent = m_wndTreeView.GetSelectedItem();
		if (!hCurrent)
			hCurrent = m_hRoot;

		// ��õ�ǰ��Ŀ����
		CString strCurTitle = m_wndTreeView.GetItemText(hCurrent);
		CString strToPath = dlgMoveTo.FullPath;				// �ƶ�����Ŀ��·��
		CString strFromPath = ItemToPath(hCurrent);	// �ƶ�����ԭ·��
		if (m_bIsFolder)	// ����Ƿ���
		{
			strFromPath += _T("\\");
			if (strToPath.Find(strFromPath) != -1)	// ���Ҫ�������ƶ��������ӷ��࣬�򱨴�
			{
				CString strContent,strTitle;
				if (m_sMyConfig.m_uiLanguage == 0)
				{
					strContent.LoadString(IDS_MSGBOX_MOVEERR);
					strTitle.LoadString(IDS_MSGBOX_WARN);
				}
				else
				{
					SetCurrentDirectory(m_strAppPath);
					LPTSTR strFileName = NULL;
					if (m_sMyConfig.m_uiLanguage == 1)
						strFileName = LANG_TCHINESE_FILE;
					else if (m_sMyConfig.m_uiLanguage == 2)
						strFileName = LANG_ENGLISH_FILE;
					LPTSTR lpszLang = new _TCHAR[LANG_STRING_LEN];
					GetPrivateProfileString(LANG_MESSAGEBOX,
						_T("CONTENT_MOVE_ERROR"), _T(""),
						lpszLang, LANG_STRING_LEN, strFileName);
					strContent = lpszLang;
					strContent.Replace(_T("\\n"), _T("\n"));
					strTitle = m_arrLanString[68];
					delete []lpszLang;
					SetCurrentDirectory(m_sMyConfig.m_strWorkPath);
				}
				MessageBox(strContent, strTitle, MB_ICONWARNING);
				return;
			}
			// ����Ƿ����ƶ�
			CFileFind cfFile;
			if (strToPath.Right(1) != _T("\\")) strToPath += _T("\\");
			CString strTarget = strToPath + strCurTitle;
			if(cfFile.FindFile(strTarget))	// ���Ŀ��·���Ѵ���ͬ�����࣬����
			{
				CString strContent,strTitle;
				if (m_sMyConfig.m_uiLanguage == 0)
				{
					strTitle.LoadString(IDS_MSGBOX_WARN);
					strContent.LoadString(IDS_MSGBOX_FILEEXIST);
				}
				else
				{
					SetCurrentDirectory(m_strAppPath);
					LPTSTR strFileName = NULL;
					if (m_sMyConfig.m_uiLanguage == 1)
						strFileName = LANG_TCHINESE_FILE;
					else if (m_sMyConfig.m_uiLanguage == 2)
						strFileName = LANG_ENGLISH_FILE;
					LPTSTR lpszLang = new _TCHAR[LANG_STRING_LEN];
					GetPrivateProfileString(LANG_MESSAGEBOX,
						_T("CONTENT_FILE_EXIST"), _T(""),
						lpszLang, LANG_STRING_LEN, strFileName);
					strContent = lpszLang;
					strContent.Replace(_T("\\n"), _T("\n"));
					strTitle = m_arrLanString[68];
					delete []lpszLang;
					SetCurrentDirectory(m_sMyConfig.m_strWorkPath);
				}
				MessageBox(strContent, strTitle, MB_ICONWARNING);
				return;
			}
			// ׼���ƶ�
			if (m_strFileName.Find(strFromPath) != -1)	// �����ǰ�򿪵��ļ�����Ҫ�ƶ��ķ���
			{
				CString strTitle,strContent;
				if (m_sMyConfig.m_uiLanguage == 0)
				{
					strTitle.LoadString(IDS_MSGBOX_WARN);
					strContent.LoadString(IDS_MSGBOX_INCLASS);
				}
				else
				{
					SetCurrentDirectory(m_strAppPath);
					LPTSTR strFileName = NULL;
					if (m_sMyConfig.m_uiLanguage == 1)
						strFileName = LANG_TCHINESE_FILE;
					else if (m_sMyConfig.m_uiLanguage == 2)
						strFileName = LANG_ENGLISH_FILE;
					LPTSTR lpszLang = new _TCHAR[LANG_STRING_LEN];
					GetPrivateProfileString(LANG_MESSAGEBOX,
						_T("CONTENT_IN_THE_CLASS"), _T(""),
						lpszLang, LANG_STRING_LEN, strFileName);
					strContent = lpszLang;
					strContent.Replace(_T("\\n"), _T("\n"));
					strTitle = m_arrLanString[68];
					delete []lpszLang;
					SetCurrentDirectory(m_sMyConfig.m_strWorkPath);
				}
				if (MessageBox(strContent, strTitle,
					MB_ICONWARNING + MB_YESNO + MB_DEFBUTTON2)
					==IDNO)
				{
					// ȡ��������ƶ�
					return;
				}
				else	// ���Ҫ�ƶ�����
				{
					if (m_wndCodeEdit.GetChangedStatus() == TRUE)	// �жϵ�ǰ�ļ��Ƿ���Ҫ����
					{
						CString strContent,strTitle;
						if (m_sMyConfig.m_uiLanguage == 0)
						{
							strContent.LoadString(IDS_MSGBOX_EXITWARN);
							strTitle.LoadString(IDS_MSGBOX_QUES);
						}
						else
						{
							SetCurrentDirectory(m_strAppPath);
							LPTSTR strFileName = NULL;
							if (m_sMyConfig.m_uiLanguage == 1)
								strFileName = LANG_TCHINESE_FILE;
							else if (m_sMyConfig.m_uiLanguage == 2)
								strFileName = LANG_ENGLISH_FILE;
							LPTSTR lpszLang = new _TCHAR[LANG_STRING_LEN];
							GetPrivateProfileString(LANG_MESSAGEBOX,
								_T("CONTENT_FILE_SAVE"), _T(""),
								lpszLang, LANG_STRING_LEN, strFileName);
							strContent = lpszLang;
							strContent.Replace(_T("\\n"), _T("\n"));
							strTitle = m_arrLanString[70];
							delete []lpszLang;
							SetCurrentDirectory(m_sMyConfig.m_strWorkPath);
						}
						int iResult = MessageBox(strContent, strTitle,
							MB_ICONQUESTION | MB_YESNOCANCEL);
						if (iResult == IDCANCEL)
							return;	// ȡ���ƶ�����
						if (iResult == IDYES)	// �����ļ�
						{
							if (!m_wndCodeEdit.Save((LPTSTR)((LPCTSTR)m_strFileName)))
							{
								if (m_sMyConfig.m_uiLanguage == 0)
								{
									strContent.LoadString(IDS_MSGBOX_SAVEERROR);
									strTitle.LoadString(IDS_MSGBOX_ERROR);
								}
								else
								{
									SetCurrentDirectory(m_strAppPath);
									LPTSTR strFileName = NULL;
									if (m_sMyConfig.m_uiLanguage == 1)
										strFileName = LANG_TCHINESE_FILE;
									else if (m_sMyConfig.m_uiLanguage == 2)
										strFileName = LANG_ENGLISH_FILE;
									LPTSTR lpszLang = new _TCHAR[LANG_STRING_LEN];
									GetPrivateProfileString(LANG_MESSAGEBOX, 
										_T("CONTENT_SAVE_ERROR"), _T(""),
										lpszLang, LANG_STRING_LEN, strFileName);
									strContent = lpszLang;
									strContent.Replace(_T("\\n"), _T("\n"));
									strTitle = m_arrLanString[69];
									delete []lpszLang;
									SetCurrentDirectory(m_sMyConfig.m_strWorkPath);
								}
								MessageBox(strContent, strTitle, MB_ICONERROR);
								return;
							}
						}
					}
					m_wndCodeEdit.ClearText();
					m_wndCodeEdit.SetChangedStatus(FALSE);
					m_enFileType = SCT_ERROR;
					m_wndCodeEdit.ShowWindow(SW_HIDE);
					m_wndSplitter.ShowWindow(SW_HIDE);
					SetWindowText(m_strAppTitle);
					m_hSelected = NULL;	// ��ǰѡ��ڵ�Ϊ��
					m_wndTreeView.SelectItem(m_hRoot);	// ѡ����ڵ�
					// ״̬������
					CString strLoad;
					if (m_sMyConfig.m_uiLanguage == 0)
						strLoad.LoadString(IDS_INDICATOR_FILE);
					else
						strLoad = m_arrLanString[54];
					m_wndStatusBar.SetPaneText(2, strLoad);
				}
			}
			// ��ʼ�ƶ�����
			HTREEITEM hTarget = PathToItem(strToPath);		// ��·��תΪ��Ŀ
			if (hTarget != NULL)	// ���·����Ч
			{
				if (!MoveFolder(strFromPath, strToPath))	// ����ƶ�ʧ��
				{
					CString strContent,strTitle;
					if (m_sMyConfig.m_uiLanguage == 0)
					{
						strTitle.LoadString(IDS_MSGBOX_ERROR);
						strContent.LoadString(IDS_MSGBOX_MOVETOERR);
					}
					else
					{
						SetCurrentDirectory(m_strAppPath);
						LPTSTR strFileName = NULL;
						if (m_sMyConfig.m_uiLanguage == 1)
							strFileName = LANG_TCHINESE_FILE;
						else if (m_sMyConfig.m_uiLanguage == 2)
							strFileName = LANG_ENGLISH_FILE;
						LPTSTR lpszLang = new _TCHAR[LANG_STRING_LEN];
						GetPrivateProfileString(LANG_MESSAGEBOX,
							_T("CONTENT_MOVETO_ERROR"), _T(""),
							lpszLang, LANG_STRING_LEN, strFileName);
						strContent = lpszLang;
						strContent.Replace(_T("\\n"), _T("\n"));
						strTitle = m_arrLanString[69];
						delete []lpszLang;
						SetCurrentDirectory(m_sMyConfig.m_strWorkPath);
					}
					MessageBox(strContent, strTitle, MB_ICONERROR);
				}
				else	// �ƶ�����ɹ�
				{
					// �����ƶ���ķ��࣬���������Ŀ
					HTREEITEM hResult = m_wndTreeView.InsertItem(
						strCurTitle, 3, 3, hTarget);
					m_wndTreeView.SelectItem(hResult);
					AddSubItem(hResult);
					// ɾ��ԭ����
					m_wndTreeView.DeleteItem(hCurrent);
					// ��ʾ�ƶ��ɹ�
					CString strTitle,strContent,strFinal;
					if (m_sMyConfig.m_uiLanguage == 0)
					{
						strTitle.LoadString(IDS_MSGBOX_INFO);
						strContent.LoadString(IDS_MSGBOX_MOVEITEM);
					}
					else
					{
						SetCurrentDirectory(m_strAppPath);
						LPTSTR strFileName = NULL;
						if (m_sMyConfig.m_uiLanguage == 1)
							strFileName = LANG_TCHINESE_FILE;
						else if (m_sMyConfig.m_uiLanguage == 2)
							strFileName = LANG_ENGLISH_FILE;
						LPTSTR lpszLang = new _TCHAR[LANG_STRING_LEN];
						GetPrivateProfileString(LANG_MESSAGEBOX,
							_T("CONTENT_MOVETO_SUCCESS"), _T(""),
							lpszLang, LANG_STRING_LEN, strFileName);
						strContent = lpszLang;
						strContent.Replace(_T("\\n"), _T("\n"));
						strTitle = m_arrLanString[67];
						delete []lpszLang;
						SetCurrentDirectory(m_sMyConfig.m_strWorkPath);
					}
					strFinal.Format(strContent,strCurTitle);
					MessageBox(strFinal, strTitle, MB_ICONINFORMATION);
				}
			}
			else	// ·����Ч
			{
				CString strContent,strTitle;
				if (m_sMyConfig.m_uiLanguage == 0)
				{
					strTitle.LoadString(IDS_MSGBOX_ERROR);
					strContent.LoadString(IDS_MSGBOX_PATHNOTEXIST);
				}
				else
				{
					SetCurrentDirectory(m_strAppPath);
					LPTSTR strFileName = NULL;
					if (m_sMyConfig.m_uiLanguage == 1)
						strFileName = LANG_TCHINESE_FILE;
					else if (m_sMyConfig.m_uiLanguage == 2)
						strFileName = LANG_ENGLISH_FILE;
					LPTSTR lpszLang = new _TCHAR[LANG_STRING_LEN];
					GetPrivateProfileString(LANG_MESSAGEBOX,
						_T("CONTENT_PATH_NOT_EXIST"), _T(""),
						lpszLang, LANG_STRING_LEN, strFileName);
					strContent = lpszLang;
					strContent.Replace(_T("\\n"), _T("\n"));
					strTitle = m_arrLanString[69];
					delete []lpszLang;
					SetCurrentDirectory(m_sMyConfig.m_strWorkPath);
				}
				MessageBox(strContent, strTitle, MB_ICONERROR);
			}
		}
		else	// ������ļ�
		{
			// ��ȡ��Ŀͼ�꣬�ж�������
			int image,imagesel;
			m_wndTreeView.GetItemImage(hCurrent, image, imagesel);
			HTREEITEM hTarget = PathToItem(strToPath);	// ·��תΪ��Ŀ
			if (hTarget != NULL)	// ���·����Ч
			{
				if (strToPath.Right(1) != _T("\\"))
					strToPath += _T("\\");
				strToPath += strCurTitle;
				switch (image)
				{
				case 1:	// C++
					strFromPath += _T(".c-p");
					strToPath += _T(".c-p");
					break;
				case 5:	// C
					strFromPath += _T(".c-a");
					strToPath += _T(".c-a");
					break;
				case 7:	// HTM
					strFromPath += _T(".h-t");
					strToPath += _T(".h-t");
					break;
				case 9:	// JScript
					strFromPath += _T(".j-s");
					strToPath += _T(".j-s");
					break;
				case 11:	// SQL
					strFromPath += _T(".s-q");
					strToPath += _T(".s-q");
					break;
				case 13:	// TXT
					strFromPath += _T(".txt");
					strToPath += _T(".txt");
					break;
				case 15:	// VBScript
					strFromPath += _T(".v-b");
					strToPath += _T(".v-b");
					break;
				case 17:	// C#
					strFromPath += _T(".c-s");
					strToPath += _T(".c-s");
					break;
				}
				CFileFind filefind;
				if(filefind.FindFile(strToPath))
				{
					CString strContent,strTitle;
					if (m_sMyConfig.m_uiLanguage == 0)
					{
						strTitle.LoadString(IDS_MSGBOX_WARN);
						strContent.LoadString(IDS_MSGBOX_FILEEXIST);
					}
					else
					{
						SetCurrentDirectory(m_strAppPath);
						LPTSTR strFileName = NULL;
						if (m_sMyConfig.m_uiLanguage == 1)
							strFileName = LANG_TCHINESE_FILE;
						else if (m_sMyConfig.m_uiLanguage == 2)
							strFileName = LANG_ENGLISH_FILE;
						LPTSTR lpszLang = new _TCHAR[LANG_STRING_LEN];
						GetPrivateProfileString(LANG_MESSAGEBOX,
							_T("CONTENT_FILE_EXIST"), _T(""),
							lpszLang, LANG_STRING_LEN, strFileName);
						strContent = lpszLang;
						strContent.Replace(_T("\\n"), _T("\n"));
						strTitle = m_arrLanString[68];
						delete []lpszLang;
						SetCurrentDirectory(m_sMyConfig.m_strWorkPath);
					}
					MessageBox(strContent, strTitle, MB_ICONWARNING);
					return;
				}
				if (!MoveFile(strFromPath, strToPath))	// �ƶ��ļ�ʧ��
				{
					CString strContent,strTitle;
					if (m_sMyConfig.m_uiLanguage == 0)
					{
						strTitle.LoadString(IDS_MSGBOX_ERROR);
						strContent.LoadString(IDS_MSGBOX_MOVETOERR);
					}
					else
					{
						SetCurrentDirectory(m_strAppPath);
						LPTSTR strFileName = NULL;
						if (m_sMyConfig.m_uiLanguage == 1)
							strFileName = LANG_TCHINESE_FILE;
						else if (m_sMyConfig.m_uiLanguage == 2)
							strFileName = LANG_ENGLISH_FILE;
						LPTSTR lpszLang = new _TCHAR[LANG_STRING_LEN];
						GetPrivateProfileString(LANG_MESSAGEBOX,
							_T("CONTENT_MOVETO_ERROR"), _T(""),
							lpszLang, LANG_STRING_LEN, strFileName);
						strContent = lpszLang;
						strContent.Replace(_T("\\n"), _T("\n"));
						strTitle = m_arrLanString[69];
						delete []lpszLang;
						SetCurrentDirectory(m_sMyConfig.m_strWorkPath);
					}
					MessageBox(strContent, strTitle, MB_ICONERROR);
				}
				else	// �ƶ��ɹ�
				{
					// ���µ�ǰ�ĵ��ĸ�����Ϣ
					m_strFileName = strToPath;
					int iLen = strToPath.GetLength();
					strToPath = strToPath.Mid(7, iLen - 11);
					CString strConv;
					if (m_sMyConfig.m_uiLanguage == 0)
						strConv.LoadString(IDS_CLASS_NORMAL);
					else
						strConv = m_arrLanString[64];
					strToPath = strConv + _T("\\") + strToPath;
					m_strTitleShow = strToPath;
					m_hSelected = m_wndTreeView.InsertItem(
						strCurTitle, image, imagesel, hTarget
						);
					m_wndTreeView.SelectItem(m_hSelected);
					// ɾ��ԭ���
					m_wndTreeView.DeleteItem(hCurrent);
					// �ɹ���ʾ
					CString strTitle, strContent, strFinal;
					if (m_sMyConfig.m_uiLanguage == 0)
					{
						strTitle.LoadString(IDS_MSGBOX_INFO);
						strContent.LoadString(IDS_MSGBOX_MOVEITEM);
					}
					else
					{
						SetCurrentDirectory(m_strAppPath);
						LPTSTR strFileName = NULL;
						if (m_sMyConfig.m_uiLanguage == 1)
							strFileName = LANG_TCHINESE_FILE;
						else if (m_sMyConfig.m_uiLanguage == 2)
							strFileName = LANG_ENGLISH_FILE;
						LPTSTR lpszLang = new _TCHAR[LANG_STRING_LEN];
						GetPrivateProfileString(LANG_MESSAGEBOX,
							_T("CONTENT_MOVETO_SUCCESS"), _T(""),
							lpszLang, LANG_STRING_LEN, strFileName);
						strContent = lpszLang;
						strContent.Replace(_T("\\n"), _T("\n"));
						strTitle = m_arrLanString[67];
						delete []lpszLang;
						SetCurrentDirectory(m_sMyConfig.m_strWorkPath);
					}
					strFinal.Format(strContent, strCurTitle);
					MessageBox(strFinal, strTitle, MB_ICONINFORMATION);
				}
			}
			else	// ���·����Ч������
			{
				CString strContent, strTitle;
				if (m_sMyConfig.m_uiLanguage == 0)
				{
					strTitle.LoadString(IDS_MSGBOX_ERROR);
					strContent.LoadString(IDS_MSGBOX_PATHNOTEXIST);
				}
				else
				{
					SetCurrentDirectory(m_strAppPath);
					LPTSTR strFileName = NULL;
					if (m_sMyConfig.m_uiLanguage == 1)
						strFileName = LANG_TCHINESE_FILE;
					else if (m_sMyConfig.m_uiLanguage == 2)
						strFileName = LANG_ENGLISH_FILE;
					LPTSTR lpszLang = new _TCHAR[LANG_STRING_LEN];
					GetPrivateProfileString(LANG_MESSAGEBOX,
						_T("CONTENT_PATH_NOT_EXIST"), _T(""),
						lpszLang, LANG_STRING_LEN, strFileName);
					strContent = lpszLang;
					strContent.Replace(_T("\\n"), _T("\n"));
					strTitle = m_arrLanString[69];
					delete []lpszLang;
					SetCurrentDirectory(m_sMyConfig.m_strWorkPath);
				}
				MessageBox(strContent, strTitle, MB_ICONERROR);
			}

		}

	}
}

HTREEITEM CCodeMateDlg::PathToItem(CString strFullPath)
{
	// ȷ��·���ұ��ǡ�\��
	if (strFullPath.Right(1) != _T("\\")) strFullPath += _T("\\");
	HTREEITEM hCurrent = m_hRoot;
	HTREEITEM hChild = m_wndTreeView.GetChildItem(hCurrent);
	// ����Ч·����ʼ����
	int iStart = 7;
	int iEnd = strFullPath.Find(_T("\\"), iStart);
	while (iEnd != -1)	// ������ҵ���һ��·��������ѭ��
	{
		if (iStart != iEnd)		// ����õ��Ľڵ��ı����Ȳ�Ϊ0
		{
			CString NowTitle = strFullPath.Mid(iStart, iEnd - iStart);
			while (hChild)	// �����ӽڵ�
			{
				CString CurrentText = m_wndTreeView.GetItemText(hChild);
				if (CurrentText == NowTitle)
				{
					m_wndTreeView.Expand(hChild, TVE_EXPAND);	// ����ҵ���ǰ���������Ľ�㣬չ������
					break;	// �Ѿ��ҵ���������ǰѭ�����ص��ⲿѭ��
				}
				hChild = m_wndTreeView.GetNextItem(hChild,TVGN_NEXT);// ��ȡ��һ������
			}
		}
		else	// �ı�����Ϊ0��˵��·�����Ϸ������ؿս��
		{
			return NULL;
		}
		iStart = iEnd + 1;
		iEnd = strFullPath.Find(_T("\\"), iStart);	// ������һ�������ı�
		hCurrent = hChild;
		if (hCurrent == NULL) return NULL;	// ����Ҳ�����Ӧ����Ŀ��˵��·�����Ϸ�
		hChild = m_wndTreeView.GetChildItem(hCurrent);	// Ѱ���ӽڵ�
		if (hChild == NULL && iEnd != -1) return NULL;	// ����ӽڵ�Ϊ�գ���·����ʾ���ӽڵ㲻Ϊ�գ���·�����Ϸ�
	}
	return hCurrent;	// ѭ���������������յõ��Ľ��
}

void CCodeMateDlg::OnFileReload()
{
	if (m_wndCodeEdit.GetChangedStatus() == TRUE)	// ����ļ��Ƿ���Ҫ����
	{
		CString strContent,strTitle;
		if (m_sMyConfig.m_uiLanguage == 0)
		{
			strContent.LoadString(IDS_MSGBOX_EXITWARN);
			strTitle.LoadString(IDS_MSGBOX_QUES);
		}
		else
		{
			SetCurrentDirectory(m_strAppPath);
			LPTSTR strFileName = NULL;
			if (m_sMyConfig.m_uiLanguage == 1)
				strFileName = LANG_TCHINESE_FILE;
			else if (m_sMyConfig.m_uiLanguage == 2)
				strFileName = LANG_ENGLISH_FILE;
			LPTSTR lpszLang = new _TCHAR[LANG_STRING_LEN];
			GetPrivateProfileString(LANG_MESSAGEBOX,
				_T("CONTENT_FILE_SAVE"), _T(""),
				lpszLang, LANG_STRING_LEN, strFileName);
			strContent = lpszLang;
			strContent.Replace(_T("\\n"), _T("\n"));
			strTitle = m_arrLanString[70];
			delete []lpszLang;
			SetCurrentDirectory(m_sMyConfig.m_strWorkPath);
		}
		int iResult = MessageBox(strContent, strTitle,
			MB_ICONQUESTION | MB_YESNOCANCEL);
		if (iResult == IDCANCEL)
			return;	// ȡ������
		if (iResult == IDYES)	// �����ļ�
		{
			if (!m_wndCodeEdit.Save((LPTSTR)((LPCTSTR)m_strFileName)))
			{
				if (m_sMyConfig.m_uiLanguage == 0)
				{
					strContent.LoadString(IDS_MSGBOX_SAVEERROR);
					strTitle.LoadString(IDS_MSGBOX_ERROR);
				}
				else
				{
					SetCurrentDirectory(m_strAppPath);
					LPTSTR strFileName = NULL;
					if (m_sMyConfig.m_uiLanguage == 1)
						strFileName = LANG_TCHINESE_FILE;
					else if (m_sMyConfig.m_uiLanguage == 2)
						strFileName = LANG_ENGLISH_FILE;
					LPTSTR lpszLang = new _TCHAR[LANG_STRING_LEN];
					GetPrivateProfileString(LANG_MESSAGEBOX,
						_T("CONTENT_SAVE_ERROR"), _T(""),
						lpszLang, LANG_STRING_LEN, strFileName);
					strContent = lpszLang;
					strContent.Replace(_T("\\n"), _T("\n"));
					strTitle = m_arrLanString[69];
					delete []lpszLang;
					SetCurrentDirectory(m_sMyConfig.m_strWorkPath);
				}
				MessageBox(strContent, strTitle, MB_ICONERROR);
				return;
			}
		}
	}
	m_wndCodeEdit.ClearText();
	m_wndCodeEdit.SetChangedStatus(FALSE);
	m_wndCodeEdit.ShowWindow(SW_HIDE);
	m_wndSplitter.ShowWindow(SW_HIDE);
	m_enFileType = SCT_ERROR;
	SetWindowText(m_strAppTitle);
	m_hSelected = NULL;	// ��ǰѡ��ڵ�Ϊ��
	// ״̬������
	CString strLoad;
	if (m_sMyConfig.m_uiLanguage == 0)
		strLoad.LoadString(IDS_INDICATOR_FILE);
	else
		strLoad = m_arrLanString[54];
	m_wndStatusBar.SetPaneText(2, strLoad);
	// ������нڵ�
	m_wndTreeView.DeleteAllItems();
	if (m_sMyConfig.m_uiLanguage == 0)
		strLoad.LoadString(IDS_CLASS_NORMAL);
	else
		strLoad = m_arrLanString[64];
	m_hRoot = m_wndTreeView.InsertItem(strLoad,3,3,TVI_ROOT);	// ��Ӹ����
	CString strFolderPath = _T(".\\Code");
	// �ж�·���Ƿ����   
	if ( !PathIsDirectory(strFolderPath))
	{
		if ( CreateDirectory(strFolderPath, NULL))
		{
			if (m_sMyConfig.m_uiLanguage == 0)
				strLoad.LoadString(IDS_ITEM_SHOW);
			else
				strLoad = m_arrLanString[65];
			strFolderPath = strFolderPath + _T("\\") + strLoad;
			CreateDirectory(strFolderPath, NULL);
		}
		else
			AfxMessageBox(_T("��������δ�ܴ������ࡣ"), MB_ICONERROR);
	}
	GetMainClass(m_hRoot, _T(".\\Code\\*.*"));
	GetClassDir(m_hRoot);
	m_wndTreeView.Expand(m_hRoot,TVE_EXPAND);	// ֻչ�����ڵ�
	m_wndTreeView.SelectItem(m_hRoot);
}

void CCodeMateDlg::OnFilePrep()
{
	CFilePrepDlg dlgFilePrep;
	// ��ȡ��ǰѡ����Ŀ
	HTREEITEM hItem = m_wndTreeView.GetSelectedItem();
	if (!hItem)
		hItem = m_hRoot;

	// ��ȡ����
	dlgFilePrep.m_strTitle = m_wndTreeView.GetItemText(hItem);
	// ����ͼ���ж���Ŀ����
	int iImage, iImageSel;
	m_wndTreeView.GetItemImage(hItem, iImage, iImageSel);
	dlgFilePrep.m_iFileType = iImage;
	SetCurrentDirectory(m_strAppPath);
	CString strLoad, strShow;
	LPTSTR strFileName = NULL;
	if (m_sMyConfig.m_uiLanguage == 1)
		strFileName = LANG_TCHINESE_FILE;
	else if (m_sMyConfig.m_uiLanguage == 2)
		strFileName = LANG_ENGLISH_FILE;
	LPTSTR lpszLang = new _TCHAR[LANG_STRING_LEN];
	if (m_sMyConfig.m_uiLanguage ==0)
		strLoad.LoadString(IDS_ATTRIB_ITEM);
	else
	{
		GetPrivateProfileString(LANG_ATTRIB_DLG,
			_T("ATTRIB_ITEM"), _T(""), lpszLang,
			LANG_STRING_LEN, strFileName);
		strLoad = lpszLang;
	}
	switch (iImage)
	{
	case 1:	// C++
		dlgFilePrep.m_strType = _T("C++ ") + strLoad;
		break;
	case 5:	// C
		dlgFilePrep.m_strType = _T("ANSI C ") + strLoad;
		break;
	case 7:	// HTM
		dlgFilePrep.m_strType = _T("HTML ") + strLoad;
		break;
	case 9:	// JScript
		dlgFilePrep.m_strType = _T("Java Script ") + strLoad;
		break;
	case 11:	// SQL
		dlgFilePrep.m_strType = _T("SQL ") + strLoad;
		break;
	case 13:	// TXT
		if (m_sMyConfig.m_uiLanguage ==0)
			dlgFilePrep.m_strType.LoadString(IDS_NORMAL_TEXT);
		else
		{
			GetPrivateProfileString(LANG_STRING,
				_T("NORMAL_TEXT"), _T(""), lpszLang,
				LANG_STRING_LEN, strFileName);
			dlgFilePrep.m_strType = lpszLang;
		}
		dlgFilePrep.m_strType = dlgFilePrep.m_strType + _T(" ") + strLoad;
		break;
	case 15:	// VBScript
		dlgFilePrep.m_strType = _T("VB Script ") + strLoad;
		break;
	case 17:	// C#
		dlgFilePrep.m_strType = _T("C#.NET ") + strLoad;
		break;
	default:	// ����
		if (m_sMyConfig.m_uiLanguage ==0)
		{
			strLoad.LoadString(IDS_ATTRIB_CLASS);
			dlgFilePrep.m_strType = strLoad;
		}
		else
		{
			GetPrivateProfileString(LANG_ATTRIB_DLG,
				_T("ATTRIB_CLASS"), _T(""),
				lpszLang, LANG_STRING_LEN, strFileName);
			dlgFilePrep.m_strType = lpszLang;
		}
	}
	delete []lpszLang;
	SetCurrentDirectory(m_sMyConfig.m_strWorkPath);
	hItem = m_wndTreeView.GetParentItem(hItem);
	if (iImage != 3 && iImage != 2) // �����ļ���
		dlgFilePrep.m_bIsFolder=false;
	else
		dlgFilePrep.m_bIsFolder=true;
	// �������·��
	CString strConv,strPath,strClassPath;
	if (m_sMyConfig.m_uiLanguage == 0)
		strConv.LoadString(IDS_CLASS_NORMAL);
	else
		strConv = m_arrLanString[64];
	if (hItem != m_hRoot)
	{
		strClassPath = ItemToPath(hItem);
		strPath = strClassPath;
		strPath = strPath.Mid(7);
		strPath = strConv +_T("\\") + strPath;
	}
	else
	{
		strClassPath = _T(".\\Code");
		strPath = strConv + _T("\\");
	}
	dlgFilePrep.m_strPath = strPath;
	dlgFilePrep.m_strFullPath = strClassPath;
	dlgFilePrep.m_uiLanguage = m_sMyConfig.m_uiLanguage;
	dlgFilePrep.m_strAppPath = m_strAppPath;
	dlgFilePrep.m_strWorkPath = m_sMyConfig.m_strWorkPath;
	dlgFilePrep.DoModal();
}

void CCodeMateDlg::OnEditUndo()
{
	// ���ñ༭���ĳ�������
	m_wndCodeEdit.Undo();
}

void CCodeMateDlg::OnEditCut()
{
	// ���ñ༭���ļ��к���
	m_wndCodeEdit.OnChar(0x18, 0, 0);
}

void CCodeMateDlg::OnEditCopy()
{
	// ���ñ༭���ĸ��ƺ���
	m_wndCodeEdit.OnChar(0x03, 0, 0);
}

void CCodeMateDlg::OnEditPaste()
{
	// ���ñ༭����ճ������
	m_wndCodeEdit.OnChar(0x16, 0, 0);
}

void CCodeMateDlg::OnEditClear()
{
	// ���ѡ����ɾ������
	int iStartlLine, iStartChar, iEndLine, iEndChar;
	m_wndCodeEdit.GetSelection(iStartlLine, iStartChar, iEndLine, iEndChar);
	m_wndCodeEdit.DeleteText(iStartlLine, iStartChar, iEndLine, iEndChar);
}

void CCodeMateDlg::OnEditSelall()
{
	// ���ñ༭����ȫѡ����
	m_wndCodeEdit.OnChar(0x01, 0, 0);
	int iStartlLine, iStartChar, iEndLine, iEndChar;
	m_wndCodeEdit.GetSelection(iStartlLine, iStartChar, iEndLine, iEndChar);
	if ( iEndLine - iStartlLine != 0 )
	{
		m_bTextSelected = true;
	}
	else
	{
		if ( iEndChar - iStartChar != 0 )
			m_bTextSelected = true;
		else
			m_bTextSelected = false;
	}
}

void CCodeMateDlg::OnEditClearAll()
{
	// ���ñ༭������պ���
	m_wndCodeEdit.ClearText();
}

void CCodeMateDlg::OnEditGoto()
{
	// �����б��
	m_wndCodeEdit.DestroyList();
	CGoToDlg dlgGoTo;
	dlgGoTo.m_uiLanguage = m_sMyConfig.m_uiLanguage;
	dlgGoTo.m_strAppPath = m_strAppPath;
	dlgGoTo.m_strWorkPath = m_sMyConfig.m_strWorkPath;
	dlgGoTo.m_LineIndex = m_wndCodeEdit.GetCurrentLine() + 1;
	dlgGoTo.m_ColumnIndex = m_wndCodeEdit.GetCurrentChar();
	dlgGoTo.LineNum = m_wndCodeEdit.GetNumberLines();
	dlgGoTo.SetRange = false;
	dlgGoTo.pMaxChar = new int [dlgGoTo.LineNum];
	for (int i = 0; i < dlgGoTo.LineNum; i++)
	{
		dlgGoTo.pMaxChar[i] = m_wndCodeEdit.GetLineLen(i);
	}
	if (dlgGoTo.DoModal() == IDOK)
	{
		m_wndCodeEdit.SetCurrentLine(dlgGoTo.m_LineIndex - 1);
		m_wndCodeEdit.SetCurrentChar(dlgGoTo.m_ColumnIndex);
		m_wndCodeEdit.SetFocus();
	}
}

void CCodeMateDlg::OnALine()
{
	// ���һ��
	m_wndCodeEdit.OnChar(0x0d, 0, 0);
}

void CCodeMateDlg::OnDelALine()
{
	// ɾ��һ��
	m_wndCodeEdit.OnChar(0x0c, 0, 0);
}

void CCodeMateDlg::OnEditShowlist()
{
	// ���ñ༭������ʾ�Զ�����б���
	m_wndCodeEdit.ShowAutoComplete();
}

void CCodeMateDlg::OnEditChangeType()
{
	// �����б��
	m_wndCodeEdit.DestroyList();
	CChangeTypeDlg dlgChType;
	dlgChType.m_CodeType = m_enFileType;
	dlgChType.m_uiLanguage = m_sMyConfig.m_uiLanguage;
	dlgChType.m_strAppPath = m_strAppPath;
	dlgChType.m_strWorkPath = m_sMyConfig.m_strWorkPath;
	CString strLoad,strShow,strLang;
	if (dlgChType.DoModal() == IDOK)
	{
		// ����ļ�ȫ·��
		HTREEITEM hItem = m_wndTreeView.GetParentItem(m_hSelected);
		CString strToPath;
		if (hItem != m_hRoot)
			strToPath =  ItemToPath(hItem);
		else
			strToPath = _T(".\\Code");
		CString strName = m_wndTreeView.GetItemText(m_hSelected);
		CString strFromPath;
		strFromPath = strToPath + _T("\\") + strName;
		strToPath = strToPath + _T("\\") + strName;
		// �ж�ԭ�����ļ�����
		int iImage, iImageSel;
		m_wndTreeView.GetItemImage(m_hSelected, iImage, iImageSel);
		CString strFileExt;
		switch (iImage)
		{
		case 1:	// C++
			strFileExt = _T(".c-p");
			break;
		case 5:	// C
			strFileExt = _T(".c-a");
			break;
		case 7:	// HTM
			strFileExt = _T(".h-t");
			break;
		case 9:	// JScript
			strFileExt = _T(".j-s");
			break;
		case 11:	// SQL
			strFileExt = _T(".s-q");
			break;
		case 13:	// TXT
			strFileExt = _T(".txt");
			break;
		case 15:	// VBScript
			strFileExt = _T(".v-b");
			break;
		case 17:	// C#
			strFileExt = _T(".c-s");
			break;
		}
		strFromPath += strFileExt;
		// �ж��µ��ļ����ͣ�ִ�и�������
		switch(dlgChType.m_CodeType)
		{
		case 0:
			strFileExt = _T(".c-p");
			strToPath += strFileExt;
			if (!ReNameFolder(strFromPath, strToPath))
			{
				CString strContent,strTitle;
				if (m_sMyConfig.m_uiLanguage == 0)
				{
					strContent.LoadString(IDS_MSGBOX_RENAMEERR);
					strTitle.LoadString(IDS_MSGBOX_ERROR);
				}
				else
				{
					SetCurrentDirectory(m_strAppPath);
					LPTSTR strFileName = NULL;
					if (m_sMyConfig.m_uiLanguage == 1)
						strFileName = LANG_TCHINESE_FILE;
					else if (m_sMyConfig.m_uiLanguage == 2)
						strFileName = LANG_ENGLISH_FILE;
					LPTSTR lpszLang = new _TCHAR[LANG_STRING_LEN];
					GetPrivateProfileString(LANG_MESSAGEBOX,
						_T("CONTENT_RENAME_ERROR"), _T(""),
						lpszLang, LANG_STRING_LEN, strFileName);
					strContent = lpszLang;
					strContent.Replace(_T("\\n"), _T("\n"));
					strTitle = m_arrLanString[69];
					delete []lpszLang;
					SetCurrentDirectory(m_sMyConfig.m_strWorkPath);
				}
				MessageBox(strContent, strTitle, MB_ICONERROR);
				return;
			}
			strLang = _T("C++");
			m_enFileType = SCT_C_PLUS_PLUS;
			m_wndCodeEdit.SetSyntaxColoring(SCT_C_PLUS_PLUS);
			m_wndCodeEdit.AddKeywords(
				(LPTSTR)(LPCTSTR)(m_sMyConfig.m_arrKeyWord[0]),
				m_sMyConfig.m_arrKeyWord[0].GetLength()
				);
			m_wndCodeEdit.AddConstants(
				(LPTSTR)(LPCTSTR)(m_sMyConfig.m_arrConstant[0]),
				m_sMyConfig.m_arrConstant[0].GetLength()
				);
			m_wndCodeEdit.UpdateSyntaxColoring();
			m_wndTreeView.SetItemImage(m_hSelected,1,0);
			break;
		case 1:
			strFileExt = _T(".c-a");
			strToPath += strFileExt;
			if (!ReNameFolder(strFromPath, strToPath))
			{
				CString strContent,strTitle;
				if (m_sMyConfig.m_uiLanguage == 0)
				{
					strContent.LoadString(IDS_MSGBOX_RENAMEERR);
					strTitle.LoadString(IDS_MSGBOX_ERROR);
				}
				else
				{
					SetCurrentDirectory(m_strAppPath);
					LPTSTR strFileName = NULL;
					if (m_sMyConfig.m_uiLanguage == 1)
						strFileName = LANG_TCHINESE_FILE;
					else if (m_sMyConfig.m_uiLanguage == 2)
						strFileName = LANG_ENGLISH_FILE;
					LPTSTR lpszLang = new _TCHAR[LANG_STRING_LEN];
					GetPrivateProfileString(LANG_MESSAGEBOX,
						_T("CONTENT_RENAME_ERROR"), _T(""),
						lpszLang, LANG_STRING_LEN, strFileName);
					strContent = lpszLang;
					strContent.Replace(_T("\\n"), _T("\n"));
					strTitle = m_arrLanString[69];
					delete []lpszLang;
					SetCurrentDirectory(m_sMyConfig.m_strWorkPath);
				}
				MessageBox(strContent, strTitle, MB_ICONERROR);
				return;
			}
			strLang = _T("ANSI C");
			m_enFileType = SCT_C_ANSI;
			m_wndCodeEdit.SetSyntaxColoring(SCT_C_ANSI);
			m_wndCodeEdit.AddKeywords(
				(LPTSTR)(LPCTSTR)(m_sMyConfig.m_arrKeyWord[1]),
				m_sMyConfig.m_arrKeyWord[1].GetLength()
				);
			m_wndCodeEdit.AddConstants(
				(LPTSTR)(LPCTSTR)(m_sMyConfig.m_arrConstant[1]),
				m_sMyConfig.m_arrConstant[1].GetLength()
				);
			m_wndCodeEdit.UpdateSyntaxColoring();
			m_wndTreeView.SetItemImage(m_hSelected,5,4);
			break;
		case 2:
			strFileExt = _T(".c-s");
			strToPath += strFileExt;
			if (!ReNameFolder(strFromPath, strToPath))
			{
				CString strContent,strTitle;
				if (m_sMyConfig.m_uiLanguage == 0)
				{
					strContent.LoadString(IDS_MSGBOX_RENAMEERR);
					strTitle.LoadString(IDS_MSGBOX_ERROR);
				}
				else
				{
					SetCurrentDirectory(m_strAppPath);
					LPTSTR strFileName = NULL;
					if (m_sMyConfig.m_uiLanguage == 1)
						strFileName = LANG_TCHINESE_FILE;
					else if (m_sMyConfig.m_uiLanguage == 2)
						strFileName = LANG_ENGLISH_FILE;
					LPTSTR lpszLang = new _TCHAR[LANG_STRING_LEN];
					GetPrivateProfileString(LANG_MESSAGEBOX,
						_T("CONTENT_RENAME_ERROR"), _T(""),
						lpszLang, LANG_STRING_LEN, strFileName);
					strContent = lpszLang;
					strContent.Replace(_T("\\n"), _T("\n"));
					strTitle = m_arrLanString[69];
					delete []lpszLang;
					SetCurrentDirectory(m_sMyConfig.m_strWorkPath);
				}
				MessageBox(strContent, strTitle, MB_ICONERROR);
				return;
			}
			strLang = _T("C#");
			m_enFileType = SCT_C_SHARP;
			m_wndCodeEdit.SetSyntaxColoring(SCT_C_SHARP);
			m_wndCodeEdit.AddKeywords(
				(LPTSTR)(LPCTSTR)(m_sMyConfig.m_arrKeyWord[2]),
				m_sMyConfig.m_arrKeyWord[2].GetLength()
				);
			m_wndCodeEdit.AddConstants(
				(LPTSTR)(LPCTSTR)(m_sMyConfig.m_arrConstant[2]),
				m_sMyConfig.m_arrConstant[2].GetLength()
				);
			m_wndCodeEdit.UpdateSyntaxColoring();
			m_wndTreeView.SetItemImage(m_hSelected,17,16);
			break;
		case 3:
			strFileExt = _T(".h-t");
			strToPath += strFileExt;
			if (!ReNameFolder(strFromPath, strToPath))
			{
				CString strContent,strTitle;
				if (m_sMyConfig.m_uiLanguage == 0)
				{
					strContent.LoadString(IDS_MSGBOX_RENAMEERR);
					strTitle.LoadString(IDS_MSGBOX_ERROR);
				}
				else
				{
					SetCurrentDirectory(m_strAppPath);
					LPTSTR strFileName = NULL;
					if (m_sMyConfig.m_uiLanguage == 1)
						strFileName = LANG_TCHINESE_FILE;
					else if (m_sMyConfig.m_uiLanguage == 2)
						strFileName = LANG_ENGLISH_FILE;
					LPTSTR lpszLang = new _TCHAR[LANG_STRING_LEN];
					GetPrivateProfileString(LANG_MESSAGEBOX,
						_T("CONTENT_RENAME_ERROR"), _T(""),
						lpszLang, LANG_STRING_LEN, strFileName);
					strContent = lpszLang;
					strContent.Replace(_T("\\n"), _T("\n"));
					strTitle = m_arrLanString[69];
					delete []lpszLang;
					SetCurrentDirectory(m_sMyConfig.m_strWorkPath);
				}
				MessageBox(strContent, strTitle, MB_ICONERROR);
				return;
			}
			strLang = _T("HTML");
			m_enFileType = SCT_HTML;
			m_wndCodeEdit.SetSyntaxColoring(SCT_HTML);
			m_wndCodeEdit.AddKeywords(
				(LPTSTR)(LPCTSTR)(m_sMyConfig.m_arrKeyWord[3]),
				m_sMyConfig.m_arrKeyWord[3].GetLength()
				);
			m_wndCodeEdit.AddConstants(
				(LPTSTR)(LPCTSTR)(m_sMyConfig.m_arrConstant[3]),
				m_sMyConfig.m_arrConstant[3].GetLength()
				);
			m_wndCodeEdit.UpdateSyntaxColoring();
			m_wndTreeView.SetItemImage(m_hSelected,7,6);
			break;
		case 4:
			strFileExt = _T(".j-s");
			strToPath += strFileExt;
			if (!ReNameFolder(strFromPath, strToPath))
			{
				CString strContent,strTitle;
				if (m_sMyConfig.m_uiLanguage == 0)
				{
					strContent.LoadString(IDS_MSGBOX_RENAMEERR);
					strTitle.LoadString(IDS_MSGBOX_ERROR);
				}
				else
				{
					SetCurrentDirectory(m_strAppPath);
					LPTSTR strFileName = NULL;
					if (m_sMyConfig.m_uiLanguage == 1)
						strFileName = LANG_TCHINESE_FILE;
					else if (m_sMyConfig.m_uiLanguage == 2)
						strFileName = LANG_ENGLISH_FILE;
					LPTSTR lpszLang = new _TCHAR[LANG_STRING_LEN];
					GetPrivateProfileString(LANG_MESSAGEBOX,
						_T("CONTENT_RENAME_ERROR"), _T(""),
						lpszLang, LANG_STRING_LEN, strFileName);
					strContent = lpszLang;
					strContent.Replace(_T("\\n"), _T("\n"));
					strTitle = m_arrLanString[69];
					delete []lpszLang;
					SetCurrentDirectory(m_sMyConfig.m_strWorkPath);
				}
				MessageBox(strContent, strTitle, MB_ICONERROR);
				return;
			}
			strLang = _T("JS");
			m_enFileType = SCT_JSCRIPT;
			m_wndCodeEdit.SetSyntaxColoring(SCT_JSCRIPT);
			m_wndCodeEdit.AddKeywords(
				(LPTSTR)(LPCTSTR)(m_sMyConfig.m_arrKeyWord[4]),
				m_sMyConfig.m_arrKeyWord[4].GetLength()
				);
			m_wndCodeEdit.AddConstants(
				(LPTSTR)(LPCTSTR)(m_sMyConfig.m_arrConstant[4]),
				m_sMyConfig.m_arrConstant[4].GetLength()
				);
			m_wndCodeEdit.UpdateSyntaxColoring();
			m_wndTreeView.SetItemImage(m_hSelected,9,8);
			break;
		case 5:
			strFileExt = _T(".v-b");
			strToPath += strFileExt;
			if (!ReNameFolder(strFromPath, strToPath))
			{
				CString strContent,strTitle;
				if (m_sMyConfig.m_uiLanguage == 0)
				{
					strContent.LoadString(IDS_MSGBOX_RENAMEERR);
					strTitle.LoadString(IDS_MSGBOX_ERROR);
				}
				else
				{
					SetCurrentDirectory(m_strAppPath);
					LPTSTR strFileName = NULL;
					if (m_sMyConfig.m_uiLanguage == 1)
						strFileName = LANG_TCHINESE_FILE;
					else if (m_sMyConfig.m_uiLanguage == 2)
						strFileName = LANG_ENGLISH_FILE;
					LPTSTR lpszLang = new _TCHAR[LANG_STRING_LEN];
					GetPrivateProfileString(LANG_MESSAGEBOX,
						_T("CONTENT_RENAME_ERROR"), _T(""),
						lpszLang, LANG_STRING_LEN, strFileName);
					strContent = lpszLang;
					strContent.Replace(_T("\\n"), _T("\n"));
					strTitle = m_arrLanString[69];
					delete []lpszLang;
					SetCurrentDirectory(m_sMyConfig.m_strWorkPath);
				}
				MessageBox(strContent, strTitle, MB_ICONERROR);
				return;
			}
			strLang = _T("VBS");
			m_enFileType = SCT_VBSCRIPT;
			m_wndCodeEdit.SetSyntaxColoring(SCT_VBSCRIPT);
			m_wndCodeEdit.AddKeywords(
				(LPTSTR)(LPCTSTR)(m_sMyConfig.m_arrKeyWord[5]),
				m_sMyConfig.m_arrKeyWord[5].GetLength()
				);
			m_wndCodeEdit.AddConstants(
				(LPTSTR)(LPCTSTR)(m_sMyConfig.m_arrConstant[5]),
				m_sMyConfig.m_arrConstant[5].GetLength()
				);
			m_wndCodeEdit.UpdateSyntaxColoring();
			m_wndTreeView.SetItemImage(m_hSelected,15,14);
			break;
		case 6:
			strFileExt = _T(".s-q");
			strToPath += strFileExt;
			if (!ReNameFolder(strFromPath, strToPath))
			{
				CString strContent,strTitle;
				if (m_sMyConfig.m_uiLanguage == 0)
				{
					strContent.LoadString(IDS_MSGBOX_RENAMEERR);
					strTitle.LoadString(IDS_MSGBOX_ERROR);
				}
				else
				{
					SetCurrentDirectory(m_strAppPath);
					LPTSTR strFileName = NULL;
					if (m_sMyConfig.m_uiLanguage == 1)
						strFileName = LANG_TCHINESE_FILE;
					else if (m_sMyConfig.m_uiLanguage == 2)
						strFileName = LANG_ENGLISH_FILE;
					LPTSTR lpszLang = new _TCHAR[LANG_STRING_LEN];
					GetPrivateProfileString(LANG_MESSAGEBOX,
						_T("CONTENT_RENAME_ERROR"), _T(""),
						lpszLang, LANG_STRING_LEN, strFileName);
					strContent = lpszLang;
					strContent.Replace(_T("\\n"), _T("\n"));
					strTitle = m_arrLanString[69];
					delete []lpszLang;
					SetCurrentDirectory(m_sMyConfig.m_strWorkPath);
				}
				MessageBox(strContent, strTitle, MB_ICONERROR);
				return;
			}
			strLang = _T("SQL");
			m_enFileType = SCT_SQL;
			m_wndCodeEdit.SetSyntaxColoring(SCT_SQL);
			m_wndCodeEdit.AddKeywords(
				(LPTSTR)(LPCTSTR)(m_sMyConfig.m_arrKeyWord[6]),
				m_sMyConfig.m_arrKeyWord[6].GetLength()
				);
			m_wndCodeEdit.AddConstants(
				(LPTSTR)(LPCTSTR)(m_sMyConfig.m_arrConstant[6]),
				m_sMyConfig.m_arrConstant[6].GetLength()
				);
			m_wndCodeEdit.UpdateSyntaxColoring();
			m_wndTreeView.SetItemImage(m_hSelected,11,10);
			break;
		case 7:
			strFileExt = _T(".txt");
			strToPath += strFileExt;
			if (!ReNameFolder(strFromPath, strToPath))
			{
				CString strContent,strTitle;
				if (m_sMyConfig.m_uiLanguage == 0)
				{
					strContent.LoadString(IDS_MSGBOX_RENAMEERR);
					strTitle.LoadString(IDS_MSGBOX_ERROR);
				}
				else
				{
					SetCurrentDirectory(m_strAppPath);
					LPTSTR strFileName = NULL;
					if (m_sMyConfig.m_uiLanguage == 1)
						strFileName = LANG_TCHINESE_FILE;
					else if (m_sMyConfig.m_uiLanguage == 2)
						strFileName = LANG_ENGLISH_FILE;
					LPTSTR lpszLang = new _TCHAR[LANG_STRING_LEN];
					GetPrivateProfileString(LANG_MESSAGEBOX,
						_T("CONTENT_RENAME_ERROR"), _T(""),
						lpszLang, LANG_STRING_LEN, strFileName);
					strContent = lpszLang;
					strContent.Replace(_T("\\n"), _T("\n"));
					strTitle = m_arrLanString[69];
					delete []lpszLang;
					SetCurrentDirectory(m_sMyConfig.m_strWorkPath);
				}
				MessageBox(strContent, strTitle, MB_ICONERROR);
				return;
			}
			strLang = _T("TXT");
			m_enFileType = SCT_NONE;
			m_wndCodeEdit.SetSyntaxColoring(SCT_NONE);
			m_wndCodeEdit.UpdateSyntaxColoring();
			m_wndTreeView.SetItemImage(m_hSelected,13,12);
			break;
		}
		// �ı�״̬����ʾ��Ϣ
		if (m_sMyConfig.m_uiLanguage == 0)
			strLoad.LoadString(ID_INDICATOR_LANG);
		else
			strLoad = m_arrLanString[58];

		strShow.Format(strLoad,strLang);
		m_wndStatusBar.SetPaneText(2,strShow);
	}
}

void CCodeMateDlg::OnEditFind()
{
	// �����б��
	m_wndCodeEdit.DestroyList();
	if (m_pFindDlg == NULL)
	{
		m_pFindDlg = new CFindReplaceDialog;   // ��ģ̬�Ի���Ҫ�� new ��������ڴ�
		DWORD dwFlags = NULL;
		if (m_sMyConfig.m_bMatchCase) dwFlags |= FR_MATCHCASE;
		if (m_sMyConfig.m_bMatchWholeWord) dwFlags |= FR_WHOLEWORD;
		if (m_sMyConfig.m_bDirectionDown) dwFlags |= FR_DOWN;
		if (!m_pFindDlg->Create(TRUE,
			(LPTSTR)(LPCTSTR)(m_sMyConfig.strFindString),
			NULL, dwFlags, this))
		{
			return;
		}
	}
	m_pFindDlg->ShowWindow(SW_SHOW);   // ���ڴ�����ϣ���ʾ����
	m_pFindDlg->SetActiveWindow();    // ����Ϊ�����
}

void CCodeMateDlg::OnEditFindNext()
{
	if (m_sMyConfig.strFindString.GetLength() == 0)
	{
		OnEditFind();
		return;
	}
	if (!(m_wndCodeEdit.Find(
		(LPTSTR)(LPCTSTR)(m_sMyConfig.strFindString),
		m_sMyConfig.strFindString.GetLength(),
		m_sMyConfig.m_bMatchCase,
		m_sMyConfig.m_bMatchWholeWord,
		m_sMyConfig.m_bDirectionDown
		)))
	{
		if (m_sMyConfig.m_bDirectionDown)
			MessageBox(_T("�Ѳ��ҵ��ĵ�ĩβ��"), _T("����"), MB_ICONWARNING);
		else
			MessageBox(_T("�Ѳ��ҵ��ĵ���ͷ��"), _T("����"), MB_ICONWARNING);
	}
	else
		m_bTextSelected = true;
}

void CCodeMateDlg::OnEditReplace()
{
	if (m_pFindDlg == NULL)
	{
		m_pFindDlg = new CFindReplaceDialog;   // ��ģ̬�Ի���Ҫ�� new ��������ڴ�
		DWORD dwFlags = NULL;
		if (m_sMyConfig.m_bMatchCase) dwFlags |= FR_MATCHCASE;
		if (m_sMyConfig.m_bMatchWholeWord) dwFlags |= FR_WHOLEWORD;
		if (!m_pFindDlg->Create(	FALSE,
			(LPTSTR)(LPCTSTR)(m_sMyConfig.strFindString),
			NULL, dwFlags, this))
		{
			return;
		}
	}
	m_pFindDlg->ShowWindow(SW_SHOW);   // ���ڴ�����ϣ���ʾ����
	m_pFindDlg->SetActiveWindow();    // ����Ϊ�����
}


void CCodeMateDlg::OnSetOption()
{
	CConfigDlg dlgOption;
	dlgOption.m_uiLanguage = m_sMyConfig.m_uiLanguage;
	dlgOption.m_strAppPath = m_strAppPath;
	dlgOption.m_strWorkPath = m_sMyConfig.m_strWorkPath;
	dlgOption.m_uiDefEncode = m_sMyConfig.m_uiDefEncode;
	dlgOption.m_bShowLineNum = m_sMyConfig.m_bShowLineNum;
	dlgOption.m_iTabLen = m_sMyConfig.m_iTabLen;
	m_wndCodeEdit.GetColorStruct(dlgOption.m_coColor);
	_tcscpy_s(dlgOption.font.lfFaceName, 32, m_sMyConfig.m_strFontName);
	dlgOption.m_iSize = m_sMyConfig.m_iSize;
	dlgOption.font.lfHeight = m_sMyConfig.m_iHeight;
	dlgOption.font.lfWeight = (m_sMyConfig.m_bBold ? FW_BOLD : FW_NORMAL);
	dlgOption.font.lfItalic = m_sMyConfig.m_bItalic;
	dlgOption.font.lfCharSet = m_sMyConfig.m_iCharSet;
	for (int i = 0; i <= 6; i++)
	{
		dlgOption.m_arrKeyWord.Add(m_sMyConfig.m_arrKeyWord[i]);
		dlgOption.m_arrConstant.Add(m_sMyConfig.m_arrConstant[i]);
	}
	if (dlgOption.DoModal() == IDOK)
	{
		// ��������
		m_sMyConfig.m_uiDefEncode = dlgOption.m_uiDefEncode;
		m_sMyConfig.m_bShowLineNum = dlgOption.m_bShowLineNum;
		m_sMyConfig.m_iTabLen = dlgOption.m_iTabLen;
		m_sMyConfig.m_strFontName = dlgOption.font.lfFaceName;
		m_sMyConfig.m_iCharSet = dlgOption.font.lfCharSet;
		m_sMyConfig.m_iHeight = dlgOption.font.lfHeight;
		m_sMyConfig.m_iSize = dlgOption.m_iSize;
		m_sMyConfig.m_bBold = (dlgOption.font.lfWeight == FW_BOLD);
		m_sMyConfig.m_bItalic = dlgOption.font.lfItalic;
		m_sMyConfig.m_coColor = dlgOption.m_coColor;
		if (dlgOption.m_CodeType != -1)
		{
			m_sMyConfig.m_arrKeyWord.RemoveAll();
			m_sMyConfig.m_arrConstant.RemoveAll();
			for (int i = 0; i <= 6; i++)
			{
				m_sMyConfig.m_arrKeyWord.Add(dlgOption.m_arrKeyWord[i]);
				m_sMyConfig.m_arrConstant.Add(dlgOption.m_arrConstant[i]);
			}
		}
		// ��������
		if (m_enFileType != SCT_ERROR)
			m_wndCodeEdit.MoveWindow(0,0,10,10);
		FILE_ENCODING_TYPE NewType;
		switch(dlgOption.m_uiDefEncode)
		{
		case 0:
			NewType = FET_ANSI;
			break;
		case 1:
			NewType = FET_UNICODE;
			break;
		case 2:
			NewType = FET_UNICODE_BE;
			break;
		case 3:
			NewType = FET_UTF_8;
			break;
		}
		m_wndCodeEdit.SetDefaultFileEncoding(NewType);
		m_wndCodeEdit.SetShowLineNumberStatus(dlgOption.m_bShowLineNum);
		m_wndCodeEdit.SetTABs(dlgOption.m_iTabLen);
		m_wndCodeEdit.SetColorStruct(m_sMyConfig.m_coColor);
		m_wndCodeEdit.SetEditorFont(
			(LPTSTR)(LPCTSTR)(dlgOption.font.lfFaceName),
			dlgOption.font.lfHeight,
			(dlgOption.font.lfWeight == FW_BOLD),
			dlgOption.font.lfItalic,
			dlgOption.font.lfCharSet
			);
		if (m_enFileType != SCT_ERROR)
		{
			int iSBHeight = (m_sMyConfig.m_bShowStatusBar ? 18 : 0);
			int iTBHeight = (m_sMyConfig.m_bShowToolBar ? 30 : 0);
			m_wndCodeEdit.MoveWindow(200 + m_sMyConfig.m_lSplPos,
				iTBHeight, m_iWidth,
				m_iHeight - iSBHeight - iTBHeight);
			if (dlgOption.m_CodeType != -1 && m_enFileType != SCT_NONE)
			{
				m_wndCodeEdit.AddKeywords(
					(LPTSTR)(LPCTSTR)m_sMyConfig.m_arrKeyWord[m_enFileType-1],
					m_sMyConfig.m_arrKeyWord[m_enFileType-1].GetLength()
					);
				m_wndCodeEdit.AddConstants(
					(LPTSTR)(LPCTSTR)m_sMyConfig.m_arrConstant[m_enFileType-1], 
					m_sMyConfig.m_arrConstant[m_enFileType-1].GetLength()
					);
				m_wndCodeEdit.UpdateSyntaxColoring();
			}
		}
		m_wndTreeView.SetBkColor(dlgOption.m_coColor.crBackgroundColor);
		m_wndTreeView.SetTextColor(dlgOption.m_coColor.crForegroundColor);
		// ���´洢λ��
		if (m_sMyConfig.m_strWorkPath != dlgOption.m_strWorkPath)
		{
			// ���¹����ļ���
			m_sMyConfig.m_strWorkPath = dlgOption.m_strWorkPath;
			SetCurrentDirectory(
				(m_sMyConfig.m_strWorkPath == _T(".\\"))
				? m_strAppPath
				: m_sMyConfig.m_strWorkPath
				);
			if (dlgOption.isMoved == false)
				m_wndCodeEdit.SetChangedStatus(FALSE);
			OnFileReload();
		}
	}
}

void CCodeMateDlg::OnViewToolbar()
{
	// ������ʾ״̬��־
	m_sMyConfig.m_bShowToolBar = !m_sMyConfig.m_bShowToolBar;
	// ��ʾ�����ؿؼ�
	int ShowStatus = (m_sMyConfig.m_bShowToolBar ? SW_SHOW : SW_HIDE);
	m_wndToolBar.ShowWindow(ShowStatus);
	// �����������ÿؼ��߶�
	int iSBHeight = (m_sMyConfig.m_bShowStatusBar ? 18 : 0);
	int iTBHeight = (m_sMyConfig.m_bShowToolBar ? 30 : 0);
	int iSearchHeight = (m_bShowSearch ? 50 : 0);
	CRect rect;
	GetClientRect(&rect);
	if (m_wndCodeEdit.GetSafeHwnd())		// �༭��
	{
		if (m_enFileType != SCT_ERROR)
		{
			m_wndCodeEdit.MoveWindow(
				200 + m_sMyConfig.m_lSplPos,
				iTBHeight,
				rect.Width()- 200 - m_sMyConfig.m_lSplPos,
				rect.Height() - iTBHeight - iSBHeight
				);
		}
	}
	if (m_wndTreeView.GetSafeHwnd())		// ���οؼ�
	{
		m_wndTreeView.MoveWindow(
			0, iTBHeight,
			197 + m_sMyConfig.m_lSplPos,
			rect.Height() - iTBHeight - iSBHeight - iSearchHeight
			);
	}
	if (m_wndSplitter.GetSafeHwnd())		// �з���
	{
		m_wndSplitter.MoveWindow(
			197 + m_sMyConfig.m_lSplPos,
			iTBHeight, 3,
			rect.Height() - iTBHeight - iSBHeight
			);
	}
}

void CCodeMateDlg::OnViewStatusBar()
{
	// ������ʾ״̬��־
	m_sMyConfig.m_bShowStatusBar = !m_sMyConfig.m_bShowStatusBar;
	// ��ʾ�����ؿؼ�
	int iShowStatus = (m_sMyConfig.m_bShowStatusBar ? SW_SHOW : SW_HIDE);
	m_wndStatusBar.ShowWindow(iShowStatus);
	// �����������ÿؼ��߶�
	int iSBHeight = (m_sMyConfig.m_bShowStatusBar ? 18 : 0);
	int iTBHeight = (m_sMyConfig.m_bShowToolBar ? 30 : 0);
	int iSearchHeight = (m_bShowSearch ? 50 : 0);
	CRect rect;
	GetClientRect(&rect);
	if (m_wndCodeEdit.GetSafeHwnd())		// �༭��
	{
		if (m_enFileType != SCT_ERROR)
		{
			m_wndCodeEdit.MoveWindow(
				200 + m_sMyConfig.m_lSplPos,
				iTBHeight,
				rect.Width()- 200 - m_sMyConfig.m_lSplPos,
				rect.Height() - iTBHeight - iSBHeight
				);
		}
	}
	if (m_wndTreeView.GetSafeHwnd())		// ���οؼ�
	{
		m_wndTreeView.MoveWindow(
			0, iTBHeight,
			197 + m_sMyConfig.m_lSplPos,
			rect.Height() - iTBHeight - iSBHeight - iSearchHeight
			);
	}
	if (m_bShowSearch)	// ��ʾ������
	{
		if (m_stcContent.GetSafeHwnd())
		{
			m_stcContent.MoveWindow(
				3,
				rect.Height() - iSBHeight - iSearchHeight + 3,
				60, 20
				);
		}

		if (m_edtKeyWord.GetSafeHwnd())
		{
			m_edtKeyWord.MoveWindow(
				63,
				rect.Height() - iSBHeight - iSearchHeight + 3,
				m_sMyConfig.m_lSplPos + 80,
				20
				);
		}

		if (m_btnSearch.GetSafeHwnd())
		{
			m_btnSearch.MoveWindow(
				197 + m_sMyConfig.m_lSplPos - 53,
				rect.Height() - iSBHeight - iSearchHeight + 3,
				50, 20
				);
		}

		if (m_chkCase.GetSafeHwnd())
		{
			m_chkCase.MoveWindow(3,
				rect.Height() - iSBHeight - iSearchHeight + 26,
				80, 20
				);
			m_chkCase.Invalidate();
		}

		if (m_cboType.GetSafeHwnd())
		{
			m_cboType.MoveWindow(
				83,
				rect.Height() - iSBHeight - iSearchHeight + 26,
				m_sMyConfig.m_lSplPos + 60,
				20
				);
			m_cboType.Invalidate();
		}

		if (m_btnHide.GetSafeHwnd())
		{
			m_btnHide.MoveWindow(
				197 + m_sMyConfig.m_lSplPos - 53,
				rect.Height() - iSBHeight - iSearchHeight + 26,
				50, 20
				);
			m_btnHide.Invalidate();
		}
	}
	if (m_wndSplitter.GetSafeHwnd())		// �з���
	{
		m_wndSplitter.MoveWindow(
			197 + m_sMyConfig.m_lSplPos,
			iTBHeight,
			3,
			rect.Height() - iTBHeight - iSBHeight
			);
	}
}

void CCodeMateDlg::OnLanChns()
{
	// ���ý�������
	if (m_sMyConfig.m_uiLanguage != 0)
	{
		if (SetMainLanguage(0, _T("")) != FALSE)
			m_sMyConfig.m_uiLanguage = 0;
	}
}

void CCodeMateDlg::OnLanChnt()
{
	// ���ý�������
	if (m_sMyConfig.m_uiLanguage != 1)
	{
		SetCurrentDirectory(m_strAppPath);
		if (SetMainLanguage(1, LANG_TCHINESE_FILE) != FALSE)
			m_sMyConfig.m_uiLanguage = 1;
		SetCurrentDirectory(m_sMyConfig.m_strWorkPath);
	}
}

void CCodeMateDlg::OnLanEng()
{
	// ���ý�������
	if (m_sMyConfig.m_uiLanguage != 2)
	{
		SetCurrentDirectory(m_strAppPath);
		if (SetMainLanguage(2, LANG_ENGLISH_FILE) != FALSE)
			m_sMyConfig.m_uiLanguage = 2;
		SetCurrentDirectory(m_sMyConfig.m_strWorkPath);
	}
}

void CCodeMateDlg::OnHelp()
{
	CString path;
	path = m_strAppPath + _T("CodeMate.chm");
	HINSTANCE hInstance = ShellExecute(
		this->m_hWnd, _T("open"), path,
		_T(""), NULL, SW_SHOWNORMAL);
	if ((LONG)hInstance <= 32)
	{
		AfxMessageBox(
			_T("�޷�����������\n��������ļ����Ƽ�·���Ƿ���ȷ��"),
			MB_ICONERROR
			);
	}
}

void CCodeMateDlg::GetMainClass(HTREEITEM hParent, LPCTSTR strText)
{
	CFileFind cfFile;
	// ��ø�Ŀ¼�µ������ļ����ļ���
	BOOL bContinue = cfFile.FindFile(strText);
	while(bContinue)
	{
		bContinue = cfFile.FindNextFile();
		CString strFileName = cfFile.GetFileName();
		if (!strFileName.IsEmpty() && !cfFile.IsDots())
		{
			if(cfFile.IsDirectory())// �����Ŀ¼
				m_wndTreeView.InsertItem(strFileName,3,3,hParent);
			// ������չ���ж��ļ�����
			CString strFileExt = strFileName.Right(4);
			strFileExt.MakeLower();
			if (strFileExt == _T(".c-p"))
				m_wndTreeView.InsertItem(cfFile.GetFileTitle(),1,0,hParent);
			if (strFileExt == _T(".c-a"))
				m_wndTreeView.InsertItem(cfFile.GetFileTitle(),5,4,hParent);
			if (strFileExt == _T(".c-s"))
				m_wndTreeView.InsertItem(cfFile.GetFileTitle(),17,16,hParent);
			if (strFileExt == _T(".h-t"))
				m_wndTreeView.InsertItem(cfFile.GetFileTitle(),7,6,hParent);
			if (strFileExt == _T(".j-s"))
				m_wndTreeView.InsertItem(cfFile.GetFileTitle(),9,8,hParent);
			if (strFileExt == _T(".v-b"))
				m_wndTreeView.InsertItem(cfFile.GetFileTitle(),15,14,hParent);
			if (strFileExt == _T(".s-q"))
				m_wndTreeView.InsertItem(cfFile.GetFileTitle(),11,10,hParent);
			if (strFileExt == _T(".txt"))
				m_wndTreeView.InsertItem(cfFile.GetFileTitle(),13,12,hParent);
		}
	}
}

void CCodeMateDlg::GetClassDir(HTREEITEM hParent)
{
	HTREEITEM hChild = m_wndTreeView.GetChildItem(hParent);
	while(hChild)	// ���������Ѽ�����Ŀ
	{
		int iImage, iImageSel;
		m_wndTreeView.GetItemImage(hChild, iImage, iImageSel);
		if (iImage != 2 && iImage != 3)	// ������ļ�������
		{
			hChild = m_wndTreeView.GetNextItem(hChild,TVGN_NEXT);
			continue;
		}
		// ���ÿ��Ŀ¼�µ������ļ����ļ���
		CString strText = _T(".\\Code\\") + m_wndTreeView.GetItemText(hChild);
		strText += _T("\\*.*");
		CFileFind cfFile;
		BOOL bContinue = cfFile.FindFile(strText);
		while(bContinue)
		{
			bContinue = cfFile.FindNextFile();
			CString strFileName = cfFile.GetFileName();
			if (!strFileName.IsEmpty() && !cfFile.IsDots())
			{
				if(cfFile.IsDirectory() )// �����Ŀ¼
					m_wndTreeView.InsertItem(strFileName,3,3,hChild);
				// ������չ���ж��ļ�����
				CString strFileExt = strFileName.Right(4);
				strFileExt.MakeLower();
				if (strFileExt == _T(".c-p"))
					m_wndTreeView.InsertItem(cfFile.GetFileTitle(),1,0,hChild);
				if (strFileExt == _T(".c-a"))
					m_wndTreeView.InsertItem(cfFile.GetFileTitle(),5,4,hChild);
				if (strFileExt == _T(".c-s"))
					m_wndTreeView.InsertItem(cfFile.GetFileTitle(),17,16,hChild);
				if (strFileExt == _T(".h-t"))
					m_wndTreeView.InsertItem(cfFile.GetFileTitle(),7,6,hChild);
				if (strFileExt == _T(".j-s"))
					m_wndTreeView.InsertItem(cfFile.GetFileTitle(),9,8,hChild);
				if (strFileExt == _T(".v-b"))
					m_wndTreeView.InsertItem(cfFile.GetFileTitle(),15,14,hChild);
				if (strFileExt == _T(".s-q"))
					m_wndTreeView.InsertItem(cfFile.GetFileTitle(),11,10,hChild);
				if (strFileExt == _T(".txt"))
					m_wndTreeView.InsertItem(cfFile.GetFileTitle(),13,12,hChild);
			}
		}
		hChild = m_wndTreeView.GetNextItem(hChild,TVGN_NEXT);
	}	
}

void CCodeMateDlg::AddSubItem(HTREEITEM hParent)// �������
{
	CString strPath = ItemToPath(hParent);
	strPath += _T("\\*.*");
	CFileFind cfFile;
	BOOL bContinue = cfFile.FindFile(strPath);
	while(bContinue)// �ݹ�������Ŀ¼�µ������ļ���Ŀ¼������Ŀ¼��ӵ����ؼ�
	{
		bContinue = cfFile.FindNextFile();
		CString strFileName = cfFile.GetFileName();
		if (!strFileName.IsEmpty() && !cfFile.IsDots())
		{
			if(cfFile.IsDirectory() )// �����Ŀ¼
				m_wndTreeView.InsertItem(strFileName,3,3,hParent);
			// ������չ���ж��ļ�����
			CString strFileExt = strFileName.Right(4);
			strFileExt.MakeLower();
			if (strFileExt == _T(".c-p"))
				m_wndTreeView.InsertItem(cfFile.GetFileTitle(),1,0,hParent);
			if (strFileExt == _T(".c-a"))
				m_wndTreeView.InsertItem(cfFile.GetFileTitle(),5,4,hParent);
			if (strFileExt == _T(".c-s"))
				m_wndTreeView.InsertItem(cfFile.GetFileTitle(),17,16,hParent);
			if (strFileExt == _T(".h-t"))
				m_wndTreeView.InsertItem(cfFile.GetFileTitle(),7,6,hParent);
			if (strFileExt == _T(".j-s"))
				m_wndTreeView.InsertItem(cfFile.GetFileTitle(),9,8,hParent);
			if (strFileExt == _T(".v-b"))
				m_wndTreeView.InsertItem(cfFile.GetFileTitle(),15,14,hParent);
			if (strFileExt == _T(".s-q"))
				m_wndTreeView.InsertItem(cfFile.GetFileTitle(),11,10,hParent);
			if (strFileExt == _T(".txt"))
				m_wndTreeView.InsertItem(cfFile.GetFileTitle(),13,12,hParent);
		}
	}
}

CString CCodeMateDlg::ItemToPath(HTREEITEM hCurrent)
{
	CString strTemp;
	CString strReturn = m_wndTreeView.GetItemText(hCurrent);
	hCurrent = m_wndTreeView.GetParentItem(hCurrent);	// ��ȡ���ڵ�
	while(hCurrent != m_hRoot)	// ѭ���ݹ飬ֱ���ҵ���Ŀ¼
	{
		strTemp = m_wndTreeView.GetItemText(hCurrent);
		strTemp += _T("\\");
		strReturn = strTemp  + strReturn;
		hCurrent = m_wndTreeView.GetParentItem(hCurrent);// ��ȡ���ڵ�
	}
	strReturn = _T(".\\Code\\") + strReturn;
	return strReturn;
}

void CCodeMateDlg::OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	TVITEM item = pNMTreeView->itemNew;
	if (item.hItem == m_hSelected) // ������Ѵ��ĵ�
	{
		return;
	}
	if(item.hItem == m_hRoot)// ����Ǹ�Ŀ¼������
	{
		m_bItemSelected = false;
		return;
	}
	CString strPath = ItemToPath(item.hItem);// ��ȡ�����ȫ·��
	CString strLoad, strLang, strShow;
	m_bIsFolder = false;
	int iImage, iImageSel;
	m_wndTreeView.GetItemImage(item.hItem, iImage, iImageSel);
	switch (iImage)
	{
	case 1:	// C++
		strPath += _T(".c-p");
		break;
	case 5:	// C
		strPath += _T(".c-a");
		break;
	case 17:	// C#
		strPath += _T(".c-s");
		break;
	case 7:	// HTM
		strPath += _T(".h-t");
		break;
	case 9:	// JScript
		strPath += _T(".j-s");
		break;
	case 15:	// VBScript
		strPath += _T(".v-b");
		break;
	case 11:	// SQL
		strPath += _T(".s-q");
		break;
	case 13:	// TXT
		strPath += _T(".txt");
		break;
	default:	// �ļ���
		m_bIsFolder=true;
	}
	if (!m_bIsFolder)	// ���ѡ�е����ļ�
	{
		// �����ļ�
		if (!m_wndCodeEdit.Load((LPTSTR)((LPCTSTR)strPath)))
		{
			if (m_sMyConfig.m_uiLanguage == 0)
			{
				strLoad.LoadString(IDS_MSGBOX_LOADERROR);
				strShow.LoadString(IDS_MSGBOX_ERROR);
			}
			else
			{
				SetCurrentDirectory(m_strAppPath);
				LPTSTR strFileName = NULL;
				if (m_sMyConfig.m_uiLanguage == 1)
					strFileName = LANG_TCHINESE_FILE;
				else if (m_sMyConfig.m_uiLanguage == 2)
					strFileName = LANG_ENGLISH_FILE;
				LPTSTR lpszLang = new _TCHAR[LANG_STRING_LEN];
				GetPrivateProfileString(LANG_MESSAGEBOX,
					_T("CONTENT_READ_ERROR"), _T(""),
					lpszLang, LANG_STRING_LEN, strFileName);
				strLoad = lpszLang;
				strLoad.Replace(_T("\\n"), _T("\n"));
				strShow = m_arrLanString[69];
				delete []lpszLang;
				SetCurrentDirectory(m_sMyConfig.m_strWorkPath);
			}
			MessageBox(strLoad, strShow, MB_ICONERROR);
		}
		else	// ���سɹ�
		{
			if (m_enFileType == SCT_ERROR)		// ���ԭ��û�д��ļ�
			{
				int iSBHeight = (m_sMyConfig.m_bShowStatusBar ? 18 : 0);
				int iTBHeight = (m_sMyConfig.m_bShowToolBar ? 30 : 0);
				// ��ʾ�༭�����з���
				m_wndSplitter.ShowWindow(SW_SHOW);
				m_wndCodeEdit.MoveWindow(
					200 + m_sMyConfig.m_lSplPos,
					iTBHeight, m_iWidth,
					m_iHeight - iSBHeight - iTBHeight
					);
			}
			switch (iImage)	// ����ͼ���ж��ļ�����
			{
			case 1:	// C++
				{
					strLang = _T("C++");
					m_wndCodeEdit.SetSyntaxColoring(SCT_C_PLUS_PLUS);
					m_wndCodeEdit.AddKeywords(
						(LPTSTR)(LPCTSTR)(m_sMyConfig.m_arrKeyWord[0]),
						m_sMyConfig.m_arrKeyWord[0].GetLength()
						);
					m_wndCodeEdit.AddConstants(
						(LPTSTR)(LPCTSTR)(m_sMyConfig.m_arrConstant[0]),
						m_sMyConfig.m_arrConstant[0].GetLength()
						);
					m_wndCodeEdit.UpdateSyntaxColoring();
				}
				break;
			case 5:	// C
				{
					strLang = _T("C");
					m_wndCodeEdit.SetSyntaxColoring(SCT_C_ANSI);
					m_wndCodeEdit.AddKeywords(
						(LPTSTR)(LPCTSTR)(m_sMyConfig.m_arrKeyWord[1]),
						m_sMyConfig.m_arrKeyWord[1].GetLength()
						);
					m_wndCodeEdit.AddConstants(
						(LPTSTR)(LPCTSTR)(m_sMyConfig.m_arrConstant[1]),
						m_sMyConfig.m_arrConstant[1].GetLength()
						);
					m_wndCodeEdit.UpdateSyntaxColoring();
				}
				break;
			case 17:	// C#
				{
					strLang = _T("C#");
					m_wndCodeEdit.SetSyntaxColoring(SCT_C_SHARP);
					m_wndCodeEdit.AddKeywords(
						(LPTSTR)(LPCTSTR)(m_sMyConfig.m_arrKeyWord[2]),
						m_sMyConfig.m_arrKeyWord[2].GetLength()
						);
					m_wndCodeEdit.AddConstants(
						(LPTSTR)(LPCTSTR)(m_sMyConfig.m_arrConstant[2]),
						m_sMyConfig.m_arrConstant[2].GetLength()
						);
					m_wndCodeEdit.UpdateSyntaxColoring();
				}
				break;
			case 7:	// HTM
				{
					strLang = _T("HTML");
					m_wndCodeEdit.SetSyntaxColoring(SCT_HTML);
					m_wndCodeEdit.AddKeywords(
						(LPTSTR)(LPCTSTR)(m_sMyConfig.m_arrKeyWord[3]),
						m_sMyConfig.m_arrKeyWord[3].GetLength()
						);
					m_wndCodeEdit.AddConstants(
						(LPTSTR)(LPCTSTR)(m_sMyConfig.m_arrConstant[3]),
						m_sMyConfig.m_arrConstant[3].GetLength()
						);
					m_wndCodeEdit.UpdateSyntaxColoring();
				}
				break;
			case 9:	// JScript
				{
					strLang = _T("JS");
					m_wndCodeEdit.SetSyntaxColoring(SCT_JSCRIPT);
					m_wndCodeEdit.AddKeywords(
						(LPTSTR)(LPCTSTR)(m_sMyConfig.m_arrKeyWord[4]),
						m_sMyConfig.m_arrKeyWord[4].GetLength()
						);
					m_wndCodeEdit.AddConstants(
						(LPTSTR)(LPCTSTR)(m_sMyConfig.m_arrConstant[4]),
						m_sMyConfig.m_arrConstant[4].GetLength()
						);
					m_wndCodeEdit.UpdateSyntaxColoring();
				}
				break;
			case 15:	// VBScript
				{
					strLang = _T("VBS");
					m_wndCodeEdit.SetSyntaxColoring(SCT_VBSCRIPT);
					m_wndCodeEdit.AddKeywords(
						(LPTSTR)(LPCTSTR)(m_sMyConfig.m_arrKeyWord[5]),
						m_sMyConfig.m_arrKeyWord[5].GetLength()
						);
					m_wndCodeEdit.AddConstants(
						(LPTSTR)(LPCTSTR)(m_sMyConfig.m_arrConstant[5]),
						m_sMyConfig.m_arrConstant[5].GetLength()
						);
					m_wndCodeEdit.UpdateSyntaxColoring();
				}
				break;
			case 11:	// SQL
				{
					strLang = _T("SQL");
					m_wndCodeEdit.SetSyntaxColoring(SCT_SQL);
					m_wndCodeEdit.AddKeywords(
						(LPTSTR)(LPCTSTR)(m_sMyConfig.m_arrKeyWord[6]),
						m_sMyConfig.m_arrKeyWord[6].GetLength()
						);
					m_wndCodeEdit.AddConstants(
						(LPTSTR)(LPCTSTR)(m_sMyConfig.m_arrConstant[6]),
						m_sMyConfig.m_arrConstant[6].GetLength()
						);
					m_wndCodeEdit.UpdateSyntaxColoring();
				}
				break;
			case 13:	// TXT
				{
					strLang = _T("TXT");
					m_wndCodeEdit.SetSyntaxColoring(SCT_NONE);
					m_wndCodeEdit.UpdateSyntaxColoring();
				}
				break;
			}
			// ����򿪵��ĵ��ĸ�����Ϣ
			m_strFileName = strPath;
			m_enFileType = m_wndCodeEdit.GetSyntaxColoring();
			m_hSelected=item.hItem;
			int len = strPath.GetLength();
			strPath = strPath.Mid(7,len-11);
			CString strConv;
			if (m_sMyConfig.m_uiLanguage == 0)
				strConv.LoadString(IDS_CLASS_NORMAL);
			else
				strConv = m_arrLanString[64];
			strPath = strConv + _T("\\") + strPath;
			m_strTitleShow = strPath;
			if (m_sMyConfig.m_uiLanguage == 0)
				strLoad.LoadString(ID_INDICATOR_LANG);
			else
				strLoad = m_arrLanString[58];
			strShow.Format(strLoad,strLang);
			m_wndStatusBar.SetPaneText(2,strShow);
		}
	}
	m_bItemSelected = true;	// �Ѿ�ѡ������Ч�ڵ�
	*pResult = 0;
}

void CCodeMateDlg::OnTvnSelchangingTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	TVITEM item = pNMTreeView->itemNew;
	if (item.hItem == m_hSelected) // ������Ѵ��ĵ�
	{
		return;
	}
	int image,imageselect;
	m_wndTreeView.GetItemImage(item.hItem, image, imageselect);
	if (image != 2 && image != 3)	// �����ļ���
	{
		if (m_wndCodeEdit.GetChangedStatus() == TRUE)	// �ж��ĵ��Ƿ���Ҫ����
		{
			CString strContent,strTitle;
			if (m_sMyConfig.m_uiLanguage == 0)
			{
				strContent.LoadString(IDS_MSGBOX_EXITWARN);
				strTitle.LoadString(IDS_MSGBOX_QUES);
			}
			else
			{
				SetCurrentDirectory(m_strAppPath);
				LPTSTR strFileName = NULL;
				if (m_sMyConfig.m_uiLanguage == 1)
					strFileName = LANG_TCHINESE_FILE;
				else if (m_sMyConfig.m_uiLanguage == 2)
					strFileName = LANG_ENGLISH_FILE;
				LPTSTR lpszLang = new _TCHAR[LANG_STRING_LEN];
				GetPrivateProfileString(LANG_MESSAGEBOX,
					_T("CONTENT_FILE_SAVE"), _T(""),
					lpszLang, LANG_STRING_LEN, strFileName);
				strContent = lpszLang;
				strContent.Replace(_T("\\n"), _T("\n"));
				strTitle = m_arrLanString[70];
				delete []lpszLang;
				SetCurrentDirectory(m_sMyConfig.m_strWorkPath);
			}
			int iResult = MessageBox(strContent, strTitle,
				MB_ICONQUESTION | MB_YESNOCANCEL);
			if (iResult == IDCANCEL)
				*pResult = 1;	// ȡ������
			if (iResult == IDYES)	// �����ļ�
			{
				if (!m_wndCodeEdit.Save((LPTSTR)((LPCTSTR)m_strFileName)))
				{
					if (m_sMyConfig.m_uiLanguage == 0)
					{
						strContent.LoadString(IDS_MSGBOX_SAVEERROR);
						strTitle.LoadString(IDS_MSGBOX_ERROR);
					}
					else
					{
						SetCurrentDirectory(m_strAppPath);
						LPTSTR strFileName = NULL;
						if (m_sMyConfig.m_uiLanguage == 1)
							strFileName = LANG_TCHINESE_FILE;
						else if (m_sMyConfig.m_uiLanguage == 2)
							strFileName = LANG_ENGLISH_FILE;
						LPTSTR lpszLang = new _TCHAR[LANG_STRING_LEN];
						GetPrivateProfileString(LANG_MESSAGEBOX,
							_T("CONTENT_SAVE_ERROR"), _T(""),
							lpszLang, LANG_STRING_LEN, strFileName);
						strContent = lpszLang;
						strContent.Replace(_T("\\n"), _T("\n"));
						strTitle = m_arrLanString[69];
						delete []lpszLang;
						SetCurrentDirectory(m_sMyConfig.m_strWorkPath);
					}
					MessageBox(strContent, strTitle, MB_ICONERROR);
					*pResult = 1;
				}
				else
				{
					*pResult = 0;
				}
			}
			if (iResult == IDNO)
			{
				m_wndCodeEdit.SetChangedStatus(FALSE);
				*pResult = 0;
			}
		}
	}
}

void CCodeMateDlg::OnTvnBeginlabeleditTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
	if (!m_bItemSelected)	// ���ڵ㲻����������
	{
		*pResult = 1;
	}
	else
	{
		// �����б��
		m_wndCodeEdit.DestroyList();
		// ����ȡ���༭��־Ϊ��
		m_bEscPressed = false;
		*pResult = 0;
	}
}

void CCodeMateDlg::OnTvnEndlabeleditTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
	CString strText = _T("");
	m_wndTreeView.GetEditControl()->GetWindowText(strText);

	HTREEITEM hItem = m_wndTreeView.GetSelectedItem();
	if (!hItem)
		hItem = m_hRoot;

	if (!m_bEscPressed)	// ���û�а���ESC
	{
		switch (m_usOperaType)	// �жϲ�������
		{
		case 0:	// ������
			{
				int iResult;
				iResult = IsValidFileName(strText);
				if (iResult != 0)	// ���ⲻ�����ļ���������
				{
					CString strContent,strTitle;
					if (m_sMyConfig.m_uiLanguage == 0)
					{
						strContent.LoadString(IDS_MSGBOX_RENAMEERR);
						strTitle.LoadString(IDS_MSGBOX_ERROR);
					}
					else
					{
						SetCurrentDirectory(m_strAppPath);
						LPTSTR strFileName = NULL;
						if (m_sMyConfig.m_uiLanguage == 1)
							strFileName = LANG_TCHINESE_FILE;
						else if (m_sMyConfig.m_uiLanguage == 2)
							strFileName = LANG_ENGLISH_FILE;
						LPTSTR lpszLang = new _TCHAR[LANG_STRING_LEN];
						GetPrivateProfileString(LANG_MESSAGEBOX,
							_T("CONTENT_INVILID_NAME"), _T(""),
							lpszLang, LANG_STRING_LEN, strFileName);
						strContent = lpszLang;
						strContent.Replace(_T("\\n"), _T("\n"));
						strTitle = m_arrLanString[68];
						delete []lpszLang;
						SetCurrentDirectory(m_sMyConfig.m_strWorkPath);
					}
					MessageBox(strContent, strTitle, MB_ICONWARNING);
					return;
				}
				int iImage, iImageSel;
				m_wndTreeView.GetItemImage(hItem, iImage, iImageSel);
				CString str;
				switch (iImage)
				{
				case 1:	// C++
					str = _T(".c-p");
					break;
				case 5:	// C
					str = _T(".c-a");
					break;
				case 17:	// C#
					str = _T(".c-s");
					break;
				case 7:	// HTM
					str = _T(".h-t");
					break;
				case 9:	// JScript
					str = _T(".j-s");
					break;
				case 15:	// VBScript
					str = _T(".v-b");
					break;
				case 11:	// SQL
					str = _T(".s-q");
					break;
				case 13:	// TXT
					str = _T(".txt");
					break;
				default:	// �ļ���
					str = _T("");
					break;
				}
				// �õ�Դ�ļ�����Ŀ���ļ���
				HTREEITEM hItem2 = m_wndTreeView.GetParentItem(hItem);
				CString strToPath;
				if (hItem2 != m_hRoot)
					strToPath =  ItemToPath(hItem2);
				else
					strToPath = _T(".\\Code");
				CString strOldName = m_wndTreeView.GetItemText(hItem);
				CString strFromPath;
				strFromPath = strToPath + _T("\\") + strOldName + str;
				strToPath = strToPath + _T("\\") + strText + str;
				if (strOldName == strText)
					return;	// �������û�иĶ���ȡ��
				if (!ReNameFolder(strFromPath, strToPath))
				{
					CString strContent,strTitle;
					if (m_sMyConfig.m_uiLanguage == 0)
					{
						strContent.LoadString(IDS_MSGBOX_RENAMEERR);
						strTitle.LoadString(IDS_MSGBOX_ERROR);
					}
					else
					{
						SetCurrentDirectory(m_strAppPath);
						LPTSTR strFileName = NULL;
						if (m_sMyConfig.m_uiLanguage == 1)
							strFileName = LANG_TCHINESE_FILE;
						else if (m_sMyConfig.m_uiLanguage == 2)
							strFileName = LANG_ENGLISH_FILE;
						LPTSTR lpszLang = new _TCHAR[LANG_STRING_LEN];
						GetPrivateProfileString(LANG_MESSAGEBOX,
							_T("CONTENT_RENAME_ERROR"), _T(""),
							lpszLang, LANG_STRING_LEN, strFileName);
						strContent = lpszLang;
						strContent.Replace(_T("\\n"), _T("\n"));
						strTitle = m_arrLanString[69];
						delete []lpszLang;
						SetCurrentDirectory(m_sMyConfig.m_strWorkPath);
					}
					MessageBox(strContent, strTitle, MB_ICONERROR);
				}
				else	// �����ɹ�
				{
					if (!m_bIsFolder)	// ��������ļ��У����ǵ�ǰ�򿪵��ļ�
					{
						// �����ļ��ĸ�����Ϣ
						m_strFileName = strToPath;
						int iLen = strToPath.GetLength();
						strToPath = strToPath.Mid(7, iLen-11);
						CString strConv;
						if (m_sMyConfig.m_uiLanguage == 0)
							strConv.LoadString(IDS_CLASS_NORMAL);
						else
							strConv = m_arrLanString[64];
						strToPath = strConv + _T("\\") + strToPath;
						m_strTitleShow = strToPath;
					}
					else	// �ļ���
					{
						// ����Ѵ򿪵��ļ���Ҫ�������ļ�����
						strFromPath += _T("\\");
						if (m_strFileName.Find(strFromPath) != -1)
						{
							CString strTmp = m_strFileName.Right(
								m_strFileName.GetLength() -
								strFromPath.GetLength() + 1);
							strToPath += strTmp;
							m_strFileName = strToPath;
							int iLen = strToPath.GetLength();
							strToPath = strToPath.Mid(7, iLen-11);
							CString strConv;
							if (m_sMyConfig.m_uiLanguage == 0)
								strConv.LoadString(IDS_CLASS_NORMAL);
							else
								strConv = m_arrLanString[64];
							strToPath = strConv + _T("\\") + strToPath;
							m_strTitleShow = strToPath;
						}
					}
					m_wndTreeView.SetItemText(hItem, strText);	// ���ñ༭����ı�Ϊ���
				}
			}
			break;
		case 1: // �����·���
			{
				int iResult;
				iResult = IsValidFileName(strText);
				if (iResult != 0)	// ���ⲻ�����ļ���������
				{
					CString strContent, strTitle;
					if (m_sMyConfig.m_uiLanguage == 0)
					{
						strContent.LoadString(IDS_MSGBOX_EXITWARN);
						strTitle.LoadString(IDS_MSGBOX_QUES);
					}
					else
					{
						SetCurrentDirectory(m_strAppPath);
						LPTSTR strFileName = NULL;
						if (m_sMyConfig.m_uiLanguage == 1)
							strFileName = LANG_TCHINESE_FILE;
						else if (m_sMyConfig.m_uiLanguage == 2)
							strFileName = LANG_ENGLISH_FILE;
						LPTSTR lpszLang = new _TCHAR[LANG_STRING_LEN];
						GetPrivateProfileString(LANG_MESSAGEBOX,
							_T("CONTENT_INVILID_NAME"), _T(""),
							lpszLang, LANG_STRING_LEN, strFileName);
						strContent = lpszLang;
						strContent.Replace(_T("\\n"), _T("\n"));
						strTitle = m_arrLanString[68];
						delete []lpszLang;
						SetCurrentDirectory(m_sMyConfig.m_strWorkPath);
					}
					MessageBox(strContent, strTitle, MB_ICONWARNING);
					m_wndTreeView.DeleteItem(hItem);
					m_usOperaType=0;	// ��ΪĬ�ϲ�������
					return;
				}
				// �õ��·���·��
				CString strFolderPath;
				HTREEITEM hItem2 = m_wndTreeView.GetParentItem(hItem);
				if (hItem2 != m_hRoot)
					strFolderPath = ItemToPath(hItem2);
				else
					strFolderPath= _T(".\\Code");
				strFolderPath = strFolderPath + _T("\\") + strText;
				if ( !CreateDirectory(strFolderPath, NULL) )
				{
					CString strContent, strTitle;
					if (m_sMyConfig.m_uiLanguage == 0)
					{
						strContent.LoadString(IDS_MSGBOX_ADDCLASS_ERR);
						strTitle.LoadString(IDS_MSGBOX_ERROR);
					}
					else
					{
						SetCurrentDirectory(m_strAppPath);
						LPTSTR strFileName = NULL;
						if (m_sMyConfig.m_uiLanguage == 1)
							strFileName = LANG_TCHINESE_FILE;
						else if (m_sMyConfig.m_uiLanguage == 2)
							strFileName = LANG_ENGLISH_FILE;
						LPTSTR lpszLang = new _TCHAR[LANG_STRING_LEN];
						GetPrivateProfileString(LANG_MESSAGEBOX,
							_T("CONTENT_NEW_CLASS_ERROR"), _T(""),
							lpszLang, LANG_STRING_LEN, strFileName);
						strContent = lpszLang;
						strContent.Replace(_T("\\n"), _T("\n"));
						strTitle = m_arrLanString[69];
						delete []lpszLang;
						SetCurrentDirectory(m_sMyConfig.m_strWorkPath);
					}
					MessageBox(strContent, strTitle, MB_ICONERROR);
					m_wndTreeView.DeleteItem(hItem);
				}
				else	// ���ഴ���ɹ�
				{
					m_wndTreeView.SetItemText(hItem, strText);// ���ñ༭����ı�Ϊ���
				}
				m_usOperaType=0;	// ��ΪĬ�ϲ�������
			}
			break;
		}
	}
	else
	{
		switch (m_usOperaType)
		{
		case 1: // �����·���
			{
				m_wndTreeView.DeleteItem(hItem);
			}
			break;
		}
		m_bEscPressed = false;
	}
	*pResult = 0;
}

void CCodeMateDlg::OnTvnItemexpandedTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	TVITEM item = pNMTreeView->itemNew;
	if (pNMTreeView-> action == 1) // ����
	{
		m_wndTreeView.SetItemImage(item.hItem, 3, 3);
	}
	else if (pNMTreeView-> action == 2)// չ��
	{
		m_wndTreeView.SetItemImage(item.hItem, 2, 2);
		if(item.hItem == m_hRoot)// ����Ǹ��ڵ��򷵻�
			return;
		HTREEITEM hChild = m_wndTreeView.GetChildItem(item.hItem);
		while(hChild)
		{
			if (m_wndTreeView.GetChildItem(hChild)==NULL)
				AddSubItem(hChild);// ������Ŀ¼���������
			hChild = m_wndTreeView.GetNextItem(hChild,TVGN_NEXT);// ��ȡ��һ������
		}
	}
	*pResult = 0;
}

void CCodeMateDlg::OnNMSetfocusTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// �����б��
	m_wndCodeEdit.DestroyList();
	*pResult = 0;
}

void CCodeMateDlg::OnNMRclickTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	HTREEITEM  hSelect = m_wndTreeView.GetSelectedItem(); 
	if (!hSelect)
		return;

	HTREEITEM  hLight = m_wndTreeView.GetDropHilightItem(); 
	if (hLight != NULL)		// ѡ���Ҽ��������Ŀ
	{
		if (hSelect != hLight)
			hSelect = hLight; 
	}
	m_wndTreeView.SelectItem(hSelect); 
	CMenu mnuMenu,*pSubMenu;// ��������Ҫ�õ���cmenu����  
	mnuMenu.LoadMenu(IDR_MENU_RBUTTON);// װ���Զ�����Ҽ��˵�  
	pSubMenu = mnuMenu.GetSubMenu(0);// ��ȡ��һ�������˵������Ե�һ���˵��������Ӳ˵�
	// ���ò˵�ͼ��
	if (m_bShowMenuImage == true)
	{
		pSubMenu ->SetMenuItemBitmaps (ID_FILE_ADDITEM,
			MF_BYCOMMAND, &m_bmpMenu[0], &m_bmpMenu[0]);
		pSubMenu ->SetMenuItemBitmaps (ID_FILE_ADDCLASS,
			MF_BYCOMMAND, &m_bmpMenu[1], &m_bmpMenu[1]);
	}
	// ���ز˵�����
	if (m_sMyConfig.m_uiLanguage != 0)
	{
		pSubMenu->ModifyMenu(ID_FILE_ADDITEM, MF_BYCOMMAND,
			ID_FILE_ADDITEM, m_arrLanString[16]);
		pSubMenu->ModifyMenu(ID_FILE_ADDCLASS, MF_BYCOMMAND,
			ID_FILE_ADDCLASS, m_arrLanString[17]);
		pSubMenu->ModifyMenu(ID_FILE_MOVETO, MF_BYCOMMAND,
			ID_FILE_MOVETO, m_arrLanString[18]);
		pSubMenu->ModifyMenu(ID_FILE_PREP, MF_BYCOMMAND,
			ID_FILE_PREP, m_arrLanString[19]);
		pSubMenu->ModifyMenu(ID_FILE_SEARCH, MF_BYCOMMAND,
			ID_FILE_SEARCH, m_arrLanString[20]);
	}
	CPoint oPoint;// ����һ������ȷ�����ָ��λ�õ�λ��  
	GetCursorPos(&oPoint);// ��ȡ��ǰ���ָ���λ�ã��Ա�ʹ�ò˵����Ը������ָ��  
	pSubMenu->TrackPopupMenu(TPM_LEFTALIGN,oPoint.x,oPoint.y,this);   // ��ָ��λ����ʾ�����˵�

	pSubMenu=NULL;
	delete pSubMenu;

	*pResult = 0;
}

LRESULT CCodeMateDlg::OnFindReplace( WPARAM wparam, LPARAM lparam )
{
	CFindReplaceDialog* pDlg = CFindReplaceDialog::GetNotifier(lparam);
	if( pDlg != NULL )
	{
		// ��ò����ַ���
		m_sMyConfig.strFindString = pDlg->GetFindString();
		// ����滻�ַ���
		CString cstReplaceString = pDlg->GetReplaceString();
		// �Ƿ����ִ�Сд
		m_sMyConfig.m_bMatchCase = pDlg->MatchCase();
		m_chkCase.SetCheck(m_sMyConfig.m_bMatchCase);
		// �Ƿ�ȫ��ƥ��
		m_sMyConfig.m_bMatchWholeWord = pDlg->MatchWholeWord();
		// �Ƿ����²���
		m_sMyConfig.m_bDirectionDown = pDlg->SearchDown();
		if(pDlg->FindNext())
		{
			// ��ʼ����
			if (!(m_wndCodeEdit.Find(
				(LPTSTR)(LPCTSTR)(m_sMyConfig.strFindString),
				m_sMyConfig.strFindString.GetLength(),
				m_sMyConfig.m_bMatchCase,
				m_sMyConfig.m_bMatchWholeWord,
				m_sMyConfig.m_bDirectionDown)))
			{
				if (m_sMyConfig.m_bDirectionDown)
				{
					pDlg->MessageBox(_T("�Ѳ��ҵ��ĵ�ĩβ��"),
						_T("����"), MB_ICONWARNING);
				}
				else
				{
					pDlg->MessageBox(_T("�Ѳ��ҵ��ĵ���ͷ��"),
						_T("����"), MB_ICONWARNING);
				}
			}
			else
			{
				m_bTextSelected = true;
			}
		}
		else if(pDlg->ReplaceAll())
		{
			// ��ò����ַ���
			m_sMyConfig.strFindString = pDlg->GetFindString();
			// ����滻�ַ���
			CString cstReplaceString = pDlg->GetReplaceString();
			// �Ƿ����ִ�Сд
			m_sMyConfig.m_bMatchCase = pDlg->MatchCase();
			m_chkCase.SetCheck(m_sMyConfig.m_bMatchCase);
			// �Ƿ�ȫ��ƥ��
			m_sMyConfig.m_bMatchWholeWord = pDlg->MatchWholeWord();
			// ȫ���滻
			m_wndCodeEdit.ReplaceAll(
				(LPTSTR)(LPCTSTR)(m_sMyConfig.strFindString),
				m_sMyConfig.strFindString.GetLength(),
				(LPTSTR)(LPCTSTR)(cstReplaceString),
				cstReplaceString.GetLength(),
				m_sMyConfig.m_bMatchCase,
				m_sMyConfig.m_bMatchWholeWord
				);
			MessageBox(_T("��ȫ���滻��"), _T("�滻"), MB_ICONINFORMATION);
		}
		else if(pDlg->ReplaceCurrent())
		{
			// ��ò����ַ���
			m_sMyConfig.strFindString = pDlg->GetFindString();
			// ����滻�ַ���
			CString cstReplaceString = pDlg->GetReplaceString();
			// �Ƿ����ִ�Сд
			m_sMyConfig.m_bMatchCase = pDlg->MatchCase();
			m_chkCase.SetCheck(m_sMyConfig.m_bMatchCase);
			// �Ƿ�ȫ��ƥ��
			m_sMyConfig.m_bMatchWholeWord = pDlg->MatchWholeWord();
			// �滻��ǰ��
			if (!(m_wndCodeEdit.Replace(
				(LPTSTR)(LPCTSTR)(m_sMyConfig.strFindString),
				m_sMyConfig.strFindString.GetLength(),
				(LPTSTR)(LPCTSTR)(cstReplaceString),
				cstReplaceString.GetLength(),
				m_sMyConfig.m_bMatchCase,
				m_sMyConfig.m_bMatchWholeWord)))
			{
				if (m_sMyConfig.m_bDirectionDown)
					pDlg->MessageBox(_T("���滻���ĵ�ĩβ��"), _T("�滻"), MB_ICONWARNING);
				else
					pDlg->MessageBox(_T("���滻���ĵ���ͷ��"), _T("�滻"), MB_ICONWARNING);
			}
		}
		else if(pDlg->IsTerminating())
		{
			delete pDlg;
			m_pFindDlg = NULL;
		}
	}

	return 1L;
}

void CCodeMateDlg::OnFileSearch()
{
	if (!m_bShowSearch)
	{
		m_bShowSearch = true;
		m_stcContent.ShowWindow(SW_SHOW);
		m_edtKeyWord.ShowWindow(SW_SHOW);
		m_btnSearch.ShowWindow(SW_SHOW);
		m_chkCase.ShowWindow(SW_SHOW);
		m_cboType.ShowWindow(SW_SHOW);
		m_btnHide.ShowWindow(SW_SHOW);
		int iSBHeight = (m_sMyConfig.m_bShowStatusBar ? 18 : 0);
		int iTBHeight = (m_sMyConfig.m_bShowToolBar ? 30 : 0);
		int iSearchHeight = (m_bShowSearch ? 50 : 0);
		CRect rect;
		GetClientRect(&rect);
		if (m_wndTreeView.GetSafeHwnd())		// ���οؼ�
		{
			m_wndTreeView.MoveWindow(
				0,
				iTBHeight,
				197 + m_sMyConfig.m_lSplPos,
				rect.Height() - iTBHeight - iSBHeight - iSearchHeight
				);
		}
		if (m_stcContent.GetSafeHwnd())
		{
			m_stcContent.MoveWindow(
				3,
				rect.Height() - iSBHeight - iSearchHeight + 3,
				60,
				20
				);
		}
		if (m_edtKeyWord.GetSafeHwnd())
		{
			m_edtKeyWord.MoveWindow(
				63,
				rect.Height() - iSBHeight - iSearchHeight + 3,
				m_sMyConfig.m_lSplPos + 80,
				20
				);
		}
		if (m_btnSearch.GetSafeHwnd())
		{
			m_btnSearch.MoveWindow(
				197 + m_sMyConfig.m_lSplPos - 53,
				rect.Height() - iSBHeight - iSearchHeight + 3,
				50,
				20
				);
		}
		if (m_chkCase.GetSafeHwnd())
		{
			m_chkCase.MoveWindow(
				3,
				rect.Height() - iSBHeight - iSearchHeight + 26,
				80,
				20
				);
			m_chkCase.Invalidate();
		}
		if (m_cboType.GetSafeHwnd())
		{
			m_cboType.MoveWindow(
				83,
				rect.Height() - iSBHeight - iSearchHeight + 26,
				m_sMyConfig.m_lSplPos + 60,
				20
				);
			m_cboType.Invalidate();
		}
		if (m_btnHide.GetSafeHwnd())
		{
			m_btnHide.MoveWindow(
				197 + m_sMyConfig.m_lSplPos - 53,
				rect.Height() - iSBHeight - iSearchHeight + 26,
				50,
				20
				);
			m_btnHide.Invalidate();
		}
	}
	if (m_edtKeyWord.GetSafeHwnd())
		m_edtKeyWord.SetFocus();
}

void CCodeMateDlg::OnBnClickedButtonHide()
{
	m_bShowSearch = false;
	m_stcContent.ShowWindow(SW_HIDE);
	m_edtKeyWord.ShowWindow(SW_HIDE);
	m_btnSearch.ShowWindow(SW_HIDE);
	m_chkCase.ShowWindow(SW_HIDE);
	m_cboType.ShowWindow(SW_HIDE);
	m_btnHide.ShowWindow(SW_HIDE);
	int iSBHeight = (m_sMyConfig.m_bShowStatusBar ? 18 : 0);
	int iTBHeight = (m_sMyConfig.m_bShowToolBar ? 30 : 0);
	int iSearchHeight = (m_bShowSearch ? 50 : 0);
	CRect rect;
	GetClientRect(&rect);
	if (m_wndTreeView.GetSafeHwnd())		// ���οؼ�
	{
		m_wndTreeView.MoveWindow(
			0,
			iTBHeight,
			197 + m_sMyConfig.m_lSplPos,
			rect.Height() - iTBHeight - iSBHeight - iSearchHeight
			);
	}
}

void CCodeMateDlg::OnBnClickedButtonSearch()
{
	// ��ʼ����
	m_sMyConfig.m_bMatchCase = m_chkCase.GetCheck();
	CString strItem;
	m_edtKeyWord.GetWindowText(strItem);
	if (strItem.IsEmpty())
	{
		CString strLoad;
		if (m_sMyConfig.m_uiLanguage == 0)
		{
			strItem.LoadStringW(IDS_MSGBOX_PL_INPUT);
			strLoad.LoadString(IDS_MSGBOX_WARN);
		}
		else
		{
			SetCurrentDirectory(m_strAppPath);
			LPTSTR strFileName = NULL;
			if (m_sMyConfig.m_uiLanguage == 1)
				strFileName = LANG_TCHINESE_FILE;
			else if (m_sMyConfig.m_uiLanguage == 2)
				strFileName = LANG_ENGLISH_FILE;
			LPTSTR lpszLang = new _TCHAR[LANG_STRING_LEN];
			GetPrivateProfileString(LANG_MESSAGEBOX,
				_T("CONTENT_PLEASE_INPUT"), _T(""),
				lpszLang, LANG_STRING_LEN, strFileName);
			strItem = lpszLang;
			strItem.Replace(_T("\\n"), _T("\n"));
			strLoad = m_arrLanString[68];
			delete []lpszLang;
			SetCurrentDirectory(m_sMyConfig.m_strWorkPath);
		}
		MessageBox(strItem, strLoad, MB_ICONWARNING);
		m_edtKeyWord.SetFocus();
		return;
	}
	int iTypeSel = m_cboType.GetCurSel();
	switch (iTypeSel)
	{
	case 0:
		iTypeSel = -1;
		break;
	case 1:
		iTypeSel = 2;
		break;
	case 2:
		iTypeSel = 1;
		break;
	case 3:
		iTypeSel = 5;
		break;
	case 4:
		iTypeSel = 17;
		break;
	case 5:
		iTypeSel = 7;
		break;
	case 6:
		iTypeSel = 9;
		break;
	case 7:
		iTypeSel = 15;
		break;
	case 8:
		iTypeSel = 11;
		break;
	case 9:
		iTypeSel = 13;
		break;
	}
	HTREEITEM hItem = m_wndTreeView.GetSelectedItem();
	if (!hItem)
		hItem = m_hRoot;

	int iImage, iImageSel;
	m_wndTreeView.GetItemImage(hItem, iImage, iImageSel);
	if (iImage == 2 || iImage == 3)
	{
		SearchTreeItem(
			hItem,
			strItem,
			iTypeSel,
			(m_sMyConfig.m_bMatchCase != FALSE)
			);
	}
	else
	{
		SearchTreeItem(
			m_wndTreeView.GetParentItem(hItem),
			strItem,
			iTypeSel,
			(m_sMyConfig.m_bMatchCase != FALSE)
			);
	}
}

void CCodeMateDlg::SearchTreeItem(HTREEITEM hItem,
	CString strText, int iIconType, bool bMatchCase)
{
	if(hItem == NULL)
		return;

	while(hItem != NULL)
	{
		int iImage, iImageSel;
		m_wndTreeView.GetItemImage(hItem, iImage, iImageSel);

		CString strTitle = m_wndTreeView.GetItemText(hItem);
		// ��������ִ�Сд��ȫ��ת�ɴ�д���бȽ�
		if (!bMatchCase)
		{
			strTitle.MakeUpper();
			strText.MakeUpper();
		}
		// �������ƥ��
		if ((iIconType == 2 && iImage == 3) ||
			iIconType == iImage ||
			iIconType == -1)
		{
			// �����ִ����ҵ���Ӵ���ʾ
			if(strTitle.Find(strText) != -1)
				m_wndTreeView.SetItemState(hItem, TVIS_BOLD, TVIS_BOLD);
			else
				m_wndTreeView.SetItemState(hItem, 0, TVIS_BOLD);
		}
		else
			m_wndTreeView.SetItemState(hItem, 0, TVIS_BOLD);
		// ����Ƿ��࣬��չ��
		if(iImage == 2 || iImage == 3)
		{
			m_wndTreeView.Expand(hItem, TVE_EXPAND);
			hItem = m_wndTreeView.GetChildItem(hItem);
			SearchTreeItem(hItem, strText, iIconType, bMatchCase);
			hItem = m_wndTreeView.GetNextSiblingItem(
				m_wndTreeView.GetParentItem(hItem)
				);
		}
		else
		{
			hItem = m_wndTreeView.GetNextSiblingItem(hItem);
			if(hItem == NULL)
				return;
		}
	}
}