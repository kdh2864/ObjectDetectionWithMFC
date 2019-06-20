
// ServerView.h : CServerView Ŭ������ �������̽�
//

#pragma once
#include "afxwin.h"
class CDataSocket;
class CServerDoc;
class CServerView : public CFormView
{
protected: // serialization������ ��������ϴ�.
	CServerView();
	DECLARE_DYNCREATE(CServerView)

public:
#ifdef AFX_DESIGN_TIME
	enum{ IDD = IDD_SERVER_FORM };
#endif

// Ư���Դϴ�.
public:
	CServerDoc* GetDocument() const;
    CDataSocket *m_pDataSocket;
	static BOOL m_sw1;
	static int color;


// �۾��Դϴ�.
public:

// �������Դϴ�.
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
	virtual void OnInitialUpdate(); // ���� �� ó�� ȣ��Ǿ����ϴ�.

// �����Դϴ�.
public:
	virtual ~CServerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ������ �޽��� �� �Լ�
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

#ifndef _DEBUG  // ServerView.cpp�� ����� ����
inline CServerDoc* CServerView::GetDocument() const
   { return reinterpret_cast<CServerDoc*>(m_pDocument); }
#endif

