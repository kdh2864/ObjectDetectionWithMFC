// DataSocket.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "Server.h"
#include "DataSocket.h"
#include "ServerDoc.h"


// CDataSocket

CDataSocket::CDataSocket(CServerDoc *pDoc)
{
    m_pDoc = pDoc;
}

CDataSocket::~CDataSocket()
{
}


// CDataSocket ��� �Լ�


void CDataSocket::OnReceive(int nErrorCode)
{
    CSocket::OnReceive(nErrorCode);
    m_pDoc->ProcessReceive(this, nErrorCode);
    
}


void CDataSocket::OnClose(int nErrorCode)
{
    // TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

    CSocket::OnClose(nErrorCode);
    m_pDoc->ProcessClose(this, nErrorCode);

}
