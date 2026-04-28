#include <iostream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
using namespace cv;
#include "XVideothread.h"
#include "XFilter.h"
#include <QMessageBox>
//1号视频源
static VideoCapture cap1;
//保存视频
static VideoWriter vw;

XVideoThread::XVideoThread()
{
	
}

XVideoThread::~XVideoThread()
{
    // 析构时停止线程，释放视频源
	requestInterruption(); // 请求线程中断
	wait(); // 等待线程结束
    if (cap1.isOpened()) {
        cap1.release(); // 释放视频源
    }
    if (vw.isOpened())  vw.release();
}

bool XVideoThread::Seek(double pos)
{ 
    double totalFrames = cap1.get(CAP_PROP_FRAME_COUNT);
    int frame= (int)pos*totalFrames;
	return Seek(frame);
}
//Seek()函数,跳转视频
/// @param frame 跳转帧位置
bool XVideoThread::Seek(int pos)
{
    QMutexLocker locker(&mutex);
    if (!cap1.isOpened()) {
        locker.unlock();
        return false;
    }
	int re = cap1.set(CAP_PROP_POS_FRAMES, pos);
	locker.unlock();
   return re;
}
double XVideoThread::GetPots()
{
    double pos = 0;
    QMutexLocker locker(&mutex);
	if (!cap1.isOpened()) {
		locker.unlock();
        return pos;
	}
    //获取总帧数
    double totalFrames = cap1.get(CAP_PROP_FRAME_COUNT);
    //获取当前帧数
    double cur=cap1.get(CAP_PROP_POS_FRAMES);
    locker.unlock();
    //计算进度
    if (totalFrames > 0.001)
    {
        pos = cur / totalFrames;
    }
    return pos;

}
void XVideoThread::run()
{
    cv::Mat mat1;
    while (!isInterruptionRequested()) { // 用Qt线程中断标志替代死循环
        // 自动加锁：仅保护后续对cap1的读写操作
        QMutexLocker locker(&mutex);
        if (!cap1.isOpened()) {
            locker.unlock();
            msleep(6);
            continue;
        }
        if (!isPlay)
        {
            locker.unlock();
			msleep(6);
            continue;
        }
		bool readSuccess = cap1.read(mat1);
        locker.unlock();
        if (!readSuccess || mat1.empty()) {
            // 【必须加锁】判断 isWrite 并执行 StopSave
            bool needStop = false;
            {
                QMutexLocker locker(&mutex);
                if (isWrite)
                {
                    needStop = true;
                }
            }//释放锁
			//导出到视频结尾，停止保存视频
            if (needStop)
            {
                qCritical() << "视频读取结束，停止导出";
                StopSave();// 停止保存视频
				SaveEend(); // 发送导出视频结束信号
            }
                msleep(5);
                continue;
        }

        // 发送原视频信号（注意：Qt跨线程信号需确保类型注册成功）
        if(!isWrite)
            emit ViewImage1(mat1.clone()); // 克隆Mat，避免线程间数据竞争

        //处理视频
		Mat des=XFilter::Get()->Filter(mat1, Mat());

        // 发送生成视频信号（注意：Qt跨线程信号需确保类型注册成功）
       //if (isWrite)
            emit ViewDes(des.clone()); // 克隆Mat，避免线程间数据竞争
        int s = 0;
        s = 1000 / fps;
		if (isWrite)
		{
            s = 1;
            msleep(s);
			QMutexLocker locker(&mutex);
            if (vw.isOpened()) {
                vw.write(des); // 写入视频帧
            }
			
		}

       // msleep(40); // 控制帧率
     
        msleep(s);
	}
}
bool XVideoThread::Open(const std::string file)
{

    // 自动加锁：仅保护后续对cap1的读写操作
    QMutexLocker locker(&mutex);
    // 先释放原有视频源
    if (cap1.isOpened()) {
        cap1.release();
    }
    bool re = cap1.open(file);
    std::cout << "open video file " << file << " " << re << std::endl;
    if (!re)
    {
        return re;
    }
    //获取fps

    fps = cap1.get(CAP_PROP_FPS);
	//获取视频宽
	this->width = cap1.get(CAP_PROP_FRAME_WIDTH);
    //获取视频高
	this->height = cap1.get(CAP_PROP_FRAME_HEIGHT);
    if (fps <= 0)
    {
        fps = 25;
    }
    return re;
}

bool XVideoThread::StartSave(const std::string filename, int width, int height)
{
	// 这里可以实现视频保存功能，使用OpenCV的VideoWriter
	// 注意：需要确保线程安全，可能需要额外的成员变量来管理VideoWriter对象
    
	std::cout << "开始导出视频 " << filename << std::endl;
    Seek(0);
	if (!cap1.isOpened())
	{
        qCritical() << "视频未打开，无法导出";
		return false;
	}
	if (width <= 0 )
		//获取视频宽
		width = static_cast<int>(cap1.get(CAP_PROP_FRAME_WIDTH));

    if (height <= 0)
        //获取视频高
        height = static_cast<int>(cap1.get(CAP_PROP_FRAME_HEIGHT));

    QMutexLocker locker(&mutex);
    int fourcc = VideoWriter::fourcc('H', '2', '6', '4');
    vw.open(filename, fourcc,
        this->fps,
        Size(width, height));
	if (!vw.isOpened())
	{
        qCritical() << "视频导出失败";
        return false;
	}
    this->isWrite = true;
	return true; // 返回是否成功开始保存
}

void XVideoThread::StopSave()
{
    std::cout << "停止导出视频 "  << std::endl;
	// 这里可以实现停止视频保存的功能，确保正确释放VideoWriter资源
	QMutexLocker locker(&mutex);
    this->isWrite = false;
    if (vw.isOpened()) {
        vw.release();
    }
}