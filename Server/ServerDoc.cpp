
// ServerDoc.cpp : CServerDoc Ŭ������ ����
//

#include "stdafx.h"
// SHARED_HANDLERS�� �̸� ����, ����� �׸� �� �˻� ���� ó���⸦ �����ϴ� ATL ������Ʈ���� ������ �� ������
// �ش� ������Ʈ�� ���� �ڵ带 �����ϵ��� �� �ݴϴ�.
#ifndef SHARED_HANDLERS
#include "Server.h"
#endif

#include "ServerDoc.h"
#include "ServerView.h"
#include "ListenSocket.h"
#include "DataSocket.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CServerDoc
IMPLEMENT_DYNCREATE(CServerDoc, CDocument)

BEGIN_MESSAGE_MAP(CServerDoc, CDocument)
END_MESSAGE_MAP()

int CServerDoc::send_nbytes = 0;

void ErrQuit(int err)
{
    LPVOID lpMsgBuf;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, err,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf, 0, NULL);
    MessageBox(NULL, (LPCTSTR)lpMsgBuf, _T("���� �߻�"), MB_ICONERROR);
    LocalFree(lpMsgBuf);
    exit(1);
}
// CServerDoc ����/�Ҹ�

CServerDoc::CServerDoc()
{
    m_pListenSocket = NULL;
    m_pDataSocket = NULL;
}

CServerDoc::~CServerDoc()
{
    if (m_pListenSocket != NULL)
        delete m_pListenSocket;
    if (m_pDataSocket != NULL)
        delete m_pDataSocket;
}

BOOL CServerDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

    ASSERT(m_pListenSocket == NULL);
    m_pListenSocket = new CListenSocket(this);
    if (m_pListenSocket->Create(8000)) {
        if (m_pListenSocket->Listen()) {
            AfxMessageBox(_T("������ �����մϴ�."), MB_ICONINFORMATION);
            return TRUE;
        }
    }

    AfxMessageBox(_T("�̹� ���� ���� ������ �ֽ��ϴ�.") 
        _T("\n���α׷��� �����մϴ�."), MB_ICONERROR);

	return FALSE;
}




// CServerDoc serialization

void CServerDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	}
	else
	{
		// TODO: ���⿡ �ε� �ڵ带 �߰��մϴ�.
	}
}

#ifdef SHARED_HANDLERS

// ����� �׸��� �����մϴ�.
void CServerDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// ������ �����͸� �׸����� �� �ڵ带 �����Ͻʽÿ�.
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// �˻� ó���⸦ �����մϴ�.
void CServerDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// ������ �����Ϳ��� �˻� �������� �����մϴ�.
	// ������ �κ��� ";"�� ���еǾ�� �մϴ�.

	// ��: strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CServerDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CServerDoc ����

#ifdef _DEBUG
void CServerDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CServerDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CServerDoc ���


void CServerDoc::DeleteContents()
{
    if (m_pListenSocket != NULL) {
        delete m_pListenSocket;
        m_pListenSocket = NULL;
    }
    if (m_pDataSocket != NULL) {
        delete m_pDataSocket;
        m_pDataSocket = NULL;
    }
    CDocument::DeleteContents();
}


void CServerDoc::ProcessAccept(int nErrorCode)
{
    CString PeerAddr;
    UINT PeerPort;
    CString str;

    ASSERT(nErrorCode == 0);
    if (m_pDataSocket == NULL) {
        m_pDataSocket = new CDataSocket(this);
        if (m_pListenSocket->Accept(*m_pDataSocket)) {
            m_pDataSocket->GetPeerName(PeerAddr, PeerPort);
            str.Format(_T("### IP �ּ�:%s, ��Ʈ ��ȣ: %d ###\r\n"),
                PeerAddr, PeerPort);
            PrintMessage(str);
        }
        else {
            delete m_pDataSocket;
            m_pDataSocket = NULL;
        }
    }
}


void CServerDoc::PrintMessage(LPCTSTR message)
{
    CFrameWnd *pMainWnd = (CFrameWnd *)AfxGetMainWnd();
    CServerView *pView = (CServerView *)pMainWnd->GetActiveView();
    pView->AddMessage(message);
}


void CServerDoc::ProcessReceive(CDataSocket *pSocket, int nErrorCode)
{
    TCHAR buf[256 + 1];
    int nbytes = pSocket->Receive(buf, 256);
    buf[nbytes] = _T('\0');
    PrintMessage(buf);

	
    TCHAR send_buf[256];
	wsprintf(send_buf, _T("**"));
	PrintMessage(send_buf);
	if (CServerView::m_sw1 == 0) {		// ��� ��ư //
		if (CServerView::color == 0)		//  red
			send_nbytes = pSocket->Send(send_buf, 21);
		else if (CServerView::color == 1)	//green
			send_nbytes = pSocket->Send(send_buf, 22);
		else if (CServerView::color == 2)	//blue
			send_nbytes = pSocket->Send(send_buf, 23);
	}

	else if (CServerView::m_sw1 == 1) {	// ������ ��ư //
		if (CServerView::color == 0)		//  red
			send_nbytes = pSocket->Send(send_buf, 24);
		else if (CServerView::color == 1)	//green
			send_nbytes = pSocket->Send(send_buf, 25);
		else if (CServerView::color == 2)	//blue
			send_nbytes = pSocket->Send(send_buf, 26);
	}

	else if (CServerView::m_sw1 == 2) {	// �ڵ��� ��ư //
		if (CServerView::color == 0)		//  red
			send_nbytes = pSocket->Send(send_buf, 27);
		else if (CServerView::color == 1)	//green
			send_nbytes = pSocket->Send(send_buf, 28);
		else if (CServerView::color == 2)	//blue
			send_nbytes = pSocket->Send(send_buf, 29);
	}

    if (send_nbytes == SOCKET_ERROR)
        ErrQuit(pSocket->GetLastError());
    else
    {
        _tprintf(_T("�������� %d ����Ʈ ����\n"), send_nbytes);
        Sleep(40);
    }

}


void CServerDoc::ProcessClose(CDataSocket *pSocket, int nErrorCode)
{
    pSocket->Close();
    delete m_pDataSocket;
    m_pDataSocket = NULL;
    PrintMessage(_T("### ���� ���� ###\r\n\r\n"));
}


