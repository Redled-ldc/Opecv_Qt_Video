#pragma once
#include <opencv2/core.hpp>

#include <vector>

enum XTaskType
{
	XTASK_MONE,
	XTASK_GAIN,//亮度,对比度调整
	XTASK_ROTATE90,//图像旋转
	XTASK_ROTATE180,
	XTASK_ROTATE270,
	//图像镜像
	XTASK_FLIPX,
	XTASK_FLIPY,
	XTASK_FLIPXY,
	//图像尺寸调整
	XTASK_RESIZE,
	//图像金字塔调整
	XTASK_PYRDOWN,
	XTASK_PYRUP,
	//视频裁剪
	XTASK_CLIP,
	//转为灰度图
	XTASK_GRAY,
	//水印
	XTASK_MARK,
	//视频融合
	XTASK_BLEND,
	//视频水平合并
	XTASK_MERGE,
};
struct XTask
{ 
	XTaskType type;
	std::vector <double> param;
};
class XFilter
{
public:
	virtual cv::Mat Filter(cv::Mat mat1,cv::Mat mat2)=0;
	virtual void Add(XTask task)=0;
	virtual void Clear() = 0;
	static XFilter* Get();
	
	virtual~XFilter();
protected:
	XFilter();
};

