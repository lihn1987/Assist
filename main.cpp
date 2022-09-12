#include "ui/main_widget/main_widget.h"

#include <QApplication>
#include <QFile>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWidget w;
//    w.show();
    return a.exec();
}
