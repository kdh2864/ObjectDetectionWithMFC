#pragma once

// CListenSocket ��� ����Դϴ�.
class CServerDoc;

class CListenSocket : public CSocket
{
public:
	CListenSocket(CServerDoc *pDoc);
	virtual ~CListenSocket();
    CServerDoc *m_pDoc;
    virtual void OnAccept(int nErrorCode);
};


