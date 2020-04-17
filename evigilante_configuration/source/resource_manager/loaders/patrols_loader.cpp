#include "patrols_loader.hpp"
#include "eos/configuration/models.pb.h"
#include "tinyxml.h"

#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

#include <QDebug>
namespace eos
{
  namespace eosconfig
  {
    namespace tag
    {
      const char PATROLS[] = "patrols";
      const char PATROL[] = "patrol";
      const char NAME[] = "name";
      const char TYPE[] = "type";
      const char POINTS[] = "points";
      const char POSITION[] = "position";
      const char POINT[] = "point";
      const char X[] = "x";
      const char Y[] = "y";
      const char IMPORTANCE[] = "importance";
      const char PRIORITY[] = "priority";
    }

    void read_points(TiXmlNode *xml, Patrol &patrol)
    {
      TiXmlNode *point = xml->FirstChildElement(tag::POINT);
      while (point)
      {
        Patrol::Point *p = patrol.mutable_points()->Add();
        p->mutable_position()->set_x(xmlhelpers::read_int(point, tag::X));
        p->mutable_position()->set_y(xmlhelpers::read_int(point, tag::Y));
        p->set_priority(xmlhelpers::read_type<Patrol::Priority>(point, tag::IMPORTANCE));
        point = point->NextSibling();
      }
    }

    void write_points(TiXmlNode *xml, Patrol const &patrol)
    {
      for (auto it = patrol.points().begin(); it != patrol.points().end(); ++it)
      {
        Patrol::Point point = *it;

        TiXmlElement *point_xml = new TiXmlElement(tag::POINT);

        xmlhelpers::dump(point_xml, tag::X, point.position().x());
        xmlhelpers::dump(point_xml, tag::Y, point.position().y());
        xmlhelpers::dump(point_xml, tag::IMPORTANCE, point.priority()); 
        xml->LinkEndChild(point_xml);
      }
    }

    TiXmlNode *operator>>(TiXmlNode *xml, Patrol &patrol)
    {
      patrol.set_name(xmlhelpers::read(xml, tag::NAME));
      patrol.set_type(xmlhelpers::read_type<Patrol::Type>(xml, tag::TYPE));
      read_points(xml->FirstChildElement(tag::POINTS), patrol);
      return xml;
    }

    TiXmlNode *operator<<(TiXmlNode *xml, Patrol const &patrol)
    {
      xmlhelpers::dump(xml, tag::NAME, patrol.name());
      xmlhelpers::dump(xml, tag::TYPE, patrol.type());

      TiXmlElement *points = new TiXmlElement(tag::POINTS);

      write_points(points, patrol);

      xml->LinkEndChild(points);
      return xml;
    }

    BaseResource::Ptr PatrolsLoader::load(const RawData &data) const
    {
        bool load_json = true;
        if(load_json)
        {
            return loadJson(data);
        }
        else
        {
            return loadXml(data);
        }
    }

    BaseResource::Ptr PatrolsLoader::loadJson(const RawData &data) const
    {
        auto patrols = rsc::alloc<Patrols>();
        // patrols->set_timestamp(xmlhelpers::read_timestamp(doc.RootElement()));
        QJsonDocument jdoc = QJsonDocument::fromJson(QByteArray(data.c_str()));
        if(!jdoc.isNull())
        {
            QJsonObject json = jdoc.object();
            QJsonArray jPatrols = json[tag::PATROLS].toArray();
            for(auto jPatrol : jPatrols)
            {

                Patrol *patrol = patrols->add_patrols();
                patrol->set_name(jPatrol.toObject()[tag::NAME].toString().toStdString());
                patrol->set_type(static_cast<Patrol::Type>(0 * jPatrol.toObject()[tag::TYPE].toString().toInt()));
              //   qInfo() << " PatrolsLoader::loadJson : " << jPatrol.toObject()[tag::TYPE] << " value"<<jPatrol.toObject()[tag::TYPE] .toString().toInt();

                auto jPtsArray = jPatrol.toObject()[tag::POINTS].toArray();
                for(auto jPts : jPtsArray)
                {
                    Patrol::Point *p = patrol->mutable_points()->Add();
                    auto jData = jPts.toObject();
                    p->mutable_position()->set_x( int(jData[tag::POSITION].toObject()[tag::X].toString().toDouble()));
                    p->mutable_position()->set_y( int(jData[tag::POSITION].toObject()[tag::Y].toString().toDouble()));
                    p->set_priority(static_cast<Patrol::Priority>(0 *  jData[tag::PRIORITY].toString().toInt()));

                   // qInfo() << " PatrolsLoader::loadJson : X " << jData[tag::POSITION].toObject()[tag::X] <<  " value" << jData[tag::POSITION].toObject()[tag::X].toString().toDouble();
                   // qInfo() << " PatrolsLoader::loadJson : Y " << jData[tag::POSITION].toObject()[tag::Y] << " value" << jData[tag::POSITION].toObject()[tag::Y].toString().toDouble() ;

                }
            }
        }

        return patrols;
    }

