// MyToolBar.cpp : ʵ���ļ�
// 

#include "stdafx.h"
#include "CodeMate.h"
#include "MyToolBar.h"


// CMyToolBar

IMPLEMENT_DYNAMIC(CMyToolBar, CToolBar)
CMyToolBar::CMyToolBar()
{
}

CMyToolBar::~CMyToolBar()
{
}


BEGIN_MESSAGE_MAP(CMyToolBar, CToolBar)
	ON_MESSAGE(WM_IDLEUPDATECMDUI, OnIdleUpdateCmdUI)
END_MESSAGE_MAP()



// CMyToolBar ��Ϣ�������

BOOL CMyToolBar::LoadTrueColorToolBar(int nBtnWidth, UINT uToolBar, UINT uToolBarHot, UINT uToolBarDisabled) 
{
	// ����Ĭ��ͼ��
	if (!SetTrueColorToolBar(TB_SETIMAGELIST, uToolBar, nBtnWidth)) return FALSE;
	// ������ͼ��
	if (!SetTrueColorToolBar(TB_SETHOTIMAGELIST, uToolBarHot, nBtnWidth))  return  FALSE;
	// ������Чͼ��
	if (uToolBarDisabled)
	{
		if (!SetTrueColorToolBar(TB_SETDISABLEDIMAGELIST, uToolBarDisabled, nBtnWidth))
			return FALSE;
	}
	return TRUE;
}

BOOL CMyToolBar::SetTrueColorToolBar(UINT uToolBarType, UINT uToolBar, int nBtnWidth) 
{ 
	CImageList	cImageList; 
	CBitmap	 cBitmap; 
	BITMAP	bmBitmap; 
	CSize cSize; 
	int	 nNbBtn; 
	// ����24λλͼ��Դ
	if (!cBitmap.Attach(LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(uToolBar), IMAGE_BITMAP, 0, 0, 
		LR_DEFAULTSIZE|LR_CREATEDIBSECTION))  || !cBitmap.GetBitmap(&bmBitmap)) 
		return FALSE; 
	cSize = CSize(bmBitmap.bmWidth, bmBitmap.bmHeight);   
	nNbBtn = cSize.cx/nBtnWidth; 
	RGBTRIPLE* rgb = (RGBTRIPLE*)(bmBitmap.bmBits); 
	COLORREF rgbMask = RGB(rgb[0].rgbtRed, rgb[0].rgbtGreen, rgb[0].rgbtBlue); 
	// ����ͼ���б�
	if (!cImageList.Create(nBtnWidth, cSize.cy, ILC_COLOR24|ILC_MASK, nNbBtn, 0)) 
		return FALSE; 
	// ��Դ��ӵ�ͼ���б�
	if (cImageList.Add(&cBitmap, rgbMask) == -1) 
		return FALSE; 
	// ���ù�����ͼ���б�
	SendMessage(uToolBarType, 0, (LPARAM)cImageList.m_hImageList); 
	cImageList.Detach();   
	cBitmap.Detach(); 
	return   TRUE; 
} 

LRESULT CMyToolBar::OnIdleUpdateCmdUI(WPARAM wParam, LPARAM) 
{
	if (IsWindowVisible()) 
	{
		CFrameWnd *pParent = (CFrameWnd *)GetParent();
		if (pParent)
			OnUpdateCmdUI(pParent, (BOOL)wParam);
	}
	return 0L;
}