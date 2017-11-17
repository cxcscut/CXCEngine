#pragma once
#include "5DT\inc\fglove.h"
#include "afxwin.h"

// CGloveDlg 对话框

class CGloveDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CGloveDlg)

public:
	CGloveDlg(CWnd* pParent = NULL);   // 标准构造函数
	bool UpdateInputs();
	fdGlove *m_pGlove;
	CListBox	m_ComList;
	CListBox	m_ComList_gesture;
	CCheckListBox m_check;
	CString iGesture;
	int gesture = -1;
	float angle[14];//存储角度值
	bool isSynchronize = false;//角度同步
	virtual ~CGloveDlg();
	virtual BOOL OnInitDialog();
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_RobotControl };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLbnSelchangeList2();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSendAngle();
	afx_msg void OnClickedSynchronize();
	afx_msg void OnBnClickedOk2();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
	CButton m_SynAngleCtrl;
};
