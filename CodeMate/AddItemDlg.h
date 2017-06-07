#pragma once

// CAddItem �Ի���

class CAddItemDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAddItemDlg)

public:
	CAddItemDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CAddItemDlg();

// �Ի�������
	enum { IDD = IDD_ADDITEM_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	void AddBitmap(CBitmap &bm, UINT uiBmpId);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedChangeClass();	// ѡ�����
	afx_msg void OnBnClickedSetDefault();	// ����Ĭ������
	afx_msg void OnBnClickedOk();			// ȷ�ϲ���
	CString m_strAppPath;	// ����Ŀ¼
	CString m_strWorkPath;	// ����Ŀ¼
	CString m_strFullPath;	// ȫ·��
	CString m_strTitle;		// ����
	CString m_strClass;		// ���࣬����·��
	bool m_bSaveAsDefault;		// ��ΪĬ��
	UINT m_uiSourceType;		// ������ʽ����Դ��
	UINT m_uiLangType;			// ��������
	bool m_bReConfirm;			// �����Ƿ���ģ��Ƿ���Ҫ���¶�λ
	CImageList m_wndImageList;	// ͼ���б�ؼ�
	CComboBoxEx	m_cboLanguage;	// ��չ��Ͽ�ؼ�
	UINT m_uiLanguage;			// ��������
};
