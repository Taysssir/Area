#include <QMessageBox>
#include <QObject>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFileInfoList>

#include "sentinel-admin_core/constants.h"

#include "qt_utils.h"

namespace core
{

    void alert(QString const& message, QWidget * parent)
    {
        QMessageBox *msgBox = new QMessageBox(parent);
        msgBox->setParent(0);
        msgBox->setWindowTitle(constants::APP_NAME);
        msgBox->setText(message);
        msgBox->setWindowFlags(Qt::Dialog | Qt::WindowStaysOnTopHint);
        //msgBox->raise();
        msgBox->show();
    }

    bool question(QString const& message)
    {
        QMessageBox *msgBox = new QMessageBox;
        msgBox->setParent(0);
        msgBox->setWindowTitle(constants::APP_NAME);
        msgBox->setText(message);
        msgBox->setWindowFlags(Qt::Dialog |Qt::WindowStaysOnTopHint);
        msgBox->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox->setButtonText(QMessageBox::Yes, QObject::tr("yes"));
        msgBox->setButtonText(QMessageBox::No, QObject::tr("no"));
        msgBox->raise();
        return msgBox->exec() == QMessageBox::Yes;
    }

    bool removeDirContent(QString const& dirName, bool first)
    {
        bool result = true;
        QDir dir(dirName);

        if (dir.exists(dirName)) {
            Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
                if (info.isDir()) {
                    result = removeDirContent(info.absoluteFilePath(), false);
                }
                else {
                    result = QFile::remove(info.absoluteFilePath());
                }

                if (!result) {
                    return result;
                }
            }
            if(first == false)
                result = dir.rmdir(dirName);
        }

        return result;
    }

}
