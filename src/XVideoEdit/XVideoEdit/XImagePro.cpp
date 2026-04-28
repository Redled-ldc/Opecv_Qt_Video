#include "XImagePro.h"
#include <opencv2/imgproc.hpp>
void XImagePro::Set(cv::Mat mat1, cv::Mat mat2)
{
    if (mat1.empty()) return;
	this->src1 = mat1;
	this->src2 = mat2;
	this->src1.copyTo(des);
}


//设置亮度和对比度
///@param bright 亮度 0-100
///@param constrast 对比度1.0-3.0
void XImagePro::Gain(double bright, double contrast)
{
	if(des.empty()) return;
	des.convertTo(des, -1, contrast, bright);
}
//图像旋转
void XImagePro::Rotate90()
{
	if (des.empty()) return;
	cv::rotate(des, des, cv::ROTATE_90_CLOCKWISE);
}
void XImagePro::Rotate180()
{
	if (des.empty()) return;
	cv::rotate(des, des, cv::ROTATE_180);
}

void XImagePro::Rotate270()
{
	if (des.empty()) return;
	cv::rotate(des, des, cv::ROTATE_90_COUNTERCLOCKWISE);
}
//镜像图像,左右镜像：0；上下镜像：1；同时水平和垂直镜像：-1
void XImagePro::FlipX()
{
	if (des.empty()) return;
	cv::flip(des, des, 0);
}
void XImagePro::FlipY()
{
	if (des.empty()) return;
	cv::flip(des, des, 1);
}
void XImagePro::FlipXY()
{
	if (des.empty()) return;
	cv::flip(des, des, -1);
}
//图像尺寸调整
void XImagePro::Resize(int width, int height)
{
	if (des.empty()) return;
	cv::resize(des, des, cv::Size(width, height));
}
//图像金字塔调整
void XImagePro::PyrDown(int count)
{
	if (des.empty()) return;
	for (int i = 0; i < count; i++)
	{
		cv::pyrDown(des, des);
	}
}
void XImagePro::PyrUp(int count)
{
	if (des.empty()) return;
	for (int i = 0; i < count; i++)
	{
		cv::pyrUp(des, des);
	}
}
//视频裁剪
void XImagePro::Clip(int x, int y, int width, int height)
{
	if (des.empty()) return;
	if (x < 0 || y<0 || width <= 0 || height <= 0 || x + width>des.cols || y + height > des.rows) return;
	cv::Rect roi(x, y, width, height);
	des = des(roi);
}
//转为灰度图
void XImagePro::Gray()
{
	if (des.empty()) return;
	cv::cvtColor(des, des, cv::COLOR_BGR2GRAY);
}

XImagePro::XImagePro()
{
}
XImagePro::~XImagePro()
{
}