#include "dialognewsensor.h"
#include "ui_dialognewsensor.h"
#include "evigilante_configuration/include/eos/configuration/models.pb.h"
#include <QDebug>

DialogNewSensor::DialogNewSensor(QString name, int type, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogNewSensor)
  , mName(name)
  , mType(type)
{
    ui->setupUi(this);
    //for (QString s: Sensor::getAvailableTypes())
    ui->SensorTypeCbb->addItems(QStringList() << tr("Base")<<tr("Door")<<tr("Radar")<<tr("Infrarouge")<<tr("Rack"));
    ui->sensor_name_input->setText(name);
    ui->SensorTypeCbb->setCurrentIndex(type);
    connect(ui->SensorTypeCbb, SIGNAL(currentIndexChanged(int)),this, SLOT(updateNameFromType()));
    connect(ui->sensor_name_input, SIGNAL(textChanged(QString)), this,SLOT(updateName()));
    setWindowFlags( Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint );
}

DialogNewSensor::~DialogNewSensor()
{
    delete ui;
}
void DialogNewSensor::updateNameFromType()
{
   int currentIdxType = ui->SensorTypeCbb->currentIndex();
   QString name = tr("door");
   switch (currentIdxType)
   {
      case eos::Sensor::RobotDock:
          name = tr("base");
          break;
      case eos::Sensor::DoorSwitch:
          name = tr("door");
          break;
      case eos::Sensor::Radar:
          name = tr("radar");
          break;
      case eos::Sensor::InfraredBarrier:
          name = tr("infrared");
          break;
      case eos::Sensor::Rack:
          name = tr("rack");
          break;
   }
   mType = currentIdxType;
   ui->sensor_name_input->setText(name);
}

void DialogNewSensor::updateName()
{
    mName = ui->sensor_name_input->text();
}
int DialogNewSensor::getMType() const
{
    return mType;
}

void DialogNewSensor::setMType(int value)
{
    mType = value;
}

QString DialogNewSensor::getMName() const
{
    return mName;
}

void DialogNewSensor::setMName(const QString &value)
{
    mName = value;
}


