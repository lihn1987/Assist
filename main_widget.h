#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QSystemTrayIcon>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWidget; }
QT_END_NAMESPACE

class MainWidget : public QWidget{
    Q_OBJECT

public:
    MainWidget(QWidget *parent = nullptr);
    ~MainWidget();
private:
    // 做一些初始化工作
    void Init();
    // 初始化任务栏图标
    void InitIcon();
    //初始化event
    void InitEvent();
    void SetFixSize(QWidget* widget, uint32_t w, uint32_t h);
    QWidget* CreateClickWidget(uint32_t icon_size, uint32_t border_width, QString icon_path);
private:
    Ui::MainWidget *ui;
    QSystemTrayIcon* m_tray_icon;
private slots:
    void OnTrayIconActivated(QSystemTrayIcon::ActivationReason reson);
    void on_pushButton_clicked();
    void on_btn_test1_clicked();

    void on_btn_test2_clicked();

private:
    bool eventFilter(QObject* obj, QEvent* event);
};
#endif // MAINWIDGET_H
