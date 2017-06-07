#pragma once

// CMoveTo �Ի���

class CMoveToDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMoveToDlg)

public:
	CMoveToDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CMoveToDlg();

// �Ի�������
	enum { IDD = IDD_MOVETO_DIALOG };

protected:
	virtual BOOL OnInitDialog();	// �Ի����ʼ��
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	HTREEITEM root;			// ��Ŀ¼ָ��
	CString m_strAppPath;		// ����Ŀ¼
	CString m_strWorkPath;	// ����Ŀ¼
	CString FullPath;		// ȫ·��
	CString m_Path;			// ����·��
	CImageList m_wndImageList;	// ͼ���б�ؼ�
	CTreeCtrl m_wndTreeView;	// ���οؼ�
	UINT m_uiLanguage;			// ��������
	afx_msg void OnBnClickedOk();	// ȷ�ϲ���
	// ���º���ע����ο�CodeMateDlg.h
	afx_msg void OnTvnItemexpandedTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult);
	void GetClassDir(HTREEITEM hParent);
	void GetMainClass(HTREEITEM hParent, CString strText);
	void AddSubItem(HTREEITEM hParent);
	CString ItemToPath(HTREEITEM hCurrent);
};
