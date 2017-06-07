// AddItem.cpp : ʵ���ļ�
// 

#include "stdafx.h"
#include "CodeMate.h"
#include "AddItemDlg.h"
#include "MoveToDlg.h"
#include "IsValidFileName.h"

// CAddItem �Ի���

IMPLEMENT_DYNAMIC(CAddItemDlg, CDialogEx)
CAddItemDlg::CAddItemDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAddItemDlg::IDD, pParent)
	, m_strTitle(_T(""))
	, m_strClass(_T(""))
	, m_bReConfirm(false)
{
}

CAddItemDlg::~CAddItemDlg()
{
}

void CAddItemDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_TITLE, m_strTitle);
	DDX_Text(pDX, IDC_EDIT_CLASS, m_strClass);
	DDX_Control(pDX, IDC_COMBO_LANG, m_cboLanguage);
}


BEGIN_MESSAGE_MAP(CAddItemDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_CHANGE_CLASS, OnBnClickedChangeClass)
	ON_BN_CLICKED(IDC_SET_DEFAULT, &CAddItemDlg::OnBnClickedSetDefault)
END_MESSAGE_MAP()

// CAddItem ��Ϣ�������

BOOL CAddItemDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// ��������
	CString strLoad;
	if (m_uiLanguage != 0)
	{
		SetCurrentDirectory(m_strAppPath);
		LPWSTR lpszFileName = NULL;
		if (m_uiLanguage == 1)
			lpszFileName = LANG_TCHINESE_FILE;
		else if (m_uiLanguage == 2)
			lpszFileName = LANG_ENGLISH_FILE;
		LPWSTR lpszLang = new WCHAR[LANG_STRING_LEN];
		// ������
		GetPrivateProfileString(LANG_ADDITEM_DLG, _T("DIALOG_TITLE"),
			_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
		strLoad = lpszLang;
		SetWindowText(strLoad);
		// ��ť
		CButton *pButton = NULL;
		GetPrivateProfileString(LANG_ADDITEM_DLG, _T("BUTTON_CHANGE_CLASS"),
			_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
		strLoad = lpszLang;
		pButton = (CButton *) GetDlgItem(IDC_CHANGE_CLASS);
		pButton -> SetWindowText(strLoad);
		GetPrivateProfileString(LANG_ADDITEM_DLG, _T("BUTTON_SET_AS_DEFAULT"),
			_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
		strLoad = lpszLang;
		pButton = (CButton *) GetDlgItem(IDC_SET_DEFAULT);
		pButton -> SetWindowText(strLoad);
		GetPrivateProfileString(LANG_ADDITEM_DLG, _T("RADIO_EMPTY_FILE"),
			_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
		strLoad = lpszLang;
		pButton = (CButton *) GetDlgItem(IDC_RADIO1);
		pButton -> SetWindowText(strLoad);
		GetPrivateProfileString(LANG_ADDITEM_DLG, _T("RADIO_CLIPBOARD"),
			_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
		strLoad = lpszLang;
		pButton = (CButton *) GetDlgItem(IDC_RADIO2);
		pButton -> SetWindowText(strLoad);
		GetPrivateProfileString(LANG_ADDITEM_DLG, _T("RADIO_CODE_FILE"),
			_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
		strLoad = lpszLang;
		pButton = (CButton *) GetDlgItem(IDC_RADIO3);
		pButton -> SetWindowText(strLoad);
		GetPrivateProfileString(LANG_STRING, _T("DIALOG_OK"),
			_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
		strLoad = lpszLang;
		pButton = (CButton *) GetDlgItem(IDOK);
		pButton -> SetWindowText(strLoad);
		GetPrivateProfileString(LANG_STRING, _T("DIALOG_CANCEL"),
			_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
		strLoad = lpszLang;
		pButton = (CButton *) GetDlgItem(IDCANCEL);
		pButton -> SetWindowText(strLoad);
		// ��̬�ı�
		CStatic *pStatic = NULL;
		GetPrivateProfileString(LANG_ADDITEM_DLG, _T("STATIC_TITLE"),
			_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
		strLoad = lpszLang;
		pStatic = (CStatic *) GetDlgItem(IDC_STATIC_TITLE);
		pStatic -> SetWindowText(strLoad);
		GetPrivateProfileString(LANG_ADDITEM_DLG, _T("STATIC_CLASS"),
			_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
		strLoad = lpszLang;
		pStatic = (CStatic *) GetDlgItem(IDC_STATIC_CLASS);
		pStatic -> SetWindowText(strLoad);
		GetPrivateProfileString(LANG_ADDITEM_DLG, _T("STATIC_LANGUAGE"),
			_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
		strLoad = lpszLang;
		pStatic = (CStatic *) GetDlgItem(IDC_STATIC_LANG);
		pStatic -> SetWindowText(strLoad);
		GetPrivateProfileString(LANG_ADDITEM_DLG, _T("STATIC_SOURCE"),
			_T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
		strLoad = lpszLang;
		pStatic = (CStatic *) GetDlgItem(IDC_STATIC_SOURCE);
		pStatic -> SetWindowText(strLoad);
		delete []lpszLang;
		SetCurrentDirectory(m_strWorkPath);
	}

	// ����Ϊͼ���б��ʼ������
	m_wndImageList.Create(16, 16, ILC_COLOR24|ILC_MASK, 4, 4);						// ����ͼ���б�
	CBitmap bm;
	AddBitmap(bm, IDB_BITMAP1);
	AddBitmap(bm, IDB_BITMAP2);
	for (UINT nId = IDB_BITMAP5; nId <= IDB_BITMAP18; nId++)
		AddBitmap(bm, nId);
	bm.Detach();

	// Ϊ��Ͽ����ͼ���б�
	m_cboLanguage.SetImageList(&m_wndImageList);
	// Ϊ��Ͽ������Ŀ
	for(int i=0; i<8; i++)
	{
		COMBOBOXEXITEM     cbi ={0};
		CString            str;
		int                nItem;

		cbi.mask = CBEIF_IMAGE | CBEIF_INDENT | CBEIF_OVERLAY |
			CBEIF_SELECTEDIMAGE | CBEIF_TEXT;
		cbi.iItem = i;
		switch(i)
		{
		case 0:
			{
				cbi.iImage = 1;
				cbi.iSelectedImage = 0;
				str=_T("C++");
			}
			break;
		case 1:
			{
				cbi.iImage = 3;
				cbi.iSelectedImage = 2;
				str=_T("ANSI C");
			}
			break;
		case 2:
			{
				cbi.iImage = 15;
				cbi.iSelectedImage = 14;
				str=_T("C#.NET");
			}
			break;
		case 3:
			{
				cbi.iImage = 5;
				cbi.iSelectedImage = 4;
				str=_T("HTML");
			}
			break;
		case 4:
			{
				cbi.iImage = 7;
				cbi.iSelectedImage = 6;
				str=_T("Java Script");
			}
			break;
		case 5:
			{
				cbi.iImage = 13;
				cbi.iSelectedImage = 12;
				str=_T("VB Script");
			}
			break;
		case 6:
			{
				cbi.iImage = 9;
				cbi.iSelectedImage = 8;
				str=_T("SQL");
			}
			break;
		case 7:
			{
				cbi.iImage = 11;
				cbi.iSelectedImage = 10;
				LPWSTR lpszLang = new WCHAR[LANG_STRING_LEN];
				if (m_uiLanguage == 0)
					str.LoadString(IDS_NORMAL_TEXT);
				else if (m_uiLanguage == 1)
				{
					SetCurrentDirectory(m_strAppPath);
					GetPrivateProfileString(LANG_STRING, _T("NORMAL_TEXT"),
						_T(""), lpszLang, LANG_STRING_LEN, LANG_TCHINESE_FILE);
					str = lpszLang;
					SetCurrentDirectory(m_strWorkPath);
				}
				else if (m_uiLanguage == 2)
				{
					SetCurrentDirectory(m_strAppPath);
					GetPrivateProfileString(LANG_STRING, _T("NORMAL_TEXT"),
						_T(""), lpszLang, LANG_STRING_LEN, LANG_ENGLISH_FILE);
					str = lpszLang;
					SetCurrentDirectory(m_strWorkPath);
				}
				delete []lpszLang;
			}
			break;
		}
		cbi.pszText = (LPTSTR)(LPCTSTR)str;
		cbi.cchTextMax = str.GetLength();
		cbi.iOverlay = 2;
		cbi.iIndent = 0;
		nItem = m_cboLanguage.InsertItem(&cbi);
	}
	// ������Ͽ�Ĭ��ѡ����Ŀ
	m_cboLanguage.SetCurSel(m_uiLangType);
	m_bSaveAsDefault = false;
	// ѡ�е�ѡť
	CButton * p1=(CButton *) GetDlgItem(IDC_RADIO1);
	p1->SetCheck(1);
	p1=NULL;
	delete p1;
	return TRUE;  // ���������˿ؼ��Ľ��㣬���򷵻� TRUE
}

void CAddItemDlg::AddBitmap(CBitmap &bm, UINT uiBmpId)
{
	bm.LoadBitmap(uiBmpId);									// ����Bitmap��Դ
	m_wndImageList.Add(&bm, NORMAL_BG_COLOR);				// ��ӵ�ͼ���б�
	bm.DeleteObject();
}

void CAddItemDlg::OnBnClickedChangeClass()
{
	CMoveToDlg dlgMoveTo;
	// ͨ���ƶ�������Ի�����ķ�����Ϣ
	dlgMoveTo.FullPath = m_strFullPath;
	dlgMoveTo.m_Path = m_strClass;
	dlgMoveTo.m_uiLanguage = m_uiLanguage;
	dlgMoveTo.m_strAppPath = m_strAppPath;
	dlgMoveTo.m_strWorkPath = m_strWorkPath;
	if (dlgMoveTo.DoModal()==IDOK)
	{
		if (m_strFullPath == dlgMoveTo.FullPath)
			m_bReConfirm = false;
		else
			m_bReConfirm = true;
		m_strFullPath = dlgMoveTo.FullPath;
		m_strClass = dlgMoveTo.m_Path;
		// �ӱ������µ��ؼ�
		UpdateData(FALSE);
	}
}

void CAddItemDlg::OnBnClickedSetDefault()
{
	// ���ø���Ĭ�����͵ı�־
	m_bSaveAsDefault = true;
}

void CAddItemDlg::OnBnClickedOk()
{
	// ���¿ؼ���Ϣ������
	UpdateData(TRUE);
	CString strTitle,strContent;
	if (m_strTitle.IsEmpty())	// ��������Ϊ��
	{
		if (m_uiLanguage == 0)
		{
			strTitle.LoadString(IDS_MSGBOX_WARN);
			strContent.LoadString(IDS_MSGBOX_NOTEMPTY);
		}
		else
		{
			SetCurrentDirectory(m_strAppPath);
			LPWSTR lpszFileName = NULL;
			if (m_uiLanguage == 1)
				lpszFileName = LANG_TCHINESE_FILE;
			else if (m_uiLanguage == 2)
				lpszFileName = LANG_ENGLISH_FILE;
			LPWSTR lpszLang = new WCHAR[LANG_STRING_LEN];
			GetPrivateProfileString(LANG_MESSAGEBOX, _T("CONTENT_TITLE_EMPTY"), _T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
			strContent = lpszLang;
			strContent.Replace(_T("\\n"), _T("\n"));
			GetPrivateProfileString(LANG_MESSAGEBOX, _T("TITLE_WARNING"), _T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
			strTitle = lpszLang;
			delete []lpszLang;
			SetCurrentDirectory(m_strWorkPath);
		}
		MessageBox(strContent, strTitle, MB_ICONWARNING);
		CEdit * p1=(CEdit *) GetDlgItem(IDC_EDIT_TITLE);
		p1->SetFocus();
		p1=NULL;
		delete p1;
		return;
	}
	else	// ���������
	{
		int iResult;
		iResult = IsValidFileName(m_strTitle);
		if (iResult != 0)	// ���ⲻ�����ļ���������
		{
			if (m_uiLanguage == 0)
			{
				strContent.LoadString(IDS_MSGBOX_EXITWARN);
				strTitle.LoadString(IDS_MSGBOX_QUES);
			}
			else
			{
				SetCurrentDirectory(m_strAppPath);
				LPWSTR lpszFileName = NULL;
				if (m_uiLanguage == 1)
					lpszFileName = LANG_TCHINESE_FILE;
				else if (m_uiLanguage == 2)
					lpszFileName = LANG_ENGLISH_FILE;
				LPWSTR lpszLang = new WCHAR[LANG_STRING_LEN];
				GetPrivateProfileString(LANG_MESSAGEBOX, _T("CONTENT_INVILID_NAME"), _T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
				strContent = lpszLang;
				strContent.Replace(_T("\\n"), _T("\n"));
				GetPrivateProfileString(LANG_MESSAGEBOX, _T("TITLE_WARNING"), _T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
				strTitle = lpszLang;
				delete []lpszLang;
				SetCurrentDirectory(m_strWorkPath);
			}
			MessageBox(strContent, strTitle, MB_ICONWARNING);
			CEdit * p1=(CEdit *) GetDlgItem(IDC_EDIT_TITLE);
			p1->SetFocus();
			p1=NULL;
			delete p1;
			return;
		}
	}
	// ���ȫ·��
	CString filepath = m_strFullPath + _T("\\") + m_strTitle;
	// �ж�Ҫ�������ļ�����
	m_uiLangType = m_cboLanguage.GetCurSel();
	switch(m_uiLangType)
	{
	case 0:
			filepath += _T(".c-p");
		break;
	case 1:
			filepath += _T(".c-a");
		break;
	case 2:
			filepath += _T(".h-t");
		break;
	case 3:
			filepath += _T(".j-s");
		break;
	case 4:
			filepath += _T(".s-q");
		break;
	case 5:
			filepath += _T(".txt");
		break;
	}
	CFileFind filefind;
	if(filefind.FindFile(filepath))	// �ļ��Ѿ�����
	{
		if (m_uiLanguage == 0)
		{
			strTitle.LoadString(IDS_MSGBOX_WARN);
			strContent.LoadString(IDS_MSGBOX_FILEEXIST);
		}
		else
		{
			SetCurrentDirectory(m_strAppPath);
			LPWSTR lpszFileName = NULL;
			if (m_uiLanguage == 1)
				lpszFileName = LANG_TCHINESE_FILE;
			else if (m_uiLanguage == 2)
				lpszFileName = LANG_ENGLISH_FILE;
			LPWSTR lpszLang = new WCHAR[LANG_STRING_LEN];
			GetPrivateProfileString(LANG_MESSAGEBOX, _T("CONTENT_FILE_EXIST"), _T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
			strContent = lpszLang;
			strContent.Replace(_T("\\n"), _T("\n"));
			GetPrivateProfileString(LANG_MESSAGEBOX, _T("TITLE_WARNING"), _T(""), lpszLang, LANG_STRING_LEN, lpszFileName);
			strTitle = lpszLang;
			delete []lpszLang;
			SetCurrentDirectory(m_strWorkPath);
		}
		MessageBox(strContent, strTitle, MB_ICONWARNING);
		CEdit * p1=(CEdit *) GetDlgItem(IDC_EDIT_TITLE);
		p1->SetFocus();
		p1=NULL;
		delete p1;
		return;
	}
	// �жϴ����ķ�ʽ
	CButton * p1=(CButton *) GetDlgItem(IDC_RADIO1);
	m_uiSourceType = p1->GetCheck();
	if (m_uiSourceType == 0)
	{
		p1=(CButton *) GetDlgItem(IDC_RADIO2);
		m_uiSourceType = p1->GetCheck();
		if (m_uiSourceType == 0)
			m_uiSourceType = 2;
		else
			m_uiSourceType = 1;
	}
	else
		m_uiSourceType = 0;
	p1=NULL;
	delete p1;
	OnOK();
}