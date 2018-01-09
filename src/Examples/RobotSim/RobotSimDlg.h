
// RobotSimDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "GloveDlg.h"
#include "KinectWin.h"

#define WM_MYMSG  WM_USER+200

using namespace cxc;

// CRobotSimDlg 对话框
class CRobotSimDlg : public CDialogEx
{
// 构造
public:
	CRobotSimDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ROBOTSIM_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// Inverse name mapping
	std::unordered_map<std::string, std::string> m_InvNameMap;

	// Robothand Ptr
	std::shared_ptr<Robothand> m_LeftPtr, m_RightPtr;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	void OutputInfo(const CString &str);
	void LoadCombobox();
	void LoadListBox();
	std::unique_ptr<CGloveDlg> glove_dlg;
	std::unique_ptr<KinectWin> kinect_dlg;
	CStatic m_PicCtrl;
	CString m_Output;
	CString m_LeftCombo;
	CString m_RightCombo;
	CComboBox m_LeftComboxCtrl;
	CComboBox m_RightComboxCtrl;
	afx_msg void OnBnClickedLeftmotion();
	CString m_LeftAngle;
	CString m_RightAngle;
	afx_msg void OnBnClickedLeftreset();
	afx_msg void OnBnClickedLeftresetall();
	CEdit m_OutputCtrl;
	afx_msg void OnBnClickedRightmotion();
	afx_msg void OnBnClickedRightreset();
	afx_msg void OnBnClickedRightresetall();
	afx_msg void OnBnClickedLoutpos();
	afx_msg void OnBnClickedLoutangle();
	afx_msg void OnBnClickedRoutangle();
	afx_msg void OnBnClickedRoutpos();
	afx_msg void OnBnClickedDataglove();
	afx_msg LRESULT MyMsgHandler(WPARAM, LPARAM);
	CListBox m_ActionListCtrl;
	afx_msg void OnBnClickedApplyaction();
	afx_msg void OnBnClickedActionreset();
	afx_msg void OnLbnDblclkList1();
	afx_msg void OnBnClickedSendaction();
	afx_msg void OnBnClickedKinect();
	afx_msg void OnBnClickedConnect();
};
