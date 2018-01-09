// KinectWin.cpp : kinect摄像头打开实现文件
//

#include "stdafx.h"
#include "KinectWin.h"
#include "afxdialogex.h"
#include "Resource.h"

#include "..\..\Engine\Graphics\SceneManager.h"

SOCKET listen_sock;
SOCKET sock;
std::string colorImagePath2 = "./images/pcd0055r.png"; //初始化截图地址
std::vector<std::string> result;
bool analyFlag=false;
bool imageFlag = false;
bool methodFlag = true;//分析方法，表示是否用fast-rcnn分析

// KinectWin 对话框
IMPLEMENT_DYNAMIC(KinectWin, CDialogEx)

KinectWin::KinectWin(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_Kinect, pParent)
{

}


void KinectWin::OnTimer(UINT_PTR nIDEvent)
{
	if (1 == nIDEvent) {
		CaptureVideo(); //捕捉自带摄像头
	}
	else if (2 == nIDEvent) {
		CaptureKinectVideo(); //捕捉彩色摄像头
	}
	else if (3 == nIDEvent) {
		CaptureKinectDepthVideo(); //捕捉深度摄像头
	}
	CDialog::OnTimer(nIDEvent);
}

void KinectWin::CaptureVideo()
{
	cv::Mat frame;
	capture >> frame;
	cv::Mat dst;
	cv::resize(frame, dst, cv::Size(x, y), 0, 0, 1);
	cv::imshow("view", dst);
}

void KinectWin::CaptureKinectVideo()
{
	const NUI_IMAGE_FRAME * pImageFrame = NULL;
	//4.1、无限等待新的数据，等到后返回  
	if (WaitForSingleObject(nextColorFrameEvent, INFINITE) == 0)
	{
		//4.2、从刚才打开数据流的流句柄中得到该帧数据，读取到的数据地址存于pImageFrame  
		hr = NuiImageStreamGetNextFrame(colorStreamHandle, 0, &pImageFrame);
		if (FAILED(hr))
		{
			AfxMessageBox(_T("不能获取彩色图像"));
			NuiShutdown();
			return;
		}

		INuiFrameTexture * pTexture = pImageFrame->pFrameTexture;
		NUI_LOCKED_RECT LockedRect;

		//4.3、提取数据帧到LockedRect，它包括两个数据对象：pitch每行字节数，pBits第一个字节地址  
		//并锁定数据，这样当我们读数据的时候，kinect就不会去修改它  
		pTexture->LockRect(0, &LockedRect, NULL, 0);
		//4.4、确认获得的数据是否有效  
		if (LockedRect.Pitch != 0)
		{
			//4.5、将数据转换为OpenCV的Mat格式  
			for (int i = 0; i < image.rows; i++)
			{
				uchar *ptr = image.ptr<uchar>(i);  //第i行的指针  

												   //每个字节代表一个颜色信息，直接使用uchar  
				uchar *pBuffer = (uchar*)(LockedRect.pBits) + i * LockedRect.Pitch;
				for (int j = 0; j < image.cols; j++)
				{
					ptr[3 * j] = pBuffer[4 * j];  //内部数据是4个字节，0-1-2是BGR，第4个现在未使用   
					ptr[3 * j + 1] = pBuffer[4 * j + 1];
					ptr[3 * j + 2] = pBuffer[4 * j + 2];
				}
			}
			cv::Mat dst;
			cv::resize(image, dst, cv::Size(x, y), 0, 0, 1);
			cv::imshow("view", dst);
		}
		else
		{
			AfxMessageBox(_T("获取数据无效"));
		}
		//5、这帧已经处理完了，所以将其解锁  
		pTexture->UnlockRect(0);
		//6、释放本帧数据，准备迎接下一帧   
		NuiImageStreamReleaseFrame(colorStreamHandle, pImageFrame);
	}
}

