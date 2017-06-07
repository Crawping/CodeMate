// CodeEdit.cpp :  ʵ���ļ�
// ���� CUltraEditWnd �Ľ�
// ��鿴 http://www.codeproject.com/KB/edit/CUltraPadWnd.aspx
// ��ԴЭ�� CPOL http://www.codeproject.com/info/cpol10.aspx
// 

#include "stdafx.h"
#include "CodeMate.h"
#include "CodeEdit.h"
#include "CodeMateDlg.h"

// CCodeEdit
IMPLEMENT_DYNAMIC(CCodeEdit, CWnd)

	CCodeEdit::CCodeEdit()
{
	// ��ʼ����Ա
	m_iSize = 2;
	m_lpszText = (LPTSTR)malloc(m_iSize*sizeof(_TCHAR));
	m_lpszText[0] = '\r';
	m_lpszText[1] = '\n';
	m_Font.CreateFont(-15, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_MODERN, _T("Courier New"));
	m_iLineStartSelection = m_iLineEndSelection = -1;
	m_iCharStartSelection = m_iCharEndSelection = -1;
	m_iNumberLines = 1;
	m_UndoInfo.lpszUndoData = (LPTSTR)malloc(sizeof(_TCHAR));
	m_UndoInfo.nLen = 0;
	m_UndoInfo.iChar = 0;
	m_UndoInfo.iLine = 0;
	m_UndoInfo.iEndChar = 0;
	m_UndoInfo.iEndLine = 0;
	m_UndoInfo.iLastLineLen = 0;
	m_lpLineInfo = (LPLINEINFO)malloc(m_iNumberLines*sizeof(LINEINFO));
	m_lpLineInfo[m_iNumberLines-1].dwOffset = 0;
	m_lpLineInfo[m_iNumberLines-1].nLen = 0;
	m_crKeywordColor = RGB(0,0,0);
	m_lpKeywordInfo = NULL;
	m_iNumberKeywords = 0;
	m_bKeywordsCaseSensitive = NULL;
	m_crConstantColor = RGB(0,0,0);
	m_lpConstantInfo = NULL;
	m_iNumberConstants = 0;
	m_bConstantsCaseSensitive = NULL;
	m_bUper = NULL;
	m_crCommentColor = RGB(0,0,0);
	m_crTextColor = RGB(0,0,0);
	m_crCodeColor = RGB(0,0,0);
	m_FileEncoding = FET_UNICODE;
	m_SyntaxColoring = SCT_NONE;
	m_iCurrentLine = 0;
	m_iCurrentChar = 0;
	m_iTABs = 4;
	m_bComment = FALSE;
	m_bText = FALSE;
	m_bSelecting = FALSE;
	m_bCopying = FALSE;
	m_crBgColor = RGB(255,255,255);
	m_crLineNumFgColor = RGB(255,255,255);
	m_crLineNumBgColor = RGB(128,128,128);
	m_lpfnLineDrawProc = NULL;
	m_changed = FALSE;
	m_bCanUndo = FALSE;
	m_LineNumbers = TRUE;
}

CCodeEdit::~CCodeEdit()
{
	// ���ٹ��
	DestroyCaret();

	// �������
	if (m_lpszText != NULL)
		free(m_lpszText);
	if (m_lpLineInfo != NULL)
	{
		free(m_lpLineInfo);
		m_lpLineInfo = NULL;
	}
	if (m_UndoInfo.lpszUndoData != NULL)
		free(m_UndoInfo.lpszUndoData);
	for (int i=0; i<m_iNumberKeywords; i++)
		delete m_lpKeywordInfo[i].lpszTagName;
	free(m_lpKeywordInfo);
	m_lpKeywordInfo = NULL;
	for (int i=0; i<m_iNumberConstants; i++)
		delete m_lpConstantInfo[i].lpszTagName;
	free(m_lpConstantInfo);
	m_lpConstantInfo = NULL;

	// �����Ա
	if (m_MemDC.m_hDC != NULL)
	{
		m_MemDC.SelectObject(m_pOldMemBitmap);
		m_MemDC.SelectObject(m_pOldFont);
		m_MemDC.DeleteDC();
		m_MemBitmap.DeleteObject();
		m_Font.DeleteObject();
	}
}


BEGIN_MESSAGE_MAP(CCodeEdit, CWnd)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_CHAR()
	ON_WM_SETCURSOR()
	ON_WM_MOUSEWHEEL()
	ON_LBN_DBLCLK(IDC_LIST_AUTOCOMPLETE, &CCodeEdit::OnLbnDblclkListAutoComplete)
END_MESSAGE_MAP()



// CCodeEdit ��Ϣ�������
BOOL CCodeEdit::Create(DWORD dwExStyle, DWORD dwStyle, RECT& rect, CWnd* pParentWnd, UINT nID)
{
	return CWnd::CreateEx(dwExStyle, NULL, NULL, dwStyle, rect, pParentWnd, nID, NULL);
}

void CCodeEdit::DrawContent()
{
	// �ĵ���Ϊ��
	if (m_lpszText != NULL)
	{
		// ��ÿͻ����Ĵ�С��λ��
		RECT rcClient;
		GetClientRect(&rcClient);

		// ��ù�������Ϣ
		int iVerticalOffset = GetScrollPos(SB_VERT);
		int iHorizontalOffset = GetScrollPos(SB_HORZ);

		// ��ÿɼ��еķ�Χ
		POINT ptTop = {0, rcClient.top+iVerticalOffset};
		POINT ptBottom = {0, (rcClient.bottom-rcClient.top)+iVerticalOffset};
		int iLineStart = LineFromPosition(ptTop);
		int iLineEnd = LineFromPosition(ptBottom);

		// �����ɼ�����
		m_bComment = FALSE;
		m_bText = FALSE;
		int iTmpOffset = 0;
		int iTmpOldOffset = iTmpOffset;
		int iTmpCurrentLine = 0;
		LPTSTR lpszTmpNextLine = NULL;
		while ((lpszTmpNextLine=GetLine(iTmpOffset)) != NULL)
		{
			// ���Բ��ɼ����н��д���
			if (iTmpCurrentLine < iLineStart)
			{
				// ������
				int iLen = iTmpOffset - iTmpOldOffset;
				ProcessLine(lpszTmpNextLine, m_lpLineInfo[iTmpCurrentLine].nLen);
			}
			iTmpOldOffset = iTmpOffset;
			iTmpCurrentLine++;

			// �˳�
			if (iTmpCurrentLine >= iLineStart)
				break;
		}

		// ������
		RECT rcLine = {-iHorizontalOffset, 0, rcClient.right, m_szTextSize.cy};
		int iOffset = m_lpLineInfo[iLineStart].dwOffset;
		int iOldOffset = iOffset;
		int iCurrentLine = iLineStart;
		LPTSTR lpszNextLine = NULL;
		while ((lpszNextLine=GetLine(iOffset)) != NULL)
		{
			// �����к�
			if (m_LineNumbers)
			{
				COLORREF cOldTextColor = m_MemDC.SetTextColor(m_crLineNumFgColor);
				COLORREF cOldBkColor = m_MemDC.SetBkColor(m_crLineNumBgColor);
				CString LineNumber;
				LineNumber.Format(_T("%4d "), iCurrentLine+1);
				m_MemDC.TextOut(rcLine.left, rcLine.top, LineNumber, 5);
				m_MemDC.SetBkColor(cOldBkColor);
				m_MemDC.SetTextColor(cOldTextColor);
			}

			// �����ƿɼ�����
			if ((iCurrentLine >= iLineStart) && (iCurrentLine <= iLineEnd))
			{
				// ��ÿɼ��ַ��ķ�Χ
				POINT ptLeft = {iHorizontalOffset, 0};
				POINT ptRight = {rcLine.right+iHorizontalOffset, 0};
				int iCharStart = CharFromPosition(iCurrentLine, ptLeft);
				int iCharEnd = CharFromPosition(iCurrentLine, ptRight);

				// �����ı�����
				int iLen = iOffset - iOldOffset;

				// �����Ч��ѡ��
				if ((m_iLineStartSelection != m_iLineEndSelection) || (m_iCharStartSelection != m_iCharEndSelection))
				{
					// ��ʽ��ѡ��
					int iLineStartSelection = min(m_iLineStartSelection, m_iLineEndSelection);
					int iLineEndSelection = max(m_iLineStartSelection, m_iLineEndSelection);
					int iCharStartSelection = m_iCharStartSelection;
					int iCharEndSelection = m_iCharEndSelection;

					// ���ѡ�е��ı�
					if ((iCurrentLine >= iLineStartSelection) && (iCurrentLine <= iLineEndSelection))
					{
						// ����ѡ�е��ַ�
						int iSelectionStart;
						if (iCurrentLine == iLineStartSelection)
						{
							if (m_iLineStartSelection < m_iLineEndSelection)
								iSelectionStart = max(iCharStartSelection, iCharStart);
							else if (m_iLineStartSelection > m_iLineEndSelection)
								iSelectionStart = max(iCharEndSelection, iCharStart);
							else
								iSelectionStart = max(min(m_iCharStartSelection, m_iCharEndSelection), iCharStart);
						}
						else if (iCurrentLine == iLineEndSelection)
						{
							if (m_iLineStartSelection < m_iLineEndSelection)
								iSelectionStart = iCharStart;
							else if (m_iLineStartSelection > m_iLineEndSelection)
								iSelectionStart = iCharStart;
							else
								iSelectionStart = max(min(m_iCharStartSelection, m_iCharEndSelection), iCharStart);
						}
						else
							iSelectionStart = iCharStart;
						int iSelectionEnd;
						if (iCurrentLine == iLineStartSelection)
						{
							if (m_iLineStartSelection < m_iLineEndSelection)
								iSelectionEnd = iCharEnd;
							else if (m_iLineStartSelection > m_iLineEndSelection)
								iSelectionEnd = iCharEnd;
							else
								iSelectionEnd = min(max(m_iCharStartSelection, m_iCharEndSelection), iCharEnd);
						}
						else if (iCurrentLine == iLineEndSelection)
						{
							if (m_iLineStartSelection < m_iLineEndSelection)
								iSelectionEnd = min(iCharEndSelection, iCharEnd);
							else if (m_iLineStartSelection > m_iLineEndSelection)
								iSelectionEnd = min(iCharStartSelection, iCharEnd);
							else
								iSelectionEnd = min(max(m_iCharStartSelection, m_iCharEndSelection), iCharEnd);
						}
						else
							iSelectionEnd = iCharEnd;
						int iSelected = iSelectionEnd - iSelectionStart;
						if (iSelected > 0)
						{
							// ����ѡ�е��ı�
							if (m_lpfnLineDrawProc == NULL)
								DrawLine(lpszNextLine, m_lpLineInfo[iCurrentLine].nLen, rcLine, iSelectionStart, iSelectionEnd);
							else
								m_lpfnLineDrawProc(m_MemDC.m_hDC, lpszNextLine, m_lpLineInfo[iCurrentLine].nLen, rcLine, iSelectionStart, iSelectionEnd);
						}
						else
						{
							// ������ͨ���ı�
							if (m_lpfnLineDrawProc == NULL)
								DrawLine(lpszNextLine, m_lpLineInfo[iCurrentLine].nLen, rcLine, -1, -1);
							else
								m_lpfnLineDrawProc(m_MemDC.m_hDC, lpszNextLine, m_lpLineInfo[iCurrentLine].nLen, rcLine, -1, -1);
						}
					}
					else
					{
						// ������ͨ���ı�
						if (m_lpfnLineDrawProc == NULL)
							DrawLine(lpszNextLine, m_lpLineInfo[iCurrentLine].nLen, rcLine, -1, -1);
						else
							m_lpfnLineDrawProc(m_MemDC.m_hDC, lpszNextLine, m_lpLineInfo[iCurrentLine].nLen, rcLine, -1, -1);
					}
				}
				else
				{
					// ������ͨ���ı�
					if (m_lpfnLineDrawProc == NULL)
						DrawLine(lpszNextLine, m_lpLineInfo[iCurrentLine].nLen, rcLine, -1, -1);
					else
						m_lpfnLineDrawProc(m_MemDC.m_hDC, lpszNextLine, m_lpLineInfo[iCurrentLine].nLen, rcLine, -1, -1);
				}

				// �����ı�����
				OffsetRect(&rcLine, 0, m_szTextSize.cy);
			}
			iOldOffset = iOffset;
			iCurrentLine++;

			// �˳�
			if (iCurrentLine > iLineEnd)
				break;
		}

		// ���ƿհ��к�����
		if (m_LineNumbers)
		{
			while (rcLine.top < rcClient.bottom)
			{
				COLORREF cOldTextColor = m_MemDC.SetTextColor(m_crLineNumFgColor);
				COLORREF cOldBkColor = m_MemDC.SetBkColor(m_crLineNumBgColor);
				m_MemDC.TextOut(rcLine.left, rcLine.top, _T("     "), 5);
				m_MemDC.SetBkColor(cOldBkColor);
				m_MemDC.SetTextColor(cOldTextColor);
				rcLine.top += m_szTextSize.cy;
			}
		}
	}
}

void CCodeEdit::DrawLine(LPTSTR lpszText, int iLen, RECT rect, int iSelectionStart, int iSelectionEnd)
{
	// ���Ƶ����ı�
	int iStartTAB=-1, iEndTAB, iTABs;
	int iStart=0, i=0;
	int iEnd=-1, iCount;
	POINT pt = {rect.left+LINE_NUMBER_TO_EDITOR, rect.top};
	if (m_LineNumbers) pt.x += 5*m_szTextSize.cx;
	BOOL bComment = m_bComment;
	BOOL bText = m_bText;
	while (i <= iLen)
	{
		// ��������ַ�
		if ((IsSpecial(lpszText[i])) || (i == iLen))
		{
			// ��õ���λ��
			iEnd = i - 1;
			iCount = iEnd - iStart + 1;

			// ���ע��
			BOOL bTurnCommentOff = FALSE;
			if ((!bComment) && (!m_bComment))
			{
				switch (m_SyntaxColoring)
				{
				case SCT_C_ANSI:
				case SCT_C_PLUS_PLUS:
				case SCT_C_SHARP:
				case SCT_JSCRIPT:
					{
						if (_tcsncmp(lpszText+iStart, _T("//"), 2) == 0)
							bComment = TRUE;
						else if (_tcsncmp(lpszText+iStart, _T("/*"), 2) == 0)
						{
							m_bComment = TRUE;
							bComment = TRUE;
						}
					}
					break;

				case SCT_VBSCRIPT:
					{
						if (_tcsncmp(lpszText+iStart, _T("'"), 1) == 0)
							bComment = TRUE;
						LPTSTR lpszKeyword = (LPTSTR)malloc(5*sizeof(_TCHAR));;
						_tcsncpy_s(lpszKeyword, 5, lpszText+iStart, 4);
						lpszKeyword[4] = '\0';
						_tcsupr_s(lpszKeyword, 5);
						if (_tcsncmp(lpszKeyword, _T("REM "), 4) == 0)
							bComment = TRUE;
						free(lpszKeyword);
					}
					break;

				case SCT_HTML:
					{
						if (_tcsncmp(lpszText+iStart, _T("<!--"), 4) == 0)
						{
							m_bComment = TRUE;
							bComment = TRUE;
						}
					}
					break;

				case SCT_SQL:
					{
						if (_tcsncmp(lpszText+iStart, _T("--"), 2) == 0)
							bComment = TRUE;
						else if (_tcsncmp(lpszText+iStart, _T("/*"), 2) == 0)
						{
							m_bComment = TRUE;
							bComment = TRUE;
						}
					}
					break;
				}
			}
			else if (m_bComment)
			{
				switch (m_SyntaxColoring)
				{
				case SCT_C_ANSI:
				case SCT_C_PLUS_PLUS:
				case SCT_C_SHARP:
				case SCT_JSCRIPT:
				case SCT_SQL:
					{
						if (_tcsncmp(lpszText+iStart, _T("*/"), 2) == 0)
						{
							m_bComment = FALSE;
							bTurnCommentOff = TRUE;
						}
					}
					break;

				case SCT_HTML:
					{
						if (_tcsncmp(lpszText+iStart, _T("-->"), 3) == 0)
						{
							m_bComment = FALSE;
							bTurnCommentOff = TRUE;
						}
					}
					break;
				}
			}

			// ���ע��
			if (!bComment)
			{
				// ������ͨ����
				m_MemDC.SetTextColor(m_crCodeColor);
				m_MemDC.TextOut(pt.x, pt.y, lpszText+iStart, iCount);

				// ���ؼ���
				int iKeywordIndex = -1;
				int iKeywordOffset = -1;
				if (IsKeyword(lpszText+iStart, iCount, iKeywordIndex, iKeywordOffset))
				{
					// ���ƹؼ���
					COLORREF cOldTextColor = m_MemDC.SetTextColor(m_crKeywordColor);
					m_MemDC.TextOut(pt.x+iKeywordOffset*m_szTextSize.cx, pt.y, lpszText+iStart+iKeywordOffset, m_lpKeywordInfo[iKeywordIndex].nLen-1);
					m_MemDC.SetTextColor(cOldTextColor);
				}

				// ��鳣��
				int iConstantIndex = -1;
				int iConstantOffset = -1;
				if (IsConstant(lpszText+iStart, iCount, iConstantIndex, iConstantOffset))
				{
					// ���Ƴ���
					COLORREF cOldTextColor = m_MemDC.SetTextColor(m_crConstantColor);
					m_MemDC.TextOut(pt.x+iConstantOffset*m_szTextSize.cx, pt.y, m_lpConstantInfo[iConstantIndex].lpszTagName, m_lpConstantInfo[iConstantIndex].nLen-1);
					m_MemDC.SetTextColor(cOldTextColor);
				}

				// ����ַ���
				int iTextStart = -1;
				int iTextEnd = -1;
				if (IsText(lpszText+iStart, iCount, iTextStart, iTextEnd) || (bText))
				{
					// �����ı�ƫ����
					if (!bText)
					{
						if (iTextEnd == -1)
						{
							iTextEnd = iCount - 1;
							bText = TRUE;
						}
					}
					else
					{
						if (iTextEnd == -1)
						{
							iTextEnd = iTextStart;
							if (iTextEnd == -1)
								iTextEnd = iCount - 1;
							else
								bText = FALSE;
							iTextStart = 0;
						}
					}

					// �����ַ���
					COLORREF cOldTextColor = m_MemDC.SetTextColor(m_crTextColor);
					m_MemDC.TextOut(pt.x+iTextStart*m_szTextSize.cx, pt.y, lpszText+iStart+iTextStart, iTextEnd-iTextStart+1);
					m_MemDC.SetTextColor(cOldTextColor);
					m_bText = bText;
				}
			}
			else
			{
				// ����ע��
				COLORREF cOldTextColor = m_MemDC.SetTextColor(m_crCommentColor);
				m_MemDC.TextOut(pt.x, pt.y, lpszText+iStart, iCount);
				m_MemDC.SetTextColor(cOldTextColor);
				if (bTurnCommentOff == TRUE)
					bComment = FALSE;
			}

			// �����Чѡ��
			int iWordStartSelection = max(iStart, iSelectionStart);
			int iWordEndSelection = min(iEnd+1, iSelectionEnd);
			int iNumberSelected = iWordEndSelection - iWordStartSelection;

			if (iNumberSelected > 0)
			{
				// ����ѡ����ʼ����λ��
				int iSelectionOffset = 0;
				for (int ch = iStart; ch < iWordStartSelection; ch++)
				{
					if (IsCJKChar(lpszText[ch]))
						iSelectionOffset += m_szCJKTextSize.cx;
					else
						iSelectionOffset += m_szTextSize.cx;
				}

				// ����ѡ��
				COLORREF cOldTextColor = m_MemDC.SetTextColor(RGB(255,255,255));
				COLORREF cOldBkColor = m_MemDC.SetBkColor(RGB(51,153,255));
				m_MemDC.TextOut(pt.x+iSelectionOffset, pt.y, lpszText+iWordStartSelection, iNumberSelected);
				m_MemDC.SetBkColor(cOldBkColor);
				m_MemDC.SetTextColor(cOldTextColor);
			}

			// ����ƫ����
			int iSpaceOffset = 0;
			for (int ch = iStart; ch < iEnd + 1; ch++)
			{
				if (IsCJKChar(lpszText[ch]))
					iSpaceOffset += m_szCJKTextSize.cx;
				else
					iSpaceOffset += m_szTextSize.cx;
			}
			pt.x += iSpaceOffset;

			// ���ո��ַ�
			if (lpszText[i] == ' ')
			{
				// �����Чѡ��
				if ((i >= iSelectionStart) && (i < iSelectionEnd))
				{
					// ����ѡ��
					COLORREF cOldTextColor = m_MemDC.SetTextColor(RGB(255,255,255));
					COLORREF cOldBkColor = m_MemDC.SetBkColor(RGB(51,153,255));
					m_MemDC.TextOut(pt.x, pt.y, _T(" "), 1);
					m_MemDC.SetBkColor(cOldBkColor);
					m_MemDC.SetTextColor(cOldTextColor);
				}

				// ����ƫ����
				pt.x += m_szTextSize.cx;
				iStart = i + 1;
			}
			// ��� TAB �ַ�
			else if (lpszText[i] == '\t')
			{
				// ���� TAB �ַ�
				if (iStartTAB == -1)
				{
					iTABs = m_iTABs;
					iStartTAB = i;
				}
				else
				{
					iEndTAB = i;
					iTABs = m_iTABs - ((iEndTAB - iStartTAB) % m_iTABs);
					if (((iEndTAB - iStartTAB) % m_iTABs) != 0)
						iTABs++;
					if ((iEndTAB - iStartTAB) == 1)
						iTABs = m_iTABs;
					iStartTAB = iEndTAB;
				}

				// �����Чѡ��
				if ((i >= iSelectionStart) && (i < iSelectionEnd))
				{

					// ����ѡ��
					COLORREF cOldTextColor = m_MemDC.SetTextColor(RGB(255,255,255));
					COLORREF cOldBkColor = m_MemDC.SetBkColor(RGB(51,153,255));
					m_MemDC.TextOut(pt.x, pt.y, _T("        "), iTABs);
					m_MemDC.SetBkColor(cOldBkColor);
					m_MemDC.SetTextColor(cOldTextColor);
				}

				// ����ƫ����
				pt.x += (iTABs * m_szTextSize.cx);
				iStart = i + 1;
			}
			else
				break;
		}
		i++;
	}
}

