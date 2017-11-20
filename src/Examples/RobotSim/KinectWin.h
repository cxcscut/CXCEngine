#pragma once
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <NuiApi.h> 
#include "CvvImage.h"
#include "winsock2.h" 
#include "afxwin.h"
#define pi 3.1415926

// KinectWin 对话框
UINT Server_Th(LPVOID p);

class KinectWin : public CDialogEx
{
	DECLARE_DYNAMIC(KinectWin)

public:
	KinectWin(CWnd* pParent = NULL);   // 标准构造函数
	const static int BUFFER_SIZE = 1024;
	cv::VideoCapture capture;
	HANDLE nextColorFrameEvent;
	HANDLE colorStreamHandle;
	HANDLE nextDepthFrameEvent;
	HANDLE depthStreamHandle;
	cv::Mat image;
	cv::Mat depthImage; //深度图像
	HRESULT hr;
	int x;
	int y;
	int xd;
	int yd;
	CEdit * send_edit; //发送框
	IplImage *imageCut; //截图图片
	CEdit* pEdit;
	int count;
	void DrawPicToHDC(IplImage *img, UINT ID);

	void OnTimer(UINT_PTR nIDEvent);
	void CaptureVideo();
	void CaptureKinectVideo();
	void CaptureKinectDepthVideo();
	void Update(CString str);
	void UpdateCls(CString cls);

	
	virtual ~KinectWin();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_Kinect };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持											
    // 生成的消息映射函数
	DECLARE_MESSAGE_MAP()
public:

	afx_msg void OnBnClickedSelf();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedCancel2();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedScreenshot();
	afx_msg void OnBnClickedRemote();
	afx_msg void OnBnClickedAnaly();
	afx_msg void OnBnClickedReset();
	CComboBox m_method;
	afx_msg void OnCbnSelchangeCombo1();
};
