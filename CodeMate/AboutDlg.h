#pragma once

#ifdef _WIN64
#define VER64 TRUE
#else
#define VER64 FALSE
#endif

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	// ʵ��
protected:
	virtual BOOL OnInitDialog();	// ��ʼ���Ի���
	HBRUSH OnCtlColor(CDC *pDC, CWnd *pWnd, UINT nCtlColor);
	CString GetAppVersion();		// �������汾��Ϣ

	DECLARE_MESSAGE_MAP()
public:
	CString m_strWorkPath;	// �����ļ���
	CString m_strAppPath;	// �����ļ���
	CStatic m_stcVersion;	// ��ʾ�汾��
	UINT m_uiLanguage;		// ��������
};