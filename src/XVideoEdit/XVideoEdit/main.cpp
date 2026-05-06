#include "XVideoUI.h"
#include <QtWidgets/QApplication>
#include "XAudio.h"
int main(int argc, char *argv[])
{
	//XAudio::Get().ExportA("../../doc/vo.mp4", "../../doc/vo2.mp3");
	//XAudio::Get().Merge("../../doc/votest.mp4", "../../doc/vo2.mp3", "../../doc/vo3.mp4");
    QApplication app(argc, argv);
    XVideoUI window;
    window.show();
    return app.exec();
}
