
// Server.h : Server ���� ���α׷��� ���� �� ��� ����
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"       // �� ��ȣ�Դϴ�.


// CServerApp:
// �� Ŭ������ ������ ���ؼ��� Server.cpp�� �����Ͻʽÿ�.
//

class CServerApp : public CWinApp
{
public:
	CServerApp();


// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
    void PrintMessage(LPCTSTR message);
};

extern CServerApp theApp;
