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
    if (mat.empty()) return;

    cv::Mat desMat;
    QImage::Format fmt;

    // 根据通道数处理
    if (mat.channels() == 1) {                    // 灰度图
        desMat = mat.clone();
        fmt = QImage::Format_Grayscale8;
    }
    else if (mat.channels() == 3) {               // BGR
        cv::cvtColor(mat, desMat, cv::COLOR_BGR2RGB);
        fmt = QImage::Format_RGB888;
    }
    else if (mat.channels() == 4) {               // BGRA
        cv::cvtColor(mat, desMat, cv::COLOR_BGRA2RGBA);
        fmt = QImage::Format_RGBA8888;            // 保留 Alpha
        // 若不需要 Alpha 可转为 RGB:
        // cv::cvtColor(mat, desMat, cv::COLOR_BGRA2RGB);
        // fmt = QImage::Format_RGB888;
    }
    else {
        return; // 不支持的格式
    }

    // 调整尺寸为控件大小
    cv::Size targetSize(width(), height());
    if (targetSize.width <= 0 || targetSize.height <= 0) return;
    cv::resize(desMat, desMat, targetSize);

    // 创建 QImage 并深拷贝
    img = QImage(desMat.data, desMat.cols, desMat.rows,
        desMat.step1(), fmt);
    img = img.copy();  // 深拷贝，避免 Mat 释放后数据失效

    update();
}