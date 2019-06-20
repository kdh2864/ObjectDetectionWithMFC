
// ServerView.h : CServerView 클래스의 인터페이스
//

#pragma once
#include "afxwin.h"
class CDataSocket;
class CServerDoc;
class CServerView : public CFormView
{
protected: // serialization에서만 만들어집니다.
	CServerView();
	DECLARE_DYNCREATE(CServerView)

public:
#ifdef AFX_DESIGN_TIME
	enum{ IDD = IDD_SERVER_FORM };
#endif

// 특성입니다.
public:
	CServerDoc* GetDocument() const;
    CDataSocket *m_pDataSocket;
	static BOOL m_sw1;
	static int color;


// 작업입니다.
public:

// 재정의입니다.
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual void OnInitialUpdate(); // 생성 후 처음 호출되었습니다.

// 구현입니다.
public:
	virtual ~CServerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()
public:
    void AddMessage(LPCTSTR message);
    CEdit m_edit;
    afx_msg void OnBnClickedPerson();
    CButton m_person;
//	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedNamusi();
	afx_msg void OnBnClickedCar();
	CButton m_radio_red;
	CButton m_radio_green;
	CButton m_radio_blue;
	afx_msg void OnBnClickedButtonSet();
};

#ifndef _DEBUG  // ServerView.cpp의 디버그 버전
inline CServerDoc* CServerView::GetDocument() const
   { return reinterpret_cast<CServerDoc*>(m_pDocument); }
#endif

