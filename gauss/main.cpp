#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/video/background_segm.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdio.h>
#include <cvaux.h>
#include <iostream>
#include <cv.h>    
#include <opencv2\opencv.hpp>  
using namespace std;
using namespace cv;

int apos = 30;
CvRect box;
bool Drawing = false;
bool out = false;
int xx1, xx2, yy1, yy2;

Mat Frame;
//--------------------------------------【help( )函数】--------------------------------------
//		 描述：输出一些帮助信息
//----------------------------------------------------------------------------------------------
static void help()
{
	printf("\n\n\n\t此程序展示了用高斯背景建模进行视频的背景分离方法.\n\n\t主要采用cvUpdateBGStatModel()函数\n"
		"\n\t程序首先会“学习背景”，然后进行分割。\n"
		"\n\t可以用过【Space】空格进行功能切换。\n\n");
}
//---------------------------------------------------------------------------------------------
void DrawRect(Mat img, CvRect rect)
{
	xx1 = box.x;
	xx2 = box.x + box.width;
	yy1 = box.y;
	yy2 = box.y + box.height;
	rectangle(img, cvPoint(box.x, box.y), cvPoint(box.x + box.width, box.y + box.height), cvScalar(0x00, 0xff, 0x00), 2);

}
//---------------------------------------------------------------------------------------------
void my_mouse_callback(int event, int x, int y, int flags, void* param)
{
	
	switch (event)
	{
	case CV_EVENT_MOUSEMOVE://0
	{
		if (Drawing)
		{
			box.width = x - box.x;
			box.height = y - box.y;
		}
	}
	break;

	case CV_EVENT_LBUTTONDOWN://1
	{
		cout << "button down" << endl;
		Drawing = true;
		box = cvRect(x, y, 0, 0);
	}
	break;

	case CV_EVENT_LBUTTONUP://4
	{
		cout << "button up" << endl;
		Drawing = false;
		if (box.width<0)
		{
			box.x += box.width;
			box.width *= -1;
		}
		if (box.height<0)
		{
			box.y += box.height;
			box.height *= -1;
		}
		out = true;
		DrawRect(Frame, box);

	}
	break;

	}
}
//-------------------------------------------------------------------------------------------------
void on_trackbar(int pos)
{
	apos = pos;
}
//-----------------------------------【main( )函数】--------------------------------------------
//		描述：控制台应用程序的入口函数，我们的程序从这里开始
//-------------------------------------------------------------------------------------------------
int main(int argc, const char** argv)
{
	help();

	int i, j;
	Scalar s;
	Mat img, fgmask, fgimg;

	VideoCapture cap;
	bool update_bg_model = true;

	//cap.open(0);
	cap.open("D:\\calib\\1080p.mp4");
	if (!cap.isOpened())
	{
		printf("can not open camera or video file\n");
		return -1;
	}
	//bool stop = false;
	//Mat frame;
	//while (!stop) {
	//	cap >> frame;
	//	imshow("result", frame);
	//	if (waitKey(1) >= 0) {
	//		system("pause");
	//		stop = true;
	//	}
	//}

	namedWindow("video", WINDOW_AUTOSIZE);
	
	cap >> Frame;
	//IplImage*temp1 = cvCloneImage(pFrame);
	setMouseCallback("video", my_mouse_callback, 0);//鼠标事件发生在制定窗口里 
	
	//while (out == false)
	//{
	//	cap >> Frame;
	////	cvCopyImage(pFrame, temp1);
	////	if (Drawing)
	////		DrawRect(temp1, box);
	////	cvShowImage("video", temp1);
	////	if (cvWaitKey(100) == 27)
	////		break;
	//	imshow("video",Frame);
	//	cvWaitKey(10);
	//}
	//printf("%d,%d,%d,%d,", xx1, xx2, yy1, yy2);

	//BackgroundSubtractorMOG2 bg_model;//(100, 3, 0.3, 5);
	Ptr<BackgroundSubtractorMOG2> bg_model = createBackgroundSubtractorMOG2();  //Mat frame, fmask; frame是源图像，fmask存放提取结果的二值化图像 mog->apply(frame, fmask); 
	
	while (1){
		cap >> Frame;
		img = Frame.clone();
		
		rectangle(Frame, cvPoint(box.x, box.y), cvPoint(box.x + box.width, box.y + box.height), cvScalar(0x00, 0xff, 0x00), 2);
		

		//if (fgimg.empty())
		//	fgimg.create(img.size(), img.type());
		//更新模型
		//bg_model(img, fgmask, update_bg_model ? -1 : 0); 
		bg_model->apply(img, fgmask, update_bg_model ? -1 : 0);
		fgimg = Scalar::all(0);
		img.copyTo(fgimg, fgmask);
		int n = 0;
		//变为灰度图像
		cvtColor(fgimg, fgimg, CV_RGB2GRAY);
        //判断警戒区域
		for (j = xx1; j < xx2; j++)
			for (i = yy1; i < yy2; i++)
			{
				//返回特殊的数组元素，
				if (fgimg.at<uchar>(i, j) >= 175)//0xff为白色的  
				{
					n++;
				}
			}
		cout << n << endl;
		if (n >= 20)
		{
			putText(Frame, "warning", Point(0, 20), FONT_HERSHEY_PLAIN, 2, Scalar(0, 0, 255), 1);
		}
		imshow("video", Frame);
		//imshow("image", img);
		imshow("fgimg", fgimg);
		//imshow("foreground image", fgimg);
	
		char k = (char)waitKey(30);
		if (k == 27) break;
		if (k == ' ')
		{
			update_bg_model = !update_bg_model;
			if (update_bg_model)
				printf("\t>背景更新(Background update)已打开\n");
			else
				printf("\t>背景更新(Background update)已关闭\n");
		}	
		
	}

	destroyAllWindows();
	cap.release();
	return 0;
}
