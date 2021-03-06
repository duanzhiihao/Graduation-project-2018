
// CellCutMFC_0Dlg.cpp: 实现文件
//

#include "stdafx.h"
#include "CellCutMFC_0.h"
#include "CellCutMFC_0Dlg.h"
#include "afxdialogex.h"
#include <opencv2\opencv.hpp> 
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
//#include <iostream>
//#include <direct.h>  
//#include <windows.h>
//#include <fstream>
//#include <string>

using namespace cv;
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 没用到这个函数
double generateGaussianNoise(double mu, double sigma)
{
	//定义一个特别小的值
	const double epsilon = numeric_limits<double>::min();//返回目标数据类型能表示的最逼近1的正数和1的差的绝对值
	static double z0, z1;
	static bool flag = false;
	flag = !flag;
	//flag为假，构造高斯随机变量
	if (!flag)
		return z1*sigma + mu;
	double u1, u2;
	//构造随机变量

	do
	{
		u1 = rand()*(1.0 / RAND_MAX);
		u2 = rand()*(1.0 / RAND_MAX);
	} while (u1 <= epsilon);
	//flag为真构造高斯随机变量X
	z0 = sqrt(-2.0*log(u1))*cos(2 * CV_PI * u2);
	z1 = sqrt(-2.0*log(u1))*sin(2 * CV_PI * u2);
	return z1*sigma + mu;
}

// 没用到这个函数
//为图像添加高斯噪声
Mat addGaussianNoise(Mat& srcImage)
{
	Mat resultImage = srcImage.clone();    //深拷贝,克隆
	int channels = resultImage.channels();    //获取图像的通道
	int nRows = resultImage.rows;    //图像的行数

	int nCols = resultImage.cols*channels;   //图像的总列数
											 //判断图像的连续性
	if (resultImage.isContinuous())    //判断矩阵是否连续，若连续，我们相当于只需要遍历一个一维数组 
	{
		nCols *= nRows;
		nRows = 1;
	}
	for (int i = 0; i < nRows; i++)
	{
		for (int j = 0; j < nCols; j++)
		{	//添加高斯噪声
			int val = resultImage.ptr<uchar>(i)[j] + generateGaussianNoise(2, 0.8) * 32;
			if (val < 0)
				val = 0;
			if (val > 255)
				val = 255;
			resultImage.ptr<uchar>(i)[j] = (uchar)val;
		}
	}
	return resultImage;
}

// CCellCutMFC0Dlg 对话框


CCellCutMFC0Dlg::CCellCutMFC0Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_CELLCUTMFC_0_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCellCutMFC0Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_edit1);
}

BEGIN_MESSAGE_MAP(CCellCutMFC0Dlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOPEN, &CCellCutMFC0Dlg::OnBnClickedOpen)
	ON_BN_CLICKED(IDCANCEL, &CCellCutMFC0Dlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_DECODE, &CCellCutMFC0Dlg::OnBnClickedDecode)
	ON_BN_CLICKED(IDGRAYOPEN, &CCellCutMFC0Dlg::OnBnClickedGrayopen)
	ON_BN_CLICKED(IDGRAYDECODE, &CCellCutMFC0Dlg::OnBnClickedGraydecode)
	ON_BN_CLICKED(IDSAVE, &CCellCutMFC0Dlg::OnBnClickedSave)
END_MESSAGE_MAP()


// CCellCutMFC0Dlg 消息处理程序