void CCodeEdit::ProcessLine(LPTSTR lpszText, int iLen)
{
	// �����ı����
	int iStartTAB=-1, iEndTAB, iTABs;
	int iStart=0, i=0;
	int iEnd=-1, iCount;
	BOOL bComment = m_bComment;
	BOOL bText = m_bText;
	while (i <= iLen)
	{
		// ��������ַ�
		if ((IsSpecial(lpszText[i])) || (i == iLen))
		{
			// ��õ��ʵĽ�βλ��
			iEnd = i - 1;
			iCount = iEnd - iStart + 1;

			// ����Ƿ�Ϊע��
			BOOL bTurnCommentOff = FALSE;
			if ((!bComment) && (!m_bComment))
			{
				switch (m_SyntaxColoring)
				{
				case SCT_C_ANSI:
				case SCT_C_PLUS_PLUS:
				case SCT_C_SHARP:
				case SCT_JSCRIPT:
					{
						if (_tcsncmp(lpszText+iStart, _T("//"), 2) == 0)
							bComment = TRUE;
						else if (_tcsncmp(lpszText+iStart, _T("/*"), 2) == 0)
						{
							m_bComment = TRUE;
							bComment = TRUE;
						}
					}
					break;

				case SCT_VBSCRIPT:
					{
						if (_tcsncmp(lpszText+iStart, _T("'"), 1) == 0)
							bComment = TRUE;
						LPTSTR lpszKeyword = (LPTSTR)malloc(5*sizeof(_TCHAR));;
						_tcsncpy_s(lpszKeyword, 5, lpszText+iStart, 4);
						lpszKeyword[4] = '\0';
						_tcsupr_s(lpszKeyword, 5);
						if (_tcsncmp(lpszKeyword, _T("REM "), 4) == 0)
							bComment = TRUE;
						free(lpszKeyword);
					}
					break;

				case SCT_HTML:
					{
						if (_tcsncmp(lpszText+iStart, _T("<!--"), 4) == 0)
						{
							m_bComment = TRUE;
							bComment = TRUE;
						}
					}
					break;

				case SCT_SQL:
					{
						if (_tcsncmp(lpszText+iStart, _T("--"), 2) == 0)
							bComment = TRUE;
						else if (_tcsncmp(lpszText+iStart, _T("/*"), 2) == 0)
						{
							m_bComment = TRUE;
							bComment = TRUE;
						}
					}
					break;
				}
			}
			else if (m_bComment)
			{
				switch (m_SyntaxColoring)
				{
				case SCT_C_ANSI:
				case SCT_C_PLUS_PLUS:
				case SCT_C_SHARP:
				case SCT_JSCRIPT:
				case SCT_SQL:
					{
						if (_tcsncmp(lpszText+iStart, _T("*/"), 2) == 0)
						{
							m_bComment = FALSE;
							bTurnCommentOff = TRUE;
						}
					}
					break;

				case SCT_HTML:
					{
						if (_tcsncmp(lpszText+iStart, _T("-->"), 3) == 0)
						{
							m_bComment = FALSE;
							bTurnCommentOff = TRUE;
						}
					}
					break;
				}
			}

			// �������ע��
			if (!bComment)
			{
				// ����Ƿ�����ͨ�ı�
				int iTextStart = -1;
				int iTextEnd = -1;
				if (IsText(lpszText+iStart, iCount, iTextStart, iTextEnd) || (bText))
				{
					// �����ı�ƫ����
					if (!bText)
					{
						if (iTextEnd == -1)
						{
							iTextEnd = iCount - 1;
							bText = TRUE;
						}
					}
					else
					{
						if (iTextEnd == -1)
						{
							iTextEnd = iTextStart;
							if (iTextEnd == -1)
								iTextEnd = iCount - 1;
							else
								bText = FALSE;
							iTextStart = 0;
						}
					}
					m_bText = bText;
				}
			}
			else
			{
				// �ر�ע��
				if (bTurnCommentOff == TRUE)
					bComment = FALSE;
			}

			// ���ո��ַ�
			if (lpszText[i] == ' ')
			{
				// ����ƫ����
				iStart = i + 1;
			}
			// ��� TAB �ַ�
			else if (lpszText[i] == '\t')
			{
				// ���� TAB �ַ�
				if (iStartTAB == -1)
				{
					iTABs = m_iTABs;
					iStartTAB = i;
				}
				else
				{
					iEndTAB = i;
					iTABs = m_iTABs - ((iEndTAB - iStartTAB) % m_iTABs);
					if (((iEndTAB - iStartTAB) % m_iTABs) != 0)
						iTABs++;
					if ((iEndTAB - iStartTAB) == 1)
						iTABs = m_iTABs;
					iStartTAB = iEndTAB;
				}

				// ����ƫ����
				iStart = i + 1;
			}
			else
				break;
		}
		i++;
	}
}

void CCodeEdit::UpdateControl()
{
	// ����ػ�ı�־
	SetRedraw(FALSE);

	// �������Ϣ
	if (m_lpLineInfo != NULL)
	{
		free(m_lpLineInfo);
		m_lpLineInfo = NULL;
	}

	// ��ÿͻ�����С��λ��
	RECT rcClient;
	GetClientRect(&rcClient);

	// ����ַ��ĳߴ�
	m_szTextSize = m_MemDC.GetTextExtent(_T("A"), 1);
	m_szCJKTextSize = m_MemDC.GetTextExtent(_T("��"), 1);

	// �������ݳߴ�
	int iWidth = 0;
	int iHeight = 0;
	int iOffset = 0;
	int iOldOffset = iOffset;
	m_iNumberLines = 0;
	LPTSTR lpszNextLine = NULL;
	while ((lpszNextLine=GetLine(iOffset)) != NULL)
	{
		// ��������Ϣ
		m_iNumberLines++;
		if (m_lpLineInfo == NULL)
			m_lpLineInfo = (LPLINEINFO)malloc(m_iNumberLines*sizeof(LINEINFO));
		else
			m_lpLineInfo = (LPLINEINFO)realloc(m_lpLineInfo, m_iNumberLines*sizeof(LINEINFO));
		m_lpLineInfo[m_iNumberLines-1].dwOffset = iOldOffset;

		// �����г���
		int iLen = iOffset - iOldOffset;
		iOldOffset = iOffset;
		if ((lpszNextLine[iLen-2] == '\r') && (lpszNextLine[iLen-1] == '\n'))
			m_lpLineInfo[m_iNumberLines-1].nLen = iLen - 2;
		else if (lpszNextLine[iLen-1] == '\n')
			m_lpLineInfo[m_iNumberLines-1].nLen = iLen - 1;
		else
			m_lpLineInfo[m_iNumberLines-1].nLen = iLen;

		// ���㵥�гߴ�
		SIZE szLine = GetLineSize(lpszNextLine, m_lpLineInfo[m_iNumberLines-1].nLen);
		if (iWidth < szLine.cx)
			iWidth = szLine.cx;
		iHeight += szLine.cy;
	}

	// ��ù������ߴ�
	int iHorizontalOffset = GetSystemMetrics(SM_CYHSCROLL);
	if (iHeight <= (rcClient.bottom - rcClient.top))
		iHorizontalOffset = 0;
	int iVerticalOffset = GetSystemMetrics(SM_CXVSCROLL);
	if (iWidth <= (rcClient.right - rcClient.left))
		iVerticalOffset = 0;

	// ������ݿ��
	if (iWidth > (rcClient.right - rcClient.left))
	{
		// ��ʾˮƽ������
		SCROLLINFO si;
		memset(&si, 0, sizeof(SCROLLINFO));
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_ALL;
		si.nMax = iWidth;
		si.nPage = ((rcClient.right - rcClient.left - iVerticalOffset) / m_szTextSize.cx + 1) * m_szTextSize.cx;
		si.nPos = GetScrollPos(SB_HORZ);
		SetScrollInfo(SB_HORZ, &si, TRUE);
		ShowScrollBar(SB_HORZ, TRUE);
	}
	else
	{
		// ����ˮƽ������
		SCROLLINFO si;
		memset(&si, 0, sizeof(SCROLLINFO));
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_ALL;
		si.nMax = iWidth;
		si.nPage = ((rcClient.right - rcClient.left - iVerticalOffset) / m_szTextSize.cx + 1) * m_szTextSize.cx;
		SetScrollInfo(SB_HORZ, &si, TRUE);
		SetScrollPos(SB_HORZ, 0, TRUE);
		ShowScrollBar(SB_HORZ, FALSE);
	}

	// ������ݸ߶�
	if (iHeight > (rcClient.bottom - rcClient.top))
	{
		// ��ʾ��ֱ������
		SCROLLINFO si;
		memset(&si, 0, sizeof(SCROLLINFO));
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_ALL;
		si.nMax = iHeight;
		si.nPage = ((rcClient.bottom - rcClient.top - iHorizontalOffset) / m_szTextSize.cy) * m_szTextSize.cy;
		si.nPos = GetScrollPos(SB_VERT);
		SetScrollInfo(SB_VERT, &si, TRUE);
		ShowScrollBar(SB_VERT, TRUE);
	}
	else
	{
		// ���´�ֱ������
		SCROLLINFO si;
		memset(&si, 0, sizeof(SCROLLINFO));
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_ALL;
		si.nMax = iHeight;
		si.nPage = ((rcClient.bottom - rcClient.top - iHorizontalOffset) / m_szTextSize.cy) * m_szTextSize.cy;
		SetScrollInfo(SB_VERT, &si, TRUE);
		SetScrollPos(SB_VERT, 0, TRUE);
		ShowScrollBar(SB_VERT, FALSE);
	}

	// �����ػ�ı�־
	SetRedraw(TRUE);
}

void CCodeEdit::DestroyList()
{
	if (m_wndListBox.m_hWnd != NULL)
	{
		// �����б��
		m_wndListBox.DestroyWindow();
	}
}

void CCodeEdit::DeleteText()
{
	// �ĵ���Ϊ��
	if (m_lpszText != NULL)
	{
		// ѡ����Ч
		if ((m_iLineStartSelection != m_iLineEndSelection) || (m_iCharStartSelection != m_iCharEndSelection))
		{
			// ���ѡ��Χ
			int iStartChar, iEndChar;
			int iStartLine, iEndLine;
			if (m_iLineStartSelection < m_iLineEndSelection)
			{
				iStartChar = m_iCharStartSelection;
				iEndChar = m_iCharEndSelection;
				iStartLine = m_iLineStartSelection;
				iEndLine = m_iLineEndSelection;
			}
			else if (m_iLineStartSelection > m_iLineEndSelection)
			{
				iStartChar = m_iCharEndSelection;
				iEndChar = m_iCharStartSelection;
				iStartLine = m_iLineEndSelection;
				iEndLine = m_iLineStartSelection;
			}
			else
			{
				iStartChar = min(m_iCharStartSelection, m_iCharEndSelection);
				iEndChar = max(m_iCharStartSelection, m_iCharEndSelection);
				iStartLine = m_iLineStartSelection;
				iEndLine = m_iLineEndSelection;
			}
			DelText(iStartLine, iStartChar, iEndLine, iEndChar);

			m_iCurrentChar = iStartChar;
			m_iCurrentLine = iStartLine;
		}
	}
}

void CCodeEdit::DelText(int iLine, int iChar, int iEndLine, int iEndChar)
{
	int iStartOffset = GetCharOffset(iLine, iChar);
	if (m_lpszText[iStartOffset] == '\r')
		iStartOffset += 2;
	int iEndOffset = GetCharOffset(iEndLine, iEndChar);
	int iLen = iEndOffset - iStartOffset;

	if (m_lpszText != NULL)
	{
		// ���泷����Ϣ
		if (m_UndoInfo.lpszUndoData == NULL)
			m_UndoInfo.lpszUndoData = (LPTSTR)malloc(iLen*sizeof(_TCHAR));
		else
			m_UndoInfo.lpszUndoData = (LPTSTR)realloc(m_UndoInfo.lpszUndoData, iLen*sizeof(_TCHAR));
		m_UndoInfo.iLastLineLen = m_lpLineInfo[max(m_UndoInfo.iLine - 1, 0)].nLen;
		m_UndoInfo.nLen = iLen;
		m_UndoInfo.iLine = iLine;
		m_UndoInfo.iChar = iChar;
		m_UndoInfo.iEndLine = iEndLine;
		m_UndoInfo.iEndChar = iEndChar;
		memcpy(m_UndoInfo.lpszUndoData, m_lpszText+iStartOffset, iLen*sizeof(_TCHAR));
		m_UndoInfo.nOperation = UOT_DELTEXT;
		m_bCanUndo = TRUE;

		// ɾ���ı�
		int iOldSize = m_iSize;
		LPTSTR lpszTemp = new _TCHAR[iOldSize];
		memcpy(lpszTemp, m_lpszText, iOldSize*sizeof(_TCHAR));
		m_iSize -= iLen;
		free(m_lpszText);
		m_lpszText = (LPTSTR)malloc(m_iSize*sizeof(_TCHAR));
		if (m_lpszText != NULL)
		{
			memcpy(m_lpszText, lpszTemp, iStartOffset*sizeof(_TCHAR));
			memcpy(m_lpszText+iStartOffset, lpszTemp+iStartOffset+iLen, (iOldSize-iStartOffset-iLen)*sizeof(_TCHAR));
		}
		delete lpszTemp;

		// �ĵ��Ѹı�
		m_changed = TRUE;

		// ���ѡ��
		m_iLineStartSelection = -1;
		m_iLineEndSelection = -1;
		m_iCharStartSelection = -1;
		m_iCharEndSelection = -1;
	}
}

void CCodeEdit::AddText(LPTSTR lpszText, int iLine, int iChar, int iLen)
{
	// �ı���Ϊ��
	if ((m_lpszText != NULL) && (lpszText != NULL))
	{
		int iStartOffset = GetCharOffset(iLine, iChar);

		// ���泷����Ϣ
		if (m_UndoInfo.lpszUndoData == NULL)
			m_UndoInfo.lpszUndoData = (LPTSTR)malloc(iLen*sizeof(_TCHAR));
		else
			m_UndoInfo.lpszUndoData = (LPTSTR)realloc(m_UndoInfo.lpszUndoData, iLen*sizeof(_TCHAR));
		m_UndoInfo.nLen = iLen;
		m_UndoInfo.iLine = iLine;
		m_UndoInfo.iChar = iChar;
		memcpy(m_UndoInfo.lpszUndoData, lpszText, iLen*sizeof(_TCHAR));
		m_UndoInfo.nOperation = UOT_ADDTEXT;
		m_bCanUndo = TRUE;

		int iOldSize = m_iSize;
		LPTSTR lpszTemp = new _TCHAR[iOldSize];
		memcpy(lpszTemp, m_lpszText, iOldSize*sizeof(_TCHAR));
		m_iSize += iLen;
		free(m_lpszText);
		m_lpszText = (LPTSTR)malloc(m_iSize*sizeof(_TCHAR));
		memcpy(m_lpszText, lpszTemp, iStartOffset*sizeof(_TCHAR));
		memcpy(m_lpszText+iStartOffset, lpszText, iLen*sizeof(_TCHAR));
		memcpy(m_lpszText+iStartOffset+iLen, lpszTemp+iStartOffset, (iOldSize-iStartOffset)*sizeof(_TCHAR));
		delete lpszTemp;

		m_changed = TRUE;
	}
}

void CCodeEdit::DelChar(int iLine, int iChar)
{
	int nCharOffset = GetCharOffset(iLine, iChar);
	if ((m_lpszText != NULL) && (nCharOffset < m_iSize-2))
	{
		// ���泷����Ϣ
		if (m_UndoInfo.lpszUndoData == NULL)
			m_UndoInfo.lpszUndoData = (LPTSTR)malloc(sizeof(_TCHAR));
		else
			m_UndoInfo.lpszUndoData = (LPTSTR)realloc(m_UndoInfo.lpszUndoData, sizeof(_TCHAR));
		m_UndoInfo.iLastLineLen = m_lpLineInfo[iLine].nLen;
		m_UndoInfo.nLen = 1;
		m_UndoInfo.iLine = iLine;
		m_UndoInfo.iChar = iChar;
		m_UndoInfo.lpszUndoData[0] = m_lpszText[nCharOffset];
		m_UndoInfo.nOperation = UOT_DELETE;
		m_bCanUndo = TRUE;

		// ɾ�������ַ�
		int iOldSize = m_iSize;
		LPTSTR lpszTemp = new _TCHAR[iOldSize];
		memcpy(lpszTemp, m_lpszText, iOldSize*sizeof(_TCHAR));
		if (m_lpszText[nCharOffset+1] == '\n')
		{
			m_iSize -= 2;
			free(m_lpszText);
			m_lpszText = (LPTSTR)malloc(m_iSize*sizeof(_TCHAR));
			if (m_lpszText != NULL)
			{
				memcpy(m_lpszText, lpszTemp, nCharOffset*sizeof(_TCHAR));
				if (nCharOffset <= m_iSize)
					memcpy(m_lpszText+nCharOffset, lpszTemp+nCharOffset+2, (iOldSize-nCharOffset-2)*sizeof(_TCHAR));
			}
			m_iCurrentChar = m_lpLineInfo[m_iCurrentLine].nLen;
		}
		else
		{
			m_iSize--;
			free(m_lpszText);
			m_lpszText = (LPTSTR)malloc(m_iSize*sizeof(_TCHAR));
			if (m_lpszText != NULL)
			{
				memcpy(m_lpszText, lpszTemp, nCharOffset*sizeof(_TCHAR));
				if (nCharOffset <= m_iSize-1)
					memcpy(m_lpszText+nCharOffset, lpszTemp+nCharOffset+1, (iOldSize-nCharOffset-1)*sizeof(_TCHAR));
			}
		}
		delete lpszTemp;

		m_changed = TRUE;
	}
}

int CCodeEdit::BackChar(int iLine, int iChar)
{
	int Myresult = 0;

	// ���ƫ����
	int iCurrentOffset = GetCharOffset(iLine, iChar);

	if ((m_lpszText != NULL) && (iCurrentOffset > 0))
	{
		// ���泷����Ϣ
		if (m_UndoInfo.lpszUndoData == NULL)
			m_UndoInfo.lpszUndoData = (LPTSTR)malloc(sizeof(_TCHAR));
		else
			m_UndoInfo.lpszUndoData = (LPTSTR)realloc(m_UndoInfo.lpszUndoData, sizeof(_TCHAR));
		m_UndoInfo.iLastLineLen = m_lpLineInfo[iLine-1].nLen;
		m_UndoInfo.nLen = 1;
		m_UndoInfo.iLine = iLine;
		m_UndoInfo.iChar = iChar;
		m_UndoInfo.lpszUndoData[0] = m_lpszText[iCurrentOffset-1];
		m_UndoInfo.nOperation = UOT_BACK;
		m_bCanUndo = TRUE;

		// ɾ�����ǰ�ַ�
		int iOldSize = m_iSize;
		LPTSTR lpszTemp = new _TCHAR[iOldSize];
		memcpy(lpszTemp, m_lpszText, iOldSize*sizeof(_TCHAR));
		if (m_lpszText[iCurrentOffset-1] == '\n')
		{
			m_iSize -= 2;
			free(m_lpszText);
			m_lpszText = (LPTSTR)malloc(m_iSize*sizeof(_TCHAR));
			if (m_lpszText != NULL)
			{
				memcpy(m_lpszText, lpszTemp, (iCurrentOffset-2)*sizeof(_TCHAR));
				if (iCurrentOffset <= m_iSize)
					memcpy(m_lpszText+iCurrentOffset-2, lpszTemp+iCurrentOffset, (iOldSize-iCurrentOffset)*sizeof(_TCHAR));
			}
			Myresult = 1;
		}
		else
		{
			m_iSize--;
			free(m_lpszText);
			m_lpszText = (LPTSTR)malloc(m_iSize*sizeof(_TCHAR));
			if (m_lpszText != NULL)
			{
				memcpy(m_lpszText, lpszTemp, (iCurrentOffset-1)*sizeof(_TCHAR));
				if (iCurrentOffset <= m_iSize)
					memcpy(m_lpszText+iCurrentOffset-1, lpszTemp+iCurrentOffset, (iOldSize-iCurrentOffset)*sizeof(_TCHAR));
			}
			Myresult = 2;
		}
		delete lpszTemp;
		m_changed = TRUE;
	}
	return Myresult;
}

