#pragma once

// CDataSocket ��� ����Դϴ�.
class CServerDoc;
class CDataSocket : public CSocket
{
public:
	CDataSocket(CServerDoc *pDoc);
	virtual ~CDataSocket();
    CServerDoc *m_pDoc;

    virtual void OnReceive(int nErrorCode);
    virtual void OnClose(int nErrorCode);
};


