// FilePrepDlg.cpp : ʵ���ļ�
// 

#include "stdafx.h"
#include "CodeMate.h"
#include "FilePrepDlg.h"


// CFilePrepDlg �Ի���

IMPLEMENT_DYNAMIC(CFilePrepDlg, CDialogEx)
CFilePrepDlg::CFilePrepDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CFilePrepDlg::IDD, pParent)
	, m_strTitle(_T(""))
	, m_strType(_T(""))
	, m_strPath(_T(""))
{
}

CFilePrepDlg::~CFilePrepDlg()
{
}

void CFilePrepDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strTitle);
	DDX_Text(pDX, IDC_STATIC_TYPE, m_strType);
	DDX_Text(pDX, IDC_EDIT2, m_strPath);
	DDX_Text(pDX, IDC_STATIC_CRETIME, m_strCreTime);
	DDX_Text(pDX, IDC_STATIC_MODTIME, m_strModTime);
	DDX_Text(pDX, IDC_STATIC_ACCTIME, m_strAccTime);
}


BEGIN_MESSAGE_MAP(CFilePrepDlg, CDialogEx)
END_MESSAGE_MAP()

// CFilePrepDlg ��Ϣ�������

BOOL CFilePrepDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CString strLoad;
	if (m_uiLanguage != 0)
	{
		SetCurrentDirectory(m_strAppPath);
		LPWSTR strFileName = NULL;
		if (m_uiLanguage == 1)
			strFileName = LANG_TCHINESE_FILE;
		else if (m_uiLanguage == 2)
			strFileName = LANG_ENGLISH_FILE;
		LPWSTR lpszLang = new WCHAR[LANG_STRING_LEN];
		// ������
		GetPrivateProfileString(LANG_ATTRIB_DLG, _T("DIALOG_TITLE"), _T(""), lpszLang, LANG_STRING_LEN, strFileName);
		strLoad = lpszLang;
		SetWindowText(strLoad);
		// ��ť
		CButton *pButton = (CButton *) GetDlgItem(IDOK);
		GetPrivateProfileString(LANG_STRING, _T("DIALOG_OK"), _T(""), lpszLang, LANG_STRING_LEN, strFileName);
		strLoad = lpszLang;
		pButton -> SetWindowText(strLoad);
		// ��̬�ı�
		CStatic *pStatic = NULL;
		GetPrivateProfileString(LANG_ATTRIB_DLG, _T("STATIC_TITLE"), _T(""), lpszLang, LANG_STRING_LEN, strFileName);
		strLoad = lpszLang;
		pStatic = (CStatic *) GetDlgItem(IDC_STATIC_TITLE);
		pStatic -> SetWindowText(strLoad);
		GetPrivateProfileString(LANG_ATTRIB_DLG, _T("STATIC_TYPE"), _T(""), lpszLang, LANG_STRING_LEN, strFileName);
		strLoad = lpszLang;
		pStatic = (CStatic *) GetDlgItem(IDC_STATIC_LANTYPE);
		pStatic -> SetWindowText(strLoad);
		GetPrivateProfileString(LANG_ATTRIB_DLG, _T("STATIC_CLASS"), _T(""), lpszLang, LANG_STRING_LEN, strFileName);
		strLoad = lpszLang;
		pStatic = (CStatic *) GetDlgItem(IDC_STATIC_CLASS);
		pStatic -> SetWindowText(strLoad);
		GetPrivateProfileString(LANG_ATTRIB_DLG, _T("STATIC_CREATED_TIME"), _T(""), lpszLang, LANG_STRING_LEN, strFileName);
		strLoad = lpszLang;
		pStatic = (CStatic *) GetDlgItem(IDC_STATIC_CTIME);
		pStatic -> SetWindowText(strLoad);
		GetPrivateProfileString(LANG_ATTRIB_DLG, _T("STATIC_MIDIFIED_TIME"), _T(""), lpszLang, LANG_STRING_LEN, strFileName);
		strLoad = lpszLang;
		pStatic = (CStatic *) GetDlgItem(IDC_STATIC_MTIME);
		pStatic -> SetWindowText(strLoad);
		GetPrivateProfileString(LANG_ATTRIB_DLG, _T("STATIC_ACCESSED_TIME"), _T(""), lpszLang, LANG_STRING_LEN, strFileName);
		strLoad = lpszLang;
		pStatic = (CStatic *) GetDlgItem(IDC_STATIC_ATIME);
		pStatic -> SetWindowText(strLoad);
		delete []lpszLang;
		SetCurrentDirectory(m_strWorkPath);
	}

	// ��ȡ�ļ�����
	SYSTEMTIME CreTime, ModTime, AccTime;
	if (!m_bIsFolder)	// ���ļ���
	{
		CString FileExt;
		switch (m_iFileType)
		{
		case 1:	// C++
			FileExt=_T(".c-p");
			break;
		case 5:	// C
			FileExt=_T(".c-a");
			break;
		case 7:	// HTM
			FileExt=_T(".h-t");
			break;
		case 9:	// JScript
			FileExt=_T(".j-s");
			break;
		case 11:	// SQL
			FileExt=_T(".s-q");
			break;
		case 13:	// TXT
			FileExt=_T(".txt");
			break;
		case 15:	// VBScript
			FileExt=_T(".v-b");
			break;
		case 17:	// C#
			FileExt=_T(".c-s");
			break;
		}
		m_strFullPath = m_strFullPath + _T("\\") + m_strTitle + FileExt;
	}
	// ��ȡ�ļ����ļ��е�ʱ������
	if (GetDirTime(m_strFullPath, CreTime, ModTime, AccTime))
	{
		if (m_uiLanguage == 2)
		{
			m_strCreTime.Format(_T("%02d/%02d/%04d %02d:%02d:%02d"),
				CreTime.wMonth, CreTime.wDay, CreTime.wYear,
				CreTime.wHour, CreTime.wMinute, CreTime.wSecond);
			m_strModTime.Format(_T("%02d/%02d/%04d %02d:%02d:%02d"),
				ModTime.wMonth, ModTime.wDay, ModTime.wYear,
				ModTime.wHour, ModTime.wMinute, ModTime.wSecond);
			m_strAccTime.Format(_T("%02d/%02d/%04d %02d:%02d:%02d"),
				AccTime.wMonth, AccTime.wDay, AccTime.wYear,
				AccTime.wHour, AccTime.wMinute, AccTime.wSecond);
		}
		else
		{
			m_strCreTime.Format(_T("%04d-%02d-%02d %02d:%02d:%02d"),
				CreTime.wYear, CreTime.wMonth, CreTime.wDay,
				CreTime.wHour, CreTime.wMinute, CreTime.wSecond);
			m_strModTime.Format(_T("%04d-%02d-%02d %02d:%02d:%02d"),
				ModTime.wYear, ModTime.wMonth, ModTime.wDay,
				ModTime.wHour, ModTime.wMinute, ModTime.wSecond);
			m_strAccTime.Format(_T("%04d-%02d-%02d %02d:%02d:%02d"),
				AccTime.wYear, AccTime.wMonth, AccTime.wDay,
				AccTime.wHour, AccTime.wMinute, AccTime.wSecond);
		}
	}
	else	// ��ȡʧ��
	{
		CString strNotAccess;
		LPWSTR lpszLang = new WCHAR[LANG_STRING_LEN];
		if (m_uiLanguage == 0)
			strNotAccess.LoadString(IDS_PREP_NOTACCES);
		else if (m_uiLanguage == 1)
		{
			SetCurrentDirectory(m_strAppPath);
			GetPrivateProfileString(LANG_STRING, _T("NORMAL_TEXT"),
				_T(""), lpszLang, LANG_STRING_LEN, LANG_TCHINESE_FILE);
			strNotAccess = lpszLang;
			SetCurrentDirectory(m_strWorkPath);
		}
		else if (m_uiLanguage == 2)
		{
			SetCurrentDirectory(m_strAppPath);
			GetPrivateProfileString(LANG_STRING, _T("NORMAL_TEXT"),
				_T(""), lpszLang, LANG_STRING_LEN, LANG_ENGLISH_FILE);
			strNotAccess = lpszLang;
			SetCurrentDirectory(m_strWorkPath);
		}
		delete []lpszLang;
		m_strCreTime = strNotAccess;
		m_strModTime = strNotAccess;
		m_strAccTime = strNotAccess;
	}
	UpdateData(FALSE);		// ���¿ؼ�����ʾ
	return TRUE;
}

