// ListenSocket.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "Server.h"
#include "ListenSocket.h"
#include "ServerDoc.h"


// CListenSocket

CListenSocket::CListenSocket(CServerDoc *pDoc)
{
    m_pDoc = pDoc;
}

CListenSocket::~CListenSocket()
{
}


// CListenSocket ��� �Լ�


void CListenSocket::OnAccept(int nErrorCode)
{
   
    CSocket::OnAccept(nErrorCode);
    m_pDoc->ProcessAccept(nErrorCode);
}
