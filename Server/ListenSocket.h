#pragma once

// CListenSocket 명령 대상입니다.
class CServerDoc;

class CListenSocket : public CSocket
{
public:
	CListenSocket(CServerDoc *pDoc);
	virtual ~CListenSocket();
    CServerDoc *m_pDoc;
    virtual void OnAccept(int nErrorCode);
};


