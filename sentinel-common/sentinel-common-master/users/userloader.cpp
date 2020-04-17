#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QFile>
#include <QDateTime>
#include <stdexcept>

#include "userloader.h"

namespace
{
  namespace
  {
    void XmlNext(QXmlStreamReader *reader)
    {
      if (!reader)
        throw std::runtime_error("reader error");
      if (reader->readNext() == QXmlStreamReader::NoToken)
        throw std::runtime_error("XMl Read next error");
      if (reader->atEnd() || reader->hasError())
        throw std::runtime_error("XMl error happened");
    }

    QString XmlNextQString(QXmlStreamReader *reader)
    {
      XmlNext(reader);
      return reader->text().toString();
    }

    double XmlNextDouble(QXmlStreamReader *reader)
    {
      XmlNext(reader);
      bool ok = false;
      double result = reader->text().toString().toDouble(&ok);
      if (!ok)
        throw std::runtime_error("XMl not a double");
      return result;
    }

    int XmlNextInt(QXmlStreamReader *reader)
    {
      XmlNext(reader);
      bool ok = false;
      int result = reader->text().toString().toInt(&ok);
      if (!ok)
        throw std::runtime_error("XMl not an integer");
      return result;
    }
  }

  namespace xml
  {
    const char USERS[] = "users";
    const char USER[] = "user";
    const char LOGIN[] = "login";
    const char PASSWORD[] = "password";
    const char TYPE[] = "type";
    const char LAYOUT[] = "layout";
    const char LAYOUT_TYPE[] = "layout_type";
    const char LAYOUT_SPLIT_TYPE[] = "split_type";
    const char LAYOUT_SIZE[] = "layout_size";
  }

  QXmlStreamWriter &operator<<(QXmlStreamWriter &xml, User::Layout const &layout)
  {
    xml.writeStartElement(xml::LAYOUT);

    xml.writeTextElement(xml::LAYOUT_TYPE, layout.type);
    xml.writeTextElement(xml::LAYOUT_SPLIT_TYPE, layout.splitType);
    xml.writeTextElement(xml::LAYOUT_SIZE, QString::number(layout.size));

    if (layout.children.size() == 2)
    {
      xml << layout.children[0];
      xml << layout.children[1];
    }

    xml.writeEndElement();

    return xml;
  }

  QXmlStreamWriter &operator<<(QXmlStreamWriter &xml, User const &user)
  {
    xml.writeStartElement(xml::USER);

    xml.writeTextElement(xml::LOGIN, user.login());
    xml.writeTextElement(xml::PASSWORD, user.password());
    xml.writeTextElement(xml::TYPE, QString::number(user.type()));

    xml << user.layout();

    xml.writeEndElement();

    return xml;
  }

  QXmlStreamWriter &operator<<(QXmlStreamWriter &xml, Users const &users)
  {
    xml.writeStartElement(xml::USERS);
    for (Users::const_iterator it = users.begin();
         it != users.end(); ++it)
    {
      xml << *it;
    }
    xml.writeEndElement();
    return xml;
  }

  QXmlStreamReader &operator>>(QXmlStreamReader &xml, User::Layout &layout)
  {
    int i = 0;
    QXmlStreamReader::TokenType type = QXmlStreamReader::NoToken;
    do
    {
      type = xml.readNext();
      if (type == QXmlStreamReader::EndElement)
      {
        if (xml.name() == xml::LAYOUT)
          return xml;
      }
      else if (type == QXmlStreamReader::StartElement)
      {
        if (xml.name() == xml::LAYOUT_SPLIT_TYPE)
        {
          layout.splitType = XmlNextQString(&xml);
          if (layout.splitType == "V" || layout.splitType == "H")
          {
            layout.children.push_back(User::Layout());
            layout.children.push_back(User::Layout());
          }
        }
        else if (xml.name() == xml::LAYOUT_SIZE)
        {
          layout.size = XmlNextDouble(&xml);
        }
        else if (xml.name() == xml::LAYOUT_TYPE)
        {
          layout.type = XmlNextQString(&xml);
        }
        else if (xml.name() == xml::LAYOUT)
        {
          if (i < 2 && layout.children.size() == 2)
            xml >> layout.children[i++];
        }
      }
    } while (!xml.atEnd() && !xml.hasError());
    return xml;
  }

  QXmlStreamReader &operator>>(QXmlStreamReader &xml, User &user)
  {
    QXmlStreamReader::TokenType type = QXmlStreamReader::NoToken;
    do
    {
      type = xml.readNext();
      if (type == QXmlStreamReader::StartElement)
      {
        if (xml.name() == xml::LOGIN)
        {
          user.setLogin(XmlNextQString(&xml));
        }
        if (xml.name() == xml::PASSWORD)
        {
          user.setPassword(XmlNextQString(&xml));
        }
        else if (xml.name() == xml::TYPE)
        {
          user.setType(static_cast<User::Type>(XmlNextInt(&xml)));
        }
        else if (xml.name() == xml::LAYOUT)
        {
          User::Layout layout;
          xml >> layout;
          //layout.print();
          user.setLayout(layout);
        }
      }
      else if (xml.name() == xml::USER)
        break;
    } while (!xml.atEnd() && !xml.hasError());
    return xml;
  }

  QXmlStreamReader &operator>>(QXmlStreamReader &xml, Users &users)
  {
    QXmlStreamReader::TokenType type;
    do
    {
      type = xml.readNext();
      if (type == QXmlStreamReader::StartElement)
      {
        if (xml.name() == xml::USER)
        {
          User user;
          xml >> user;
          users.add(user);
          //user.layout().print();
        }
      }
    } while (!xml.atEnd() && !xml.hasError());
    return xml;
  }
}

Users UserLoader::load(QString const &filePath)
{
  Users users;
  QFile file(filePath);
  if (file.open(QIODevice::ReadOnly | QIODevice::Text) == false)
    return users;
  QXmlStreamReader xml(&file);
  xml >> users;
  return users;
}

bool UserLoader::save(QString const &filePath, Users const &users)
{
  QFile file(filePath);
  if (file.open(QIODevice::WriteOnly | QIODevice::Text) == false)
    return false;
  QXmlStreamWriter stream(&file);
  stream.writeStartDocument();
  stream << users;
  stream.writeEndDocument();
  if (stream.hasError())
    return false;
  return true;
}
