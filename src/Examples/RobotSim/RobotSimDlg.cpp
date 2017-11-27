
// RobotSimDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "RobotSim.h"
#include "RobotSimDlg.h"
#include "afxdialogex.h"
#include "GloveDlg.h"

// Support for native win32 API
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#include <GLFW/glfw3native.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace cxc;
// CRobotSimDlg 对话框

auto m_Engine = EngineFacade::GetInstance();

CRobotSimDlg::CRobotSimDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_ROBOTSIM_DIALOG, pParent)
	, m_Output(_T(""))
	, m_LeftCombo(_T(""))
	, m_RightCombo(_T(""))
	, m_LeftAngle(_T(""))
	, m_RightAngle(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CRobotSimDlg::OutputInfo(const CString & str)
{
	// Get current time
	auto tt = std::chrono::system_clock::to_time_t
	(std::chrono::system_clock::now());

	struct tm* ptm = localtime(&tt);
	char date[60] = {0};

	sprintf_s(date, "[%d-%02d-%02d %02d:%02d:%02d] ",
		(int)ptm->tm_year + 1900, (int)ptm->tm_mon + 1, (int)ptm->tm_mday,
		(int)ptm->tm_hour, (int)ptm->tm_min, (int)ptm->tm_sec);

	// Update output buffer
	UpdateData(true);
	m_Output += date;
	m_Output += str;
	UpdateData(false);

	// scroll to the bottom
	m_OutputCtrl.LineScroll(m_OutputCtrl.GetLineCount(), 0);
}

void CRobotSimDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PIC, m_PicCtrl);
	DDX_Text(pDX, IDC_OUTPUT, m_Output);
	DDX_CBString(pDX, IDC_LEFTCOMBO, m_LeftCombo);
	DDX_CBString(pDX, IDC_RIGHTCOMBO, m_RightCombo);
	DDX_Control(pDX, IDC_LEFTCOMBO, m_LeftComboxCtrl);
	DDX_Control(pDX, IDC_RIGHTCOMBO, m_RightComboxCtrl);
	DDX_Text(pDX, IDC_LEFTANGLE, m_LeftAngle);
	DDX_Text(pDX, IDC_RIGHTANGLE, m_RightAngle);
	DDX_Control(pDX, IDC_OUTPUT, m_OutputCtrl);
	DDX_Control(pDX, IDC_LIST1, m_ActionListCtrl);
}

BEGIN_MESSAGE_MAP(CRobotSimDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CRobotSimDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CRobotSimDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_LEFTMOTION, &CRobotSimDlg::OnBnClickedLeftmotion)
	ON_BN_CLICKED(IDC_LEFTRESET, &CRobotSimDlg::OnBnClickedLeftreset)
	ON_BN_CLICKED(IDC_LEFTRESETALL, &CRobotSimDlg::OnBnClickedLeftresetall)
	ON_BN_CLICKED(IDC_RIGHTMOTION, &CRobotSimDlg::OnBnClickedRightmotion)
	ON_BN_CLICKED(IDC_RIGHTRESET, &CRobotSimDlg::OnBnClickedRightreset)
	ON_BN_CLICKED(IDC_RIGHTRESETALL, &CRobotSimDlg::OnBnClickedRightresetall)
	ON_BN_CLICKED(IDC_LOUTPOS, &CRobotSimDlg::OnBnClickedLoutpos)
	ON_BN_CLICKED(IDC_LOUTANGLE, &CRobotSimDlg::OnBnClickedLoutangle)
	ON_BN_CLICKED(IDC_ROUTANGLE, &CRobotSimDlg::OnBnClickedRoutangle)
	ON_BN_CLICKED(IDC_ROUTPOS, &CRobotSimDlg::OnBnClickedRoutpos)
	ON_BN_CLICKED(IDC_DATAGLOVE, &CRobotSimDlg::OnBnClickedDataglove)
	ON_MESSAGE(WM_MYMSG, MyMsgHandler)
	ON_BN_CLICKED(IDC_APPLYACTION, &CRobotSimDlg::OnBnClickedApplyaction)
	ON_BN_CLICKED(IDC_ACTIONRESET, &CRobotSimDlg::OnBnClickedActionreset)
	ON_LBN_DBLCLK(IDC_LIST1, &CRobotSimDlg::OnLbnDblclkList1)
	ON_BN_CLICKED(IDC_SENDACTION, &CRobotSimDlg::OnBnClickedSendaction)
	ON_BN_CLICKED(IDC_BUTTON1, &CRobotSimDlg::OnBnClickedKinect)
