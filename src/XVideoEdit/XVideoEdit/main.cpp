#include "XVideoUI.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    XVideoUI window;
    window.show();
    return app.exec();
}
