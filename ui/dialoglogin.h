#ifndef DIALOGLOGIN_H
#define DIALOGLOGIN_H

#include <QDialog>
#include <QMap>

namespace Ui
{
    class DialogLogin;
}

namespace core
{
    class AppInstance;
}

class DialogLogin : public QDialog
{
    Q_OBJECT

public:
    explicit DialogLogin(core::AppInstance &instance, QWidget *parent = 0);
    ~DialogLogin();

    void addLanguage(QString const &name, QString const &translate_file);

    QString languageSelected() const;
private slots:
    void on_pushButton_connect_clicked();

    void on_pushButton_cancel_clicked();

    void on_lineEdit_login_textChanged(const QString &arg1);

    void on_lineEdit_password_textChanged(const QString &arg1);

private:
    Ui::DialogLogin *ui;
    core::AppInstance &instance_;
    QString initialPassword_;
    QString language_selected_;
    QMap<QString, QString> languages_;
};

#endif // DIALOGLOGIN_H
