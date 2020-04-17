#include <QSplitter>
#include <QMessageBox>
#include <QSizePolicy>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QProgressBar>
#include <thread>
#include <QMessageBox>
#include <QFileDialog>
#include <QStandardPaths>
#include <QDesktopServices>
#include <QItemSelection>
#include <QSplitter>
#include <QDebug>
#include <QBuffer>
#include <cstdio>

#include "ui/dialogabout.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui/map/mapwidget.h"
#include "ui/admin/patroladminwidget.h"
#include "ui/config/dialogconfiguration.h"
#include "ui/admin/patrolstab.h"
#include "core/constants.h"
#include "sentinel-admin_core/qt_utils.h"
#include "sentinel-common/helper/map.h"
#include "core/sentinel_logger.h"
#include "core/workspace.h"
#include "sentinel-common/core/helper.h"

MainWindow::MainWindow(core::AppInstance &instance, QWidget *parent)
    : QMainWindow(parent)
    , core::EventClient<MainWindow>(instance)
    , ui(new Ui::MainWindow)
    , mapWidget_(nullptr)
    , newMapWizard_(new MapCreationWizard(this))
    , bEditModeFlag(false)
    , transfer_( instance )
{
    ui->setupUi(this);

    ui->actionReboot_the_robot->setVisible(instance_.getExpertMode());

    setupToolBar();
    setupStatusBar();
    setupWidgets();

    is_trying_connection_ = false;
    robot_service_ = instance_.getService<Robot>();

    connect(robot_service_, SIGNAL(mapChanged()), this, SLOT(updateMap()));
    connect(robot_service_, SIGNAL(patrolsChanged()), this, SLOT(updatePatrols()));
    connect(robot_service_, SIGNAL(sensorsChanged()), this, SLOT(updateSensors()));
    connect(robot_service_, SIGNAL(zonesChanged()), this, SLOT(updateZones()));
    connect(robot_service_, SIGNAL(tasksScheduldedChanged()), this, SLOT(updateTasksScheduled()));

    //connect(ui->actionSave_As,SIGNAL(triggered()),this,SLOT(on_actionSaveAs_triggered()));
    connect(ui->actionNew_Configurations,SIGNAL(triggered()),this,SLOT(on_actionNew_Configurations_triggered()));

    connect(newMapWizard_, SIGNAL(createMap(bool,float,uint)), this, SLOT(createMap(bool,float,uint)));
    connect(newMapWizard_, SIGNAL(abortCarto()), this, SLOT(abortCarto()));

    subscribe<core::RobotConnected>(*this);
    subscribe<core::RobotDisconnected>(*this);
    subscribe<core::RobotStandby>(*this);
    subscribe<core::MapDownloading>(*this);
    subscribe<core::NewMapAvailable>(*this);

    //setWindowTitle(tr("%1 (%2)").arg(constants::APP_NAME).arg(instance.configuration()));

    on_actionNew_Configurations_triggered();

    connect(&transfer_, SIGNAL(downloading(int,double)), this, SLOT(downloading(int,double)));
    connect(&transfer_, SIGNAL(uploading(int,double)), this, SLOT(uploading(int,double)));
    connect(&transfer_, SIGNAL(downloaded(int,bool)), this, SLOT(downloaded(int,bool)));
    connect(&transfer_, SIGNAL(uploaded(int,bool)), this, SLOT(uploaded(int,bool)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupWidgets()
{
    patrolsTab_ = new PatrolsTab(instance_);
    patrolsTab_->setMaximumWidth(440);

    QHBoxLayout *layout = new QHBoxLayout();
    QVBoxLayout *layout_widget = new QVBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(0);
    layout_widget->setMargin(0);
    layout_widget->setSpacing(0);
    layout->addWidget(patrolsTab_);
    layout->addLayout(layout_widget);

    QSplitter *h_Splitter = new QSplitter(Qt::Vertical);
    layout_widget->addWidget(h_Splitter);
    mapWidget_ = new MapWidget(instance_, patrolsTab_);
    patrolsTab_->linkWithMapWidget(mapWidget_->patrolsItem());
    patrolsTab_->currentChanged(0);
    h_Splitter->addWidget(mapWidget_);

    calendar_widget_ = new CalendarWidget(instance_, this);
    calendar_widget_->setVisible(false);
    h_Splitter->addWidget(calendar_widget_);

    h_Splitter->setChildrenCollapsible(false);

    QSplitterHandle *handle = h_Splitter->handle(1);
    QVBoxLayout *shLayout = new QVBoxLayout(handle);
    shLayout->setSpacing(0);
    shLayout->setContentsMargins(9,0,0,0);

    QFrame *line = new QFrame(handle);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    shLayout->addWidget(line);

    QWidget *main = new QWidget(this);
    main->setLayout(layout);
    setCentralWidget(main);

    connect(mapWidget_, SIGNAL(activateZoneTool(bool)), this, SLOT(activateZone(bool)));
    connect(mapWidget_, SIGNAL(activatePointTool(bool)), this, SLOT(activatePoint(bool)));
    connect(mapWidget_, SIGNAL(activateSensorsTool(bool)), this, SLOT(activateSensors(bool)));
    connect(mapWidget_, SIGNAL(activateCalendarTool(bool)), this, SLOT(displayCalendar(bool)));

    connect(mapWidget_, SIGNAL(activateWhitePencilTool(bool)), this, SLOT(activateWhitePencil(bool)));
    connect(mapWidget_, SIGNAL(activateBlackPencilTool(bool)), this, SLOT(activateBlackPencil(bool)));
    connect(mapWidget_, SIGNAL(activateGrayPencilTool(bool)), this, SLOT(activateGrayPencil(bool)));

    connect(mapWidget_, SIGNAL(activateRulerTool(bool)), this, SLOT(activateRuler(bool)));

    connect(mapWidget_, SIGNAL(activateMapRotationTool(bool)), this, SLOT(activateMapRotation(bool)));

    connect(mapWidget_, SIGNAL(editModeTriggered(bool)), this, SLOT(on_actionMapEdit_triggered(bool)));

    connect(calendar_widget_,SIGNAL(updateTaskUsageStatus(QString const&)),patrolsTab_,SLOT(updatePatrolStatus(QString const&)));
    connect(calendar_widget_,SIGNAL(updateTasksUsageStatus()),patrolsTab_,SLOT(updateAllPatrolStatus()));
    connect(calendar_widget_,SIGNAL(signal_closeCalendar()),this,SLOT(slot_triggerCalendar()));

    connect(patrolsTab_,SIGNAL(removePatrolTasks(QString const&)),calendar_widget_,SLOT(removeTaskFromPatrol(QString const&)));
}

void MainWindow::setupToolBar()
{
    // tool bar configuration
    QToolBar *tb = ui->mainToolBar;

    // new
    new_config_ = new QAction(this);
    new_config_->setIcon(QPixmap(":/icons/toolbar/new"));
    new_config_->setToolTip(tr("New EOS Configuration"));
    tb->addAction(new_config_);
    connect(new_config_, SIGNAL(triggered()), this, SLOT(on_actionNew_Configurations_triggered()));

    // open
    open_config_ = new QAction(this);
    open_config_->setIcon(QPixmap(":/icons/toolbar/open"));
    open_config_->setToolTip(tr("Open EOS Configuration"));
    tb->addAction(open_config_);
    connect(open_config_, SIGNAL(triggered()), this, SLOT(on_actionOpen_triggered()));

    // save
    save_config_ = new QAction(this);
    save_config_->setIcon(QPixmap(":/icons/toolbar/save"));
    save_config_->setToolTip(tr("Save EOS Configuration"));
    tb->addAction(save_config_);
    connect(save_config_, SIGNAL(triggered()), this, SLOT(on_actionEnregistrer_triggered()));

    // save as
    saveas_config_ = new QAction(this);
    saveas_config_->setIcon(QPixmap(":/icons/toolbar/saveas"));
    saveas_config_->setToolTip(tr("Save EOS Configuration as"));
    tb->addAction(saveas_config_);
    connect(saveas_config_, SIGNAL(triggered()), this, SLOT(on_actionSave_As_triggered()));

    tb->addSeparator();

    settings_ = new QAction(this);
    settings_->setIcon(QPixmap(":/icons/toolbar/configurationf"));
    settings_->setToolTip(tr("Settings"));
    tb->addAction(settings_);
    connect(settings_, SIGNAL(triggered()), this, SLOT(showSettings()));

    //*! No need of demo feature in the delivered software
    //*! Only EOS INNOVATION can have this privilege
    //if (instance_.isAdmin() == true)
    //{
    // play a demo
    //tb->addSeparator();

    //}

    ui->mainToolBar->setStyleSheet("QToolBar"
                                   "{ "
                                   "background-position: right;"
                                   "background-repeat: no-repeat;}");
    ui->mainToolBar->setFixedHeight(50);
    ui->mainToolBar->setMovable(false);

    QAction *about = new QAction(this);
    about->setIcon(QPixmap(":/icons/toolbar/aboutf"));
    about->setToolTip(tr("About"));
    tb->addAction(about);
    connect(about, SIGNAL(triggered()), this, SLOT(about()));

    // toolbar robot !

    comboBox_ip_ = new QComboBox(this);
    comboBox_ip_->setEditable(true);
    comboBox_ip_->setMinimumWidth(150);
    ui->toolBar_robot->addWidget(comboBox_ip_);
    core::Memento memento = instance_.restoreMemento(constants::APP_NAME);
    for(auto const& ip : memento.ip)
    {
        comboBox_ip_->addItem(ip);
    }
    // Restore the last connected ip as current index
    comboBox_ip_->setCurrentIndex(comboBox_ip_->findText(memento.current_ip));

    connectRobot_ = new QAction(this);
    connectRobot_->setIcon(QPixmap(":/icons/toolbar/fail"));
    connectRobot_->setToolTip(tr("Connect Robot"));
    ui->toolBar_robot->addAction(connectRobot_);
    connect(connectRobot_, SIGNAL(triggered()), this, SLOT(connectRobot()));

    ui->toolBar_robot->addSeparator();

    pushAction_ = new QAction(ui->toolBar_robot);
    {
        //pushAction_->setEnabled(false);
        pushAction_->setIcon(QPixmap(":/icons/toolbar/uploadf"));
        pushAction_->setToolTip(tr("Upload to Robot"));
        connect(pushAction_, SIGNAL(triggered()), this, SLOT(pushEverything()));
        QMenu *menu = new QMenu(ui->toolBar_robot);
        menu->setStyleSheet("QMenu {"
                            "border: 1px solid rgb(60,60,60);"
                            "}"
                            "QMenu::item:pressed {"
                            "background: 2px solid #f07f10; "
                            "}");
        QAction *u = menu->addAction(tr("Map"));
        u->setIcon(QPixmap(":/icons/toolbar/uploadf"));
        connect(u, SIGNAL(triggered()), this, SLOT(pushMap()));
        QAction *b = menu->addAction(tr("Patrols"));
        b->setIcon(QPixmap(":/icons/toolbar/uploadf"));
        connect(b, SIGNAL(triggered()), this, SLOT(pushPatrols()));
        QAction *c = menu->addAction(tr("Areas"));
        c->setIcon(QPixmap(":/icons/toolbar/uploadf"));
        connect(c, SIGNAL(triggered()), this, SLOT(pushZones()));
        QAction *d = menu->addAction(tr("Tasks Schedulded"));
        d->setIcon(QPixmap(":/icons/toolbar/uploadf"));
        connect(d, SIGNAL(triggered()), this, SLOT(pushTasksScheduled()));
        QAction *e = menu->addAction(tr("Sensors"));
        e->setIcon(QPixmap(":/icons/toolbar/uploadf"));
        connect(e, SIGNAL(triggered()), this, SLOT(pushSensors()));
        QAction *f = menu->addAction(tr("Settings"));
        f->setIcon(QPixmap(":/icons/toolbar/uploadf"));
        connect(f, SIGNAL(triggered()), this, SLOT(pushSettings()));
        pushAction_->setMenu(menu);
        ui->toolBar_robot->addAction(pushAction_);
    }

    pullAction_ = new QAction(ui->toolBar_robot);
    {
        //pullAction_->setEnabled(false);
        pullAction_->setIcon(QPixmap(":/icons/toolbar/downloadf"));
        pullAction_->setToolTip(tr("Download from Robot"));
        connect(pullAction_, SIGNAL(triggered()), this, SLOT(pullEverything()));
        QMenu *menu = new QMenu(ui->toolBar_robot);
        menu->setStyleSheet("QMenu {"
                            "border: 1px solid rgb(60,60,60);"
                            "}"
                            "QMenu::item:pressed {"
                            "background: 2px solid #f07f10; "
                            "}");
        QAction *u = menu->addAction(tr("Map"));
        connect(u, SIGNAL(triggered()), this, SLOT(pullMap()));
        u->setIcon(QPixmap(":/icons/toolbar/downloadf"));
        QAction *b = menu->addAction(tr("Patrols"));
        connect(b, SIGNAL(triggered()), this, SLOT(pullPatrols()));
        b->setIcon(QPixmap(":/icons/toolbar/downloadf"));
        QAction *c = menu->addAction(tr("Areas"));
        connect(c, SIGNAL(triggered()), this, SLOT(pullZones()));
        c->setIcon(QPixmap(":/icons/toolbar/downloadf"));
        QAction *d = menu->addAction(tr("Tasks Scheduled"));
        connect(d, SIGNAL(triggered()), this, SLOT(pullTasksScheduled()));
        d->setIcon(QPixmap(":/icons/toolbar/downloadf"));
        QAction *e = menu->addAction(tr("Sensors"));
        connect(e, SIGNAL(triggered()), this, SLOT(pullSensors()));
        e->setIcon(QPixmap(":/icons/toolbar/downloadf"));
        QAction *f = menu->addAction(tr("Settings"));
        connect(f, SIGNAL(triggered()), this, SLOT(pullSettings()));
        f->setIcon(QPixmap(":/icons/toolbar/downloadf"));
        pullAction_->setMenu(menu);
        ui->toolBar_robot->addAction(pullAction_);
    }

    ui->toolBar_robot->addSeparator();

    createMap_ = new QAction(ui->toolBar_robot);
    {
        createMap_->setIcon(QPixmap(":/icons/toolbar/mapbuilder"));
        createMap_->setToolTip(tr("Start map creation wizard"));
        connect(createMap_, SIGNAL(triggered()), this, SLOT(startMapCreationWizard()));

        if ( instance_.getExpertMode() ) // Will be available only in EXPERT mode
            ui->toolBar_robot->addAction(createMap_);
    }
}

void MainWindow::setupStatusBar()
{
    statusLabel = new QLabel(this);
    information_status_ = new QLabel(this);
    filepath_ = new QLabel(this);

    if (instance_.isConnected())
        receive(core::RobotConnected());
    else
        receive(core::RobotDisconnected());

    ui->statusBar->addPermanentWidget(filepath_);
    ui->statusBar->addPermanentWidget(information_status_);
    ui->statusBar->addPermanentWidget(statusLabel);
}

void MainWindow::updateMap()
{
    emitEvent<core::MapUpdate>();
}

void MainWindow::receive(core::NewMapAvailable const &)
{
    newMapAvailable();
}

void MainWindow::updatePatrols()
{
    emitEvent<core::PatrolsUpdate>();
}

void MainWindow::updateSensors()
{
    emitEvent<core::SensorsUpdate>();
}

void MainWindow::updateZones()
{
    emitEvent<core::ZonesUpdate>();
}

void MainWindow::updateTasksScheduled()
{
    emitEvent<core::PlanificationUpdate>();
}

void MainWindow::closeEvent(QCloseEvent *closeEvent)
{
    if (mapWidget_->isEditing())
        mapWidget_->triggerEditMode();

    if ( saveCurrentConfigAndContinue() )
        closeEvent->accept();
    else
        closeEvent->ignore();
}

void MainWindow::about()
{
    DialogAbout about(this);
    about.exec();
}

void MainWindow::startMapCreationWizard()
{
    newMapWizard_->restart();
    newMapWizard_->show();
}

void MainWindow::abortCarto()
{
  robot_service_->abortCarto();
}

void MainWindow::createMap(bool create, float resolution, unsigned int size)
{
    if (robot_service_)
    {
        if (create)
        {
            map_resolution_ = resolution;
            map_size_ = size;
            information_status_->setText(tr("MapCreation::Recording laser data..."));
            robot_service_->startCarto(map_resolution_,map_size_);
        }
        else
        {
            information_status_->setText(tr("MapCreation::Waiting for the new map..."));
            robot_service_->stopCarto(map_resolution_,map_size_);
        }
    }
}

void MainWindow::newMapAvailable()
{
    information_status_->setText(tr("MapCreation::New map available. Download triggered!"));
    //core::alert("MapCreation::New map is now available. Download triggered!", this);

    if ( ( newMapWizard_->isVisible() ) && ( newMapWizard_->isWaitingForNewMap() ) )
    {
        newMapWizard_->setBusyMapRecoveryProgressBar(false); // Prepare to show progress
    }

    pullMap();
}

void MainWindow::showSettings()
{
    ConfigDialog settings(&instance_);
    settings.exec();
}

void MainWindow::activateZone(bool checked)
{
    mapWidget_->setMode(checked ? constants::CreateZone
                                : constants::Normal);
}

void MainWindow::activatePoint(bool checked)
{
    mapWidget_->setMode(checked ? constants::CreatePoint
                                : constants::Normal);
}
void MainWindow::activateSensors(bool checked)
{
    mapWidget_->setMode(checked ? constants::CreateSensors
                                : constants::Normal);
}

void MainWindow::activateWhitePencil(bool checked)
{
    mapWidget_->setMode(checked ? constants::AddDetails
                                : constants::Normal);

    if (checked)
        information_status_->setText(tr("[ Map edition tool : LEFT CLICK on map to add WHITE details ]"));
    else
        information_status_->setText(tr(""));
}

void MainWindow::activateBlackPencil(bool checked)
{
    mapWidget_->setMode(checked ? constants::AddDetails
                                : constants::Normal);

    if (checked)
        information_status_->setText(tr("[ Map edition tool : LEFT CLICK on map to add BLACK details ]"));
    else
        information_status_->setText(tr(""));
}

void MainWindow::activateGrayPencil(bool checked)
{
    mapWidget_->setMode(checked ? constants::AddDetails
                                : constants::Normal);

    if (checked)
        information_status_->setText(tr("[ Map edition tool : LEFT CLICK on map to add GRAY details ]"));
    else
        information_status_->setText(tr(""));
}

void MainWindow::activateRuler(bool checked)
{
    mapWidget_->setMode(checked ? constants::DistanceMeasurement
                                : constants::Normal);

    if (checked)
        information_status_->setText(tr("[ Map distance measurement tool : CLICK on map to define a starting point then move the mouse to take a measurement. CLICK on map again to end the process ]"));
    else
        information_status_->setText(tr(""));
}

void MainWindow::activateMapRotation(bool checked)
{
    mapWidget_->setMode(checked ? constants::RotateMap
                                : constants::Normal);

    if (checked)
        information_status_->setText(tr("[ Map rotation tool : CTRL+CLICK on map to define the rotation center ]"));
    else
        information_status_->setText(tr(""));
}

void MainWindow::on_actionA_propos_triggered()
{
    about();
}

void MainWindow::on_actionQuitter_triggered()
{
    close();
}

void MainWindow::pushPatrols()
{
    information_status_->setText(tr("Starting PATROLS upload.."));
    transfer_.upload(eos::Configuration::Model::Patrols);
}

void MainWindow::pushZones()
{
    information_status_->setText(tr("Starting ZONES upload.."));
    transfer_.upload(eos::Configuration::Model::Zones);
}

void MainWindow::pushTasksScheduled()
{
    information_status_->setText(tr("Starting TASKS upload.."));
    transfer_.upload(eos::Configuration::Model::TasksScheduled);
}

void MainWindow::pushMap()
{
    information_status_->setText(tr("Starting MAP upload.."));
    transfer_.upload(eos::Configuration::Model::BaseMap);
}

void MainWindow::pushSensors()
{
    information_status_->setText(tr("Starting SENSORS upload.."));
    transfer_.upload(eos::Configuration::Model::Sensors);
}

void MainWindow::pushSettings()
{
    information_status_->setText(tr("Starting SETTINGS upload.."));
    transfer_.upload(eos::Configuration::Model::Settings);
}

void MainWindow::pushEverything()
{
    pushPatrols();
    pushZones();
    pushTasksScheduled();
    pushSensors();
    pushSettings();
    pushMap();
}

void MainWindow::pullPatrols()
{
    information_status_->setText(tr("Starting PATROLS download.."));
    transfer_.download(eos::Configuration::Model::Patrols);
}

void MainWindow::pullZones()
{
    information_status_->setText(tr("Starting ZONES download.."));
    transfer_.download(eos::Configuration::Model::Zones);
}

void MainWindow::pullMap()
{
    information_status_->setText(tr("Starting MAP download.."));
    transfer_.download(eos::Configuration::Model::BaseMap);
}

void MainWindow::pullTasksScheduled()
{
    information_status_->setText(tr("Starting TASKS download.."));
    transfer_.download(eos::Configuration::Model::TasksScheduled);
}

void MainWindow::pullSensors()
{
    information_status_->setText(tr("Starting SENSORS download.."));
    transfer_.download(eos::Configuration::Model::Sensors);
}

void MainWindow::pullSettings()
{
    information_status_->setText(tr("Starting SETTINGS download.."));
    transfer_.download(eos::Configuration::Model::Settings);
}

void MainWindow::pullEverything()
{
    pullPatrols();
    pullZones();
    pullTasksScheduled();
    pullSensors();
    pullSettings();
    pullMap();
}

void MainWindow::downloading(int model, double progression)
{
    QString modelName = getModelName(model);
    int percent = 100 * progression;

    // If created new map : update new map creation wizard's map recovery progressbar
    if ( ( newMapWizard_->isVisible() ) && ( model == eos::Configuration::Model::BaseMap ) )
    {
        newMapWizard_->updateMapRecoveryProgressBarValue(percent);
    }

    information_status_->setText(tr("Downloading ") + modelName + QString(" %1%").arg(percent));
}

void MainWindow::uploading(int model, double progression)
{
    QString modelName = getModelName(model);
    int percent = 100 * progression;
    information_status_->setText(tr("Uploading ") + modelName + QString(" %1%").arg(percent));
}

void MainWindow::downloaded(int model, bool ok)
{
    QString modelName = getModelName(model);

    if(ok) // Download successful
    {
        if(model == eos::Configuration::Model::Patrols)
        {
            emitEvent<core::PatrolsUpdate>();
        }
        else if(model == eos::Configuration::Model::Sensors)
        {
            emitEvent<core::SensorsUpdate>();
        }
        else if(model == eos::Configuration::Model::BaseMap)
        {
            emitEvent<core::MapUpdate>();
        }
        else if(model == eos::Configuration::Model::TasksScheduled)
        {
            emitEvent<core::PlanificationUpdate>();
        }
        else if(model == eos::Configuration::Model::Zones)
        {
            emitEvent<core::ZonesUpdate>();
        }
        else if(model == eos::Configuration::Model::Settings)
        {
            emitEvent<core::ConfigurationUpdate>();
        }

        information_status_->setText(modelName + tr(" download successful !"));
    }
    else
    {
        information_status_->setText(modelName + tr(" download failed !"));
    }

    qDebug() << information_status_->text();
}

void MainWindow::uploaded(int model, bool ok)
{   
    QString modelName = getModelName(model);

    if(ok) // Upload successful
    {
        information_status_->setText(modelName + tr(" upload successful !"));
    }
    else
    {
        information_status_->setText(modelName + tr(" upload failed !"));
    }

    qDebug() << information_status_->text();
}

void MainWindow::receive(core::MapDownloading const &event)
{
    information_status_->setText(tr("Downloading %1 %").arg(int(event.percent * 100)));
}

void MainWindow::receive(core::RobotConnected const &)
{
    statusLabel->setText(tr("Connected"));
    pushAction_->setEnabled(true);
    pullAction_->setEnabled(true);
    createMap_->setEnabled(true);
    ui->actionReboot_the_robot->setEnabled(true);

    connectRobot_->setIcon(QPixmap(":/icons/toolbar/ok"));
    connectRobot_->setToolTip(tr("Disconnect Robot"));
    core::Memento memento = instance_.restoreMemento(constants::APP_NAME);
    QString ip = comboBox_ip_->currentText();
    if(memento.ip.contains(ip) == false)
      memento.ip.push_back(ip);
    memento.current_ip = ip; // Save current ip to be restored on next application restart as last connected ip
    instance_.saveMemento(memento, constants::APP_NAME);
    is_trying_connection_ = false;

    comboBox_ip_->setEnabled(false);
}

void MainWindow::receive(core::RobotDisconnected const &)
{
    connectRobot_->setIcon(QPixmap(":/icons/toolbar/fail"));
    connectRobot_->setToolTip(tr("Connect Robot"));
    is_trying_connection_ = false;
    statusLabel->setText(tr("Disconnected"));
    information_status_->setText(tr(""));
    pushAction_->setEnabled(false);
    pullAction_->setEnabled(false);
    createMap_->setEnabled(false);
    ui->actionReboot_the_robot->setEnabled(false);

    comboBox_ip_->setEnabled(!bEditModeFlag);
}

void MainWindow::receive(core::RobotStandby const &)
{
    connectRobot_->setIcon(QPixmap(":/icons/toolbar/try_connect"));
    connectRobot_->setToolTip(tr("Cancel Robot connection"));
    is_trying_connection_ = true;
    statusLabel->setText(tr("Disconnected"));
    information_status_->setText(tr(""));
    pushAction_->setEnabled(false);
    pullAction_->setEnabled(false);
    createMap_->setEnabled(false);
    ui->actionReboot_the_robot->setEnabled(false);

    comboBox_ip_->setEnabled(false);
}

void MainWindow::displayCalendar(bool )
{
    calendar_widget_->loadTasksScheduled();
    calendar_widget_->setVisible(!calendar_widget_->isVisible());
    calendar_widget_->setMaximumWidth(mapWidget_->width());
}

void MainWindow::on_actionImport_Map_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(0, tr("open_file"),
                                                    QDir::homePath(),
                                                    tr("images (*.png *.xpm *.jpg *.jpeg);; all (*)"));
    if (fileName != NULL)
    {
        QImage nmap(fileName);
        //instance_.map()->updateMap(nmap);
        //mapWidget_->setMap(instance_.map());
        //mapWidget_->update();
    }
}

void MainWindow::on_actionMapEdit_triggered(bool activated)
{
    if ( activated )
    {
        new_config_->setEnabled(false);
        open_config_->setEnabled(false);
        save_config_->setEnabled(false);
        saveas_config_->setEnabled(false);
        settings_->setEnabled(false);

        pushAction_->setVisible(false);
        pullAction_->setVisible(false);
        createMap_->setVisible(false);
        comboBox_ip_->setEnabled(false);
        connectRobot_->setEnabled(false);

        ui->menuBar->setEnabled(false);
        bEditModeFlag = true;
    }
    else
    {
        new_config_->setEnabled(true);
        open_config_->setEnabled(true);
        save_config_->setEnabled(true);
        saveas_config_->setEnabled(true);
        settings_->setEnabled(true);

        pushAction_->setVisible(true);
        pullAction_->setVisible(true);
        createMap_->setVisible(true);
        comboBox_ip_->setEnabled(!(instance_.read_only().isConnected() || is_trying_connection_));
        connectRobot_->setEnabled(true);

        ui->menuBar->setEnabled(true);
        bEditModeFlag = false;
    }
}

void MainWindow::on_actionHelp_triggered()
{
    if (tr("lang") == "fr")
    {
        QDesktopServices::openUrl(QUrl("manual_fr.pdf"));
        return;
    }
    QDesktopServices::openUrl(QUrl("manual_en.pdf"));
}

void MainWindow::connectRobot()
{
  if(is_trying_connection_)
  {
      emitEvent<core::DisconnectRobot>();
      connectRobot_->setIcon(QPixmap(":/icons/toolbar/fail"));
      connectRobot_->setToolTip(tr("Connect Robot"));
      is_trying_connection_ = false;
      comboBox_ip_->setEnabled(true);
      return ;
  }
  if(instance_.isConnected())
  {
    emitEvent<core::DisconnectRobot>();
    connectRobot_->setIcon(QPixmap(":/icons/toolbar/fail"));
    connectRobot_->setToolTip(tr("Connect Robot"));
    is_trying_connection_ = false;
    comboBox_ip_->setEnabled(true);
    return ;
  }
  QString ip = comboBox_ip_->currentText();
  if(core::isValidIP(ip) == false)
  {
    emitEvent<core::DisconnectRobot>();
    core::alert(QString("This is IP Address is not valid : (%1) !").arg(ip));
    is_trying_connection_ = false;
    connectRobot_->setIcon(QPixmap(":/icons/toolbar/fail"));
    connectRobot_->setToolTip(tr("Connect Robot"));
    comboBox_ip_->setEnabled(true);
    return ;
  }
  connectRobot_->setIcon(QIcon(":/icons/toolbar/try_connect"));
  connectRobot_->setToolTip(tr("Cancel Robot connection"));
  is_trying_connection_ = true;
  instance_.setRobotIP(ip);
  emitEvent<core::IPChanged>();
  comboBox_ip_->setEnabled(false);
  transfer_.hostChanged(ip);
}

bool MainWindow::saveCurrentConfigAndContinue()
{
    // Save current config if modified
    if ( instance_.configuration().isModified() )
    {
        QMessageBox saveMsgBox;
        saveMsgBox.setText(tr("The current configuration has been modified!"));
        saveMsgBox.setInformativeText(tr("Do you want to save this configuration?"));
        saveMsgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        saveMsgBox.setDefaultButton(QMessageBox::Save);

        int iSave = saveMsgBox.exec();

        switch (iSave)
        {
          case QMessageBox::Save:
              on_actionEnregistrer_triggered();

              if ( !instance_.configuration().isModified() )
                  break;
              else
                  return false;
              break;
          case QMessageBox::Discard:
              // do nothing
              break;
          case QMessageBox::Cancel:
              return false;
              break;
          default:
              // should never be reached
              break;
        }
    }

    return true;
}

QString MainWindow::getModelName(int iModel)
{
    if(iModel == eos::Configuration::Model::Patrols)
    {
        return tr("PATROLS");
    }
    else if(iModel == eos::Configuration::Model::Sensors)
    {
        return tr("SENSORS");
    }
    else if(iModel == eos::Configuration::Model::BaseMap)
    {
        return tr("MAP");
    }
    else if(iModel == eos::Configuration::Model::TasksScheduled)
    {
        return tr("TASKS");
    }
    else if(iModel == eos::Configuration::Model::Zones)
    {
        return tr("ZONES");
    }
    else if(iModel == eos::Configuration::Model::Settings)
    {
        return tr("SETTINGS");
    }
    else
        return "404 Model Not Found!";
}

// new configuration !
void MainWindow::on_actionNew_Configurations_triggered()
{
    // Save current config if modified
    if ( !saveCurrentConfigAndContinue() )
        return;

    instance_.configuration().clear();
    eos::MapSharedPointer map = instance_.configuration().base_map();

    // create a default map !
    static constexpr int width = 1000;
    static constexpr int height = 1000;
    map->set_timestamp(0);
    map->set_width(width);
    map->set_height(height);
    map->set_resolution(0.03);

    QImage image(width, height, QImage::Format_RGB32);
    image.fill(Qt::white);
    QByteArray bytes;
    QBuffer buffer( &bytes );
    buffer.open( QIODevice::WriteOnly );
    image.save( &buffer, "PNG" );

    map->mutable_content()->resize(buffer.size());
    std::string *content = map->mutable_content();
    char *p = &(*content)[0];
    memcpy(p, buffer.data(), buffer.size());

    //map->mutable_content()->resize(width * height * 3);
    //std::fill(map->mutable_content()->begin(), map->mutable_content()->end(), 255);
    // normally set in setmap method !
    //constants::map::width() = map->width();
    //constants::map::height() = map->height();

    instance_.configuration().settings()->set_timestamp(0);
    filepath_->setText("");
    information_status_->setText("");
    // empty calendar
    emitEvent<core::MapUpdate>();
    emitEvent<core::PatrolsUpdate>();
    emitEvent<core::ZonesUpdate>();
    emitEvent<core::SensorsUpdate>();
    emitEvent<core::PlanificationUpdate>();

    // Reset modification flag for new config
    instance_.configuration().reset_modification_status();
}

void MainWindow::on_actionSettings_triggered()
{
    showSettings();
}

void MainWindow::on_actionReboot_the_robot_triggered()
{
    if (robot_service_)
    {
        if ( robot_service_->sendRobotReboot() )
            qDebug() << "REBOOT MESSAGE SENT SUCCESSFULLY !!";
        else
            qDebug() << "REBOOT MESSAGE NOT SENT !!";
    }
    else
        qDebug() << "REBOOT MESSAGE : NO ROBOT SERVICE !!";
}

// save configuration !
void MainWindow::on_actionEnregistrer_triggered()
{
  if(instance_.configuration().isOpened())
  {
     instance_.configuration().save();
  }
  else
  {
    on_actionSave_As_triggered();
  }
}

// save as !
void MainWindow::on_actionSave_As_triggered()
{
  QString configuration_eos_filepath = QFileDialog::getSaveFileName(this,
                                                         tr("Save configuration file"),
                                                         "",
                                                         "EOS Configuration (*.eos)");
  if(configuration_eos_filepath.isEmpty() == false)
  {
    QFileInfo fi(configuration_eos_filepath);
    QString ext = fi.suffix();
    if(ext != "eos")
        configuration_eos_filepath += ".eos";
    filepath_->setText(configuration_eos_filepath);
    instance_.configuration().save(configuration_eos_filepath.toStdString());
  }
}

void MainWindow::on_actionOpen_triggered()
{
    // Save current config if modified
    if ( !saveCurrentConfigAndContinue() )
        return;

  QString configuration_eos_filepath = QFileDialog::getOpenFileName(this,
                                                         tr("Open configuration file"),
                                                         "",
                                                         "EOS Configuration (*.eos)");
  if(configuration_eos_filepath.isEmpty() == false)
  {
    instance_.configuration().open(configuration_eos_filepath.toStdString());
    filepath_->setText(configuration_eos_filepath);
    emitEvent<core::MapUpdate>();
    emitEvent<core::PlanificationUpdate>();

    instance_.configuration().reset_modification_status();
  }
}
