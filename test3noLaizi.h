// test3noLaizi.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error �ڰ������� PCH �Ĵ��ļ�֮ǰ������stdafx.h��
#endif

#include "resource.h"		// ������


// Ctest3noLaiziApp:
// �йش����ʵ�֣������ test3noLaizi.cpp
//

class Ctest3noLaiziApp : public CWinApp
{
public:
	Ctest3noLaiziApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern Ctest3noLaiziApp theApp;
