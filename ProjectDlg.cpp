
// ProjectDlg.cpp : 구현 파일
//

#define CRTDBG_MAP_ALLOC

#include "stdafx.h"
#include "Project.h"
#include "ProjectDlg.h"
#include "afxdialogex.h"
#include "DFT2D.h"
#include <math.h>
#include <algorithm>

#define BUTTERWORTH_ORDER 2
#define PI 3.141597
#define HEIGHT 480
#define WIDTH 640

typedef unsigned char BYTE;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

enum Img_idx
{
	In,
	DFT_Real,
	IDFT_Real
};

CornerPoints leftHrPoints, rightHrPoints;	//Harris검출 된 영역을 위한 포인트
CornerPoints matchingPoints;	//매칭 된 영역을 위한 포인트

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CProjectDlg 대화 상자



CProjectDlg::CProjectDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_PROJECT_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CProjectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PICTURE_IN, m_PicOri);
	DDX_Control(pDX, IDC_PICTURE_OUT, m_PicProc);
}

BEGIN_MESSAGE_MAP(CProjectDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_COMMAND(ID_FILE_FILEOPEN, &CProjectDlg::OnFileOpenBtnClick)
	ON_COMMAND(ID_FILE_FILECLOSE, &CProjectDlg::OnFileFileclose)
	ON_COMMAND(ID_FILE_FILEOPEN32772, &CProjectDlg::OnFileFileopenRight)
	ON_BN_CLICKED(IDC_Cancel, &CProjectDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_Harris, &CProjectDlg::OnBnClickedHarris)
	ON_BN_CLICKED(IDC_BUTTON2, &CProjectDlg::OnBnClickedMatching)
	ON_BN_CLICKED(IDC_BUTTON4, &CProjectDlg::OnBnClickedBlend)
	ON_BN_CLICKED(IDC_BUTTON3, &CProjectDlg::OnBnClickedFiltering)
END_MESSAGE_MAP()


// CProjectDlg 메시지 처리기

BOOL CProjectDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CProjectDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CProjectDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CProjectDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
void CProjectDlg::DisplayImage(Mat DispMat, bool blnOut)
{
	if (blnOut)
	{
		GetDlgItem(IDC_PICTURE_IN)->GetWindowRect(rect);
		ScreenToClient(rect);
		m_pDC = m_PicOri.GetDC();
		m_PicOri.GetClientRect(&rect);
	}
	else
	{
		GetDlgItem(IDC_PICTURE_OUT)->GetWindowRect(rect);
		ScreenToClient(rect);
		m_pDC = m_PicProc.GetDC();
		m_PicProc.GetClientRect(&rect);
	}

	IplImage* Ori = &IplImage(DispMat);
	IplImage* Resize = NULL;

	ResizeImage(Ori, &Resize, &rect);
	DisplayBitmap(m_pDC, rect, Resize);
}
void CProjectDlg::DisplayBitmap(CDC* pDC, CRect rect, IplImage* DispIplImage)
{
	BITMAPINFO bitmapInfo;
	bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitmapInfo.bmiHeader.biPlanes = 1;
	bitmapInfo.bmiHeader.biCompression = BI_RGB;
	bitmapInfo.bmiHeader.biXPelsPerMeter = 100;
	bitmapInfo.bmiHeader.biYPelsPerMeter = 100;
	bitmapInfo.bmiHeader.biClrUsed = 0;
	bitmapInfo.bmiHeader.biClrImportant = 0;
	bitmapInfo.bmiHeader.biSizeImage = 0;
	bitmapInfo.bmiHeader.biWidth = DispIplImage->width;
	bitmapInfo.bmiHeader.biHeight = DispIplImage->height;

	if (DispIplImage->nChannels == 3)
	{
		IplImage* ReverseImg = cvCreateImage(CvSize(DispIplImage->width, DispIplImage->height), 8, 3);

		for (int h = 0; h < DispIplImage->height; h++)
		{
			for (int w = 0; w < DispIplImage->width; w++)
			{
				ReverseImg->imageData[h* DispIplImage->widthStep + w * 3] = DispIplImage->imageData[(DispIplImage->height - h)*DispIplImage->widthStep + w * 3];
				ReverseImg->imageData[h* DispIplImage->widthStep + w * 3 + 1] = DispIplImage->imageData[(DispIplImage->height - h)*DispIplImage->widthStep + w * 3 + 1];
				ReverseImg->imageData[h* DispIplImage->widthStep + w * 3 + 2] = DispIplImage->imageData[(DispIplImage->height - h)*DispIplImage->widthStep + w * 3 + 2];
			}
		}

		bitmapInfo.bmiHeader.biBitCount = DispIplImage->depth*DispIplImage->nChannels;
		pDC->SetStretchBltMode(COLORONCOLOR);
		::StretchDIBits(pDC->GetSafeHdc(), rect.left, rect.top, rect.right, rect.bottom, 0, 0, DispIplImage->width, DispIplImage->height, ReverseImg->imageData, &bitmapInfo, DIB_RGB_COLORS, SRCCOPY);

		cvReleaseImage(&ReverseImg);
	}
	else
	{
		IplImage* ReverseImg = cvCreateImage(cvGetSize(DispIplImage), DispIplImage->depth, 1);

		for (int h = 0; h < DispIplImage->height; h++)
		{
			for (int w = 0; w < DispIplImage->width; w++)
			{
				ReverseImg->imageData[h*DispIplImage->widthStep + w] = DispIplImage->imageData[(DispIplImage->height - h)*DispIplImage->widthStep + w];
			}
		}

		IplImage* tmp = cvCreateImage(cvGetSize(DispIplImage), DispIplImage->depth, 3);
		cvCvtColor(ReverseImg, tmp, CV_GRAY2BGR);

		bitmapInfo.bmiHeader.biBitCount = tmp->depth*tmp->nChannels;
		pDC->SetStretchBltMode(COLORONCOLOR);
		::StretchDIBits(pDC->GetSafeHdc(), rect.left, rect.top, rect.right, rect.bottom, 0, 0, tmp->width, tmp->height, tmp->imageData, &bitmapInfo, DIB_RGB_COLORS, SRCCOPY);

		cvReleaseImage(&ReverseImg);
		cvReleaseImage(&tmp);
	}
}
void CProjectDlg::ResizeImage(IplImage* src, IplImage** dst, CRect* rect)
{
	int ndst_width = 0, ndst_height = 0;

	if (src->width > src->height) {
		ndst_width = rect->Width();
		ndst_height = (src->height*rect->Width()) / src->width;
	}
	else {
		ndst_width = (src->width*rect->Height()) / src->height;
		ndst_height = rect->Height();
	}

	(*dst) = cvCreateImage(cvSize(ndst_width, ndst_height), IPL_DEPTH_8U, src->nChannels);

	cvResize(src, (*dst));
	rect->right = rect->left + ndst_width;
	rect->bottom = rect->top + ndst_height;
}
unsigned char** MemAlloc2D(int nHeight, int nWidth, unsigned char nlnitVal)
{
	unsigned char** rtn = new unsigned char*[nHeight];
	for (int n = 0; n < nHeight; n++)
	{
		rtn[n] = new unsigned char[nWidth];
		memset(rtn[n], nlnitVal, sizeof(unsigned char)*nWidth);
	}
	return rtn;
}
void MemFree2D(unsigned char** Mem, int nHeight)
{
	for (int n = 0; n < nHeight; n++)
	{
		delete[] Mem[n];
	}
	delete[] Mem;
}

void CProjectDlg::OnFileOpenBtnClick()
{
	CFileDialog dlg(TRUE);

	if (isImageOpened)
	{
		if (ch_in_1 != NULL)
		{
			MemFree2D(ch_in_1, nHeight_in);
		}
		if (ch_in_2 != NULL)
		{
			if (nFormat == FORMAT_YUV420)
			{
				MemFree2D(ch_in_2, nHeight_in / 2);
			}
			else MemFree2D(ch_in_2, nHeight_in);
		}
		if (ch_in_3 != NULL)
		{
			if (nFormat == FORMAT_YUV420)
			{
				MemFree2D(ch_in_3, nHeight_in / 2);
			}
			else MemFree2D(ch_in_3, nHeight_in);
		}
		if (ch_in_gray != NULL)
		{
			MemFree2D(ch_in_gray, nHeight_in);
		}
	}

	if (dlg.DoModal() == IDOK)
	{
		if (dlg.GetFileExt() != "jpg"&&dlg.GetFileExt() != "JPG"&&dlg.GetFileExt() != "raw"&&dlg.GetFileExt() != "yuv"&&dlg.GetFileExt() != "RAW")
		{
			MessageBox("JPG 또는 YUV, Gray 파일이 아닙니다.");
			return;
		}


		if (dlg.GetFileExt() == "jpg" || dlg.GetFileExt() == "JPG")
		{
			Ori_Img = imread((const char*)dlg.GetPathName());
			nHeight_in = Ori_Img.rows;
			nWidth_in = Ori_Img.cols;
			if (Ori_Img.channels() == 3)
			{
				ch_in_1 = MemAlloc2D(nHeight_in, nWidth_in, 0);
				ch_in_2 = MemAlloc2D(nHeight_in, nWidth_in, 0);
				ch_in_3 = MemAlloc2D(nHeight_in, nWidth_in, 0);

				for (int h = 0; h < nHeight_in; h++)
				{
					for (int w = 0; w < nWidth_in; w++)
					{
						Vec3b RGB = Ori_Img.at<Vec3b>(h, w);
						ch_in_1[h][w] = RGB[2];
						ch_in_2[h][w] = RGB[1];
						ch_in_3[h][w] = RGB[0];
					}
				}
				nFormat = FORMAT_RGB;
			}
			else
			{
				ch_in_gray = MemAlloc2D(nHeight_in, nWidth_in, 0);
				for (int h = 0; h < nHeight_in; h++)
				{
					for (int w = 0; w < nWidth_in; w++)
					{
						ch_in_gray[h][w] = Ori_Img.at<unsigned char>(h, w);
					}
				}
				nFormat = FORMAT_GRAY;
			}
			isImageOpened = true;
			DisplayImage(Ori_Img, true);
		}
		else
		{
			CRawInfoDlg RawInfoDlg;
			if (RawInfoDlg.DoModal() == IDOK)
			{
				nHeight_in = RawInfoDlg.GetRawHeight();
				nWidth_in = RawInfoDlg.GetRawWidth();
				nFormat = RawInfoDlg.GetRawFormat();

				if (nHeight_in == 0 || nWidth_in == 0)
				{
					MessageBox("값을 잘못 입력하였습니다.");
					return;
				}

				FILE *fp_in;

				fopen_s(&fp_in, (const char*)dlg.GetPathName(), "rb");

				if (nFormat == FORMAT_GRAY)
				{
					ch_in_gray = MemAlloc2D(nHeight_in, nWidth_in, 0);

					for (int h = 0; h < nHeight_in; h++)
					{
						fread(ch_in_gray[h], sizeof(unsigned char), nWidth_in, fp_in);
					}

					Ori_Img = Mat(nHeight_in, nWidth_in, CV_8UC1);

					for (int h = 0; h < nHeight_in; h++)
					{
						for (int w = 0; w < nWidth_in; w++)
						{
							Ori_Img.at<unsigned char>(h, w) = ch_in_gray[h][w];
						}
					}
				}
				else if (nFormat == FORMAT_YUV444)
				{
					ch_in_1 = MemAlloc2D(nHeight_in, nWidth_in, 0);
					ch_in_2 = MemAlloc2D(nHeight_in, nWidth_in, 0);
					ch_in_3 = MemAlloc2D(nHeight_in, nWidth_in, 0);
					for (int h = 0; h < nHeight_in; h++)
					{
						fread(ch_in_1[h], sizeof(unsigned char), nWidth_in, fp_in);
					}
					for (int h = 0; h < nHeight_in; h++)
					{
						fread(ch_in_2[h], sizeof(unsigned char), nWidth_in, fp_in);
					}
					for (int h = 0; h < nHeight_in; h++)
					{
						fread(ch_in_3[h], sizeof(unsigned char), nWidth_in, fp_in);
					}

					Ori_Img = Mat(nHeight_in, nWidth_in, CV_8UC3);
					for (int h = 0; h < nHeight_in; h++)
					{
						for (int w = 0; w < nWidth_in; w++)
						{
							Vec3b BGR;

							BGR[2] = static_cast<unsigned char>(ch_in_1[h][w] + 1.402*(ch_in_3[h][w]));
							BGR[1] = static_cast<unsigned char>(ch_in_1[h][w] - 0.344*(ch_in_2[h][w]) - 0.714*(ch_in_3[h][w]));
							BGR[0] = static_cast<unsigned char>(ch_in_1[h][w] + 1.772*(ch_in_2[h][w]));

							Ori_Img.at<Vec3b>(h, w) = BGR;
						}
					}
				}
				else
				{
					ch_in_1 = MemAlloc2D(nHeight_in, nWidth_in, 0);
					ch_in_2 = MemAlloc2D(nHeight_in / 2, nWidth_in / 2, 0);
					ch_in_3 = MemAlloc2D(nHeight_in / 2, nWidth_in / 2, 0);

					for (int h = 0; h < nHeight_in; h++)
					{
						fread(ch_in_1[h], sizeof(unsigned char), nWidth_in, fp_in);
					}
					for (int h = 0; h < nHeight_in / 2; h++)
					{
						fread(ch_in_2[h], sizeof(unsigned char), nWidth_in / 2, fp_in);
					}
					for (int h = 0; h < nHeight_in / 2; h++)
					{
						fread(ch_in_3[h], sizeof(unsigned char), nWidth_in / 2, fp_in);
					}

					Ori_Img = Mat(nHeight_in, nWidth_in, CV_8UC3);

					for (int h = 0; h < nHeight_in / 2; h++)
					{
						for (int w = 0; w < nWidth_in / 2; w++)
						{
							for (int y = 0; y < 2; y++)
							{
								for (int x = 0; x < 2; x++)
								{
									Vec3b BGR;

									BGR[2] = static_cast<unsigned char>(ch_in_1[h * 2 + y][w * 2 + x] + 1.402*(ch_in_3[h][w]));
									BGR[1] = static_cast<unsigned char>(ch_in_1[h * 2 + y][w * 2 + x] - 0.344*(ch_in_2[h][w]) - 0.714*(ch_in_3[h][w]));
									BGR[0] = static_cast<unsigned char>(ch_in_1[h * 2 + y][w * 2 + x] + 1.772*(ch_in_2[h][w]));

									Ori_Img.at<Vec3b>(h * 2 + y, w * 2 + x) = BGR;
								}
							}
						}
					}
				}
				fclose(fp_in);

				isImageOpened = true;
				DisplayImage(Ori_Img, true);
			}
		}
	}
}
void CProjectDlg::OnFileFileclose()
{
	if (isImageOpened)
	{
		if (ch_in_1 != NULL)
		{
			MemFree2D(ch_in_1, nHeight_in);
		}
		if (ch_in_2 != NULL)
		{
			if (nFormat == FORMAT_YUV420)
			{
				MemFree2D(ch_in_2, nHeight_in / 2);
			}
			else MemFree2D(ch_in_2, nHeight_in);
		}
		if (ch_in_3 != NULL)
		{
			if (nFormat == FORMAT_YUV420)
			{
				MemFree2D(ch_in_3, nHeight_in / 2);
			}
			else MemFree2D(ch_in_3, nHeight_in);
		}
		if (ch_in_gray != NULL)
		{
			MemFree2D(ch_in_gray, nHeight_in);
		}
	}
	::SendMessage(this->m_hWnd, WM_CLOSE, NULL, NULL);
}
void CProjectDlg::OnFileFileopenRight()
{
	CFileDialog dlg(TRUE);

	if (isImageOpened)
	{
		if (ch_in_1 != NULL)
		{
			MemFree2D(ch_in_1, nHeight_in);
		}
		if (ch_in_2 != NULL)
		{
			if (nFormat == FORMAT_YUV420)
			{
				MemFree2D(ch_in_2, nHeight_in / 2);
			}
			else MemFree2D(ch_in_2, nHeight_in);
		}
		if (ch_in_3 != NULL)
		{
			if (nFormat == FORMAT_YUV420)
			{
				MemFree2D(ch_in_3, nHeight_in / 2);
			}
			else MemFree2D(ch_in_3, nHeight_in);
		}
		if (ch_in_gray2 != NULL)
		{
			MemFree2D(ch_in_gray2, nHeight_in);
		}
	}

	if (dlg.DoModal() == IDOK)
	{
		if (dlg.GetFileExt() != "jpg"&&dlg.GetFileExt() != "JPG"&&dlg.GetFileExt() != "raw"&&dlg.GetFileExt() != "yuv"&&dlg.GetFileExt() != "RAW")
		{
			MessageBox("JPG 또는 YUV, Gray 파일이 아닙니다.");
			return;
		}


		if (dlg.GetFileExt() == "jpg" || dlg.GetFileExt() == "JPG")
		{
			Proc_Img = imread((const char*)dlg.GetPathName());
			nHeight_in = Proc_Img.rows;
			nWidth_in = Proc_Img.cols;
			if (Proc_Img.channels() == 3)
			{
				ch_in_1 = MemAlloc2D(nHeight_in, nWidth_in, 0);
				ch_in_2 = MemAlloc2D(nHeight_in, nWidth_in, 0);
				ch_in_3 = MemAlloc2D(nHeight_in, nWidth_in, 0);

				for (int h = 0; h < nHeight_in; h++)
				{
					for (int w = 0; w < nWidth_in; w++)
					{
						Vec3b RGB = Proc_Img.at<Vec3b>(h, w);
						ch_in_1[h][w] = RGB[2];
						ch_in_2[h][w] = RGB[1];
						ch_in_3[h][w] = RGB[0];
					}
				}
				nFormat = FORMAT_RGB;
			}
			else
			{
				ch_in_gray2 = MemAlloc2D(nHeight_in, nWidth_in, 0);
				for (int h = 0; h < nHeight_in; h++)
				{
					for (int w = 0; w < nWidth_in; w++)
					{
						ch_in_gray2[h][w] = Proc_Img.at<unsigned char>(h, w);
					}
				}
				nFormat = FORMAT_GRAY;
			}
			isImageOpened = true;
			DisplayImage(Proc_Img, false);
		}
		else
		{//File Format -- YUV

			CRawInfoDlg RawInfoDlg;
			if (RawInfoDlg.DoModal() == IDOK)
			{
				nHeight_in = RawInfoDlg.GetRawHeight();
				nWidth_in = RawInfoDlg.GetRawWidth();
				nFormat = RawInfoDlg.GetRawFormat();

				if (nHeight_in == 0 || nWidth_in == 0)
				{
					MessageBox("값을 잘못 입력하였습니다.");
					return;
				}

				FILE *fp_in;

				fopen_s(&fp_in, (const char*)dlg.GetPathName(), "rb");

				if (nFormat == FORMAT_GRAY)
				{
					
					ch_in_gray2 = MemAlloc2D(nHeight_in, nWidth_in, 0);	//오른쪽 영상
					for (int h = 0; h < nHeight_in; h++)
					{
						fread(ch_in_gray2[h], sizeof(unsigned char), nWidth_in, fp_in);
					}


					Proc_Img = Mat(nHeight_in, nWidth_in, CV_8UC1);

					for (int h = 0; h < nHeight_in; h++)
					{
						for (int w = 0; w < nWidth_in; w++)
						{
							Proc_Img.at<unsigned char>(h, w) = ch_in_gray2[h][w];
						}
					}
				}
				else if (nFormat == FORMAT_YUV444)
				{
					ch_in_1 = MemAlloc2D(nHeight_in, nWidth_in, 0);
					ch_in_2 = MemAlloc2D(nHeight_in, nWidth_in, 0);
					ch_in_3 = MemAlloc2D(nHeight_in, nWidth_in, 0);
					for (int h = 0; h < nHeight_in; h++)
					{
						fread(ch_in_1[h], sizeof(unsigned char), nWidth_in, fp_in);
					}
					for (int h = 0; h < nHeight_in; h++)
					{
						fread(ch_in_2[h], sizeof(unsigned char), nWidth_in, fp_in);
					}
					for (int h = 0; h < nHeight_in; h++)
					{
						fread(ch_in_3[h], sizeof(unsigned char), nWidth_in, fp_in);
					}

					Proc_Img = Mat(nHeight_in, nWidth_in, CV_8UC3);
					for (int h = 0; h < nHeight_in; h++)
					{
						for (int w = 0; w < nWidth_in; w++)
						{
							Vec3b BGR;

							BGR[2] = static_cast<unsigned char>(ch_in_1[h][w] + 1.402*(ch_in_3[h][w]));
							BGR[1] = static_cast<unsigned char>(ch_in_1[h][w] - 0.344*(ch_in_2[h][w]) - 0.714*(ch_in_3[h][w]));
							BGR[0] = static_cast<unsigned char>(ch_in_1[h][w] + 1.772*(ch_in_2[h][w]));

							Proc_Img.at<Vec3b>(h, w) = BGR;
						}
					}
				}
				else
				{
					ch_in_1 = MemAlloc2D(nHeight_in, nWidth_in, 0);
					ch_in_2 = MemAlloc2D(nHeight_in / 2, nWidth_in / 2, 0);
					ch_in_3 = MemAlloc2D(nHeight_in / 2, nWidth_in / 2, 0);

					for (int h = 0; h < nHeight_in; h++)
					{
						fread(ch_in_1[h], sizeof(unsigned char), nWidth_in, fp_in);
					}
					for (int h = 0; h < nHeight_in / 2; h++)
					{
						fread(ch_in_2[h], sizeof(unsigned char), nWidth_in / 2, fp_in);
					}
					for (int h = 0; h < nHeight_in / 2; h++)
					{
						fread(ch_in_3[h], sizeof(unsigned char), nWidth_in / 2, fp_in);
					}

					Proc_Img = Mat(nHeight_in, nWidth_in, CV_8UC3);

					for (int h = 0; h < nHeight_in / 2; h++)
					{
						for (int w = 0; w < nWidth_in / 2; w++)
						{
							for (int y = 0; y < 2; y++)
							{
								for (int x = 0; x < 2; x++)
								{
									Vec3b BGR;

									BGR[2] = static_cast<unsigned char>(ch_in_1[h * 2 + y][w * 2 + x] + 1.402*(ch_in_3[h][w]));
									BGR[1] = static_cast<unsigned char>(ch_in_1[h * 2 + y][w * 2 + x] - 0.344*(ch_in_2[h][w]) - 0.714*(ch_in_3[h][w]));
									BGR[0] = static_cast<unsigned char>(ch_in_1[h * 2 + y][w * 2 + x] + 1.772*(ch_in_2[h][w]));

									Proc_Img.at<Vec3b>(h * 2 + y, w * 2 + x) = BGR;
								}
							}
						}
					}
				}
				fclose(fp_in);

				isImageOpened = true;
				DisplayImage(Proc_Img, false);
			}
		}
	}
}

void CProjectDlg::OnBnClickedCancel()
{
	if (isImageOpened)
	{
		if (ch_in_1 != NULL)
		{
			MemFree2D(ch_in_1, nHeight_in);
		}
		if (ch_in_2 != NULL)
		{
			if (nFormat == FORMAT_YUV420)
			{
				MemFree2D(ch_in_2, nHeight_in / 2);
			}
			else MemFree2D(ch_in_2, nHeight_in);
		}
		if (ch_in_3 != NULL)
		{
			if (nFormat == FORMAT_YUV420)
			{
				MemFree2D(ch_in_3, nHeight_in / 2);
			}
			else MemFree2D(ch_in_3, nHeight_in);
		}
		if (ch_in_gray != NULL)
		{
			MemFree2D(ch_in_gray, nHeight_in);
		}
	}
	::SendMessage(this->m_hWnd, WM_CLOSE, NULL, NULL);
}
void CProjectDlg::OnBnClickedHarris()
{

	//2D Memory Allocation

	if (isImageOpened == true && nFormat == FORMAT_GRAY)
	{
		if (Ori_Img.data != NULL)	//밝은 이미지
		{
			unsigned char** Out_left = MemAlloc2D(nHeight_in, nWidth_in, 0);
			double threshold = 20000.0;

			HarrisDetection(ch_in_gray, Out_left, nHeight_in, nWidth_in, 0.1,threshold,&leftHrPoints);
			//왼쪽 이미지, 결과 이미지, 높이, 너비, K값, Threshold값, 추출 된 영역 저장 구조체

			printf(" : %d", leftHrPoints.num);
			DisplayImage(GrayToMat(Out_left, nHeight_in, nWidth_in), true);
		}

		if (Proc_Img.data != NULL)	//어두운 이미지
		{
			unsigned char** Out_right = MemAlloc2D(nHeight_in, nWidth_in, 0);
			double threshold = 2000.0;

			HarrisDetection(ch_in_gray2, Out_right, nHeight_in, nWidth_in, 0.01, threshold,&rightHrPoints);
			//왼쪽 이미지, 결과 이미지, 높이, 너비, K값, Threshold값, 추출 된 영역 저장 구조체

			printf(" : %d", rightHrPoints.num);
			DisplayImage(GrayToMat(Out_right, nHeight_in, nWidth_in), false);
		}
	}
}
void CProjectDlg::OnBnClickedMatching() {	//Matching -> using heming weghit algorithm
	if (isImageOpened == true && nFormat == FORMAT_GRAY)
	{
		if (Ori_Img.data != NULL && Proc_Img.data != NULL)	//두 이미지 둘다 비교하기 위해 null이 아니여야 함
		{
			unsigned char**Pad = Padding(ch_in_gray, HEIGHT, WIDTH, 5);	//window로 계속 비교하기위해 필터 값이 5인 padding이미지로 순회
			unsigned char**Pad2 = Padding(ch_in_gray2, HEIGHT, WIDTH, 5);
			unsigned char** window1;	//5*5 window생성
			unsigned char** window2;

			matchingPoints.num = 0;	//매칭 된 영역 초기화

			for (int i = 0; i < leftHrPoints.num; i++) {
				for (int j = 0; j < rightHrPoints.num; j++) {
					//if (leftHrPoints.x[i] == rightHrPoints.x[j] && leftHrPoints.y[i] == rightHrPoints.y[j]) 
					if (abs(leftHrPoints.x[i] - rightHrPoints.x[j]) <= 10 && abs(leftHrPoints.y[i] - rightHrPoints.y[j]) <= 10) {
						//Hamming Weight검사
						window1 = HammingWeight(Pad, 5, (leftHrPoints.y[i] + 2), (leftHrPoints.x[i] + 2));
						window2 = HammingWeight(Pad2, 5, (rightHrPoints.y[j] + 2), (rightHrPoints.x[j] + 2));

						int bitCheck = 0;

						for (int i = 0; i < 5; i++) {	//window내의 값 비교를 위해
							for (int j = 0; j < 5; j++) {
								if (window1[i][j] == window2[i][j]) {
									bitCheck++;
								}
							}
						}
						printf("%d ", bitCheck);
						if (bitCheck >= 25) {
							matchingPoints.x[matchingPoints.num] = leftHrPoints.x[i];
							matchingPoints.y[matchingPoints.num] = leftHrPoints.y[i];
							matchingPoints.num++;
						}
					}
				}
			}
			unsigned char **testImg = MemAlloc2D(HEIGHT, WIDTH, 0);
			////////////결정된 점을 흰색으로 표시////////////////
			for (int i = 0; i <matchingPoints.num; i++)
			{
				//object_img[cp.y[i]][cp.x[i]] = 255; //원래 이밎 test하기 위해 임시로 testImg적용
				testImg[matchingPoints.y[i]][matchingPoints.x[i]] = 255;

			}

			DisplayImage(GrayToMat(testImg, nHeight_in, nWidth_in), false); //matching 오른쪽 화면에 표시
		}
	}
}
void HarrisDetection(unsigned char ** object_img, unsigned char** Out, int nHeight, int nWidth, double k, double THRESHOLD, CornerPoints* cp)
{
	int i, j, x, y;
	int w = 640, h = 480;
	double **dx2, **dy2, **dxy;

	dx2 = MemAllocDouble2D(h, w, 0);
	dy2 = MemAllocDouble2D(h, w, 0);
	dxy = MemAllocDouble2D(h, w, 0);

	double tx = 0, ty = 0;
	for (j = 1; j < h - 1; j++) {
		for (i = 1; i < w - 1; i++) {
			tx = (object_img[j - 1][i + 1] + object_img[j][i + 1] + object_img[j + 1][i + 1]
				- object_img[j - 1][i - 1] - object_img[j][i - 1] - object_img[j + 1][i - 1]) / 6.0;

			ty = (object_img[j + 1][i - 1] + object_img[j + 1][i] + object_img[j + 1][i + 1]
				- object_img[j - 1][i - 1] - object_img[j - 1][i] - object_img[j - 1][i + 1]) / 6.0;

			dx2[j][i] = tx*tx;
			dy2[j][i] = ty*ty;
			dxy[j][i] = tx*ty;
		}
	}

	////////////////////가우시안 필터////////////////////////////
	double **gdx2, **gdy2, **gdxy;
	gdx2 = MemAllocDouble2D(h, w, 0);
	gdy2 = MemAllocDouble2D(h, w, 0);
	gdxy = MemAllocDouble2D(h, w, 0);

	double **g = GaussianFilter(5, 1.0);	//가우시안 필터 생성

	double tx2, ty2, txy;
	for (int j = 2; j < h - 2; j++) {
		for (int i = 2; i < w - 2; i++) {
			tx2 = ty2 = txy = 0;
			for (int y = 0; y < 5; y++) {
				for (int x = 0; x < 5; x++) {
					tx2 += (dx2[j + y - 2][i + x - 2] * g[y][x]);
					ty2 += (dy2[j + y - 2][i + x - 2] * g[y][x]);
					txy += (dxy[j + y - 2][i + x - 2] * g[y][x]);
				}
				gdx2[j][i] = tx2;
				gdy2[j][i] = ty2;
				gdxy[j][i] = txy;
				//printf("%f %f %f", tx2, ty2, txy);
			}
		}
	}

	////////////코너 응답 함수 생성////////////////
	double **crf;
	crf = MemAllocDouble2D(h, w, 0);

	for (j = 2; j < h - 2; j++) {
		for (i = 2; i < w - 2; i++) {
			crf[j][i] = (gdx2[j][i] * gdy2[j][i] - gdxy[j][i] * gdxy[j][i])
				- k*(gdx2[j][i] + gdy2[j][i])*(gdx2[j][i] + gdy2[j][i]);
			//printf("%lf ", crf[j][i]);
		}
	}

	////////////코너 포인트 결정////////////////
	cp->num = 0;

	for (j = 2; j < h - 2; j++)	{
		for (i = 2; i < w - 2; i++)	{
			if (crf[j][i] > THRESHOLD) {
				if (crf[j][i] > crf[j - 1][i] && crf[j][i] > crf[j - 1][i + 1] &&
					crf[j][i] > crf[j][i + 1] && crf[j][i] > crf[j + 1][i + 1] &&
					crf[j][i] > crf[j + 1][i] && crf[j][i] > crf[j + 1][i - 1] &&
					crf[j][i] > crf[j][i - 1] && crf[j][i] > crf[j - 1][i - 1])	{

					if (cp->num < MAX_CORNER) {
						cp->x[cp->num] = i;
						cp->y[cp->num] = j;
						cp->num++;
					}
				}
			}
		}
	}

	unsigned char **testImg = MemAlloc2D(h, w, 0);
	////////////결정된 점을 흰색으로 표시////////////////
	for (int i = 0; i < cp->num; i++) {
		//object_img[cp.y[i]][cp.x[i]] = 255; //원래 이밎 test하기 위해 임시로 testImg적용
		testImg[cp->y[i]][cp->x[i]] = 255;

	}

	////////////흰색 표시한 이미지 배열을 다른 이름으로 저장////////////////
	for (int h = 0; h < nHeight; h++) {
		for (int w = 0; w < nWidth; w++) {
			Out[h][w] = static_cast<unsigned char>(testImg[h][w]);
		}
	}

}// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.
Mat GrayToMat(unsigned char** Img, int nHeight, int nWidth)
{
	Mat rtn = Mat(nHeight, nWidth, CV_8UC1);

	for(int h = 0; h < nHeight; h ++)
	{
		for(int w = 0; w < nWidth; w++)
		{
			rtn.at<unsigned char>(h, w) = Img[h][w];
		}
	}
	return rtn;
}
double ** MemAllocDouble2D(int nHeight, int nWidth, unsigned char nInitVal)
{
	double** rtn = new double*[nHeight];
	for (int n = 0; n < nHeight; n++)
	{
		rtn[n] = new double[nWidth];
		memset(rtn[n], nInitVal, sizeof(double) * nWidth);
	}
	return rtn;
}
double** GaussianFilter(int nFilterSize, double dSigma) {
	double r = 0, s = 2 * dSigma*dSigma;
	double sum = 0.0;

	int pad_size = (nFilterSize / 2);
	double **GaussianF = new double *[nFilterSize];   //필터를 위한 배열 생성
	for (int i = 0; i < nFilterSize; i++) {
		GaussianF[i] = new double[nFilterSize];
		memset(GaussianF[i], 0, sizeof(double)*nFilterSize);
	}

	for (int x = -pad_size; x <= pad_size; x++) {   //필터 커널 생성
		for (int y = -pad_size; y <= pad_size; y++) {
			double expNum = -1 * (pow(x, 2) + pow(y, 2)) / s;   //지수함수 x값
			GaussianF[x + pad_size][y + pad_size] = (1 / PI*s)*(exp(expNum));
			sum += GaussianF[x + pad_size][y + pad_size];
		}
	}

	for (int i = 0; i < nFilterSize; i++) {
		for (int j = 0; j < nFilterSize; j++) {
			GaussianF[i][j] /= sum;
		}
	}

	return GaussianF;
}
unsigned char** Padding(unsigned char** In, int nHeight, int nWidth, int nFilterSize) {
	int nPadSize = (int)(nFilterSize / 2);
	unsigned char** Pad = MemAlloc2D(nHeight + 2 * nPadSize, nWidth + 2 * nPadSize, 0);

	for (int h = 0; h < nHeight; h++) {
		for (int w = 0; w < nWidth; w++) {
			Pad[h + nPadSize][w + nPadSize] = In[h][w];
		}
	}

	for (int h = 0; h < nPadSize; h++) {
		for (int w = 0; w < nWidth; w++) {
			Pad[h][w + nPadSize] = In[0][w];
			Pad[h + (nHeight - 1)][w + nPadSize] = In[nHeight - 1][w];
		}
	}

	for (int h = 0; h < nHeight; h++) {
		for (int w = 0; w < nPadSize; w++) {
			Pad[h + nPadSize][w] = In[h][0];
			Pad[h + nPadSize][w + (nWidth - 1)] = In[h][nWidth - 1];
		}
	}

	for (int h = 0; h < nPadSize; h++) {
		for (int w = 0; w < nPadSize; w++) {
			Pad[h][w] = In[0][0];
			Pad[h + (nHeight - 1)][w] = In[nHeight - 1][0];
			Pad[h][w + (nWidth - 1)] = In[0][nWidth - 1];
			Pad[h + (nHeight - 1)][w + (nWidth - 1)] = In[nHeight - 1][nWidth - 1];
		}
	}
	return Pad;
}

unsigned char** HammingWeight(unsigned char** In,int nFilterSize,int targetHeight,int targetWidth) {

	unsigned char** window = MemAlloc2D(nFilterSize, nFilterSize, 0);	//5*5 window생성
	unsigned char centerPixel = In[targetHeight][targetWidth];	//기준이 되는 중앙값

	int windowSize = nFilterSize / 2;

	for (int h = -windowSize; h < windowSize; h++) {
		for (int w = -windowSize; w < windowSize; w++) {
			window[h + windowSize][w + windowSize] = In[h + targetHeight][h + targetWidth];
			//중앙 픽셀값을 기준으로 filter size만큼 할당
		}
	}

	for (int h = 0; h < nFilterSize; h++) {
		for (int w = 0; w < nFilterSize; w++) {
			if (window[h][w] > centerPixel) {
				window[h][w] = 1;
			}
			else {
				window[h][w] = 0;
			}
			//printf("%d ", window[h][w]);
		}
	}
	return window;
}


void CProjectDlg::OnBnClickedBlend()
{
	if (isImageOpened == true && nFormat == FORMAT_GRAY)
	{
		if (Ori_Img.data != NULL && Proc_Img.data != NULL)	//두 이미지 둘다 비교하기 위해 null이 아니여야 함
		{
			unsigned char **resultImg = MemAlloc2D(HEIGHT, WIDTH, 0); //결과 이미지 저장할 변수
			unsigned char **resultImg2 = MemAlloc2D(HEIGHT, WIDTH, 0);
			unsigned char **resultImg3 = MemAlloc2D(HEIGHT, WIDTH, 0);
	
			int w = 640, h = 480;
			int w1, w2, h1, h2;
			int num = 0;
			bool check2 = false;
			// 결과 영상 픽셀 추출을 위한 반복문
			for (int i = 0; i < w; i++)
			{
				for (int j = 0; j < h; j++)
				{
					int m = 300; // 필터크기
					int left_num;
					int right_num;
					bool check = true;

					int match_x = 0, match_y = 0;
	


					while (1) {
						// 범위 즉정을 위한 변수
						w1 = i - m;
						w2 = i + m;
						h1 = j - m;
						h2 = j + m;

						// 매칭 값에 해당되면 출력 영상 전환
						if (check) {

							for (int match = 0; match < matchingPoints.num; match++)
							{
								if (check2 == false && matchingPoints.x[match] >= w1 && matchingPoints.x[match] <= w2 && matchingPoints.y[match] >= h1 && matchingPoints.y[match] <= h2)
								{
									check2 = true;
									break;
								}
								else if (check2 == true && matchingPoints.x[match] >= w1 && matchingPoints.x[match] <= w2 && matchingPoints.y[match] >= h1 && matchingPoints.y[match] <= h2)
								{
									check2 = false;
									break;
								}
							}

								if (check2)
								{
									num = 1;
								
								}
								else {
									num = 0;								
								}
							
						}			
						//증가하는 숫자가 짝수면 배경 그림을, 홀수면 오브젝트 그림을 대입
						if (num == 0)
						{
							resultImg[j][i] = ch_in_gray2[j][i];
							break;
						}
						else if (num == 1)
						{
							resultImg[j][i] = ch_in_gray[j][i];
							break;
						}
					}
				}
			}

			for (int j = 0; j < h; j++)
			{
				for (int i = 0; i < w; i++)
				{
					int m = 300; // 필터크기
					int left_num;
					int right_num;
					bool check = true;

					int match_x = 0, match_y = 0;



					while (1) {
						// 범위 즉정을 위한 변수
						w1 = i - m;
						w2 = i + m;
						h1 = j - m;
						h2 = j + m;

						// 매칭 값에 해당되면 출력 영상 전환
						if (check) {

							for (int match = 0; match < matchingPoints.num; match++)
							{
								if (check2 == false && matchingPoints.x[match] >= w1 && matchingPoints.x[match] <= w2 && matchingPoints.y[match] >= h1 && matchingPoints.y[match] <= h2)
								{
									check2 = true;
									break;
								}
								else if (check2 == true && matchingPoints.x[match] >= w1 && matchingPoints.x[match] <= w2 && matchingPoints.y[match] >= h1 && matchingPoints.y[match] <= h2)
								{
									check2 = false;
									break;
								}
							}

							if (check2)
							{
								num = 1;

							}
							else {
								num = 0;
							}

						}
						//증가하는 숫자가 짝수면 배경 그림을, 홀수면 오브젝트 그림을 대입
						if (num == 0)
						{
							resultImg2[j][i] = ch_in_gray2[j][i];
							break;
						}
						else if (num == 1)
						{
							resultImg2[j][i] = ch_in_gray[j][i];
							break;
						}
					}
				}
			}
			
			for (int i = 0; i < w; i++)
			{
				for (int j = 0; j < h; j++)
				{
					if (resultImg[j][i] == resultImg2[j][i])
						resultImg3[j][i] = resultImg[j][i];
					else {
						if (resultImg[j][i]>resultImg2[j][i])
							resultImg3[j][i] = resultImg[j][i];
						else
							resultImg3[j][i] = resultImg2[j][i];
					}
				}
			}

			int *Hist = new int[256];
			memset(Hist, 0, sizeof(int) * 256);

			//히스토그램 얻기
			for (int h = 0; h < HEIGHT; h++)
			{
				for (int w = 0; w < WIDTH; w++)
				{
					Hist[resultImg3[h][w]]++;
				}
			}

			//히스토그램 정규화
			int nMax = 0;
			for (int i = 0; i < 256; i++)
			{
				if (nMax < Hist[i])
				{
					nMax = Hist[i];
				}
			}
			double dNormalizeFactor = 255.0 / nMax;

			//CDF 구하기
			float Hist_CDF[256] = { 0,0 };
			Hist_CDF[0] = (double)Hist[0] / (HEIGHT * WIDTH);
			for (int i = 1; i < 256; i++)
			{
				Hist_CDF[i] = (double)Hist[i] / (HEIGHT * WIDTH) + Hist_CDF[i - 1];
			}

			//Scaling (픽셀 값이 i인 픽셀을 Hist_CDF[i]*255로 바꿔준다.
			for (int h = 0; h < HEIGHT; h++)
			{
				for (int w = 0; w < WIDTH; w++)
				{
					resultImg3[h][w] = Hist_CDF[resultImg3[h][w]] * 255;
				}
			}

			FileWrite("Original.raw", resultImg3, HEIGHT, WIDTH);

			DisplayImage(GrayToMat(resultImg3, nHeight_in, nWidth_in), true);
		}
	}
}

template<typename T> T** MemAlloc2D(int nHeight, int nWidth, int nInitVal)
{
	T** rtn = new T*[nHeight];
	for (int h = 0; h < nHeight; h++)
	{
		rtn[h] = new T[nWidth];
		memset(rtn[h], nInitVal, sizeof(T) * nWidth);
	}
	return rtn;
}
template<typename T> void MemFree2D(T** arr2D, int nHeight)
{
	for (int h = 0; h < nHeight; h++)
	{
		delete[] arr2D[h];
	}
	delete[] arr2D;
}
void FileRead(const char* strFilename, BYTE** arr2D, int nHeight, int nWidth)
{
	FILE* fp_in = fopen(strFilename, "rb");
	for (int h = 0; h < nHeight; h++)
	{
		fread(arr2D[h], sizeof(BYTE), nWidth, fp_in);
	}

	fclose(fp_in);
}
void FileWrite(const char* strFilename, BYTE** arr2D, int nHeight, int nWidth)
{
	FILE* fp_out = fopen(strFilename, "wb");
	for (int h = 0; h < nHeight; h++)
	{
		fwrite(arr2D[h], sizeof(BYTE), nWidth, fp_out);
	}

	fclose(fp_out);
}
BYTE clip_d(double a)
{
	if (a > 255)
	{
		return (BYTE)255;
	}
	if (a < 0)
	{
		return 0;
	}
	else return (BYTE)floor(a + 0.5);
}

void DFT_2D(double **dReal_2D, double **dImag_2D, int nHeight, int nWidth, int DFT_ID)
{
	// 행 단위 변환

	// 실수부와 허수부 1차열 배열 메모리 할당
	double *dTemp_R = new double[nWidth];
	double *dTemp_I = new double[nWidth];

	//배열 초기화
	memset(dTemp_R, 0, sizeof(double) * nWidth);
	memset(dTemp_I, 0, sizeof(double) * nWidth);

	// 행 단위 DFT 1차 변환
	for (int j = 0; j < nHeight; j++)
	{
		for (int i = 0; i < nWidth; i++)
		{
			dTemp_R[i] = dReal_2D[j][i];
			dTemp_I[i] = dImag_2D[j][i];
		}

		DFT_1D(dTemp_R, dTemp_I, nWidth, DFT_ID);

		for (int i = 0; i < nWidth; i++)
		{
			dReal_2D[j][i] = dTemp_R[i];
			dImag_2D[j][i] = dTemp_I[i];
		}
	}

	// 할당된 메모리 해제
	delete[] dTemp_R;
	delete[] dTemp_I;

	// 열 단위 변환

	// 실수부와 허수부 1차원 배열 메모리 할당
	dTemp_R = new double[nHeight];
	dTemp_I = new double[nHeight];

	// 할당된 메모리 초기화
	memset(dTemp_R, 0, sizeof(double) * nHeight);
	memset(dTemp_I, 0, sizeof(double) * nHeight);


	// 열 단위 DFT 1차 변환
	for (int i = 0; i < nWidth; i++)
	{
		for (int j = 0; j < nHeight; j++)
		{
			dTemp_R[j] = dReal_2D[j][i];
			dTemp_I[j] = dImag_2D[j][i];
		}

		DFT_1D(dTemp_R, dTemp_I, nHeight, DFT_ID);

		for (int j = 0; j < nHeight; j++)
		{
			dReal_2D[j][i] = dTemp_R[j];
			dImag_2D[j][i] = dTemp_I[j];
		}
	}

	// 할당된 메모리 해제 
	delete[] dTemp_R;
	delete[] dTemp_I;
}
void DFT_1D(double* dReal_1D, double* dImag_1D, int nLength, int DFT_ID)
{
	double dArg, dCosArg, dSinArg;

	double *dTemp_R = new double[nLength];
	double *dTemp_I = new double[nLength];

	memset(dTemp_R, 0, sizeof(double) * nLength);
	memset(dTemp_I, 0, sizeof(double) * nLength);

	for (int m = 0; m < nLength; m++)
	{
		dTemp_R[m] = 0;
		dTemp_I[m] = 0;
		dArg = DFT_ID * 2.0 * PI * (double)m / (double)nLength;
		for (int n = 0; n < nLength; n++)
		{
			dCosArg = cos(n * dArg);
			dSinArg = sin(n * dArg);

			dTemp_R[m] += (dReal_1D[n] * dCosArg - dImag_1D[n] * dSinArg);
			dTemp_I[m] += (dReal_1D[n] * dSinArg + dImag_1D[n] * dCosArg);
		}
	}

	if (DFT_ID == IDFT)
	{
		for (int m = 0; m < nLength; m++)
		{
			dReal_1D[m] = dTemp_R[m] / (double)nLength;
			dImag_1D[m] = dTemp_I[m] / (double)nLength;
		}
	}
	else
	{
		for (int m = 0; m < nLength; m++)
		{
			dReal_1D[m] = dTemp_R[m];
			dImag_1D[m] = dTemp_I[m];
		}
	}

	delete[] dTemp_R;
	delete[] dTemp_I;

}
void ConvImage(BYTE** arr2D, double** dReal_2D, double** dImag_2D, int nHeight, int nWidth, int DFT_ID)
{
	double dMax = 0, dNor, dMean = 0;
	double** dTemp_2D = MemAlloc2D<double>(nHeight, nWidth, 0);

	for (int h = 0; h < nHeight; h++)
	{
		memcpy(dTemp_2D[h], dReal_2D[h], sizeof(double) * nWidth);
	}

	if (DFT_ID == DFT)
	{
		for (int h = 0; h < nHeight; h++)
		{
			for (int w = 0; w < nWidth; w++)
			{
				double dTemp = sqrt(dReal_2D[h][w] * dReal_2D[h][w] + dImag_2D[h][w] * dImag_2D[h][w]);
				dTemp_2D[h][w] = log10(dTemp + 1);

				if (dMax < dTemp_2D[h][w])
				{
					dMax = dTemp_2D[h][w];
				}
			}
		}

		dNor = 255 / dMax;
	}
	else
	{
		dNor = 1;
	}

	for (int h = 0; h < nHeight; h++)
	{
		for (int w = 0; w < nWidth; w++)
		{
			double dTemp = (dTemp_2D[h][w] * dNor);
			arr2D[h][w] = clip_d(dTemp);
		}
	}
	if (DFT_ID == DFT)
	{
		SetCenterDC<BYTE>(arr2D, nHeight, nWidth);
	}

	MemFree2D<double>(dTemp_2D, nHeight);
}
template <typename T> void SetCenterDC(T** arr2D, int nHeight, int nWidth)
{
	int nHalf_H = nHeight / 2, nHalf_W = nWidth / 2;

	T** arr2DTemp = MemAlloc2D<T>(nHeight, nWidth, 0);

	for (int h = 0; h < nHeight; h++)
	{
		for (int w = 0; w < nWidth; w++)
		{
			arr2DTemp[h][w] = arr2D[h][w];
		}
	}

	for (int h = 0; h < nHalf_H; h++)
	{
		for (int w = 0; w < nHalf_W; w++)
		{
			arr2D[h][w] = arr2DTemp[h + nHalf_H][w + nHalf_W];
			arr2D[h + nHalf_H][w] = arr2DTemp[h][w + nHalf_W];
			arr2D[h][w + nHalf_W] = arr2DTemp[h + nHalf_H][w];
			arr2D[h + nHalf_H][w + nHalf_W] = arr2DTemp[h][w];
		}
	}

	MemFree2D<T>(arr2DTemp, nHeight);
}

void LowPassFilter(double** dReal2D, double** dImag2D, int nHeight, int nWidth, int nThres, int nFilterType)
{
	int nHalf_H = nHeight / 2, nHalf_W = nWidth / 2;

	double** dFilter = MemAlloc2D<double>(nHeight, nWidth, 0);

	SetCenterDC<double>(dReal2D, nHeight, nWidth);
	SetCenterDC<double>(dImag2D, nHeight, nWidth);

	// Make Filter
	switch (nFilterType)
	{
	case LPF_IDEAL:
		LowPassIdeal(nHeight, nWidth, nThres, dFilter);
		break;
	case LPF_BUTTERWORTH:
		LowPassButterworth(nHeight, nWidth, nThres, dFilter);
		break;
	case LPF_GAUSSIAN:
		LowPassGaussian(nHeight, nWidth, nThres, dFilter);
		break;
	}

	for (int h = 0; h < nHeight; h++)
	{
		for (int w = 0; w < nWidth; w++)
		{
			dReal2D[h][w] = dReal2D[h][w] * dFilter[h][w];
			dImag2D[h][w] = dImag2D[h][w] * dFilter[h][w];
		}
	}
	SetCenterDC<double>(dReal2D, nHeight, nWidth);
	SetCenterDC<double>(dImag2D, nHeight, nWidth);

	MemFree2D<double>(dFilter, nHeight);
}

void LowPassIdeal(int nHeight, int nWidth, int nThres, double** dFilter)
{
	int w2 = nWidth / 2;
	int h2 = nHeight / 2;
	int temp_h, temp_w;

	for (int j = 0; j < nHeight; j++)
	{
		for (int i = 0; i < nWidth; i++)
		{
			temp_w = i + w2;
			temp_h = j + h2;

			if (temp_w >= w2) temp_w -= w2;
			if (temp_h >= h2) temp_h -= h2;


			double check = sqrt((double)((temp_h - h2)*(temp_h - h2)) + ((temp_w - w2)*(temp_w - w2)));

			if (check > nThres)
			{
				dFilter[j][i] = 0;
			}
			else
				dFilter[j][i] = 1;

		}
	}
}

void LowPassGaussian(int nHeight, int nWidth, int nThres, double** dFilter)
{
	int w2 = nWidth / 2;
	int h2 = nHeight / 2;
	int temp_h, temp_w;

	double dist2, hval;

	for (int j = 0; j < nHeight; j++)
	{
		for (int i = 0; i < nWidth; i++)
		{
			temp_w = i + w2;
			temp_h = j + h2;

			if (temp_w >= w2) temp_w -= w2;
			if (temp_h >= h2) temp_h -= h2;


			dist2 = (double)((temp_h - h2)*(temp_h - h2)) + ((temp_w - w2)*(temp_w - w2));

			hval = exp(-dist2 / (2 * nThres*nThres));

			dFilter[j][i] = hval;

		}
	}
}

void LowPassButterworth(int nHeight, int nWidth, int nThres, double** dFilter)
{
	int w2 = nWidth / 2;
	int h2 = nHeight / 2;
	int temp_h, temp_w;

	double hval;
	int n = 1; //order

	for (int j = 0; j < nHeight; j++)
	{
		for (int i = 0; i < nWidth; i++)
		{
			temp_w = i + w2;
			temp_h = j + h2;

			if (temp_w >= w2) temp_w -= w2;
			if (temp_h >= h2) temp_h -= h2;


			double check = sqrt((double)((temp_h - h2)*(temp_h - h2)) + ((temp_w - w2)*(temp_w - w2)));

			hval = 1.0 + pow((check / nThres), 2 * n);

			hval = 1 / hval;

			dFilter[j][i] = hval;

		}
	}
}

void CProjectDlg::OnBnClickedFiltering()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// File Open & Memory Allocation

	BYTE*** arr2D = new BYTE**[3];  	// 0 : arr2D_in, 1 = DFT_Re, 2 = IDFT_Re

	for (int n = 0; n < 3; n++)
	{
		arr2D[n] = MemAlloc2D<BYTE>(HEIGHT, WIDTH, 0);
	}

	double** dReal_2D = MemAlloc2D<double>(HEIGHT, WIDTH, 0);
	double** dImag_2D = MemAlloc2D<double>(HEIGHT, WIDTH, 0);

	FileRead("Original.raw", arr2D[In], HEIGHT, WIDTH);

	// DFT

	for (int i = 0; i < HEIGHT; i++)
	{
		for (int j = 0; j < WIDTH; j++)
		{
			dReal_2D[i][j] = (double)arr2D[In][i][j];
			dImag_2D[i][j] = 0;
		}
	}

	DFT_2D(dReal_2D, dImag_2D, HEIGHT, WIDTH, DFT);
	ConvImage(arr2D[DFT_Real], dReal_2D, dImag_2D, HEIGHT, WIDTH, DFT);

	// Filter Processing

	LowPassFilter(dReal_2D, dImag_2D, HEIGHT, WIDTH, 120, LPF_GAUSSIAN);

	// IDFT

	DFT_2D(dReal_2D, dImag_2D, HEIGHT, WIDTH, IDFT);
	ConvImage(arr2D[IDFT_Real], dReal_2D, dImag_2D, HEIGHT, WIDTH, IDFT);
	FileWrite("Result.raw", arr2D[IDFT_Real], HEIGHT, WIDTH);

	DisplayImage(GrayToMat(arr2D[IDFT_Real], nHeight_in, nWidth_in), false);
}