END_MESSAGE_MAP()


// CRobotSimDlg 消息处理程序

BOOL CRobotSimDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// window'size can not be changed
	ModifyStyle(m_hWnd, WS_THICKFRAME, 0, 0);

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	std::shared_ptr<Object3D> Plane;
	std::shared_ptr<Object3D> Table;
	std::shared_ptr<Object3D> Widget;

	auto start = std::chrono::system_clock::now();
	auto LoadRobothand = [&](int type) {
		if (type == ROBOTHAND_LEFT)
			m_LeftPtr = std::make_shared<Robothand>(type);
		else
			m_RightPtr = std::make_shared<Robothand>(type);
	};

	auto LoadPlane = [&]() {Plane = std::make_shared<Object3D>("plane",PlaneFile); };
	auto LoadTable_and_widget= [&]() {
		Table = std::make_shared<Object3D>("table", TableFile); 
		Widget = std::make_shared<Object3D>("widget", WidgetFile,GL_FALSE);
		
	};

	std::thread left_hand(LoadRobothand, ROBOTHAND_LEFT);
	std::thread right_hand(LoadRobothand, ROBOTHAND_RIGHT);
	std::thread plane(LoadPlane);
	std::thread table(LoadTable_and_widget);

	left_hand.join();
	right_hand.join();
	plane.join();
	table.join();

	auto end = std::chrono::system_clock::now();
	auto LoadingTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
	CString TimeStr;
	TimeStr.Format("%.3f", static_cast<double>(LoadingTime.count()) *
		std::chrono::microseconds::period::num / std::chrono::microseconds::period::den);

	if (m_LeftPtr->CheckLoaded() && m_RightPtr->CheckLoaded())
	{
		OutputInfo("手臂加载成功\r\n");
		OutputInfo("加载用时: " + TimeStr + "s \r\n");
	}
	else
		OutputInfo("手臂加载失败\r\n");
		
	m_Engine->m_pSceneMgr->AddObject(m_LeftPtr->GetObjectName(), m_LeftPtr);
	m_Engine->m_pSceneMgr->AddObject(m_RightPtr->GetObjectName(), m_RightPtr);
	m_Engine->m_pSceneMgr->AddObject(Plane->GetObjectName(), Plane);
	m_Engine->m_pSceneMgr->AddObject(Table->GetObjectName(), Table);
	m_Engine->m_pSceneMgr->AddObject(Widget->GetObjectName(), Widget);

	// Init ComboBox
	LoadCombobox();

	// Init ListBox
	LoadListBox();

	// reset original degrees 
	m_LeftPtr->InitOriginalDegrees();
	m_RightPtr->InitOriginalDegrees();

	// Init hand pose
	m_LeftPtr->IntegralTranslation(750, 0, 0);
	m_LeftPtr->RotateJoint("arm_left2", 90.0f);
	m_RightPtr->IntegralTranslation(-750, 0, 0);
	m_RightPtr->RotateJoint("arm_right2", 90.0f);


	m_LeftPtr->SetBaseDegrees({0,0,90,0,-90,0});
	m_RightPtr->SetBaseDegrees({ 0,0,90,0,-90,0 });

	// Init OpenGL in main thread
	m_Engine->m_pWindowMgr->InitGL();

	auto m_Hwnd = GetDlgItem(IDC_PIC)->m_hWnd;
	RECT display_size;
	m_PicCtrl.GetWindowRect(&display_size);

	// Set display window size
	m_Engine->m_pWindowMgr->SetWindowHeight(display_size.bottom - display_size.top);
	m_Engine->m_pWindowMgr->SetWindowWidth(display_size.right - display_size.left);

	// Begin rendering thread
	m_Engine->run();

	OutputInfo("渲染线程开始\r\n");

	// Until window has been created
	while (!m_Engine->m_pWindowMgr->WindowIsReady());

	auto RenderingHwnd = glfwGetWin32Window(m_Engine->m_pWindowMgr->GetWindowHandle());

	// Set display window position
	::SetParent(RenderingHwnd, m_Hwnd);
	
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CRobotSimDlg::LoadListBox()
{
	for (auto action : g_ActionExams_Hand) 
		m_ActionListCtrl.AddString(action.first.c_str());
}

void CRobotSimDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CRobotSimDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CRobotSimDlg::LoadCombobox()
{
	auto left = dynamic_cast<Robothand*>(m_Engine->m_pSceneMgr->GetObject3D("SZRobothandL").get());
	auto right = dynamic_cast<Robothand*>(m_Engine->m_pSceneMgr->GetObject3D("SZRobothandR").get());

	if (left && right)
	{
		auto left_modelmap = left->GetJointAxisMap();
		auto right_modelmap = right->GetJointAxisMap();

		// Loading left combobox
		for (auto it_left : left_modelmap)
		{
			auto model_name = it_left.first;
			CString tmp_str;

			auto name = g_NameMap.find(model_name);
			if (name != g_NameMap.end())
			{
				tmp_str.Format("%s", (name->second).c_str());
				m_LeftComboxCtrl.AddString(tmp_str);
				m_InvNameMap.insert(std::make_pair(name->second,model_name));
			}
		}

		// Loading right combobox
		for (auto it_right : right_modelmap)
		{
			auto model_name = it_right.first;
			CString tmp_str;

			auto name = g_NameMap.find(model_name);
			if (name != g_NameMap.end())
			{
				tmp_str.Format("%s", (name->second).c_str());
				m_RightComboxCtrl.AddString(tmp_str);
				m_InvNameMap.insert(std::make_pair(name->second, model_name));
			}
		}

		m_LeftComboxCtrl.SetCurSel(0);
		m_RightComboxCtrl.SetCurSel(0);
	}
}

void CRobotSimDlg::OnBnClickedOk()
{

}


void CRobotSimDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码

	m_Engine->GameOver = true;
	// wait for rendering thread stopping
	m_Engine->waitForStop();

	CDialogEx::OnCancel();

}



void CRobotSimDlg::OnBnClickedLeftmotion()
{
	// TODO: 在此添加控件通知处理程序代码

	auto index = m_LeftComboxCtrl.GetCurSel();
	if (-1 == index)
		OutputInfo("请选择一个左手关节\r\n");
	else
	{
		CString Jointname;
		m_LeftComboxCtrl.GetLBText(index, Jointname);
		UpdateData(true);

		auto angle = _tstof(m_LeftAngle);

		if (m_LeftPtr)
		{
			if (!m_LeftPtr->RotateJoint(m_InvNameMap[Jointname.GetBuffer(0)] , angle))
				OutputInfo("操作失败\r\n");
			else {
				CString result;
				result.Format("%s 旋转%s度\r\n",Jointname,m_LeftAngle);
				OutputInfo(result);
			}
		}

		GetDlgItem(IDC_LEFTANGLE)->SetWindowTextA("");
	}
}


void CRobotSimDlg::OnBnClickedLeftreset()
{
	// TODO: 在此添加控件通知处理程序代码

	auto index = m_LeftComboxCtrl.GetCurSel();
	if (-1 == index)
		OutputInfo("请选择一个左手关节\r\n");
	else
	{

		if (m_LeftPtr)
		{
			CString Jointname;
			m_LeftComboxCtrl.GetLBText(index, Jointname);

			m_LeftPtr->ResetJointPose(m_InvNameMap[Jointname.GetBuffer(0)]);
			CString result;
			result.Format("%s已经复位\r\n",Jointname);
			OutputInfo(result);
		}
	}
}


void CRobotSimDlg::OnBnClickedLeftresetall()
{
	// TODO: 在此添加控件通知处理程序代码

	if (m_LeftPtr)
	{
		m_LeftPtr->ResetAllJointPose();
		OutputInfo("所有左手关节已经复位\r\n");
	}
}


