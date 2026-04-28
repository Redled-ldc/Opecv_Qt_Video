#pragma once
#include <opencv2/core.hpp>
class XImagePro
{
	public:
		// 设置原图像，会清理处理结果
		void Set(cv::Mat mat1,cv::Mat mat2);

        // 获取处理结果
		cv::Mat Get(){return des;}

		//设置亮度和对比度
		///@param bright 亮度 0-100
		///@param constrast 对比度1.0-3.0
		void Gain(double bright, double constrast);
		//图像旋转
		/// @param angle 旋转角度，正数逆时针旋转，负数顺时针旋转
		//void Rotate(double angle);
		void Rotate90();
		void Rotate180();
		void Rotate270();
		//镜像图像
		void FlipX();
		void FlipY();
		void FlipXY();
		//尺寸调整
		void Resize(int width, int height);
		//图像金字塔
		void PyrDown(int count);//高斯金字塔
		void PyrUp(int count);//拉普拉斯金字塔

		// 视频裁剪
		void Crop(int x, int y, int width, int height);

		XImagePro();
        ~XImagePro();
  private:
      //原图
	  cv::Mat src1,src2;
	  //处理结果
      cv::Mat des;
};

