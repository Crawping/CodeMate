#pragma once
#include "afxcolorbutton.h"
#include "afxwin.h"
#include "afxcmn.h"
#include "CodeEdit.h"

// CConfigDlg �Ի���

class CConfigDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CConfigDlg)

public:
	CConfigDlg(CWnd* pParent = NULL);   // ��׼���캯��
	~CConfigDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_CONFIG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	HBRUSH OnCtlColor(CDC *pDC, CWnd *pWnd, UINT nCtlColor);

	DECLARE_MESSAGE_MAP()

public:
	UINT m_uiLanguage;			// ��������
	COLORINFO m_coColor;
	CMFCColorButton m_ColorLineNumBG;		// ��ɫѡ��ť����ͬ
	CMFCColorButton m_ColorLineNumFG;
	CMFCColorButton m_ColorKeyword;
	CMFCColorButton m_ColorConstant;
	CMFCColorButton m_ColorComment;
	CMFCColorButton m_ColorText;
	CMFCColorButton m_ColorBackground;
	CMFCColorButton m_ColorForeground;
	CMFCEditBrowseCtrl m_Folder_Browser;	// ����ļ��пؼ�
	CVSListBox m_VSListBox;	// �ؼ�������б�ؼ�
	CListCtrl m_ListCode;	// �����б�
	CImageList m_wndImageList;	// ����ͼ���б�ؼ�
	CStringArray m_arrKeyWord;	// �ؼ����б�����
	CStringArray m_arrConstant;	// �����б�����
	int m_CodeType;			// ��������
	LOGFONT font;		// ����������Ϣ
	CEdit m_FontShow;	// ��ʾ������Ϣ�ı༭��
	CTabCtrl m_Tabs;	// ѡ��ؼ�
	CComboBox m_Combo_Encoding;	// ѡ�����������
	CButton m_Check_LineNum;	// �Ƿ���ʾ�кŸ�ѡ��
	CEdit m_Edit_Tabs;			// Tab ����
	CSpinButtonCtrl m_Spin;		// ���ֵ���
	CButton m_Button_Font;		// ѡ������
	UINT m_uiDefEncode;		// Ĭ�ϱ���
	CString m_strAppPath;	// ����Ŀ¼
	CString m_strWorkPath;	// ����Ŀ¼
	CString bold;		// ���桰���塱�Ķ������ַ���
	CString italic;		// ���桰б�塱�Ķ������ַ���
	CString name;		// �������
	int m_iSize;		// �ֺ�
	int m_iTabLen;			// Tab ����
	BOOL m_bShowLineNum;	// �Ƿ���ʾ�к�
	BOOL isKeyWord;		// ��ǰ���õ��ǹؼ��ʻ���
	bool isMoved;		// �ļ����Ƿ��Ѿ��ƶ�
	bool MoveFolder(CString strFromPath, CString strToPath);		// �ƶ��ļ���
	void AddWords(LPTSTR lpszKeywords, int iLen);					// ��ӹؼ���
	void SaveWords();												// ����ؼ���
	afx_msg void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);	// ѡ��л�
	afx_msg void OnBnClickedRadio1();	// ��ѡť����
	afx_msg void OnBnClickedRadio2();	// ��ѡť����
	afx_msg void OnEnChangeEdit2();		// �༭�����ݸı�
	afx_msg void OnBnClickedOk();		// ���¡�ȷ����
	afx_msg void OnBnClickedButton1();	// ѡ������
	afx_msg void OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);
};
