#include "XFilter.h"
#include <QMutex>
#include "XImagePro.h"
class CXFilter :public XFilter
{
public:
	
	std::vector<XTask> tasks;
	QMutex mutex;
	cv::Mat Filter(cv::Mat mat1, cv::Mat mat2)override
	{

		QMutexLocker locker(&mutex);
		
			XImagePro p;
			size_t sum = tasks.size();
			p.Set(mat1, mat2);
			for (size_t i = 0; i < sum; i++)
			{
				XTask task = tasks[i];
				switch (task.type)
				{
				case XTASK_GAIN:

					//亮度对比度调整
					p.Gain(task.param[0], task.param[1]);
					break;
				case XTASK_ROTATE90:
					//图像旋转90度
					p.Rotate90();
					break;
				case XTASK_ROTATE180:
					//图像旋转180度
					p.Rotate180();
					break;
				case XTASK_ROTATE270:
					//图像旋转270度
					p.Rotate270();
					break;
				case XTASK_FLIPX:
					//x轴镜像
					p.FlipX();
					break;
				case XTASK_FLIPY:
					//y镜像
					p.FlipY();
					break;
				case XTASK_FLIPXY:
					//xy镜像
					p.FlipXY();
					break;
				case XTASK_RESIZE:
					//图像尺寸调整
					p.Resize(static_cast<int>(task.param[0]), static_cast<int>(task.param[1]));
					break;
				case XTASK_PYRDOWN:
					//图像金字塔下采样
					p.PyrDown(task.param[0]);
					break;
				case XTASK_PYRUP:
					//图像金字塔上采样
					p.PyrUp(task.param[0]);
					break;
				default:
					break;
				}
			}
			cv::Mat re = p.Get();
			return re;
	}
	void Add(XTask task) override
	{
		QMutexLocker locker(&mutex);
		tasks.push_back(task);

	}
	void Clear()override
	{
		QMutexLocker locker(&mutex);
		tasks.clear();
	}
};
//单例模式，获取实例对象
 XFilter *XFilter::Get()
{
	 static CXFilter cx;
	 return &cx;
}
XFilter::XFilter()
{

}

XFilter::~XFilter()
{
}
