#pragma once


// CChangeTypeDlg �Ի���

class CChangeTypeDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CChangeTypeDlg)

public:
	CChangeTypeDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CChangeTypeDlg();

// �Ի�������
	enum { IDD = IDD_CHANGELANG_DIALOG };

protected:
	virtual BOOL OnInitDialog();	// �Ի����ʼ��
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);	// �б���ͼ�ı�ѡ��
	afx_msg void OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);			// �б���ͼ��Ŀ��˫��
	CString m_strAppPath;		// ����Ŀ¼
	CString m_strWorkPath;	// ����Ŀ¼
	CImageList m_wndImageList;	// ����ͼ���б�ؼ�
	CListCtrl m_ListCode;	// �����б���ͼ�ؼ�
	int m_CodeType;			// ��������
	UINT m_uiLanguage;			// ��������
};
