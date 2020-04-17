#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "restapiconfiguration.h"
#include "ui_restapiconfiguration.h"

namespace rest_api_configuration
{
namespace tag
{
constexpr char REST_API_CONFIGURATION[] = "remote_rest";
constexpr char WHITE_LIST[] = "white_list";
}
}

RestApiConfiguration::RestApiConfiguration(core::AppInstance &instance, QWidget *parent) :
    QWidget(parent),
    core::EventClient<RestApiConfiguration>(instance),
    ui(new Ui::restapiconfiguration)
{
    ui->setupUi(this);

    connect(ui->ipWhitelist, SIGNAL(itemSelectionChanged()), this, SLOT(slot_ipSelectionChanged()));
    connect(ui->ipLineEdit, SIGNAL(textChanged(QString)), this, SLOT(slot_ipTextChanged(QString)));
    connect(ui->ip_add_pushButton, SIGNAL(clicked()), this, SLOT(slot_addIP()));
    connect(ui->ip_remove_pushButton, SIGNAL(clicked()), this, SLOT(slot_removeIP()));
    connect(ui->buttonBoxRestApi, SIGNAL(clicked(QAbstractButton*)), this, SLOT(slot_onDlgBtnBxClicked(QAbstractButton*)));

    loadIpWhitelist();
}

RestApiConfiguration::~RestApiConfiguration()
{
    delete ui;
}

void RestApiConfiguration::slot_addIP()
{
    QStringList ipv4 = ui->ipLineEdit->text().split(QLatin1String("."));
    QString ip = ""; // This will be the clean ip without leading zeros

    for (int i = 0; i < 4; ++i)
    {
        ip += QString::number(ipv4.at(i).toInt());
        if(i!=3)
            ip += '.';
    }

    if ( ui->ipWhitelist->findItems( ip, Qt::MatchExactly ).size() )
    {
        QMessageBox::warning(this, tr("Add IP to whitelist"), ip + tr(" : This IP address already exists!"));
        return;
    }

    ui->ipWhitelist->addItem( ip );
    ui->ipLineEdit->clear();
}

void RestApiConfiguration::slot_removeIP()
{
    QListWidgetItem *itemToRemove = ui->ipWhitelist->takeItem( ui->ipWhitelist->currentRow() );

    if ( itemToRemove != 0)
    {
        delete itemToRemove;
        ui->ipWhitelist->clearSelection();
    }
}

void RestApiConfiguration::slot_ipSelectionChanged()
{
    ui->ip_remove_pushButton->setEnabled( ui->ipWhitelist->selectedItems().size() );
}

void RestApiConfiguration::slot_ipTextChanged(QString ip)
{
    ui->ip_add_pushButton->setEnabled( validateIp4(ip) );
}

void RestApiConfiguration::slot_onDlgBtnBxClicked(QAbstractButton *button)
{
    if ( button == ui->buttonBoxRestApi->button(QDialogButtonBox::Save) )
    {
        saveIpWhitelist();
    }
    else if ( button == ui->buttonBoxRestApi->button(QDialogButtonBox::Reset) )
    {
        loadIpWhitelist();
    }
}

bool RestApiConfiguration::validateIp4(const QString& address)
{
    QStringList ipv4 = address.split(QLatin1String("."));

    if (ipv4.count() != 4)
        return false;

    for (int i = 0; i < 4; ++i)
    {
        bool ok = false;
        uint byteValue = ipv4.at(i).toUInt(&ok);
        if (!ok || byteValue > 255)
            return false;
    }

    return true;
}

namespace
{

  bool readDoc(eos::SettingsSharedPointer const& settings, QJsonDocument & doc)
  {
    for(auto const& setting : settings->settings())
    {
        if(setting.keys() == rest_api_configuration::tag::REST_API_CONFIGURATION)
        {
            doc = QJsonDocument::fromJson(QString::fromStdString(setting.value()).toUtf8());
            return true;
        }
    }
    return false;
  }

}
void RestApiConfiguration::loadIpWhitelist()
{
    ui->ipWhitelist->clear();
    QJsonDocument doc;
    if(readDoc(instance_.read_only().configuration().settings(), doc))
    {
        QJsonObject settings = doc.object();
        QString white_list = settings.value(rest_api_configuration::tag::WHITE_LIST).toString();
        if(white_list.size())
           ui->ipWhitelist->addItems( white_list.split(",") );
    }
}

void RestApiConfiguration::saveIpWhitelist()
{
    //Prepare the ip white list to a string in order to replace it in the JSON Settings List
    QStringList ipWhiteList;
    for(int i = 0; i < ui->ipWhitelist->count(); ++i)
    {
        ipWhiteList << ui->ipWhitelist->item(i)->text();
    }
    QString white_list = ipWhiteList.join(',');

    QJsonDocument doc; // will contains ALL the settings
    readDoc(instance_.read_only().configuration().settings(), doc);
    int remote_rest_settings_exist = false;
    for (int i(0); i<instance_.configuration().settings()->settings_size();++i)
    {
        //search after the rest settings and not the sia one for example !!
        if(instance_.configuration().settings()->settings(i).keys() == rest_api_configuration::tag::REST_API_CONFIGURATION)
        {
            remote_rest_settings_exist = true;
            // Stock the ALL the settings on object to manipulate it.
            QJsonObject settings = doc.object();
            // Find the last white_list and remove it and insert the new one. (this lines just touch at the white_list we keep all the others settings)
            if (settings.keys().contains(rest_api_configuration::tag::WHITE_LIST))
                settings.remove(rest_api_configuration::tag::WHITE_LIST);
            settings.insert(rest_api_configuration::tag::WHITE_LIST,white_list);
            // replace the whole settings by the new one.
            QJsonDocument doc_to_set;
            doc_to_set.setObject(settings);
            instance_.configuration().settings()->mutable_settings(i)->set_value(doc_to_set.toJson(QJsonDocument::Compact));
        }
    }
    if(remote_rest_settings_exist == false) //if remote_rest settings not found
    {
        //create json object with {white_list:ip,ip,...}
        QJsonObject remote_rest_configuration;
        remote_rest_configuration[rest_api_configuration::tag::WHITE_LIST] = white_list;
        // create a doc with the json settings
        QJsonDocument doc(remote_rest_configuration);
        QString string_settings = doc.toJson(QJsonDocument::Compact);

        ::eos::JSONSettings *JSON = instance_.configuration().settings()->add_settings();
        JSON->set_keys(rest_api_configuration::tag::REST_API_CONFIGURATION);
        JSON->set_value(doc.toJson(QJsonDocument::Compact));
    }
    instance_.configuration().save(eos::Configuration::Settings);
}

