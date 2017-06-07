
// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // �� Windows ͷ���ų�����ʹ�õ�����
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // ĳЩ CString ���캯��������ʽ��

// �ر� MFC ��ĳЩ�����������ɷ��ĺ��Եľ�����Ϣ������
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC ��������ͱ�׼���
#include <afxext.h>         // MFC ��չ


#include <afxdisp.h>        // MFC �Զ�����



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC �� Internet Explorer 4 �����ؼ���֧��
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC �� Windows �����ؼ���֧��
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // �������Ϳؼ����� MFC ֧��

#include "SkinPPWTL.h"

#define CONFIG_FILE_NAME	_T(".\\Config.ini")
#define NAME_MAIN_FRAME		_T("Main Frame")
#define NAME_USER_CONFIG	_T("User Config")

#define NORMAL_BG_COLOR		RGB(192, 192, 192)

#define LANG_STRING_LEN		256

#define LANG_TCHINESE_FILE	_T(".\\TChinese.lng")
#define LANG_ENGLISH_FILE	_T(".\\English.lng")
#define LANG_APP_TITLE		_T("Code Mate")
#define LANG_FILE_MENU		_T("File Menu")
#define LANG_EDIT_MENU		_T("Edit Menu")
#define LANG_SETTING_MENU	_T("Setting Menu")
#define LANG_SKIN_MENU		_T("Skin Menu")
#define LANG_HELP_MENU		_T("Help Menu")
#define LANG_TOOLBAR		_T("Tool Bar")
#define LANG_STRING			_T("String")
#define LANG_SEARCH_AREA	_T("Search Area")
#define LANG_ADDITEM_DLG	_T("Add Item Dialog")
#define LANG_CHTYPE_DLG		_T("Change Type Dialog")
#define LANG_CONFIG_DLG		_T("Config Dialog")
#define LANG_ATTRIB_DLG		_T("Attribute Dialog")
#define LANG_MOVETO_DLG		_T("Move To Dialog")
#define LANG_GOTO_DLG		_T("Go To Dialog")
#define LANG_MESSAGEBOX		_T("Message Box")

#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


