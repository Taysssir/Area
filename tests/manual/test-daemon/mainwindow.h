#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "robot.h"

namespace Ui {
    class MainWindow;
}

class MainWindow
        : public QMainWindow
        , public skynet::Robot::Receiver
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    virtual void onConnected();
    virtual void onDisconnected();
    virtual void onReceive(skynet::PidofResponse const& event);
    virtual void onReceive(skynet::ConfigHashResponse const& event);
private slots:
    void on_pushButton_run_clicked();

    void on_pushButton_stop_clicked();

    void on_pushButton_kill_clicked();

    void on_pushButton_pids_clicked();

    void on_pushButton_connect_clicked();

    void on_pushButton_hash_clicked();
signals:
   void maj(QString const& str) ;
private slots:
    void majHASH(QString const& str);
private:
    Ui::MainWindow *ui;
    skynet::Robot * robot_;
};

#endif // MAINWINDOW_H
