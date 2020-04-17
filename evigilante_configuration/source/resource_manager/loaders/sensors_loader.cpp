#include "sensors_loader.hpp"
#include "eos/configuration/models.pb.h"

namespace eos
{
  namespace eosconfig
  {
    namespace tag
    {
      const char SENSORS[] = "sensors";
      const char SENSOR[] = "sensor";
      const char NAME[] = "name";
      const char TYPE[] = "type";
      const char POSITION[] = "position";
      const char SECOND_POSITION[] = "second_position";
      const char ORIENTATION[] = "orientation";
      const char ROBOT_POINT[] = "robot_point";
      const char X[] = "x";
      const char Y[] = "y";
      const char DISTANCE[] = "distance";
      const char ANGLE[] = "angle";
    }

    namespace
    {
      struct TempXY
      {
        TempXY(int x, int y)
            : x(x)
            , y(y)
        {
        }
        int x;
        int y;
      };
    }

    TiXmlNode *operator>>(TiXmlNode *xml, Sensor &sensor)
    {
      sensor.set_name(xmlhelpers::read(xml, tag::NAME));
      sensor.set_type(xmlhelpers::read_type<Sensor::Type>(xml, tag::TYPE));
      TempXY temp = xmlhelpers::read_tuple<TempXY>(xml, tag::POSITION);
      sensor.mutable_position()->set_x(temp.x);
      sensor.mutable_position()->set_y(temp.y);
      temp = xmlhelpers::read_tuple<TempXY>(xml, tag::SECOND_POSITION);
      sensor.mutable_position_second_element()->set_x(temp.x);
      sensor.mutable_position_second_element()->set_y(temp.y);
      temp = xmlhelpers::read_tuple<TempXY>(xml, tag::ROBOT_POINT);
      sensor.mutable_robot_point()->set_x(temp.x);
      sensor.mutable_robot_point()->set_y(temp.y);
      temp = xmlhelpers::read_tuple<TempXY>(xml, tag::DISTANCE);
      sensor.mutable_distance()->set_min(temp.x);
      sensor.mutable_distance()->set_max(temp.y);
      temp = xmlhelpers::read_tuple<TempXY>(xml, tag::ANGLE);
      sensor.mutable_angle()->set_min(temp.x);
      sensor.mutable_angle()->set_max(temp.y);
      sensor.set_orientation(xmlhelpers::read_int(xml, tag::ORIENTATION));
      return xml;
    }

    BaseResource::Ptr SensorsLoader::load(RawData const &data) const
    {
      TiXmlDocument doc;
      doc.Parse(data.c_str(), 0, TIXML_DEFAULT_ENCODING);

      auto sensors = rsc::alloc<Sensors>();

      sensors->set_timestamp(xmlhelpers::read_timestamp(doc.RootElement()));

      TiXmlNode *sensors_xml = doc.FirstChildElement(tag::SENSORS);
      if (sensors_xml)
      {
        TiXmlNode *sensor_xml = sensors_xml->FirstChildElement(tag::SENSOR);
        while (sensor_xml)
        {
          Sensor *sensor = sensors->mutable_sensors()->Add();
          sensor_xml >> *sensor;
          sensor_xml = sensor_xml->NextSibling();
        }
      }
      return sensors;
    }

    TiXmlNode *operator<<(TiXmlNode *xml, Sensor const &sensor)
    {
      xmlhelpers::dump(xml, tag::NAME, sensor.name());
      xmlhelpers::dump(xml, tag::TYPE, sensor.type());
      xmlhelpers::dump_x_y(xml, tag::POSITION, sensor.position().x(), sensor.position().y());
      xmlhelpers::dump_x_y(xml, tag::SECOND_POSITION, sensor.position_second_element().x(), sensor.position_second_element().y());
      xmlhelpers::dump_x_y(xml, tag::ROBOT_POINT, sensor.robot_point().x(), sensor.robot_point().y());
      xmlhelpers::dump(xml, tag::ORIENTATION, sensor.orientation());
      xmlhelpers::dump_x_y(xml, tag::DISTANCE, sensor.distance().min(), sensor.distance().max());
      xmlhelpers::dump_x_y(xml, tag::ANGLE, sensor.angle().min(), sensor.angle().max());
      return xml;
    }

    bool SensorsLoader::save(RawData &data, BaseResource::Ptr const &rsc) const
    {
      TiXmlDocument doc;
      TiXmlElement *root = xmlhelpers::createDocument(tag::SENSORS, doc);

      auto sensors = rsc::as<Sensors>(rsc);

      xmlhelpers::dump_timestamp(root, sensors->timestamp());

      for (auto const &sensor : sensors->sensors())
      {
        TiXmlElement *xml_sensor = new TiXmlElement(tag::SENSOR);
        xml_sensor << sensor;
        root->LinkEndChild(xml_sensor);
      }

      return xmlhelpers::dump(doc, data);
    }
  }
}
