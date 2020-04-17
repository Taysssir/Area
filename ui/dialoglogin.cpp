#include <QDir>
#include <QRegExp>
#include <QDebug>
#include <QFile>
#include <QMessageBox>

#include "dialoglogin.h"
#include "ui_dialoglogin.h"
#include "core/workspace.h"
#include "core/appinstance.h"
#include "core/constants.h"
#include "sentinel-admin_core/constants.h"

DialogLogin::DialogLogin(core::AppInstance &instance, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogLogin)
    , instance_(instance)
{
  ui->setupUi(this);

  // were locations are stored
  QString configurationsLocations = core::Workspace::local(core::Workspace::SentinelAdminConfigurations);
  // list all concerned files
  QDir dir(configurationsLocations);
 /* QStringList listConfigurations = dir.entryList(QDir::Files);
  ui->comboBox_configuration->addItem("localhost");
  ui->comboBox_configuration->setCurrentText("localhost");
  for (auto const &file : listConfigurations)
    ui->comboBox_configuration->addItem(file);*/
  //{
  //  if (file.contains(".eos"))
  //  {
  //    ui->comboBox_configuration_source->addItem(file);
  //  }
  //}

  core::Memento memento = instance.restoreMemento(constants::APP_NAME);
  ui->lineEdit_login->setText(memento.login);
  initialPassword_ = memento.password;
  ui->lineEdit_password->setText(memento.password);
  //ui->comboBox_configuration->setCurrentText(memento.configuration);
  if (memento.login.size())
    ui->checkBox_remember->setChecked(true);
  else
    ui->pushButton_connect->setFocus();

  QPixmap mypix = QPixmap(":/images/background");
  ui->label_background->setPixmap(mypix);
  ui->label_background->setFixedSize(640, 364);
  this->setFixedSize(640, 364);
  ui->label_Login->setStyleSheet("color: white;");
  ui->label_password->setStyleSheet("color: black;");
 // ui->label_configuration->setStyleSheet("color: white;");
  ui->label_version->setStyleSheet("color: rgb(240,127,16);");
 // ui->checkBox_duplicate->setStyleSheet("color: black;");
  ui->pushButton_connect->setDefault(true);

  const QString description = tr("v%1.%2")
                                  .arg(QString::number(constants::MAJOR),
                                       QString::number(constants::MINOR));
  ui->label_version->setFont(QFont("Arial", 20, QFont::Bold));
  ui->label_version->setText(description);

  setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint);
}

DialogLogin::~DialogLogin()
{
  delete ui;
}

void DialogLogin::on_pushButton_connect_clicked()
{
  core::Memento memento;
 // memento.configuration = ui->comboBox_configuration->currentText();
  if (QFileInfo(memento.configuration).suffix() != "eos")
  {
    memento.configuration += ".eos";
  }

  //// JBE : Remove for test
  ////if (instance_.users()->has(memento.login, memento.password) == false)
  ////{
  ////    QMessageBox dlg(QMessageBox::Critical, tr("connexion_error"), tr("user_password_mismatch"));
  ////    dlg.setWindowFlags(dlg.windowFlags() | Qt::WindowStaysOnTopHint);
  ////    dlg.exec();
  ////    return;
  ////}

  instance_.setMemento(memento);
  instance_.saveMemento(memento, constants::APP_NAME);

  QString current_ip = memento.configuration;
  if(QFileInfo(memento.configuration).suffix() == "eos")
      current_ip = current_ip.left(current_ip.size()-4);
  instance_.setRobotIP(current_ip);
  //    core::Memento memento;
  //
  //    memento.login = ui->lineEdit_login->text();
  //    if(initialPassword_ != ui->lineEdit_password->text())
  //        memento.password = User::hash(ui->lineEdit_password->text());
  //    else
  //        memento.password = ui->lineEdit_password->text();
  //    memento.configuration = ui->comboBox_configuration->currentText();
  //
  //    if(languages_.contains(ui->comboBox_language->currentText()))
  //        memento.language = languages_[ui->comboBox_language->currentText()];
  //
  //    QString configurationSource = ui->comboBox_configuration_source->currentText();
  //
  //
  //    if(!memento.configuration.size())
  //    {
  //        QMessageBox::critical(0,
  //                              tr("connexion_error"),
  //                              tr("enter_configuration_name"));
  //        return ;
  //    }
  //
  //    if(QFileInfo(memento.configuration).suffix() != "eos")
  //    {
  //        memento.configuration += ".eos";
  //    }
  //
  //    ///if(instance_.users()->has(memento.login, memento.password) == false)
  //    ///{
  //    ///    QMessageBox dlg(QMessageBox::Critical, tr("connexion_error"), tr("user_password_incorrect"));
  //    ///    dlg.setWindowFlags(dlg.windowFlags() | Qt::WindowStaysOnTopHint);
  //    ///    dlg.exec();
  //    ///    return ;
  //    ///}
  //
  //    if(ui->checkBox_duplicate->isChecked())
  //    {
  //        if(configurationSource == memento.configuration)
  //        {
  //            QMessageBox::critical(0,
  //                                  tr("connexion_error"),
  //                                  tr("configuration_source_same_configuration_destination"));
  //            return ;
  //        }
  //        if(!configurationSource.size())
  //        {
  //            QMessageBox::critical(0,
  //                                  tr("connexion_error"),
  //                                  tr("no_configuration_source"));
  //            return ;
  //        }
  //        using namespace core;
  //        QFile::remove(
  //                    join(Workspace::local(Workspace::Configurations), memento.configuration)
  //                    );
  //        QFile::copy(
  //                    join(Workspace::local(Workspace::Configurations), configurationSource),
  //                    join(Workspace::local(Workspace::Configurations), memento.configuration)
  //                    );
  //
  //    }
  //
  //    instance_.setMemento( memento );
  //
  //    if(!ui->checkBox_remember->isChecked())
  //        instance_.saveMemento( core::Memento(), constants::APP_NAME );
  //    else
  //        instance_.saveMemento( memento, constants::APP_NAME );
  //
  //    instance_.login( memento.login );
  //
  accept();
}

void DialogLogin::on_pushButton_cancel_clicked()
{
  reject();
}

namespace
{
  QString clean(QString const &arg)
  {
    static QString ignore = "\\/@{}=+-*!:;,?%$&~°œ“[]()£“œ/-_#^\'\"";
    QString temp = arg;
    for (auto c : ignore)
      temp.replace(QString(c), "");
    return temp;
  }
}

void DialogLogin::on_lineEdit_login_textChanged(const QString &arg1)
{
  ui->lineEdit_login->setText(clean(arg1));
}

void DialogLogin::on_lineEdit_password_textChanged(const QString &arg1)
{
  ui->lineEdit_password->setText(clean(arg1));
}

