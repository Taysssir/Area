#include <QCryptographicHash>

#include "user.h"
#include "core/sentinel_logger.h"
#include "core/workspace.h"

User Users::None;

namespace
{
constexpr char DEFAULT_SETTINGS[] = "default_settings";
}

void User::Layout::defineDefault()
{
  EOS_Info() << "Define default layout config" << std::endl;
  QString fileName = core::join(core::Workspace::local(core::Workspace::App),
                                DEFAULT_SETTINGS);
  //UsersSharedPointer users = rm::ResourceManager::instance()->load<Users>(
  //  fileName,
  //  "users");
  //if (!users)
  //{
  //  Error() << "Impossible to load user default settings !" << std::endl;
  //  return;
  //}
  //if (users->size() == 0)
  //{
  //  Error() << "No user in default config !" << std::endl;
  //  return;
  //}
  //*this = users->layout(users->at(0).login());
}

User::User()
  : type_(NoRole)
{
}

QString User::login() const
{
  return login_;
}

void User::setLogin(const QString &name)
{
  login_ = name;
}
QString User::password() const
{
  return password_;
}

QString User::hash(QString const &input)
{
  return QCryptographicHash::hash(QByteArray(
                                    input.toStdString().c_str(),
                                    input.size()),
                                  QCryptographicHash::Md5).toHex();
}

void User::setPassword(const QString &password)
{
  password_ = password;
}

User::Type User::type() const
{
  return type_;
}

void User::setType(const Type &type)
{
  type_ = type;
}

User::Layout User::layout() const
{
  return layout_;
}

void User::setLayout(const Layout &layout)
{
  layout_ = layout;
}

Users::Users()
{
}

bool Users::has(QString const &login, QString &password)
{
  for (auto const &user : *this)
  {
    if (user.login() == login)
    {
      if (user.password() == User::hash(password)
          || user.password() == password)
      {
        return true;
      }
    }
  }
  return false;
}

void Users::saveLayout(QString const &user_name, User::Layout const &layout)
{
  for (auto &user : *this)
  {
    if (user.login() == user_name)
    {
      user.setLayout(layout);
      return;
    }
  }
}

User::Layout Users::layout(QString const &user_name) const
{
  for (auto const &user : *this)
  {
    if (user.login() == user_name)
    {
      return user.layout();
    }
  }
  return User::Layout();
}

User::Type Users::type(QString const &login)
{
  for (auto const &user : *this)
  {
    if (login == user.login())
    {
      return user.type();
    }
  }
  return User::NoRole;
}
