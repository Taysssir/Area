#ifndef DIALOGNEWSENSOR_H
#define DIALOGNEWSENSOR_H

#include <QDialog>
#include <QObject>

namespace Ui {
class DialogNewSensor;
}

class DialogNewSensor : public QDialog
{
    Q_OBJECT

public:
    explicit DialogNewSensor(QString name, int type, QWidget *parent = 0);
    ~DialogNewSensor();


    QString getMName() const;
    void setMName(const QString &value);

    int getMType() const;
    void setMType(int value);

public slots:
    void updateNameFromType();
    void updateName();

private:
    Ui::DialogNewSensor *ui;
    QString mName;
    int mType;

};

#endif // DIALOGNEWSENSOR_H
