#include "zones_loader.hpp"
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
      const char ZONES[] = "zones";
      const char ZONE[] = "zone";
      const char TOP_LEFT[] = "topLeft";
      const char TOP_RIGHT[] = "topRight";
      const char BOTTOM_RIGHT[] = "bottomRight";
      const char PERMISSIONS[] = "permissions";
      const char TYPE[] = "type";
      const char POINTS[] = "points";
      const char X[] = "x";
      const char Y[] = "y";
    }

    TiXmlNode *operator>>(TiXmlNode *xml, Zone &zone)
    {
      TiXmlElement *perm_xml = xml->FirstChildElement(tag::PERMISSIONS);

      zone.set_permissions(static_cast<eos::Zone_Permission>(atoi(perm_xml->GetText())));

      TiXmlElement *top_left = xml->FirstChildElement(tag::TOP_LEFT);
      if (top_left)
      {
        TiXmlElement *top_left_x = top_left->FirstChildElement(tag::X);
        if (top_left_x)
          zone.mutable_top_left()->set_x(atoi(top_left_x->GetText()));
        TiXmlElement *top_left_y = top_left->FirstChildElement(tag::Y);
        if (top_left_y)
          zone.mutable_top_left()->set_y(atoi(top_left_y->GetText()));
      }

      TiXmlElement *bottom_right = xml->FirstChildElement(tag::BOTTOM_RIGHT);
      // manage old version of xml zone file
      // with a small bug of tag name :(
      if (!bottom_right)
        bottom_right = xml->FirstChildElement(tag::TOP_RIGHT);
      if (bottom_right)
      {
        TiXmlElement *bottom_right_x = bottom_right->FirstChildElement(tag::X);
        if (bottom_right_x)
          zone.mutable_bottom_right()->set_x(atoi(bottom_right_x->GetText()));
        TiXmlElement *bottom_right_y = bottom_right->FirstChildElement(tag::Y);
        if (bottom_right_y)
          zone.mutable_bottom_right()->set_y(atoi(bottom_right_y->GetText()));
      }

      return xml;
    }

    namespace
    {
      template <typename T>
      std::string toStr(T value)
      {
        std::stringstream ss;
        ss << value;
        return ss.str();
      }
    }

    TiXmlNode *operator<<(TiXmlNode *xml, Zone const &zone)
    {
      TiXmlElement *perm = new TiXmlElement(tag::PERMISSIONS);
      perm->LinkEndChild(new TiXmlText(toStr<int>(zone.permissions()).c_str()));
      xml->LinkEndChild(perm);

      TiXmlElement *top_left = new TiXmlElement(tag::TOP_LEFT);
      TiXmlElement *top_left_x = new TiXmlElement(tag::X);
      top_left_x->LinkEndChild(new TiXmlText(toStr<int>(zone.top_left().x()).c_str()));
      top_left->LinkEndChild(top_left_x);
      TiXmlElement *top_left_y = new TiXmlElement(tag::Y);
      top_left_y->LinkEndChild(new TiXmlText(toStr<int>(zone.top_left().y()).c_str()));
      top_left->LinkEndChild(top_left_y);

      xml->LinkEndChild(top_left);

      TiXmlElement *bottom_right = new TiXmlElement(tag::BOTTOM_RIGHT);
      TiXmlElement *bottom_right_x = new TiXmlElement(tag::X);
      bottom_right_x->LinkEndChild(new TiXmlText(toStr<int>(zone.bottom_right().x()).c_str()));
      bottom_right->LinkEndChild(bottom_right_x);
      TiXmlElement *bottom_right_y = new TiXmlElement(tag::Y);
      bottom_right_y->LinkEndChild(new TiXmlText(toStr<int>(zone.bottom_right().y()).c_str()));
      bottom_right->LinkEndChild(bottom_right_y);

      xml->LinkEndChild(bottom_right);

      return xml;
    }

    //Load Areas

    BaseResource::Ptr ZonesLoader::load(const RawData &data) const
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



    BaseResource::Ptr ZonesLoader::loadJson(const RawData &data) const
    {
        auto zones = rsc::alloc<Zones>();
        QJsonDocument jdoc = QJsonDocument::fromJson(QByteArray(data.c_str()));
        if(!jdoc.isNull())
        {
            QJsonObject json = jdoc.object();
            QJsonArray jZones = json[tag::ZONES].toArray();
            for(auto jZone : jZones)
            {

                Zone *zone = zones->add_zones();
                //type area
                zone->set_permissions(static_cast<Zone::Permission>(0 * jZone.toObject()[tag::TYPE].toString().toInt()));
                qInfo() << " ZonesLoader::loadJson : " << jZone.toObject()[tag::TYPE] ;

                auto jPtsArray = jZone.toObject()[tag::POINTS].toArray();
                for(auto jPts : jPtsArray)
                {
                    auto jData = jPts.toObject();
                    qInfo() << "jData " << jData;
                    qInfo() << "X ::: " << jData[tag::X].toString().toDouble();
                    qInfo() << "Y ::: " << jData[tag::Y].toString().toDouble();

                    zone->mutable_top_left()->set_y(jData[tag::Y].toString().toDouble());
                    zone->mutable_bottom_right()->set_x(jData[tag::X].toString().toDouble());
                     zone->mutable_top_left()->set_x(jData[tag::X].toString().toDouble());

                    //zone->mutable_bottom_right()->set_y(jData[tag::Y].toString().toDouble());

                    //Zone::Point *p = zone->mutable_top_left();
                    // Zone::Point *pp = zone->mutable_bottom_right();

                }
            }
        }

        return zones;
    }




    BaseResource::Ptr ZonesLoader::loadXml(RawData const &data) const
    {
      TiXmlDocument doc;
      doc.Parse(data.c_str(), 0, TIXML_DEFAULT_ENCODING);
      auto zones = rsc::alloc<Zones>();
      TiXmlNode *zones_xml = doc.FirstChildElement(tag::ZONES);

      zones->set_timestamp(xmlhelpers::read_timestamp(doc.RootElement()));

      if (zones_xml)
      {
        TiXmlNode *zone_xml = zones_xml->FirstChildElement(tag::ZONE);
        while (zone_xml)
        {
          Zone *zone = zones->mutable_zones()->Add();
          zone_xml >> *zone;
          zone_xml = zone_xml->NextSibling();
        }
      }
      return zones;
    }

    //save Areas
    bool ZonesLoader::save(RawData &data, BaseResource::Ptr const &rsc) const
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

    bool ZonesLoader::saveXml(RawData &data, BaseResource::Ptr const &rsc) const
    {
      TiXmlDocument doc;
      TiXmlElement *root = xmlhelpers::createDocument(tag::ZONES, doc);

      auto zones = rsc::as<Zones>(rsc);

      xmlhelpers::dump_timestamp(root, zones->timestamp());

      for (auto const &zone : zones->zones())
      {
        TiXmlElement *xml_zone = new TiXmlElement(tag::ZONE);
        xml_zone << zone;
        root->LinkEndChild(xml_zone);
      }

      return xmlhelpers::dump(doc, data);
    }

    bool ZonesLoader::saveJson(RawData &data, BaseResource::Ptr const &rsc) const
    {
        /*
         * "
        {
            "zones":
            [
                {
                "type":"No Mouvement",
                    "points":
                    [
                            {
                                "x": "757",
                                "y": "832",
                            },
                            {
                                "x": "757",
                                "y": "729",
                            },
                            {
                                "x": "568",
                                "y": "729",
                            },
                            {
                                "x": "568",
                                "y": "832",
                            }
                        }
                    ]
                }
            ]
        }
            "
        */

        QJsonObject jZones;
        QJsonArray jZoneArray;
        auto zones = rsc::as<Zones>(rsc);
        for (auto &zone : zones->zones())
        {

            QJsonObject jZone;
            QJsonArray jPointArray;
            //for (int i=0; i<=3 ;i++)
           // {
            QJsonObject jPoint;
            //qInfo() <<"Type   " << zone.permissions();
            jPoint.insert(tag::X, QJsonValue::fromVariant(QString::number(zone.bottom_right().x())));
            jPoint.insert(tag::Y, QJsonValue::fromVariant(QString::number(zone.bottom_right().y())));
            jPoint.insert(tag::X, QJsonValue::fromVariant(QString::number(zone.top_left().x())));
            jPoint.insert(tag::Y, QJsonValue::fromVariant(QString::number(zone.top_left().y())));

            jPointArray.push_back(jPoint);
           // }
            jZone.insert(tag::TYPE,QJsonValue::fromVariant((QString::number(zone.permissions()))));
            jZone.insert(tag::POINTS,jPointArray);
            jZoneArray.push_back(jZone);
        }
        jZones.insert(tag::ZONES,jZoneArray);

        QJsonDocument doc(jZones);
        QString json_str = doc.toJson();
        // data = json_str;
        //  qDebug() << " " << json_str;
        data = json_str.toStdString();
        return not data.empty();

}}}
