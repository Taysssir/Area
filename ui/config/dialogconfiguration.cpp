#include <QListWidget>
#include <QStackedWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "dialogconfiguration.h"
#include "robotconfiguration.h"
#include "networkconfiguration.h"
#include "siaconfiguration.h"
#include "restapiconfiguration.h"
#include "useraccountsconfiguration.h"
#include "core/appinstance.h"

ConfigDialog::ConfigDialog(core::AppInstance *instance)
{
    contentsWidget = new QListWidget(this);
    contentsWidget->setViewMode(QListView::ListMode);
    contentsWidget->setIconSize(QSize(60, 60));
    contentsWidget->setResizeMode(QListView::Adjust);
    contentsWidget->setMovement(QListView::Static);
    contentsWidget->setFixedWidth(200);
    contentsWidget->setMinimumHeight(300);
    contentsWidget->setSpacing(2);

    pagesWidget = new QStackedWidget(this);
    pagesWidget->setFrameShadow(QStackedWidget::Sunken);
    pagesWidget->setFrameShape(QStackedWidget::StyledPanel);

//    NetworkConfiguration* network_config =  new NetworkConfiguration(*instance);
//    pagesWidget->addWidget( network_config );
//    pagesWidget->setContentsMargins(1,1,1,1);
    //connect(network_config,SIGNAL(mementoUpdated()),this,SLOT(connectMementoUpdated()));

    SiaConfiguration* sia_config = new SiaConfiguration(*instance);
    pagesWidget->addWidget( sia_config );

//    RobotConfiguration* robot_config = new RobotConfiguration(*instance);
//    pagesWidget->addWidget( robot_config );

    RestApiConfiguration* rest_api_config = new RestApiConfiguration(*instance);
    pagesWidget->addWidget( rest_api_config );

    UserAccountsConfiguration* user_accounts_config = new UserAccountsConfiguration(*instance);
    pagesWidget->addWidget( user_accounts_config );

    createIcons();

    QPushButton *closeButton = new QPushButton(tr("Close"), this);
    connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));
    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    buttonsLayout->addWidget(closeButton, 0, Qt::AlignRight);

    QHBoxLayout *horizontalLayout = new QHBoxLayout();
    horizontalLayout->addWidget(contentsWidget);
    horizontalLayout->addWidget(pagesWidget, 1);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addLayout(horizontalLayout);
    mainLayout->addLayout(buttonsLayout);
    setLayout(mainLayout);

    setWindowTitle(tr("Settings Dialog"));
    setWindowFlags( Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint );
    contentsWidget->setCurrentRow(0);
}

void ConfigDialog::createIcons()
{
//    QListWidgetItem *networkButton = new QListWidgetItem(contentsWidget);
//    networkButton->setIcon(QIcon(":/configuration/network"));
//    networkButton->setText(tr("Network"));
//    networkButton->setFlags(Qt::ItemIsSelectable /*| Qt::ItemIsEnabled*/);

    QListWidgetItem *siaButton = new QListWidgetItem(contentsWidget);
    siaButton->setIcon(QIcon(":/configuration/siadc09"));
    siaButton->setText(tr("Sia parameters"));
    siaButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

//    QListWidgetItem *evigilanteButton = new QListWidgetItem(contentsWidget);
//    evigilanteButton->setIcon(QIcon(":/map/evigilante"));
//    evigilanteButton->setText(tr("e-vigilante"));
//    evigilanteButton->setFlags(Qt::ItemIsSelectable /*| Qt::ItemIsEnabled*/);

    QListWidgetItem *restApiButton = new QListWidgetItem(contentsWidget);
    restApiButton->setIcon(QIcon(":/configuration/rest_api"));
    restApiButton->setText(tr("REST API"));
    restApiButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    QListWidgetItem *acountsButton = new QListWidgetItem(contentsWidget);
    acountsButton->setIcon(QIcon(":/configuration/account"));
    acountsButton->setText(tr("User accounts"));
    acountsButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    connect( contentsWidget, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
             this,           SLOT(changePage(QListWidgetItem*,QListWidgetItem*)) );
}

void ConfigDialog::changePage(QListWidgetItem *current, QListWidgetItem *previous)
{
    if (!current)
        current = previous;

    pagesWidget->setCurrentIndex(contentsWidget->row(current));
}

void ConfigDialog::connectMementoUpdated()
{
    emit mementoUpdated();
}
