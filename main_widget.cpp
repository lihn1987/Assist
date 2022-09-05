#include "main_widget.h"
#include "ui_main_widget.h"
#include <QDebug>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QFileInfo>
#include <QFileIconProvider>


MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWidget){
    ui->setupUi(this);
    Init();
    InitEvent();
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

void MainWidget::InitEvent(){
    ui->widget1->installEventFilter(this);
}

void MainWidget::SetFixSize(QWidget *widget, uint32_t w, uint32_t h){
    widget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    widget->setMinimumSize(w, h);
    widget->setMaximumSize(w, h);
    qDebug()<<"set fix size"<<w<<h;
}

QWidget *MainWidget::CreateClickWidget(uint32_t icon_size, uint32_t border_width, QString icon_path){
    //部分配置
    uint32_t label_height = 24;
    uint32_t v_space = 4;
    // 设置外层widget属性
    QWidget* widget = new QWidget();
    widget->setStyleSheet("background:#ff0000;");
    SetFixSize(widget, icon_size + border_width*2, icon_size + border_width*2 + label_height + v_space);


    // 设置layout相关
    QVBoxLayout* vlay = new QVBoxLayout();
    vlay->setContentsMargins(border_width, border_width, border_width, border_width);
    widget->setLayout(vlay);

    // 设置icon_btn
    QPushButton* icon_btn = new QPushButton();
    QFileInfo file_info("C:\\Users\\Administrator\\Desktop\\guo\\2022-09-03.txt");
    QFileIconProvider seekIcon;
    QIcon icon = seekIcon.icon(file_info);

    SetFixSize(icon_btn, icon_size, icon_size);
    icon_btn->setIcon(icon);
    icon_btn->setIconSize(QSize(icon_size,icon_size));
    icon_btn->setFlat(true);
    vlay->addWidget(icon_btn);

    // 设置标签
    QLabel* lbl = new QLabel("1234");
    SetFixSize(lbl, icon_size, label_height);
    lbl->setAlignment(Qt::AlignHCenter);
    vlay->addWidget(lbl);
    return widget;
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






void MainWidget::on_btn_test1_clicked(){
    QWidget* widget_new = CreateClickWidget(64, 10, "C:\\Users\\Administrator\\Desktop\\guo\\2022-09-03.txt");

    ui->grid_layout->addWidget(widget_new, 0, ui->grid_layout->columnCount());
    widget_new->show();
}


void MainWidget::on_btn_test2_clicked(){
    QFileInfo file_info("C:\\Users\\Administrator\\Desktop\\guo\\2022-09-03.txt");
    QFileIconProvider seekIcon;
    QIcon icon = seekIcon.icon(file_info);
    ui->btn_test2->setIcon(icon);
}