void KinectWin::CaptureKinectDepthVideo()
{
	const NUI_IMAGE_FRAME *pImageFrame_depth = NULL;
	//深度图像的处理
	if (WaitForSingleObject(nextDepthFrameEvent, INFINITE) == 0)
	{
		hr = NuiImageStreamGetNextFrame(depthStreamHandle, 0, &pImageFrame_depth);
		if (FAILED(hr))
		{
			AfxMessageBox(_T("不能读取深度图像"));
			NuiShutdown();
			return;
		}
		INuiFrameTexture * pTexture = pImageFrame_depth->pFrameTexture;
		NUI_LOCKED_RECT LockedRect;
		//7.3、提取数据帧到LockedRect，它包括两个数据对象：pitch每行字节数，pBits第一个字节地址  
		//并锁定数据，这样当我们读数据的时候，kinect就不会去修改它  
		pTexture->LockRect(0, &LockedRect, NULL, 0);
		//7.4、确认获得的数据是否有效  
		if (LockedRect.Pitch != 0)
		{
			//7.5、将数据转换为OpenCV的Mat格式  
			for (int i = 0; i<depthImage.rows; i++)
			{
				uchar *ptr = depthImage.ptr(i);  //第i行的指针  
												 //每个字节代表一个颜色信息，直接使用uchar  
				uchar *pBuffer = (uchar*)(LockedRect.pBits) + i * LockedRect.Pitch;
				USHORT *pBufferRun = (USHORT *)pBuffer;//这里需要转换，因为每个深度数据是2个字节，应将BYTE转成USHORT  
				for (int j = 0; j<depthImage.cols; j++)
				{
					ptr[j] = 255 - (BYTE)(256 * pBufferRun[j] / 0x1fff); //将数据归一化处理*  
				}
			}
			cv::Mat dst;
			cv::resize(depthImage, dst, cv::Size(xd, yd), 0, 0, 1);
			cv::imshow("depth", dst);
		}
		else
		{
			AfxMessageBox(_T("获取数据无效"));
		}
		//8、这帧已经处理完了，所以将其解锁  
		pTexture->UnlockRect(0);
		//9、释放本帧数据，准备迎接下一帧  
		NuiImageStreamReleaseFrame(depthStreamHandle, pImageFrame_depth);
	}
}

void KinectWin::Update(CString str)
{
	pEdit = (CEdit*)GetDlgItem(IDC_EDIT1);
	int nLength =pEdit->GetWindowTextLength();
	//选定当前文本的末端  
	pEdit->SetSel(nLength, nLength);
	pEdit->ReplaceSel(str + _T("\r\n"));
}
void KinectWin::UpdateCls(CString cls)
{
	//更新类别框
	CEdit* pCls = (CEdit*)GetDlgItem(IDC_CLS);
	int nLength = pCls->GetWindowTextLength();
	//选定当前文本的末端  
	pCls->SetSel(nLength, nLength);
	pCls->ReplaceSel(cls + _T("\r\n"));
}

KinectWin::~KinectWin()
{
}

void KinectWin::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_method);
}



BEGIN_MESSAGE_MAP(KinectWin, CDialogEx)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_SELF, &KinectWin::OnBnClickedSelf)
	ON_BN_CLICKED(IDC_BUTTON4, &KinectWin::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON1, &KinectWin::OnBnClickedButton1)
	ON_BN_CLICKED(IDCANCEL2, &KinectWin::OnBnClickedCancel2)
	ON_BN_CLICKED(IDC_BUTTON5, &KinectWin::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_ScreenShot, &KinectWin::OnBnClickedScreenshot)
	ON_BN_CLICKED(IDC_REMOTE, &KinectWin::OnBnClickedRemote)
	ON_BN_CLICKED(IDC_ANALY, &KinectWin::OnBnClickedAnaly)
	ON_BN_CLICKED(IDC_RESET, &KinectWin::OnBnClickedReset)
	ON_CBN_SELCHANGE(IDC_COMBO1, &KinectWin::OnCbnSelchangeCombo1)
	ON_BN_CLICKED(IDC_BUTTON2, &KinectWin::OnBnClickedButton2)
END_MESSAGE_MAP()


