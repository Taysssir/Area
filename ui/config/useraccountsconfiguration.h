#ifndef USERACCOUNTSCONFIGURATION_H
#define USERACCOUNTSCONFIGURATION_H

#include <QWidget>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include "core/appinstance.h"

namespace Ui {
class useraccountsconfiguration;
}

class UserAccountsConfiguration : public QWidget, public core::EventClient<UserAccountsConfiguration>
{
    Q_OBJECT

public:
    explicit UserAccountsConfiguration(core::AppInstance & instance, QWidget *parent = 0);
    ~UserAccountsConfiguration();

private:
    Ui::useraccountsconfiguration *ui;

    void loadUserAccounts();
    void saveUserAccounts();

private slots:
    void slot_addUser();
    void slot_removeUser();
    void slot_unameOrPassTextChanged(QString);
    void slot_userSelectionChanged();
    void slot_onDlgBtnBxClicked(QAbstractButton* button);
};

#endif // USERACCOUNTSCONFIGURATION_H
