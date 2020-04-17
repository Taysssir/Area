#ifndef SENTINEL_USER_H
#define SENTINEL_USER_H

#include <QVector>
#include <QString>

struct User
{
  static const int kMinUserNameSize = 4;
  static const int kMinPasswordSize = 4;

  typedef enum
  {
    NoRole,
    Agent,
    //Demonstrator,
    Administrator
    //Root
  } Type;

  struct Layout
  {
    qreal size;
    QString type;
    QString splitType;
    std::vector<Layout> children;
    Layout()
      : size(1)
      , type("")
    {
    }

    void defineDefault();
  };

public:
  User();
  QString login() const;
  void setLogin(const QString &login);

  static QString hash(QString const &input);

  QString password() const;
  void setPassword(const QString &password);

  Type type() const;
  void setType(const Type &type);

  Layout layout() const;
  void setLayout(const Layout &layout);

private:
  QString login_;
  QString password_;
  Type type_;
  Layout layout_;
};

class Users
{
public:
  typedef typename QVector<User>::iterator iterator;
  typedef typename QVector<User>::const_iterator const_iterator;

  Users();

  bool has(QString const &login, QString &password);

  bool has(QString const &login) const
  {
    for (auto const &user : *this)
      if (login == user.login()) return true;
    return false;
  }

  User::Type type(QString const &login);

  void add(User const &user)
  {
    data_.push_back(user);
  }
  void remove(int index)
  {
    if (index < data_.size())
      data_.remove(index);
  }
  User &at(int index)
  {
    if (index < data_.size())
      return data_[index];
    return None;
  }
  size_t size() const
  {
    return data_.size();
  }

  void saveLayout(QString const &user_name, User::Layout const &layout);
  User::Layout layout(QString const &user_name) const;

  inline iterator begin()
  {
    return data_.begin();
  }
  inline const_iterator begin() const
  {
    return data_.constBegin();
  }
  inline iterator end()
  {
    return data_.end();
  }
  inline const_iterator end() const
  {
    return data_.constEnd();
  }

private:
  QVector<User> data_;
  static User None;
};

#endif // SENTINEL_USER_H