// KinectWin 消息处理程序
BOOL KinectWin::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_method.AddString("Faster RCNN");
	m_method.AddString("SSD");
	m_method.SetCurSel(0);

	count = 0;
	cvNamedWindow("view", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("depth", CV_WINDOW_AUTOSIZE);

	HWND hWnd = (HWND)cvGetWindowHandle("view");
	HWND hParent = ::GetParent(hWnd);
	::SetParent(hWnd, GetDlgItem(IDC_STATIC)->m_hWnd);
	::ShowWindow(hParent, SW_HIDE);//隐藏父窗口

	HWND hWndDept = (HWND)cvGetWindowHandle("depth");
	HWND hParentDept = ::GetParent(hWndDept);
	::SetParent(hWndDept, GetDlgItem(IDC_Dept)->m_hWnd);
	::ShowWindow(hParentDept, SW_HIDE);//隐藏父窗口

	/*初始化所有编辑框*/
	pEdit = (CEdit*)GetDlgItem(IDC_EDIT1);
	imageCut = NULL; //初始化原始图像

	image.create(480, 640, CV_8UC3);
	depthImage.create(480, 640, CV_8UC1);
	//1、初始化NUI   
	hr = NuiInitialize(NUI_INITIALIZE_FLAG_USES_COLOR | NUI_INITIALIZE_FLAG_USES_DEPTH); //初始化彩色和深度图像
	if (FAILED(hr))
	{
		//追加文本  
		Update("打开kinect失败,不能使用kinect读取图像");
	}
	else
	{
		Update("打开kinect成功");
	}
	/*socket部分*/
	AfxBeginThread(&Server_Th, 0); //初始化socket线程
	send_edit = (CEdit *)GetDlgItem(IDC_ESEND);
	send_edit->SetFocus();

	char name[80];
	CString IP;
	hostent * pHost;
	WSADATA wsData;
	::WSAStartup(MAKEWORD(2, 2), &wsData);
	//获得主机名  
	if (gethostname(name, sizeof(name)))
	{
		Update("无法获取本机地址");
		return TRUE;
	}

	pHost = gethostbyname(name);//获得主机结构  
	IP = inet_ntoa(*(in_addr *)pHost->h_addr);
	Update("本机地址" + IP);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void KinectWin::OnBnClickedSelf()
{
	// TODO: 在此添加控件通知处理程序代码
	Update("打开自带摄像头");
	CRect rect;
	CWnd *pWnd = GetDlgItem(IDC_STATIC);
	pWnd->GetClientRect(&rect); //获取控件大小
	x = rect.Width();
	y = rect.Height();
	capture = 0;
	SetTimer(1, 25, NULL); //定时器，定时时间和帧率一致
}

void KinectWin::OnBnClickedButton4()
{
	//创建读取下一帧的信号事件句柄，控制KINECT是否可以开始读取下一帧数据  
	nextColorFrameEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	colorStreamHandle = NULL; //保存图像数据流的句柄，用以提取数据   
							  //3、打开KINECT设备的彩色图信息通道，并用colorStreamHandle保存该流的句柄，以便于以后读取  
	hr = NuiImageStreamOpen(NUI_IMAGE_TYPE_COLOR, NUI_IMAGE_RESOLUTION_640x480,
		0, 2, nextColorFrameEvent, &colorStreamHandle);
	if (FAILED(hr))//判断是否提取正确   
	{
		AfxMessageBox(_T("读取错误"));
		NuiShutdown();
		return;
	}
	CRect rect;
	CWnd *pWnd = GetDlgItem(IDC_STATIC);
	pWnd->GetClientRect(&rect); //获取控件大小
	x = rect.Width();
	y = rect.Height();
	//4、开始读取彩色图数据   
	//追加文本  
	Update("开始读取kinect彩色摄像头数据");
	SetTimer(2, 25, NULL); //定时器，定时时间和帧率一致
}

void KinectWin::DrawPicToHDC(IplImage * img, UINT ID)
{
	CDC *pDC = GetDlgItem(ID)->GetDC();
	HDC hDC = pDC->GetSafeHdc();
	CRect rect;
	GetDlgItem(ID)->GetClientRect(&rect);
	CvvImage cimg;
	cimg.CopyOf(img); // 复制图片
	cimg.DrawToHDC(hDC, &rect); // 将图片绘制到显示控件的指定区域内
	ReleaseDC(pDC);
}

void KinectWin::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码	
	/*
	if (imageCut) cvReleaseImage(&imageCut);
	imageCut = cvLoadImage(colorImagePath.c_str(), 1); //显示图片
	DrawPicToHDC(imageCut, IDC_Cut);
	Update("已打开截图图片");*/
	CString filter;
	filter = "所有图片(*.png;*.jpg)||";
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, filter);
	CString result;
	CString strPath = "F:\\3DSLoader\\images\\";
	dlg.m_ofn.lpstrInitialDir = strPath;//初始化路径。
	if (dlg.DoModal() == IDOK)
	{
		result = dlg.GetPathName();
		colorImagePath2 = result;
	}

	if (imageCut) cvReleaseImage(&imageCut);
	imageCut = cvLoadImage(colorImagePath2.c_str(), 1); //显示图片
	DrawPicToHDC(imageCut, IDC_Cut);
	Update("已打开选择图片");
}

