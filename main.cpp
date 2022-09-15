#include "ui/main_widget/main_widget.h"

#include <QApplication>
#include <QFile>
#include <stdio.h>
int main(int argc, char *argv[])
{
    // printf immediately
    setbuf(stdout, NULL);
    QApplication a(argc, argv);
    MainWidget w;
    return a.exec();
}
