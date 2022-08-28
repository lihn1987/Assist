#include "main_widget.h"
#include "ui_main_widget.h"
#include <QDebug>
#include <QDragEnterEvent>
#include <QMimeData>
MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWidget){
    ui->setupUi(this);
    Init();
    InitEvent();


}

MainWidget::~MainWidget(){
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

void MainWidget::InitEvent(){
    ui->widget1->installEventFilter(this);
}

void MainWidget::OnTrayIconActivated(QSystemTrayIcon::ActivationReason reson){
    switch(reson) {
    case QSystemTrayIcon::DoubleClick: {
        qDebug()<<QString::fromWCharArray(L"双击任务栏图标");
        this->show();
        auto window_state = this->windowState();
        this->setWindowState(window_state|Qt::WindowActive);
        break;
    }
    case QSystemTrayIcon::Trigger:{
        qDebug()<<QString::fromWCharArray(L"单击任务栏图标").toStdString().c_str();
        qDebug()<<QString("打");
        qDebug()<<QString("打").toStdString().c_str();
        qDebug()<<QString::fromStdWString(L"打");
        qDebug()<<"???";
        qDebug()<<"阿斯蒂芬";
        qDebug()<<QStringLiteral("中文");
        break;
    }
    case QSystemTrayIcon::MiddleClick:{
        qDebug()<<QString::fromWCharArray(L"中健任务栏图标");
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

bool MainWidget::eventFilter(QObject *obj, QEvent *event){
//    qDebug()<<event;
//    QDragEnterEvent
//    QDragLeaveEvent
//    QDragMoveEvent
//    dropEvent
    if (obj == ui->widget1 && event->type() == QEvent::DragEnter) {
        qDebug()<<"drag enter";
        QDragEnterEvent* enter_event = dynamic_cast<QDragEnterEvent*>(event);
        if (enter_event->mimeData()->hasUrls()) {
            enter_event->acceptProposedAction();
            qDebug()<<enter_event->mimeData()->urls();
        }
    }
}





