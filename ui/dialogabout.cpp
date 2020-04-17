#include <QUrl>
#include <QDesktopServices>

#include "core/workspace.h"
#include "dialogabout.h"
#include "ui_dialogabout.h"
#include "core/constants.h"
#include "sentinel-admin_core/constants.h"

namespace
{

    static QString compilerString()
    {
#if defined(Q_CC_CLANG) // must be before GNU, because clang claims to be GNU too
        QString isAppleString;
#if defined(__apple_build_version__) // Apple clang has other version numbers
        isAppleString = QLatin1String(" (Apple)");
#endif
        return QLatin1String("Clang " ) + QString::number(__clang_major__) + QLatin1Char('.')
                + QString::number(__clang_minor__) + isAppleString;
#elif defined(Q_CC_GNU)
        return QLatin1String("GCC " ) + QLatin1String(__VERSION__);
#elif defined(Q_CC_MSVC)
        if (_MSC_VER >= 1800) // 1800: MSVC 2013 (yearly release cycle)
            return QLatin1String("MSVC ") + QString::number(2008 + ((_MSC_VER / 100) - 13));
        if (_MSC_VER >= 1500) // 1500: MSVC 2008, 1600: MSVC 2010, ... (2-year release cycle)
            return QLatin1String("MSVC ") + QString::number(2008 + 2 * ((_MSC_VER / 100) - 15));
#endif
        return QLatin1String("<unknown compiler>");
    }

    inline QString buildCompatibilityString()
    {
        return QString("Based on Qt %1 (%2, %3 bit)").arg(QLatin1String(qVersion()),
                                                          compilerString(),
                                                          QString::number(QSysInfo::WordSize));
    }

}

DialogAbout::DialogAbout(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogAbout)
{
    ui->setupUi(this);
    const QString description = tr(
                "<h3>Sentinel-admin %1.%2.%3 [%4]</h3>"
                "Built on %5 - %6, %7 - %8")
            .arg(QString::number(constants::MAJOR),
                 QString::number(constants::MINOR),
                 QString::number(constants::PATCH),
                 QLatin1String(GIT_BRANCH),
                 QLatin1String(__DATE__), QLatin1String(__TIME__),
                 buildCompatibilityString(),
                 QLatin1String(GIT_SHA));

    this->setFixedSize(744,400);

    QLabel *description_label = ui->label_version;
    description_label->setText( description );
    description_label->setWordWrap(true);
    description_label->setOpenExternalLinks(true);
    description_label->setTextInteractionFlags(Qt::TextBrowserInteraction);
    description_label->setStyleSheet("color: white;font:bold;");

    QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect(this);
    effect->setBlurRadius(0);
    effect->setColor(Qt::darkGray);
    effect->setOffset(1,2);
    description_label->setGraphicsEffect(effect);

    setWindowFlags( Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint );
}

DialogAbout::~DialogAbout()
{
    delete ui;
}

void DialogAbout::on_pushButto_ok_clicked()
{
   accept();
}
