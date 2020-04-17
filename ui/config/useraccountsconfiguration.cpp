#include "useraccountsconfiguration.h"
#include "ui_useraccountsconfiguration.h"

#define PASSWORD_ROLE 69

namespace user_accounts_configuration
{
namespace tag
{
constexpr char USER_ACCOUNTS[] = "user_accounts";
constexpr char USERNAME[] = "USERNAME";
constexpr char PASSWORD[] = "PASSWORD";
}
}

UserAccountsConfiguration::UserAccountsConfiguration(core::AppInstance &instance, QWidget *parent) :
    QWidget(parent),
    core::EventClient<UserAccountsConfiguration>(instance),
    ui(new Ui::useraccountsconfiguration)
{
    ui->setupUi(this);

    connect(ui->userName_lineEdit,SIGNAL(textChanged(QString)), this, SLOT(slot_unameOrPassTextChanged(QString)));
    connect(ui->password_lineEdit,SIGNAL(textChanged(QString)), this, SLOT(slot_unameOrPassTextChanged(QString)));
    connect(ui->userAccountsList, SIGNAL(itemSelectionChanged()), this, SLOT(slot_userSelectionChanged()));
    connect(ui->user_add_pushButton, SIGNAL(clicked()), this, SLOT(slot_addUser()));
    connect(ui->user_remove_pushButton, SIGNAL(clicked()), this, SLOT(slot_removeUser()));
    connect(ui->buttonBoxUserAccounts, SIGNAL(clicked(QAbstractButton*)), this, SLOT(slot_onDlgBtnBxClicked(QAbstractButton*)));

    loadUserAccounts();
}

UserAccountsConfiguration::~UserAccountsConfiguration()
{
    delete ui;
}

void UserAccountsConfiguration::loadUserAccounts()
{
    ui->userAccountsList->clear();

    eos::SettingsSharedPointer settings = instance_.read_only().configuration().settings();
    for(auto setting : settings->settings())
    {
        if(setting.keys() == user_accounts_configuration::tag::USER_ACCOUNTS)
        {
            QJsonDocument jAccountsDoc =  QJsonDocument::fromJson(QString::fromStdString(setting.value()).toUtf8());
            QJsonArray jAccountsArray = jAccountsDoc.array();

            for (int i=0; i<jAccountsArray.size(); ++i)
            {
                QJsonObject jAccountObj = jAccountsArray.at(i).toObject();
                QString uname = jAccountObj.value(user_accounts_configuration::tag::USERNAME).toString();
                QString pass = jAccountObj.value(user_accounts_configuration::tag::PASSWORD).toString();
                QListWidgetItem* userItem =  new QListWidgetItem( uname );
                userItem->setData( PASSWORD_ROLE, pass );
                ui->userAccountsList->addItem( userItem );
            }
        }
    }
}

void UserAccountsConfiguration::saveUserAccounts()
{
    QJsonDocument jAccountsDoc;
    QJsonArray jAccountsArray;

    for(int i = 0; i < ui->userAccountsList->count(); ++i)
    {
        QListWidgetItem* userItem =  ui->userAccountsList->item(i);

        QJsonObject jAccountObj;
        jAccountObj.insert( user_accounts_configuration::tag::USERNAME, userItem->text() );
        jAccountObj.insert( user_accounts_configuration::tag::PASSWORD, userItem->data(PASSWORD_ROLE).toString() );
        jAccountsArray.append(jAccountObj);
    }

    jAccountsDoc.setArray( jAccountsArray );

    bool found_settings = false;
    eos::SettingsSharedPointer settings = instance_.configuration().settings();
    for(int i = 0; i < settings->settings().size() ; ++i)
    {
        if(settings->settings(i).keys() == user_accounts_configuration::tag::USER_ACCOUNTS)
        {
            settings->mutable_settings(i)->set_value(jAccountsDoc.toJson(QJsonDocument::Compact).toStdString());
            found_settings = true;
        }
    }
    if(!found_settings)
    {
        ::eos::JSONSettings *JSON = settings->add_settings();
        JSON->set_keys(user_accounts_configuration::tag::USER_ACCOUNTS);
        JSON->set_value(jAccountsDoc.toJson(QJsonDocument::Compact).toStdString());
    }
    instance_.configuration().save(eos::Configuration::Settings);
}

void UserAccountsConfiguration::slot_addUser()
{
    QString userName = ui->userName_lineEdit->text().simplified();
    // Test if user already exists
    if ( ui->userAccountsList->findItems( userName, Qt::MatchFixedString ).size() )
    {
        QMessageBox::warning(this, tr("Add new user"), userName + tr(" : This user already exists!"));
        return;
    }
    // Create the new user item
    QListWidgetItem *newUserItem = new QListWidgetItem( userName );
    // Set the user password as the new item's data field for role PASSWORD_ROLE
    newUserItem->setData( PASSWORD_ROLE, ui->password_lineEdit->text() );
    // Add the new item to the list
    ui->userAccountsList->addItem( newUserItem );
    // Clear input fields
    ui->userName_lineEdit->clear();
    ui->password_lineEdit->clear();
}

void UserAccountsConfiguration::slot_removeUser()
{
    QListWidgetItem *itemToRemove = ui->userAccountsList->takeItem( ui->userAccountsList->currentRow() );

    if ( itemToRemove != 0)
    {
        delete itemToRemove;
        ui->userAccountsList->clearSelection();
    }
}

void UserAccountsConfiguration::slot_unameOrPassTextChanged(QString)
{
    ui->user_add_pushButton->setEnabled( !ui->userName_lineEdit->text().isEmpty() && !ui->password_lineEdit->text().isEmpty() );
}

void UserAccountsConfiguration::slot_userSelectionChanged()
{
    ui->user_remove_pushButton->setEnabled( ui->userAccountsList->selectedItems().size() );
}

void UserAccountsConfiguration::slot_onDlgBtnBxClicked(QAbstractButton *button)
{
    if ( button == ui->buttonBoxUserAccounts->button(QDialogButtonBox::Save) )
    {
        saveUserAccounts();
    }
    else if ( button == ui->buttonBoxUserAccounts->button(QDialogButtonBox::Reset) )
    {
        loadUserAccounts();
    }
}