void KinectWin::OnBnClickedCancel2()
{
	KillTimer(1);
	KillTimer(2);
	KillTimer(3);
	/*if (SUCCEEDED(hr)) {
	NuiShutdown();
	}*/
	if (analyFlag) {
		closesocket(sock);
		closesocket(listen_sock);
	}
	else {
		closesocket(sock);
		closesocket(listen_sock);
		this->PostMessage(WM_CLOSE, 0, 0);
	}
}


void KinectWin::OnBnClickedButton5()
{
	// 打开深度摄像头
	//2、定义事件句柄   
	//创建读取下一帧的信号事件句柄，控制KINECT是否可以开始读取下一帧数据  
	nextDepthFrameEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	//深度图像事件句柄
	depthStreamHandle = NULL;
	//实例打开数据流，这里NUI_IMAGE_TYPE_DEPTH表示深度图像
	hr = NuiImageStreamOpen(NUI_IMAGE_TYPE_DEPTH, NUI_IMAGE_RESOLUTION_640x480, 0, 2, nextDepthFrameEvent, &depthStreamHandle);
	if (FAILED(hr))//判断是否提取正确   
	{
		AfxMessageBox(_T("读取深度错误"));
		NuiShutdown();
		return;
	}
	Update("开始读取kinect深度摄像头数据");
	CRect rect;
	CWnd *pWnd = GetDlgItem(IDC_Dept);
	pWnd->GetClientRect(&rect); //获取控件大小
	xd = rect.Width();
	yd = rect.Height();
	SetTimer(3, 25, NULL); //定时器，定时时间和帧率一致
}


void KinectWin::OnBnClickedScreenshot()
{
	if (FAILED(hr))
	{
		AfxMessageBox(_T("未打开kinect，不能截图!"));
		return;
	}
	std::string basePath = "./images/";
	std::string colorPathPrefix = "Color";
	std::string depthPathPrefix = "Depth";
	std::string commonPathSuffix = ".png";
	std::stringstream ss;
	ss << basePath << "pcd00" << count << "r" << commonPathSuffix;
	colorImagePath2 = ss.str();
	imwrite(colorImagePath2, image);
	/*
	std::stringstream ssd;
	ssd << basePath<<count<<"-" << depthPathPrefix << commonPathSuffix;
	std::string depthImagePath = ssd.str();
	imwrite(depthImagePath, depthImage);*/
	count++;
	Update("截图成功,已保存到images文件夹中");
}


