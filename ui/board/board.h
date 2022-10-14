#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QSystemTrayIcon>
QT_BEGIN_NAMESPACE
namespace Ui { class Board; }
QT_END_NAMESPACE

class SocketItem;
class NetMessage;

class Board : public QWidget{
    Q_OBJECT

public:
    Board(QWidget *parent = nullptr);
    ~Board();
public:
    void Flush();
private:
    // 做一些初始化工作
    void Init();
    // 初始化任务栏图标
    void InitIcon();
public:
    void OnConnect();
    void OnRecieveMsg(std::shared_ptr<NetMessage> msg);
private:
    Ui::Board *ui;
    QSystemTrayIcon* m_tray_icon;
private slots:
    void OnTrayIconActivated(QSystemTrayIcon::ActivationReason reson);
    void on_pushButton_clicked();
    void on_btn_test1_clicked();

    void on_btn_test2_clicked();

};
#endif // MAINWIDGET_H
