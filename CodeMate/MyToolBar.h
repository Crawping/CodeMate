#pragma once


// CMyToolBar

class CMyToolBar : public CToolBar
{
	DECLARE_DYNAMIC(CMyToolBar)

public:
	CMyToolBar();
	virtual ~CMyToolBar();

protected:
	DECLARE_MESSAGE_MAP()

private:
	afx_msg LRESULT OnIdleUpdateCmdUI(WPARAM wParam, LPARAM);	// �������ڴ��������µ� WM_IDLEUPDATECMDUI ��Ϣ 
	BOOL   SetTrueColorToolBar(UINT uToolBarType, UINT uToolBar, int nBtnWidth);		// �������ͼ��

public: 
	BOOL   LoadTrueColorToolBar(int nBtnWidth, UINT uToolBar, UINT uToolBarHot, UINT uToolBarDisabled = 0);		// �������ͼ��
};


