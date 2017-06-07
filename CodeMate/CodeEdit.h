
// CodeEdit.h : ͷ�ļ�
// ���� CUltraEditWnd �Ľ�
// ��鿴 http://www.codeproject.com/KB/edit/CUltraPadWnd.aspx
// ��ԴЭ�� CPOL http://www.codeproject.com/info/cpol10.aspx
// 
#pragma once


typedef enum _SYNTAX_COLORING_TYPE	// ��������
{
	SCT_ERROR = -1,		// û�д��ĵ�
	SCT_NONE = 0,		// ��ͨ�ı�
	SCT_C_PLUS_PLUS,	// C++ ����
	SCT_C_ANSI,			// ANSI ��׼ C ����
	SCT_C_SHARP,		// C# ����
	SCT_HTML,			// HTML ����
	SCT_JSCRIPT,		// Java Script �ű�
	SCT_VBSCRIPT,		// VB Script �ű�
	SCT_SQL				// SQL ����
} SYNTAX_COLORING_TYPE;

typedef enum _UNDO_OPERATION_TYPE	// ������������
{
	UOT_NONE = 0,		// ��Ч����
	UOT_BACK,			// �˸�
	UOT_CHAR,			// ����һ���ַ�
	UOT_ADDLINE,		// ���һ��
	UOT_DELETE,			// ɾ���ַ�
	UOT_ADDTEXT,		// ����ַ���
	UOT_DELTEXT,		// ɾ���ַ���
} UNDO_OPERATION_TYPE;

typedef enum _FILE_ENCODING_TYPE	// ��������
{
	FET_ANSI = 0,		// ANSI ����
	FET_UNICODE,		// Unicode ����
	FET_UNICODE_BE,		// Unicode big endian ����
	FET_UTF_8			// UTF-8 ����
} FILE_ENCODING_TYPE;

typedef struct _LINEINFO	// �༭������Ϣ
{
	DWORD dwOffset;		// ƫ����
	int nLen;			// ����
} LINEINFO, *LPLINEINFO;

typedef struct _TAGINFO		// �ؼ���
{
	LPTSTR lpszTagName;		// ����
	int nLen;				// ����
} TAGINFO, *LPTAGINFO;

typedef struct _UNDOINFO	// �����ṹ��Ϣ
{
	LPTSTR lpszUndoData;				// ��������
	int iLastLineLen;					// ��ԭʼ����
	int nLen;							// �ı�����
	int iLine;							// ԭ��������к�
	int iChar;							// ԭ��������к�
	int iEndLine;						// ԭѡ��ĩβ�����к�
	int iEndChar;						// ԭѡ��ĩβ�����к�
	UNDO_OPERATION_TYPE nOperation;		// ��һ�β���
} UNDOINFO;

typedef struct _COLORINFO	// ���������ɫ��Ϣ
{
	COLORREF crLineNumBGColor;
	COLORREF crLineNumFGColor;
	COLORREF crKeywordColor;
	COLORREF crConstantColor;
	COLORREF crCommentColor;
	COLORREF crTextColor;
	COLORREF crForegroundColor;
	COLORREF crBackgroundColor;
} COLORINFO;

// �л滭����
typedef void (*LPLINEDRAWPROC)(HDC hDC, LPTSTR lpszText, int iLen, RECT rect, int iSelectionStart, int iSelectionEnd);

#define IDC_LIST_AUTOCOMPLETE 0x10001	// �����Զ�����б��Ŀؼ�ID
#define LIST_HEIGHT	150		// �б��߶�
#define LIST_WIDTH 150		// �б����
#define LINE_NUMBER_TO_EDITOR 5			// �����кźͱ༭���ľ���

