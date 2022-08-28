#include "main_widget.h"

#include <QApplication>
#include <QFile>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWidget w;
    w.show();
    QFile fff("./test.txt");
    fff.open(QFile::ReadWrite);
    fff.close();
    return a.exec();
}