void CRobotSimDlg::OnBnClickedRightmotion()
{
	// TODO: 在此添加控件通知处理程序代码

	auto index = m_RightComboxCtrl.GetCurSel();
	if (-1 == index)
		OutputInfo("请选择一个右手关节\r\n");
	else
	{
		CString Jointname;
		m_RightComboxCtrl.GetLBText(index, Jointname);
		UpdateData(true);

		auto angle = _tstof(m_RightAngle);
		auto RightPtr = dynamic_cast<Robothand*>(m_Engine->m_pSceneMgr->GetObject3D("SZRobothandR").get());
		if (RightPtr)
		{
			if (!RightPtr->RotateJoint(m_InvNameMap[Jointname.GetBuffer(0)], angle))
				OutputInfo("操作失败\r\n");
			else {
				CString result;
				result.Format("%s 旋转%s度\r\n", Jointname, m_RightAngle);
				OutputInfo(result);
			}
		}

		GetDlgItem(IDC_RIGHTANGLE)->SetWindowTextA("");

	}
}


void CRobotSimDlg::OnBnClickedRightreset()
{
	// TODO: 在此添加控件通知处理程序代码

	auto index = m_RightComboxCtrl.GetCurSel();
	if (-1 == index)
		OutputInfo("请选择一个右手关节\r\n");
	else
	{
		auto RightPtr = dynamic_cast<Robothand*>(m_Engine->m_pSceneMgr->GetObject3D("SZRobothandR").get());

		if (RightPtr)
		{
			CString Jointname;
			m_RightComboxCtrl.GetLBText(index, Jointname);

			RightPtr->ResetJointPose(m_InvNameMap[Jointname.GetBuffer(0)]);
			CString result;
			result.Format("%s已经复位\r\n", Jointname);
			OutputInfo(result);
		}
	}
}


void CRobotSimDlg::OnBnClickedRightresetall()
{
	// TODO: 在此添加控件通知处理程序代码

	auto RightPtr = dynamic_cast<Robothand*>(m_Engine->m_pSceneMgr->GetObject3D("SZRobothandR").get());

	if (RightPtr)
	{
		RightPtr->ResetAllJointPose();
		OutputInfo("所有左手关节已经复位\r\n");
	}
}

void CRobotSimDlg::OnBnClickedLoutangle()
{
	// TODO: 在此添加控件通知处理程序代码

	auto index = m_LeftComboxCtrl.GetCurSel();
	if (-1 == index)
		OutputInfo("请选择一个左手关节\r\n");
	else
	{

		if (m_LeftPtr)
		{
			CString Jointname;
			m_LeftComboxCtrl.GetLBText(index, Jointname);

			auto degree = m_LeftPtr->GetJointDegree(m_InvNameMap[Jointname.GetBuffer(0)]);
			CString result;
			result.Format("%s, 角度：%.2f\r\n",Jointname,degree);

			OutputInfo(result);
		}
	}
}


void CRobotSimDlg::OnBnClickedRoutangle()
{
	// TODO: 在此添加控件通知处理程序代码

	auto index = m_RightComboxCtrl.GetCurSel();
	if (-1 == index)
		OutputInfo("请选择一个右手关节\r\n");
	else
	{
		auto RighttPtr = dynamic_cast<Robothand*>(m_Engine->m_pSceneMgr->GetObject3D("SZRobothandR").get());

		if (RighttPtr)
		{
			CString Jointname;
			m_RightComboxCtrl.GetLBText(index, Jointname);

			auto degree = RighttPtr->GetJointDegree(m_InvNameMap[Jointname.GetBuffer(0)]);
			CString result;
			result.Format("%s, 角度：%.2f\r\n", Jointname, degree);

			OutputInfo(result);
		}
	}
}


void CRobotSimDlg::OnBnClickedRoutpos()
{
	// TODO: 在此添加控件通知处理程序代码
	auto index = m_RightComboxCtrl.GetCurSel();
	if (-1 == index)
		OutputInfo("请选择一个右手关节\r\n");
	else
	{
		auto RightPtr = dynamic_cast<Robothand*>(m_Engine->m_pSceneMgr->GetObject3D("SZRobothandR").get());

		if (RightPtr)
		{
			CString Jointname;
			m_RightComboxCtrl.GetLBText(index, Jointname);

			auto model_map = RightPtr->GetModelMap();

			auto it = model_map.find(m_InvNameMap[Jointname.GetBuffer(0)]);
			std::vector<glm::vec3> points;
			if (it != model_map.end())
				points = it->second->GetKeyPoints();

			for (auto point : points) {
				CString result;

				point = Robothand::CoordinateSysTransTo3DS(Robothand::RightToLeftCoordinateTrans(point));

				result.Format("%s, 坐标：x=%.3f, y=%.3f, z=%.3f\r\n",Jointname,point.x,point.y,point.z);
				OutputInfo(result);
			}
		}
	}

}