void CCodeEdit::AddChar(int iLine, int iChar, UINT nChar)
{
	int iCurrentOffset = GetCharOffset(iLine, iChar);

	// ���һ���ַ�
	int iOldSize = m_iSize;
	LPTSTR lpszTemp = new _TCHAR[iOldSize];
	memcpy(lpszTemp, m_lpszText, iOldSize*sizeof(_TCHAR));
	m_iSize++;
	free(m_lpszText);
	m_lpszText = (LPTSTR)malloc(m_iSize*sizeof(_TCHAR));
	memcpy(m_lpszText, lpszTemp, iCurrentOffset*sizeof(_TCHAR));
	m_lpszText[iCurrentOffset] = nChar;
	memcpy(m_lpszText+iCurrentOffset+1, lpszTemp+iCurrentOffset, (iOldSize-iCurrentOffset)*sizeof(_TCHAR));
	delete lpszTemp;
	m_changed = TRUE;

	// ���泷����Ϣ
	m_UndoInfo.nOperation = UOT_CHAR;
	if (m_UndoInfo.lpszUndoData == NULL)
		m_UndoInfo.lpszUndoData = (LPTSTR)malloc(sizeof(_TCHAR));
	else
		m_UndoInfo.lpszUndoData = (LPTSTR)realloc(m_UndoInfo.lpszUndoData, sizeof(_TCHAR));
	m_UndoInfo.iLastLineLen = m_lpLineInfo[m_UndoInfo.iLine - 1].nLen;
	m_UndoInfo.nLen = 1;
	m_UndoInfo.iLine = iLine;
	m_UndoInfo.iChar = iChar;
	m_UndoInfo.lpszUndoData[0] = nChar;
	m_bCanUndo = TRUE;
}

void CCodeEdit::AddLine(int iLine, int iChar)
{
	// ƫ����
	int iCurrentOffset = GetCharOffset(iLine, iChar);

	int iOldSize = m_iSize;
	LPTSTR lpszTemp = new _TCHAR[iOldSize];
	memcpy(lpszTemp, m_lpszText, iOldSize*sizeof(_TCHAR));
	m_iSize += 2;
	free(m_lpszText);
	m_lpszText = (LPTSTR)malloc(m_iSize*sizeof(_TCHAR));
	memcpy(m_lpszText, lpszTemp, iCurrentOffset*sizeof(_TCHAR));
	m_lpszText[iCurrentOffset] = '\r';
	m_lpszText[iCurrentOffset+1] = '\n';
	memcpy(m_lpszText+iCurrentOffset+2, lpszTemp+iCurrentOffset, (iOldSize-iCurrentOffset)*sizeof(_TCHAR));
	delete lpszTemp;
	m_changed = TRUE;

	// ���泷����Ϣ
	m_UndoInfo.nOperation = UOT_ADDLINE;
	if (m_UndoInfo.lpszUndoData == NULL)
		m_UndoInfo.lpszUndoData = (LPTSTR)malloc(sizeof(_TCHAR));
	else
		m_UndoInfo.lpszUndoData = (LPTSTR)realloc(m_UndoInfo.lpszUndoData, sizeof(_TCHAR));
	m_UndoInfo.iLastLineLen = m_lpLineInfo[iLine].nLen;
	m_UndoInfo.nLen = 1;
	m_UndoInfo.iLine = iLine;
	m_UndoInfo.iChar = iChar;
	m_UndoInfo.lpszUndoData[0] = '\n';
	m_bCanUndo = TRUE;
}

SIZE CCodeEdit::GetLineSize(LPTSTR lpszText, int iLen)
{
	SIZE szResult = {0, m_szTextSize.cy};

	// ����п�
	BOOL bDone = FALSE;
	int iStartTAB=-1;
	int iEndTAB, iTABs;
	for (int i=0; i<iLen; i++)
	{
		// ��� TAB �ַ� 
		if (lpszText[i] == '\t')
		{
			if (iStartTAB == -1)
			{
				iTABs = m_iTABs;
				iStartTAB = i;
			}
			else
			{
				iEndTAB = i;
				iTABs = m_iTABs - ((iEndTAB - iStartTAB) % m_iTABs);
				if (((iEndTAB - iStartTAB) % m_iTABs) != 0)
					iTABs++;
				if ((iEndTAB - iStartTAB) == 1)
					iTABs = m_iTABs;
				iStartTAB = iEndTAB;
			}
			szResult.cx += (iTABs * m_szTextSize.cx);
		}
		else
		{
			if (IsCJKChar(lpszText[i]))
				szResult.cx += m_szCJKTextSize.cx;
			else
				szResult.cx += m_szTextSize.cx;
		}
	}

	szResult.cx += 3;
	if (m_LineNumbers) szResult.cx += m_szTextSize.cx * 5;

	return szResult;
}

void CCodeEdit::ClearText()
{
	// ɾ�������ı�
	if (m_iSize != 2)
		DelText(0, 0, m_iNumberLines - 1, m_lpLineInfo[m_iNumberLines - 1].nLen);
	m_iCurrentChar = 0;
	m_iCurrentLine = 0;

	// ���¿ؼ�
	UpdateControl();

	// ���¹��
	UpdateCaret();
}

void CCodeEdit::Undo()
{
	// �жϲ�������
	switch(m_UndoInfo.nOperation)
	{
	case UOT_CHAR:	// ����һ���ַ�
		{
			m_iCurrentLine = m_UndoInfo.iLine;
			m_iCurrentChar = m_UndoInfo.iChar;
			BackChar(m_UndoInfo.iLine, m_UndoInfo.iChar + 1);
		}
		break;
	case UOT_BACK:	// ɾ��ǰ��һ���ַ�
		{
			if (m_UndoInfo.lpszUndoData[0] == '\n')
			{
				AddLine(m_UndoInfo.iLine - 1, m_UndoInfo.iLastLineLen);
				m_iCurrentLine = m_UndoInfo.iLine + 1;
				m_iCurrentChar = 0;
			}
			else
			{
				AddChar(m_UndoInfo.iLine, m_UndoInfo.iChar - 1, m_UndoInfo.lpszUndoData[0]);
				m_iCurrentLine = m_UndoInfo.iLine;
				m_iCurrentChar = m_UndoInfo.iChar + 1;
			}
		}
		break;
	case UOT_DELETE:	// ɾ������һ���ַ�
		{
			if (m_UndoInfo.lpszUndoData[0] == '\r')
			{
				m_iCurrentLine = m_UndoInfo.iLine;
				m_iCurrentChar = m_UndoInfo.iChar;
				AddLine(m_UndoInfo.iLine, m_UndoInfo.iChar);
			}
			else
			{
				m_iCurrentLine = m_UndoInfo.iLine;
				m_iCurrentChar = m_UndoInfo.iChar;
				AddChar(m_UndoInfo.iLine, m_UndoInfo.iChar, m_UndoInfo.lpszUndoData[0]);
			}
		}
		break;
	case UOT_ADDLINE:	// ���һ��
		{
			m_iCurrentLine = m_UndoInfo.iLine;
			m_iCurrentChar = m_UndoInfo.iChar;
			BackChar(m_UndoInfo.iLine + 1, 0);
		}
		break;
	case UOT_DELTEXT:	// ɾ���ı�
		{
			// �ָ�ѡ��
			m_iLineStartSelection = m_UndoInfo.iLine;
			m_iLineEndSelection = m_UndoInfo.iEndLine;
			m_iCharStartSelection = m_UndoInfo.iChar;
			m_iCharEndSelection = m_UndoInfo.iEndChar;

			// ����ı�
			AddText(m_UndoInfo.lpszUndoData, m_UndoInfo.iLine, m_UndoInfo.iChar, m_UndoInfo.nLen);

			// ���ù��λ��
			m_iCurrentLine = m_UndoInfo.iEndLine;
			m_iCurrentChar = m_UndoInfo.iEndChar;
		}
		break;
	case UOT_ADDTEXT:	// ����ı�
		{
			// ���ù��λ��
			m_iCurrentLine = m_UndoInfo.iLine;
			m_iCurrentChar = m_UndoInfo.iChar;

			// ɾ���ı�
			DelText(m_UndoInfo.iLine, m_UndoInfo.iChar, m_UndoInfo.iEndLine, m_UndoInfo.iEndChar);
		}
		break;
	}

	// ���¿ؼ�
	UpdateControl();

	// ���¹��
	UpdateCaret();
}

BOOL CCodeEdit::Find(LPTSTR lpszText, int iLen, BOOL bMatchCase, BOOL bMatchWhole, BOOL bDirectionDown)
{
	// ���Ʋ����ַ���
	LPTSTR lpszFind = LPTSTR(malloc((iLen+1)*sizeof(_TCHAR)));
	memcpy(lpszFind, lpszText, iLen*sizeof(_TCHAR));
	lpszFind[iLen] = '\0';

	if (bDirectionDown)
	{
		// ���忪ʼ����λ��
		int iLine, iChar;
		if ((m_iLineStartSelection != m_iLineEndSelection) || (m_iCharStartSelection != m_iCharEndSelection))
		{
			iLine = m_iLineEndSelection;
			iChar = m_iCharEndSelection;
		}
		else
		{
			iLine = m_iCurrentLine;
			iChar = m_iCurrentChar;
		}
		int iStartOffset = GetCharOffset(iLine, iChar);

		// ��ʼ����
		for (int i = iStartOffset; i <= m_iSize - iLen - 2; i++)
		{
			// �һ���
			if (m_lpszText[i] == '\r')
			{
				i++;
				iLine++;
				iChar = 0;
				continue;
			}

			// Ϊ����׼��
			if (m_lpszText[i] == '\n')
			{
				iLine++;
				iChar = 0;
				continue;
			}

			// ����Ҫ�Ƚϵ��ַ���
			LPTSTR lpszTemp = LPTSTR(malloc((iLen+1)*sizeof(_TCHAR)));
			memcpy(lpszTemp, m_lpszText+i, iLen*sizeof(_TCHAR));
			lpszTemp[iLen] = '\0';

			// ��������ִ�Сд
			if (!bMatchCase)
			{
				// ���ַ���תΪ��д
				_tcsupr_s(lpszFind, iLen+1);
				_tcsupr_s(lpszTemp, iLen+1);
			}

			// ��ʼƥ��
			BOOL bMatch = TRUE;
			for (int j = 0; j < iLen; j++)
			{
				if (!(lpszTemp[j] == lpszFind[j]))
				{
					bMatch = FALSE;
					break;
				}
			}

			free(lpszTemp);

			// ���ȫ��ƥ��
			if (bMatchWhole && bMatch)
			{
				if (i >= 1)
				{
					if(IsLetter(m_lpszText[i - 1]))
						bMatch = FALSE;
				}
				if (i <= m_iSize - iLen - 3)
				{
					if(IsLetter(m_lpszText[i + iLen]))
						bMatch = FALSE;
				}
			}

			// ���ƥ��ɹ�
			if (bMatch == TRUE)
			{
				// ����ѡ���͹��
				m_iLineStartSelection = iLine;
				m_iCharStartSelection = iChar;
				m_iLineEndSelection = iLine;
				m_iCharEndSelection = iChar + iLen;
				m_iCurrentLine = m_iLineEndSelection;
				m_iCurrentChar = m_iCharEndSelection;

				// ���¹��
				UpdateCaret();

				// ȷ���������ͼ��Χ
				EnsureVisible();

				free(lpszFind);

				return TRUE;
			}
			iChar++;
		}
	}
	else
	{
		// ���忪ʼ����λ��
		int iLine, iChar;
		if ((m_iLineStartSelection != m_iLineEndSelection) || (m_iCharStartSelection != m_iCharEndSelection))
		{
			iLine = m_iLineStartSelection;
			iChar = m_iCharStartSelection;
		}
		else
		{
			iLine = m_iCurrentLine;
			iChar = m_iCurrentChar;
		}
		int iStartOffset = GetCharOffset(iLine, iChar);

		// ��ʼ����
		for (int i = iStartOffset - 1; i >= iLen - 1; i--)
		{
			// �һ���
			if (m_lpszText[i] == '\n')
			{
				i--;
				iLine--;
				iChar = m_lpLineInfo[iLine].nLen;
				continue;
			}

			// Ϊ����׼��
			if (m_lpszText[i] == '\r')
			{
				iLine--;
				iChar = m_lpLineInfo[iLine].nLen;
				continue;
			}

			// ����Ҫ�Ƚϵ��ַ���
			LPTSTR lpszTemp = LPTSTR(malloc((iLen+1)*sizeof(_TCHAR)));
			memcpy(lpszTemp, m_lpszText+i-iLen+1, iLen*sizeof(_TCHAR));
			lpszTemp[iLen] = '\0';

			// ��������ִ�Сд
			if (!bMatchCase)
			{
				// ���ַ���תΪ��д
				_tcsupr_s(lpszFind, iLen+1);
				_tcsupr_s(lpszTemp, iLen+1);
			}

			// ��ʼƥ��
			BOOL bMatch = TRUE;
			for (int j = 0; j < iLen; j++)
			{
				if (!(lpszTemp[j] == lpszFind[j]))
				{
					bMatch = FALSE;
					break;
				}
			}

			free(lpszTemp);

			// ���ȫ��ƥ��
			if (bMatchWhole && bMatch)
			{
				if (i <= m_iSize - 3)
				{
					if(IsLetter(m_lpszText[i + 1]))
						bMatch = FALSE;
				}
				if (i >= iLen)
				{
					if(IsLetter(m_lpszText[i - iLen]))
						bMatch = FALSE;
				}
			}

			// ���ƥ��ɹ�
			if (bMatch == TRUE)
			{
				// ����ѡ���͹��
				m_iLineStartSelection = iLine;
				m_iCharStartSelection = iChar - iLen;
				m_iLineEndSelection = iLine;
				m_iCharEndSelection = iChar;
				m_iCurrentLine = m_iLineEndSelection;
				m_iCurrentChar = m_iCharEndSelection;

				// ���¹��
				UpdateCaret();

				// ȷ���������ͼ��Χ
				EnsureVisible();

				free(lpszFind);

				return TRUE;
			}
			iChar--;
		}
	}
	free(lpszFind);

	return FALSE;
}

BOOL CCodeEdit::Replace(LPTSTR lpszText, int iLen, LPTSTR lpszText2, int iLen2, BOOL bMatchCase, BOOL bMatchWhole)
{
	// ���Ʋ����ַ���
	LPTSTR lpszFind = LPTSTR(malloc((iLen+1)*sizeof(_TCHAR)));
	memcpy(lpszFind, lpszText, iLen*sizeof(_TCHAR));
	lpszFind[iLen] = '\0';

	// �����滻�ַ���
	LPTSTR lpszReplace = LPTSTR(malloc((iLen2+1)*sizeof(_TCHAR)));
	memcpy(lpszReplace, lpszText2, iLen2*sizeof(_TCHAR));
	lpszReplace[iLen2] = '\0';

	// ����ѡ���ַ���
	LPTSTR lpszTemp = LPTSTR(malloc((iLen+1)*sizeof(_TCHAR)));
	if ((m_iLineStartSelection != m_iLineEndSelection) || (m_iCharStartSelection != m_iCharEndSelection))
	{
		int iLine, iChar;

		iLine = m_iLineStartSelection;
		iChar = m_iCharStartSelection;

		int iSelection = GetCharOffset(iLine, iChar);
		memcpy(lpszTemp, m_lpszText+iSelection, iLen*sizeof(_TCHAR));
		lpszTemp[iLen] = '\0';

		// ��������ִ�Сд
		if (!bMatchCase)
		{
			// ���ַ���תΪ��д
			_tcsupr_s(lpszFind, iLen+1);
			_tcsupr_s(lpszTemp, iLen+1);
		}

		// ��ʼƥ��
		BOOL bMatch = TRUE;
		for (int j = 0; j < iLen; j++)
		{
			if (!(lpszTemp[j] == lpszFind[j]))
			{
				bMatch = FALSE;
				break;
			}
		}

		// ���ƥ�䣬���滻�ַ���
		if (bMatch)
		{
			// ���泷����Ϣ
			if (iLen2 == 0)
			{
				if (m_UndoInfo.lpszUndoData == NULL)
					m_UndoInfo.lpszUndoData = (LPTSTR)malloc(iLen*sizeof(_TCHAR));
				else
					m_UndoInfo.lpszUndoData = (LPTSTR)realloc(m_UndoInfo.lpszUndoData, iLen*sizeof(_TCHAR));
			}
			else
			{
				if (m_UndoInfo.lpszUndoData == NULL)
					m_UndoInfo.lpszUndoData = (LPTSTR)malloc(iLen2*sizeof(_TCHAR));
				else
					m_UndoInfo.lpszUndoData = (LPTSTR)realloc(m_UndoInfo.lpszUndoData, iLen2*sizeof(_TCHAR));
			}
			m_UndoInfo.nLen = (iLen2 == 0) ? iLen : iLen2;
			m_UndoInfo.iLine = iLine;
			m_UndoInfo.iChar = iChar;
			m_UndoInfo.iEndLine = iLine;
			m_UndoInfo.iEndChar = (iLen2 == 0) ? (iChar + iLen) : (iChar + iLen2);
			if (iLen2 == 0)
			{
				memcpy(m_UndoInfo.lpszUndoData, lpszText, iLen*sizeof(_TCHAR));
				m_UndoInfo.nOperation = UOT_DELTEXT;
			}
			else
			{
				memcpy(m_UndoInfo.lpszUndoData, lpszText2, iLen2*sizeof(_TCHAR));
				m_UndoInfo.nOperation = UOT_ADDTEXT;
			}
			m_bCanUndo = TRUE;

			int iOldSize = m_iSize;
			LPTSTR lpszTmp = new _TCHAR[iOldSize];
			memcpy(lpszTmp, m_lpszText, iOldSize*sizeof(_TCHAR));
			m_iSize += (iLen2 - iLen);
			free(m_lpszText);
			m_lpszText = (LPTSTR)malloc(m_iSize*sizeof(_TCHAR));
			memcpy(m_lpszText, lpszTmp, iSelection*sizeof(_TCHAR));
			memcpy(m_lpszText+iSelection, lpszReplace, iLen2*sizeof(_TCHAR));
			memcpy(m_lpszText+iSelection+iLen2, lpszTmp+iSelection+iLen, (iOldSize-iSelection)*sizeof(_TCHAR));
			delete lpszTmp;

			m_changed = TRUE;

			// ���ѡ��
			m_iLineStartSelection = -1;
			m_iLineEndSelection = -1;
			m_iCharStartSelection = -1;
			m_iCharEndSelection = -1;

			// ���ù��
			m_iCurrentChar = iChar + iLen2;
			m_iCurrentLine = iLine;

			// ���¿ؼ�
			UpdateControl();

			// ������ʾ
			Invalidate();

			// ���¹��
			UpdateCaret();

			free(lpszTemp);
			free(lpszFind);
			free(lpszReplace);

			// ������һ��
			if (Find(lpszText, iLen, bMatchCase, bMatchWhole, TRUE))
				return TRUE;
			else
				return FALSE;
		}
	}
	free(lpszTemp);
	free(lpszFind);
	free(lpszReplace);

	// ������һ��
	if (Find(lpszText, iLen, bMatchCase, bMatchWhole, TRUE)) return TRUE;

	return FALSE;
}

void CCodeEdit::ReplaceAll(LPTSTR lpszText, int iLen, LPTSTR lpszText2, int iLen2, BOOL bMatchCase, BOOL bMatchWhole)
{
	// ���Ʋ����ַ���
	LPTSTR lpszFind = LPTSTR(malloc((iLen+1)*sizeof(_TCHAR)));
	memcpy(lpszFind, lpszText, iLen*sizeof(_TCHAR));
	lpszFind[iLen] = '\0';

	// �����滻�ַ���
	LPTSTR lpszReplace = LPTSTR(malloc((iLen2+1)*sizeof(_TCHAR)));
	memcpy(lpszReplace, lpszText2, iLen2*sizeof(_TCHAR));
	lpszReplace[iLen2] = '\0';

	// ���忪ʼ����λ��
	int iLine = 0, iChar = 0;

	// ��ʼ����
	for (int i = 0; i <= m_iSize - iLen - 2; i++)
	{
		// �һ���
		if (m_lpszText[i] == '\r')
		{
			i++;
			iLine++;
			iChar = 0;
			continue;
		}

		// Ϊ����׼��
		if (m_lpszText[i] == '\n')
		{
			iLine++;
			iChar = 0;
			continue;
		}

		// ����Ҫ�Ƚϵ��ַ���
		LPTSTR lpszTemp = LPTSTR(malloc((iLen+1)*sizeof(_TCHAR)));
		memcpy(lpszTemp, m_lpszText+i, iLen*sizeof(_TCHAR));
		lpszTemp[iLen] = '\0';

		// ��������ִ�Сд
		if (!bMatchCase)
		{
			// ���ַ���תΪ��д
			_tcsupr_s(lpszFind, iLen+1);
			_tcsupr_s(lpszTemp, iLen+1);
		}

		// ��ʼƥ��
		BOOL bMatch = TRUE;
		for (int j = 0; j < iLen; j++)
		{
			if (!(lpszTemp[j] == lpszFind[j]))
			{
				bMatch = FALSE;
				break;
			}
		}

		free(lpszTemp);

		// ���ȫ��ƥ��
		if (bMatchWhole && bMatch)
		{
			if (i >= 1)
			{
				if(IsLetter(m_lpszText[i - 1]))
					bMatch = FALSE;
			}
			if (i <= m_iSize - iLen - 3)
			{
				if(IsLetter(m_lpszText[i + iLen]))
					bMatch = FALSE;
			}
		}

		// ���ƥ��ɹ�
		if (bMatch == TRUE)
		{
			int iOldSize = m_iSize;
			LPTSTR lpszTmp = new _TCHAR[iOldSize];
			memcpy(lpszTmp, m_lpszText, iOldSize*sizeof(_TCHAR));
			m_iSize += (iLen2 - iLen);
			free(m_lpszText);
			m_lpszText = (LPTSTR)malloc(m_iSize*sizeof(_TCHAR));
			memcpy(m_lpszText, lpszTmp, i*sizeof(_TCHAR));
			memcpy(m_lpszText+i, lpszReplace, iLen2*sizeof(_TCHAR));
			memcpy(m_lpszText+i+iLen2, lpszTmp+i+iLen, (iOldSize-i)*sizeof(_TCHAR));
			delete lpszTmp;

			m_changed = TRUE;

			iChar += (iLen2 - iLen);
		}
		iChar++;
	}
	free(lpszFind);
	free(lpszReplace);

	// ���ѡ��
	m_iLineStartSelection = -1;
	m_iLineEndSelection = -1;
	m_iCharStartSelection = -1;
	m_iCharEndSelection = -1;

	// ���λ��
	if (m_iCurrentChar > m_lpLineInfo[m_iCurrentLine].nLen)
		m_iCurrentChar = m_lpLineInfo[m_iCurrentLine].nLen;

	// ���¿ؼ�
	UpdateControl();
	
	// ���¹��
	UpdateCaret();

	// ȷ���������ͼ��Χ
	EnsureVisible();
}

