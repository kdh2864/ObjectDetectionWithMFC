
// ServerDoc.h : CServerDoc Ŭ������ �������̽�
//


#pragma once

class CListenSocket;
class CDataSocket;
class ServerView;

class CServerDoc : public CDocument
{
protected: // serialization������ ��������ϴ�.
	CServerDoc();
	DECLARE_DYNCREATE(CServerDoc)

// Ư���Դϴ�.
public:
    CListenSocket *m_pListenSocket;
    CDataSocket *m_pDataSocket;
	ServerView *m_pServerView;
	static int send_nbytes;

// �۾��Դϴ�.
public:

// �������Դϴ�.
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// �����Դϴ�.
public:
	virtual ~CServerDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ������ �޽��� �� �Լ�
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// �˻� ó���⿡ ���� �˻� �������� �����ϴ� ����� �Լ�
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
public:
    virtual void DeleteContents();
    void ProcessAccept(int nErrorCode);
    void PrintMessage(LPCTSTR message);
    void ProcessReceive(CDataSocket * pSocket, int nErrorCode);
    void ProcessClose(CDataSocket * pSocket, int nErrorCode);
};