// CCodeEdit
class CCodeEdit : public CWnd
{
	DECLARE_DYNAMIC(CCodeEdit)

// ����
public:
	CCodeEdit();			// ���캯��
	virtual ~CCodeEdit();	// ��������
	BOOL Create(DWORD dwExStyle, DWORD dwStyle, RECT& rect, CWnd* pParentWnd, UINT nID);	// �����ؼ�

// ʵ��
public:
	// ��������
	BOOL Load(LPTSTR lpszFilePath);		// �����ļ�
	BOOL Save(LPTSTR lpszFilePath);		// �����ļ�
	BOOL GetChangedStatus();			// ����ĵ��Ƿ�ı�ı�־
	void SetChangedStatus(BOOL status);	// �����ĵ��Ƿ�ı�ı�־
	BOOL GetUndoStatus();				// ��ñ༭���Ƿ���Գ����ı�־
	void Undo();						// ��������
	BOOL Find(LPTSTR lpszText, int iLen, BOOL bMatchCase, BOOL bMatchWhole, BOOL bDirectionDown);				// �����ı�
	BOOL Replace(LPTSTR lpszText, int iLen, LPTSTR lpszText2, int iLen2, BOOL bMatchCase, BOOL bMatchWhole);	// �滻�ı�
	void ReplaceAll(LPTSTR lpszText, int iLen, LPTSTR lpszText2, int iLen2, BOOL bMatchCase, BOOL bMatchWhole);	// ȫ���滻
	void AddText(LPTSTR lpszText, int iLine, int iChar, int iLen);					// ����ı����ض�λ��
	void DeleteText(int iStartLine, int iStartChar, int iEndLine, int iEndChar);	// ɾ���ı�
	void ClearText();					// ��ձ༭��
	void DestroyList();					// �����б��
	LPTSTR GetTextBuffer();				// ����ı�����������
	int GetBufferLen();					// ����ı�����������
	void GetTextFromLine(int iLineIndex, LPTSTR lpszText);					// ��õ� iLineIndex �е��ı�
	void SetEditorFont(LPTSTR lpszFacename, LONG iHeight, BOOL bBold, BOOL bItalic, BYTE uiCharSet);	// ��������
	void SetKeywordColor(COLORREF crKeywordColor);							// ���ùؼ��ʵ���ɫ
	COLORREF GetKeywordColor();												// ��ùؼ�����ɫ
	void AddKeywords(LPTSTR lpszKeywords, int iLen);						// ��ӹؼ���
	void SetConstantColor(COLORREF crConstantColor);						// ���ó�����ɫ
	COLORREF GetConstantColor();											// ��ó�����ɫ
	void AddConstants(LPTSTR lpszConstants, int iLen);						// ��ӳ���
	void SetCommentColor(COLORREF crCommentColor);							// ����ע����ɫ
	COLORREF GetCommentColor();												// ���ע����ɫ
	void SetTextColor(COLORREF crTextColor);								// �����ַ�����ɫ
	COLORREF GetTextColor();												// ����ַ�����ɫ
	void SetNormalCodeColor(COLORREF crTextColor);							// ������ͨ������ɫ
	COLORREF GetNormalCodeColor();											// �����ͨ������ɫ
	void SetColorStruct(COLORINFO coColor);									// ����������ɫ��Ϣ
	void GetColorStruct(COLORINFO &coColor);								// ��ȡ������ɫ��Ϣ
	void SetDefaultFileEncoding(FILE_ENCODING_TYPE fileEncoding);			// ����Ĭ�ϱ������
	void SetSyntaxColoring(SYNTAX_COLORING_TYPE syntaxColoring);			// �����﷨��ɫ����
	SYNTAX_COLORING_TYPE GetSyntaxColoring();								// ����﷨��ɫ����
	void UpdateSyntaxColoring();											// �����﷨��ɫ��ʾ
	void SetCurrentLine(int index);											// ���õ�ǰ�кţ���ת��
	int GetCurrentLine();													// ��õ�ǰ�к�
	void SetCurrentChar(int index);											// ���õ�ǰ�ַ����
	int GetCurrentChar();													// ��õ�ǰ�ַ����
	int GetCurrentColumn();													// ��õ�ǰ�к�
	void SetTABs(int iTABs);												// ���� Tab ����
	int GetTABs();															// ��� Tab ����
	void EnsureVisible();																// ȷ���༭�����ɼ�
	void SetSelection(int iStartLine, int iStartChar, int iEndLine, int iEndChar);		// ����ѡ��
	void GetSelection(int& iStartLine, int& iStartChar, int& iEndLine, int& iEndChar);	// ���ѡ��
	int GetNumberLines();							// �������
	int GetLineLen(int LineIndex);					// �����Ӧ������ַ���
	void SetBackgroundColor(COLORREF cBgColor);		// ���ñ���ɫ
	COLORREF GetBackgroundColor();					// ��ñ���ɫ
	void SetLineNumberBackgroundColor(COLORREF cBgColor);		// �����кű���ɫ
	COLORREF GetLineNumberBackgroundColor();					// ����кű���ɫ
	void SetLineNumberForegroundColor(COLORREF cBgColor);		// �����кű���ɫ
	COLORREF GetLineNumberForegroundColor();					// ����кű���ɫ
	void SetShowLineNumberStatus(BOOL isShowed);				// �����Ƿ���ʾ�к�
	void ShowAutoComplete();								// ��ʾ�Զ�����б�
	void SetLineDrawProc(LPLINEDRAWPROC lpfnLineDrawProc);														// �����Ի�����ʾ����
	void DefaultLineDrawProc(LPTSTR lpszText, int iLen, RECT rect, int iSelectionStart, int iSelectionEnd);		// Ĭ�ϻ�������ʾ����

private:
	// ˽�з���
	void UpdateControl();		// ���¿ؼ�
	void DeleteText();			// ɾ��ѡ���ı�
	void DelText(int iLine, int iChar, int iEndLine, int iEndChar);					// ɾ���ض�λ���ı�
	void DelChar(int iLine, int iChar);					// ɾ������һ���ַ�
	int BackChar(int iLine, int iChar);					// ɾ��ǰ��һ���ַ�
	void AddChar(int iLine, int iChar, UINT nChar);		// ���һ���ַ�
	void AddLine(int iLine, int iChar);					// ���һ��
	SIZE GetLineSize(LPTSTR lpszText, int iLen);	// ����гߴ�
	void DrawContent();																				// ��������
	void DrawLine(LPTSTR lpszText, int iLen, RECT rect, int iSelectionStart, int iSelectionEnd);	// ������
	void ProcessLine(LPTSTR lpszText, int iLen);	// �Ե����ı������﷨��ɫ���
	int LineFromPosition(POINT pt);							// ������λ�û���к�
	int CharFromPosition(int iLineIndex, POINT pt);			// ������λ�û���ַ����
	POINT PositionFromChar(int iLineIndex, int iCharIndex);	// ���ַ���Ż������λ��
	int GetCharOffset(int iLineIndex, int iCharIndex);		// ����ַ�ƫ����
	int GetCharIndex(int iLine, int iColumn);				// ���кŻ���ַ����
	int GetColumnIndex(int iLine, int iChar);				// ���ַ���Ż���к�
	void UpdateCaret();										// ���¹����ʾ
	void EnsureVisible(int iLineIndex, int iCharIndex);		// ȷ����ʾ���
	LPTSTR GetLine(int& iOffset);							// ���������
	void SelectWord(POINT pt, BOOL FirstLetter);			// ѡ�񵥴�
	BOOL IsSpecial(_TCHAR tch);								// �Ƿ�Ϊ�����ַ�
	BOOL IsLetter(_TCHAR tch);								// �Ƿ�����ĸ
	BOOL IsCJKChar(_TCHAR tch);								// �Ƿ��Ǻ���
	BOOL IsKeyword(LPTSTR lpszText, int iLen, int& iKeywordIndex, int& iKeywordOffset);		// �Ƿ��ǹؼ���
	BOOL IsConstant(LPTSTR lpszText, int iLen, int& iConstantIndex, int& iConstantOffset);	// �Ƿ��ǳ���
	BOOL IsText(LPTSTR lpszText, int iLen, int& iTextStart, int& iTextEnd);					// �Ƿ����ַ���
	void SetKeywords(LPTSTR lpszKeywords, int iLen, BOOL bCaseSensitive, BOOL bUper);	// ���ùؼ���
	void SetConstants(LPTSTR lpszConstants, int iLen, BOOL bCaseSensitive, BOOL bUper);	// ���ó���

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);			// �����༭������
	afx_msg void OnPaint();							// �༭������
	afx_msg void OnSize(UINT nType, int cx, int cy);// �༭��������С
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);	// �༭����ͼ��ֱ����
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);	// �༭����ͼˮƽ����
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);			// �����ֹ���
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);						// ����������
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);						// ����ƹ�~�ң�
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);						// ����Ҽ�����
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);						// ����������
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);					// ������˫��
	afx_msg void OnSetFocus(CWnd* pOldWnd);										// �༭����ý���
	afx_msg void OnKillFocus(CWnd* pOldWnd);									// �༭��ʧȥ����
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);				// �а�������
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);				// �а�������
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);					// ���ַ�������
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);			// �������ָ��
	afx_msg void OnLbnDblclkListAutoComplete();									// �Զ�����б�ؼ�˫���¼�

private:
	CDC m_MemDC;				// ���Ʊ༭����ͼ�ζ���
	CBitmap m_MemBitmap;		// �����ڴ�λͼ����
	CBitmap* m_pOldMemBitmap;	// ����ɵ��ڴ�λͼ����
	CFont m_Font;				// ����
	CFont* m_pOldFont;			// ������
	LPTSTR m_lpszText;			// ����༭�������ı���Ϣ
	int m_iSize;				// �ı�����
	CSize m_szTextSize;			// �ַ��ߴ�
	CSize m_szCJKTextSize;		// CJK�ַ��ߴ�
	int m_iLineStartSelection;	// ѡ����ʼ���к�
	int m_iLineEndSelection;	// ѡ���������к�
	int m_iCharStartSelection;	// ѡ����ʼ���ַ����
	int m_iCharEndSelection;	// ѡ���������ַ����
	int m_iNumberLines;			// ������
	UNDOINFO m_UndoInfo;		// ������Ϣ
	LPLINEINFO m_lpLineInfo;	// ����Ϣ
	COLORREF m_crKeywordColor;	// �ؼ�����ɫ
	LPTAGINFO m_lpKeywordInfo;	// �ؼ�����Ϣ
	int m_iNumberKeywords;		// �ؼ�������
	BOOL m_bKeywordsCaseSensitive;	// �ؼ����Ƿ��Сд����
	BOOL m_bUper;					// ת��Ϊ��д����Сд
	COLORREF m_crConstantColor;		// ������ɫ
	LPTAGINFO m_lpConstantInfo;		// ������Ϣ
	int m_iNumberConstants;			// ��������
	BOOL m_bConstantsCaseSensitive;	// �����Ƿ��Сд����
	COLORREF m_crCommentColor;		// ע����ɫ
	COLORREF m_crTextColor;			// �ַ�����ɫ
	FILE_ENCODING_TYPE m_FileEncoding;		// Ĭ�ϱ����ļ�����
	SYNTAX_COLORING_TYPE m_SyntaxColoring;	// ��ǰ�﷨��ɫ����
	COLORREF m_crCodeColor;		// ��ͨ������ɫ
	int m_iCurrentLine;			// ��ǰ��
	int m_iCurrentChar;			// ��ǰ�ַ����
	int m_iTABs;				// Tab ����
	BOOL m_LineNumbers;			// �Ƿ���ʾ�к�
	BOOL m_bComment;			// �Ƿ�ע��
	BOOL m_bText;				// �Ƿ��ı�
	BOOL m_bSelecting;			// �Ƿ�����ѡ��,Shift
	BOOL m_bCopying;			// �Ƿ����ڸ���,Ctrl
	COLORREF m_crBgColor;		// ������ɫ
	BOOL m_changed;				// �ĵ��Ƿ��Ѿ�����
	COLORREF m_crLineNumBgColor;	// �кű�����ɫ
	COLORREF m_crLineNumFgColor;	// �к�ǰ����ɫ
	BOOL m_bCanUndo;				// �༭���Ƿ���Գ���
	CListBox m_wndListBox;			// �Զ�����б��
	LPLINEDRAWPROC m_lpfnLineDrawProc;		// ���Ի溯��
};
