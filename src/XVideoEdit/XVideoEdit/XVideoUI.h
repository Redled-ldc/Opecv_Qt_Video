#pragma once

#include <QtWidgets/QWidget>
#include "ui_XVideoUI.h"
#include"XVideoThread.h"
class XVideoUI : public QWidget
{
    Q_OBJECT

public:
    XVideoUI(QWidget *parent = nullptr);
    ~XVideoUI();
    //重载定时器
    void timerEvent(QTimerEvent* e) override;

private:
    Ui::XVideoUIClass ui;
public slots:
	void on_pushButton_clicked();
	void openFile();
    //进度条按下
    void SliderPress();
    //进度条释放
    void SliderRelease();
    //拖动进度条
    void SetPos(int);
    //设置参数
	void SetParam();
    //导出视频
	void ExportVideo();
	//导出视频结束
	void ExportEnd();
    //视频播放
	void PlayVideo();
	//视频暂停
	void PauseVideo();
};

