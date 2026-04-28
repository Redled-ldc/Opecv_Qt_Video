#include "XVideoWidget.h"
#include <QPainter>
#include <opencv2/imgproc.hpp>
XVideoWidget::XVideoWidget(QWidget* p) :QOpenGLWidget(p)
{
	
}
XVideoWidget::~XVideoWidget()
{
}

void XVideoWidget::paintEvent(QPaintEvent* event)
{
	QPainter p;
	p.begin(this);
	//绘制视频帧
	p.drawImage(QRect(0, 0, width(), height()), img);
	p.end();
}

void XVideoWidget::SetImage(cv::Mat mat)
{
	/*if (img.isNull())
	{
		uchar* buf = new uchar[width() * height() * 3];
		img = QImage(buf, width(), height(), QImage::Format_RGB888);
	}
	cv::Mat des;
	cv::resize(mat, des, cv::Size(img.size().width(),img.size().height()));
	memcpy(img.bits(), des.data, (des.rows * des.cols * des.elemSize()));
	*/
	if (mat.empty()) return;

	// 1. 转换OpenCV BGR → RGB
	cv::Mat rgbMat;
	cv::cvtColor(mat, rgbMat, cv::COLOR_BGR2RGB);

	// 2. 调整尺寸为控件当前尺寸（避免尺寸为0）
	cv::Size targetSize(width(), height());
	if (targetSize.width <= 0 || targetSize.height <= 0) return;
	cv::resize(rgbMat, rgbMat, targetSize);

	// 3. 重新初始化QImage（避免手动分配内存泄漏）
	// 直接用Mat数据创建QImage，利用Qt内存管理
	img = QImage(rgbMat.data, rgbMat.cols, rgbMat.rows,
		rgbMat.step1(), QImage::Format_RGB888);
	// 深拷贝Mat数据（避免Mat释放后QImage数据失效）
	img = img.copy();

	update();
}