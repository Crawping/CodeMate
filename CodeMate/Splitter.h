#pragma once


// CSplitter

class CSplitter : public CBitmapButton
{
	DECLARE_DYNAMIC(CSplitter)

public:
	CSplitter();
	virtual ~CSplitter();
	BOOL AutoLoad(UINT nID, CWnd* pParent);     // ��д���뺯��
	void DrawItem(LPDRAWITEMSTRUCT lpDIS);		// ��д���ƺ���

protected:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);	// ����������
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);	// ����������
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);	// ����ƶ�

	DECLARE_MESSAGE_MAP()
};


