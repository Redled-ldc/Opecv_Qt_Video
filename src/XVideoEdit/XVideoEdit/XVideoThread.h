#pragma once

#include <opencv2/core.hpp>
#include <QThread>
#include <string>
#include <QMutex>

class XVideoThread :public QThread
{
	Q_OBJECT
public:

	int fps = 0;
	int width = 0;
	int height = 0;
	//单例模式，获取实例对象
	static XVideoThread* Get()
	{
		static XVideoThread vt;
		return &vt;
	}
	//打开1视频源文件
	bool Open(const std::string file);

	//播放视频
	void Play() { isPlay = true; }

	// 暂停视频
	void Pause() { isPlay = false; }


	//返回当前播放位置
	double GetPots();

	//Seek()函数,跳转视频
	/// @param frame 跳转帧位置
    bool Seek(int pos);
	
	bool Seek(double pos);

	//开始保存视频
	bool StartSave(const std::string filename, int width = 0, int height = 0);

	//停止保存视频，写入视频帧的索引
	void StopSave();

	~XVideoThread();
	//线程入口函数
	void run()override;
signals:
	//发送原视频帧1信号，参数为视频帧数据
	void ViewImage1(cv::Mat mat);
	//发送生成视频信号，参数为视频帧数据
	void ViewDes(cv::Mat mat);
	//导出视频结束信号
	void SaveEend();
protected:
	XVideoThread();
	//是否开始写视频
	std::atomic<bool> isWrite = false;
	std::atomic<bool> isPlay = false;
	QMutex mutex;

};