void CRobotSimDlg::OnBnClickedLoutpos()
{
	// TODO: 在此添加控件通知处理程序代码

	auto index = m_LeftComboxCtrl.GetCurSel();
	if (-1 == index)
		OutputInfo("请选择一个左手关节\r\n");
	else
	{

		if (m_LeftPtr)
		{
			CString Jointname;
			m_LeftComboxCtrl.GetLBText(index, Jointname);

			auto model_map = m_LeftPtr->GetModelMap();

			auto it = model_map.find(m_InvNameMap[Jointname.GetBuffer(0)]);
			std::vector<glm::vec3> points;
			if (it != model_map.end())
				points = it->second->GetKeyPoints();

			for (auto point : points) {

				CString result;

				point = Robothand::CoordinateSysTransTo3DS(Robothand::RightToLeftCoordinateTrans(point));

				result.Format("%s, 坐标：x=%.3f, y=%.3f, z=%.3f\r\n", Jointname, point.x, point.y, point.z);
				OutputInfo(result);
			}
		}
	}
}

void CRobotSimDlg::OnBnClickedDataglove()
{
	// TODO: 在此添加控件通知处理程序代码
	glove_dlg = std::make_unique<CGloveDlg>();
	glove_dlg->Create(IDD_DIALOG_RobotControl);
	glove_dlg->ShowWindow(SW_SHOWNORMAL);
}

LRESULT CRobotSimDlg::MyMsgHandler(WPARAM, LPARAM)
{
	// Right hand 
	auto angles = glove_dlg->angle;
	float degree;

	auto RightPtr = dynamic_cast<Robothand*>(m_Engine->m_pSceneMgr->GetObject3D("SZRobothandR").get());
	 
	if (!RightPtr) return 0;

	// Thumb palm right
	degree = RightPtr->GetJointDegree("thumb_palm_right");
	RightPtr->RotateJoint("thumb_palm_right",-angles[2] - 15 - degree);

	// Thumb right 1
	degree = RightPtr->GetJointDegree("thumb_right1");
	RightPtr->RotateJoint("thumb_right1", angles[2] - 20 - degree);

	// Thumb right 2
	degree = RightPtr->GetJointDegree("thumb_right2");
	RightPtr->RotateJoint("thumb_right2", -angles[0] - degree);

	// Thumb right 3
	degree = RightPtr->GetJointDegree("thumb_right3");
	RightPtr->RotateJoint("thumb_right3", -angles[1] - degree);

	// Thumb right 4
	degree = RightPtr->GetJointDegree("thumb_right4");
	RightPtr->RotateJoint("thumb_right4", -angles[1] * 0.67  - degree);

	// Index right 1
	degree = RightPtr->GetJointDegree("index_right1");
	RightPtr->RotateJoint("index_right1", angles[5] - degree);

	// Index right 2
	degree = RightPtr->GetJointDegree("index_right2");
	RightPtr->RotateJoint("index_right2", angles[3] - degree);

	// Index right 3
	degree = RightPtr->GetJointDegree("index_right3");
	RightPtr->RotateJoint("index_right3",  angles[4] - degree);

	// Index right 4
	degree = RightPtr->GetJointDegree("index_right4");
	RightPtr->RotateJoint("index_right4", angles[4] * 0.67f - degree);

	// Middle right 2
	degree = RightPtr->GetJointDegree("middle_right2");
	RightPtr->RotateJoint("middle_right2", angles[6] - degree);

	// Middle right 3
	degree = RightPtr->GetJointDegree("middle_right3");
	RightPtr->RotateJoint("middle_right3", angles[7] - degree);

	// Middle right 4
	degree = RightPtr->GetJointDegree("middle_right4");
	RightPtr->RotateJoint("middle_right4",angles[7] * 0.67f - degree);

	// Ring right 1
	degree = RightPtr->GetJointDegree("ring_right1");
	RightPtr->RotateJoint("ring_right1",-angles[8] - degree);

	// Ring right 2
	degree = RightPtr->GetJointDegree("ring_right2");
	RightPtr->RotateJoint("ring_right2", angles[9] - degree);

	// Ring right 3
	degree = RightPtr->GetJointDegree("ring_right3");
	RightPtr->RotateJoint("ring_right3",angles[10] - degree);

	// Ring right 4
	degree = RightPtr->GetJointDegree("ring_right4");
	RightPtr->RotateJoint("ring_right4",angles[10] * 0.67f - degree);

	// Little right 1
	degree = RightPtr->GetJointDegree("little_right1");
	RightPtr->RotateJoint("little_right1",-(angles[11] + angles[8]) - degree);

	// Little right 2
	degree = RightPtr->GetJointDegree("little_right2");
	RightPtr->RotateJoint("little_right2",angles[12] - degree);

	// Little right 3
	degree = RightPtr->GetJointDegree("little_right3");
	RightPtr->RotateJoint("little_right3",angles[13] - degree);

	// Little right 4
	degree = RightPtr->GetJointDegree("little_right4");
	RightPtr->RotateJoint("little_right4",angles[13] * 0.67f - degree);

	return 0;
}

