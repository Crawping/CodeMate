
// CodeMate.cpp : ����Ӧ�ó��������Ϊ��
// 

#include "stdafx.h"
#include "CodeMate.h"
#include "CodeMateDlg.h"

#pragma comment(lib, "SkinPPWTL.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define defWinUniqueMutex	_T("{CodeMate_LiuXindian_201110211500}")
#define defWinClass			_T("CodeMateClass")

// CCodeMateApp

BEGIN_MESSAGE_MAP(CCodeMateApp, CWinApp)
END_MESSAGE_MAP()


// CCodeMateApp ����

CCodeMateApp::CCodeMateApp()
{
	// ֧����������������
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;

	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CCodeMateApp ����

CCodeMateApp theApp;


// CCodeMateApp ��ʼ��

BOOL CCodeMateApp::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	// ����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	WNDCLASS wc;
	// ��ȡ��������Ϣ��MFC Ĭ�ϵ����жԻ���Ĵ�������Ϊ #32770 
	::GetClassInfo(AfxGetInstanceHandle(), _T("#32770"), &wc);
	// �ı䴰������ 
	wc.lpszClassName = defWinClass;
	// ע���´����࣬ʹ������ʹ���� 
	AfxRegisterClass(&wc);

	CWinApp::InitInstance();

	bool bSkinLoad = false;
	LPTSTR lpszWords = new _TCHAR[LANG_STRING_LEN];
	GetPrivateProfileString(NAME_MAIN_FRAME, _T("Skin"), _T("AquaOS.ssk"),
		lpszWords, LANG_STRING_LEN, CONFIG_FILE_NAME);

	CString strSkin;
	strSkin.Format(_T("%s"), lpszWords);
	delete []lpszWords;
	if (strSkin.IsEmpty())
	{
		bSkinLoad = (skinppLoadSkin("skins\\AquaOS.ssk") != FALSE);
		if (!bSkinLoad)
			AfxMessageBox(_T("����Ƥ���ļ� AquaOS.ssk ʧ�ܣ�"));
	}
	else
	{
		if (strSkin != _T("<ϵͳ����>"))
		{
			CString PathName = _T("Skins\\") + strSkin;
			char *pText = NULL;
#ifdef _UNICODE
			int n = PathName.GetLength();
			int len = WideCharToMultiByte(CP_ACP,0,PathName,n,NULL,0,NULL,NULL);
			pText = new char[len+1];
			WideCharToMultiByte(CP_ACP,0,PathName,n,pText,len,NULL,NULL);
			pText[len] = '\0';
#else
			int len = PathName.GetLength();
			pText = new char[len+1];
			strcpy_s(pText, len+1, PathName.GetBuffer(0));
			pText[len] = '\0';
#endif
			bSkinLoad = (skinppLoadSkin(pText) != FALSE);
			if (!bSkinLoad)
				AfxMessageBox(_T("����Ƥ���ļ� ") + strSkin + _T(" ʧ�ܣ�"));

			delete []pText;
		}
	}

	AfxEnableControlContainer();

	// ʹ�� Mutex �ó���ֻ������һ��ʵ��
	HANDLE hMutex;
	hMutex = CreateMutex(NULL, true, defWinUniqueMutex);
	if(hMutex)
	{
		HWND hWnd = NULL;
		if(ERROR_ALREADY_EXISTS == GetLastError())
		{
			hWnd = ::FindWindow(defWinClass, NULL);
			if (hWnd != NULL)
			{
				if (::IsIconic(hWnd))
					::ShowWindow(hWnd, SW_RESTORE);

				hWnd = ::GetLastActivePopup(hWnd);
				::SwitchToThisWindow(hWnd, TRUE);
			}
			return FALSE;
		}
	}
	AfxEnableControlContainer();

	// ���� shell ���������Է��Ի������
	// �κ� shell ����ͼ�ؼ��� shell �б���ͼ�ؼ���
	CShellManager *pShellManager = new CShellManager;

	CCodeMateDlg dlgMain;
	dlgMain.m_bSkinLoad = bSkinLoad;
	m_pMainWnd = &dlgMain;
	INT_PTR nResponse = dlgMain.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �ڴ˷��ô����ʱ��
		// ��ȷ�������رնԻ���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �ڴ˷��ô����ʱ��
		// ��ȡ�������رնԻ���Ĵ���
	}


	// ɾ�����洴���� shell ��������
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}


	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	// ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}

int CCodeMateApp::ExitInstance()
{
	skinppExitSkin();
	return 0;
}