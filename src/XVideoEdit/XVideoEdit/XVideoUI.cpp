#include "XVideoUI.h"
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>
#include "XFilter.h"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <iostream>
#include "XAudio.h"
#include <QFile>
static bool pressSlider = false;
static bool isExport = false;
static bool isColor = true;
static bool isMark = false;
static bool isBlend = false;
static bool isMegre = false;

XVideoUI::XVideoUI(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
	//隐藏标题栏
	setWindowFlags(Qt::FramelessWindowHint);
	//连接信号与槽
	connect(ui.closeButton, &QPushButton::clicked, this, &XVideoUI::on_pushButton_clicked);
	connect(ui.openButton, &QPushButton::clicked, this, &XVideoUI::openFile);
    connect(ui.playSlider, &QSlider::sliderMoved, this, &XVideoUI::SetPos);
    connect(ui.playSlider, &QSlider::sliderPressed, this, &XVideoUI::SliderPress);
    connect(ui.playSlider, &QSlider::sliderReleased, this, &XVideoUI::SliderRelease);
	connect(ui.setButton, &QPushButton::clicked, this, &XVideoUI::SetParam);
	connect(ui.exportButton, &QPushButton::clicked, this, &XVideoUI::ExportVideo);
	connect(XVideoThread::Get(), &XVideoThread::SaveEend, this, &XVideoUI::ExportEnd);
	connect(ui.playButton, &QPushButton::clicked, this, &XVideoUI::PlayVideo);
	connect(ui.pauseButton, &QPushButton::clicked, this, &XVideoUI::PauseVideo);
	connect(ui.blendButton, &QPushButton::clicked, this, &XVideoUI::Blend);
	connect(ui.markButton, &QPushButton::clicked, this, &XVideoUI::SetMark);
	connect(ui.mergeButton, &QPushButton::clicked, this, &XVideoUI::Merge);
	connect(ui.endplay, &QSlider::sliderMoved, this, &XVideoUI::CutRight);
	connect(ui.startplay, &QSlider::sliderMoved, this, &XVideoUI::CutLeft);
	qRegisterMetaType<cv::Mat>("cv::Mat");

	//原1视频显示信号和槽连接
	connect(XVideoThread::Get(), &XVideoThread::ViewImage1,
		ui.src1video, &XVideoWidget::SetImage);
	//原2视频显示信号和槽连接
	connect(XVideoThread::Get(), &XVideoThread::ViewImage2,
		ui.src2video, &XVideoWidget::SetImage);
	//输出视频显示信号和槽连接
	connect(XVideoThread::Get(), &XVideoThread::ViewDes,
		ui.desvideo, &XVideoWidget::SetImage);

	PauseVideo();
	//启动定时器	
	startTimer(40);

}

XVideoUI::~XVideoUI()
{
}

void XVideoUI::on_pushButton_clicked()
{
	//关闭窗口
	this->close();
}