UINT Server_Th(LPVOID p)
{
	//kinect线程
	WSADATA wsaData;

	WORD wVersion;

	wVersion = MAKEWORD(2, 2);

	WSAStartup(wVersion, &wsaData);

	SOCKADDR_IN local_addr;
	SOCKADDR_IN client_addr;
	int iaddrSize = sizeof(SOCKADDR_IN);
	int res;
	char msg[1024];
	HWND hWnd = ::FindWindow(NULL, _T("Kinect"));      //得到对话框的句柄
	if (!hWnd) return 0;
	KinectWin * dlg = (KinectWin *)KinectWin::FromHandle(hWnd); //由句柄得到对话框的对象指针
	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(5150);
	local_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if ((listen_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
	{
		dlg->Update("创建监听失败");
	}
	if (bind(listen_sock, (struct sockaddr*) &local_addr, sizeof(SOCKADDR_IN)))
	{
		dlg->Update("绑定错误");
	}
	listen(listen_sock, 1);
	if ((sock = accept(listen_sock, (struct sockaddr *)&client_addr, &iaddrSize)) == INVALID_SOCKET)
	{
		dlg->Update("accept 失败");
	}
	else
	{
		CString port;
		int temp = ntohs(client_addr.sin_port);
		port.Format(_T("%d"), temp);
		dlg->Update("已连接来自：" + CString(inet_ntoa(client_addr.sin_addr)) + "  端口：" + port);
	}

	//接收数据  
	while (1)
	{
		if ((res = recv(sock, msg, 1024, 0)) == -1)
		{
			dlg->Update("失去连接");
			break;
		}
		else
		{
			int temp = strlen(msg);
			msg[temp] = '\0';
			if (strcmp(msg, "leave") == 0) {
				AfxMessageBox(_T("失去连接！"));
				break;
			}
			else if (strcmp(msg, "sendResult") == 0) {
				analyFlag = true;
				dlg->Update("准备获得分析结果");
				memset(msg, 0, 1024);
				res = recv(sock, msg, 1024, 0);
				dlg->Update("结果:" + CString(msg));

				// 以逗号为分隔符拆分字符串
				std::string temp = msg;
				int count = 0;
				int head = 0;
				for (int i = 0; i <= temp.length(); i++) {
					if (i == temp.length() || temp[i] == ',') {
						result.push_back(temp.substr(head, count));
						head = i + 1;
						count = 0;
					}
					else
					{
						count++;
					}
				}
				//将结果写入坐标框内
				dlg->UpdateCls("结果:" + CString(msg));
				dlg->OnBnClickedAnaly();
				result.clear();
			}
			else {
				dlg->Update("client:" + CString(msg));
				memset(msg, 0, 1024);
			}
		}
	}
	
	dlg->OnBnClickedCancel2();
	return 0;
}

void KinectWin::OnBnClickedRemote()
{
	HANDLE hFile;
	imageFlag = false; //图片未分析
	GetDlgItem(IDC_CLS)->SetWindowText(_T("")); //清空结果显示框

	if (imageCut) cvReleaseImage(&imageCut);
	imageCut = cvLoadImage(colorImagePath2.c_str(), 1); //显示图片
	DrawPicToHDC(imageCut, IDC_Cut);
	Update("已打开选择图片");

	hFile = CreateFile(CString(colorImagePath2.c_str()), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	unsigned long long file_size = 0;
	file_size = GetFileSize(hFile, NULL);
	char buffer[BUFFER_SIZE];
	const char * str;
	if (methodFlag) {
		str = "sendImage_f";
	}
	else
	{
		str = "sendImage_s";
	}
	//发送准备发送命令
	memset(buffer, 0, BUFFER_SIZE);
	strncpy(buffer, str, strlen(str));
	if (send(sock, buffer, strlen(str), 0) == SOCKET_ERROR)
	{
		Update("发送失败");
		return;
	}
	else
	{
		Update("server:准备发送图片");
	}

	//发送图片长度
	memset(buffer, 0, sizeof(buffer));
	memcpy(buffer, &file_size, sizeof(file_size) + 1);
	if (send(sock, buffer, sizeof(file_size) + 1, 0) == SOCKET_ERROR)
	{
		Update("发送失败");
		return;
	}
	else
	{
		Update("开始发送图片");
	}
	memset(buffer, 0, sizeof(buffer));
	DWORD dwNumberOfBytesRead;
	do
	{
		::ReadFile(hFile, buffer, sizeof(buffer), &dwNumberOfBytesRead, NULL);
		::send(sock, buffer, dwNumberOfBytesRead, 0);
	} while (dwNumberOfBytesRead);
	CloseHandle(hFile);
	Update("成功发送图片");
}


void KinectWin::OnBnClickedAnaly()
{
	CString x, y, angle, width;
	float xRaw[4], yRaw[4];
	float xNew[4], yNew[4];
	float height = 33;
	if (!imageFlag) {
		imageCut = cvLoadImage(colorImagePath2.c_str(), 1); //显示图片
		imageFlag = true;
	}
	if (result.size() < 4) {
		AfxMessageBox("还未得到分析结果！");
		return;
	}
	float xf = atof(result[1].c_str());
	float yf = atof(result[2].c_str());
	float anglef = atof(result[3].c_str());
	float widthf = atof(result[4].c_str());

	analysis_ret.x = xf;
	analysis_ret.y = yf;
	analysis_ret.angle = anglef;
	analysis_ret.width = widthf;
	analysis_ret.result_update = true;

	xRaw[1] = xf - 0.5*widthf;
	yRaw[1] = yf - 0.5*height;

	xRaw[0] = xf + 0.5*widthf;
	yRaw[0] = yRaw[1];

	xRaw[2] = xRaw[1];
	yRaw[2] = yf + 0.5*height;

	xRaw[3] = xRaw[0];
	yRaw[3] = yRaw[2];



	float anglePi = -anglef*pi / 180.0;
	float cosA = cos(anglePi);
	float sinA = sin(anglePi);

	xNew[0] = (xRaw[0] - xf)*cosA - (yRaw[0] - yf)*sinA + xf;
	yNew[0] = (xRaw[0] - xf)*sinA + (yRaw[0] - yf)*cosA + yf;

	xNew[1] = (xRaw[1] - xf)*cosA - (yRaw[1] - yf)*sinA + xf;
	yNew[1] = (xRaw[1] - xf)*sinA + (yRaw[1] - yf)*cosA + yf;

	xNew[2] = (xRaw[2] - xf)*cosA - (yRaw[2] - yf)*sinA + xf;
	yNew[2] = (xRaw[2] - xf)*sinA + (yRaw[2] - yf)*cosA + yf;

	xNew[3] = (xRaw[3] - xf)*cosA - (yRaw[3] - yf)*sinA + xf;
	yNew[3] = (xRaw[3] - xf)*sinA + (yRaw[3] - yf)*cosA + yf;

	cvLine(imageCut, cvPoint(xNew[0], yNew[0]), cvPoint(xNew[1], yNew[1]), cvScalar(0, 0, 255), 3);
	cvLine(imageCut, cvPoint(xNew[1], yNew[1]), cvPoint(xNew[2], yNew[2]), cvScalar(255, 0, 0), 3);
	cvLine(imageCut, cvPoint(xNew[2], yNew[2]), cvPoint(xNew[3], yNew[3]), cvScalar(0, 0, 255), 3);
	cvLine(imageCut, cvPoint(xNew[0], yNew[0]), cvPoint(xNew[3], yNew[3]), cvScalar(255, 0, 0), 3);

	DrawPicToHDC(imageCut, IDC_Cut);
	Update("图片已修改");
}

void KinectWin::OnBnClickedReset()
{
	//复位图片
	if (imageCut) cvReleaseImage(&imageCut);
	imageCut = cvLoadImage(colorImagePath2.c_str(), 1); //显示图片
	DrawPicToHDC(imageCut, IDC_Cut);
	Update("图片已复位");
}


void KinectWin::OnCbnSelchangeCombo1()
{
	// TODO: 在此添加控件通知处理程序代码
	int nIndex = m_method.GetCurSel();
	if (nIndex == 0) {
		methodFlag = true;
		Update("选择Faster RCNN方法分析");
	}
	else
	{
		Update("选择SSD方法分析");
		methodFlag = false;
	}
}


void KinectWin::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!analysis_ret.result_update)
		return;

	auto pSceneMgr = SceneManager::GetInstance();

	auto pWidget = pSceneMgr->GetObject3D("widget");
	if (!pWidget) return;

	pWidget->ComputeCenterPos();
	auto current_pos = pWidget->GetCenterPos();
	auto origin = glm::vec3({ table_origin.x,current_pos.y,table_origin.y });
	auto destination = glm::vec3({ analysis_ret.x,analysis_ret.y,0.0f });
	auto movement = glm::vec3({ destination.x,destination.z,-destination.y});

	pWidget->g_lock.lock();
	pWidget->Translation("widget", movement + origin - current_pos);
	pWidget->RotateWithArbitraryAxis("widget", current_pos, glm::vec3(0, 1, 0),glm::radians(analysis_ret.angle));
	pWidget->g_lock.unlock();
	pWidget->Enable();

}
