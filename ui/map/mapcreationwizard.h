#ifndef MAPCREATIONWIZARD_H
#define MAPCREATIONWIZARD_H

#include <QWizard>
#include <QCloseEvent>
#include <QMessageBox>

namespace Ui {
class MapCreationWizard;
}

class MapCreationWizard : public QWizard
{
    Q_OBJECT

public:
    explicit MapCreationWizard(QWidget *parent = 0);
    ~MapCreationWizard();

    void setBusyMapRecoveryProgressBar(bool busy);
    void updateMapRecoveryProgressBarValue(int iValue);

    bool isWaitingForNewMap();

    virtual void hideEvent(QHideEvent *hideEvent);
    virtual void closeEvent(QCloseEvent *closeEvent);
    virtual void keyPressEvent(QKeyEvent *keyEvent);

private:
    Ui::MapCreationWizard *ui;

    void setupMapSizeAndResolution();
    int roundItUpToMultiple8(int iValue);
    void updateCustomApproxSizes();

    bool waitingForNewMap_;
    unsigned int mapSize_;
    float mapResolution_;

private slots:
    void on_currentIdChanged(int);
    void cancelWizard();
    void on_customSizeChanged(int);

signals:
    void createMap(bool create, float resolution, unsigned int size);
    void abortCarto();
};

#endif // MAPCREATIONWIZARD_H
