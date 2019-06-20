
// ServerView.cpp : CServerView 클래스의 구현
//

#include "stdafx.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
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

// CServerView 생성/소멸
BOOL CServerView::m_sw1 = FALSE;
int CServerView::color = 0;
CServerView::CServerView()
	: CFormView(IDD_SERVER_FORM)
{
	// TODO: 여기에 생성 코드를 추가합니다.

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
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CFormView::PreCreateWindow(cs);
}

void CServerView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	// 버튼 컨트롤을 초기화한다.
	m_radio_red.SetCheck(1);
	m_radio_green.SetCheck(0);
	m_radio_blue.SetCheck(0);

    m_pDataSocket = NULL;
    

}


// CServerView 진단

#ifdef _DEBUG
void CServerView::AssertValid() const
{
	CFormView::AssertValid();
}

void CServerView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CServerDoc* CServerView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CServerDoc)));
	return (CServerDoc*)m_pDocument;
}
#endif //_DEBUG


// CServerView 메시지 처리기


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
	str.Format(_T("사람을 탐지합니다"));
	MessageBox(str, _T("사람 탐지"));
}


void CServerView::OnBnClickedNamusi()
{
	m_sw1 = 1;
	CString str;
	str.Format(_T("사물을 탐지합니다"));
	MessageBox(str, _T("사물 탐지"));
}


void CServerView::OnBnClickedCar()
{
	m_sw1 = 2;
	CString str;
	str.Format(_T("자동차를 탐지합니다"));
	MessageBox(str, _T("자동차 탐지"));
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
