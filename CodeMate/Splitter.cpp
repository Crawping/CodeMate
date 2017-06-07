// Splitter.cpp : ʵ���ļ�
// 

#include "stdafx.h"
#include "CodeMate.h"
#include "Splitter.h"
#include "CodeMateDlg.h"

// CSplitter

IMPLEMENT_DYNAMIC(CSplitter, CBitmapButton)
CSplitter::CSplitter()
{
}

CSplitter::~CSplitter()
{
}


BEGIN_MESSAGE_MAP(CSplitter, CBitmapButton)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

// CSplitter ��Ϣ�������
BOOL CSplitter::AutoLoad(UINT nID, CWnd* pParent)
{
	// ��һ�κͶԻ���ؼ�����
	if (!SubclassDlgItem(nID, pParent))
		return FALSE;

	CString buttonName;
	GetWindowText(buttonName);
	ASSERT(!buttonName.IsEmpty());      // �����ṩ����

	LoadBitmaps(buttonName + _T("U"), buttonName + _T("D"),
		buttonName + _T("F"), buttonName + _T("X"));

	// ��Ҫ������λͼ
	if (m_bitmap.m_hObject == NULL)
		return FALSE;

	return TRUE;
}

void CSplitter::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
	ASSERT(lpDIS != NULL);
	// ����Ҫ��һ��λͼ���ܵ��ô˺���
	ASSERT(m_bitmap.m_hObject != NULL);     // �����

	// ��λͼ���ϣ�ѡ��λͼ����
	CBitmap* pBitmap = &m_bitmap;
	UINT state = lpDIS->itemState;
	if ((state & ODS_SELECTED) && m_bitmapSel.m_hObject != NULL)
		pBitmap = &m_bitmapSel;
	else if ((state & ODS_FOCUS) && m_bitmapFocus.m_hObject != NULL)
		pBitmap = &m_bitmapFocus;   // ������ͼ���ʾ�н���״̬
	else if ((state & ODS_DISABLED) && m_bitmapDisabled.m_hObject != NULL)
		pBitmap = &m_bitmapDisabled;   // ����ͼ���ʾ��Ч״̬

	// ����������ť
	CDC* pDC = CDC::FromHandle(lpDIS->hDC);
	CDC memDC;
	memDC.CreateCompatibleDC(pDC);
	CBitmap* pOld = memDC.SelectObject(pBitmap);
	if (pOld == NULL)
		return;
	// ����ͼƬ
	CRect rect;
	rect.CopyRect(&lpDIS->rcItem);
	BITMAP bits;
	pBitmap->GetObject(sizeof(BITMAP),&bits);
	pDC->StretchBlt(rect.left,rect.top,rect.Width(),rect.Height(),
		&memDC,0,0,bits.bmWidth, bits.bmHeight, SRCCOPY);

	memDC.SelectObject(pOld);
}

void CSplitter::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// ���ز���ʾ�з����ָ��
	HCURSOR hCursor = (HCURSOR)AfxGetApp()->LoadCursor(IDC_CURSOR_SPLITTER);
	::SetCursor(hCursor);
	// ִ������ܴ�����
	((CCodeMateDlg*)GetParent())->OnSplitterDown(this->GetDlgCtrlID(), point);
	CButton::OnLButtonDown(nFlags, point);
}

void CSplitter::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// ִ������ܴ�����
	((CCodeMateDlg*)GetParent())->OnSplitterUp(this->GetDlgCtrlID(), point);
	CButton::OnLButtonUp(nFlags, point);
}

void CSplitter::OnMouseMove(UINT nFlags, CPoint point) 
{
	// ���ز���ʾ�з����ָ��
	HCURSOR hCursor = (HCURSOR)AfxGetApp()->LoadCursor(IDC_CURSOR_SPLITTER);
	::SetCursor(hCursor);
	// ִ������ܴ�����
	((CCodeMateDlg*)GetParent())->OnSplitterMove(this->GetDlgCtrlID(), point);
	CButton::OnMouseMove(nFlags, point);
}