BOOL CFilePrepDlg::GetDirTime(CString strDirName, SYSTEMTIME &Cretime,
	SYSTEMTIME &Modtime, SYSTEMTIME &Acctime)
{
    // ���ļ���
	HANDLE hDir = CreateFile (strDirName, GENERIC_READ,
		FILE_SHARE_READ|FILE_SHARE_DELETE, NULL,
		OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
	if (hDir != INVALID_HANDLE_VALUE)
	{
		FILETIME lpCreationTime; 	// �ļ��еĴ���ʱ��
		FILETIME lpLastAccessTime; 	// ���ļ��е��������ʱ��
		FILETIME lpLastWriteTime; 	// �ļ��е�����޸�ʱ�� ��
		// ��ȡ�ļ���ʱ��������Ϣ
		if (GetFileTime(hDir, &lpCreationTime, &lpLastAccessTime, &lpLastWriteTime))
		{
			FILETIME ftime;
			// ����ʱ��
			FileTimeToLocalFileTime(&lpCreationTime, &ftime); 
			FileTimeToSystemTime(&ftime, &Cretime); 
			// �޸�ʱ��
			FileTimeToLocalFileTime(&lpLastWriteTime, &ftime); 
			FileTimeToSystemTime(&ftime, &Modtime); 
			// ����ʱ��
			FileTimeToLocalFileTime(&lpLastAccessTime, &ftime); 
			FileTimeToSystemTime(&ftime, &Acctime); 
		}
		else	// ��ȡʧ��
		{
			CloseHandle(hDir); 		// �رմ򿪹����ļ���
			return FALSE;
		}
		CloseHandle(hDir); 	// �رմ򿪹����ļ���
		return TRUE;
	}
	else	// �鿴����ʧ��
	{
		CloseHandle(hDir); 		// �رմ򿪹����ļ���
		return FALSE;
	}
}
