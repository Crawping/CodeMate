
// CodeMate.h : CodeMate Ӧ�ó������ͷ�ļ�
// 

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CCodeMateApp:
// �йش����ʵ�֣������ CodeMate.cpp
// 

class CCodeMateApp : public CWinApp
{
public:
	CCodeMateApp();

// ��д
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CCodeMateApp theApp;