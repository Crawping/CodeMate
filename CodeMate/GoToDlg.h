#pragma once
#include "afxcmn.h"


// CGoToDlg �Ի���

class CGoToDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CGoToDlg)

public:
	CGoToDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CGoToDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_GOTO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();				// �Ի����ʼ��

	DECLARE_MESSAGE_MAP()

public:
	int LineNum;		// ����
	int m_LineIndex;	// ����ת���ڼ���
	int m_ColumnIndex;	// ����ת���ڼ���
	int *pMaxChar;		// ÿ������ַ���
	bool SetRange;		// �Ƿ����÷�Χ
	UINT m_uiLanguage;		// ��������
	CString m_strAppPath;		// ����Ŀ¼
	CString m_strWorkPath;	// ����Ŀ¼
	CSpinButtonCtrl m_Spin;				// ��ת�ؼ�
	CSpinButtonCtrl m_Spin2;			// ��ת�ؼ�2
	afx_msg void OnEnChangeEditLine();		// �༭��1�ı��仯
	afx_msg void OnEnChangeEditColumn();	// �༭��2�ı��仯
};