BOOL CCellCutMFC0Dlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	for (int i = 0; i<47; i++) {
		knndata[i][0] = knndata[i][0] * 255;
		knndata[i][1] = knndata[i][1] * 255;
		knndata[i][2] = knndata[i][2] * 255;
	}
	for (int i = 0; i<26; i++) {
		knndata2[i][0] = knndata2[i][0] * 255;
		knndata2[i][1] = knndata2[i][1] * 255;
		knndata2[i][2] = knndata2[i][2] * 255;
	}
	//bg = cv::imread("background.jpg", 1);
	graybg = cv::imread("graybg.jpg", IMREAD_GRAYSCALE);
	/*AllocConsole();
	freopen("CONOUT$", "w+t", stdout);*/

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CCellCutMFC0Dlg::OnPaint()
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
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CCellCutMFC0Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// 压缩彩色图片的函数
void CCellCutMFC0Dlg::OnBnClickedOpen()
{
	// TODO: 在此添加控件通知处理程序代码

	CString Img_path, Img_folder, file_name;
	//浏览文件打开图片对话框
	CFileDialog dlg(TRUE, _T("*.jpg"), NULL, OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY, "All Files (*.*)|*.*", NULL);
	dlg.m_ofn.lpstrTitle = _T("打开图片");
	if (dlg.DoModal()!=IDOK) return;

	clock_t startTime, crtTime;
	startTime = clock();
	textstr = "开始 time:0s \r\n";
	m_edit1.SetWindowText(textstr);

	EnableWindow(FALSE);
	Img_folder = dlg.GetFolderPath();
	Img_path = dlg.GetPathName();
	file_name = dlg.GetFileTitle();
	//f_SavePath = Img_folder.Left(Img_folder.ReverseFind('\\'));

	string srcStr;
	srcStr = Img_path.GetBuffer(0);
	Mat srcRGB;
	srcRGB = imread(srcStr);
	if (srcRGB.data==NULL)
	{
		MessageBox("请打开正确的图片!", "错误", MB_OK);
		EnableWindow(TRUE);
		return;
	}
	/*namedWindow("source");
	imshow("source", srcRGB);*/

	Mat srcgray, edge;
	cvtColor(srcRGB, srcgray, COLOR_BGR2GRAY);
	blur(srcgray, edge, Size(3, 3));
	Canny(edge, edge, 15, 45, 3);
	/*namedWindow("edge");
	imshow("edge", edge);
	waitKey(0);*/

	crtTime = clock();
	timestr.Format("%f", (float)(crtTime-startTime)/CLOCKS_PER_SEC);
	textstr = textstr + "读取成功 time:" + timestr + "s \r\n";
	m_edit1.SetWindowText(textstr);

	/*if (srcRGB.rows%8 != 0) {
	}*/

	int temprows = srcRGB.rows/8;
	int tempcols = srcRGB.cols/8;

	if (temprows<25)
	{
		MessageBox("打开的图片太小", "错误", MB_OK);
		EnableWindow(TRUE);
		return;
	}

	// templet是记录哪一块是细胞哪一块是背景 的模板
	vector<vector<bool> > templet(temprows, vector<bool>(tempcols));

	// 开始knn
	double avg88r, avg88g, avg88b, dr2, dg2, db2, d2;
	double temp0;
	int temp1, countb, countedge;
	int countcell = 0;
	for (int m = 0; m<temprows; m++) {
		for (int n = 0; n<tempcols; n++) {
			avg88r = avg88g = avg88b = dr2 = dg2 = db2 = d2 = countedge = 0;

			for (int i = 0; i<8; i++) {
				uchar* data = srcRGB.ptr<uchar>(m*8+i);
				uchar* dataedge = edge.ptr<uchar>(m*8+i);
				for (int j = 0; j<8; j++) {
					avg88b = avg88b + data[(n*8+j)*3+0];
					avg88g = avg88g + data[(n*8+j)*3+1];
					avg88r = avg88r + data[(n*8+j)*3+2];
					if (dataedge[n*8+j]>200) {
						countedge = countedge + 1;
					}
				}
			}
			avg88r = avg88r/64;
			avg88g = avg88g/64;
			avg88b = avg88b/64;

			int knnmin[5][2] = {
				{ 65535,0 },
			{ 65535,0 },
			{ 65535,0 },
			{ 65535,0 },
			{ 65535,0 }
			};
			for (int k = 0; k<57; k++) {
				dr2 = pow(avg88r - knndata[k][0], 2);
				dg2 = pow(avg88g - knndata[k][1], 2);
				db2 = pow(avg88b - knndata[k][2], 2);
				d2 = dr2 + dg2 + db2;

				if (d2<knnmin[4][0]) {
					knnmin[4][0] = d2;
					knnmin[4][1] = knndata[k][3];
					for (int i = 0; i<4; i++) {
						if (knnmin[4-i][0]<knnmin[3-i][0]) {
							temp0 = knnmin[3-i][0];
							temp1 = knnmin[3-i][1];
							knnmin[3-i][0] = knnmin[4-i][0];
							knnmin[3-i][1] = knnmin[4-i][1];
							knnmin[4-i][0] = temp0;
							knnmin[4-i][1] = temp1;
						}
					}
				}
			}

			countb = 0;
			//cout<<countb<<endl;
			for (int k = 0; k<4; k++) {
				if (knnmin[k][1]==0) countb = countb + 1;
			}

			if (countb>=3&&countedge==0) {
				templet[m][n] = 0;
			}
			else {
				templet[m][n] = 1;
				countcell++;
			}
		}
	}
	crtTime = clock();
	timestr.Format("%f", (float)(crtTime-startTime)/CLOCKS_PER_SEC);
	textstr = textstr + "分类完成 time:" + timestr + "s \r\n";
	m_edit1.SetWindowText(textstr);

	int tempcount = 0;
	int tempsumr, tempsumg, tempsumb, avgbgr, avgbgg, avgbgb;
	tempsumr = tempsumg = tempsumb = avgbgr = avgbgg = avgbgb = 0;
	for (int m = 0; m<temprows; m=m+2) {
		for (int n = 0; n<tempcols; n=n+2) {
			if (tempcount < 30) {
				if (templet[m][n]==0) {
					uchar* data = srcRGB.ptr<uchar>(m*8);
					tempsumb = tempsumb + data[n*8*3+0];
					tempsumg = tempsumg + data[n*8*3+1];
					tempsumr = tempsumr + data[n*8*3+2];
					
					tempcount++;
				}
			}
		}
	}
	avgbgr = tempsumr/tempcount;
	avgbgg = tempsumg/tempcount;
	avgbgb = tempsumb/tempcount;

	Mat cell2code(8*(1+countcell/100), 100*8, CV_8UC3, Scalar(0, 0, 0));
	int crtcell = 0;
	int crtrow = 0;
	for (int m = 0; m<temprows; m++) {
		for (int n = 0; n<tempcols; n++) {
			if (templet[m][n]==1) {
				for (int i = 0; i<8; i++) {
					uchar* data = srcRGB.ptr<uchar>(m*8+i);
					uchar* datacell = cell2code.ptr<uchar>(crtrow*8+i);
					for (int j = 0; j<8; j++) {
						datacell[(crtcell*8+j)*3+0] = data[(n*8+j)*3+0];
						datacell[(crtcell*8+j)*3+1] = data[(n*8+j)*3+1];
						datacell[(crtcell*8+j)*3+2] = data[(n*8+j)*3+2];
					}
				}
				crtcell++;
				if (crtcell>99) {
					crtcell = 0;
					crtrow++;
				}
			}
		}
	}

	vector<uchar> buf;
	imencode(".jpg", cell2code, buf);

	crtTime = clock();
	timestr.Format("%f", (float)(crtTime-startTime)/CLOCKS_PER_SEC);
	textstr = textstr + "编码完成 time:" + timestr + "s \r\n";
	m_edit1.SetWindowText(textstr);

	CString save_pathCS = Img_folder + "\\" + file_name;
	std::string save_path;
	save_path = save_pathCS.GetBuffer(0);
	save_path = save_path + ".xw";

	ofstream file2(save_path.c_str(), std::ios::out | std::ios::binary | ios::ate);
	file2<<avgbgr<<' '<<avgbgg<<' '<<avgbgb;
	file2<<endl;
	file2<<temprows<<' '<<tempcols;
	file2<<endl;
	for (int m = 0; m<temprows; m++) {
		for (int n = 0; n<tempcols; n++) {
			file2<<templet[m][n];
		}
	}
	file2<<endl;
	
	file2.write((char*)&buf[0], buf.size()*sizeof(uchar));
	file2.close();

	crtTime = clock();
	timestr.Format("%f", (float)(crtTime-startTime)/CLOCKS_PER_SEC);
	textstr = textstr + "保存完成，总用时 time:" + timestr + "s \r\n";
	m_edit1.SetWindowText(textstr);

	EnableWindow(TRUE);
	return;
}