BOOL CCodeEdit::GetUndoStatus()
{
	return m_bCanUndo;
}

BOOL CCodeEdit::Load(LPTSTR lpszFilePath)
{
	BOOL bResult = FALSE;

	// �򿪴����ļ�
	CFile file;
	if (file.Open(lpszFilePath, CFile::modeRead, NULL))
	{
		// ���������
		if (m_lpszText != NULL)
			free(m_lpszText);

		// ���ѡ��
		m_iLineStartSelection = -1;
		m_iLineEndSelection = -1;
		m_iCharStartSelection = -1;
		m_iCharEndSelection = -1;

		// ���ļ���ȡ����
		DWORD dwSize = (DWORD)file.GetLength();
		LPBYTE lpData = (LPBYTE)malloc(dwSize*sizeof(BYTE));
		file.Read(lpData, dwSize);
		file.Close();
		bResult = TRUE;

		// ����ļ�����
		WORD wFlag = 0;
		memcpy(&wFlag, lpData, 2);
		DWORD dwFlag = 0;
		memcpy(&dwFlag, lpData, 3);
		int iOffset;
		if (wFlag == 0xFEFF)
		{
			// ���� UNICODE ����
			iOffset = 1;
			if (dwSize == sizeof(_TCHAR))
			{
				m_iSize = 0;
			}
			else
			{
				m_iSize = (dwSize-sizeof(_TCHAR)) / sizeof(_TCHAR);
				m_lpszText = (LPTSTR)malloc(m_iSize*sizeof(_TCHAR));
				memcpy(m_lpszText, (LPTSTR)lpData+iOffset, m_iSize*sizeof(_TCHAR));
			}
		}
		else if (wFlag == 0xFFFE)
		{
			// ���� UNICODE BIG ENDIAN ����
			iOffset = 1;
			if (dwSize == sizeof(_TCHAR))
			{
				m_iSize = 0;
			}
			else
			{
				BYTE *pText = (BYTE*)lpData+iOffset;
				// ����ÿ�����ֽڵ�λ��
				for(DWORD i = 0; i < dwSize - 3; i += 2)
				{
					BYTE tmp = pText[iOffset+i];
					pText[iOffset+i] = pText[iOffset+i+1];
					pText[iOffset+i+1] = tmp;
				}
				m_iSize = (dwSize-sizeof(_TCHAR)) / sizeof(_TCHAR);
				m_lpszText = (LPTSTR)malloc(m_iSize*sizeof(_TCHAR));
				memcpy(m_lpszText, (LPTSTR)lpData+iOffset, m_iSize*sizeof(_TCHAR));
				pText = NULL;
				delete pText;
			}
		}
		else if (dwFlag == 0xBFBBEF)
		{
			// �� UTF-8 ת���� UNICODE ����
			iOffset = 3;
			if (dwSize == 3)
			{
				m_iSize = 0;
			}
			else
			{
				m_iSize = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)lpData+iOffset, dwSize-3, NULL, 0);
				m_lpszText = (LPTSTR)malloc(m_iSize*sizeof(_TCHAR));
				MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)lpData+iOffset, dwSize-3, m_lpszText, m_iSize);
			}
		}
		else
		{
			// �� ANSI ת���� UNICODE ����
			iOffset = 0;
			if (dwSize == 0)
			{
				m_iSize = 0;
			}
			else
			{
				m_iSize = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)lpData+iOffset, dwSize, NULL, 0);
				m_lpszText = (LPTSTR)malloc(m_iSize*sizeof(_TCHAR));
				MultiByteToWideChar(CP_ACP, 0, (LPCSTR)lpData+iOffset, dwSize, m_lpszText, m_iSize);
			}
		}
		free(lpData);

		// �ָ�������λ��
		SetScrollPos(SB_HORZ, 0, TRUE);
		SetScrollPos(SB_VERT, 0, TRUE);

		// ȡ���б����ʾ״̬
		m_bCopying = FALSE;

		// �����ı�
		m_iSize += 2;
		if (m_iSize == 2)
			m_lpszText = (LPTSTR)malloc(2*sizeof(_TCHAR));
		else
			m_lpszText = (LPTSTR)realloc(m_lpszText, m_iSize*sizeof(_TCHAR));
		m_lpszText[m_iSize-2] = '\r';
		m_lpszText[m_iSize-1] = '\n';

		if (m_UndoInfo.lpszUndoData == NULL)
			m_UndoInfo.lpszUndoData = (LPTSTR)malloc(sizeof(_TCHAR));
		else
			m_UndoInfo.lpszUndoData = (LPTSTR)realloc(m_UndoInfo.lpszUndoData, sizeof(_TCHAR));
		m_UndoInfo.nLen = 0;
		m_UndoInfo.iChar = 0;
		m_UndoInfo.iLine = 0;
		m_UndoInfo.iEndChar = 0;
		m_UndoInfo.iEndLine = 0;
		m_UndoInfo.iLastLineLen = 0;

		m_changed = FALSE;
		m_bCanUndo = FALSE;

		// �����б��
		DestroyList();

		// ���¿ؼ�
		UpdateControl();

		// ������Ļ
		Invalidate(FALSE);
		UpdateWindow();
	}

	return bResult;
}

BOOL CCodeEdit::Save(LPTSTR lpszFilePath)
{
	BOOL bResult = FALSE;

	switch(m_FileEncoding)
	{
	case FET_ANSI:
		{
			// ���� ANSI �ַ�������
			int ansiLen = WideCharToMultiByte(CP_ACP, NULL, m_lpszText, m_iSize - 2, NULL, 0, NULL, NULL);
			// ����ռ�Ҫ��'\0'�����ռ�
			char* szAnsi = new char[ansiLen];
			// ת��
			WideCharToMultiByte(CP_ACP, NULL, m_lpszText, m_iSize - 2, szAnsi, ansiLen, NULL, NULL);
			// д�ı��ļ���ANSI�ļ�û��BOM
			CFile cFile;
			if (cFile.Open(lpszFilePath, CFile::modeWrite | CFile::modeCreate))
			{
				// �ļ���ͷ
				cFile.SeekToBegin();
				// д������
				cFile.Write(szAnsi, ansiLen * sizeof(char));
				cFile.Flush();
				cFile.Close();
				m_changed = FALSE;
				bResult = TRUE;
			}
			delete[] szAnsi;
			szAnsi = NULL;
		}
		break;
	case FET_UNICODE:
		{
			CFile cFile;
			if (cFile.Open(lpszFilePath, CFile::modeWrite | CFile::modeCreate))
			{
				// �ļ���ͷ
				cFile.SeekToBegin();
				// ͷ 2 ���ֽ� 0xfeff����λ 0xff д��ǰ
				cFile.Write("\xff\xfe", 2);
				// д������
				cFile.Write(m_lpszText, (m_iSize - 2) * sizeof(wchar_t));
				cFile.Flush();
				cFile.Close();
				m_changed = FALSE;
				bResult = TRUE;
			}
		}
		break;
	case FET_UNICODE_BE:
		{
			DWORD dwSize = (DWORD)((m_iSize - 2) * sizeof(wchar_t));
			LPBYTE lpData = (LPBYTE)malloc(dwSize*sizeof(BYTE));
			memcpy(lpData, (LPBYTE)m_lpszText, dwSize*sizeof(BYTE));
			// ����ÿ�����ֽڵ�λ��
			for(DWORD i = 0; i < dwSize; i += 2)
			{
				BYTE tmp = lpData[i];
				lpData[i] = lpData[i+1];
				lpData[i+1] = tmp;
			}
			CFile cFile;
			if (cFile.Open(lpszFilePath, CFile::modeWrite | CFile::modeCreate))
			{
				// �ļ���ͷ
				cFile.SeekToBegin();
				// ͷ 2 ���ֽ� 0xfffe����λ 0xfe д��ǰ
				cFile.Write("\xfe\xff", 2);
				// д������
				cFile.Write(lpData, dwSize);
				cFile.Flush();
				cFile.Close();
				m_changed = FALSE;
				bResult = TRUE;
			}
		}
		break;
	case FET_UTF_8:
		{
			int u8Len = WideCharToMultiByte(CP_UTF8, NULL, m_lpszText, m_iSize - 2, NULL, 0, NULL, NULL);
			// UTF8 ��Ȼ�� Unicode ��ѹ����ʽ����Ҳ�Ƕ��ֽ��ַ��������Կ����� char ����ʽ����
			char* szU8 = new char[u8Len];
			// ת��
			WideCharToMultiByte(CP_UTF8, NULL, m_lpszText, m_iSize - 2, szU8, u8Len, NULL, NULL);
			// д�ı��ļ���UTF8 �� BOM �� 0xbfbbef
			CFile cFile;
			if (cFile.Open(lpszFilePath, CFile::modeWrite | CFile::modeCreate))
			{
				// �ļ���ͷ
				cFile.SeekToBegin();
				// д BOM��ͬ����λд��ǰ
				cFile.Write("\xef\xbb\xbf", 3);
				// д������
				cFile.Write(szU8, u8Len * sizeof(char));
				cFile.Flush();
				cFile.Close();
				m_changed = FALSE;
				bResult = TRUE;
			}
			delete[] szU8;
			szU8 =NULL;
		}
		break;
	}
	return bResult;
}

BOOL CCodeEdit::GetChangedStatus()
{
	return m_changed;
}

void CCodeEdit::SetChangedStatus(BOOL status)
{
	m_changed = status;
}

void CCodeEdit::DeleteText(int iStartLine, int iStartChar, int iEndLine, int iEndChar)
{
	// �ı���Ϊ��
	if (m_lpszText != NULL)
	{
		// ��ʽ��ѡ��
		m_iLineStartSelection = max(0, iStartLine);
		m_iCharStartSelection = max(0, iStartChar);
		m_iLineEndSelection = min(m_iNumberLines-1, iEndLine);
		if (m_iLineEndSelection == -1)
			m_iLineEndSelection = m_iNumberLines - 1;
		m_iCharEndSelection = min(m_lpLineInfo[m_iLineEndSelection].nLen, iEndChar);
		if (m_iCharEndSelection == -1)
			m_iCharEndSelection = m_lpLineInfo[m_iLineEndSelection].nLen;

		// ɾ��ѡ�е�����
		OnChar(VK_BACK, 0, 0);
		m_changed = TRUE;
		m_bCanUndo = TRUE;
	}
}

LPTSTR CCodeEdit::GetTextBuffer()
{
	return m_lpszText;
}

int CCodeEdit::GetBufferLen()
{
	return m_iSize;
}

void CCodeEdit::GetTextFromLine(int iLineIndex, LPTSTR lpszText)
{
	// �����Ч����
	int iLine = max(0, min(m_iNumberLines-1, iLineIndex));
	if (lpszText != NULL)
	{
		// ������ı�
		int iOffset = m_lpLineInfo[iLine].dwOffset;
		int iOldOffset = iOffset;
		LPTSTR lpszNextLine = NULL;
		if ((lpszNextLine=GetLine(iOffset)) != NULL)
		{
			// �����г���
			int iLen = iOffset - iOldOffset;
			if ((lpszNextLine[iLen-2] == '\r') && (lpszNextLine[iLen-1] == '\n'))
				iLen = iLen - 2;
			else if (lpszNextLine[iLen-1] == '\n')
				iLen = iLen - 1;

			// ������
			_tcsncpy_s(lpszText, iLen+1, lpszNextLine, iLen);
			lpszText[iLen] = '\0';
		}
	}
}

int CCodeEdit::LineFromPosition(POINT pt)
{
	int iResult = -1;

	// �����Ч����
	if (m_lpszText != NULL)
	{
		// ����к�
		iResult = min(pt.y/m_szTextSize.cy, m_iNumberLines-1);
	}

	return iResult;
}

int CCodeEdit::CharFromPosition(int iLineIndex, POINT pt)
{
	int iResult = -1;

	// �����Ч����
	if (m_lpszText != NULL)
	{
		// ����ַ����
		int iOffset = m_lpLineInfo[iLineIndex].dwOffset;
		int iOldOffset = iOffset;
		int iCurrentLine = iLineIndex;
		LPTSTR lpszNextLine = NULL;
		while ((lpszNextLine=GetLine(iOffset)) != NULL)
		{
			// �����г���
			int iLen = iOffset - iOldOffset;
			iOldOffset = iOffset;

			// �����Ч��
			if (iCurrentLine == iLineIndex)
			{
				// ����ַ����
				BOOL bDone = FALSE;
				int iStartTAB=-1;
				int iEndTAB, iTABs;
				int iCharOffset = 0;
				for (int i=0; i<m_lpLineInfo[iLineIndex].nLen; i++)
				{
					// ��� TAB �ַ�
					if (lpszNextLine[i] == '\t')
					{
						if (iStartTAB == -1)
						{
							iTABs = m_iTABs;
							iStartTAB = i;
						}
						else
						{
							iEndTAB = i;
							iTABs = m_iTABs - ((iEndTAB - iStartTAB) % m_iTABs);
							if (((iEndTAB - iStartTAB) % m_iTABs) != 0)
								iTABs++;
							if ((iEndTAB - iStartTAB) == 1)
								iTABs = m_iTABs;
							iStartTAB = iEndTAB;
						}
						iCharOffset += (iTABs * m_szTextSize.cx);
					}
					else
					{
						if(IsCJKChar(lpszNextLine[i]))		// �ַ���CJK����
						{
							iCharOffset += m_szCJKTextSize.cx;
						}
						else
							iCharOffset += m_szTextSize.cx;
					}
					int PointOffset;
					if (m_LineNumbers)
						PointOffset = (int(pt.x) - 4 * m_szTextSize.cx - LINE_NUMBER_TO_EDITOR);
					else
						PointOffset = int(pt.x) + m_szTextSize.cx - LINE_NUMBER_TO_EDITOR;
					if (iCharOffset >= PointOffset)
					{
						bDone = TRUE;
						iResult = i;
						break;
					}
				}
				if (!bDone)
					iResult = m_lpLineInfo[iLineIndex].nLen;
			}
			iCurrentLine++;
			break;
		}
	}

	return iResult;
}

POINT CCodeEdit::PositionFromChar(int iLineIndex, int iCharIndex)
{
	POINT ptResult = {0, 0};

	// �����Ч����
	if (m_lpszText != NULL)
	{
		// ����ַ�λ��
		int iOffset = m_lpLineInfo[iLineIndex].dwOffset;
		int iOldOffset = iOffset;
		int iCurrentLine = iLineIndex;
		LPTSTR lpszNextLine = NULL;
		while ((lpszNextLine=GetLine(iOffset)) != NULL)
		{
			// �����г���
			int iLen = iOffset - iOldOffset;
			iOldOffset = iOffset;

			// �����Ч��
			if (iCurrentLine == iLineIndex)
			{
				// ����ַ����
				BOOL bDone = FALSE;
				int iStartTAB=-1;
				int iEndTAB, iTABs;
				int iCharOffset = 0;
				for (int i=0; i<=m_lpLineInfo[iLineIndex].nLen; i++)
				{
					// ����ҵ����ַ�
					if (i == iCharIndex)
					{
						bDone = TRUE;
						ptResult.y = iLineIndex * m_szTextSize.cy;
						ptResult.x = iCharOffset;
						break;
					}
					else
					{
						// ��� TAB �ַ�
						if (lpszNextLine[i] == '\t')
						{
							if (iStartTAB == -1)
							{
								iTABs = m_iTABs;
								iStartTAB = i;
							}
							else
							{
								iEndTAB = i;
								iTABs = m_iTABs - ((iEndTAB - iStartTAB) % m_iTABs);
								if (((iEndTAB - iStartTAB) % m_iTABs) != 0)
									iTABs++;
								if ((iEndTAB - iStartTAB) == 1)
									iTABs = m_iTABs;
								iStartTAB = iEndTAB;
							}
							iCharOffset += (iTABs * m_szTextSize.cx);
						}
						else
						{
							if(IsCJKChar(lpszNextLine[i]))		// �ַ���CJK����
								iCharOffset += m_szCJKTextSize.cx;
							else
								iCharOffset += m_szTextSize.cx;
						}
					}
				}
				if (!bDone)
				{
					ptResult.y = iLineIndex * m_szTextSize.cy;
					ptResult.x = iCharOffset - m_szTextSize.cx;
				}
			}
			iCurrentLine++;
			break;
		}
	}

	ptResult.x += LINE_NUMBER_TO_EDITOR;
	if (m_LineNumbers) ptResult.x += 5 * m_szTextSize.cx;

	return ptResult;
}

int CCodeEdit::GetCharOffset(int iLineIndex, int iCharIndex)
{
	int iResult = 0;

	// �����Ч����
	if (m_lpszText != NULL)
	{
		// ����ַ�ƫ����
		iResult = m_lpLineInfo[iLineIndex].dwOffset + iCharIndex;
	}

	return iResult;
}

int CCodeEdit::GetCharIndex(int iLine, int iColumn)
{
	int iCNum=0, i=0;

	// �����ж��ַ�����
	for (i = 0; i < m_lpLineInfo[iLine].nLen; i++)
	{
		if (IsCJKChar(m_lpszText[m_lpLineInfo[iLine].dwOffset + i]))
			iCNum += 2;
		else if (m_lpszText[m_lpLineInfo[iLine].dwOffset + i] == '\t')
			iCNum += m_iTABs;
		else
			iCNum++;
		for (int j = 0; j < m_iTABs; j++)
		{
			if (iCNum == iColumn + j) return i+1;
		}
	}
	return i+1;
}

int CCodeEdit::GetColumnIndex(int iLine, int iChar)
{
	int iCNum=0, i=0;

	// ���������ַ����
	for (i = 0; i < iChar; i++)
	{
		if (IsCJKChar(m_lpszText[m_lpLineInfo[iLine].dwOffset + i]))
			iCNum += 2;
		else if (m_lpszText[m_lpLineInfo[iLine].dwOffset + i] == '\t')
			iCNum += m_iTABs;
		else
			iCNum++;
	}

	return iCNum;
}

void CCodeEdit::UpdateCaret()
{
	// ȷ�����ɼ�
	EnsureVisible(m_iCurrentLine, m_iCurrentChar);

	// ��ù�������Ϣ
	int iVerticalOffset = GetScrollPos(SB_VERT);
	int iHorizontalOffset = GetScrollPos(SB_HORZ);

	// �����ַ�λ��
	POINT pt = PositionFromChar(m_iCurrentLine, m_iCurrentChar);
	pt.x -= iHorizontalOffset;
	pt.y -= iVerticalOffset;
	SetCaretPos(pt);
}

void CCodeEdit::EnsureVisible(int iLineIndex, int iCharIndex)
{
	// �����Ч����
	if (m_lpszText != NULL)
	{
		// �����Ч�ı��ߴ�
		if (m_iSize == 2)
		{
			// ������Ļ
			Invalidate(FALSE);
			UpdateWindow();
			return;
		}

		// ������ڻ��Ʊ�־
		SetRedraw(FALSE);

		// ��ÿͻ�����С��λ��
		RECT rcClient;
		GetClientRect(&rcClient);

		// ��ù�������Ϣ
		int iVerticalOffset = GetScrollPos(SB_VERT);
		int iHorizontalOffset = GetScrollPos(SB_HORZ);

		// ��ÿɼ��з�Χ
		POINT ptTop = {0, rcClient.top+iVerticalOffset};
		POINT ptBottom = {0, (rcClient.bottom-rcClient.top)+iVerticalOffset};
		int iLineStart = LineFromPosition(ptTop);
		int iLineEnd = LineFromPosition(ptBottom);
		RECT rcLine = {-iHorizontalOffset, 0, rcClient.right, m_szTextSize.cy};
		POINT ptLeft = {iHorizontalOffset, 0};
		POINT ptRight = {rcLine.right+iHorizontalOffset, 0};
		int iCharStart = CharFromPosition(iLineIndex, ptLeft);
		int iCharEnd = CharFromPosition(iLineIndex, ptRight);

		// ��õ�ǰ���λ��
		POINT ptCurrent = PositionFromChar(iLineIndex, iCharIndex);

		// ȷ�����ɼ�
		if (iLineIndex >= iLineEnd)
		{
			// ���¹���
			int iNumberScrolls = iLineIndex - iLineEnd;
			for (int i=0; i<=iNumberScrolls; i++)
				OnVScroll(SB_LINEDOWN, 0, NULL);
		}
		else if (iLineIndex < iLineStart)
		{
			// ���Ϲ���
			int iNumberScrolls = iLineStart - iLineIndex;
			for (int i=0; i<iNumberScrolls; i++)
				OnVScroll(SB_LINEUP, 0, NULL);
		}
		if ((iCharIndex > iCharEnd) && (iLineIndex <= m_iNumberLines-1))
		{
			// ���ҹ���
			POINT ptEnd = PositionFromChar(iLineIndex, iCharEnd);
			int iNumberScrolls = (ptCurrent.x - ptEnd.x) / m_szTextSize.cx;
			for (int i=0; i<iNumberScrolls; i++)
				OnHScroll(SB_LINERIGHT, 0, NULL);
		}
		else if (iCharIndex <= iCharStart)
		{
			// �������
			int iNumberScrolls = (iHorizontalOffset - ptCurrent.x) / m_szTextSize.cx;
			for (int i=0; i<=iNumberScrolls; i++)
				OnHScroll(SB_LINELEFT, 0, NULL);
		}

		// �������ڻ��Ʊ�־
		SetRedraw(TRUE);
	}

	// ������Ļ
	Invalidate(FALSE);
	UpdateWindow();
}