void CRobotSimDlg::OnBnClickedApplyaction()
{
	// TODO: 在此添加控件通知处理程序代码
	CString ActionString;
	auto nIndex = m_ActionListCtrl.GetCurSel();
	if (nIndex == -1) return;

	m_ActionListCtrl.GetText(nIndex,ActionString);

	float degree;

	if (!m_LeftPtr || !m_RightPtr) return;

	// Hand
	auto it = g_ActionExams_Hand.find(ActionString.GetBuffer(0));
	if (it != g_ActionExams_Hand.end())
	{
		auto angles = it->second;
		
		// Reset all joints pose of left and right hand
		m_LeftPtr->ResetAllJointPose();
		m_RightPtr->ResetAllJointPose();

		// Left thumb palm
		degree = m_LeftPtr->GetJointDegree("thumb_palm_left");
		m_LeftPtr->RotateJoint("thumb_palm_left",angles[0] - degree);

		// Right thumb palm
		degree = m_RightPtr->GetJointDegree("thumb_palm_right");
		m_RightPtr->RotateJoint("thumb_palm_right", -angles[1] - degree);

		// Left thumb
		degree = m_LeftPtr->GetJointDegree("thumb_left1");
		m_LeftPtr->RotateJoint("thumb_left1",-angles[2] - degree);

		degree = m_LeftPtr->GetJointDegree("thumb_left2");
		m_LeftPtr->RotateJoint("thumb_left2", -angles[3] - degree);

		degree = m_LeftPtr->GetJointDegree("thumb_left3");
		m_LeftPtr->RotateJoint("thumb_left3", -angles[4] - degree);

		degree = m_LeftPtr->GetJointDegree("thumb_left4");
		m_LeftPtr->RotateJoint("thumb_left4", -angles[5] - degree);

		// Right thumb
		degree = m_RightPtr->GetJointDegree("thumb_right1");
		m_RightPtr->RotateJoint("thumb_right1",angles[6] - degree);

		degree = m_RightPtr->GetJointDegree("thumb_right2");
		m_RightPtr->RotateJoint("thumb_right2", -angles[7] - degree);

		degree = m_RightPtr->GetJointDegree("thumb_right3");
		m_RightPtr->RotateJoint("thumb_right3", -angles[8] - degree);

		degree = m_RightPtr->GetJointDegree("thumb_right4");
		m_RightPtr->RotateJoint("thumb_right4", -angles[9] - degree);

		// Left index
		degree = m_LeftPtr->GetJointDegree("index_left1");
		m_LeftPtr->RotateJoint("index_left1", angles[10] - degree);

		degree = m_LeftPtr->GetJointDegree("index_left2");
		m_LeftPtr->RotateJoint("index_left2", angles[11] - degree);

		degree = m_LeftPtr->GetJointDegree("index_left3");
		m_LeftPtr->RotateJoint("index_left3", angles[12] - degree);

		degree = m_LeftPtr->GetJointDegree("index_left4");
		m_LeftPtr->RotateJoint("index_left4", angles[13] - degree);
		
		// Right index
		degree = m_RightPtr->GetJointDegree("index_right1");
		m_RightPtr->RotateJoint("index_right1", -angles[14] - degree);

		degree = m_RightPtr->GetJointDegree("index_right2");
		m_RightPtr->RotateJoint("index_right2", angles[15] - degree);

		degree = m_RightPtr->GetJointDegree("index_right3");
		m_RightPtr->RotateJoint("index_right3", angles[16] - degree);

		degree = m_RightPtr->GetJointDegree("index_right4");
		m_RightPtr->RotateJoint("index_right4", angles[17] - degree);

		// Left middle
		degree = m_LeftPtr->GetJointDegree("middle_left1");
		m_LeftPtr->RotateJoint("middle_left1", angles[18] - degree);

		degree = m_LeftPtr->GetJointDegree("middle_left2");
		m_LeftPtr->RotateJoint("middle_left2", angles[19] - degree);

		degree = m_LeftPtr->GetJointDegree("middle_left3");
		m_LeftPtr->RotateJoint("middle_left3", angles[20] - degree);

		degree = m_LeftPtr->GetJointDegree("middle_left4");
		m_LeftPtr->RotateJoint("middle_left4", angles[21] - degree);

		// Right middle
		degree = m_RightPtr->GetJointDegree("middle_right1");
		m_RightPtr->RotateJoint("middle_right1", -angles[22] - degree);

		degree = m_RightPtr->GetJointDegree("middle_right2");
		m_RightPtr->RotateJoint("middle_right2", angles[23] - degree);

		degree = m_RightPtr->GetJointDegree("middle_right3");
		m_RightPtr->RotateJoint("middle_right3", angles[24] - degree);

		degree = m_RightPtr->GetJointDegree("middle_right4");
		m_RightPtr->RotateJoint("middle_right4", angles[25] - degree);

		// Left ring
		degree = m_LeftPtr->GetJointDegree("ring_left1");
		m_LeftPtr->RotateJoint("ring_left1", angles[26] - degree);

		degree = m_LeftPtr->GetJointDegree("ring_left2");
		m_LeftPtr->RotateJoint("ring_left2", angles[27] - degree);

		degree = m_LeftPtr->GetJointDegree("ring_left3");
		m_LeftPtr->RotateJoint("ring_left3", angles[28] - degree);

		degree = m_LeftPtr->GetJointDegree("ring_left4");
		m_LeftPtr->RotateJoint("ring_left4", angles[29] - degree);

		// Right ring
		degree = m_RightPtr->GetJointDegree("ring_right1");
		m_RightPtr->RotateJoint("ring_right1", -angles[30] - degree);

		degree = m_RightPtr->GetJointDegree("ring_right2");
		m_RightPtr->RotateJoint("ring_right2", angles[31] - degree);

		degree = m_RightPtr->GetJointDegree("ring_right3");
		m_RightPtr->RotateJoint("ring_right3", angles[32] - degree);

		degree = m_RightPtr->GetJointDegree("ring_right4");
		m_RightPtr->RotateJoint("ring_right4", angles[33] - degree);

		// Left little
		degree = m_LeftPtr->GetJointDegree("little_left1");
		m_LeftPtr->RotateJoint("little_left1", angles[34] - degree);

		degree = m_LeftPtr->GetJointDegree("little_left2");
		m_LeftPtr->RotateJoint("little_left2", angles[35] - degree);

		degree = m_LeftPtr->GetJointDegree("little_left3");
		m_LeftPtr->RotateJoint("little_left3", angles[36] - degree);

		degree = m_LeftPtr->GetJointDegree("little_left4");
		m_LeftPtr->RotateJoint("little_left4", angles[37] - degree);

		// Right little
		degree = m_RightPtr->GetJointDegree("little_right1");
		m_RightPtr->RotateJoint("little_right1", -angles[38] - degree);

		degree = m_RightPtr->GetJointDegree("little_right2");
		m_RightPtr->RotateJoint("little_right2", angles[39] - degree);

		degree = m_RightPtr->GetJointDegree("little_right3");
		m_RightPtr->RotateJoint("little_right3", angles[40] - degree);

		degree = m_RightPtr->GetJointDegree("little_right4");
		m_RightPtr->RotateJoint("little_right4", angles[41] - degree);
	}

	// Arm
	auto _it = g_ActionExams_Arm.find(ActionString.GetBuffer(0));
	if (_it != g_ActionExams_Arm.end())
	{
		auto angles = _it->second;

		// Left arm2
		degree = m_LeftPtr->GetJointDegree("arm_left2");
		m_LeftPtr->RotateJoint("arm_left2", angles[0] - degree);

		// Left arm3
		degree = m_LeftPtr->GetJointDegree("arm_left3");
		m_LeftPtr->RotateJoint("arm_left3", angles[1] - degree);

		// Left arm4
		degree = m_LeftPtr->GetJointDegree("arm_left4");
		m_LeftPtr->RotateJoint("arm_left4", angles[2] - degree);

		// Left arm5
		degree = m_LeftPtr->GetJointDegree("arm_left5");
		m_LeftPtr->RotateJoint("arm_left5", angles[3] - degree);

		// Left arm6
		degree = m_LeftPtr->GetJointDegree("arm_left6");
		m_LeftPtr->RotateJoint("arm_left6", angles[4] - degree);

		// Left palm
		degree = m_LeftPtr->GetJointDegree("palm_left");
		m_LeftPtr->RotateJoint("palm_left", angles[5] - degree);

		// Right arm2
		degree = m_RightPtr->GetJointDegree("arm_right2");
		m_RightPtr->RotateJoint("arm_right2", angles[6] - degree);

		// Right arm3
		degree = m_RightPtr->GetJointDegree("arm_right3");
		m_RightPtr->RotateJoint("arm_right3", angles[7] - degree);

		// Right arm4
		degree = m_RightPtr->GetJointDegree("arm_right4");
		m_RightPtr->RotateJoint("arm_right4", angles[8] - degree);

		// Right arm5
		degree = m_RightPtr->GetJointDegree("arm_right5");
		m_RightPtr->RotateJoint("arm_right5", angles[9] - degree);

		// Right arm6
		degree = m_RightPtr->GetJointDegree("arm_right6");
		m_RightPtr->RotateJoint("arm_right6", angles[10] - degree);

		// Right palm
		degree = m_RightPtr->GetJointDegree("palm_right");
		m_RightPtr->RotateJoint("palm_right", angles[11] - degree);
	}

	CString res;
	res.Format("执行%s\r\n",ActionString);
	OutputInfo(res);

}


