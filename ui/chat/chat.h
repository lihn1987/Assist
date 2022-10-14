#ifndef CHAT_H
#define CHAT_H

#include <QWidget>
class NetMessage;
namespace Ui {
class Chat;
}

class Chat : public QWidget
{
    Q_OBJECT

public:
    explicit Chat(QWidget *parent = nullptr);
    ~Chat();

private slots:
    void on_btn_send_clicked();
public:
    void OnConnect();
    void OnRecieveMsg(std::shared_ptr<NetMessage> msg);
private:
    Ui::Chat *ui;
};

#endif // CHAT_H