LPTSTR CCodeEdit::GetLine(int& iOffset)
{
	LPTSTR lpszResult = NULL;

	// �����Чƫ����
	if (iOffset < m_iSize)
	{
		// ��ȡ��һ��
		lpszResult = m_lpszText + iOffset;
		LPTSTR lpszNextLine = _tcsstr(lpszResult, _T("\r\n"));
		if (lpszNextLine != NULL)
		{
			// ����ƫ����
			iOffset = (int)(lpszNextLine - m_lpszText + 2);
		}
		else
		{
			// ����ƫ����
			iOffset = m_iSize;
		}
	}

	return lpszResult;
}

void CCodeEdit::SelectWord(POINT pt, BOOL FirstLetter)
{
	// ��ù�������Ϣ
	int iVerticalOffset = GetScrollPos(SB_VERT);
	int iHorizontalOffset = GetScrollPos(SB_HORZ);

	// ����ַ�
	int iSelectedLine = LineFromPosition(pt);
	int iSelectedChar = CharFromPosition(iSelectedLine, pt);

	// ��鵥��˫��
	int iOffset = m_lpLineInfo[iSelectedLine].dwOffset;
	int iOldOffset = iOffset;
	int iCurrentLine = iSelectedLine;
	LPTSTR lpszNextLine = NULL;
	while ((lpszNextLine=GetLine(iOffset)) != NULL)
	{
		// �����г���
		int iLen = iOffset - iOldOffset;
		iOldOffset = iOffset;

		// �����Ч��
		if (iCurrentLine == iSelectedLine)
		{
			// ���ѡ�еĵ���
			if (!IsSpecial(lpszNextLine[iSelectedChar]))
			{
				// ѡ�񵥴�
				int iStart = iSelectedChar;
				while ((IsLetter(lpszNextLine[iStart])) && (iStart > 0))
					iStart--;
				if ((iStart >= 0) && !IsLetter(lpszNextLine[iStart]))
					iStart++;
				int iEnd = iSelectedChar;
				while ((IsLetter(lpszNextLine[iEnd])) && (iEnd < iLen))
					iEnd++;

				// ����ѡ��
				m_iLineStartSelection = iSelectedLine;
				m_iLineEndSelection = iSelectedLine;
				m_iCharStartSelection = iStart;
				m_iCharEndSelection = iEnd;

				// ���ù��λ��
				m_iCurrentLine = iSelectedLine;
				if (FirstLetter)
					m_iCurrentChar = iStart;					
				else
					m_iCurrentChar = iEnd;
				POINT pt = PositionFromChar(m_iCurrentLine, m_iCurrentChar);
				pt.x -= iHorizontalOffset;
				pt.y -= iVerticalOffset;
				SetCaretPos(pt);
			}
			else
			{
				// ���ѡ��
				m_iLineStartSelection = -1;
				m_iLineEndSelection = -1;
				m_iCharStartSelection = -1;
				m_iCharEndSelection = -1;
			}
			break;
		}
		iCurrentLine++;
	}
}

BOOL CCodeEdit::IsSpecial(_TCHAR tch)
{
	BOOL bResult = FALSE;

	// ��������ַ� ���� �ո�, TAB, ����, �س�
	if ((tch == ' ') || (tch == '\t') || (tch == '\r') || (tch == '\n'))
	{
		// �˳�
		bResult = TRUE;
	}

	return bResult;
}

BOOL CCodeEdit::IsLetter(_TCHAR tch)
{
	BOOL bResult = FALSE;

	// �����ĸ������ ���� 'A' �� 'Z', 'a' �� 'z', '0' �� '9'
	if (((tch >= 'A') && (tch <= 'Z')) || ((tch >= 'a') && (tch <= 'z')) || ((tch >= '0') && (tch <= '9')) || (tch == '_') || (tch == '#'))
	{
		// �˳�
		bResult = TRUE;
	}

	return bResult;
}

BOOL CCodeEdit::IsCJKChar(_TCHAR tch)
{
	BOOL bResult = FALSE;

	// ����Ƿ���Unicode����CJK�ַ���Χ֮��
	if ((tch >= 0x4E00 && tch <= 0x9FA5) || (tch >= 0x3400 && tch <= 0x4DB5) 
		|| (tch >= 0x20000 && tch <= 0x2A6D6) || (tch >= 0x9FA6 && tch <= 0x9FBB)
		|| (tch >= 0x2F800 && tch <= 0x2FA1D) || (tch >= 0xF900 && tch <= 0xFA2D)
		|| (tch >= 0xFA30 && tch <= 0xFA6A) || (tch >= 0xFA70 && tch <= 0xFAD9)
		|| (tch >= 0xFF00 && tch <= 0xFFEF)	|| (tch >= 0x2E80 && tch <= 0x2EFF)
		|| (tch >= 0x2010 && tch <= 0x33D5))
	{
		// �˳�
		bResult = TRUE;
	}

	return bResult;
}

BOOL CCodeEdit::IsKeyword(LPTSTR lpszText, int iLen, int& iKeywordIndex, int& iKeywordOffset)
{
	BOOL bResult = FALSE;

	// �����Ч����
	if (iLen == 0)
		return FALSE;

	// ��ʽ���ı�
	LPTSTR lpszKeywordText = new _TCHAR[iLen+1];
	_tcsncpy_s(lpszKeywordText, iLen+1, lpszText, iLen);
	lpszKeywordText[iLen] = '\0';
	if (!m_bKeywordsCaseSensitive)
	{
		if (m_bUper)
			_tcsupr_s(lpszKeywordText, iLen+1);
		else
			_tcslwr_s(lpszKeywordText, iLen+1);
	}

	// ���ؼ���
	for (int i=0; i<m_iNumberKeywords; i++)
	{
		// ���ؼ���
		LPTSTR lpszSearch = _tcsstr(lpszKeywordText, m_lpKeywordInfo[i].lpszTagName);
		if (lpszSearch != NULL)
		{
			// �����Ч�ؼ���
			int iFirst = (int)(lpszSearch - lpszKeywordText - 1);
			int iLast = iFirst + m_lpKeywordInfo[i].nLen;
			bResult = TRUE;
			if (iFirst >= 0)
			{
				if (m_SyntaxColoring != SCT_HTML)
				{
					if (((lpszKeywordText[iFirst] >= 'A') && (lpszKeywordText[iFirst] <= 'Z')) || 
						((lpszKeywordText[iFirst] >= 'a') && (lpszKeywordText[iFirst] <= 'z')))
						bResult = FALSE;
				}

			}
			if (bResult)
			{
				if (((lpszKeywordText[iLast] >= 'A') && (lpszKeywordText[iLast] <= 'Z')) || 
					((lpszKeywordText[iLast] >= 'a') && (lpszKeywordText[iLast] <= 'z')))
					bResult = FALSE;
				else
				{
					// �˳�
					iKeywordIndex = i;
					iKeywordOffset = iFirst + 1;
					break;
				}
			}
		}
	}
	delete lpszKeywordText;

	return bResult;
}

BOOL CCodeEdit::IsConstant(LPTSTR lpszText, int iLen, int& iConstantIndex, int& iConstantOffset)
{
	BOOL bResult = FALSE;

	// ��ʽ���ı�
	LPTSTR lpszConstantText = new _TCHAR[iLen+1];
	_tcsncpy_s(lpszConstantText, iLen+1, lpszText, iLen);
	lpszConstantText[iLen] = '\0';
	if (!m_bConstantsCaseSensitive)
	{
		if (m_bUper)
			_tcsupr_s(lpszConstantText, iLen+1);
		else
			_tcslwr_s(lpszConstantText, iLen+1);
	}

	// ��鳣��
	for (int i=0; i<m_iNumberConstants; i++)
	{
		// ��鳣��
		LPTSTR lpszSearch = _tcsstr(lpszConstantText, m_lpConstantInfo[i].lpszTagName);
		if (lpszSearch != NULL)
		{
			// �����Ч����
			int iFirst = (int)(lpszSearch - lpszConstantText - 1);
			int iLast = iFirst + m_lpConstantInfo[i].nLen;
			bResult = TRUE;
			if (iFirst >= 0)
			{
				if (((lpszConstantText[iFirst] >= 'A') && (lpszConstantText[iFirst] <= 'Z')) || 
					((lpszConstantText[iFirst] >= 'a') && (lpszConstantText[iFirst] <= 'z')))
					bResult = FALSE;

			}
			if (bResult)
			{
				if (((lpszConstantText[iLast] >= 'A') && (lpszConstantText[iLast] <= 'Z')) || 
					((lpszConstantText[iLast] >= 'a') && (lpszConstantText[iLast] <= 'z')))
					bResult = FALSE;
				else
				{
					// �˳�
					iConstantIndex = i;
					iConstantOffset = iFirst + 1;
					break;
				}
			}
		}
	}
	delete lpszConstantText;

	return bResult;
}

BOOL CCodeEdit::IsText(LPTSTR lpszText, int iLen, int& iTextStart, int& iTextEnd)
{
	BOOL bResult = FALSE;

	// ����ַ���
	iTextStart = -1;
	iTextEnd = -1;
	for (int i=0; i<iLen; i++)
	{
		// ����ַ�����ʼ���
		if ((iTextStart == -1) && ((lpszText[i] == '\"') || (lpszText[i] == '\'')))
		{
			iTextStart = i;
			bResult = TRUE;
		}
		else
		{
			// ����ַ����������
			if ((iTextStart != -1) && ((lpszText[i] == '\"') || (lpszText[i] == '\'')))
				iTextEnd = i;
		}
	}

	return bResult;
}

void CCodeEdit::ShowAutoComplete()
{
	if (m_wndListBox.m_hWnd != NULL)
	{
		// �����б��
		m_wndListBox.DestroyWindow();

		return;
	}

	// ��ù�������Ϣ
	int iVerticalOffset = GetScrollPos(SB_VERT);
	int iHorizontalOffset = GetScrollPos(SB_HORZ);

	// �����б��
	POINT pt = PositionFromChar(m_iCurrentLine, (m_iCurrentChar)==0 ? m_iCurrentChar : m_iCurrentChar-1);
	int iLine = m_iCurrentLine;
	int iChar = m_iCurrentChar;
	SelectWord(pt, TRUE);
	pt = PositionFromChar(m_iCurrentLine, m_iCurrentChar);
	pt.x -= iHorizontalOffset;
	pt.y -= iVerticalOffset;
	RECT rect = {pt.x, pt.y+m_szTextSize.cy, pt.x+LIST_HEIGHT, pt.y+m_szTextSize.cy+LIST_WIDTH};
	m_wndListBox.Create(WS_CHILD | WS_SIZEBOX | WS_VISIBLE | WS_HSCROLL | LBS_STANDARD | LBS_WANTKEYBOARDINPUT, rect, this, IDC_LIST_AUTOCOMPLETE);
	m_wndListBox.SetFont(&m_Font, TRUE);

	// ���ؼ��ʻ���
	int iEndOffset = GetCharOffset(iLine, iChar);
	if (!IsLetter(m_lpszText[iEndOffset]))
		iEndOffset--;
	int iStartOffset = iEndOffset;
	while ((iStartOffset > 0) && (IsLetter(m_lpszText[iStartOffset])))
		iStartOffset--;
	if (iStartOffset > 0)
		iStartOffset++;
	int iLen = iEndOffset - iStartOffset + 1;
	if (iLen > 0)
	{
		LPTSTR lpszSearch = new _TCHAR[iLen+1];
		_tcsncpy_s(lpszSearch, iLen+1, m_lpszText+iStartOffset, iLen);
		lpszSearch[iLen] = '\0';
		if (!m_bKeywordsCaseSensitive)
		{
			if (m_bUper)
				_tcsupr_s(lpszSearch, iLen+1);
			else
				_tcslwr_s(lpszSearch, iLen+1);
		}
		for (int i=0; i<m_iNumberKeywords; i++)
		{
			// �����Ч�ؼ���
			if (_tcsncmp(m_lpKeywordInfo[i].lpszTagName, lpszSearch, iLen) == 0)
			{
				// ��ӹؼ���
				m_wndListBox.AddString(m_lpKeywordInfo[i].lpszTagName);
			}
		}
		if (m_bConstantsCaseSensitive)
		{
			delete lpszSearch;
			lpszSearch = new _TCHAR[iLen+1];
			_tcsncpy_s(lpszSearch, iLen+1, m_lpszText+iStartOffset, iLen);
			lpszSearch[iLen] = '\0';
		}
		for (int j=0; j<m_iNumberConstants; j++)
		{
			// �����Ч����
			if (_tcsncmp(m_lpConstantInfo[j].lpszTagName, lpszSearch, iLen) == 0)
			{
				// ��ӳ���
				m_wndListBox.AddString(m_lpConstantInfo[j].lpszTagName);
			}
		}
		delete lpszSearch;
	}

	// ��ʾ�б��
	if (m_wndListBox.GetCount() > 0)
	{
		m_wndListBox.SetFocus();
		m_wndListBox.SetCurSel(0);
		// �༭�����ý����Խ��ռ��̲���
		((CCodeMateDlg*)GetParent())->m_bFocused = true;
	}
	else
	{
		m_wndListBox.DestroyWindow();
	}
}

void CCodeEdit::OnLbnDblclkListAutoComplete()
{
	// ���ѡ�йؼ���
	CString strKeyword;
	m_wndListBox.GetText(m_wndListBox.GetCurSel(), strKeyword);

	// �����б��
	m_wndListBox.DestroyWindow();

	// �뿪����ģʽ
	m_bCopying = FALSE;

	// ɾ��ѡ���ı�
	DeleteText();

	// �����ı�����
	int iLen = strKeyword.GetLength();

	// ����ı�
	AddText((LPTSTR)((LPCTSTR)strKeyword), m_iCurrentLine, m_iCurrentChar, iLen);

	// ���泷����Ϣ
	m_UndoInfo.iEndLine = m_iCurrentLine;
	m_UndoInfo.iEndChar = m_iCurrentChar + iLen;

	// ���¿ؼ�
	UpdateControl();

	// ���¹��λ��
	m_iCurrentChar += strKeyword.GetLength();
	UpdateCaret();
}

void CCodeEdit::SetKeywordColor(COLORREF cKeywordColor)
{
	m_crKeywordColor = cKeywordColor;
}

COLORREF CCodeEdit::GetKeywordColor()
{
	return m_crKeywordColor;
}

void CCodeEdit::SetKeywords(LPTSTR lpszKeywords, int iLen, BOOL bCaseSensitive, BOOL bUper)
{
	// ����ؼ���
	for (int i=0; i<m_iNumberKeywords; i++)
		delete m_lpKeywordInfo[i].lpszTagName;
	free(m_lpKeywordInfo);
	m_lpKeywordInfo = NULL;

	// ���ô�Сд���б�־
	m_bKeywordsCaseSensitive = bCaseSensitive;
	m_bUper = bUper;

	// �������
	m_iNumberKeywords = 0;

	// ��ӹؼ���
	AddKeywords(lpszKeywords, iLen);
}

void CCodeEdit::AddKeywords(LPTSTR lpszKeywords, int iLen)
{
	// ��ȡ�ؼ���
	int iOffset = 0;
	int iOldOffset = iOffset;
	int iCount;
	LPTSTR lpszNextKeyword = NULL;
	while ((lpszNextKeyword=_tcsstr(lpszKeywords+iOffset, _T(";"))) != NULL)
	{
		// ��ȡ�ؼ���
		iCount = (int)(lpszNextKeyword - (lpszKeywords+iOffset));
		iOffset = (int)(lpszNextKeyword - lpszKeywords);
		m_iNumberKeywords++;
		if (m_lpKeywordInfo == NULL)
			m_lpKeywordInfo = (LPTAGINFO)malloc(m_iNumberKeywords*sizeof(TAGINFO));
		else
			m_lpKeywordInfo = (LPTAGINFO)realloc(m_lpKeywordInfo, m_iNumberKeywords*sizeof(TAGINFO));
		m_lpKeywordInfo[m_iNumberKeywords-1].nLen = iCount + 1;
		m_lpKeywordInfo[m_iNumberKeywords-1].lpszTagName = new _TCHAR[m_lpKeywordInfo[m_iNumberKeywords-1].nLen];
		_tcsncpy_s(m_lpKeywordInfo[m_iNumberKeywords-1].lpszTagName, m_lpKeywordInfo[m_iNumberKeywords-1].nLen, lpszKeywords+iOldOffset, m_lpKeywordInfo[m_iNumberKeywords-1].nLen-1);
		m_lpKeywordInfo[m_iNumberKeywords-1].lpszTagName[m_lpKeywordInfo[m_iNumberKeywords-1].nLen-1] = '\0';
		if (!m_bKeywordsCaseSensitive)
		{
			if (m_bUper)
				_tcsupr_s(m_lpKeywordInfo[m_iNumberKeywords-1].lpszTagName, m_lpKeywordInfo[m_iNumberKeywords-1].nLen);
			else
				_tcslwr_s(m_lpKeywordInfo[m_iNumberKeywords-1].lpszTagName, m_lpKeywordInfo[m_iNumberKeywords-1].nLen);
		}
		iOffset++;
		iOldOffset = iOffset;
	}
}

void CCodeEdit::SetConstantColor(COLORREF crConstantColor)
{
	m_crConstantColor = crConstantColor;
}

COLORREF CCodeEdit::GetConstantColor()
{
	return m_crConstantColor;
}

void CCodeEdit::SetConstants(LPTSTR lpszConstants, int iLen, BOOL bCaseSensitive, BOOL bUper)
{
	// �������
	for (int i=0; i<m_iNumberConstants; i++)
		delete m_lpConstantInfo[i].lpszTagName;
	free(m_lpConstantInfo);
	m_lpConstantInfo = NULL;

	// ���ô�Сд���б�־
	m_bConstantsCaseSensitive = bCaseSensitive;
	m_bUper = bUper;

	// �������
	m_iNumberConstants = 0;

	// ��ӳ���
	AddConstants(lpszConstants, iLen);
}

void CCodeEdit::AddConstants(LPTSTR lpszConstants, int iLen)
{
	// ��ȡ����
	int iOffset = 0;
	int iOldOffset = iOffset;
	int iCount;
	LPTSTR lpszNextConstant = NULL;
	while ((lpszNextConstant=_tcsstr(lpszConstants+iOffset, _T(";"))) != NULL)
	{
		// ��ȡ����
		iCount = (int)(lpszNextConstant - (lpszConstants+iOffset));
		iOffset = (int)(lpszNextConstant - lpszConstants);
		m_iNumberConstants++;
		if (m_lpConstantInfo == NULL)
			m_lpConstantInfo = (LPTAGINFO)malloc(m_iNumberConstants*sizeof(TAGINFO));
		else
			m_lpConstantInfo = (LPTAGINFO)realloc(m_lpConstantInfo, m_iNumberConstants*sizeof(TAGINFO));
		m_lpConstantInfo[m_iNumberConstants-1].nLen = iCount + 1;
		m_lpConstantInfo[m_iNumberConstants-1].lpszTagName = new _TCHAR[m_lpConstantInfo[m_iNumberConstants-1].nLen];
		_tcsncpy_s(m_lpConstantInfo[m_iNumberConstants-1].lpszTagName, m_lpConstantInfo[m_iNumberConstants-1].nLen, lpszConstants+iOldOffset, m_lpConstantInfo[m_iNumberConstants-1].nLen-1);
		m_lpConstantInfo[m_iNumberConstants-1].lpszTagName[m_lpConstantInfo[m_iNumberConstants-1].nLen-1] = '\0';
		if (!m_bConstantsCaseSensitive)
		{
			if (m_bUper)
				_tcsupr_s(m_lpConstantInfo[m_iNumberConstants-1].lpszTagName, m_lpConstantInfo[m_iNumberConstants-1].nLen);
			else
				_tcslwr_s(m_lpConstantInfo[m_iNumberConstants-1].lpszTagName, m_lpConstantInfo[m_iNumberConstants-1].nLen);
		}
		iOffset++;
		iOldOffset = iOffset;
	}
}

void CCodeEdit::UpdateSyntaxColoring()
{
	// ������Ļ
	Invalidate(FALSE);
	UpdateWindow();
}

void CCodeEdit::SetCommentColor(COLORREF crCommentColor)
{
	m_crCommentColor = crCommentColor;
}

COLORREF CCodeEdit::GetCommentColor()
{
	return m_crCommentColor;
}

void CCodeEdit::SetTextColor(COLORREF crTextColor)
{
	m_crTextColor = crTextColor;
}

COLORREF CCodeEdit::GetTextColor()
{
	return m_crTextColor;
}