void CCellCutMFC0Dlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialog::OnCancel();
}


void CCellCutMFC0Dlg::OnBnClickedDecode()
{
	// TODO: 在此添加控件通知处理程序代码
	CString Img_path, Img_folder, file_name;
	//浏览文件打开图片对话框
	CFileDialog dlg(TRUE, _T("*.xw"), NULL, OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY, "xw Files (*.xw)|*.xw", NULL);
	dlg.m_ofn.lpstrTitle = _T("打开xw");
	if (dlg.DoModal()!=IDOK) return;

	clock_t startTime, crtTime;
	startTime = clock();
	textstr = "开始 time:0s \r\n";
	m_edit1.SetWindowText(textstr);

	EnableWindow(FALSE);
	Img_folder = dlg.GetFolderPath();
	Img_path = dlg.GetPathName();
	file_name = dlg.GetFileTitle();
	//f_SavePath = Img_folder.Left(Img_folder.ReverseFind('\\'));

	string img2dcdStr;
	img2dcdStr = Img_path.GetBuffer(0);

	ifstream infile;
	infile.open(img2dcdStr, std::ios::in | std::ios::binary | std::ios::ate);
	std::streampos sizeend = infile.tellg();

	crtTime = clock();
	timestr.Format("%f", (float)(crtTime-startTime)/CLOCKS_PER_SEC);
	textstr = textstr + "打开xw文件成功，开始读取数据 time:" + timestr + "s \r\n";
	m_edit1.SetWindowText(textstr);

	infile.seekg(0, std::ios::beg);
	int inrows, incols, avgbgr, avgbgg, avgbgb;
	char enter;
	infile>>avgbgr>>avgbgg>>avgbgb;
	infile.get(enter);
	if (enter!='\n')
	{
		MessageBox("文件读取发生错误 after infile>>avgbgr>>avgbgg>>avgbgb;", "错误", MB_OK);
		EnableWindow(TRUE);
		return;
	}
	infile>>inrows>>incols;
	infile.get(enter);
	if (enter!='\n')
	{
		MessageBox("文件读取发生错误 after infile>inrows>incols", "错误", MB_OK);
		EnableWindow(TRUE);
		return;
	}

	vector<vector<char> > templet(inrows, vector<char>(incols));
	for (int m = 0; m<inrows; m++) {
		for (int n = 0; n<incols; n++) {
			infile.get(templet[m][n]);
		}
	}
	infile.get(enter);
	if (enter!='\n')
	{
		MessageBox("文件读取发生错误 after infileget(templet)", "错误", MB_OK);
		EnableWindow(TRUE);
		return;
	}

	std::streampos sizestart = infile.tellg();
	std::streampos size = sizeend - sizestart;
	std::string buffer(size, ' ');
	infile.read(&buffer[0], size);
	infile.close();

	crtTime = clock();
	timestr.Format("%f", (float)(crtTime-startTime)/CLOCKS_PER_SEC);
	textstr = textstr + "读取数据成功，开始解码 time:" + timestr + "s \r\n";
	m_edit1.SetWindowText(textstr);

	std::vector<char> vec_data(&buffer[0], &buffer[0] + size);
	cv::Mat cells = cv::imdecode(vec_data, 1);
	
	Mat cellRGB1(inrows*8, incols*8, CV_8UC3, Scalar(255, 255, 255));
	int crtcell = 0;
	int crtrow = 0;
	for (int m = 0; m<inrows; m++) {
		for (int n = 0; n<incols; n++) {
			if (templet[m][n]=='1') {
				for (int i = 0; i<8; i++) {
					uchar* data = cellRGB1.ptr<uchar>(m*8+i);
					uchar* datacell = cells.ptr<uchar>(crtrow*8+i);
					for (int j = 0; j<8; j++) {
						data[(n*8+j)*3+0] = datacell[(crtcell*8+j)*3+0];
						data[(n*8+j)*3+1] = datacell[(crtcell*8+j)*3+1];
						data[(n*8+j)*3+2] = datacell[(crtcell*8+j)*3+2];
					}
				}
				crtcell++;
				if (crtcell>99) {
					crtcell = 0;
					crtrow++;
				}
			}
		}
	}
	/*namedWindow("picture3");
	imshow("picture3", cellRGB1);
	waitKey(0);*/
	crtTime = clock();
	timestr.Format("%f", (float)(crtTime-startTime)/CLOCKS_PER_SEC);
	textstr = textstr + "解码完成，开始填充背景 time:" + timestr + "s \r\n";
	m_edit1.SetWindowText(textstr);
	/*namedWindow("picture3");
	imshow("picture3", bg);
	waitKey(0);*/

	//avgbgr avgbgg avgbgb
	Mat bgRGB0(inrows*8, incols*8, CV_8UC3, Scalar(avgbgb, avgbgg, avgbgr));
	/*namedWindow("picture3");
	imshow("picture3", bgRGB0);*/
	/*Mat bgRGB1 = addGaussianNoise(bgRGB0);
	namedWindow("resultImage");
	imshow("resultImage", bgRGB1);
	waitKey(0);*/

	for (int m = 0; m<inrows; m++) {
		for (int n = 0; n<incols; n++) {
			if (templet[m][n]=='0') {
				for (int i = 0; i<8; i++) {
					uchar* data = cellRGB1.ptr<uchar>(m*8+i);
					uchar* databg = bgRGB0.ptr<uchar>(m*8+i);
					for (int j = 0; j<8; j++) {
						data[(n*8+j)*3+0] = databg[(n*8+j)*3+0];
						data[(n*8+j)*3+1] = databg[(n*8+j)*3+1];
						data[(n*8+j)*3+2] = databg[(n*8+j)*3+2];
					}
				}
			}
		}
	}

	Mat cellRGBX = cellRGB1.clone();
	Mat cellRGBY = cellRGB1.clone();

	for (int m = 1; m<inrows-1; m++) {
		for (int n = 1; n<incols-1; n++) {
			if ( (templet[m][n-1]=='1'&&templet[m][n]=='0')||(templet[m][n]=='0'&&templet[m][n+1]=='1') ) {
				for (int i = 0; i<8; i++) {
					uchar* data = cellRGBX.ptr<uchar>(m*8+i);
					for (int j = 0; j<8; j++) {
						data[(n*8+j)*3+0] = data[(n*8+8)*3+0] + (data[(n*8-1)*3+0]-data[(n*8+8)*3+0])*(8-j)/9;
						data[(n*8+j)*3+1] = data[(n*8+8)*3+1] + (data[(n*8-1)*3+1]-data[(n*8+8)*3+1])*(8-j)/9;
						data[(n*8+j)*3+2] = data[(n*8+8)*3+2] + (data[(n*8-1)*3+2]-data[(n*8+8)*3+2])*(8-j)/9;
						//data[(n*8+j)*3+0] = 0;
						//data[(n*8+j)*3+1] = 0;
						//data[(n*8+j)*3+2] = 0;
					}
				}
			}
		}
	}
	/*for (int m = 1; m<inrows-1; m++) {
		for (int n = 1; n<incols-1; n++) {
			if (templet[m][n-1]=='1'&&templet[m][n]=='0'&&templet[m][n+1]=='0') {
				for (int i = 0; i<8; i++) {
					uchar* data = cellRGBX.ptr<uchar>(m*8+i);
					for (int j = 0; j<5; j++) {
						data[(n*8+j)*3+0] = data[(n*8-1)*3+0];
						data[(n*8+j)*3+1] = data[(n*8-1)*3+1];
						data[(n*8+j)*3+2] = data[(n*8-1)*3+2];
					}
				}
			}
			else if (templet[m][n-1]=='0'&&templet[m][n]=='0'&&templet[m][n+1]=='1') {
				for (int i = 0; i<8; i++) {
					uchar* data = cellRGBX.ptr<uchar>(m*8+i);
					for (int j = 3; j<8; j++) {
						data[(n*8+j)*3+0] = data[(n*8+8)*3+0];
						data[(n*8+j)*3+1] = data[(n*8+8)*3+1];
						data[(n*8+j)*3+2] = data[(n*8+8)*3+2];
					}
				}
			}
			else if (templet[m][n-1]=='1'&&templet[m][n]=='0'&&templet[m][n+1]=='1') {
				for (int i = 0; i<8; i++) {
					uchar* data = cellRGBX.ptr<uchar>(m*8+i);
					for (int j = 0; j<8; j++) {
						data[(n*8+j)*3+0] = data[(n*8+8)*3+0] + (data[(n*8-1)*3+0]-data[(n*8+8)*3+0])*(8-j)/9;
						data[(n*8+j)*3+1] = data[(n*8+8)*3+1] + (data[(n*8-1)*3+1]-data[(n*8+8)*3+1])*(8-j)/9;
						data[(n*8+j)*3+2] = data[(n*8+8)*3+2] + (data[(n*8-1)*3+2]-data[(n*8+8)*3+2])*(8-j)/9;
					}
				}
			}
		}
	}*/

	for (int m = 1; m<inrows-1; m++) {
		for (int n = 1; n<incols-1; n++) {
			if ((templet[m-1][n]=='1'&&templet[m][n]=='0')||(templet[m][n]=='0'&&templet[m+1][n]=='1')) {
				uchar* dataup = cellRGBY.ptr<uchar>(m*8-1);
				uchar* datadn = cellRGBY.ptr<uchar>(m*8+8);
				for (int i = 0; i<8; i++) {
					uchar* data = cellRGBY.ptr<uchar>(m*8+i);
					for (int j = 0; j<8; j++) {
						data[(n*8+j)*3+0] = datadn[(n*8+j)*3+0] + (dataup[(n*8+j)*3+0]-datadn[(n*8+j)*3+0])*(8-i)/9;
						data[(n*8+j)*3+1] = datadn[(n*8+j)*3+1] + (dataup[(n*8+j)*3+1]-datadn[(n*8+j)*3+1])*(8-i)/9;
						data[(n*8+j)*3+2] = datadn[(n*8+j)*3+2] + (dataup[(n*8+j)*3+2]-datadn[(n*8+j)*3+2])*(8-i)/9;
					}
				}
			}
		}
	}
	/*for (int m = 1; m<inrows-1; m++) {
		for (int n = 1; n<incols-1; n++) {
			if (templet[m-1][n]=='1'&&templet[m][n]=='0'&&templet[m+1][n]=='0') {
				uchar* dataup = cellRGBY.ptr<uchar>(m*8-1);
				for (int i = 0; i<5; i++) {
					uchar* data = cellRGBY.ptr<uchar>(m*8+i);
					for (int j = 0; j<8; j++) {
						data[(n*8+j)*3+0] = dataup[(n*8+j)*3+0];
						data[(n*8+j)*3+1] = dataup[(n*8+j)*3+1];
						data[(n*8+j)*3+2] = dataup[(n*8+j)*3+2];
					}
				}
			}
			else if (templet[m-1][n]=='0'&&templet[m][n]=='0'&&templet[m+1][n]=='1') {
				uchar* datadn = cellRGBY.ptr<uchar>(m*8+8);
				for (int i = 3; i<8; i++) {
					uchar* data = cellRGBY.ptr<uchar>(m*8+i);
					for (int j = 0; j<8; j++) {
						data[(n*8+j)*3+0] = datadn[(n*8+j)*3+0];
						data[(n*8+j)*3+1] = datadn[(n*8+j)*3+1];
						data[(n*8+j)*3+2] = datadn[(n*8+j)*3+2];
					}
				}
			}
			else if (templet[m-1][n]=='1'&&templet[m][n]=='0'&&templet[m+1][n]=='1') {
				uchar* dataup = cellRGBY.ptr<uchar>(m*8-1);
				uchar* datadn = cellRGBY.ptr<uchar>(m*8+8);
				for (int i = 0; i<8; i++) {
					uchar* data = cellRGBY.ptr<uchar>(m*8+i);
					for (int j = 0; j<8; j++) {
						data[(n*8+j)*3+0] = datadn[(n*8+j)*3+0] + (dataup[(n*8+j)*3+0]-datadn[(n*8+j)*3+0])*(8-i)/9;
						data[(n*8+j)*3+1] = datadn[(n*8+j)*3+1] + (dataup[(n*8+j)*3+1]-datadn[(n*8+j)*3+1])*(8-i)/9;
						data[(n*8+j)*3+2] = datadn[(n*8+j)*3+2] + (dataup[(n*8+j)*3+2]-datadn[(n*8+j)*3+2])*(8-i)/9;
					}
				}
			}
		}
	}*/

	for (int m = 1; m<inrows-1; m++) {
		for (int n = 1; n<incols-1; n++) {
			for (int i = 0; i<8; i++) {
				uchar* data = cellRGB1.ptr<uchar>(m*8+i);
				uchar* datax = cellRGBX.ptr<uchar>(m*8+i);
				uchar* datay = cellRGBY.ptr<uchar>(m*8+i);
				for (int j = 0; j<8; j++) {
					data[(n*8+j)*3+0] = (datax[(n*8+j)*3+0]+datay[(n*8+j)*3+0])/2;
					data[(n*8+j)*3+1] = (datax[(n*8+j)*3+1]+datay[(n*8+j)*3+1])/2;
					data[(n*8+j)*3+2] = (datax[(n*8+j)*3+2]+datay[(n*8+j)*3+2])/2;
				}
			}
		}
	}

	Mat cellRGB2;
	/*blur(cellRGB1, cellRGB2, Size(9, 9));
	for (int m = 0; m<inrows; m++) {
		for (int n = 0; n<incols; n++) {
			if (templet[m][n]=='1') {
				for (int i = 0; i<8; i++) {
					uchar* data = cellRGB2.ptr<uchar>(m*8+i);
					uchar* databg = cellRGB1.ptr<uchar>(m*8+i);
					for (int j = 0; j<8; j++) {
						data[(n*8+j)*3+0] = databg[(n*8+j)*3+0];
						data[(n*8+j)*3+1] = databg[(n*8+j)*3+1];
						data[(n*8+j)*3+2] = databg[(n*8+j)*3+2];
					}
				}
			}
		}
	}*/
	blur(cellRGB1, cellRGB2, Size(3, 3));
	for (int m = 0; m<inrows; m++) {
		for (int n = 0; n<incols; n++) {
			if (templet[m][n]=='1') {
				for (int i = 0; i<8; i++) {
					uchar* data = cellRGB2.ptr<uchar>(m*8+i);
					uchar* databg = cellRGB1.ptr<uchar>(m*8+i);
					for (int j = 0; j<8; j++) {
						data[(n*8+j)*3+0] = databg[(n*8+j)*3+0];
						data[(n*8+j)*3+1] = databg[(n*8+j)*3+1];
						data[(n*8+j)*3+2] = databg[(n*8+j)*3+2];
					}
				}
			}
		}
	}
	blur(cellRGB2, cellRGB2, Size(3, 3));
	for (int m = 0; m<inrows; m++) {
		for (int n = 0; n<incols; n++) {
			if (templet[m][n]=='1') {
				for (int i = 0; i<8; i++) {
					uchar* data = cellRGB2.ptr<uchar>(m*8+i);
					uchar* databg = cellRGB1.ptr<uchar>(m*8+i);
					for (int j = 0; j<8; j++) {
						data[(n*8+j)*3+0] = databg[(n*8+j)*3+0];
						data[(n*8+j)*3+1] = databg[(n*8+j)*3+1];
						data[(n*8+j)*3+2] = databg[(n*8+j)*3+2];
					}
				}
			}
		}
	}

	crtTime = clock();
	timestr.Format("%f", (float)(crtTime-startTime)/CLOCKS_PER_SEC);
	textstr = textstr + "完成，总用时 time:" + timestr + "s \r\n";
	m_edit1.SetWindowText(textstr);

	EnableWindow(TRUE);

	//namedWindow("picture3");
	namedWindow("picture3",0);
	imshow("picture3", cellRGB2);
	waitKey(0);
	
	return;
}