void XVideoUI::timerEvent(QTimerEvent* e) {
	if (pressSlider)
	{
		return;
	}
   double pos= XVideoThread::Get()->GetPots();
   ui.playSlider->setValue(pos * 1000);

}
void XVideoUI::openFile()
{
	//打开文件
	QString fileName = QFileDialog::getOpenFileName(this, tr("选择视频文件"), "", tr("Video Files (*.mp4 *.avi *.mkv)"));
	if (!fileName.isEmpty()) {
		//在这里处理打开的视频文件，例如加载到视频播放器中
		//绘制视频帧到XVideoWidget
		//测试
		//QMessageBox::information(this, tr("文件路径"), fileName);
		
		if (!XVideoThread::Get()->Open(fileName.toStdString()))
		{
			QMessageBox::critical(this, tr("错误"), tr("无法打开视频文件！"));
			return;
		}
		else
		{
			XVideoThread::Get()->start();
		}
	}
	PlayVideo();
}
//视频剪切左端点
void XVideoUI::CutLeft(int pos)
{
	XVideoThread::Get()->SetBegin((double)pos / 1000.0);
	SetPos(pos);
}
//视频剪切右端点
void XVideoUI::CutRight(int pos)
{
	XVideoThread::Get()->SetEnd((double)pos / 1000.0);
}
//滑动条改变，改变播放位置
void XVideoUI::SetPos(int pos)
{
	XVideoThread::Get()->Seek((double)pos / 1000.0);
}
void XVideoUI::SliderPress()
{
    pressSlider = true;
	
}
void XVideoUI::SliderRelease()
{
	pressSlider = false;
}
//视频合并
void XVideoUI::Merge()
{
	isMark = false;
	isBlend = false;
	isMegre = false;
	QString fileName = QFileDialog::getOpenFileName(this, tr("选择融合视频文件"), "", tr("Video Files (*.mp4 *.avi *.mkv)"));
	if (fileName.isEmpty()) {
		return;
	}
	isMegre=XVideoThread::Get()->Open2(fileName.toStdString());
}
//视频融合
void XVideoUI::Blend() {
	
	isMark = false;
	isBlend = false;
	isMegre = false;
	QString fileName = QFileDialog::getOpenFileName(this, tr("选择融合视频文件"), "", tr("Video Files (*.mp4 *.avi *.mkv)"));
	if (fileName.isEmpty()) {
		return;
	}

	isBlend=XVideoThread::Get()->Open2(fileName.toStdString());
	//std::cout << isBlend << std::endl;

}
//设置水印
void XVideoUI::SetMark()
{
	isMark = false;
	isBlend = false;
	isMegre = false;
	QString fileName = QFileDialog::getOpenFileName(this, tr("选择水印图片"), "", tr("Image Files (*.png *.jpg *.bmp)"));
	if (fileName.isEmpty()) {
		
		return;
	}
	cv::Mat mark = cv::imread(fileName.toStdString(), cv::IMREAD_UNCHANGED);
	if (mark.empty())
	{
		QMessageBox::critical(this, tr("错误"), tr("无法打开水印图片！"));
		return;
	}
	XVideoThread::Get()->SetMark(mark);
	isMark = true;
}
void XVideoUI::SetParam()
{
	XFilter::Get()->Clear();
	isColor = true;
	//设置对比度和亮度
	if (ui.bright->value() >= 0 || ui.contrast->value() > 1)
	{

		XFilter::Get()->Add(XTask{ XTASK_GAIN,
			{(double)ui.bright->value(),ui.contrast->value()}
			});
	}
	else
	{
		QMessageBox::warning(this, tr("参数错误"), tr("亮度必须大于0，对比度必须大于1！"));
	}
	//颜色调整
	if (ui.color->currentIndex() == 1)
	{
		XFilter::Get()->Add(XTask{ XTASK_GRAY });
		isColor = false;
	}
	//设置旋转1:90;2:180;3:270
	if (ui.rotateBox->currentIndex()==1)
	{
		XFilter::Get()->Add(XTask{ XTASK_ROTATE90 });
	}
	else if (ui.rotateBox->currentIndex() == 2)
	{
		XFilter::Get()->Add(XTask{ XTASK_ROTATE180 });
	}
	else if (ui.rotateBox->currentIndex() == 3)
	{
		XFilter::Get()->Add(XTask{ XTASK_ROTATE270 });
	}
	//设置镜像1:上下镜像;2:左右镜像;3:上下左右镜像
	if (ui.flipBox->currentIndex() == 1)
	{
		XFilter::Get()->Add(XTask{ XTASK_FLIPX });
	}
	else if (ui.flipBox->currentIndex() == 2)
	{
		XFilter::Get()->Add(XTask{ XTASK_FLIPY });
	}
	else if (ui.flipBox->currentIndex() == 3)
	{
		XFilter::Get()->Add(XTask{ XTASK_FLIPXY });
	}

	//设置视频裁剪
	bool isClip = false;
	double x = ui.clipx->value();
	double y = ui.clipy->value();
	double w = ui.clipw->value();
	double h = ui.cliph->value();
	if (x + y + w + h > 0.0001)
	{
		isClip = true;
		XFilter::Get()->Add(XTask{ XTASK_CLIP, { x,y,w,h } });
		double cw = XVideoThread::Get()->width;
		double ch = XVideoThread::Get()->height;
		XFilter::Get()->Add(XTask{ XTASK_RESIZE, { cw,ch } });
	}

	//图像金字塔
	bool isPy = false;
	double down = ui.pyrdown->value();
	double up = ui.pyrup->value();
	if (down > 0)
	{
		isPy = true;
		XFilter::Get()->Add(XTask{ XTASK_PYRDOWN, { down } });
		int w = XVideoThread::Get()->width;
		int h = XVideoThread::Get()->height;
		for (int i = 0; i < down; i++)
		{
			w =w/2;
			h = h/2;
		}
		ui.width->setValue(w);
		ui.height->setValue(h);
	}
	if (up > 0)
	{
		isPy = true;
		XFilter::Get()->Add(XTask{ XTASK_PYRUP, { up } });
		int w = XVideoThread::Get()->width;
		int h= XVideoThread::Get()->height;
		for (int i = 0; i < up; i++)
		{
			w = w*2;
			h = h*2;
		}
		ui.width->setValue(w);
		ui.height->setValue(h);
	}
	//设置尺寸调整
	double rw = ui.width->value();
	double rh = ui.height->value();
	if (!isMegre&&!isClip&&!isPy&&rw> 0.0001 && rh > 0.0001)
	{
		XFilter::Get()->Add(XTask{ XTASK_RESIZE, { rw, rh } });
	}
	if (rw > 0 && rh > 0)
	{
		int srcW = XVideoThread::Get()->width;
		int srcH = XVideoThread::Get()->height;
		if (rw != srcW || rh != srcH)
		{
			XFilter::Get()->Add(XTask{ XTASK_RESIZE, { rw, rh } });
		}
	}
	//设置水印
	if (isMark)
	{
		double mx = ui.mx->value();
		double my = ui.my->value();
		double alpha = ui.malph->value();
		XFilter::Get()->Add(XTask{ XTASK_MARK,{mx,my,alpha} });
	}
	//融合
	if (isBlend)
	{
		double alpha = ui.balph->value();
		XFilter::Get()->Add(XTask{ XTASK_BLEND,{alpha} });
	}
	//合并
	if (isMegre)
	{
		double h2 = XVideoThread::Get()->height2;
		double h1 = XVideoThread::Get()->height;
		double w2 = XVideoThread::Get()->width2 * (h2 / h1);
		XFilter::Get()->Add(XTask{ XTASK_MERGE });
		ui.width->setValue(XVideoThread::Get()->width+w2);
		ui.height->setValue(h1);
	}
}
void XVideoUI::ExportVideo()
{
	if (isExport)
	{
		//停止导出
		XVideoThread::Get()->StopSave();
		isExport = false;
		ui.exportButton->setText(tr("Start Export"));
		return;
	}
	//开始导出
	QString fileName = QFileDialog::getSaveFileName(this, tr("保存视频文件"), "", tr("Video Files (*.mp4 *.avi *.mkv)"));
	if (fileName.isEmpty())
	{
		return;
	}
	int w = ui.width->value();
	int h = ui.height->value();
	if (XVideoThread::Get()->StartSave(fileName.toStdString(),w,h,isColor))
	{
		isExport = true;
		ui.exportButton->setText(tr("Stop Export"));
		return;
	}
}
//导出视频到结尾，停止导出
void XVideoUI::ExportEnd()
{
	isExport = false;
	ui.exportButton->setText(tr("Start Export"));
	std::string srcfile = XVideoThread::Get()->src1file;
	std::string desfile = XVideoThread::Get()->desfile;
	//处理音频
	int ss = 0;
	int t = 0;
	ss=XVideoThread::Get()->totalMs*((double)ui.startplay->value()/1000.0);
	int end = XVideoThread::Get()->totalMs * ((double)ui.endplay->value() / 1000.0);
	if (end - ss < 1000)
	{
		t = 1000;
	}
	else
	{
		t = end - ss;
	}
	XAudio::Get().ExportA(srcfile,srcfile+".mp3",ss,t);
	std::string tmp = desfile+".mp4";
	QFile::remove(tmp.c_str());
	QFile::rename(desfile.c_str(), tmp.c_str());
	XAudio::Get().Merge(tmp, srcfile + ".mp3", desfile);
}
//播放视频
void XVideoUI::PlayVideo()
{

	ui.pauseButton->show();
	ui.pauseButton->setGeometry(ui.playButton->geometry());
	XVideoThread::Get()->Play();
	ui.playButton->hide();
}
//暂停视频
void XVideoUI::PauseVideo()
{
	ui.playButton->show();
	//ui.playButton->setGeometry(ui.pauseButton->geometry());
	XVideoThread::Get()->Pause();
	ui.pauseButton->hide();
}