void CCodeEdit::SetNormalCodeColor(COLORREF crCodeColor)
{
	m_crCodeColor = crCodeColor;
}

COLORREF CCodeEdit::GetNormalCodeColor()
{
	return m_crCodeColor;
}

void CCodeEdit::SetColorStruct(COLORINFO coColor)
{
	m_crKeywordColor   = coColor.crKeywordColor;
	m_crConstantColor  = coColor.crConstantColor;
	m_crCommentColor   = coColor.crCommentColor;
	m_crTextColor      = coColor.crTextColor;
	m_crCodeColor	   = coColor.crForegroundColor;
	m_crBgColor		   = coColor.crBackgroundColor;
	m_crLineNumBgColor = coColor.crLineNumBGColor;
	m_crLineNumFgColor = coColor.crLineNumFGColor;
}

void CCodeEdit::GetColorStruct(COLORINFO &coColor)
{
	coColor.crKeywordColor    = m_crKeywordColor;
	coColor.crConstantColor	= m_crConstantColor;
	coColor.crCommentColor	= m_crCommentColor;
	coColor.crTextColor		= m_crTextColor;
	coColor.crForegroundColor = m_crCodeColor;
	coColor.crBackgroundColor = m_crBgColor;
	coColor.crLineNumBGColor	= m_crLineNumBgColor;
	coColor.crLineNumFGColor	= m_crLineNumFgColor;
}

void CCodeEdit::SetEditorFont(LPTSTR lpszFacename, LONG iHeight, BOOL bBold, BOOL bItalic, BYTE uiCharSet)
{
	m_Font.DeleteObject();
	m_Font.CreateFont(
		iHeight, // �߶�
		0, 0, 0,
		(bBold ? FW_BOLD :FW_NORMAL),
		bItalic, // ��б
		FALSE, // �»���
		FALSE, // ɾ����
		uiCharSet, // �ַ���
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_MODERN,
		lpszFacename); // ��������
}

void CCodeEdit::SetDefaultFileEncoding(FILE_ENCODING_TYPE fileEncoding)
{
	m_FileEncoding = fileEncoding;
}

void CCodeEdit::SetSyntaxColoring(SYNTAX_COLORING_TYPE syntaxColoring)
{
	// �����﷨��ɫ��Ϣ
	m_SyntaxColoring = syntaxColoring;

	// �����﷨��ɫ
	switch (m_SyntaxColoring)
	{
	case SCT_NONE:
		{
			SetKeywords(_T(""), 0, TRUE, FALSE);
			SetConstants(_T(""), 0, TRUE, FALSE);
			break;
		}

	case SCT_C_ANSI:
		{
			// ���� ANSI C �﷨��ɫ
			CString strKeywords = _T("auto;__asm;__based;break;case;__cdecl;char;const;continue;__declspec;default;");
			strKeywords += _T("dllexport;dllimport;do;double;__except;else;enum;extern;__fastcall;__finally;float;for;");
			strKeywords += _T("goto;__inline;__int8;__int16;__int32;__int64;if;int;__leave;long;naked;register;return;");
			strKeywords += _T("__stdcall;short;signed;sizeof;static;struct;switch;__try;thread;typedef;union;unsigned;");
			strKeywords += _T("void;volatile;while;once;pack;#define;#elif;#else;#endif;#error;");
			strKeywords += _T("#if;#ifdef;#ifndef;#import;#include;#line;#pragma;#undef;#using;");
			SetKeywords((LPTSTR)((LPCTSTR)strKeywords), strKeywords.GetLength(), TRUE, FALSE);
			SetConstants(_T(""), 0, TRUE, FALSE);
		}
		break;

	case SCT_C_PLUS_PLUS:
		{
			// ���� C++ �﷨��ɫ
			CString strKeywords = _T("__abstract;__alignof;__asm;__assume;__based;__box;__cdecl;__declspec;__delegate;");
			strKeywords += _T("__event;__except;__fastcall;__finally;__forceinline;__gc;__hook;__identifier;__if_exists;");
			strKeywords += _T("__if_not_exists;__inline;__int8;__int16;__int32;__int64;__interface;__leave;__m64;__m128;");
			strKeywords += _T("__m128d;__m128i;__multiple_inheritance;__nogc;__noop;__pin;__property;__raise;__sealed;");
			strKeywords += _T("__single_inheritance;__stdcall;__super;__try_cast;__try;__except;__unhook;__uuidof;");
			strKeywords += _T("__value;__virtual_inheritance;__w64;bool;break;case;catch;char;class;const;const_cast;continue;");
			strKeywords += _T("default;delete;deprecated;dllexport;dllimport;do;double;dynamic_cast;else;enum;explicit;extern;");
			strKeywords += _T("false;float;for;friend;goto;if;inline;int;long;mutable;naked;namespace;new;noinline;noreturn;");
			strKeywords += _T("nothrow;novtable;operator;private;property;protected;public;register;reinterpret_cast;return;");
			strKeywords += _T("selectany;short;signed;sizeof;static;static_cast;struct;switch;template;this;thread;throw;true;");
			strKeywords += _T("try;typedef;typeid;typename;union;unsigned;using;uuid;virtual;void;volatile;__wchar_t;wchar_t;while;");
			strKeywords += _T("auto;#define;#elif;#else;#endif;#error;#if;#ifdef;#ifndef;#import;#include;");
			strKeywords += _T("#line;#pragma;#undef;#using;auto_rename;auto_search;embedded_idl;exclude;once;pack;");
			SetKeywords((LPTSTR)((LPCTSTR)strKeywords), strKeywords.GetLength(), TRUE, FALSE);
			CString strConstants = _T("TRUE;FALSE;NULL;__LINE__;__FILE__;__DATE__;__TIME__;_cplusplus;__STDC__;");
			SetConstants((LPTSTR)((LPCTSTR)strConstants), strConstants.GetLength(), TRUE, FALSE);
		}
		break;

	case SCT_C_SHARP:
		{
			// ���� C# �﷨��ɫ
			CString strKeywords = _T("abstract;as;base;bool;break;byte;case;catch;char;checked;class;const;continue;decimal;default;");
			strKeywords += _T("delegate;do;double;else;enum;explicit;extern;False;finally;fixed;float;for;foreach;goto;if;implicit;");
			strKeywords += _T("in;int;interface;internal;is;lock;long;new;null;object;operator;out;override;params;private;protected;");
			strKeywords += _T("public;readonly;ref;return;sbyte;sealed;short;sizeof;stackalloc;static;string;volatile;while;true;false;");
			strKeywords += _T("struct;switch;this;throw;True;try;typeof;uint;ulong;unchecked;unsafe;ushort;using;virtual;void;");
			strKeywords += _T("add;dynamic;from;get;global;group;into;join;let;orderby;partial;remove;select;set;value;var;where;yield;");
			SetKeywords((LPTSTR)((LPCTSTR)strKeywords), strKeywords.GetLength(), TRUE, FALSE);
			CString strConstants = _T("TRUE;FALSE;NULL;");
			SetConstants((LPTSTR)((LPCTSTR)strConstants), strConstants.GetLength(), TRUE, FALSE);
		}
		break;

	case SCT_HTML:
		{
			// ���� HTML �﷨��ɫ
			CString strKeywords = _T("<a>;</a>;<a;<acronym>;</acronym>;<acronym;<address>;</address>;<address;<applet>;");
			strKeywords += _T("</applet>;<applet;<area>;</area>;<area;<b>;</b>;<b;<base>;</base>;<base;<baseFont>;</baseFont>;");
			strKeywords += _T("<baseFont;<bdo>;</bdo>;<bdo;<bgSound>;</bgSound>;<bgSound;<big>;</big>;<big;<blockQuote>;");
			strKeywords += _T("</blockQuote>;<blockQuote;<body>;</body>;<body;<br>;</br>;<br;<button>;</button>;<button;");
			strKeywords += _T("<caption>;</caption>;<caption;<center>;</center>;<center;<cite>;</cite>;<cite;<code>;</code>;<code;");
			strKeywords += _T("<col>;</col>;<col;<colGroup>;</colGroup>;<colGroup;<comment>;</comment>;<comment;<custom>;</custom>;<custom;");
			strKeywords += _T("<dd>;</dd>;<dd;<del>;</del>;<del;<dfn>;</dfn>;<dfn;<dir>;</dir>;<dir;<div>;</div>;<div;<dl>;</dl>;<dl;");
			strKeywords += _T("<dt>;</dt>;<dt;<em>;</em>;<em;<embed>;</embed>;<embed;<fieldSet>;</fieldSet>;<fieldSet;<font>;</font>;<font;");
			strKeywords += _T("<form>;</form>;<form;<frame>;</frame>;<frame;<frameSet>;</frameSet>;<frameSet;<head>;</head>;<head;");
			strKeywords += _T("<hn>;</hn>;<hn;<hr>;</hr>;<hr;<html>;</html>;<html;<i>;</i>;<i;<iframe>;</iframe>;<iframe;<img>;</img>;<img;");
			strKeywords += _T("<input>;</input>;<input;<ins>;</ins>;<ins;<isIndex>;</isIndex>;<isIndex;<kbd>;</kbd>;<kbd;<label>;</label>;<label;");
			strKeywords += _T("<legend>;</legend>;<legend;<li>;</li>;<li;<link>;</link>;<link;<listing>;</listing>;<listing;<map>;</map>;<map;");
			strKeywords += _T("<marquee>;</marquee>;<marquee;<menu>;</menu>;<menu;<meta>;</meta>;<meta;<noBR>;</noBR>;<noBR;");
			strKeywords += _T("<noFrames>;</noFrames>;<noFrames;<noScript>;</noScript>;<noScript;<object>;</object>;<object;<ol>;</ol>;<ol;");
			strKeywords += _T("<optGroup>;</optGroup>;<optGroup;<option>;</option>;<option;<p>;</p>;<p;<param>;</param>;<param;");
			strKeywords += _T("<plainText>;</plainText>;<plainText;<pre>;</pre>;<pre;<q>;</q>;<q;<rt>;</rt>;<rt;<ruby>;</ruby>;<ruby;");
			strKeywords += _T("<s>;</s>;<s;<samp>;</samp>;<samp;<script>;</script>;<script;<select>;</select>;<select;<small>;</small>;<small;");
			strKeywords += _T("<span>;</span>;<span;<strike>;</strike>;<strike;<strong>;</strong>;<strong;<style>;</style>;<style;");
			strKeywords += _T("<sub>;</sub>;<sub;<sup>;</sup>;<sup;<table>;</table>;<table;<tBody>;</tBody>;<tBody;<td>;</td>;<td;");
			strKeywords += _T("<textArea>;</textArea>;<textArea;<tFoot>;</tFoot>;<tFoot;<th>;</th>;<th;<tHead>;</tHead>;<tHead;");
			strKeywords += _T("<title>;</title>;<title;<tr>;</tr>;<tr;<tt>;</tt>;<tt;<u>;</u>;<u;<ul>;</ul>;<ul;<var>;</var>;<var;");
			strKeywords += _T("<wbr>;</wbr>;<wbr;<xml>;</xml>;<xml;<xmp>;</xmp>;<xmp;<h1>;</h1>;<h1;<h2>;</h2>;<h2;<h3>;</h3>;<h3;");
			strKeywords += _T("<h4>;</h4>;<h4;<h5>;</h5>;<h5;<h6></h6>;<h6;<;/>;>;");
			SetKeywords((LPTSTR)((LPCTSTR)strKeywords), strKeywords.GetLength(), FALSE, TRUE);
			SetConstants(_T(""), 0, TRUE, FALSE);
		}
		break;

	case SCT_JSCRIPT:
		{
			// ���� Java Script �﷨��ɫ
			CString strKeywords = _T("break;catch;@cc_on;continue;debugger;do;else;@elif;@else;@end;for;function;in;@if;if;");
			strKeywords += _T("new;return;switch;case;default;@set;this;throw;try;var;while;with;delete;finally;instanceof;typeof;");
			strKeywords += _T("abstract;boolean;byte;char;class;const;double;enum;export;extends;final;float;goto;implements;import;int;");
			strKeywords += _T("interface;long;native;package;private;protected;public;short;static;super;synchronized;throws;transient;volatile;");
			SetKeywords((LPTSTR)((LPCTSTR)strKeywords), strKeywords.GetLength(), FALSE, FALSE);
			CString strConstants = _T("true;false;null;");
			SetConstants((LPTSTR)((LPCTSTR)strConstants), strConstants.GetLength(), FALSE, FALSE);
		}
		break;

	case SCT_VBSCRIPT:
		{
			// ���� VB Script �﷨��ɫ
			CString strKeywords = _T("static;dim;public;private;const;option;explicit;do;else;for;function;sub;step;end;if;then;");
			strKeywords += _T("new;select;as;to;on;error;resume;while;with;goto;case;call;msgbox;next;loop;wend;each;");
			strKeywords += _T("variant;byte;boolean;integer;long;single;double;currency;date;object;string;is;not;");
			SetKeywords((LPTSTR)((LPCTSTR)strKeywords), strKeywords.GetLength(), FALSE, FALSE);
			CString strConstants = _T("true;false;null;empty;nothing;pi;scode;");
			SetConstants((LPTSTR)((LPCTSTR)strConstants), strConstants.GetLength(), FALSE, FALSE);
		}
		break;


	case SCT_SQL:
		{
			// ���� SQL �﷨��ɫ
			CString strKeywords = _T("@@IDENTITY;ENCRYPTION;ORDER;ADD;END;ALL;ERRLVL;OVER;ALTER;ESCAPE;PERCENT;");
			strKeywords += _T("AND;EXCEPT;PLAN;ANY;EXEC;PRECISION;AS;EXECUTE;PRIMARY;EXISTS;PRINT;AUTHORIZATION;");
			strKeywords += _T("EXIT;PROC;EXPRESSION;PROCEDURE;BACKUP;FETCH;PUBLIC;BEGIN;FILE;RAISERROR;BETWEEN;");
			strKeywords += _T("FILLFACTOR;READ;ASC;BREAK;FOR;READTEXT;BROWSE;FOREIGN;RECONFIGURE;BULK;FREETEXT;");
			strKeywords += _T("REFERENCES;BY;FREETEXTTABLE;REPLICATION;CASCADE;FROM;RESTORE;CASE;RESTRICT;CHECK;");
			strKeywords += _T("FUNCTION;RETURN;CHECKPOINT;GOTO;REVOKE;CLOSE;GRANT;CLUSTERED;GROUP;ROLLBACK;");
			strKeywords += _T("COALESCE;HAVING;ROWCOUNT;COLLATE;HOLDLOCK;ROWGUIDCOL;COLUMN;IDENTITY;RULE;COMMIT;IDENTITY_INSERT;");
			strKeywords += _T("SAVE;COMPUTE;IDENTITYCOL;SCHEMA;CONSTRAINT;IF;SELECT;CONTAINS;IN;SESSION_USER;CONTAINSTABLE;");
			strKeywords += _T("INDEX;SET;CONTINUE;SETUSER;INSERT;SHUTDOWN;INTERSECT;SOME;CREATE;INTO;");
			strKeywords += _T("STATISTICS;IS;CURRENT;JOIN;SYSTEM_USER;CURRENT_DATE;KEY;TABLE;CURRENT_TIME;KILL;");
			strKeywords += _T("TEXTSIZE;CURRENT_TIMESTAMP;THEN;CURRENT_USER;LIKE;TO;CURSOR;LINENO;TOP;DATABASE;LOAD;");
			strKeywords += _T("TRAN;DATABASEPASSWORD;TRANSACTION;DATEADD;TRIGGER;DATEDIFF;NATIONAL;TRUNCATE;DATENAME;");
			strKeywords += _T("NOCHECK;TSEQUAL;DATEPART;NONCLUSTERED;UNION;DBCC;NOT;UNIQUE;DEALLOCATE;UPDATE;DECLARE;NULLIF;");
			strKeywords += _T("UPDATETEXT;DEFAULT;OF;USE;DELETE;OFF;USER;DENY;OFFSETS;VALUES;DESC;ON;VARYING;DISK;OPEN;VIEW;");
			strKeywords += _T("DISTINCT;OPENDATASOURCE;WAITFOR;DISTRIBUTED;OPENQUERY;WHEN;DOUBLE;OPENROWSET;WHERE;DROP;OPENXML;");
			strKeywords += _T("WHILE;DUMP;OPTION;WITH;ELSE;OR;WRITETEXT;");
			SetKeywords((LPTSTR)((LPCTSTR)strKeywords), strKeywords.GetLength(), FALSE, TRUE);
			CString strConstants = _T("INNER;OUTER;CROSS;FULL;LEFT;RIGHT;TRUE;FALSE;NULL;COLUMNS;INDEXES;KEY_COLUMN_USAGE;DECIMAL;");
			strConstants += _T("PROVIDER_TYPES;TABLES;TABLE_CONSTRAINTS;BIGINT;INTEGER;SMALLINT;TINYINT;BIT;NUMERIC;MONEY;CHAR;VARCHAR;");
			strConstants += _T("FLOAT;REAL;DATETIME;NCHAR;NVARCHAR;NTEXT;BINARY;VARBINARY;IMAGE;UNIQUEIDENTIFIER;IDENTITY;ROWGUIDCOL;");
			strConstants += _T("AVG;MAX;MIN;SUM;COUNT;DATEADD;DATEDIFF;DATENAME;DATEPART;GETDATE;ABS;ACOS;ASIN;ATAN;ATN2;CEILING;COS;");
			strConstants += _T("COT;DEGREES;EXP;FLOOR;LOG;LOG10;PI;POWER;RADIANS;RAND;ROUND;SIGN;SIN;SQRT;TAN;NCHAR;CHARINDEX;LEN;");
			strConstants += _T("LOWER;LTRIM;PATINDEX;REPLACE;REPLICATE;RTRIM;SPACE;STR;STUFF;SUBSTRING;UNICODE;UPPER;CONVERT;");
			SetConstants((LPTSTR)((LPCTSTR)strConstants), strConstants.GetLength(), FALSE, TRUE);
		}
		break;
	}
}

SYNTAX_COLORING_TYPE CCodeEdit::GetSyntaxColoring()
{
	return m_SyntaxColoring;
}

void CCodeEdit::SetCurrentLine(int index)
{
	m_iCurrentLine = max(0, min(m_iNumberLines-1, index));

	// ȡ��ѡ��
	m_iLineStartSelection = -1;
	m_iLineEndSelection = -1;
	m_iCharStartSelection = -1;
	m_iCharEndSelection = -1;
}

int CCodeEdit::GetCurrentLine()
{
	return m_iCurrentLine;
}

void CCodeEdit::SetCurrentChar(int index)
{
	m_iCurrentChar = max(0, min(m_lpLineInfo[m_iCurrentLine].nLen, index));

	// ȡ��ѡ��
	m_iLineStartSelection = -1;
	m_iLineEndSelection = -1;
	m_iCharStartSelection = -1;
	m_iCharEndSelection = -1;
}

int CCodeEdit::GetCurrentChar()
{
	return m_iCurrentChar;
}

int CCodeEdit::GetCurrentColumn()
{
	return GetColumnIndex(m_iCurrentLine, m_iCurrentChar);
}

void CCodeEdit::SetTABs(int iTABs)
{
	m_iTABs = max(1, min(8, iTABs));
}

int CCodeEdit::GetTABs()
{
	return m_iTABs;
}

void CCodeEdit::EnsureVisible()
{
	UpdateCaret();
}

void CCodeEdit::SetSelection(int iStartLine, int iStartChar, int iEndLine, int iEndChar)
{
	// ����ѡ��
	m_iLineStartSelection = max(0, iStartLine);
	m_iCharStartSelection = max(0, iStartChar);
	m_iLineEndSelection = min(m_iNumberLines-1, iEndLine);
	if (m_iLineEndSelection == -1)
		m_iLineEndSelection = m_iNumberLines - 1;
	m_iCharEndSelection = min(m_lpLineInfo[m_iLineEndSelection].nLen, iEndChar);
	if (m_iCharEndSelection == -1)
		m_iCharEndSelection = m_lpLineInfo[m_iLineEndSelection].nLen;
}

void CCodeEdit::GetSelection(int& iStartLine, int& iStartChar, int& iEndLine, int& iEndChar)
{
	// ��ȡѡ��
	iStartLine = m_iLineStartSelection;
	iStartChar = m_iCharStartSelection;
	iEndLine = m_iLineEndSelection;
	iEndChar = m_iCharEndSelection;
}

int CCodeEdit::GetNumberLines()
{
	return m_iNumberLines;
}

int CCodeEdit::GetLineLen(int LineIndex)
{
	if (LineIndex >= m_iNumberLines || LineIndex < 0)
		return -1;
	else
		return m_lpLineInfo[LineIndex].nLen;
}

void CCodeEdit::SetBackgroundColor(COLORREF cBgColor)
{
	m_crBgColor = cBgColor;
}

COLORREF CCodeEdit::GetBackgroundColor()
{
	return m_crBgColor;
}

void CCodeEdit::SetLineNumberBackgroundColor(COLORREF cBgColor)
{
	m_crLineNumBgColor = cBgColor;
}

COLORREF CCodeEdit::GetLineNumberBackgroundColor()
{
	return m_crLineNumBgColor;
}

void CCodeEdit::SetLineNumberForegroundColor(COLORREF cBgColor)
{
	m_crLineNumFgColor = cBgColor;
}

COLORREF CCodeEdit::GetLineNumberForegroundColor()
{
	return m_crLineNumFgColor;
}

void CCodeEdit::SetShowLineNumberStatus(BOOL isShowed)
{
	m_LineNumbers = isShowed;
}

