#ifndef DIALOGCONFIGURATION_H
#define DIALOGCONFIGURATION_H

#include <QDialog>

class QListWidget;
class QListWidgetItem;
class QStackedWidget;

namespace core
{
class AppInstance;
}

class ConfigDialog
        : public QDialog
{
    Q_OBJECT
public:
    ConfigDialog(core::AppInstance * instance);
public slots:
    void changePage(QListWidgetItem *current, QListWidgetItem *previous);
    void connectMementoUpdated();

signals:
    void mementoUpdated();

private:
    void createIcons();

    QListWidget *contentsWidget;
    QStackedWidget *pagesWidget;
};

#endif // DIALOGCONFIGURATION_H
