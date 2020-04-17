#include "tasksscheduled_loader.hpp"
#include "eos/configuration/models.pb.h"

namespace eos
{
  namespace eosconfig
  {
    namespace tag
    {
      constexpr char TASKS_SCHEDULED[] = "tasks_scheduled";
      constexpr char TASK_SCHEDULED[] = "task_scheduled";
      constexpr char PATROL_NAME[] = "patrol_name";
      constexpr char START[] = "start";
      constexpr char STOP[] = "stop";
      constexpr char GROUP_ID[] = "group_id";
    }

    namespace
    {
      TiXmlNode *operator>>(TiXmlNode *xml, TaskScheduled &task_scheduled)
      {
        task_scheduled.set_patrol_name(xmlhelpers::read(xml, tag::PATROL_NAME));
        task_scheduled.set_start(xmlhelpers::read_int64(xml, tag::START));
        task_scheduled.set_stop(xmlhelpers::read_int64(xml, tag::STOP));
        task_scheduled.set_group_id(xmlhelpers::read_int64(xml, tag::GROUP_ID));
        return xml;
      }

      TiXmlNode *operator<<(TiXmlNode *xml, TaskScheduled const &task_scheduled)
      {
        xmlhelpers::dump(xml, tag::PATROL_NAME, task_scheduled.patrol_name());
        xmlhelpers::dump<int64_t>(xml, tag::START, task_scheduled.start());
        xmlhelpers::dump<int64_t>(xml, tag::STOP, task_scheduled.stop());
        xmlhelpers::dump<int64_t>(xml, tag::GROUP_ID, task_scheduled.group_id());
        return xml;
      }

    }

    BaseResource::Ptr TasksScheduledLoader::load(RawData const &data) const
    {
      TiXmlDocument doc;
      doc.Parse(data.c_str(), 0, TIXML_DEFAULT_ENCODING);
      auto tasks_scheduled = rsc::alloc<TasksScheduled>();

      TiXmlNode *tasks_scheduled_xml = doc.FirstChildElement(tag::TASKS_SCHEDULED);

      tasks_scheduled->set_timestamp(xmlhelpers::read_timestamp(doc.RootElement()));

      if (tasks_scheduled_xml)
      {
        TiXmlNode *task_scheduled_xml = tasks_scheduled_xml->FirstChildElement(tag::TASK_SCHEDULED);
        while (task_scheduled_xml)
        {
          TaskScheduled *task_scheduled = tasks_scheduled->mutable_tasks()->Add();
          task_scheduled_xml >> *task_scheduled;
          task_scheduled_xml = task_scheduled_xml->NextSibling();
        }
      }
      return tasks_scheduled;
    }

    bool TasksScheduledLoader::save(RawData &data, BaseResource::Ptr const &rsc) const
    {
      TiXmlDocument doc;
      TiXmlElement *root = xmlhelpers::createDocument(tag::TASKS_SCHEDULED, doc);

      auto tasks_scheduled = rsc::as<TasksScheduled>(rsc);

      xmlhelpers::dump_timestamp(root, tasks_scheduled->timestamp());

      for (auto const &task_scheduled : tasks_scheduled->tasks())
      {
        TiXmlElement *xml_task_scheduled = new TiXmlElement(tag::TASK_SCHEDULED);
        xml_task_scheduled << task_scheduled;
        root->LinkEndChild(xml_task_scheduled);
      }

      return xmlhelpers::dump(doc, data);
    }
  }
}
