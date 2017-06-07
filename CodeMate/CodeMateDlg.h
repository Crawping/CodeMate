
// CodeMateDlg.h : ͷ�ļ�
// 

#include "CodeEdit.h"
#include "MyToolBar.h"
#include "Splitter.h"

#pragma once

const int WM_FINDREPLACE = ::RegisterWindowMessage(FINDMSGSTRING);	// ��FINDMSGSTRINGע��ΪWM_FINDREPLACE��Ϣ
#define IDC_EDITOR 0x10000	// �༭��ID
#define IMG_COUNT 10		// ��������ť����
#define LIMITED_LEFT 200	// �����������߽糤��
#define LIMITED_RIGHT 200	// ����������ұ߽糤��
#define MINDLG_HEIGHT 200	// �Ի�����С�߶�
#define MINDLG_WIDTH 500	// �Ի�����С���

// ����������Ϣ���ඨ��
class CSetting
{
public:
	// ��Ա����
	UINT m_uiLanguage;		// ��������
	CString m_strSkin;		// ����Ƥ��
	BOOL m_bMaximize;		// �Ƿ����
	CRect m_recWinPos;		// ����λ��
	long m_lSplPos;			// �з���λ��
	BOOL m_bShowToolBar;	// ��ʾ������
	BOOL m_bShowStatusBar;	// ��ʾ״̬��
	CString m_strWorkPath;	// ����Ŀ¼
	BOOL m_bShowLineNum;	// ��ʾ�к�
	UINT m_iTabLen;			// Tab ����
	CString m_strFontName;	// ��������
	int m_iHeight;			// ����߶�
	int m_iSize;			// �ֺ�
	BOOL m_bBold;			// ����
	BOOL m_bItalic;			// б��
	int m_iCharSet;			// �ַ���
	UINT m_uiDefType;		// Ĭ���ļ�����
	UINT m_uiDefEncode;		// Ĭ���ļ�����
	COLORINFO m_coColor;	// �û��Զ�����ɫ
	BOOL m_bMatchCase;		// ���ִ�Сд
	BOOL m_bMatchWholeWord;	// ȫ��ƥ��
	BOOL m_bDirectionDown;	// ���²���
	CString strFindString;	// �����ַ���
	CStringArray m_arrKeyWord;	// �ؼ����б�����
	CStringArray m_arrConstant;	// �����б�����

public:
	// ��Ա����
	void SaveSettings(LPCTSTR lpszFileName);	// ����������Ϣ
	void LoadSettings(LPCTSTR lpszFileName);	// ����������Ϣ
};

// CCodeMateDlg �Ի���
class CCodeMateDlg : public CDialogEx
{
	// ����
public:
	CCodeMateDlg(CWnd* pParent = NULL);	// ��׼���캯��
	~CCodeMateDlg();	// ��������

	// �Ի�������
	enum { IDD = IDD_CODEMATE_MAIN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


	// ʵ��
protected:
	HICON m_hIcon;		// ͼ��
	CMenu* m_pMainMenu;	// �˵�
	HTREEITEM m_hRoot;	// ������ڵ�
	HTREEITEM m_hSelected;	// �Ѵ򿪵��ļ��ڵ�
	int m_iWidth,m_iHeight;	// ��ʾ�༭��ʱ��Ҫ�ָ���ȷ�Ŀ�Ⱥ͸߶�
	CString m_strAppTitle;	// Ӧ�ó������
	CString m_strAppPath;	// Ӧ�ó���Ŀ¼
	CString m_strFileName;	// ��ǰ���ļ�����
	CString m_strTitleShow;	// ������Ҫ��ʾ���ļ�·��
	SYNTAX_COLORING_TYPE m_enFileType;		// ��ǰ�򿪵��ļ����ͣ����Ϊ-1��û�д��ļ�
	bool m_bTextSelected;	// �Ƿ��ڱ༭����ѡ���ı�
	bool m_bItemSelected;	// �Ƿ������οؼ���ѡ����ʵ�ʵķ������Ŀ
	bool m_bIsFolder;		// ��ǰѡ�����Ƿ�Ϊ���ࣨ�ļ��У�
	bool m_bEscPressed;		// ������ʱ�Ƿ���ESC��ȡ������
	unsigned short m_usOperaType;		// ��־���οؼ��༭�ı��Ĳ����������������½����࣬��������
	POINT m_ptLineStart,m_ptLineEnd;	// �з����ƶ�ʱ��ͼ�����ĵ������յ�
	long m_point_x;		// �����з����ƶ�ʱԭ����λ��
	bool m_bMoving;		// ��־�з����Ƿ������ƶ�
	CFindReplaceDialog *m_pFindDlg;	// �����滻�Ի���ָ��

	HACCEL hAcc;
	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();	// ��ʼ���Ի���
	virtual BOOL ContinueModal();	// ���ڷ���״̬��Ϣ��������
	virtual BOOL PreTranslateMessage(MSG* pMsg);	// ���巭����Ϣ֮ǰ�Ĳ���
	afx_msg HCURSOR OnQueryDragIcon();				// ���û��϶���С������ʱϵͳ���ô˺���ȡ�ù����ʾ��
	afx_msg void OnMove(int x, int y);					// ����λ�øı���ô˺��������Ա��洰��λ����Ϣ
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);	// �����С���ڱ仯���ô˺������������ƴ�����С�Ĵ�С
	afx_msg void OnSize(UINT uiType, int iX, int iY);	// �����С�仯���ô˺��������Ա��洰�ڴ�С��Ϣ
	afx_msg BOOL OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult);		// ��ʾ��������ʾ��Ϣ
	afx_msg void OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu);	// ����ƶ����˵��ϣ���״̬����ʾ��ʾ��Ϣ
	afx_msg void OnInitMenuPopup(CMenu *pPopupMenu, UINT nIndex, BOOL bSysMenu);		// ��ʼ���˵�״̬
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);		// �����������ָ��ʱ���ô˺�����������ʾ״̬����ʾ
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);		// ���´�����ⰴť��ϵͳ�˵����ô˺���
	afx_msg void OnPaint();		// ����滭
	afx_msg void OnSkinList(UINT nIdMenuCmd);
	LRESULT OnThemeChanged();	// ϵͳ����ı�

	DECLARE_MESSAGE_MAP()

