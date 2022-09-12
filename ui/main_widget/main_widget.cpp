#include "main_widget.h"
#include "ui_main_widget.h"
#include <QDebug>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QFileInfo>
#include <QFileIconProvider>
#include "ui/login/login.h"

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWidget){
    ui->setupUi(this);
    Init();
    QWidget* d = new Login();

    d->show();
    this->hide();
}

MainWidget::~MainWidget(){
    m_tray_icon->hide();
    delete ui;
}

void MainWidget::Init(){
    InitIcon();
}

void MainWidget::InitIcon(){
    m_tray_icon = new QSystemTrayIcon(this);
    m_tray_icon->setIcon(QIcon(":/base/res/tray_icon.png"));
    m_tray_icon->show();
    connect(m_tray_icon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), SLOT(OnTrayIconActivated(QSystemTrayIcon::ActivationReason)));
}

void MainWidget::OnTrayIconActivated(QSystemTrayIcon::ActivationReason reson){
    switch(reson) {
    case QSystemTrayIcon::DoubleClick: {
        qDebug()<<QString::fromWCharArray(L"double click");
        this->show();
        auto window_state = this->windowState();
        this->setWindowState(window_state|Qt::WindowActive);
        break;
    }
    case QSystemTrayIcon::Trigger:{
        qDebug()<<QString::fromWCharArray(L"click").toStdString().c_str();
        break;
    }
    case QSystemTrayIcon::MiddleClick:{
        qDebug()<<QString::fromWCharArray(L"mid click ");
        break;
    }
    default:
        break;
    }
    qDebug()<<reson;
}


void MainWidget::on_pushButton_clicked(){
    this->hide();
}

void MainWidget::on_btn_test1_clicked(){

}

void MainWidget::on_btn_test2_clicked(){
    QFileInfo file_info("C:\\Users\\Administrator\\Desktop\\guo\\2022-09-03.txt");
    QFileIconProvider seekIcon;
    QIcon icon = seekIcon.icon(file_info);
    ui->btn_test2->setIcon(icon);
}

