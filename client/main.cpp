

#include <QApplication>
#include <QFile>
#include <stdio.h>

#include "app_engine.h"
int main(int argc, char *argv[])
{
    // printf immediately
    setbuf(stdout, NULL);
    QApplication a(argc, argv);
    GetAppEngine();
    a.exec();
    ClearAppEngine();
    return 0;
}
