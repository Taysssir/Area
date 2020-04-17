#include <QApplication>
#include <QTranslator>
#include <QMessageBox>
#include <iostream>
#include <QTime>
#include <QFontDatabase>

#include "mainwindow.h"
#include "ui/dialoglogin.h"
#include "ui/admin/patroladminwidget.h"
#include "ui/config/dialogconfiguration.h"
#include "tools/patrolcolor.h"
#include "remote/robot.h"
#include "core/sentinel_logger.h"

#include "sentinel-common/sftp/sftp.h"

int main(int argc, char *argv[])
{
  qsrand((uint)QTime::currentTime().msec());
  QApplication app(argc, argv);

  //eos::sftp::QSftp test;
  //test.setParameters("172.16.25.10", "e-vigilante", "RSd49gm9");
  //test.connect();
  //test.download("/home/e-vigilante/.config/eos/shared/base_map", "/home/jerome/test");
  //return 0;
  core::Logger::instance()->open("sentinel-admin-bootstrap.logs");

  // contains models
  core::AppInstance instance;
  bool is_config_file_readable = false;
  is_config_file_readable = instance.read_config_settings();

  EOS_Info() << "use translator !";
  QTranslator *translator = new QTranslator();
  QLocale::Language locale_language;
  if(is_config_file_readable)
      instance.get_language(locale_language);
  else
      instance.setLocaleLanguage(QLocale::English);
  if(locale_language == QLocale::French)
      translator->load(":/translations/sentinel-admin_fr");
  else if(locale_language == QLocale::German)
      translator->load(":/translations/sentinel-admin_de");
  else
      translator->load(":/translations/sentinel-admin_en");

  EOS_Info() << QString("Translator is loaded : %1").arg(app.installTranslator(translator)).toStdString();

  QFontDatabase::addApplicationFont(":/font");

  // setup app mode (expert/normal)
  if(is_config_file_readable)
      instance.setExpertMode();
  std::string appMode = instance.getExpertMode()?"EXPERT":"NORMAL";
  EOS_Info() << "Running in "+appMode+" mode.";

  int currentExitCode = 0;
  //Robot robot(instance);
  EOS_Info() << "sentinel-admin started !";
  do
  {
    // connexion
    //DialogLogin login(instance);
    // login.addLanguage(QObject::tr("english"), "sentinel-admin_en");
    // login.addLanguage(QObject::tr("french"), "sentinel-admin_fr");
    //if (login.exec() == 0) break;

    instance.launchService<Robot>("Robot", true);
    // if (login.languageSelected() != language)
    // {
    //     app.removeTranslator(translator);
    //     if (translator->load(memento.language))
    //     {
    //         app.installTranslator(translator);
    //     }
    // }
   // instance.loadConfiguration(instance.robotIP());

    //instance.emitEvent<core::IPChanged>();

    //core::Workspace::setConfigurationName(instance.configuration());

    //core::Logger::instance()->open("sentinel-admin.logs");

    //// try to open the selected configuration
    //if (instance.openConfiguration(instance.configuration()) == false)
    //{
    //    QMessageBox::critical(0,
    //                          QObject::tr("error_opening"),
    //                          QObject::tr("error_file_access"));
    //    // error, just loop
    //    continue;
    //}

    //if (!instance.isAdmin())
    //{
    //    QMessageBox::critical(0,
    //                          QObject::tr("authentification_error"),
    //                          QObject::tr("only_adminsitrator_can_use_this_application"));
    //    // error, just loop
    //    continue;
    //}
    // show the main window
    MainWindow w(instance);
    w.showMaximized();
    //w.show();

    // run the app
    currentExitCode = app.exec();

    // reset resources (?)
    //rm::ResourceManager::instance()->reset();

  } while (currentExitCode == MainWindow::EXIT_CODE_REBOOT);

  return currentExitCode;
}