void CCellCutMFC0Dlg::OnBnClickedGrayopen()
{
	// TODO: 在此添加控件通知处理程序代码

	CString Img_path, Img_folder, file_name;
	//浏览文件打开图片对话框
	CFileDialog dlg(TRUE, _T("*.jpg"), NULL, OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY, "All Files (*.*)|*.*", NULL);
	dlg.m_ofn.lpstrTitle = _T("打开图片");
	if (dlg.DoModal()!=IDOK) return;

	clock_t startTime, crtTime;
	startTime = clock();
	textstr = "开始 time:0s \r\n";
	m_edit1.SetWindowText(textstr);

	EnableWindow(FALSE);
	Img_folder = dlg.GetFolderPath();
	Img_path = dlg.GetPathName();
	file_name = dlg.GetFileTitle();
	//f_SavePath = Img_folder.Left(Img_folder.ReverseFind('\\'));

	string srcStr;
	srcStr = Img_path.GetBuffer(0);
	Mat srcGRAY;
	srcGRAY = imread(srcStr, IMREAD_GRAYSCALE);
	if (srcGRAY.data==NULL)
	{
		MessageBox("请打开正确的图片!", "错误", MB_OK);
		EnableWindow(TRUE);
		return;
	}
	/*namedWindow("source");
	imshow("source", srcRGB);*/

	Mat edge;
	blur(srcGRAY, edge, Size(5, 5));
	Canny(edge, edge, 20, 45, 3);
	/*namedWindow("source");
	imshow("source", edge);
	waitKey(0);*/

	crtTime = clock();
	timestr.Format("%f", (float)(crtTime-startTime)/CLOCKS_PER_SEC);
	textstr = textstr + "读取成功 time:" + timestr + "s \r\n";
	m_edit1.SetWindowText(textstr);

	/*if (srcRGB.rows%8 != 0) {
	}*/

	int temprows = srcGRAY.rows/8;
	int tempcols = srcGRAY.cols/8;

	vector<vector<bool> > templet(temprows, vector<bool>(tempcols));

	double avg88, variance88;
	int countedge;
	int countcell = 0;
	for (int m = 0; m<temprows; m++) {
		for (int n = 0; n<tempcols; n++) {
			avg88 = variance88 = 0;
			countedge = 0;

			for (int i = 0; i<8; i++) {
				uchar* data = srcGRAY.ptr<uchar>(m*8+i);
				uchar* dataedge = edge.ptr<uchar>(m*8+i);
				for (int j = 0; j<8; j++) {
					avg88 = avg88 + data[n*8+j];
					if (dataedge[n*8+j]>200) {
						countedge = countedge + 1;
					}
				}
			}
			avg88 = avg88/64;

			for (int i = 0; i<8; i++) {
				uchar* data = srcGRAY.ptr<uchar>(m*8+i);
				for (int j = 0; j<8; j++) {
					variance88 = variance88 + pow(data[n*8+j] - avg88, 2);
				}
			}
			variance88 = variance88/64;			

			if (avg88>210 && variance88<80 && countedge==0) {
				templet[m][n] = 0;
			}
			else {
				templet[m][n] = 1;
				countcell++;
			}
		}
	}
	crtTime = clock();
	timestr.Format("%f", (float)(crtTime-startTime)/CLOCKS_PER_SEC);
	textstr = textstr + "分类完成 time:" + timestr + "s \r\n";
	m_edit1.SetWindowText(textstr);

	Mat cell2code(8*(1+countcell/100), 100*8, CV_8UC1, Scalar(0));
	int crtcell = 0;
	int crtrow = 0;
	for (int m = 0; m<temprows; m++) {
		for (int n = 0; n<tempcols; n++) {
			if (templet[m][n]==1) {
				for (int i = 0; i<8; i++) {
					uchar* data = srcGRAY.ptr<uchar>(m*8+i);
					uchar* datacell = cell2code.ptr<uchar>(crtrow*8+i);
					for (int j = 0; j<8; j++) {
						datacell[crtcell*8+j] = data[n*8+j];
					}
				}
				crtcell++;
				if (crtcell>99) {
					crtcell = 0;
					crtrow++;
				}
			}
		}
	}
	/*namedWindow("source");
	imshow("source", cell2code);
	waitKey(0);*/

	vector<uchar> buf;
	imencode(".jpg", cell2code, buf);

	crtTime = clock();
	timestr.Format("%f", (float)(crtTime-startTime)/CLOCKS_PER_SEC);
	textstr = textstr + "编码完成 time:" + timestr + "s \r\n";
	m_edit1.SetWindowText(textstr);

	CString save_pathCS = Img_folder + "\\" + file_name;
	std::string save_path;
	save_path = save_pathCS.GetBuffer(0);
	save_path = save_path + ".xwgray";

	ofstream file2(save_path.c_str(), std::ios::out | std::ios::binary | ios::ate);
	file2<<temprows<<' '<<tempcols;
	file2<<endl;
	for (int m = 0; m<temprows; m++) {
		for (int n = 0; n<tempcols; n++) {
			file2<<templet[m][n];
		}
	}
	file2<<endl;

	file2.write((char*)&buf[0], buf.size()*sizeof(uchar));
	file2.close();

	crtTime = clock();
	timestr.Format("%f", (float)(crtTime-startTime)/CLOCKS_PER_SEC);
	textstr = textstr + "保存完成，总用时 time:" + timestr + "s \r\n";
	m_edit1.SetWindowText(textstr);

	EnableWindow(TRUE);
	return;
}