void CRobotSimDlg::OnBnClickedActionreset()
{
	// TODO: 在此添加控件通知处理程序代码

	if (!m_LeftPtr || !m_RightPtr) return;

	// Reset all joints pose of left and right hand
	m_LeftPtr->ResetAllJointPose();
	m_RightPtr->ResetAllJointPose();

	OutputInfo("复位动作\r\n");
}


void CRobotSimDlg::OnLbnDblclkList1()
{
	// TODO: 在此添加控件通知处理程序代码

	OnBnClickedApplyaction();
}


void CRobotSimDlg::OnBnClickedSendaction()
{
	// TODO: 在此添加控件通知处理程序代码
	
	auto pSceneMgr = m_Engine->m_pSceneMgr;
	auto pWidget = pSceneMgr->GetObject3D("widget");
	auto center_pos = pWidget->GetCenterPos();
	auto target_pos = glm::vec3({center_pos.x,-center_pos.z,center_pos.y});

	m_LeftPtr->MovingArm({target_pos.x,target_pos.y,target_pos.z+200.0f,glm::radians(90.0f),glm::radians(90.0f),0 });

}


void CRobotSimDlg::OnBnClickedKinect()
{
	
	// TODO: 在此添加控件通知处理程序代码
	kinect_dlg = std::make_unique<KinectWin>();
	kinect_dlg->Create(IDD_Kinect);
	kinect_dlg->ShowWindow(SW_SHOWNORMAL);
	
}
