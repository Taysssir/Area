#ifndef RESTAPICONFIGURATION_H
#define RESTAPICONFIGURATION_H

#include <QWidget>
#include <QMessageBox>
#include "core/appinstance.h"

namespace Ui {
class restapiconfiguration;
}

class RestApiConfiguration : public QWidget, public core::EventClient<RestApiConfiguration>
{
    Q_OBJECT

public:
    explicit RestApiConfiguration(core::AppInstance & instance, QWidget *parent = 0);
    ~RestApiConfiguration();

private:
    Ui::restapiconfiguration *ui;

    bool validateIp4(const QString& address);
    void loadIpWhitelist();
    void saveIpWhitelist();

private slots:
    void slot_addIP();
    void slot_removeIP();
    void slot_ipSelectionChanged();
    void slot_ipTextChanged(QString ip);
    void slot_onDlgBtnBxClicked(QAbstractButton* button);
};

#endif // RESTAPICONFIGURATION_H