void CCodeEdit::SetLineDrawProc(LPLINEDRAWPROC lpfnLineDrawProc)
{
	m_lpfnLineDrawProc = lpfnLineDrawProc;
}

void CCodeEdit::DefaultLineDrawProc(LPTSTR lpszText, int iLen, RECT rect, int iSelectionStart, int iSelectionEnd)
{
	// ��Ĭ�Ϸ�ʽ������
	DrawLine(lpszText, iLen, rect, iSelectionStart, iSelectionEnd);
}

BOOL CCodeEdit::OnEraseBkgnd(CDC* pDC)
{
	return FALSE;
}

void CCodeEdit::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	// ��ÿͻ��˴�С��λ��
	RECT rcClient;
	GetClientRect(&rcClient);

	// �����Ա
	if (m_MemDC.m_hDC != NULL)
	{
		m_MemDC.SelectObject(m_pOldMemBitmap);
		m_MemDC.SelectObject(m_pOldFont);
		m_MemDC.DeleteDC();
		m_MemBitmap.DeleteObject();
	}

	// �����ڴ� DC ��λͼ
	CDC* pDC = GetDC();
	m_MemDC.CreateCompatibleDC(pDC);
	m_MemBitmap.CreateCompatibleBitmap(pDC, rcClient.right-rcClient.left, rcClient.bottom-rcClient.top);
	m_pOldMemBitmap = m_MemDC.SelectObject(&m_MemBitmap);
	m_pOldFont = m_MemDC.SelectObject(&m_Font);
	ReleaseDC(pDC);

	// �������
	DestroyCaret();
	CSize sz = m_MemDC.GetTextExtent(_T("A"), 1);
	::CreateCaret(m_hWnd, NULL, 1, sz.cy);
	CPoint pt = GetCaretPos();
	SetCaretPos(pt);
	if (((CCodeMateDlg*)GetParent())->m_bFocused == true)
	{
		ShowCaret();
	}

	// ���¿ؼ�
	UpdateControl();

	// ������Ļ
	Invalidate(FALSE);
	UpdateWindow();
}

void CCodeEdit::OnPaint()
{
	CPaintDC dc(this);

	// �������
	RECT rcClient;
	GetClientRect(&rcClient);
	m_MemDC.FillSolidRect(&rcClient, m_crBgColor);

	// ��������
	DrawContent();

	// ���Ƶ���Ļ DC
	dc.BitBlt(0, 0, rcClient.right-rcClient.left, rcClient.bottom-rcClient.top, &m_MemDC, 0, 0, SRCCOPY);
}

void CCodeEdit::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// �����б��
	DestroyList();

	// ��ù�������Ϣ
	SCROLLINFO si;
	memset(&si, 0, sizeof(SCROLLINFO));
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_ALL;
	GetScrollInfo(SB_VERT, &si);

	// ����ˮƽ����
	switch (nSBCode)
	{
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		{
			// ���¹���λ��
			si.nPos = (si.nTrackPos / m_szTextSize.cy) * m_szTextSize.cy;
		}
		break;

	case SB_LINEDOWN:
		{
			// ���¹���λ��
			si.nPos += m_szTextSize.cy;
			if (si.nPos > (int)(si.nMax-si.nPage))
				si.nPos -= m_szTextSize.cy;
		}
		break;

	case SB_PAGEDOWN:
		{
			// ��ÿͻ����Ĵ�С��λ��
			RECT rcClient;
			GetClientRect(&rcClient);

			// ���ҳ��߶�
			int iHeight = rcClient.bottom - rcClient.top;

			// ���¹��λ�ú͹���λ��
			m_iCurrentLine += int(iHeight / m_szTextSize.cy);
			if (m_iCurrentLine > m_iNumberLines - 1)
				m_iCurrentLine = m_iNumberLines - 1;
			if (m_iCurrentChar > m_lpLineInfo[m_iCurrentLine].nLen)
				m_iCurrentChar = m_lpLineInfo[m_iCurrentLine].nLen;
			si.nPos += iHeight;
			if (si.nPos > (int)(si.nMax-si.nPage))
				si.nPos = (int)(si.nMax-si.nPage);
		}
		break;

	case SB_LINEUP:
		{
			// ���¹���λ��
			si.nPos -= m_szTextSize.cy;
			if (si.nPos < si.nMin)
				si.nPos = si.nMin;
		}
		break;

	case SB_PAGEUP:
		{
			// ��ÿͻ����Ĵ�С��λ��
			RECT rcClient;
			GetClientRect(&rcClient);

			// ���ҳ��߶�
			int iHeight = rcClient.bottom - rcClient.top;

			// ���¹��λ�ú͹���λ��
			m_iCurrentLine -= int(iHeight / m_szTextSize.cy);
			if (m_iCurrentLine < 0)
				m_iCurrentLine = 0;
			if (m_iCurrentChar > m_lpLineInfo[m_iCurrentLine].nLen)
				m_iCurrentChar = m_lpLineInfo[m_iCurrentLine].nLen;
			si.nPos -= iHeight;
			if (si.nPos < si.nMin)
				si.nPos = si.nMin;
		}
		break;
	}

	// ���ù�������Ϣ
	SetScrollInfo(SB_VERT, &si, TRUE);

	// ��ù�������Ϣ
	int iVerticalOffset = GetScrollPos(SB_VERT);
	int iHorizontalOffset = GetScrollPos(SB_HORZ);

	// ����ַ�λ��
	POINT pt = PositionFromChar(m_iCurrentLine, m_iCurrentChar);
	pt.x -= iHorizontalOffset;
	pt.y -= iVerticalOffset;
	SetCaretPos(pt);

	// ������Ļ
	Invalidate(FALSE);
	UpdateWindow();

	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CCodeEdit::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// �����б��
	DestroyList();

	// ��ȡ��������Ϣ
	SCROLLINFO si;
	memset(&si, 0, sizeof(SCROLLINFO));
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_ALL;
	GetScrollInfo(SB_HORZ, &si);

	// ���´�ֱ����
	switch (nSBCode)
	{
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		{
			// ���¹���λ��
			si.nPos = (si.nTrackPos / m_szTextSize.cx) * m_szTextSize.cx;
		}
		break;

	case SB_LINERIGHT:
	case SB_PAGERIGHT:
		{
			// ���¹���λ��
			si.nPos += m_szTextSize.cx;
			if (si.nPos > (int)(si.nMax-si.nPage))
				si.nPos -= m_szTextSize.cx;
		}
		break;

	case SB_LINELEFT:
	case SB_PAGELEFT:
		{
			// ���¹���λ��
			si.nPos -= m_szTextSize.cx;
			if (si.nPos < si.nMin)
				si.nPos = si.nMin;
		}
		break;
	}

	// ���ù�������Ϣ
	SetScrollInfo(SB_HORZ, &si, TRUE);

	// ��ù�������Ϣ
	int iVerticalOffset = GetScrollPos(SB_VERT);
	int iHorizontalOffset = GetScrollPos(SB_HORZ);

	// ����ַ�λ��
	POINT pt = PositionFromChar(m_iCurrentLine, m_iCurrentChar);
	pt.x -= iHorizontalOffset;
	pt.y -= iVerticalOffset;
	SetCaretPos(pt);

	// ������Ļ
	Invalidate(FALSE);
	UpdateWindow();

	CWnd::OnHScroll(nSBCode, nPos, pScrollBar);
}

BOOL CCodeEdit::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// ����б���Ƿ�ɼ�
	if (m_wndListBox.m_hWnd != NULL)
	{
		// ������Ϣ���б��
		if (zDelta<0)
			m_wndListBox.SendMessage(WM_VSCROLL,SB_LINEDOWN);
		else
			m_wndListBox.SendMessage(WM_VSCROLL,SB_LINEUP);
		return FALSE;
	}

	// ������������Ϣ
	if (zDelta < 0)
	{
		// ���¹���
		OnVScroll(SB_LINEDOWN, 0, 0);
	}
	else
	{
		// ���Ϲ���
		OnVScroll(SB_LINEUP, 0, 0);
	}

	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}

void CCodeEdit::OnLButtonDown(UINT nFlags, CPoint point)
{
	// �����Ч����
	if (m_lpszText != NULL)
	{
		// �����б��
		DestroyList();

		// ��׽���
		SetCapture();

		// ��ù�����Ϣ
		int iVerticalOffset = GetScrollPos(SB_VERT);
		int iHorizontalOffset = GetScrollPos(SB_HORZ);

		// ������λ��
		POINT pt = {point.x+iHorizontalOffset, point.y+iVerticalOffset};

		// ����Ƿ���ʾ�к�
		if (m_LineNumbers)
		{
			// ������λ���к���ʾ��Χ
			if (int(point.x) < 5 * m_szTextSize.cx - iHorizontalOffset)
			{
				// ����ѡ����Χ
				m_iLineStartSelection = LineFromPosition(pt);
				m_iLineEndSelection = (m_iLineStartSelection ==  m_iNumberLines - 1) ? m_iNumberLines - 1 : m_iLineStartSelection + 1;
				m_iCharStartSelection = 0;
				m_iCharEndSelection = (m_iLineStartSelection ==  m_iNumberLines - 1) ? m_lpLineInfo[m_iNumberLines - 1].nLen : 0;

				// ���ù��λ��
				m_iCurrentLine = m_iLineEndSelection;
				m_iCurrentChar = m_iCharEndSelection;

				POINT ptCaret = PositionFromChar(m_iCurrentLine, m_iCurrentChar);
				ptCaret.x -= iHorizontalOffset;
				ptCaret.y -= iVerticalOffset;
				SetCaretPos(ptCaret);

				// �༭����ý���
				SetFocus();

				// ������Ļ
				Invalidate(FALSE);
				UpdateWindow();

				// �˳�
				return;
			}
		}

		// ����Ƿ�����ѡ��
		if (!m_bSelecting)
		{
			// ����ѡ����Χ
			m_iLineStartSelection = LineFromPosition(pt);
			m_iLineEndSelection = m_iLineStartSelection;
			m_iCharStartSelection = CharFromPosition(m_iLineStartSelection, pt);
			m_iCharEndSelection = m_iCharStartSelection;

			// ���ù��λ��
			m_iCurrentLine = m_iLineStartSelection;
			m_iCurrentChar = m_iCharStartSelection;
			POINT ptCaret = PositionFromChar(m_iCurrentLine, m_iCurrentChar);
			ptCaret.x -= iHorizontalOffset;
			ptCaret.y -= iVerticalOffset;
			SetCaretPos(ptCaret);
		}
		else
		{
			// ����ѡ������λ��
			m_iLineEndSelection = LineFromPosition(pt);
			m_iCharEndSelection = CharFromPosition(m_iLineEndSelection, pt);

			// ���ù��λ��
			m_iCurrentLine = m_iLineEndSelection;
			m_iCurrentChar = m_iCharEndSelection;
			POINT ptCaret = PositionFromChar(m_iCurrentLine, m_iCurrentChar);
			ptCaret.x -= iHorizontalOffset;
			ptCaret.y -= iVerticalOffset;
			SetCaretPos(ptCaret);
		}

		// �༭����ý���
		SetFocus();

		// ������Ļ
		Invalidate(FALSE);
		UpdateWindow();
	}

	CWnd::OnLButtonDown(nFlags, point);
}

void CCodeEdit::OnRButtonDown(UINT nFlags, CPoint point)
{
	// ����Ƿ���ʾ�к�
	if (m_LineNumbers)
	{
		// ������λ���к���ʾ��Χ
		int iHorizontalOffset = GetScrollPos(SB_HORZ);
		if (int(point.x) < 5 * m_szTextSize.cx - iHorizontalOffset) return;
	}

	// �����Ч����
	if (m_lpszText != NULL)
	{
		// �����б��
		DestroyList();

		// ��׽���
		SetCapture();

		// ��ù�������Ϣ
		int iVerticalOffset = GetScrollPos(SB_VERT);
		int iHorizontalOffset = GetScrollPos(SB_HORZ);

		// ������λ��
		POINT pt = {point.x+iHorizontalOffset, point.y+iVerticalOffset};

		if (m_iLineStartSelection==m_iLineEndSelection && m_iCharStartSelection==m_iCharEndSelection)
		{
			// ����ѡȡ��ʼλ��
			m_iLineStartSelection = LineFromPosition(pt);
			m_iLineEndSelection = m_iLineStartSelection;
			m_iCharStartSelection = CharFromPosition(m_iLineStartSelection, pt);
			m_iCharEndSelection = m_iCharStartSelection;

			// ���ù��λ��
			m_iCurrentLine = m_iLineStartSelection;
			m_iCurrentChar = m_iCharStartSelection;
			POINT ptCaret = PositionFromChar(m_iCurrentLine, m_iCurrentChar);
			ptCaret.x -= iHorizontalOffset;
			ptCaret.y -= iVerticalOffset;
			SetCaretPos(ptCaret);
		}

		// �༭����ý���
		SetFocus();

		// ������Ļ
		Invalidate(FALSE);
		UpdateWindow();
	}

	// �����Ҽ��˵�
	CMenu menu,*pSubMenu;
	menu.LoadMenu(IDR_MENU_RBUTTON);
	pSubMenu = menu.GetSubMenu(1);

	// ���ò˵�ͼ��
	if (((CCodeMateDlg*)GetParent())->m_bShowMenuImage == true)
	{
		pSubMenu ->SetMenuItemBitmaps (ID_EDIT_UNDO, MF_BYCOMMAND, &(((CCodeMateDlg*)GetParent())->m_bmpMenu[3]), &(((CCodeMateDlg*)GetParent())->m_bmpMenu[3]));
		pSubMenu ->SetMenuItemBitmaps (ID_EDIT_CUT, MF_BYCOMMAND, &(((CCodeMateDlg*)GetParent())->m_bmpMenu[4]), &(((CCodeMateDlg*)GetParent())->m_bmpMenu[4]));
		pSubMenu ->SetMenuItemBitmaps (ID_EDIT_COPY, MF_BYCOMMAND, &(((CCodeMateDlg*)GetParent())->m_bmpMenu[5]), &(((CCodeMateDlg*)GetParent())->m_bmpMenu[5]));
		pSubMenu ->SetMenuItemBitmaps (ID_EDIT_PASTE, MF_BYCOMMAND, &(((CCodeMateDlg*)GetParent())->m_bmpMenu[6]), &(((CCodeMateDlg*)GetParent())->m_bmpMenu[6]));
	}

	// ���ò˵�����
	if (((CCodeMateDlg*)GetParent())->m_sMyConfig.m_uiLanguage != 0)
	{
		pSubMenu->ModifyMenu(ID_EDIT_UNDO, MF_BYCOMMAND, ID_EDIT_UNDO, ((CCodeMateDlg*)GetParent())->m_arrLanString[21]);
		pSubMenu->ModifyMenu(ID_EDIT_CUT, MF_BYCOMMAND, ID_EDIT_CUT, ((CCodeMateDlg*)GetParent())->m_arrLanString[22]);
		pSubMenu->ModifyMenu(ID_EDIT_COPY, MF_BYCOMMAND, ID_EDIT_COPY, ((CCodeMateDlg*)GetParent())->m_arrLanString[23]);
		pSubMenu->ModifyMenu(ID_EDIT_PASTE, MF_BYCOMMAND, ID_EDIT_PASTE, ((CCodeMateDlg*)GetParent())->m_arrLanString[24]);
		pSubMenu->ModifyMenu(ID_EDIT_CLEAR, MF_BYCOMMAND, ID_EDIT_CLEAR, ((CCodeMateDlg*)GetParent())->m_arrLanString[25]);
		pSubMenu->ModifyMenu(ID_EDIT_SELALL, MF_BYCOMMAND, ID_EDIT_SELALL, ((CCodeMateDlg*)GetParent())->m_arrLanString[26]);
		pSubMenu->ModifyMenu(ID_A_LINE, MF_BYCOMMAND, ID_A_LINE, ((CCodeMateDlg*)GetParent())->m_arrLanString[72]);
		pSubMenu->ModifyMenu(ID_DEL_A_LINE, MF_BYCOMMAND, ID_DEL_A_LINE, ((CCodeMateDlg*)GetParent())->m_arrLanString[74]);
		pSubMenu->ModifyMenu(ID_EDIT_SHOWLIST, MF_BYCOMMAND, ID_EDIT_SHOWLIST, ((CCodeMateDlg*)GetParent())->m_arrLanString[27]);
	}

	// ��ʾ�˵�
	CPoint oPoint;
	GetCursorPos(&oPoint);
	pSubMenu->TrackPopupMenu(TPM_LEFTALIGN,oPoint.x,oPoint.y,((CCodeMateDlg*)GetParent()));
	pSubMenu=NULL;
	delete pSubMenu;

	CWnd::OnRButtonDown(nFlags, point);
}

void CCodeEdit::OnMouseMove(UINT nFlags, CPoint point)
{
	// �����Ч����
	if (m_lpszText != NULL)
	{
		// ��ù�������Ϣ
		int iVerticalOffset = GetScrollPos(SB_VERT);
		int iHorizontalOffset = GetScrollPos(SB_HORZ);

		// ����Ƿ���ʾ�к�
		if (m_LineNumbers)
		{
			// ������λ���к���ʾ��Χ
			if (int(point.x) < 5 * m_szTextSize.cx - iHorizontalOffset)
			{
				HCURSOR hCursor = (HCURSOR)AfxGetApp()->LoadCursor(IDC_POINTER);
				SetCursor(hCursor);
			}
		}

		// �������϶�
		if ((nFlags & MK_LBUTTON) && (m_iLineStartSelection != -1) && (m_iCharStartSelection != -1))
		{
			// ������λ��
			POINT pt = {max(0, point.x+iHorizontalOffset), max(0, point.y+iVerticalOffset)};

			// ����ѡ������λ��
			m_iLineEndSelection = LineFromPosition(pt);
			m_iCharEndSelection = CharFromPosition(m_iLineEndSelection, pt);

			// ���ù��λ��
			m_iCurrentLine = m_iLineEndSelection;
			m_iCurrentChar = m_iCharEndSelection;

			POINT ptCaret = PositionFromChar(m_iCurrentLine, m_iCurrentChar);
			ptCaret.x -= iHorizontalOffset;
			ptCaret.y -= iVerticalOffset;
			SetCaretPos(ptCaret);

			// ȷ�������ʾ
			EnsureVisible(m_iCurrentLine, m_iCurrentChar);

			// ������Ļ
			Invalidate(FALSE);
			UpdateWindow();
		}
	}

	CString sText;
	if (((CCodeMateDlg*)GetParent())->m_sMyConfig.m_uiLanguage == 0)
		sText.LoadString(AFX_IDS_IDLEMESSAGE);
	else
		sText = ((CCodeMateDlg*)GetParent())->m_arrLanString[53];
	((CCodeMateDlg*)GetParent())->m_wndStatusBar.SetPaneText(0, sText);

	CWnd::OnMouseMove(nFlags, point);
}

void CCodeEdit::OnLButtonUp(UINT nFlags, CPoint point)
{
	// �����Ч����
	if (m_lpszText != NULL)
	{
		// �ͷ����
		ReleaseCapture();

		// �����Чѡ��
		if ((m_iLineStartSelection == m_iLineEndSelection) && (m_iCharStartSelection == m_iCharEndSelection))
		{
			// ���ѡ��
			m_iLineStartSelection = -1;
			m_iLineEndSelection = -1;
			m_iCharStartSelection = -1;
			m_iCharEndSelection = -1;
		}

		// ������Ļ
		Invalidate(FALSE);
		UpdateWindow();
	}

	CWnd::OnLButtonUp(nFlags, point);
}

void CCodeEdit::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// �����Ч����
	if (m_lpszText != NULL)
	{
		// ��ù�������Ϣ
		int iVerticalOffset = GetScrollPos(SB_VERT);
		int iHorizontalOffset = GetScrollPos(SB_HORZ);

		// ������λ��
		POINT pt = {point.x+iHorizontalOffset, point.y+iVerticalOffset};

		// ѡ�񵥴�
		SelectWord(pt, FALSE);

		// ������Ļ
		Invalidate(FALSE);
		UpdateWindow();
	}
	CWnd::OnLButtonDblClk(nFlags, point);
}

void CCodeEdit::OnSetFocus(CWnd* pOldWnd)
{
	CWnd::OnSetFocus(pOldWnd);
	DestroyCaret();
	CSize sz = m_MemDC.GetTextExtent(_T("A"), 1);
	::CreateCaret(m_hWnd, NULL, 1, sz.cy);

	// �������
	CPoint pt = GetCaretPos();
	SetCaretPos(pt);

	// ���¹��
	UpdateCaret();

	// ��ʾ���
	ShowCaret();
	((CCodeMateDlg*)GetParent())->m_bFocused=true;
}

void CCodeEdit::OnKillFocus(CWnd* pOldWnd)
{
	CWnd::OnKillFocus(pOldWnd);
	HideCaret();
	((CCodeMateDlg*)GetParent())->m_bFocused=false;
}

void CCodeEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// �����Ч����
	if (m_lpszText != NULL)
	{
		// �����������
		BOOL bUpdate = FALSE;
		switch (nChar)
		{
		case VK_LEFT:	// �����
			{
				// ����б���Ƿ�ɼ�
				if (m_wndListBox.m_hWnd != NULL)
				{
					// ������Ϣ���б��
					m_wndListBox.SendMessage(WM_KEYDOWN, (WPARAM)nChar, 0);

					// ����ģʽ
					bUpdate = TRUE;

					return;
				}
				else
				{
					// ����������ƶ�
					if (m_iCurrentChar > 0)
						m_iCurrentChar--;
					else if (m_iCurrentLine > 0)
					{
						m_iCurrentLine--;
						m_iCurrentChar = m_lpLineInfo[m_iCurrentLine].nLen;
					}

					// ����Ƿ�����ѡ��
					if (m_bSelecting)
					{
						// ����ѡ������λ��
						m_iLineEndSelection = m_iCurrentLine;
						m_iCharEndSelection = m_iCurrentChar;
					}
				}
			}
			break;

		case VK_RIGHT:	// �ҷ����
			{
				// ����б���Ƿ�ɼ�
				if (m_wndListBox.m_hWnd != NULL)
				{
					// ������Ϣ���б��
					m_wndListBox.SendMessage(WM_KEYDOWN, (WPARAM)nChar, 0);

					// ����ģʽ
					bUpdate = TRUE;

					return;
				}
				else
				{
					// ����������ƶ�
					if (m_iCurrentChar < m_lpLineInfo[m_iCurrentLine].nLen)
						m_iCurrentChar++;
					else if (m_iCurrentLine < m_iNumberLines-1)
					{
						m_iCurrentLine++;
						m_iCurrentChar = 0;
					}

					// ����Ƿ�����ѡ��
					if (m_bSelecting)
					{
						// ����ѡ������λ��
						m_iLineEndSelection = m_iCurrentLine;
						m_iCharEndSelection = m_iCurrentChar;
					}
				}
			}
			break;

		case VK_UP:	// �Ϸ����
			{
				// ����б���Ƿ�ɼ�
				if (m_wndListBox.m_hWnd != NULL)
				{
					// ������Ϣ���б��
					m_wndListBox.SendMessage(WM_KEYDOWN, (WPARAM)nChar, 0);

					// ����ģʽ
					bUpdate = TRUE;

					return;
				}
				else
				{
					// ����������ƶ�
					if (m_iCurrentLine > 0)
					{
						int cnum = GetColumnIndex(m_iCurrentLine, m_iCurrentChar);
						m_iCurrentChar = GetCharIndex(m_iCurrentLine - 1, cnum);
						m_iCurrentLine--;
						if (m_iCurrentChar > m_lpLineInfo[m_iCurrentLine].nLen)
							m_iCurrentChar = m_lpLineInfo[m_iCurrentLine].nLen;
					}
					else
						m_iCurrentChar = 0;

					// ����Ƿ�����ѡ��
					if (m_bSelecting)
					{
						// ����ѡ������λ��
						m_iLineEndSelection = m_iCurrentLine;
						m_iCharEndSelection = m_iCurrentChar;
					}
				}
			}
			break;

		case VK_DOWN:	// �·����
			{
				// ����б���Ƿ�ɼ�
				if (m_wndListBox.m_hWnd != NULL)
				{
					// ������Ϣ���б��
					m_wndListBox.SendMessage(WM_KEYDOWN, (WPARAM)nChar, 0);

					// ����ģʽ
					bUpdate = TRUE;

					return;
				}
				else
				{
					// ����������ƶ�
					if (m_iCurrentLine < m_iNumberLines-1)
					{
						int cnum = GetColumnIndex(m_iCurrentLine, m_iCurrentChar);
						m_iCurrentChar = GetCharIndex(m_iCurrentLine + 1, cnum);
						m_iCurrentLine++;
						if (m_iCurrentChar > m_lpLineInfo[m_iCurrentLine].nLen)
							m_iCurrentChar = m_lpLineInfo[m_iCurrentLine].nLen;
					}
					else
						m_iCurrentChar = m_lpLineInfo[m_iCurrentLine].nLen;

					// ����Ƿ�����ѡ��
					if (m_bSelecting)
					{
						// ����ѡ������λ��
						m_iLineEndSelection = m_iCurrentLine;
						m_iCharEndSelection = m_iCurrentChar;
					}
				}
			}
			break;

		case VK_HOME:	// Home ��
			{
				// ����б���Ƿ�ɼ�
				if (m_wndListBox.m_hWnd != NULL)
				{
					// ������Ϣ���б��
					m_wndListBox.SendMessage(WM_KEYDOWN, (WPARAM)nChar, 0);
					return;
				}
				else
				{
					if (m_bCopying == TRUE)
					{
						// ������ƶ�����һ��
						m_iCurrentLine = 0;
					}

					// ������ƶ��������
					m_iCurrentChar = 0;

					// �����Чѡ��
					if (m_bSelecting)
					{
						// ����ѡ������λ��
						m_iLineEndSelection = m_iCurrentLine;
						m_iCharEndSelection = m_iCurrentChar;
					}
				}
			}
			break;

		case VK_END:	// End ��
			{
				// ����б���Ƿ�ɼ�
				if (m_wndListBox.m_hWnd != NULL)
				{
					// ������Ϣ���б��
					m_wndListBox.SendMessage(WM_KEYDOWN, (WPARAM)nChar, 0);
					return;
				}
				else
				{
					if (m_bCopying == TRUE)
					{
						// ������ƶ������һ��
						m_iCurrentLine = m_iNumberLines - 1;
					}

					// ������ƶ������ұ�
					m_iCurrentChar = m_lpLineInfo[m_iCurrentLine].nLen;

					// �����Чѡ��
					if (m_bSelecting)
					{
						// ����ѡ������λ��
						m_iLineEndSelection = m_iCurrentLine;
						m_iCharEndSelection = m_iCurrentChar;
					}
				}
			}
			break;

		case VK_SHIFT:	// Shift ��
			{
				// ����б���Ƿ�ɼ�
				if (m_wndListBox.m_hWnd != NULL)
				{
					// ������Ϣ���б��
					m_wndListBox.SendMessage(WM_KEYDOWN, (WPARAM)nChar, 0);
					return;
				}
				else
				{
					// �����Чѡ��
					if (!m_bSelecting)
					{
						// ѡ��ģʽ
						m_bSelecting = TRUE;

						// ����ѡ����ʼ�����λ��
						m_iLineStartSelection = m_iCurrentLine;
						m_iCharStartSelection = m_iCurrentChar;
						m_iLineEndSelection = m_iLineStartSelection;
						m_iCharEndSelection = m_iCharStartSelection;
					}
				}
			}
			break;

		case VK_CONTROL:	// Ctrl ��
			{
				// ����б���Ƿ�ɼ�
				if (m_wndListBox.m_hWnd != NULL)
				{
					// ������Ϣ���б��
					m_wndListBox.SendMessage(WM_KEYDOWN, (WPARAM)nChar, 0);
					return;
				}
				else
				{
					// ����ѡ��ģʽ
					m_bCopying = TRUE;
				}
			}
			break;

		case VK_DELETE:	// Delete ��
			{
				// ����б���Ƿ�ɼ�
				if (m_wndListBox.m_hWnd != NULL)
				{
					// ������Ϣ���б��
					m_wndListBox.SendMessage(WM_KEYDOWN, (WPARAM)nChar, 0);
					return;
				}
				else
				{
					// �����ס Shift
					if (m_bSelecting)
					{
						m_bSelecting = FALSE;
						return;
					}

					// �����Чѡ��
					if ((m_iLineStartSelection != m_iLineEndSelection) || (m_iCharStartSelection != m_iCharEndSelection))
					{
						// ɾ��ѡ���ı�
						DeleteText();
					}
					else
					{
						// ɾ����һ���ַ�
						DelChar(m_iCurrentLine, m_iCurrentChar);
					}

					// ���¿ؼ�
					UpdateControl();

					// ����ģʽ
					bUpdate = TRUE;
				}
			}
			break;

		case VK_BACK:	// Back Space ��
			{
				// ����б���Ƿ�ɼ�
				if (m_wndListBox.m_hWnd != NULL)
				{
					// ������Ϣ���б��
					m_wndListBox.SendMessage(WM_KEYDOWN, (WPARAM)nChar, 0);
					return;
				}
				else
				{
					// ����ģʽ
					bUpdate = TRUE;
				}
			}
			break;

		case VK_PRIOR:	// Page Up ��
			{
				OnVScroll(SB_PAGEUP, 0, 0);
			}
			break;

		case VK_NEXT:	// Page Down ��
			{
				OnVScroll(SB_PAGEDOWN, 0, 0);
			}
			break;

		case VK_LWIN:
		case VK_RWIN:
		case VK_APPS:
		case VK_NUMLOCK:
		case VK_SCROLL:
		case VK_INSERT:
		case VK_PAUSE:
		case VK_CAPITAL:
		case VK_ESCAPE:
		case VK_F1:
			break;

		default:
			{
				// ����б���Ƿ�ɼ�
				if (m_wndListBox.m_hWnd != NULL)
				{
					// ������Ϣ���б��
					m_wndListBox.SendMessage(WM_KEYDOWN, (WPARAM)nChar, 0);
					return;
				}
				else
				{
					// ������ڸ��Ʊ�־
					if (!m_bCopying)
					{
						// �����Чѡ��
						if ((m_iLineStartSelection != m_iLineEndSelection) || (m_iCharStartSelection != m_iCharEndSelection))
						{
							// ɾ��ѡ���ı�
							DeleteText();
						}
					}
				}
			}
			break;
		}

		// �����Чѡ��
		if ((!m_bSelecting) && (!bUpdate) && (!m_bCopying))
		{
			// ���ѡ��
			m_iLineStartSelection = -1;
			m_iLineEndSelection = -1;
			m_iCharStartSelection = -1;
			m_iCharEndSelection = -1;
		}

		// ���¹��
		UpdateCaret();
	}

	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CCodeEdit::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// �����Ч����
	if (m_lpszText != NULL)
	{
		// �����������
		switch (nChar)
		{
		case VK_SHIFT:
			{
				// ����б���Ƿ�ɼ�
				if (m_wndListBox.m_hWnd != NULL)
				{
					// ������Ϣ���б��
					m_wndListBox.SendMessage(WM_KEYDOWN, (WPARAM)nChar, 0);
				}
				else
				{
					// ѡ��ģʽ
					m_bSelecting = FALSE;
				}
			}
			break;

		case VK_CONTROL:
			{
				// ����б���Ƿ�ɼ�
				if (m_wndListBox.m_hWnd != NULL)
				{
					// ������Ϣ���б��
					m_wndListBox.SendMessage(WM_KEYDOWN, (WPARAM)nChar, 0);
				}
				else
				{
					// ����ѡ��ģʽ
					m_bCopying = FALSE;
				}
			}
			break;
		}
	}

	CWnd::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CCodeEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// ����������
	switch (nChar)
	{
	case VK_RETURN:		// �س� && Ctrl+M
		{
			// ����б���Ƿ�ɼ�
			if (m_wndListBox.m_hWnd != NULL)
			{
				OnLbnDblclkListAutoComplete();
				return;
			}
			else
			{
				// �������
				AddLine(m_iCurrentLine, m_iCurrentChar);

				// ���λ��
				m_iCurrentChar = 0;
				m_iCurrentLine++;
			}
		}
		break;

	case VK_BACK:		// �˸�
		{
			// �����б��
			DestroyList();

			// �����Чѡ��
			if ((m_iLineStartSelection != m_iLineEndSelection) || (m_iCharStartSelection != m_iCharEndSelection))
			{
				// ɾ��ѡ���ı�
				DeleteText();
			}
			else
			{
				// ɾ��ǰ���ַ�
				int back = BackChar(m_iCurrentLine, m_iCurrentChar);
				if (back == 1)
				{
					m_iCurrentLine = max(0, m_iCurrentLine-1);
					m_iCurrentChar = m_lpLineInfo[m_iCurrentLine].nLen;
				}
				else if (back == 2)
				{
					m_iCurrentChar = max(0, m_iCurrentChar-1);
				}
			}
		}
		break;

	case VK_ESCAPE:		// ESC
		{
			// �����־
			m_bSelecting = FALSE;
			m_bCopying = FALSE;

			// �����б��
			DestroyList();
		}
		break;

	case 0x1A:		// Ctrl+Z
		{
			// ������Գ��������ó�������
			if (m_bCanUndo == TRUE)
			{
				// �����б��
				DestroyList();

				// ��������
				Undo();
			}
		}
		break;

	case 0x03:		// Ctrl+C
		{
			// �����Чѡ��
			if ((m_iLineStartSelection != m_iLineEndSelection) || (m_iCharStartSelection != m_iCharEndSelection))
			{
				// ���ѡ����Χ
				int iStartChar, iEndChar;
				int iStartLine, iEndLine;
				if (m_iLineStartSelection < m_iLineEndSelection)
				{
					iStartChar = m_iCharStartSelection;
					iEndChar = m_iCharEndSelection;
					iStartLine = m_iLineStartSelection;
					iEndLine = m_iLineEndSelection;
				}
				else if (m_iLineStartSelection > m_iLineEndSelection)
				{
					iStartChar = m_iCharEndSelection;
					iEndChar = m_iCharStartSelection;
					iStartLine = m_iLineEndSelection;
					iEndLine = m_iLineStartSelection;
				}
				else
				{
					iStartChar = min(m_iCharStartSelection, m_iCharEndSelection);
					iEndChar = max(m_iCharStartSelection, m_iCharEndSelection);
					iStartLine = m_iLineStartSelection;
					iEndLine = m_iLineEndSelection;
				}
				int iStartOffset = GetCharOffset(iStartLine, iStartChar);
				if (m_lpszText[iStartOffset] == '\r')
					iStartOffset += 2;
				int iEndOffset = GetCharOffset(iEndLine, iEndChar);
				int iCount = iEndOffset - iStartOffset;

				// ����ѡ���ı���������
				if (::OpenClipboard(m_hWnd))
				{
					::EmptyClipboard();
					HGLOBAL hMem = ::GlobalAlloc(GMEM_MOVEABLE, (iCount+1)*sizeof(_TCHAR));
					LPTSTR lpszText = (LPTSTR)::GlobalLock(hMem);
					memcpy(lpszText, m_lpszText+iStartOffset, iCount*sizeof(_TCHAR));
					lpszText[iCount] = '\0';
					::SetClipboardData(CF_UNICODETEXT, hMem);
					::GlobalUnlock(hMem);
					::CloseClipboard();
				}
			}
		}
		break;

	case 0x16:		// Ctrl+V
		{
			// �����б��
			DestroyList();

			// �����Чѡ��
			if ((m_iLineStartSelection != m_iLineEndSelection) || (m_iCharStartSelection != m_iCharEndSelection))
			{
				// ɾ��ѡ���ı�
				DeleteText();
			}

			// �Ӽ�����ճ���ı�
			if (::OpenClipboard(m_hWnd))
			{
				int iLine = m_iCurrentLine;
				int iChar = 0;
				HGLOBAL hMem = ::GetClipboardData(CF_UNICODETEXT);
				LPTSTR lpszText = (LPTSTR)::GlobalLock(hMem);
				if (lpszText != NULL)
				{
					// ��� UNICODE �ı�
					int iLen = (int)_tcslen(lpszText);
					AddText(lpszText, m_iCurrentLine, m_iCurrentChar, iLen);
					for (int i = 0; i < iLen; i++)
					{
						if (lpszText[i] == '\n')
						{
							iLine++;
							iChar = 0;
						}
						else
						{
							if (lpszText[i] != '\r')
								iChar++;
						}
					}
				}
				else
				{
					// ��� ANSI �ı�
					hMem = ::GetClipboardData(CF_TEXT);
					LPSTR lpszText = (LPSTR)::GlobalLock(hMem);
					if (lpszText != NULL)
					{
						int iLen = (int)strlen(lpszText);
						int iSize = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)lpszText, iLen, NULL, 0);
						LPTSTR lpszNewText = (LPTSTR)malloc(iSize*sizeof(_TCHAR));
						MultiByteToWideChar(CP_ACP, 0, (LPCSTR)lpszText, iLen, lpszNewText, iSize);
						AddText(lpszNewText, m_iCurrentLine, m_iCurrentChar, iLen);
						for (int i = 0; i < iLen; i++)
						{
							if (lpszText[i] == '\n')
							{
								iLine++;
								iChar = 0;
							}
							else
							{
								if (lpszText[i] != '\r')
									iChar++;
							}
						}
						free(lpszNewText);
					}
				}
				::GlobalUnlock(hMem);
				::CloseClipboard();

				// ���ù��λ��
				if (m_iCurrentLine == iLine)
					m_iCurrentChar += iChar;
				else
					m_iCurrentChar = iChar;
				m_iCurrentLine = iLine;

				// ��������Ϣ
				m_UndoInfo.iEndLine = m_iCurrentLine;
				m_UndoInfo.iEndChar = m_iCurrentChar;
			}
		}
		break;

	case 0x18:		// Ctrl+X
		{
			// �����б��
			DestroyList();

			if ((m_iLineStartSelection != m_iLineEndSelection) || (m_iCharStartSelection != m_iCharEndSelection))
			{
				// ���ѡ����Χ
				int iStartChar, iEndChar;
				int iStartLine, iEndLine;
				if (m_iLineStartSelection < m_iLineEndSelection)
				{
					iStartChar = m_iCharStartSelection;
					iEndChar = m_iCharEndSelection;
					iStartLine = m_iLineStartSelection;
					iEndLine = m_iLineEndSelection;
				}
				else if (m_iLineStartSelection > m_iLineEndSelection)
				{
					iStartChar = m_iCharEndSelection;
					iEndChar = m_iCharStartSelection;
					iStartLine = m_iLineEndSelection;
					iEndLine = m_iLineStartSelection;
				}
				else
				{
					iStartChar = min(m_iCharStartSelection, m_iCharEndSelection);
					iEndChar = max(m_iCharStartSelection, m_iCharEndSelection);
					iStartLine = m_iLineStartSelection;
					iEndLine = m_iLineEndSelection;
				}
				int iStartOffset = GetCharOffset(iStartLine, iStartChar);
				if (m_lpszText[iStartOffset] == '\r')
					iStartOffset += 2;
				int iEndOffset = GetCharOffset(iEndLine, iEndChar);
				int iCount = iEndOffset - iStartOffset;

				// ����ѡ�����ݵ�������
				if (::OpenClipboard(m_hWnd))
				{
					::EmptyClipboard();
					HGLOBAL hMem = ::GlobalAlloc(GMEM_MOVEABLE, (iCount+1)*sizeof(_TCHAR));
					LPTSTR lpszText = (LPTSTR)::GlobalLock(hMem);
					memcpy(lpszText, m_lpszText+iStartOffset, iCount*sizeof(_TCHAR));
					lpszText[iCount] = '\0';
					::SetClipboardData(CF_UNICODETEXT, hMem);
					::GlobalUnlock(hMem);
					::CloseClipboard();
				}

				// ɾ��ѡ���ı�
				DeleteText();
			}
		}
		break;

	case 0x01:		// Ctrl+A
		{
			// �����б��
			DestroyList();

			// ѡ��ȫ���ı�
			m_iCharStartSelection = 0;
			m_iLineStartSelection = 0;
			m_iCharEndSelection = m_lpLineInfo[m_iNumberLines-1].nLen;
			m_iLineEndSelection = m_iNumberLines - 1;

			// ���ù��λ��
			m_iCurrentLine = m_iLineEndSelection;
			m_iCurrentChar = m_iCharEndSelection;
		}
		break;

	case 0x0a:	// Ctrl+J
		{
			// �����־
			m_bSelecting = FALSE;

			// ��ʾ�Զ�����б��
			ShowAutoComplete();

			// �˳�
			return;
		}
		break;
	case 0x0c:		//Ctrl+L ɾ��һ��
		{
			if (m_lpszText != NULL)
			{
				// ���ѡ��
				m_iLineStartSelection = -1;
				m_iLineEndSelection = -1;
				m_iCharStartSelection = -1;
				m_iCharEndSelection = -1;

				// �����б��
				DestroyList();

				// �ж�Ҫɾ���ķ�Χ
				if (m_iCurrentLine == m_iNumberLines - 1)	// ��������һ��
				{
					// �ǵ�һ����Ҫɾ������
					if (m_lpLineInfo[m_iCurrentLine].nLen == 0)	// �ǿ���
					{
						BackChar(m_iCurrentLine, 0);
					}
					else
					{
						// ɾ���ı�
						if (m_iNumberLines != 1)
							DelText(m_iCurrentLine - 1, m_lpLineInfo[m_iCurrentLine - 1].nLen, m_iCurrentLine, m_lpLineInfo[m_iCurrentLine].nLen);
						else
							DelText(m_iCurrentLine, 0, m_iCurrentLine, m_lpLineInfo[m_iCurrentLine].nLen);
					}
					m_iCurrentLine = max(0, m_iCurrentLine-1);
				}
				else
				{
					if (m_lpLineInfo[m_iCurrentLine].nLen == 0)	// �ǿ���
					{
						BackChar(m_iCurrentLine, 0);
					}
					else
					{
						// ɾ���ı�
						DelText(m_iCurrentLine, 0, m_iCurrentLine + 1, 0);
					}
				}
				m_iCurrentChar = 0;
			}
		}
		break;

	default:
		{
			// ������ڸ��Ʊ�־
			if (!m_bCopying)
			{
				// ������ַ�
				AddChar(m_iCurrentLine, m_iCurrentChar, nChar);
				m_iCurrentChar++;
			}
			else
			{
				// �����־
				m_bSelecting = FALSE;
				m_bCopying = FALSE;

				// �����б��
				DestroyList();
			}
		}
	}

	// ���¿ؼ�
	UpdateControl();

	// ���¹��
	UpdateCaret();

	CWnd::OnChar(nChar, nRepCnt, nFlags);
}

BOOL CCodeEdit::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// �������ָ��
	if ((nHitTest == HTCLIENT) && (m_wndListBox.m_hWnd == NULL))
	{
		SetCursor(LoadCursor(NULL, IDC_IBEAM));
		return TRUE;
	}
	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}