    BaseResource::Ptr PatrolsLoader::loadXml(const RawData &data) const
    {
      // load json format file

      TiXmlDocument doc;
      doc.Parse(data.c_str(), 0, TIXML_DEFAULT_ENCODING);

      auto patrols = rsc::alloc<Patrols>();

      patrols->set_timestamp(xmlhelpers::read_timestamp(doc.RootElement()));

      TiXmlNode *xml_patrols = doc.FirstChildElement(tag::PATROLS);
      if (xml_patrols)
      {
        TiXmlNode *xml_patrol = xml_patrols->FirstChildElement(tag::PATROL);
        while (xml_patrol)
        {
          Patrol *patrol = patrols->add_patrols();
          xml_patrol >> *patrol;
          xml_patrol = xml_patrol->NextSibling();
        }
      }
      return patrols;
    }

    bool PatrolsLoader::save(RawData &data, BaseResource::Ptr const &rsc) const
    {
        static bool isJson = true;
        if(isJson)
        {
            return saveJson(data,rsc);
        }
        else
        {
            return saveXml(data,rsc);
        }
    }

    bool PatrolsLoader::saveJson(RawData &data, BaseResource::Ptr const &rsc) const
    {
        /*
         * "
        {
            "patrols":
            [
                {
                    "name": "Patrol 1",
                    "type": "0",
                    "points":
                    [
                        {
                            "priority": "0",
                            "position":
                            {
                                "x": "267",
                                "y": "264",
                                "z": "0"
                            }
                        }
                    ]
                }
            ]
        }
            "
        */

        QJsonObject jPatrols;
        QJsonArray jPatrolArray;
        auto patrols = rsc::as<Patrols>(rsc);
        for (auto &patrol : patrols->patrols())
        {
            QJsonObject jPatrol;
            QJsonArray jPointArray;
              for (auto point : patrol.points())
              {
                QJsonObject jPoint;
                QJsonObject jPosition;
                // qDebug() << "------------ Position " << QString::number(point.position().x());

                jPosition.insert(tag::X, QJsonValue::fromVariant(QString::number(point.position().x())));
                jPosition.insert(tag::Y, QJsonValue::fromVariant(QString::number(point.position().y())));
                jPoint.insert(tag::POSITION,jPosition);
                jPoint.insert(tag::PRIORITY,  QJsonValue::fromVariant(QString::number(point.priority())));
                jPointArray.push_back(jPoint);
              }
              jPatrol.insert(tag::NAME, QJsonValue::fromVariant(patrol.name().c_str()));
              jPatrol.insert(tag::TYPE,QJsonValue::fromVariant((QString::number(patrol.type()))));
              jPatrol.insert(tag::POINTS,jPointArray);
              jPatrolArray.push_back(jPatrol);
        }
        jPatrols.insert(tag::PATROLS,jPatrolArray);

        QJsonDocument doc(jPatrols);
        QString json_str = doc.toJson();
        // data = json_str;
        //  qDebug() << " " << json_str;
        data = json_str.toStdString();
        return not data.empty();
    }


    bool PatrolsLoader::saveXml(RawData &data, BaseResource::Ptr const &rsc) const
    {
      TiXmlDocument doc;
      TiXmlElement *root = xmlhelpers::createDocument(tag::PATROLS, doc);

      auto patrols = rsc::as<Patrols>(rsc);

      xmlhelpers::dump_timestamp(root, patrols->timestamp());

      for (auto const &patrol : patrols->patrols())
      {
        TiXmlElement *xml_patrol = new TiXmlElement(tag::PATROL);
        xml_patrol << patrol;
        root->LinkEndChild(xml_patrol);
      }

      return xmlhelpers::dump(doc, data);
    }
  }
}
