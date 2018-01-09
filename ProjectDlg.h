
// ProjectDlg.h : ��� ����
//

#include <opencv2\opencv.hpp>
#include "RawInfoDlg.h"

using namespace cv;
#define MAX_CORNER 5000

#pragma once

// CProjectDlg ��ȭ ����
class CProjectDlg : public CDialogEx
{
// �����Դϴ�.
public:
	CProjectDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROJECT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.

	enum ImageFormat
	{
		FORMAT_RGB,
		FORMAT_YUV444,
		FORMAT_YUV420,
		FORMAT_GRAY
	};


// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
	unsigned char **ch_in_1, **ch_in_2, **ch_in_3;
	unsigned char **ch_in_gray,**ch_in_gray2;

	int nHeight_in, nWidth_in;
	int nFormat;
	bool isImageOpened,isLeftImg=true;	//���� ������ ���� �����ϱ� ����

public:
	CStatic m_PicOri;
	CStatic m_PicProc;
	CRawInfoDlg RawInfoDlg;

	Mat Ori_Img, Proc_Img;
	CRect rect;
	CDC* m_pDC;
	unsigned char ** GrayImg;

	void DisplayImage(Mat DispMat, bool blnOut);
	void DisplayBitmap(CDC* pDC, CRect rect, IplImage* DispIplImage);
	void ResizeImage(IplImage* src, IplImage** dst, CRect* rect);
	afx_msg void OnFileOpenBtnClick();
	afx_msg void OnFileFileclose();
	afx_msg void OnFileFileopenRight();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedHarris();
	afx_msg void OnBnClickedMatching();
	afx_msg void OnBnClickedBlend();
	afx_msg void OnBnClickedFiltering();
};

typedef struct _CornerPoints {
	int num;			//���� ����� ���� ����
	int x[MAX_CORNER];	//���� �ڳ� ���� x��ǥ
	int y[MAX_CORNER];	//���� �ڳ� ���� y��ǥ
} CornerPoints;

unsigned char** MemAlloc2D(int nHeight, int nWidth, unsigned char nInitVal);
double ** MemAllocDouble2D(int nHeight, int nWidth, unsigned char nInitVal);
void MemFree2D(unsigned char** Mem, int nHeight);
unsigned char** Padding(unsigned char** In, int nHeight, int nWidth, int nFilterSize);
double** GaussianFilter(int nFilterSize, double dSigma);
void HarrisDetection(unsigned char ** object_img, unsigned char** Out, int nHeight, int nWidth, double k,double THRESHOLD, CornerPoints* cp);
unsigned char** HammingWeight(unsigned char** In, int nFilterSize, int targetHeight, int targetWidth);
Mat GrayToMat(unsigned char** Img, int nHeight, int nWidth);