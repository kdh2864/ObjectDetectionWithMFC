// ListenSocket.cpp : 구현 파일입니다.
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


// CListenSocket 멤버 함수


void CListenSocket::OnAccept(int nErrorCode)
{
   
    CSocket::OnAccept(nErrorCode);
    m_pDoc->ProcessAccept(nErrorCode);
}