void CCellCutMFC0Dlg::OnBnClickedGraydecode()
{
	// TODO: 在此添加控件通知处理程序代码
	CString Img_path, Img_folder, file_name;
	//浏览文件打开图片对话框
	CFileDialog dlg(TRUE, _T("*.xwgray"), NULL, OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY, "xw Files (*.xwgray)|*.xwgray", NULL);
	dlg.m_ofn.lpstrTitle = _T("打开xwgray");
	if (dlg.DoModal()!=IDOK) return;

	clock_t startTime, crtTime;
	startTime = clock();
	textstr = "开始 time:0s \r\n";
	m_edit1.SetWindowText(textstr);

	EnableWindow(FALSE);
	Img_folder = dlg.GetFolderPath();
	Img_path = dlg.GetPathName();
	file_name = dlg.GetFileTitle();
	//f_SavePath = Img_folder.Left(Img_folder.ReverseFind('\\'));

	string img2dcdStr;
	img2dcdStr = Img_path.GetBuffer(0);

	ifstream infile;
	infile.open(img2dcdStr, std::ios::in | std::ios::binary | std::ios::ate);
	std::streampos sizeend = infile.tellg();

	crtTime = clock();
	timestr.Format("%f", (float)(crtTime-startTime)/CLOCKS_PER_SEC);
	textstr = textstr + "打开xw文件成功，开始读取数据 time:" + timestr + "s \r\n";
	m_edit1.SetWindowText(textstr);

	infile.seekg(0, std::ios::beg);
	int inrows, incols;
	char enter;
	infile>>inrows>>incols;
	infile.get(enter);
	if (enter!='\n')
	{
		MessageBox("文件读取发生错误 after infile>inrows>incols", "错误", MB_OK);
		EnableWindow(TRUE);
		return;
	}

	vector<vector<char> > templet(inrows, vector<char>(incols));
	for (int m = 0; m<inrows; m++) {
		for (int n = 0; n<incols; n++) {
			infile.get(templet[m][n]);
		}
	}
	infile.get(enter);
	if (enter!='\n')
	{
		MessageBox("文件读取发生错误 after infileget(templet)", "错误", MB_OK);
		EnableWindow(TRUE);
		return;
	}

	std::streampos sizestart = infile.tellg();
	std::streampos size = sizeend - sizestart;
	std::string buffer(size, ' ');
	infile.read(&buffer[0], size);
	infile.close();

	crtTime = clock();
	timestr.Format("%f", (float)(crtTime-startTime)/CLOCKS_PER_SEC);
	textstr = textstr + "读取数据成功，开始解码 time:" + timestr + "s \r\n";
	m_edit1.SetWindowText(textstr);

	std::vector<char> vec_data(&buffer[0], &buffer[0] + size);
	cv::Mat cells = cv::imdecode(vec_data, IMREAD_GRAYSCALE);

	Mat cellGRAY1(inrows*8, incols*8, CV_8UC1, Scalar(255));
	int crtcell = 0;
	int crtrow = 0;
	for (int m = 0; m<inrows; m++) {
		for (int n = 0; n<incols; n++) {
			if (templet[m][n]=='1') {
				for (int i = 0; i<8; i++) {
					uchar* data = cellGRAY1.ptr<uchar>(m*8+i);
					uchar* datacell = cells.ptr<uchar>(crtrow*8+i);
					for (int j = 0; j<8; j++) {
						data[n*8+j] = datacell[crtcell*8+j];
					}
				}
				crtcell++;
				if (crtcell>99) {
					crtcell = 0;
					crtrow++;
				}
			}
		}
	}
	/*namedWindow("picture3");
	imshow("picture3", cellGRAY1);
	waitKey(0);*/
	crtTime = clock();
	timestr.Format("%f", (float)(crtTime-startTime)/CLOCKS_PER_SEC);
	textstr = textstr + "解码完成，开始填充背景 time:" + timestr + "s \r\n";
	m_edit1.SetWindowText(textstr);
	/*namedWindow("picture3");
	imshow("picture3", bg);
	waitKey(0);*/
	for (int m = 0; m<inrows; m++) {
		for (int n = 0; n<incols; n++) {
			if (templet[m][n]=='0') {
				for (int i = 0; i<8; i++) {
					uchar* data = cellGRAY1.ptr<uchar>(m*8+i);
					uchar* databg = graybg.ptr<uchar>(m*8+i);
					for (int j = 0; j<8; j++) {
						data[n*8+j] = databg[n*8+j];
					}
				}
			}
		}
	}
	crtTime = clock();
	timestr.Format("%f", (float)(crtTime-startTime)/CLOCKS_PER_SEC);
	textstr = textstr + "完成，总用时 time:" + timestr + "s \r\n";
	m_edit1.SetWindowText(textstr);

	EnableWindow(TRUE);

	namedWindow("picture3");
	imshow("picture3", cellGRAY1);
	waitKey(0);

	return;
}


void CCellCutMFC0Dlg::OnBnClickedSave()
{
	// TODO: 在此添加控件通知处理程序代码


}


