#pragma once
#include <QtOpenGLWidgets/qopenglwidget.h>
#include <QOpenGLWidget>
#include <opencv2/core.hpp>
class XVideoWidget :public QOpenGLWidget
{
	Q_OBJECT
public:
	XVideoWidget(QWidget* p);
	
	void paintEvent(QPaintEvent* event);
	virtual~XVideoWidget();
public slots:
	//接收视频帧数据的槽函数，参数为视频帧数据
	void SetImage(cv::Mat mat);
protected:
	QImage img;
};

