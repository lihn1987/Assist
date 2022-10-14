#include "board.h"
#include "ui_board.h"
#include <QDebug>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QFileInfo>
#include <QFileIconProvider>
#include <functional>

#include "../../client/app_engine.h"
#include "../../config/config.h"
#include "../../crypto_tools/string_tools.h"
#include "../../net_lib/chat_client.h"
Board::Board(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Board){
    ui->setupUi(this);
    Init();

}

Board::~Board(){
    m_tray_icon->hide();
    delete ui;
}

void Board::Init(){
    InitIcon();
}

void Board::InitIcon(){
    m_tray_icon = new QSystemTrayIcon(this);
    m_tray_icon->setIcon(QIcon(":/base/res/tray_icon.png"));
    m_tray_icon->show();
    connect(m_tray_icon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), SLOT(OnTrayIconActivated(QSystemTrayIcon::ActivationReason)));
}

void Board::OnConnect(){

}

void Board::OnRecieveMsg(std::shared_ptr<NetMessage> msg){
    int x;
    x = 0;
}

void Board::Flush(){
    ui->lbl_pubkey->setText(QString::fromStdString(Byte2HexAsc(GetAppEngine()->GetCurrentAccount()->GetPubKey())));
}

void Board::OnTrayIconActivated(QSystemTrayIcon::ActivationReason reson){
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


void Board::on_pushButton_clicked(){
    this->hide();
}

void Board::on_btn_test1_clicked(){

}

void Board::on_btn_test2_clicked(){
    QFileInfo file_info("C:\\Users\\Administrator\\Desktop\\guo\\2022-09-03.txt");
    QFileIconProvider seekIcon;
    QIcon icon = seekIcon.icon(file_info);
//    ui->btn_test2->setIcon(icon);
}

