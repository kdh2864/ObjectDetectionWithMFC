
// ServerView.cpp : CServerView Ŭ������ ����
//

#include "stdafx.h"
// SHARED_HANDLERS�� �̸� ����, ����� �׸� �� �˻� ���� ó���⸦ �����ϴ� ATL ������Ʈ���� ������ �� ������
// �ش� ������Ʈ�� ���� �ڵ带 �����ϵ��� �� �ݴϴ�.
#ifndef SHARED_HANDLERS
#include "Server.h"
#endif

#include "ServerDoc.h"
#include "ServerView.h"
#include "DataSocket.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CServerView

IMPLEMENT_DYNCREATE(CServerView, CFormView)

BEGIN_MESSAGE_MAP(CServerView, CFormView)
    ON_BN_CLICKED(IDC_PERSON, &CServerView::OnBnClickedPerson)
//	ON_WM_LBUTTONDOWN()
ON_BN_CLICKED(IDC_NAMUSI, &CServerView::OnBnClickedNamusi)
ON_BN_CLICKED(IDC_CAR, &CServerView::OnBnClickedCar)
ON_BN_CLICKED(IDC_BUTTON_SET, &CServerView::OnBnClickedButtonSet)
END_MESSAGE_MAP()

// CServerView ����/�Ҹ�
BOOL CServerView::m_sw1 = FALSE;
int CServerView::color = 0;
CServerView::CServerView()
	: CFormView(IDD_SERVER_FORM)
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.

}

CServerView::~CServerView()
{
}

void CServerView::DoDataExchange(CDataExchange* pDX)
{
    CFormView::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_MYEDIT, m_edit);
    DDX_Control(pDX, IDC_PERSON, m_person);

	DDX_Control(pDX, IDC_RADIO_RED, m_radio_red);
	DDX_Control(pDX, IDC_RADIO_GREEN, m_radio_green);
	DDX_Control(pDX, IDC_RADIO_BLUE, m_radio_blue);
}

BOOL CServerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	//  Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.

	return CFormView::PreCreateWindow(cs);
}

void CServerView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	// ��ư ��Ʈ���� �ʱ�ȭ�Ѵ�.
	m_radio_red.SetCheck(1);
	m_radio_green.SetCheck(0);
	m_radio_blue.SetCheck(0);

    m_pDataSocket = NULL;
    

}


// CServerView ����

#ifdef _DEBUG
void CServerView::AssertValid() const
{
	CFormView::AssertValid();
}

void CServerView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CServerDoc* CServerView::GetDocument() const // ����׵��� ���� ������ �ζ������� �����˴ϴ�.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CServerDoc)));
	return (CServerDoc*)m_pDocument;
}
#endif //_DEBUG


// CServerView �޽��� ó����


void CServerView::AddMessage(LPCTSTR message)
{
    int len = GetWindowTextLength();
    m_edit.SetSel(len, len);
    m_edit.ReplaceSel(message);
}


void CServerView::OnBnClickedPerson()
{
	m_sw1 = 0;
	CString str;
	str.Format(_T("����� Ž���մϴ�"));
	MessageBox(str, _T("��� Ž��"));
}


void CServerView::OnBnClickedNamusi()
{
	m_sw1 = 1;
	CString str;
	str.Format(_T("�繰�� Ž���մϴ�"));
	MessageBox(str, _T("�繰 Ž��"));
}


void CServerView::OnBnClickedCar()
{
	m_sw1 = 2;
	CString str;
	str.Format(_T("�ڵ����� Ž���մϴ�"));
	MessageBox(str, _T("�ڵ��� Ž��"));
}


void CServerView::OnBnClickedButtonSet()
{
	int state_radio_red = m_radio_red.GetCheck();
	int state_radio_green = m_radio_green.GetCheck();
	int state_radio_blue = m_radio_blue.GetCheck();
	
	if (state_radio_red == 1)
		color = 0;
	else if (state_radio_green == 1)
		color = 1;
	else if (state_radio_blue == 1)
		color = 2;
}
