#pragma once


// CFilePrepDlg �Ի���

class CFilePrepDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CFilePrepDlg)

public:
	CFilePrepDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CFilePrepDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_PREP };

protected:
	virtual BOOL OnInitDialog();	// �Ի����ʼ��
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	BOOL GetDirTime(CString strDirName, SYSTEMTIME &Cretime, SYSTEMTIME &Modtime, SYSTEMTIME &Acctime);	// ����ļ����ļ���ʱ������
	bool m_bIsFolder;		// �Ƿ�Ϊ�ļ���
	int m_iFileType;		// �ļ�����
	CString m_strAppPath;	// ����Ŀ¼
	CString m_strWorkPath;	// ����Ŀ¼
	CString m_strFullPath;	// ȫ·��
	CString m_strTitle;		// ��ʾ�ļ�����
	CString m_strType;		// ��ʾ�ļ�����
	CString m_strPath;		// ��ʾ�ļ�����·��
	CString m_strCreTime;	// ��ʾ�ļ�����ʱ��
	CString m_strModTime;	// ��ʾ�ļ��޸�ʱ��
	CString m_strAccTime;	// ��ʾ�ļ�����ʱ��
	UINT m_uiLanguage;		// ��������
};