public:
	bool m_bFocused;			// �༭���Ƿ��ý���
	bool m_bShowMenuImage;		// �Ƿ���ʾ�˵�ͼ��
	bool m_bShowSearch;			// �Ƿ���	ʾ������
	bool m_bSkinLoad;			// �Ƿ����Ƥ��
	CSetting m_sMyConfig;		// ʵ��������
	CImageList m_wndImageList;		// ����ͼ���б�ؼ�
	CBitmap m_bmpMenu[IMG_COUNT];	// ����˵�ͼƬ
	CCodeEdit m_wndCodeEdit;	// ����༭���ؼ�
	CStatusBar m_wndStatusBar;	// ����״̬���ؼ�
	CMyToolBar m_wndToolBar;	// ���幤�����ؼ�
	CTreeCtrl m_wndTreeView;	// �������οؼ�
	CButton m_btnSearch;		// ����������ť
	CEdit m_edtKeyWord;			// ���������ؼ���
	CStatic m_stcContent;		// ��ʾ��ʾ�ľ�̬�ؼ�
	CButton m_chkCase;			// �Ƿ����ִ�Сд�ĸ�ѡ��
	CComboBox m_cboType;		// ��������������
	CButton m_btnHide;			// �������ذ�ť
	CSplitter m_wndSplitter;	// �����з����ؼ�
	CStringArray m_arrLanString;		// ��������ַ���������
	void inline ShowKeyBoardState();	// �������������жϼ���NUMLOCK��CAPSLOCK��SCROLLLOCK��״̬������ʾ��״̬��
	void inline AddBitmap(CBitmap &bm, UINT uiBmpId);	// ����������������ͼƬ��Դ���ڴ�
	BOOL LoadSkinMenu();								// ���ض�̬Ƥ���˵�
	BOOL SetMainLanguage(UINT uiLanguage, LPCTSTR lpszFileName);				// ��������������
	void ShowInterfaceLanguage(LPCTSTR lpszFileName);							// ��ָ�������ַ�����ʾ��������
	void GetLanguageString(UINT nItemID, LPTSTR strText, bool bTooltip);		// ��ȡ������Ҫ�õ����ض����Ե��ַ���
	void SetMenuImage(bool bFirst, bool bDelImg);								// ���ò˵�ͼ��
	void GetListImage(CImageList &ImageList, CBitmap &Bitmap, int nImageIdx);	// ��ͼ���б��л��ͼƬ��Ϣ
	void OnSplitterDown(UINT nID, CPoint point);	// �����з���
	void OnSplitterUp(UINT nID, CPoint point);		// �ſ��з���
	void OnSplitterMove(UINT nID, CPoint point);	// �ƶ��з���
	void SetPaneText(UINT nID = 0);					// ����״̬������
	void GetMainClass(HTREEITEM hParent, LPCTSTR strText);	// ��һ�������Ϣ����ӵ����οؼ�
	void GetClassDir(HTREEITEM hParent);					// ��ȡ�ڶ���������Ϣ����ӵ����οؼ�
	void AddSubItem(HTREEITEM hParent);						// Ϊ���οؼ������һ������
	CString ItemToPath(HTREEITEM hCurrent);					// �����οؼ���Ŀת��Ϊȫ·��
	HTREEITEM PathToItem(CString strFullPath);					// ��ȫ·��ת��Ϊ���οؼ���Ӧ��Ŀ
	bool DeleteDirectory(CString strDirName);					// ɾ���ļ���
	bool ReNameFolder(CString strFromPath,CString strToPath);	// �������ļ����ļ���
	bool MoveFolder(CString strFromPath,CString strToPath);		// �ƶ��ļ����ļ���
	void SearchTreeItem(HTREEITEM hItem, CString strText, int iIconType, bool bMatchCase);	// �����������Ŀ
	afx_msg LRESULT OnFindReplace(WPARAM wParam, LPARAM lParam);	// ���Һ��滻�Ի�����Ӧ����
	afx_msg void OnMainAbout();		// ��ʾ�����ڡ��Ի���
	afx_msg void OnFileExit();		// �˳�����
	afx_msg void OnFileAdditem();	// �����Ŀ
	afx_msg void OnFileAddclass();	// ��ӷ���
	afx_msg void OnFileReload();	// ����������Ŀ�ͷ���
	afx_msg void OnClearClass();	// ������з������Ŀ
	afx_msg void OnFileSave();		// ���浱ǰ�ļ�
	afx_msg void OnFileSaveAs();	// ������ǰ�ļ�
	afx_msg void OnFileClose();		// �رյ�ǰ�ļ�
	afx_msg void OnRenameItem();	// ������ѡ�е���Ŀ�����
	afx_msg void OnDeleteItem();	// ɾ��ѡ�е���Ŀ�����
	afx_msg void OnFileMoveto();	// �ƶ�ѡ�е���Ŀ�����
	afx_msg void OnFilePrep();		// �鿴ѡ�е���Ŀ����������
	afx_msg void OnFileSearch();	// ��ʾ������
	afx_msg void OnEditUndo();		// �����༭������
	afx_msg void OnEditCut();		// ����ѡ���ı�
	afx_msg void OnEditCopy();		// ����ѡ���ı�
	afx_msg void OnEditPaste();		// ճ���������ı�
	afx_msg void OnEditClear();		// ɾ��ѡ���ı�
	afx_msg void OnEditSelall();	// ѡ��ȫ���ı�
	afx_msg void OnEditClearAll();	// ��ձ༭��
	afx_msg void OnEditFind();		// ��ʾ���ҶԻ���
	afx_msg void OnEditReplace();	// ��ʾ�滻�Ի���
	afx_msg void OnEditFindNext();	// ������һ��ƥ���ַ���
	afx_msg void OnEditGoto();		// �ı���λ��ת���ض���
	afx_msg void OnALine();			// ���һ��
	afx_msg void OnDelALine();		// ɾ��һ��
	afx_msg void OnEditShowlist();	// ��ʾ�Զ�����б�
	afx_msg void OnEditChangeType();// ���ĵ�ǰ�ļ�������
	afx_msg void OnSetOption();		// �����öԻ���
	afx_msg void OnViewToolbar();	// ��ʾ�����ع�����
	afx_msg void OnViewStatusBar();	// ��ʾ������״̬��
	afx_msg void OnLanChns();		// ���ý�������Ϊ��������
	afx_msg void OnLanChnt();		// ���ý�������Ϊ��������
	afx_msg void OnLanEng();		// ���ý�������ΪӢ��
	afx_msg void OnHelp();			// ��ʾ�����ļ�
	// ����Ϊ�˵��͹���������״̬���º���
	afx_msg void OnUpdateFileSave(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFileSaveAs(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFileClose(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFilePrep(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFileMoveTo(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileDeleteItem(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileRenameItem(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditUndo(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditCopy(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditPaste(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditClear(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditSelall(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditClearAll(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditFind(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditFindNext(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditReplace(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditGoto(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditShowlist(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditChangeType(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewToolbar(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewStatusBar(CCmdUI *pCmdUI);
	afx_msg void OnUpdateLanChns(CCmdUI *pCmdUI);
	afx_msg void OnUpdateLanChnt(CCmdUI *pCmdUI);
	afx_msg void OnUpdateLanEng(CCmdUI *pCmdUI);
	// ����Ϊ�˵��͹���������״̬���º���
	afx_msg void OnNMRclickTree1(NMHDR *pNMHDR, LRESULT *pResult);			// ���οؼ���������Ҽ�
	afx_msg void OnNMSetfocusTree1(NMHDR *pNMHDR, LRESULT *pResult);		// ���οؼ���ý���
	afx_msg void OnTvnSelchangingTree1(NMHDR *pNMHDR, LRESULT *pResult);	// ���οؼ����ڸı�ѡ��
	afx_msg void OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult);		// ���οؼ��ı�ѡ��
	afx_msg void OnTvnBeginlabeleditTree1(NMHDR *pNMHDR, LRESULT *pResult);	// ���οؼ���ʼ�༭�ı�
	afx_msg void OnTvnEndlabeleditTree1(NMHDR *pNMHDR, LRESULT *pResult);	// ���οؼ������༭�ı�
	afx_msg void OnTvnItemexpandedTree1(NMHDR *pNMHDR, LRESULT *pResult);	// ���οؼ�չ������
	afx_msg void OnBnClickedButtonHide();		// ����������
	afx_msg void OnBnClickedButtonSearch();		// ����
};