
// Image_cut.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CImage_cutApp:
// �йش����ʵ�֣������ Image_cut.cpp
//

class CImage_cutApp : public CWinApp
{
public:
	CImage_cutApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CImage_cutApp theApp;