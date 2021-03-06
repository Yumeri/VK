// VK.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"


#define PREPARE 0

using namespace cv;

HWND hYYSWnd = NULL;
int waste_TiLi = 0;
time_t init_time;

#define IOS_AREA_X 15872
#define IOS_AREA_Y 23210
#define ANDROID_AREA_X 21964
#define ANDROID_AREA_Y IOS_AREA_Y
#define SCREEN_W 1920
#define SCREEN_H 1080

#define MIN_EPS 0.05
#define MAX_STEPS 30
//APIs

void print_info(const char *str) {
	SYSTEMTIME sys;
	GetLocalTime(&sys);
	printf("%02d:%02d:%02d %s\n", sys.wHour, sys.wMinute, sys.wSecond, str);
}

void delay(int ms = 50) {
	Sleep(ms + rand() % 200);
}

void moveTo(int x, int y) {
	mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, x, y, 0, GetMessageExtraInfo());
}

void randMoveTo(int left, int top, int right, int bottom) {
	int x = left + rand() % (right - left);
	int y = top + rand() % (bottom - top);
	moveTo(x * 0xffff / SCREEN_W, y * 0xffff / SCREEN_H);
}

void leftClick() {
	mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, 0, GetMessageExtraInfo());
}

void pressKey(char c) {
	if (c >= 'a' && c <= 'z')
		keybd_event(c - 32, 0, 0, 0);
	if (c >= '0' && c <= '9')
		keybd_event(c, 0, 0, 0);
	if (c == '@') {
		keybd_event(16, 0, 0, 0);
		delay();
		keybd_event(50, 0, 0, 0);
		delay();
		keybd_event(16, 0, KEYEVENTF_KEYUP, 0);
		delay();
	}
	if (c == '.')
		keybd_event(110, 0, 0, 0);
}

void sayString(const char *str) {
	int len = (int)strlen(str);
	for (int i = 0; i < len; i++) {
		pressKey(str[i]);
		delay();
	}
}

Mat gdiScreenCapture(RECT rect) {
	rect.right += 20;
	rect.bottom += 20;

	HDC hdc = GetDC(GetDesktopWindow());
	HDC comHdc = CreateCompatibleDC(hdc);
	int nWidth = rect.right - rect.left;
	int nHeight = rect.bottom - rect.top;
	HBITMAP hBitmap = CreateCompatibleBitmap(hdc, nWidth, nHeight);
	SelectObject(comHdc, hBitmap);
	BitBlt(comHdc, rect.left, rect.top, rect.right, rect.bottom, hdc, 0, 0, SRCCOPY);

	BITMAP bmp;
	GetObject(hBitmap, sizeof(BITMAP), &bmp);
	int nCh = bmp.bmBitsPixel == 1 ? 1 : bmp.bmBitsPixel / 8;
	int depth = bmp.bmBitsPixel == 1 ? IPL_DEPTH_1U : IPL_DEPTH_8U;

	Mat mat;
	mat.create(CvSize(nWidth, nHeight), CV_MAKETYPE(CV_8U, nCh));
	GetBitmapBits(hBitmap, nWidth*nHeight*nCh, mat.data);
	return mat;
}

Point findPic(const char *tpl_path, HWND hwnd, double &eps) {
	Mat img, origin_img, templ, result;
	templ = imread(tpl_path);
	//cvtColor(origin_templ, templ, CV_GRAY2RGB);
	//It makes an error when img type = 24, templ type = 16, result type = 5,
	//but it's right when img type = 16, templ type = 16, result type = 5.
	//We should change the type of img. Get more info from the website
	//https://stackoverflow.com/questions/18988910/opencv-imread-gives-cv-8uc3-cant-convert-to-cv-8uc4
	RECT rect;
	GetClientRect(hwnd, &rect);
	origin_img = gdiScreenCapture(rect);
	cvtColor(origin_img, img, CV_BGRA2BGR);

	int result_cols = img.cols - templ.cols + 1;
	int result_rows = img.rows - templ.rows + 1;
	result.create(result_cols, result_rows, CV_32FC1);
	//printf("img type = %d, templ type = %d, result type = %d\n", img.type(), templ.type(), result.type());
	//这里我们使用的匹配算法是标准平方差匹配 method=CV_TM_SQDIFF_NORMED，数值越小匹配度越好
	matchTemplate(img, templ, result, CV_TM_SQDIFF_NORMED);
	//不进行正则化，因为我们是直接从原图上截图出的模板，绝对误差本身就很小。
	//normalize(result, result, 0, 1, NORM_MINMAX, -1, Mat());

	double minVal = -1;
	double maxVal;
	Point minLoc;
	Point maxLoc;
	minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());
	eps = fabs(minVal);

	//定位到template匹配位置附近的一个随机点，抗行为检测
	int x0 = templ.cols;
	int y0 = templ.rows;
	minLoc.x += rand() % x0;
	minLoc.y += rand() % y0;

	//printf("minVal = %.9lf, maxVal = %.9lf\n", minVal, maxVal);
	return minLoc;
}

bool attack_finished_check() {
	double eps;
	findPic("C:\\img\\attack_finished.png", hYYSWnd, eps);
	if (eps > MIN_EPS)return false;
	else return true;
}


void attack(int x, int y) {
	print_info("Attacking...");
	waste_TiLi += 3;
	time_t cur_time = time(NULL);
	printf("TiLi / time = %lf\n", (float)waste_TiLi / (cur_time - init_time));
	moveTo(x * 0xffff / SCREEN_W, y * 0xffff / SCREEN_H);
	delay();
	leftClick();
	//假设10秒结束战斗
	delay(10000);
	int st = (int)time(NULL);
	while (!attack_finished_check()) {
		if (time(NULL) - st > 50)break;//超过1分钟，说明不在战斗中
		randMoveTo(336, 415, 586, 508);
		delay(1000);
		leftClick();
	}
	randMoveTo(336, 415, 586, 508);
	leftClick();
}

void enter_fb() {
	print_info("Entering couterpart...");
	randMoveTo(678, 450, 756, 477);
	delay();
	leftClick();
}

void get_award() {
	print_info("Get award...");
	double eps = 1.0f;
	while (true)
	{
		Point pos = findPic("C:\\img\\fb_get.png", hYYSWnd, eps);
		if (eps < MIN_EPS) {
			moveTo(pos.x * 0xffff / SCREEN_W, pos.y * 0xffff / SCREEN_H);
			delay();
			leftClick();
			delay();
			moveTo(721* 0xffff / SCREEN_W, 465 * 0xffff / SCREEN_H);
			delay();
			leftClick();
			delay();
		}
		else {
			return;
		}
	}
}

//副本
void fb() {
	double eps = 1.0f;
	int move_count = 0;
	int move_dir = 0;//向右移动
	bool fb_finished = false;
	while (true)
	{	//进入副本
		enter_fb();
		delay(3000);
		fb_finished = false;
		while (true)//副本内循环
		{	
			if (fb_finished) {
				print_info("Finished counterpart.");
				break;
			}
			fb_finished = false;
			Point pos;
			//寻找是否有boss
			pos = findPic("C:\\img\\fb_boss.png", hYYSWnd, eps);
			if (eps < MIN_EPS) {
				//有boss
				print_info("Find boss.");
				attack(pos.x, pos.y);
				while (true)
				{
					pos = findPic("C:\\img\\enter.png", hYYSWnd, eps);
					if (eps > MIN_EPS) {
						get_award();
						delay(1000);
						randMoveTo(357, 511, 605, 562);
						delay();
						leftClick();
						delay();
					}
					else {
						fb_finished = true;
						print_info("Leaving counterpart.");
						delay();
						break;
					}
					delay(3000);
				}
			}
			else//没有boss
			{
				pos = findPic("C:\\img\\attack.PNG", hYYSWnd, eps);
				if (eps < MIN_EPS) {
					attack(pos.x, pos.y);
					delay(1000);
				}
				else {
					if (move_dir == 0)
					{
						randMoveTo(623, 431, 695, 529);
						move_count++;
						if (move_count > MAX_STEPS) {
							move_dir = 1;
							move_count = 0;
						}
					}
					else {
						randMoveTo(279, 470, 348, 500);
						move_count++;
						if (move_count > MAX_STEPS) {
							move_dir = 0;
							move_count = 0;
						}
					}
					delay();
					leftClick();
					delay(500);
				}
				delay(1000);
			}
		}
		randMoveTo(339, 522, 588, 567);
		delay();
		leftClick();
		delay(2000);
	}
}


//自动做任务
void mission() {
	Point pos;
	while (true)
	{
		double eps = 1.0f;
		pos = findPic("C:\\img\\mission\\talk.png", hYYSWnd, eps);
		if (eps > MIN_EPS) {
			pos = findPic("C:\\img\\mission\\skip.png", hYYSWnd, eps);
			if (eps > MIN_EPS) {
				pos = findPic("C:\\img\\mission\\quick.png", hYYSWnd, eps);
				if (eps > MIN_EPS) {
					pos = findPic("C:\\img\\mission\\quest.png", hYYSWnd, eps);
					if (eps > MIN_EPS) {
						pos = findPic("C:\\img\\mission\\view.png", hYYSWnd, eps);
						if (eps > MIN_EPS) {
							pos = findPic("C:\\img\\attack.png", hYYSWnd, eps);
							if (eps < MIN_EPS) {
								moveTo(pos.x * 0xffff / SCREEN_W, pos.y * 0xffff / SCREEN_H);
								delay();
								leftClick();
								print_info("attack.");
								delay(20000);
								while (!attack_finished_check()) {
									randMoveTo(336, 415, 586, 508);
									delay(1000);
									leftClick();
								}
								randMoveTo(336, 415, 586, 508);
								delay(3000);
								leftClick();

							}
							continue;
						}
						print_info("view.");
					}
					else {
						print_info("question.");
					}
				}
				else {
					print_info("quick.");
				}
			}
			else {
				print_info("skip.");
			}
		}
		else {
			print_info("talk.");
		}
		print_info("next...");
		moveTo(pos.x * 0xffff / SCREEN_W, pos.y * 0xffff / SCREEN_H);
		delay();
		leftClick();
		delay(500);
	}
}


int main()
{
	srand((unsigned int)time(NULL));

	init_time = time(NULL);
	while (!hYYSWnd)
	{
		hYYSWnd = FindWindow(NULL, L"阴阳师-网易游戏");
	}

	BOOL bRet = FALSE;
	HDC hdc = GetDC(hYYSWnd);

	while (!bRet)
	{
		bRet = MoveWindow(hYYSWnd, 0, 0, 800, 600, TRUE);
	}
	
	SetForegroundWindow(hYYSWnd);

	fb();
	//mission();

	ReleaseDC(hYYSWnd, hdc);
    return 0;
}

